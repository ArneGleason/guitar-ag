#!/usr/bin/env python3
"""Perceptual-ish comparison between Guitar AG renders and DI references.

This is not a final realism score. It is a discovery report intended to catch
cases where simple harmonic ratios improve while the sound becomes hissy,
overly static, or otherwise unlike an electric guitar string.
"""

from __future__ import annotations

import argparse
import csv
import math
import wave
from dataclasses import dataclass
from pathlib import Path


@dataclass
class NoteRegion:
    note: int
    note_name: str
    velocity: int
    start_seconds: float
    end_seconds: float
    duration_seconds: float


@dataclass
class AudioData:
    sample_rate: int
    channels: int
    samples: list[float]


@dataclass
class FrameFeatures:
    time_seconds: float
    rms: float
    log_power: list[float]
    centroid_hz: float
    flatness: float
    rolloff_hz: float
    harmonicity: float


def read_notes(path: Path) -> list[NoteRegion]:
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        return [
            NoteRegion(note=int(row["note"]),
                       note_name=row["note_name"],
                       velocity=int(row["velocity"]),
                       start_seconds=float(row["start_seconds"]),
                       end_seconds=float(row["end_seconds"]),
                       duration_seconds=float(row["duration_seconds"]))
            for row in reader
        ]


def pcm_to_float(raw: bytes, sample_width: int) -> list[float]:
    values: list[float] = []

    if sample_width == 1:
        return [(value - 128) / 128.0 for value in raw]

    if sample_width == 2:
        for index in range(0, len(raw) - 1, 2):
            value = int.from_bytes(raw[index:index + 2], byteorder="little", signed=True)
            values.append(value / 32768.0)
        return values

    if sample_width == 3:
        for index in range(0, len(raw) - 2, 3):
            value = raw[index] | (raw[index + 1] << 8) | (raw[index + 2] << 16)
            if value & 0x800000:
                value -= 0x1000000
            values.append(value / 8388608.0)
        return values

    if sample_width == 4:
        for index in range(0, len(raw) - 3, 4):
            value = int.from_bytes(raw[index:index + 4], byteorder="little", signed=True)
            values.append(value / 2147483648.0)
        return values

    raise ValueError(f"unsupported sample width: {sample_width}")


def read_wav_mono(path: Path) -> AudioData:
    with wave.open(str(path), "rb") as wav:
        sample_rate = wav.getframerate()
        channels = wav.getnchannels()
        sample_width = wav.getsampwidth()
        raw = wav.readframes(wav.getnframes())

    interleaved = pcm_to_float(raw, sample_width)

    if channels == 1:
        return AudioData(sample_rate=sample_rate, channels=channels, samples=interleaved)

    mono = []

    for index in range(0, len(interleaved) - channels + 1, channels):
        mono.append(sum(interleaved[index:index + channels]) / channels)

    return AudioData(sample_rate=sample_rate, channels=channels, samples=mono)


def slice_samples(audio: AudioData, start_seconds: float, duration_seconds: float) -> list[float]:
    start = max(0, int(round(start_seconds * audio.sample_rate)))
    length = max(1, int(round(duration_seconds * audio.sample_rate)))
    return audio.samples[start:start + length]


def rms(samples: list[float]) -> float:
    if not samples:
        return 0.0
    return math.sqrt(sum(sample * sample for sample in samples) / len(samples))


def mean(values: list[float]) -> float:
    if not values:
        return 0.0
    return sum(values) / len(values)


def db(value: float) -> float:
    return 20.0 * math.log10(max(value, 1.0e-12))


