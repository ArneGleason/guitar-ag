#!/usr/bin/env python3
"""Create a compact high-E reference/control/candidate listening set."""

from __future__ import annotations

import argparse
import importlib.util
import json
import math
import sys
from pathlib import Path

import numpy as np


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load helper module: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def mono(samples: np.ndarray) -> np.ndarray:
    return np.mean(samples, axis=1, keepdims=True)


def excerpt(samples: np.ndarray, sample_rate: int, start_seconds: float, duration_seconds: float):
    start = max(0, round(start_seconds * sample_rate))
    length = round(duration_seconds * sample_rate)
    result = np.zeros((length, 1), dtype=np.float64)
    available = mono(samples)[start : start + length]
    result[: len(available)] = available
    fade = min(round(0.008 * sample_rate), length // 2)
    if fade:
        ramp = np.linspace(0.0, 1.0, fade, endpoint=True)[:, None]
        result[:fade] *= ramp
        result[-fade:] *= ramp[::-1]
    return result


def build_montage(
    analyzer,
    helpers,
    paths: dict[str, Path],
    independent_distance: float = 0.40,
    alternate_distance: float = 0.20,
) -> tuple[int, np.ndarray, dict[str, object]]:
    loaded = {}
    onsets = {}
    for direction, path in paths.items():
        rate, samples = helpers.read_pcm24(path)
        if loaded and rate != next(iter(loaded.values()))[0]:
            raise ValueError("source sample rates do not match")
        loaded[direction] = (rate, samples)
        audio = analyzer.read_audio(path)
        expected = 12 if direction == "alternate" else 6
        distance = alternate_distance if direction == "alternate" else independent_distance
        onsets[direction] = analyzer.select_onsets(audio, expected, distance)

    sample_rate = loaded["down"][0]
    sections = (
        ("independent down", "down", onsets["down"][2] - 0.05, 1.25),
        ("independent up", "up", onsets["up"][2] - 0.05, 1.25),
        ("final downstroke decay", "down", onsets["down"][-1] - 0.05, 3.50),
        ("continuous alternate", "alternate", onsets["alternate"][0] - 0.05, 7.40),
    )
    gap = np.zeros((round(0.35 * sample_rate), 1), dtype=np.float64)
    parts = []
    section_report = []
    for label, direction, start, duration in sections:
        parts.append(excerpt(loaded[direction][1], sample_rate, start, duration))
        parts.append(gap)
        section_report.append(
            {"label": label, "direction": direction, "start_seconds": start, "duration_seconds": duration}
        )
    return sample_rate, np.concatenate(parts[:-1], axis=0), {"sections": section_report}


def normalize(tracks: dict[str, np.ndarray], target_dbfs: float):
    target = 10.0 ** (target_dbfs / 20.0)
    gains = {}
    for label, samples in tracks.items():
        rms = math.sqrt(float(np.mean(samples * samples)))
        peak = float(np.max(np.abs(samples)))
        gain = min(target / max(rms, 1.0e-12), 0.891 / max(peak, 1.0e-12))
        tracks[label] = samples * gain
        gains[label] = 20.0 * math.log10(max(gain, 1.0e-12))
    return gains


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture-root", type=Path, required=True)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument(
        "--analyzer",
        type=Path,
        default=Path(__file__).with_name("analyze-low-e-model-reference.py"),
    )
    parser.add_argument(
        "--audio-helpers",
        type=Path,
        default=Path(__file__).with_name("create-low-e-candidate-listening-set.py"),
    )
    args = parser.parse_args()
    analyzer = load_module("guitar_ag_endpoint_listening_analyzer", args.analyzer.resolve())
    helpers = load_module("guitar_ag_endpoint_audio_helpers", args.audio_helpers.resolve())

    reference_sessions = args.capture_root / "sessions"
    sources = {
        "reference": {
            direction: reference_sessions / f"high-e-eval-ringing-{direction}" / "take-002.wav"
            for direction in ("down", "up", "alternate")
        },
        "production": {
            direction: args.render_directory / f"production-high-e-{direction}.wav"
            for direction in ("down", "up", "alternate")
        },
        "uniform": {
            direction: args.render_directory / f"accepted-high-e-{direction}.wav"
            for direction in ("down", "up", "alternate")
        },
        "endpoint": {
            direction: args.render_directory / f"endpoint-high-e-{direction}.wav"
            for direction in ("down", "up", "alternate")
        },
    }

    tracks = {}
    source_reports = {}
    sample_rate = 0
    for label, paths in sources.items():
        rate, track, report = build_montage(analyzer, helpers, paths)
        if sample_rate and rate != sample_rate:
            raise ValueError("montage sample rates do not match")
        sample_rate = rate
        tracks[label] = track
        source_reports[label] = report
    shapes = {track.shape for track in tracks.values()}
    if len(shapes) != 1:
        raise ValueError("montages do not align")
    gains = normalize(tracks, -20.0)

    args.output_directory.mkdir(parents=True, exist_ok=True)
    outputs = {
        "01-reference-high-e.wav": tracks["reference"],
        "02-production-eg089-high-e.wav": tracks["production"],
        "03-uniform-low-e-candidate-high-e.wav": tracks["uniform"],
        "04-endpoint-candidate-high-e.wav": tracks["endpoint"],
    }
    pause = np.zeros((round(0.70 * sample_rate), 1), dtype=np.float64)
    outputs.update(
        {
            "05-reference-then-endpoint.wav": np.concatenate((tracks["reference"], pause, tracks["endpoint"])),
            "06-endpoint-then-reference.wav": np.concatenate((tracks["endpoint"], pause, tracks["reference"])),
            "07-production-then-endpoint.wav": np.concatenate((tracks["production"], pause, tracks["endpoint"])),
            "08-endpoint-then-production.wav": np.concatenate((tracks["endpoint"], pause, tracks["production"])),
        }
    )
    for filename, samples in outputs.items():
        helpers.write_pcm24(args.output_directory / filename, sample_rate, samples)

    report = {
        "sample_rate": sample_rate,
        "channels": 1,
        "duration_seconds_each": len(tracks["reference"]) / sample_rate,
        "target_rms_dbfs": -20.0,
        "gain_db": gains,
        "source_sections": source_reports,
    }
    report_path = args.output_directory / "listening-set.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for path in [*(args.output_directory / name for name in outputs), report_path]:
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
