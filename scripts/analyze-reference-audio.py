#!/usr/bin/env python3
"""Create a small dependency-free manifest for reference WAV files."""

from __future__ import annotations

import argparse
import csv
import math
import struct
import wave
from pathlib import Path


def iter_pcm_values(raw: bytes, sample_width: int):
    if sample_width == 1:
        for value in raw:
            yield (value - 128) / 128.0
        return

    if sample_width == 2:
        count = len(raw) // 2
        for (value,) in struct.iter_unpack("<h", raw[: count * 2]):
            yield value / 32768.0
        return

    if sample_width == 3:
        for index in range(0, len(raw) - 2, 3):
            value = raw[index] | (raw[index + 1] << 8) | (raw[index + 2] << 16)
            if value & 0x800000:
                value -= 0x1000000
            yield value / 8388608.0
        return

    if sample_width == 4:
        count = len(raw) // 4
        for (value,) in struct.iter_unpack("<i", raw[: count * 4]):
            yield value / 2147483648.0
        return

    raise ValueError(f"unsupported sample width: {sample_width}")


def analyze_wav(path: Path, root: Path) -> dict[str, str]:
    with wave.open(str(path), "rb") as wav:
        channels = wav.getnchannels()
        sample_rate = wav.getframerate()
        sample_width = wav.getsampwidth()
        frame_count = wav.getnframes()

        peak = 0.0
        sum_squares = 0.0
        sample_count = 0

        while True:
            raw = wav.readframes(32768)
            if not raw:
                break

            for value in iter_pcm_values(raw, sample_width):
                abs_value = abs(value)
                if abs_value > peak:
                    peak = abs_value
                sum_squares += value * value
                sample_count += 1

    duration = frame_count / sample_rate if sample_rate else 0.0
    rms = math.sqrt(sum_squares / sample_count) if sample_count else 0.0

    return {
        "path": path.relative_to(root).as_posix(),
        "bytes": str(path.stat().st_size),
        "sample_rate": str(sample_rate),
        "channels": str(channels),
        "sample_width_bytes": str(sample_width),
        "frames": str(frame_count),
        "duration_seconds": f"{duration:.6f}",
        "peak": f"{peak:.6f}",
        "rms": f"{rms:.6f}",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("root", type=Path, help="Directory containing extracted reference audio")
    parser.add_argument("--output", type=Path, required=True, help="CSV manifest output path")
    args = parser.parse_args()

    root = args.root.expanduser().resolve()
    output = args.output.expanduser().resolve()

    if not root.exists():
        raise SystemExit(f"reference root does not exist: {root}")

    wav_paths = sorted(
        path for path in root.rglob("*")
        if path.suffix.lower() == ".wav"
        and "__MACOSX" not in path.parts
        and not path.name.startswith("._")
    )

    output.parent.mkdir(parents=True, exist_ok=True)

    fieldnames = [
        "path",
        "bytes",
        "sample_rate",
        "channels",
        "sample_width_bytes",
        "frames",
        "duration_seconds",
        "peak",
        "rms",
    ]

    with output.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()

        processed = 0

        for path in wav_paths:
            try:
                writer.writerow(analyze_wav(path, root))
                processed += 1
            except (wave.Error, EOFError, ValueError) as exc:
                print(f"Skipping {path}: {exc}")

    print(f"Analyzed {processed} WAV files")
    print(f"Wrote {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