def midi_frequency(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def log_frequency_bins(min_hz: float, max_hz: float, count: int) -> list[float]:
    if count < 2:
        return [min_hz]

    log_min = math.log(min_hz)
    log_max = math.log(max_hz)
    return [math.exp(log_min + (log_max - log_min) * index / (count - 1)) for index in range(count)]


def hann_window(length: int) -> list[float]:
    if length <= 1:
        return [1.0] * max(1, length)
    return [0.5 - 0.5 * math.cos(2.0 * math.pi * index / (length - 1)) for index in range(length)]


def goertzel_power(samples: list[float], sample_rate: int, frequency: float, window_values: list[float]) -> float:
    if not samples or frequency <= 0.0 or frequency >= sample_rate * 0.5:
        return 0.0

    omega = 2.0 * math.pi * frequency / sample_rate
    coefficient = 2.0 * math.cos(omega)
    state_1 = 0.0
    state_2 = 0.0
    window_energy = 0.0

    for sample, window_value in zip(samples, window_values):
        value = sample * window_value
        state_0 = value + coefficient * state_1 - state_2
        state_2 = state_1
        state_1 = state_0
        window_energy += window_value * window_value

    power = state_1 * state_1 + state_2 * state_2 - coefficient * state_1 * state_2
    return power / max(window_energy, 1.0e-12)


def band_features(samples: list[float],
                  sample_rate: int,
                  note: int,
                  frequencies: list[float],
                  window_values: list[float]) -> tuple[list[float], float, float, float, float]:
    powers = [goertzel_power(samples, sample_rate, frequency, window_values) for frequency in frequencies]
    total = sum(powers)
    safe_powers = [max(power, 1.0e-18) for power in powers]
    log_power = [10.0 * math.log10(power) for power in safe_powers]

    centroid = sum(frequency * power for frequency, power in zip(frequencies, powers)) / max(total, 1.0e-18)
    geometric = math.exp(mean([math.log(power) for power in safe_powers]))
    flatness = geometric / max(total / max(1, len(powers)), 1.0e-18)

    rolloff_target = total * 0.85
    cumulative = 0.0
    rolloff = frequencies[-1] if frequencies else 0.0

    for frequency, power in zip(frequencies, powers):
        cumulative += power
        if cumulative >= rolloff_target:
            rolloff = frequency
            break

    fundamental = midi_frequency(note)
    harmonic_indices = set()

    for harmonic in range(1, 17):
        harmonic_frequency = fundamental * harmonic
        if harmonic_frequency >= sample_rate * 0.45:
            break

        for index, frequency in enumerate(frequencies):
            if abs(frequency - harmonic_frequency) / harmonic_frequency < 0.045:
                harmonic_indices.add(index)

    harmonic_power = sum(powers[index] for index in harmonic_indices)
    harmonicity = harmonic_power / max(total, 1.0e-18)
    return (log_power, centroid, flatness, rolloff, harmonicity)


def frame_features(samples: list[float],
                   sample_rate: int,
                   note: int,
                   duration_seconds: float,
                   window_seconds: float,
                   hop_seconds: float,
                   frequencies: list[float]) -> list[FrameFeatures]:
    window_samples = max(64, int(round(window_seconds * sample_rate)))
    hop_samples = max(1, int(round(hop_seconds * sample_rate)))
    max_samples = min(len(samples), int(round(duration_seconds * sample_rate)))
    window_values = hann_window(window_samples)
    frames: list[FrameFeatures] = []

    if max_samples < window_samples:
        return frames

    for start in range(0, max_samples - window_samples + 1, hop_samples):
        frame = samples[start:start + window_samples]
        log_power, centroid, flatness, rolloff, harmonicity = band_features(frame,
                                                                           sample_rate,
                                                                           note,
                                                                           frequencies,
                                                                           window_values)
        frames.append(FrameFeatures(time_seconds=start / sample_rate,
                                    rms=rms(frame),
                                    log_power=log_power,
                                    centroid_hz=centroid,
                                    flatness=flatness,
                                    rolloff_hz=rolloff,
                                    harmonicity=harmonicity))

    return frames


def scale_to_reference(render: list[float], reference: list[float]) -> tuple[list[float], float]:
    render_rms = rms(render)
    reference_rms = rms(reference)

    if render_rms <= 1.0e-12:
        return (render[:], 0.0)

    gain = reference_rms / render_rms
    return ([sample * gain for sample in render], gain)


def region_frames(frames: list[FrameFeatures], start: float, end: float) -> list[FrameFeatures]:
    return [frame for frame in frames if start <= frame.time_seconds < end]


def log_spectral_distance(render_frames: list[FrameFeatures], reference_frames: list[FrameFeatures]) -> float:
    count = min(len(render_frames), len(reference_frames))

    if count == 0:
        return 0.0

    distances = []

    for render_frame, reference_frame in zip(render_frames[:count], reference_frames[:count]):
        bins = min(len(render_frame.log_power), len(reference_frame.log_power))
        if bins == 0:
            continue
        squared = [(render_frame.log_power[index] - reference_frame.log_power[index]) ** 2.0
                   for index in range(bins)]
        distances.append(math.sqrt(mean(squared)))

    return mean(distances)


def mean_feature(frames: list[FrameFeatures], attribute: str) -> float:
    return mean([getattr(frame, attribute) for frame in frames])


def spectral_flux(frames: list[FrameFeatures]) -> float:
    if len(frames) < 2:
        return 0.0

    values = []

    for previous, current in zip(frames, frames[1:]):
        bins = min(len(previous.log_power), len(current.log_power))
        positive = [max(0.0, current.log_power[index] - previous.log_power[index]) for index in range(bins)]
        values.append(mean(positive))

    return mean(values)


def attack_decay_db(frames: list[FrameFeatures]) -> float:
    attack = region_frames(frames, 0.0, 0.08)
    early = region_frames(frames, 0.20, 0.45)
    attack_rms = mean_feature(attack, "rms")
    early_rms = mean_feature(early, "rms")
    return db(early_rms / max(attack_rms, 1.0e-12))


def summarize_pair(render_frames: list[FrameFeatures],
                   reference_frames: list[FrameFeatures]) -> dict[str, float]:
    regions = {
        "attack": (0.00, 0.10),
        "early": (0.10, 0.45),
        "late": (0.70, 1.25),
    }
    values: dict[str, float] = {}

    values["full_log_spectral_distance_db"] = log_spectral_distance(render_frames, reference_frames)

    for name, (start, end) in regions.items():
        values[f"{name}_log_spectral_distance_db"] = log_spectral_distance(region_frames(render_frames, start, end),
                                                                           region_frames(reference_frames, start, end))

    render_flatness = mean_feature(render_frames, "flatness")
    reference_flatness = mean_feature(reference_frames, "flatness")
    render_harmonicity = mean_feature(render_frames, "harmonicity")
    reference_harmonicity = mean_feature(reference_frames, "harmonicity")

    values["render_centroid_hz"] = mean_feature(render_frames, "centroid_hz")
    values["reference_centroid_hz"] = mean_feature(reference_frames, "centroid_hz")
    values["render_flatness"] = render_flatness
    values["reference_flatness"] = reference_flatness
    values["flatness_ratio_render_to_reference"] = render_flatness / max(reference_flatness, 1.0e-12)
    values["render_rolloff_hz"] = mean_feature(render_frames, "rolloff_hz")
    values["reference_rolloff_hz"] = mean_feature(reference_frames, "rolloff_hz")
    values["render_harmonicity"] = render_harmonicity
    values["reference_harmonicity"] = reference_harmonicity
    values["harmonicity_delta_render_minus_reference"] = render_harmonicity - reference_harmonicity
    values["residual_ratio_render"] = (1.0 - render_harmonicity) / max(render_harmonicity, 1.0e-12)
    values["residual_ratio_reference"] = (1.0 - reference_harmonicity) / max(reference_harmonicity, 1.0e-12)
    values["harmonicity_ratio_render_to_reference"] = render_harmonicity / max(reference_harmonicity, 1.0e-12)
    values["render_spectral_flux"] = spectral_flux(render_frames)
    values["reference_spectral_flux"] = spectral_flux(reference_frames)
    values["spectral_flux_ratio_render_to_reference"] = values["render_spectral_flux"] / max(values["reference_spectral_flux"], 1.0e-12)
    values["render_attack_decay_db"] = attack_decay_db(render_frames)
    values["reference_attack_decay_db"] = attack_decay_db(reference_frames)
    return values


def loud_reference_candidates(notes: list[NoteRegion],
                              target: NoteRegion,
                              reference_audio: AudioData,
                              duration_seconds: float,
                              count: int) -> list[NoteRegion]:
    matching = [note for note in notes if note.note == target.note]

    if not matching:
        raise ValueError(f"no reference note found for MIDI note {target.note}")

    def candidate_rms(note: NoteRegion) -> float:
        return rms(slice_samples(reference_audio, note.start_seconds, min(duration_seconds, note.duration_seconds)))

    return sorted(matching, key=candidate_rms, reverse=True)[:max(1, count)]


def format_float(value: float) -> str:
    return f"{value:.6f}"


def write_csv(path: Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "note_name",
        "note",
        "render_start_seconds",
        "reference_start_seconds",
        "reference_rank",
        "render_gain_to_reference",
        "full_log_spectral_distance_db",
        "attack_log_spectral_distance_db",
        "early_log_spectral_distance_db",
        "late_log_spectral_distance_db",
        "render_centroid_hz",
        "reference_centroid_hz",
        "render_flatness",
        "reference_flatness",
        "flatness_ratio_render_to_reference",
        "render_rolloff_hz",
        "reference_rolloff_hz",
        "render_harmonicity",
        "reference_harmonicity",
        "harmonicity_delta_render_minus_reference",
        "harmonicity_ratio_render_to_reference",
        "residual_ratio_render",
        "residual_ratio_reference",
        "render_spectral_flux",
        "reference_spectral_flux",
        "spectral_flux_ratio_render_to_reference",
        "render_attack_decay_db",
        "reference_attack_decay_db",
    ]

    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def write_summary(path: Path, rows: list[dict[str, str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    numeric_fields = [
        "full_log_spectral_distance_db",
        "attack_log_spectral_distance_db",
        "early_log_spectral_distance_db",
        "late_log_spectral_distance_db",
        "flatness_ratio_render_to_reference",
        "harmonicity_delta_render_minus_reference",
        "spectral_flux_ratio_render_to_reference",
    ]

    lines = ["# Perceptual Analysis Summary", ""]

    if rows:
        lines.append("## Means")
        lines.append("")
        for field in numeric_fields:
            values = [float(row[field]) for row in rows]
            lines.append(f"- `{field}`: {mean(values):.4f}")
        lines.append("")

    lines.append("## Notes")
    lines.append("")
    lines.append("| Note | Ref start | Full dB | Attack dB | Early dB | Flatness x | Harmonicity delta | Flux x |")
    lines.append("|---|---:|---:|---:|---:|---:|---:|---:|")

    for row in rows:
        lines.append("| "
                     f"{row['note_name']} | "
                     f"{float(row['reference_start_seconds']):.3f} | "
                     f"{float(row['full_log_spectral_distance_db']):.2f} | "
                     f"{float(row['attack_log_spectral_distance_db']):.2f} | "
                     f"{float(row['early_log_spectral_distance_db']):.2f} | "
                     f"{float(row['flatness_ratio_render_to_reference']):.2f} | "
                     f"{float(row['harmonicity_delta_render_minus_reference']):.3f} | "
                     f"{float(row['spectral_flux_ratio_render_to_reference']):.2f} |")

    path.write_text("\n".join(lines) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-wav", type=Path, required=True)
    parser.add_argument("--render-notes", type=Path, required=True)
    parser.add_argument("--reference-wav", type=Path, required=True)
    parser.add_argument("--reference-notes", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--summary", type=Path)
    parser.add_argument("--duration", type=float, default=1.35)
    parser.add_argument("--window", type=float, default=0.046)
    parser.add_argument("--hop", type=float, default=0.023)
    parser.add_argument("--bins", type=int, default=56)
    parser.add_argument("--min-frequency", type=float, default=55.0)
    parser.add_argument("--max-frequency", type=float, default=12000.0)
    parser.add_argument("--reference-candidates", type=int, default=3)
    parser.add_argument("--long-notes-only", action="store_true")
    args = parser.parse_args()

    render_audio = read_wav_mono(args.render_wav.expanduser().resolve())
    reference_audio = read_wav_mono(args.reference_wav.expanduser().resolve())
    render_notes = read_notes(args.render_notes.expanduser().resolve())
    reference_notes = read_notes(args.reference_notes.expanduser().resolve())

    if args.long_notes_only:
        render_notes = [note for note in render_notes if note.duration_seconds > 1.0]

    if render_audio.sample_rate != reference_audio.sample_rate:
        raise ValueError("render and reference WAVs must use the same sample rate")

    frequencies = log_frequency_bins(args.min_frequency,
                                     min(args.max_frequency, render_audio.sample_rate * 0.45),
                                     args.bins)
    rows: list[dict[str, str]] = []

    for render_note in render_notes:
        render_samples = slice_samples(render_audio, render_note.start_seconds, args.duration)
        candidates = loud_reference_candidates(reference_notes,
                                               render_note,
                                               reference_audio,
                                               args.duration,
                                               args.reference_candidates)
        best_row: dict[str, str] | None = None
        best_distance = float("inf")

        for rank, reference_note in enumerate(candidates, start=1):
            reference_samples = slice_samples(reference_audio, reference_note.start_seconds, args.duration)
            scaled_render, gain = scale_to_reference(render_samples, reference_samples)
            render_frames = frame_features(scaled_render,
                                           render_audio.sample_rate,
                                           render_note.note,
                                           args.duration,
                                           args.window,
                                           args.hop,
                                           frequencies)
            reference_frames = frame_features(reference_samples,
                                             reference_audio.sample_rate,
                                             reference_note.note,
                                             args.duration,
                                             args.window,
                                             args.hop,
                                             frequencies)
            summary = summarize_pair(render_frames, reference_frames)
            distance = summary["full_log_spectral_distance_db"]

            row = {
                "note_name": render_note.note_name,
                "note": str(render_note.note),
                "render_start_seconds": f"{render_note.start_seconds:.6f}",
                "reference_start_seconds": f"{reference_note.start_seconds:.6f}",
                "reference_rank": str(rank),
                "render_gain_to_reference": format_float(gain),
            }
            row.update({key: format_float(value) for key, value in summary.items()})

            if distance < best_distance:
                best_distance = distance
                best_row = row

        if best_row is not None:
            rows.append(best_row)

    output = args.output.expanduser().resolve()
    write_csv(output, rows)

    if args.summary:
        write_summary(args.summary.expanduser().resolve(), rows)

    print(f"Analyzed {len(rows)} notes")
    print(f"Wrote {output}")
    if args.summary:
        print(f"Wrote {args.summary.expanduser().resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
