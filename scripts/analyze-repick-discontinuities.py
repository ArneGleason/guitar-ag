#!/usr/bin/env python3
"""Measure derivative spikes around Plan 0106 alternate-note restarts."""

from __future__ import annotations

import argparse
import importlib.util
import json
import math
import sys
from pathlib import Path

import numpy as np


def load_analyzer(path: Path):
    spec = importlib.util.spec_from_file_location("low_e_reference_analysis", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"could not load analyzer: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def db(value: float) -> float:
    return 20.0 * math.log10(max(float(value), 1.0e-12))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument(
        "--analyzer",
        type=Path,
        default=Path(__file__).with_name("analyze-low-e-model-reference.py"),
    )
    args = parser.parse_args()
    analyzer = load_analyzer(args.analyzer.resolve())
    note_on_seconds = [1.0 + index * 0.42 for index in range(1, 12)]
    results = []

    for path in sorted(args.render_directory.glob("*.wav")):
        if path.name.startswith("neutral"):
            continue
        audio = analyzer.read_audio(path)
        derivative = np.abs(np.diff(audio.samples))
        spike_ratios = []
        boundary_jumps = []
        for onset in note_on_seconds:
            sample = round(onset * audio.sample_rate)
            post = derivative[sample : sample + round(0.005 * audio.sample_rate)]
            baseline = derivative[sample - round(0.050 * audio.sample_rate) : sample]
            baseline_p99 = float(np.percentile(baseline, 99))
            spike_ratios.append(db(float(np.max(post)) / max(baseline_p99, 1.0e-12)))
            boundary_jumps.append(db(derivative[sample - 1]))
        row = {
            "file": path.name,
            "events": len(note_on_seconds),
            "boundary_jump_dbfs_median": float(np.median(boundary_jumps)),
            "post_onset_spike_over_preceding_p99_db_median": float(np.median(spike_ratios)),
            "post_onset_spike_over_preceding_p99_db_p90": float(np.percentile(spike_ratios, 90)),
            "post_onset_spike_over_preceding_p99_db_max": float(np.max(spike_ratios)),
        }
        results.append(row)
        print(
            f"{path.name}: boundary={row['boundary_jump_dbfs_median']:.1f} dBFS, "
            f"spike median={row['post_onset_spike_over_preceding_p99_db_median']:.1f} dB, "
            f"p90={row['post_onset_spike_over_preceding_p99_db_p90']:.1f} dB, "
            f"max={row['post_onset_spike_over_preceding_p99_db_max']:.1f} dB"
        )

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(results, indent=2) + "\n", encoding="utf-8")
    print(args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
