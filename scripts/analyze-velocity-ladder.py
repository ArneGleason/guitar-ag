#!/usr/bin/env python3
"""Summarize how a Guitar AG render changes across a velocity ladder."""

from __future__ import annotations

import argparse
import csv
import math
import wave
from collections import defaultdict
from dataclasses import dataclass
from pathlib import Path


@dataclass
class NoteRegion:
    note: int
    note_name: str
    velocity: int
    start_seconds: float
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
                       duration_seconds=float(row["duration_seconds"]))
            for row in reader
        ]


def pcm_to_float(raw: bytes, sample_width: int) -> list[float]:
    if sample_width == 1:
        return [(value - 128) / 128.0 for value in raw]

    values: list[float] = []

    if sample_width == 2:
        for index in range(0, len(raw) - 1, 2):
            values.append(int.from_bytes(raw[index:index + 2], "little", signed=True) / 32768.0)
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
            values.append(int.from_bytes(raw[index:index + 4], "little", signed=True) / 2147483648.0)
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


def mean_abs(samples: list[float]) -> float:
    if not samples:
        return 0.0
    return sum(abs(sample) for sample in samples) / len(samples)


def midi_frequency(note: int) -> float:
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def harmonic_power(samples: list[float], sample_rate: int, frequency: float) -> float:
    if not samples or frequency <= 0.0 or frequency >= sample_rate * 0.48:
        return 0.0

    omega = 2.0 * math.pi * frequency / sample_rate
    real = 0.0
    imag = 0.0
    length = len(samples)

    for index, sample in enumerate(samples):
        window = 0.5 - 0.5 * math.cos (2.0 * math.pi * index / max(1, length - 1))
        value = sample * window
        real += value * math.cos (omega * index)
        imag -= value * math.sin (omega * index)

    return (real * real + imag * imag) / max(1, length)


def harmonic_ratios(samples: list[float], sample_rate: int, note: int) -> tuple[float, float]:
    fundamental = midi_frequency(note)
    low = 0.0
    upper = 0.0
    air = 0.0

    for harmonic in range(1, 25):
        frequency = fundamental * harmonic
        if frequency >= sample_rate * 0.45:
            break

        power = harmonic_power(samples, sample_rate, frequency)

        if harmonic <= 4:
            low += power
        elif harmonic <= 14:
            upper += power
        else:
            air += power

    return upper / max(low, 1.0e-18), air / max(low, 1.0e-18)


def analyze_note(audio: AudioData, note: NoteRegion) -> dict[str, float | int | str]:
    full = slice_samples(audio, note.start_seconds, note.duration_seconds)
    attack = slice_samples(audio, note.start_seconds, 0.060)
    sustain = slice_samples(audio, note.start_seconds + 0.300, 0.300)
    early = slice_samples(audio, note.start_seconds + 0.030, 0.370)
    upper_to_low, air_to_low = harmonic_ratios(early, audio.sample_rate, note.note)

    return {
        "note_name": note.note_name,
        "note": note.note,
        "velocity": note.velocity,
        "rms": rms(full),
        "attack_to_sustain": mean_abs(attack) / max(mean_abs(sustain), 1.0e-12),
        "upper_to_low": upper_to_low,
        "air_to_low": air_to_low,
    }


def write_csv(path: Path, rows: list[dict[str, float | int | str]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = ["note_name", "note", "velocity", "rms", "attack_to_sustain", "upper_to_low", "air_to_low"]

    with path.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def write_summary(path: Path, rows: list[dict[str, float | int | str]]) -> None:
    by_note: dict[str, list[dict[str, float | int | str]]] = defaultdict(list)

    for row in rows:
        by_note[str(row["note_name"])].append(row)

    lines = ["# Velocity Ladder Summary", "", "| Note | RMS x | Attack/Sustain x | Upper/Low x | Air/Low x |", "|---|---:|---:|---:|---:|"]

    for note_name in sorted(by_note, key=lambda name: int(by_note[name][0]["note"])):
        note_rows = sorted(by_note[note_name], key=lambda row: int(row["velocity"]))
        low = note_rows[0]
        high = note_rows[-1]

        def ratio(key: str) -> float:
            return float(high[key]) / max(float(low[key]), 1.0e-12)

        lines.append(f"| {note_name} | {ratio('rms'):.2f} | {ratio('attack_to_sustain'):.2f} | "
                     f"{ratio('upper_to_low'):.2f} | {ratio('air_to_low'):.2f} |")

    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-wav", required=True, type=Path)
    parser.add_argument("--notes", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--summary", required=True, type=Path)
    args = parser.parse_args()

    audio = read_wav_mono(args.render_wav.expanduser())
    notes = read_notes(args.notes.expanduser())
    rows = [analyze_note(audio, note) for note in notes]

    write_csv(args.output.expanduser(), rows)
    write_summary(args.summary.expanduser(), rows)
    print(f"Analyzed {len(rows)} velocity-ladder notes")
    print(f"Wrote {args.output}")
    print(f"Wrote {args.summary}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
