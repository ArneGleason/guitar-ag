#!/usr/bin/env python3
"""Compare a Guitar AG calibration render with Guitar-TECHS DI note regions."""

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


def peak(samples: list[float]) -> float:
    return max((abs(sample) for sample in samples), default=0.0)


def zero_crossing_rate(samples: list[float]) -> float:
    if len(samples) < 2:
        return 0.0

    crossings = 0
    previous = samples[0]

    for sample in samples[1:]:
        if (previous <= 0.0 < sample) or (previous >= 0.0 > sample):
            crossings += 1
        previous = sample

    return crossings / (len(samples) - 1)


def average_abs(samples: list[float]) -> float:
    if not samples:
        return 0.0
    return sum(abs(sample) for sample in samples) / len(samples)


def window(samples: list[float], sample_rate: int, start_seconds: float, duration_seconds: float) -> list[float]:
    start = max(0, int(round(start_seconds * sample_rate)))
    length = max(1, int(round(duration_seconds * sample_rate)))
    return samples[start:start + length]


def stats_for(samples: list[float], sample_rate: int) -> dict[str, float]:
    full_rms = rms(samples)
    attack = window(samples, sample_rate, 0.0, 0.05)
    sustain = window(samples, sample_rate, 0.30, 0.25)
    late = window(samples, sample_rate, 1.00, 0.25)
    noise_floor = window(samples, sample_rate, 1.55, 0.20)

    attack_abs = average_abs(attack)
    sustain_abs = average_abs(sustain)
    late_abs = average_abs(late)
    noise_abs = average_abs(noise_floor)

    attack_to_sustain = attack_abs / sustain_abs if sustain_abs > 0.0 else 0.0
    late_to_sustain = late_abs / sustain_abs if sustain_abs > 0.0 else 0.0

    return {
        "peak": peak(samples),
        "rms": full_rms,
        "attack_abs_50ms": attack_abs,
        "sustain_abs_300_550ms": sustain_abs,
        "late_abs_1000_1250ms": late_abs,
        "noise_abs_after_1550ms": noise_abs,
        "attack_to_sustain": attack_to_sustain,
        "late_to_sustain": late_to_sustain,
        "zero_crossing_rate": zero_crossing_rate(samples),
    }


