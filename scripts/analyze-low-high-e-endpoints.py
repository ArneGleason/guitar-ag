#!/usr/bin/env python3
"""Measure low/high-E references and matching endpoint model render sets."""

from __future__ import annotations

import argparse
import csv
import importlib.util
import json
import sys
from pathlib import Path


def load_analyzer(path: Path):
    spec = importlib.util.spec_from_file_location("guitar_ag_reference_analyzer", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load analyzer: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


REFERENCE_SPECS = (
    ("low-e", "low-e-eval-ringing-down", "down", 6, 0.75, 82.4069),
    ("low-e", "low-e-eval-ringing-up", "up", 6, 0.75, 82.4069),
    ("low-e", "low-e-eval-ringing-alternate", "alternate", 12, 0.25, 82.4069),
    ("high-e", "high-e-eval-ringing-down", "down", 6, 0.40, 329.6276),
    ("high-e", "high-e-eval-ringing-up", "up", 6, 0.40, 329.6276),
    ("high-e", "high-e-eval-ringing-alternate", "alternate", 12, 0.20, 329.6276),
)


def parse_model_set(value: str) -> tuple[str, str]:
    if "=" not in value:
        raise argparse.ArgumentTypeError("model sets must use LABEL=PREFIX")
    label, prefix = value.split("=", 1)
    if not label or not prefix:
        raise argparse.ArgumentTypeError("model sets must use non-empty LABEL=PREFIX")
    return label, prefix


def validation_row(analyzer, audio, source: str, endpoint: str, request: str, take: str, onsets):
    return {
        "source": source,
        "endpoint": endpoint,
        "request": request,
        "take": take,
        "sample_rate": audio.sample_rate,
        "channels": audio.channels,
        "bits": audio.sample_width * 8,
        "duration_seconds": len(audio.samples) / audio.sample_rate,
        "peak_dbfs": analyzer.db(abs(audio.samples).max()),
        "clip_samples": int((abs(audio.samples) >= 0.999).sum()),
        "longest_zero_run_samples": analyzer.longest_zero_run(audio.samples),
        "detected_onsets": len(onsets),
        "onset_seconds": " ".join(f"{value:.3f}" for value in onsets),
    }


def add_events(analyzer, events, audio, source, endpoint, direction, take, onsets, target_hz):
    rows = analyzer.event_rows(
        audio,
        source,
        "ringing",
        direction,
        take,
        onsets,
        fundamental_target_hz=target_hz,
    )
    for row in rows:
        row["endpoint"] = endpoint
    events.extend(rows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture-root", type=Path, required=True)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument(
        "--model-set",
        action="append",
        type=parse_model_set,
        default=[],
        help="Render set as LABEL=PREFIX; reads PREFIX-{low-e|high-e}-{down|up|alternate}.wav",
    )
    parser.add_argument(
        "--analyzer",
        type=Path,
        default=Path(__file__).with_name("analyze-low-e-model-reference.py"),
    )
    parser.add_argument("--plot-directory", type=Path)
    args = parser.parse_args()
    if not args.model_set:
        parser.error("at least one --model-set is required")

    analyzer = load_analyzer(args.analyzer.resolve())
    args.output_directory.mkdir(parents=True, exist_ok=True)
    validation = []
    events = []

    for endpoint, request_id, direction, expected, minimum_distance, target_hz in REFERENCE_SPECS:
        session_directory = args.capture_root / "sessions" / request_id
        manifest = json.loads((session_directory / "session.json").read_text(encoding="utf-8"))
        approved = [take for take in manifest["takes"] if take["status"] == "approved"]
        for take in approved:
            path = session_directory / take["file"]
            audio = analyzer.read_audio(path)
            onsets = analyzer.select_onsets(audio, expected, minimum_distance)
            validation.append(
                validation_row(analyzer, audio, f"reference-{endpoint}", endpoint, request_id, take["file"], onsets)
            )
            add_events(
                analyzer,
                events,
                audio,
                f"reference-{endpoint}",
                endpoint,
                direction,
                take["file"],
                onsets,
                target_hz,
            )
            if args.plot_directory:
                analyzer.plot_onsets(
                    audio,
                    onsets,
                    args.plot_directory / f"{request_id}-{take['file']}.png",
                    f"{request_id} / {take['file']}",
                )

    direction_specs = (("down", 6, 0.75), ("up", 6, 0.75), ("alternate", 12, 0.25))
    for label, prefix in args.model_set:
        for endpoint, target_hz in (("low-e", 82.4069), ("high-e", 329.6276)):
            source = f"{label}-{endpoint}"
            for direction, expected, minimum_distance in direction_specs:
                filename = f"{prefix}-{endpoint}-{direction}.wav"
                path = args.render_directory / filename
                audio = analyzer.read_audio(path)
                onsets = analyzer.select_onsets(audio, expected, minimum_distance)
                validation.append(validation_row(analyzer, audio, source, endpoint, filename, filename, onsets))
                add_events(
                    analyzer,
                    events,
                    audio,
                    source,
                    endpoint,
                    direction,
                    filename,
                    onsets,
                    target_hz,
                )

    summaries = analyzer.summarize(events)
    validation_path = args.output_directory / "validation.csv"
    events_path = args.output_directory / "events.csv"
    summary_path = args.output_directory / "summary.json"
    with validation_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(validation[0]))
        writer.writeheader()
        writer.writerows(validation)
    with events_path.open("w", newline="", encoding="utf-8") as handle:
        fieldnames = list(events[0])
        fieldnames.remove("endpoint")
        fieldnames.insert(fieldnames.index("condition") + 1, "endpoint")
        writer = csv.DictWriter(handle, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(events)
    summary_path.write_text(json.dumps(summaries, indent=2) + "\n", encoding="utf-8")

    for row in validation:
        print(
            f"{row['source']} {row['endpoint']} {row['take']}: "
            f"{row['detected_onsets']} onsets peak={row['peak_dbfs']:.2f} dBFS "
            f"clips={row['clip_samples']} zeros={row['longest_zero_run_samples']}"
        )
    print(validation_path)
    print(events_path)
    print(summary_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
