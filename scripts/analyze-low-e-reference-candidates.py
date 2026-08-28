#!/usr/bin/env python3
"""Compare Plan 0105 ringing-down candidates with Plan 0104 event evidence."""

from __future__ import annotations

import argparse
import csv
import importlib.util
import json
import sys
from pathlib import Path

import numpy as np


METRICS = (
    "attack_centroid_hz",
    "attack_band_50_150",
    "attack_band_150_500",
    "attack_band_500_1500",
    "attack_band_1500_4000",
    "attack_band_4000_10000",
    "early_to_attack_peak_db",
    "decay_db_per_second",
)


def load_analyzer(path: Path):
    spec = importlib.util.spec_from_file_location("low_e_reference_analysis", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"could not load analyzer: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def median_row(label: str, direction: str, rows: list[dict[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {"candidate": label, "direction": direction, "events": len(rows)}
    for metric in METRICS:
        values = np.asarray([float(row[metric]) for row in rows], dtype=np.float64)
        values = values[np.isfinite(values)]
        if metric == "decay_db_per_second":
            values = values[values != 0.0]
        result[metric] = float(np.median(values)) if len(values) else 0.0
    result["attack_below_500"] = float(result["attack_band_50_150"]) + float(
        result["attack_band_150_500"]
    )
    result["attack_above_1500"] = float(result["attack_band_1500_4000"]) + float(
        result["attack_band_4000_10000"]
    )
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference-events", type=Path, required=True)
    parser.add_argument("--candidate-directory", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument(
        "--analyzer",
        type=Path,
        default=Path(__file__).with_name("analyze-low-e-model-reference.py"),
    )
    args = parser.parse_args()

    with args.reference_events.open(newline="", encoding="utf-8") as handle:
        evidence = list(csv.DictReader(handle))
    reference_by_direction = {
        direction: [
            row
            for row in evidence
            if row["source"] == "reference"
            and row["condition"] == "ringing"
            and row["direction_group"] == direction
        ]
        for direction in ("down", "up", "alternate")
    }
    if any(not rows for rows in reference_by_direction.values()):
        raise RuntimeError("Plan 0104 ringing reference events are incomplete")

    analyzer = load_analyzer(args.analyzer.resolve())
    results = [
        median_row(f"reference-{direction}", direction, rows)
        for direction, rows in reference_by_direction.items()
    ]
    for path in sorted(args.candidate_directory.glob("*.wav")):
        direction = "alternate" if "alternate" in path.stem else "up" if path.stem.endswith("-up") else "down"
        expected = 12 if direction == "alternate" else 6
        minimum_distance = 0.25 if direction == "alternate" else 0.75
        audio = analyzer.read_audio(path)
        onsets = analyzer.select_onsets(audio, expected, minimum_distance)
        rows = analyzer.event_rows(audio, path.stem, "ringing", direction, path.name, onsets)
        results.append(median_row(path.stem, direction, rows))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(results, indent=2) + "\n", encoding="utf-8")
    for row in results:
        print(
            f"{row['candidate']}: centroid={row['attack_centroid_hz']:.1f} Hz, "
            f"<500 Hz={100.0 * row['attack_below_500']:.1f}%, "
            f">1.5 kHz={100.0 * row['attack_above_1500']:.1f}%, "
            f"body/peak={row['early_to_attack_peak_db']:.1f} dB, "
            f"decay={row['decay_db_per_second']:.2f} dB/s"
        )
    print(args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
