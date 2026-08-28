#!/usr/bin/env python3
"""Summarize high-E pluck/decay candidate renders against endpoint evidence."""

from __future__ import annotations

import argparse
import csv
import importlib.util
import json
import re
import sys
from pathlib import Path

import numpy as np


GRID_PATTERN = re.compile(r"grid-(?P<label>.+)-high-e-(?P<direction>down|up|alternate)\.wav$")


def load_analyzer(path: Path):
    spec = importlib.util.spec_from_file_location("guitar_ag_grid_analyzer", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load analyzer: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def median(rows: list[dict[str, object]], key: str) -> float:
    return float(np.median([float(row[key]) for row in rows]))


def summarized_row(label: str, direction: str, stroke: str, rows: list[dict[str, object]]):
    selected = [
        row
        for row in rows
        if row["direction_group"] == direction and row["stroke_direction"] == stroke
    ]
    if not selected:
        raise RuntimeError(f"missing {label} {direction}/{stroke} events")
    return {
        "label": label,
        "direction_group": direction,
        "stroke_direction": stroke,
        "events": len(selected),
        "attack_centroid_hz": median(selected, "attack_centroid_hz"),
        "attack_below_500_fraction": median(selected, "attack_band_50_150")
        + median(selected, "attack_band_150_500"),
        "attack_above_1500_fraction": median(selected, "attack_band_1500_4000")
        + median(selected, "attack_band_4000_10000"),
        "early_centroid_hz": median(selected, "early_centroid_hz"),
        "early_to_attack_peak_db": median(selected, "early_to_attack_peak_db"),
        "decay_db_per_second": median(
            [row for row in selected if float(row["decay_db_per_second"]) != 0.0],
            "decay_db_per_second",
        )
        if any(float(row["decay_db_per_second"]) != 0.0 for row in selected)
        else 0.0,
    }


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

    events_by_label: dict[str, list[dict[str, object]]] = {}
    for path in sorted(args.render_directory.glob("grid-*-high-e-*.wav")):
        match = GRID_PATTERN.match(path.name)
        if not match:
            continue
        label = match.group("label")
        direction = match.group("direction")
        expected = 12 if direction == "alternate" else 6
        minimum_distance = 0.25 if direction == "alternate" else 0.75
        audio = analyzer.read_audio(path)
        onsets = analyzer.select_onsets(audio, expected, minimum_distance)
        events_by_label.setdefault(label, []).extend(
            analyzer.event_rows(
                audio,
                label,
                "ringing",
                direction,
                path.name,
                onsets,
                fundamental_target_hz=329.6276,
            )
        )

    rows = []
    for label, events in sorted(events_by_label.items()):
        rows.extend(
            (
                summarized_row(label, "down", "down", events),
                summarized_row(label, "up", "up", events),
                summarized_row(label, "alternate", "down", events),
                summarized_row(label, "alternate", "up", events),
            )
        )

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8")
    csv_path = args.output.with_suffix(".csv")
    with csv_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0]))
        writer.writeheader()
        writer.writerows(rows)
    for row in rows:
        if row["direction_group"] in ("down", "up"):
            print(
                f"{row['label']} {row['direction_group']}: "
                f"centroid={row['attack_centroid_hz']:.0f} Hz "
                f"<500={100.0 * row['attack_below_500_fraction']:.1f}% "
                f">1500={100.0 * row['attack_above_1500_fraction']:.1f}% "
                f"decay={row['decay_db_per_second']:.2f} dB/s"
            )
    print(args.output)
    print(csv_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