def midi_frequency(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def harmonic_power(samples: list[float], sample_rate: int, frequency: float) -> float:
    if not samples or frequency <= 0.0:
        return 0.0

    omega = 2.0 * math.pi * frequency / sample_rate
    real = 0.0
    imag = 0.0
    length = len(samples)

    for index, sample in enumerate(samples):
        window_value = 0.5 - 0.5 * math.cos (2.0 * math.pi * index / max(1, length - 1))
        value = sample * window_value
        angle = omega * index
        real += value * math.cos(angle)
        imag -= value * math.sin(angle)

    return (real * real + imag * imag) / max(1, length)


def harmonic_stats(samples: list[float], sample_rate: int, note: int) -> dict[str, float]:
    analysis = window(samples, sample_rate, 0.03, 0.35)
    fundamental = midi_frequency(note)
    powers = []

    for harmonic in range(1, 13):
        frequency = fundamental * harmonic

        if frequency >= sample_rate * 0.45:
            powers.append(0.0)
        else:
            powers.append(harmonic_power(analysis, sample_rate, frequency))

    low = sum(powers[0:3])
    upper = sum(powers[3:12])
    even = sum(powers[index] for index in (1, 3, 5, 7, 9, 11))
    odd = sum(powers[index] for index in (0, 2, 4, 6, 8, 10))

    return {
        "upper_to_low_harmonics": upper / low if low > 0.0 else 0.0,
        "even_to_odd_harmonics": even / odd if odd > 0.0 else 0.0,
    }


def closest_reference_note(notes: list[NoteRegion], target: NoteRegion) -> NoteRegion:
    matching = [note for note in notes if note.note == target.note]

    if not matching:
        raise ValueError(f"no reference note found for MIDI note {target.note}")

    return min(matching, key=lambda note: abs(note.velocity - target.velocity))


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


def write_rows(output: Path, rows: list[dict[str, str]]) -> None:
    output.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "note_name",
        "note",
        "render_start_seconds",
        "reference_start_seconds",
        "render_rms",
        "reference_rms",
        "rms_ratio_render_to_reference",
        "render_attack_to_sustain",
        "reference_attack_to_sustain",
        "render_late_to_sustain",
        "reference_late_to_sustain",
        "render_zcr",
        "reference_zcr",
        "render_peak",
        "reference_peak",
        "render_upper_to_low_harmonics",
        "reference_upper_to_low_harmonics",
        "render_even_to_odd_harmonics",
        "reference_even_to_odd_harmonics",
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
    parser.add_argument("--duration", type=float, default=1.7)
    parser.add_argument("--long-notes-only", action="store_true")
    parser.add_argument("--reference-selection",
                        choices=("closest-velocity", "loudest"),
                        default="loudest")
    args = parser.parse_args()

    render_audio = read_wav_mono(args.render_wav.expanduser().resolve())
    reference_audio = read_wav_mono(args.reference_wav.expanduser().resolve())
    render_notes = read_notes(args.render_notes.expanduser().resolve())
    reference_notes = read_notes(args.reference_notes.expanduser().resolve())

    if args.long_notes_only:
        render_notes = [note for note in render_notes if note.duration_seconds > 1.0]

    rows: list[dict[str, str]] = []

    for render_note in render_notes:
        if args.reference_selection == "closest-velocity":
            reference_note = closest_reference_note(reference_notes, render_note)
        else:
            reference_note = loudest_reference_note(reference_notes, render_note, reference_audio, args.duration)
        render_samples = slice_samples(render_audio, render_note.start_seconds, args.duration)
        reference_samples = slice_samples(reference_audio, reference_note.start_seconds, args.duration)
        render_stats = stats_for(render_samples, render_audio.sample_rate)
        reference_stats = stats_for(reference_samples, reference_audio.sample_rate)
        render_harmonics = harmonic_stats(render_samples, render_audio.sample_rate, render_note.note)
        reference_harmonics = harmonic_stats(reference_samples, reference_audio.sample_rate, reference_note.note)

        rows.append({
            "note_name": render_note.note_name,
            "note": str(render_note.note),
            "render_start_seconds": f"{render_note.start_seconds:.6f}",
            "reference_start_seconds": f"{reference_note.start_seconds:.6f}",
            "render_rms": f"{render_stats['rms']:.8f}",
            "reference_rms": f"{reference_stats['rms']:.8f}",
            "rms_ratio_render_to_reference": f"{(render_stats['rms'] / reference_stats['rms']) if reference_stats['rms'] > 0.0 else 0.0:.6f}",
            "render_attack_to_sustain": f"{render_stats['attack_to_sustain']:.6f}",
            "reference_attack_to_sustain": f"{reference_stats['attack_to_sustain']:.6f}",
            "render_late_to_sustain": f"{render_stats['late_to_sustain']:.6f}",
            "reference_late_to_sustain": f"{reference_stats['late_to_sustain']:.6f}",
            "render_zcr": f"{render_stats['zero_crossing_rate']:.8f}",
            "reference_zcr": f"{reference_stats['zero_crossing_rate']:.8f}",
            "render_peak": f"{render_stats['peak']:.8f}",
            "reference_peak": f"{reference_stats['peak']:.8f}",
            "render_upper_to_low_harmonics": f"{render_harmonics['upper_to_low_harmonics']:.8f}",
            "reference_upper_to_low_harmonics": f"{reference_harmonics['upper_to_low_harmonics']:.8f}",
            "render_even_to_odd_harmonics": f"{render_harmonics['even_to_odd_harmonics']:.8f}",
            "reference_even_to_odd_harmonics": f"{reference_harmonics['even_to_odd_harmonics']:.8f}",
        })

    write_rows(args.output.expanduser().resolve(), rows)
    print(f"Compared {len(rows)} notes")
    print(f"Wrote {args.output.expanduser().resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
