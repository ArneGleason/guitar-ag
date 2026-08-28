#!/usr/bin/env python3
"""Create the Plan 0109 high-E transient-preserving listening set."""

from __future__ import annotations

import argparse
import importlib.util
import json
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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture-root", type=Path, required=True)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument(
        "--montage-helpers",
        type=Path,
        default=Path(__file__).with_name("create-endpoint-candidate-listening-set.py"),
    )
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
    montage = load_module("guitar_ag_transient_montage_helpers", args.montage_helpers.resolve())
    analyzer = load_module("guitar_ag_transient_reference_analyzer", args.analyzer.resolve())
    audio = load_module("guitar_ag_transient_audio_helpers", args.audio_helpers.resolve())

    sessions = args.capture_root / "sessions"
    sources = {
        "reference": {
            direction: sessions / f"high-e-eval-ringing-{direction}" / "take-002.wav"
            for direction in ("down", "up", "alternate")
        },
        **{
            label: {
                direction: args.render_directory / f"{prefix}-high-e-{direction}.wav"
                for direction in ("down", "up", "alternate")
            }
            for label, prefix in (
                ("production", "production"),
                ("rejected", "endpoint"),
                ("transient", "transient"),
            )
        },
    }

    tracks = {}
    reports = {}
    sample_rate = 0
    for label, paths in sources.items():
        rate, track, report = montage.build_montage(analyzer, audio, paths)
        if sample_rate and rate != sample_rate:
            raise ValueError("montage sample rates do not match")
        sample_rate = rate
        tracks[label] = track
        reports[label] = report
    if len({track.shape for track in tracks.values()}) != 1:
        raise ValueError("montages do not align")
    gains = montage.normalize(tracks, -20.0)

    pause = np.zeros((round(0.70 * sample_rate), 1), dtype=np.float64)
    outputs = {
        "01-reference-high-e.wav": tracks["reference"],
        "02-production-eg089-high-e.wav": tracks["production"],
        "03-rejected-dark-endpoint-high-e.wav": tracks["rejected"],
        "04-transient-preserving-high-e.wav": tracks["transient"],
        "05-reference-then-transient.wav": np.concatenate((tracks["reference"], pause, tracks["transient"])),
        "06-transient-then-reference.wav": np.concatenate((tracks["transient"], pause, tracks["reference"])),
        "07-production-then-transient.wav": np.concatenate((tracks["production"], pause, tracks["transient"])),
        "08-transient-then-production.wav": np.concatenate((tracks["transient"], pause, tracks["production"])),
        "09-dark-endpoint-then-transient.wav": np.concatenate((tracks["rejected"], pause, tracks["transient"])),
        "10-transient-then-dark-endpoint.wav": np.concatenate((tracks["transient"], pause, tracks["rejected"])),
    }
    args.output_directory.mkdir(parents=True, exist_ok=True)
    for filename, samples in outputs.items():
        audio.write_pcm24(args.output_directory / filename, sample_rate, samples)

    report = {
        "sample_rate": sample_rate,
        "channels": 1,
        "duration_seconds_each": len(tracks["reference"]) / sample_rate,
        "target_rms_dbfs": -20.0,
        "gain_db": gains,
        "source_sections": reports,
    }
    report_path = args.output_directory / "listening-set.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for path in [*(args.output_directory / name for name in outputs), report_path]:
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
