#!/usr/bin/env python3
"""Build a compact RMS-matched Plan 0105 current/candidate listening set."""

from __future__ import annotations

import argparse
import json
import math
import wave
from pathlib import Path

import numpy as np


SECTIONS = (
    ("ringing-down", 0.75, 3.00),
    ("ringing-up", 0.75, 3.00),
    ("ringing-down", 11.25, 18.00),
    ("ringing-alternate", 0.75, 7.20),
)


def read_pcm24(path: Path) -> tuple[int, np.ndarray]:
    with wave.open(str(path), "rb") as handle:
        if handle.getsampwidth() != 3:
            raise ValueError(f"expected 24-bit PCM: {path}")
        channels = handle.getnchannels()
        sample_rate = handle.getframerate()
        frames = handle.getnframes()
        packed = np.frombuffer(handle.readframes(frames), dtype=np.uint8).reshape(-1, 3)
    integers = (
        packed[:, 0].astype(np.int32)
        | (packed[:, 1].astype(np.int32) << 8)
        | (packed[:, 2].astype(np.int32) << 16)
    )
    integers = np.where(integers & 0x800000, integers - 0x1000000, integers)
    return sample_rate, integers.astype(np.float64).reshape(-1, channels) / 8388608.0


def write_pcm24(path: Path, sample_rate: int, samples: np.ndarray) -> None:
    clipped = np.clip(samples, -1.0, 1.0 - 1.0 / 8388608.0)
    integers = np.rint(clipped * 8388608.0).astype(np.int32)
    unsigned = integers & 0xFFFFFF
    packed = np.empty((unsigned.size, 3), dtype=np.uint8)
    flat = unsigned.reshape(-1)
    packed[:, 0] = flat & 0xFF
    packed[:, 1] = (flat >> 8) & 0xFF
    packed[:, 2] = (flat >> 16) & 0xFF
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as handle:
        handle.setnchannels(samples.shape[1])
        handle.setsampwidth(3)
        handle.setframerate(sample_rate)
        handle.writeframes(packed.tobytes())


def db(value: float) -> float:
    return 20.0 * math.log10(max(value, 1.0e-12))


def build_track(directory: Path, prefix: str) -> tuple[int, np.ndarray]:
    sections: list[np.ndarray] = []
    sample_rate = 0
    channels = 0
    for name, start_seconds, end_seconds in SECTIONS:
        rate, samples = read_pcm24(directory / f"{prefix}-{name}.wav")
        if sample_rate and rate != sample_rate:
            raise ValueError("source sample rates do not match")
        if channels and samples.shape[1] != channels:
            raise ValueError("source channel counts do not match")
        sample_rate = rate
        channels = samples.shape[1]
        start = round(start_seconds * rate)
        end = round(end_seconds * rate)
        excerpt = samples[start:end].copy()
        fade = min(round(0.008 * rate), len(excerpt) // 2)
        if fade:
            ramp = np.linspace(0.0, 1.0, fade, endpoint=True)[:, None]
            excerpt[:fade] *= ramp
            excerpt[-fade:] *= ramp[::-1]
        sections.append(excerpt)
        sections.append(np.zeros((round(0.40 * rate), channels), dtype=np.float64))
    return sample_rate, np.concatenate(sections[:-1], axis=0)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    args = parser.parse_args()

    current_rate, current = build_track(args.render_directory, "current")
    candidate_rate, candidate = build_track(args.render_directory, "candidate")
    if current_rate != candidate_rate or current.shape != candidate.shape:
        raise ValueError("current and candidate sources do not align")

    current_rms = math.sqrt(float(np.mean(current * current)))
    candidate_rms = math.sqrt(float(np.mean(candidate * candidate)))
    current_peak = float(np.max(np.abs(current)))
    candidate_peak = float(np.max(np.abs(candidate)))
    maximum_target_rms = min(
        current_rms * 0.891 / max(current_peak, 1.0e-12),
        candidate_rms * 0.891 / max(candidate_peak, 1.0e-12),
    )
    target_rms = min(10.0 ** (-18.0 / 20.0), maximum_target_rms)
    current_gain = target_rms / max(current_rms, 1.0e-12)
    candidate_gain = target_rms / max(candidate_rms, 1.0e-12)
    current *= current_gain
    candidate *= candidate_gain

    args.output_directory.mkdir(parents=True, exist_ok=True)
    current_path = args.output_directory / "01-current-eg089-neck-humbucker.wav"
    candidate_path = args.output_directory / "02-reference-candidate-p014-decay150.wav"
    write_pcm24(current_path, current_rate, current)
    write_pcm24(candidate_path, current_rate, candidate)

    pause = np.zeros((round(0.80 * current_rate), current.shape[1]), dtype=np.float64)
    write_pcm24(
        args.output_directory / "03-current-then-candidate.wav",
        current_rate,
        np.concatenate((current, pause, candidate), axis=0),
    )
    write_pcm24(
        args.output_directory / "04-candidate-then-current.wav",
        current_rate,
        np.concatenate((candidate, pause, current), axis=0),
    )

    report = {
        "sample_rate": current_rate,
        "channels": current.shape[1],
        "duration_seconds_each": len(current) / current_rate,
        "sections": [
            {"source": name, "start_seconds": start, "end_seconds": end}
            for name, start, end in SECTIONS
        ],
        "target_rms_dbfs": db(target_rms),
        "current_gain_db": db(current_gain),
        "candidate_gain_db": db(candidate_gain),
        "current_peak_dbfs": db(float(np.max(np.abs(current)))),
        "candidate_peak_dbfs": db(float(np.max(np.abs(candidate)))),
    }
    report_path = args.output_directory / "listening-set.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for path in (current_path, candidate_path, report_path):
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
