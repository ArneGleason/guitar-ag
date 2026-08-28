#!/usr/bin/env python3
"""Measure Plan 0103 low-E captures and matching EG-089 model renders.

This research script depends on NumPy and SciPy. It writes reproducible header,
onset, event, and aggregate evidence; it is not a single guitar-realism score.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import wave
from dataclasses import dataclass
from pathlib import Path

import numpy as np
from scipy import signal


REFERENCE_SPECS = {
    "low-e-eval-ringing-down": ("reference", "ringing", "down", 6, 0.75),
    "low-e-eval-ringing-up": ("reference", "ringing", "up", 6, 0.75),
    "low-e-eval-ringing-alternate": ("reference", "ringing", "alternate", 12, 0.25),
    "low-e-eval-hand-damped-down": ("reference", "damped", "down", 6, 0.30),
    "low-e-eval-hand-damped-up": ("reference", "damped", "up", 6, 0.30),
    "low-e-eval-hand-damped-alternate": ("reference", "damped", "alternate", 6, 0.30),
}

MODEL_SPECS = {
    "model-ringing-down.wav": ("model", "ringing", "down", 6, 0.75),
    "model-ringing-up.wav": ("model", "ringing", "up", 6, 0.75),
    "model-ringing-alternate.wav": ("model", "ringing", "alternate", 12, 0.25),
    "model-damped-down.wav": ("model", "damped", "down", 6, 0.30),
    "model-damped-up.wav": ("model", "damped", "up", 6, 0.30),
    "model-damped-alternate.wav": ("model", "damped", "alternate", 6, 0.30),
    "model-short-down-no-mute.wav": ("model_no_mute", "damped", "down", 6, 0.30),
    "ablate-direct-mix.wav": ("ablate_direct_mix", "ringing", "down", 6, 0.75),
    "ablate-attack-modes.wav": ("ablate_attack_modes", "ringing", "down", 6, 0.75),
    "ablate-pick-transient.wav": ("ablate_pick_transient", "ringing", "down", 6, 0.75),
    "ablate-contact-layer.wav": ("ablate_contact_layer", "ringing", "down", 6, 0.75),
    "control-pick-bite-zero.wav": ("control_pick_bite_zero", "ringing", "down", 6, 0.75),
    "control-pick-texture-zero.wav": ("control_pick_texture_zero", "ringing", "down", 6, 0.75),
    "control-soft-pick.wav": ("control_soft_pick", "ringing", "down", 6, 0.75),
    "control-old-string.wav": ("control_old_string", "ringing", "down", 6, 0.75),
    "control-palm-085.wav": ("control_palm_085", "damped", "down", 6, 0.30),
    "control-palm-090.wav": ("control_palm_090", "damped", "down", 6, 0.30),
    "control-palm-095.wav": ("control_palm_095", "damped", "down", 6, 0.30),
    "control-palm-0975.wav": ("control_palm_0975", "damped", "down", 6, 0.30),
    "control-player-feel-one.wav": ("control_player_feel_one", "ringing", "down", 6, 0.75),
    "matched-ringing-down.wav": ("matched_model", "ringing", "down", 6, 0.75),
    "matched-ringing-up.wav": ("matched_model", "ringing", "up", 6, 0.75),
    "matched-ringing-alternate.wav": ("matched_model", "ringing", "alternate", 12, 0.25),
    "matched-damped-down.wav": ("matched_model", "damped", "down", 6, 0.30),
    "matched-damped-up.wav": ("matched_model", "damped", "up", 6, 0.30),
    "matched-damped-alternate.wav": ("matched_model", "damped", "alternate", 6, 0.30),
    "matched-neck-070.wav": ("matched_neck_070", "ringing", "down", 6, 0.75),
    "matched-neck-085.wav": ("matched_neck_085", "ringing", "down", 6, 0.75),
    "matched-ablate-all-direct.wav": ("matched_ablate_all_direct", "ringing", "down", 6, 0.75),
    "matched-old-string.wav": ("matched_old_string", "ringing", "down", 6, 0.75),
}

BANDS = ((50, 150), (150, 500), (500, 1500), (1500, 4000), (4000, 10000))


@dataclass
class Audio:
    path: Path
    sample_rate: int
    channels: int
    sample_width: int
    samples: np.ndarray


def db(value: float) -> float:
    return 20.0 * math.log10(max(float(value), 1.0e-12))


def read_audio(path: Path) -> Audio:
    with wave.open(str(path), "rb") as handle:
        channels = handle.getnchannels()
        sample_width = handle.getsampwidth()
        sample_rate = handle.getframerate()
        frames = handle.getnframes()
        raw = handle.readframes(frames)

    if sample_width == 1:
        values = (np.frombuffer(raw, dtype=np.uint8).astype(np.float64) - 128.0) / 128.0
    elif sample_width == 2:
        values = np.frombuffer(raw, dtype="<i2").astype(np.float64) / 32768.0
    elif sample_width == 3:
        packed = np.frombuffer(raw, dtype=np.uint8).reshape(-1, 3)
        integers = (
            packed[:, 0].astype(np.int32)
            | (packed[:, 1].astype(np.int32) << 8)
            | (packed[:, 2].astype(np.int32) << 16)
        )
        integers = np.where(integers & 0x800000, integers - 0x1000000, integers)
        values = integers.astype(np.float64) / 8388608.0
    elif sample_width == 4:
        values = np.frombuffer(raw, dtype="<i4").astype(np.float64) / 2147483648.0
    else:
        raise ValueError(f"unsupported PCM width {sample_width}: {path}")

    values = values[: frames * channels].reshape(-1, channels).mean(axis=1)
    return Audio(path, sample_rate, channels, sample_width, values)


def longest_zero_run(samples: np.ndarray) -> int:
    zeros = samples == 0.0
    if not np.any(zeros):
        return 0
    edges = np.diff(np.concatenate(([0], zeros.astype(np.int8), [0])))
    starts = np.flatnonzero(edges == 1)
    ends = np.flatnonzero(edges == -1)
    return int(np.max(ends - starts))


def select_onsets(audio: Audio, expected: int, minimum_distance_seconds: float) -> list[float]:
    samples = audio.samples - np.median(audio.samples)
    frame = 512
    hop = 64
    frequencies, times, spectrum = signal.stft(
        samples,
        fs=audio.sample_rate,
        window="hann",
        nperseg=frame,
        noverlap=frame - hop,
        boundary=None,
        padded=False,
    )
    use = (frequencies >= 350.0) & (frequencies <= 12000.0)
    magnitudes = np.log1p(np.abs(spectrum[use]) * 2000.0)
    flux = np.r_[0.0, np.maximum(np.diff(magnitudes, axis=1), 0.0).sum(axis=0)]
    flux = signal.savgol_filter(flux, 7, 2, mode="interp")

    energy_window = max(8, round(0.012 * audio.sample_rate))
    mean_square = signal.convolve(
        samples * samples,
        np.ones(energy_window, dtype=np.float64) / energy_window,
        mode="same",
        method="fft",
    )
    envelope_db = 10.0 * np.log10(np.maximum(mean_square, 1.0e-16))

    minimum_frames = max(1, round(minimum_distance_seconds * audio.sample_rate / hop))
    candidates, properties = signal.find_peaks(
        flux,
        distance=max(2, minimum_frames // 3),
        prominence=max(float(np.median(flux)) * 0.5, 1.0e-9),
    )
    if len(candidates) < expected:
        candidates, properties = signal.find_peaks(flux, distance=max(2, minimum_frames // 4))

    prominences = signal.peak_prominences(flux, candidates)[0]
    scores: list[tuple[float, int]] = []
    look = max(1, round(0.040 * audio.sample_rate))
    separated_events = expected <= 6 and minimum_distance_seconds >= 0.30
    for candidate, prominence in zip(candidates, prominences):
        center = min(len(samples) - 1, candidate * hop + frame // 2)
        if center < round(0.15 * audio.sample_rate):
            continue
        before_index = max(0, center - look)
        after_index = min(len(envelope_db) - 1, center + look)
        energy_rise_db = float(envelope_db[after_index] - envelope_db[before_index])
        if separated_events and energy_rise_db < 3.0:
            continue
        if separated_events:
            rise_bonus = np.clip(energy_rise_db / 12.0, 0.25, 4.0) ** 2
        else:
            rise_bonus = np.clip((energy_rise_db + 8.0) / 12.0, 0.35, 2.0)
        scores.append((float(prominence) * float(rise_bonus), center))

    chosen: list[int] = []
    minimum_samples = round(minimum_distance_seconds * audio.sample_rate)
    for _, center in sorted(scores, reverse=True):
        if all(abs(center - prior) >= minimum_samples for prior in chosen):
            chosen.append(center)
            if len(chosen) == expected:
                break

    if len(chosen) != expected:
        raise RuntimeError(
            f"found only {len(chosen)} of {expected} onsets in {audio.path}; "
            f"candidate count was {len(candidates)}"
        )
    return [sample / audio.sample_rate for sample in sorted(chosen)]


def window(audio: Audio, start_seconds: float, duration_seconds: float) -> np.ndarray:
    start = max(0, round(start_seconds * audio.sample_rate))
    end = min(len(audio.samples), start + max(1, round(duration_seconds * audio.sample_rate)))
    return audio.samples[start:end]


def spectrum_features(samples: np.ndarray, sample_rate: int) -> dict[str, float]:
    if len(samples) < 16:
        return {"centroid_hz": 0.0, "flatness": 0.0, **{f"band_{lo}_{hi}": 0.0 for lo, hi in BANDS}}
    samples = samples - np.mean(samples)
    size = max(4096, 1 << (len(samples) - 1).bit_length())
    spectrum = np.abs(np.fft.rfft(samples * np.hanning(len(samples)), n=size)) ** 2
    frequencies = np.fft.rfftfreq(size, 1.0 / sample_rate)
    use = (frequencies >= 50.0) & (frequencies <= 10000.0)
    powers = spectrum[use]
    used_frequencies = frequencies[use]
    total = float(np.sum(powers)) + 1.0e-30
    centroid = float(np.sum(used_frequencies * powers) / total)
    flat_use = (used_frequencies >= 500.0) & (used_frequencies <= 10000.0)
    flat_powers = powers[flat_use] + 1.0e-30
    flatness = float(np.exp(np.mean(np.log(flat_powers))) / np.mean(flat_powers))
    result = {"centroid_hz": centroid, "flatness": flatness}
    for lo, hi in BANDS:
        mask = (frequencies >= lo) & (frequencies < hi)
        result[f"band_{lo}_{hi}"] = float(np.sum(spectrum[mask]) / total)
    return result


def estimate_fundamental(
    samples: np.ndarray, sample_rate: int, target_hz: float = 82.4069
) -> float:
    if len(samples) < round(0.25 * sample_rate):
        return 0.0
    samples = samples - np.mean(samples)
    size = 1 << (len(samples) - 1).bit_length()
    powers = np.abs(np.fft.rfft(samples * np.hanning(len(samples)), n=size)) ** 2
    frequencies = np.fft.rfftfreq(size, 1.0 / sample_rate)
    use = (frequencies >= target_hz * 0.87) & (frequencies <= target_hz * 1.14)
    indices = np.flatnonzero(use)
    if len(indices) == 0:
        return 0.0
    peak = int(indices[np.argmax(powers[indices])])
    if peak <= 0 or peak >= len(powers) - 1:
        return float(frequencies[peak])
    left, center, right = np.log(powers[peak - 1 : peak + 2] + 1.0e-30)
    denominator = left - 2.0 * center + right
    offset = 0.0 if abs(denominator) < 1.0e-12 else 0.5 * (left - right) / denominator
    return float((peak + offset) * sample_rate / size)


def decay_slope(audio: Audio, onset: float, available_seconds: float) -> float:
    duration = min(4.0, max(0.0, available_seconds - 0.05))
    if duration < 0.5:
        return 0.0
    samples = window(audio, onset + 0.05, duration)
    frame = max(16, round(0.025 * audio.sample_rate))
    hop = max(1, round(0.0125 * audio.sample_rate))
    if len(samples) < frame * 4:
        return 0.0
    rms_values = []
    times = []
    for start in range(0, len(samples) - frame + 1, hop):
        part = samples[start : start + frame]
        rms_values.append(db(math.sqrt(float(np.mean(part * part)))))
        times.append((start + frame / 2) / audio.sample_rate)
    rms_array = np.asarray(rms_values)
    time_array = np.asarray(times)
    peak_db = float(np.max(rms_array[: max(1, round(0.25 / (hop / audio.sample_rate)))]))
    use = (time_array >= 0.15) & (rms_array <= peak_db - 3.0) & (rms_array >= peak_db - 35.0)
    if np.count_nonzero(use) < 8:
        return 0.0
    return float(np.polyfit(time_array[use], rms_array[use], 1)[0])


def event_rows(
    audio: Audio,
    source: str,
    condition: str,
    direction: str,
    take: str,
    onsets: list[float],
    fundamental_target_hz: float = 82.4069,
) -> list[dict[str, object]]:
    rows: list[dict[str, object]] = []
    for index, onset in enumerate(onsets):
        next_onset = onsets[index + 1] if index + 1 < len(onsets) else len(audio.samples) / audio.sample_rate
        pre = window(audio, max(0.0, onset - 0.060), 0.045)
        post = window(audio, onset + 0.015, 0.045)
        attack = window(audio, onset - 0.003, 0.063)
        early = window(audio, onset + 0.055, min(0.20, max(0.05, next_onset - onset - 0.08)))
        pitch_window = window(audio, onset + 0.18, min(0.80, max(0.0, next_onset - onset - 0.25)))
        peak = float(np.max(np.abs(attack))) if len(attack) else 0.0
        rms = math.sqrt(float(np.mean(attack * attack))) if len(attack) else 0.0
        early_rms = math.sqrt(float(np.mean(early * early))) if len(early) else 0.0
        pre_rms = math.sqrt(float(np.mean(pre * pre))) if len(pre) else 0.0
        post_rms = math.sqrt(float(np.mean(post * post))) if len(post) else 0.0
        features = spectrum_features(attack, audio.sample_rate)
        early_features = spectrum_features(early, audio.sample_rate)
        stroke_direction = direction
        if direction == "alternate":
            stroke_direction = "down" if index % 2 == 0 else "up"
        row: dict[str, object] = {
            "source": source,
            "condition": condition,
            "direction_group": direction,
            "stroke_direction": stroke_direction,
            "take": take,
            "event": index + 1,
            "onset_seconds": round(onset, 6),
            "interval_seconds": round(onset - onsets[index - 1], 6) if index else "",
            "attack_peak_dbfs": db(peak),
            "attack_rms_dbfs": db(rms),
            "early_rms_dbfs": db(early_rms),
            "early_to_attack_peak_db": db(early_rms) - db(peak),
            "pre_rms_dbfs": db(pre_rms),
            "post_rms_dbfs": db(post_rms),
            "onset_level_change_db": db(post_rms) - db(pre_rms),
            "attack_crest_db": db(peak / max(rms, 1.0e-12)),
            "attack_centroid_hz": features["centroid_hz"],
            "attack_flatness": features["flatness"],
            "early_centroid_hz": early_features["centroid_hz"],
            "fundamental_hz": (
                estimate_fundamental(pitch_window, audio.sample_rate, fundamental_target_hz)
                if condition == "ringing"
                else 0.0
            ),
            "decay_db_per_second": (
                decay_slope(audio, onset, next_onset - onset)
                if condition == "ringing" and index + 1 == len(onsets)
                else 0.0
            ),
        }
        for lo, hi in BANDS:
            row[f"attack_band_{lo}_{hi}"] = features[f"band_{lo}_{hi}"]
            row[f"early_band_{lo}_{hi}"] = early_features[f"band_{lo}_{hi}"]
        rows.append(row)
    return rows


def plot_onsets(audio: Audio, onsets: list[float], output: Path, title: str) -> None:
    import matplotlib.pyplot as plt

    frame = max(16, round(0.020 * audio.sample_rate))
    hop = max(1, round(0.010 * audio.sample_rate))
    values = []
    times = []
    for start in range(0, len(audio.samples) - frame + 1, hop):
        part = audio.samples[start : start + frame]
        values.append(db(math.sqrt(float(np.mean(part * part)))))
        times.append((start + frame / 2) / audio.sample_rate)

    figure, axis = plt.subplots(figsize=(14, 3.6), constrained_layout=True)
    axis.plot(times, values, linewidth=0.8, color="#244a68")
    for index, onset in enumerate(onsets, start=1):
        axis.axvline(onset, linewidth=0.8, color="#c44e52")
        axis.text(onset, -7.0, str(index), ha="center", va="top", fontsize=7)
    axis.set_title(title)
    axis.set_xlabel("Seconds")
    axis.set_ylabel("20 ms RMS (dBFS)")
    axis.set_ylim(-90.0, 0.0)
    axis.grid(alpha=0.2)
    output.parent.mkdir(parents=True, exist_ok=True)
    figure.savefig(output, dpi=140)
    plt.close(figure)


def summarize(rows: list[dict[str, object]]) -> list[dict[str, object]]:
    metrics = [
        "attack_peak_dbfs",
        "attack_rms_dbfs",
        "early_rms_dbfs",
        "early_to_attack_peak_db",
        "pre_rms_dbfs",
        "post_rms_dbfs",
        "onset_level_change_db",
        "attack_crest_db",
        "attack_centroid_hz",
        "attack_flatness",
        "early_centroid_hz",
        "fundamental_hz",
        "decay_db_per_second",
        *[f"attack_band_{lo}_{hi}" for lo, hi in BANDS],
        *[f"early_band_{lo}_{hi}" for lo, hi in BANDS],
    ]
    groups: dict[tuple[str, str, str, str], list[dict[str, object]]] = {}
    for row in rows:
        key = (
            str(row["source"]),
            str(row["condition"]),
            str(row["direction_group"]),
            str(row["stroke_direction"]),
        )
        groups.setdefault(key, []).append(row)

    summaries: list[dict[str, object]] = []
    for (source, condition, direction_group, direction), group in sorted(groups.items()):
        summary: dict[str, object] = {
            "source": source,
            "condition": condition,
            "direction_group": direction_group,
            "stroke_direction": direction,
            "events": len(group),
        }
        for metric in metrics:
            values = np.asarray([float(row[metric]) for row in group], dtype=np.float64)
            values = values[np.isfinite(values)]
            if metric in ("fundamental_hz", "decay_db_per_second"):
                values = values[values != 0.0]
            if len(values):
                summary[f"{metric}_median"] = float(np.median(values))
                summary[f"{metric}_p10"] = float(np.percentile(values, 10))
                summary[f"{metric}_p90"] = float(np.percentile(values, 90))
        summaries.append(summary)
    return summaries


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture-root", type=Path, required=True)
    parser.add_argument("--model-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument("--plot-directory", type=Path)
    args = parser.parse_args()
    args.output_directory.mkdir(parents=True, exist_ok=True)

    validation_rows: list[dict[str, object]] = []
    events: list[dict[str, object]] = []

    for request_id, spec in REFERENCE_SPECS.items():
        source, condition, direction, expected, minimum_distance = spec
        session_directory = args.capture_root / "sessions" / request_id
        manifest = json.loads((session_directory / "session.json").read_text(encoding="utf-8"))
        approved = [take for take in manifest["takes"] if take["status"] == "approved"]
        for take in approved:
            path = session_directory / take["file"]
            audio = read_audio(path)
            onsets = select_onsets(audio, expected, minimum_distance)
            if args.plot_directory:
                plot_onsets(
                    audio,
                    onsets,
                    args.plot_directory / f"{request_id}-{take['file']}.png",
                    f"{request_id} / {take['file']}",
                )
            validation_rows.append(
                {
                    "source": source,
                    "request": request_id,
                    "take": take["file"],
                    "sample_rate": audio.sample_rate,
                    "channels": audio.channels,
                    "bits": audio.sample_width * 8,
                    "duration_seconds": len(audio.samples) / audio.sample_rate,
                    "peak_dbfs": db(np.max(np.abs(audio.samples))),
                    "clip_samples": int(np.count_nonzero(np.abs(audio.samples) >= 0.999)),
                    "longest_zero_run_samples": longest_zero_run(audio.samples),
                    "dropped_audio": bool(take.get("dropped_audio", False)),
                    "detected_onsets": len(onsets),
                    "onset_seconds": " ".join(f"{value:.3f}" for value in onsets),
                }
            )
            events.extend(event_rows(audio, source, condition, direction, take["file"], onsets))

    for filename, spec in MODEL_SPECS.items():
        source, condition, direction, expected, minimum_distance = spec
        path = args.model_directory / filename
        audio = read_audio(path)
        onsets = select_onsets(audio, expected, minimum_distance)
        validation_rows.append(
            {
                "source": source,
                "request": filename,
                "take": filename,
                "sample_rate": audio.sample_rate,
                "channels": audio.channels,
                "bits": audio.sample_width * 8,
                "duration_seconds": len(audio.samples) / audio.sample_rate,
                "peak_dbfs": db(np.max(np.abs(audio.samples))),
                "clip_samples": int(np.count_nonzero(np.abs(audio.samples) >= 0.999)),
                "longest_zero_run_samples": longest_zero_run(audio.samples),
                "dropped_audio": False,
                "detected_onsets": len(onsets),
                "onset_seconds": " ".join(f"{value:.3f}" for value in onsets),
            }
        )
        events.extend(event_rows(audio, source, condition, direction, filename, onsets))

    summaries = summarize(events)
    validation_path = args.output_directory / "validation.csv"
    events_path = args.output_directory / "events.csv"
    summary_path = args.output_directory / "summary.json"
    with validation_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(validation_rows[0]))
        writer.writeheader()
        writer.writerows(validation_rows)
    with events_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(events[0]))
        writer.writeheader()
        writer.writerows(events)
    summary_path.write_text(json.dumps(summaries, indent=2) + "\n", encoding="utf-8")

    for row in validation_rows:
        print(
            f"{row['request']} {row['take']}: {row['detected_onsets']} onsets "
            f"[{row['onset_seconds']}] peak={row['peak_dbfs']:.2f} dBFS "
            f"zeros={row['longest_zero_run_samples']} clips={row['clip_samples']}"
        )
    print(validation_path)
    print(events_path)
    print(summary_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
