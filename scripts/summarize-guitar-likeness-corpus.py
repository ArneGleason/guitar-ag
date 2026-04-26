#!/usr/bin/env python3
"""Validate and summarize the Guitar AG guitar-likeness corpus manifest."""

from __future__ import annotations

import argparse
import csv
import json
import re
import sys
import wave
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import Any


DEFAULT_MANIFEST = Path("analysis/guitar-likeness-corpus.json")
MEAN_PATTERN = re.compile(r"^- `([^`]+)`: ([+-]?(?:\d+(?:\.\d*)?|\.\d+))$")


@dataclass
class WavInfo:
    sample_rate: int
    channels: int
    sample_width_bytes: int
    frames: int

    @property
    def duration_seconds(self) -> float:
        return self.frames / self.sample_rate if self.sample_rate else 0.0


def resolve_path(value: str | None) -> Path | None:
    if not value:
        return None
    return Path(value).expanduser()


def read_wav_info(path: Path) -> WavInfo:
    with wave.open(str(path), "rb") as handle:
        return WavInfo(sample_rate=handle.getframerate(),
                       channels=handle.getnchannels(),
                       sample_width_bytes=handle.getsampwidth(),
                       frames=handle.getnframes())


def count_note_rows(path: Path) -> int:
    with path.open(newline="") as handle:
        return sum(1 for _ in csv.DictReader(handle))


def parse_perceptual_means(path: Path) -> dict[str, float]:
    means: dict[str, float] = {}
    in_means = False

    for line in path.read_text().splitlines():
        if line.strip() == "## Means":
            in_means = True
            continue

        if in_means and line.startswith("## "):
            break

        if in_means:
            match = MEAN_PATTERN.match(line.strip())
            if match:
                means[match.group(1)] = float(match.group(2))

    return means


def load_manifest(path: Path) -> dict[str, Any]:
    with path.open() as handle:
        manifest = json.load(handle)

    if not isinstance(manifest, dict):
        raise ValueError("manifest root must be an object")

    entries = manifest.get("entries")
    if not isinstance(entries, list):
        raise ValueError("manifest must contain an entries list")

    return manifest


def summarize_entry(entry: dict[str, Any], warnings: list[str]) -> dict[str, str]:
    entry_id = str(entry.get("id", ""))
    bucket = str(entry.get("bucket", ""))
    wav_path = resolve_path(entry.get("wav"))
    notes_path = resolve_path(entry.get("notes_csv"))
    exists = wav_path.exists() if wav_path else False
    duration = ""
    format_summary = ""
    note_count = ""
    full_distance = ""
    flatness = ""
    flux = ""

    if wav_path is None:
        warnings.append(f"{entry_id}: missing wav field")
    elif exists:
        try:
            info = read_wav_info(wav_path)
            duration = f"{info.duration_seconds:.2f}s"
            format_summary = f"{info.sample_rate} Hz, {info.channels} ch, {info.sample_width_bytes * 8}-bit"
        except wave.Error as error:
            warnings.append(f"{entry_id}: cannot read WAV {wav_path}: {error}")
    else:
        warnings.append(f"{entry_id}: WAV not found: {wav_path}")

    if notes_path:
        if notes_path.exists():
            try:
                note_count = str(count_note_rows(notes_path))
            except (OSError, csv.Error) as error:
                warnings.append(f"{entry_id}: cannot read notes CSV {notes_path}: {error}")
        else:
            warnings.append(f"{entry_id}: notes CSV not found: {notes_path}")

    analyses = entry.get("analyses", {})
    if isinstance(analyses, dict):
        summary_path = resolve_path(analyses.get("perceptual_summary"))
        if summary_path:
            if summary_path.exists():
                means = parse_perceptual_means(summary_path)
                if "full_log_spectral_distance_db" in means:
                    full_distance = f"{means['full_log_spectral_distance_db']:.2f}"
                if "flatness_ratio_render_to_reference" in means:
                    flatness = f"{means['flatness_ratio_render_to_reference']:.2f}"
                if "spectral_flux_ratio_render_to_reference" in means:
                    flux = f"{means['spectral_flux_ratio_render_to_reference']:.2f}"
            else:
                warnings.append(f"{entry_id}: perceptual summary not found: {summary_path}")

    return {
        "id": entry_id,
        "bucket": bucket,
        "exists": "yes" if exists else "no",
        "duration": duration,
        "format": format_summary,
        "notes": note_count,
        "full_db": full_distance,
        "flatness": flatness,
        "flux": flux,
    }


def print_table(rows: list[dict[str, str]]) -> None:
    columns = [
        ("id", "ID"),
        ("bucket", "Bucket"),
        ("exists", "WAV"),
        ("duration", "Duration"),
        ("format", "Format"),
        ("notes", "Notes"),
        ("full_db", "Full dB"),
        ("flatness", "Flatness"),
        ("flux", "Flux"),
    ]
    widths = {
        key: max(len(header), *(len(row.get(key, "")) for row in rows))
        for key, header in columns
    }
    header = "  ".join(header.ljust(widths[key]) for key, header in columns)
    ruler = "  ".join("-" * widths[key] for key, _ in columns)
    print(header)
    print(ruler)

    for row in rows:
        print("  ".join(row.get(key, "").ljust(widths[key]) for key, _ in columns))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest",
                        type=Path,
                        default=DEFAULT_MANIFEST,
                        help="Path to guitar-likeness corpus manifest.")
    parser.add_argument("--strict",
                        action="store_true",
                        help="Return a non-zero exit code when files are missing or unreadable.")
    args = parser.parse_args()

    manifest_path = args.manifest.expanduser()
    manifest = load_manifest(manifest_path)
    entries = manifest["entries"]
    warnings: list[str] = []
    rows = [summarize_entry(entry, warnings) for entry in entries]
    bucket_counts = Counter(row["bucket"] for row in rows)

    print("# Guitar-Likeness Corpus")
    print()
    print(f"Manifest: {manifest_path}")
    print(f"Schema: {manifest.get('schema_version', 'unknown')}")
    print(f"Entries: {len(rows)}")
    print()
    print("Buckets:")
    for bucket, count in sorted(bucket_counts.items()):
        print(f"- {bucket}: {count}")
    print()
    print_table(rows)

    if warnings:
        print()
        print("Warnings:")
        for warning in warnings:
            print(f"- {warning}")

    return 1 if args.strict and warnings else 0


if __name__ == "__main__":
    raise SystemExit(main())
