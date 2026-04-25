#!/usr/bin/env python3
"""Analyze time-varying harmonic behavior in calibration renders and references."""

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
class HarmonicFrame:
    time_seconds: float
    amplitude: list[float]
    phase: list[float]


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


def midi_frequency(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def slice_samples(audio: AudioData, start_seconds: float, duration_seconds: float) -> list[float]:
    start = max(0, int(round(start_seconds * audio.sample_rate)))
    length = max(1, int(round(duration_seconds * audio.sample_rate)))
    return audio.samples[start:start + length]


def rms(samples: list[float]) -> float:
    if not samples:
        return 0.0

    return math.sqrt(sum(sample * sample for sample in samples) / len(samples))


def loudest_reference_note(notes: list[NoteRegion],
                           target: NoteRegion,
                           reference_audio: AudioData,
                           duration_seconds: float) -> NoteRegion:
    matching = [note for note in notes if note.note == target.note]

    if not matching:
        raise ValueError(f"no reference note found for MIDI note {target.note}")

    def candidate_rms(note: NoteRegion) -> float:
        return rms(slice_samples(reference_audio, note.start_seconds, min(duration_seconds, note.duration_seconds)))

    return max(matching, key=candidate_rms)


def harmonic_coefficient(samples: list[float], sample_rate: int, frequency: float) -> tuple[float, float]:
    length = len(samples)

    if length < 2:
        return (0.0, 0.0)

    omega = 2.0 * math.pi * frequency / sample_rate
    cos_step = math.cos(omega)
    sin_step = math.sin(omega)
    cos_value = 1.0
    sin_value = 0.0
    real = 0.0
    imag = 0.0
    window_sum = 0.0

    for index, sample in enumerate(samples):
        window_value = 0.5 - 0.5 * math.cos(2.0 * math.pi * index / (length - 1))
        value = sample * window_value
        real += value * cos_value
        imag -= value * sin_value
        window_sum += window_value

        next_cos = cos_value * cos_step - sin_value * sin_step
        sin_value = sin_value * cos_step + cos_value * sin_step
        cos_value = next_cos

    scale = 2.0 / max(window_sum, 1.0)
    return (real * scale, imag * scale)


def harmonic_frames(samples: list[float],
                    sample_rate: int,
                    note: int,
                    duration_seconds: float,
                    window_seconds: float,
                    hop_seconds: float,
                    harmonics: int) -> list[HarmonicFrame]:
    window_samples = max(16, int(round(window_seconds * sample_rate)))
    hop_samples = max(1, int(round(hop_seconds * sample_rate)))
    max_start = min(len(samples), int(round(duration_seconds * sample_rate))) - window_samples
    fundamental = midi_frequency(note)
    frames: list[HarmonicFrame] = []

    for start in range(0, max_start + 1, hop_samples):
        frame = samples[start:start + window_samples]
        amplitudes: list[float] = []
        phases: list[float] = []

        for harmonic in range(1, harmonics + 1):
            frequency = fundamental * harmonic

            if frequency >= sample_rate * 0.45:
                amplitudes.append(0.0)
                phases.append(0.0)
                continue

            real, imag = harmonic_coefficient(frame, sample_rate, frequency)
            amplitudes.append(math.sqrt(real * real + imag * imag))
            phases.append(math.atan2(imag, real))

        frames.append(HarmonicFrame(time_seconds=start / sample_rate,
                                    amplitude=amplitudes,
                                    phase=phases))

    return frames


def unwrap_delta(delta: float) -> float:
    while delta > math.pi:
        delta -= 2.0 * math.pi
    while delta < -math.pi:
        delta += 2.0 * math.pi
    return delta


def mean(values: list[float]) -> float:
    if not values:
        return 0.0
    return sum(values) / len(values)


def standard_deviation(values: list[float]) -> float:
    if len(values) < 2:
        return 0.0
    average = mean(values)
    return math.sqrt(sum((value - average) ** 2.0 for value in values) / (len(values) - 1))


def db(value: float) -> float:
    return 20.0 * math.log10(max(value, 1.0e-12))


def amplitude_at(frames: list[HarmonicFrame], target_seconds: float, harmonic: int) -> float:
    if not frames:
        return 0.0

    frame = min(frames, key=lambda item: abs(item.time_seconds - target_seconds))
    return frame.amplitude[harmonic - 1]


def ratio_at(frames: list[HarmonicFrame], target_seconds: float, numerator: range, denominator: range) -> float:
    if not frames:
        return 0.0

    frame = min(frames, key=lambda item: abs(item.time_seconds - target_seconds))
    top = sum(frame.amplitude[index - 1] ** 2.0 for index in numerator)
    bottom = sum(frame.amplitude[index - 1] ** 2.0 for index in denominator)
    return top / bottom if bottom > 0.0 else 0.0


def relative_db_spread(frames: list[HarmonicFrame], start_harmonic: int, end_harmonic: int) -> float:
    spreads = []

    for frame in frames:
        fundamental = frame.amplitude[0]
        if fundamental <= 1.0e-9:
            continue

        relative = [db(frame.amplitude[harmonic - 1] / fundamental)
                    for harmonic in range(start_harmonic, end_harmonic + 1)]
        spreads.append(max(relative) - min(relative))

    return mean(spreads)


def strongest_harmonic_shift(frames: list[HarmonicFrame], start_harmonic: int, end_harmonic: int) -> float:
    strongest = []

    for frame in frames:
        harmonic = max(range(start_harmonic, end_harmonic + 1),
                       key=lambda item: frame.amplitude[item - 1])
        strongest.append(float(harmonic))

    return standard_deviation(strongest)


def phase_drift_cents(frames: list[HarmonicFrame],
                      note: int,
                      sample_rate: int,
                      hop_seconds: float,
                      harmonic: int) -> list[float]:
    if len(frames) < 2:
        return []

    fundamental = midi_frequency(note)
    frequency = fundamental * harmonic
    if frequency >= sample_rate * 0.45:
        return []

    values = []

    for previous, current in zip(frames, frames[1:]):
        expected = 2.0 * math.pi * frequency * (current.time_seconds - previous.time_seconds)
        delta = unwrap_delta(current.phase[harmonic - 1] - previous.phase[harmonic - 1] - expected)
        instantaneous = frequency + delta / (2.0 * math.pi * max(hop_seconds, 1.0e-6))

        if instantaneous > 0.0:
            values.append(1200.0 * math.log2(instantaneous / frequency))

    return values


def summarize_frames(label: str,
                     note: NoteRegion,
                     frames: list[HarmonicFrame],
                     sample_rate: int,
                     hop_seconds: float) -> dict[str, str]:
    early_upper = ratio_at(frames, 0.04, range(4, 13), range(1, 4))
    mid_upper = ratio_at(frames, 0.32, range(4, 13), range(1, 4))
    late_upper = ratio_at(frames, 0.95, range(4, 13), range(1, 4))
    early_air = ratio_at(frames, 0.04, range(9, 17), range(1, 4))
    mid_air = ratio_at(frames, 0.32, range(9, 17), range(1, 4))
    h2_decay = db(amplitude_at(frames, 0.95, 2) / max(amplitude_at(frames, 0.04, 2), 1.0e-12))
    h8_decay = db(amplitude_at(frames, 0.95, 8) / max(amplitude_at(frames, 0.04, 8), 1.0e-12))

    phase_values = []
    for harmonic in range(2, 9):
        phase_values.extend(phase_drift_cents(frames, note.note, sample_rate, hop_seconds, harmonic))

    return {
        "source": label,
        "note_name": note.note_name,
        "note": str(note.note),
        "early_upper_to_low": f"{early_upper:.8f}",
        "mid_upper_to_low": f"{mid_upper:.8f}",
        "late_upper_to_low": f"{late_upper:.8f}",
        "early_air_to_low": f"{early_air:.8f}",
        "mid_air_to_low": f"{mid_air:.8f}",
        "h2_decay_db_40ms_to_950ms": f"{h2_decay:.4f}",
        "h8_decay_db_40ms_to_950ms": f"{h8_decay:.4f}",
        "relative_h4_h12_spread_db": f"{relative_db_spread(frames, 4, 12):.4f}",
        "strongest_h4_h12_shift_std": f"{strongest_harmonic_shift(frames, 4, 12):.4f}",
        "phase_drift_cents_std_h2_h8": f"{standard_deviation(phase_values):.4f}",
        "phase_drift_cents_mean_abs_h2_h8": f"{mean([abs(value) for value in phase_values]):.4f}",
    }


def write_rows(output: Path, rows: list[dict[str, str]]) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)

    fieldnames = [
        "source",
        "note_name",
        "note",
        "early_upper_to_low",
        "mid_upper_to_low",
        "late_upper_to_low",
        "early_air_to_low",
        "mid_air_to_low",
        "h2_decay_db_40ms_to_950ms",
        "h8_decay_db_40ms_to_950ms",
        "relative_h4_h12_spread_db",
        "strongest_h4_h12_shift_std",
        "phase_drift_cents_std_h2_h8",
        "phase_drift_cents_mean_abs_h2_h8",
    ]

    with output.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-wav", type=Path, required=True)
    parser.add_argument("--render-notes", type=Path, required=True)
    parser.add_argument("--reference-wav", type=Path, required=True)
    parser.add_argument("--reference-notes", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--duration", type=float, default=1.35)
    parser.add_argument("--window", type=float, default=0.120)
    parser.add_argument("--hop", type=float, default=0.040)
    parser.add_argument("--harmonics", type=int, default=16)
    parser.add_argument("--long-notes-only", action="store_true")
    args = parser.parse_args()

    render_audio = read_wav_mono(args.render_wav.expanduser().resolve())
    reference_audio = read_wav_mono(args.reference_wav.expanduser().resolve())
    render_notes = read_notes(args.render_notes.expanduser().resolve())
    reference_notes = read_notes(args.reference_notes.expanduser().resolve())

    if args.long_notes_only:
        render_notes = [note for note in render_notes if note.duration_seconds > 1.0]

    rows: list[dict[str, str]] = []

    for render_note in render_notes:
        reference_note = loudest_reference_note(reference_notes, render_note, reference_audio, args.duration)
        render_samples = slice_samples(render_audio, render_note.start_seconds, args.duration)
        reference_samples = slice_samples(reference_audio, reference_note.start_seconds, args.duration)

        render_frames = harmonic_frames(render_samples,
                                        render_audio.sample_rate,
                                        render_note.note,
                                        args.duration,
                                        args.window,
                                        args.hop,
                                        args.harmonics)
        reference_frames = harmonic_frames(reference_samples,
                                          reference_audio.sample_rate,
                                          reference_note.note,
                                          args.duration,
                                          args.window,
                                          args.hop,
                                          args.harmonics)

        rows.append(summarize_frames("render", render_note, render_frames, render_audio.sample_rate, args.hop))
        rows.append(summarize_frames("reference", reference_note, reference_frames, reference_audio.sample_rate, args.hop))

    write_rows(args.output.expanduser().resolve(), rows)
    print(f"Analyzed {len(render_notes)} render notes and references")
    print(f"Wrote {args.output.expanduser().resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
