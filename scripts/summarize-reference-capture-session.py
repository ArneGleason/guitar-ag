#!/usr/bin/env python3
"""Validate and summarize a Guitar AG reference-capture session."""

from __future__ import annotations

import argparse
import json
import sys
import wave
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Validate a Guitar AG Reference Capture session.json file."
    )
    parser.add_argument("manifest", type=Path)
    parser.add_argument(
        "--approved-paths",
        action="store_true",
        help="Print only absolute paths of valid approved WAV files.",
    )
    args = parser.parse_args()

    manifest_path = args.manifest.expanduser().resolve()
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 2

    errors: list[str] = []
    if manifest.get("schema_version") != 1:
        errors.append("schema_version must be 1")
    if not manifest.get("request_id"):
        errors.append("request_id is missing")
    takes = manifest.get("takes")
    if not isinstance(takes, list):
        errors.append("takes must be an array")
        takes = []

    summaries: list[dict[str, object]] = []
    for index, take in enumerate(takes):
        if not isinstance(take, dict):
            errors.append(f"takes[{index}] is not an object")
            continue

        status = take.get("status")
        if status not in {"candidate", "approved", "rejected"}:
            errors.append(f"takes[{index}] has invalid status {status!r}")

        relative_file = take.get("file")
        if not isinstance(relative_file, str) or not relative_file:
            errors.append(f"takes[{index}] has no file")
            continue

        session_directory = manifest_path.parent.resolve()
        wav_path = (session_directory / relative_file).resolve()
        if not wav_path.is_relative_to(session_directory):
            errors.append(f"takes[{index}] escapes the session directory")
            continue
        if not wav_path.is_file():
            errors.append(f"missing WAV: {wav_path}")
            continue

        try:
            with wave.open(str(wav_path), "rb") as wav_file:
                channels = wav_file.getnchannels()
                sample_width = wav_file.getsampwidth()
                sample_rate = wav_file.getframerate()
                frames = wav_file.getnframes()
        except (OSError, wave.Error) as error:
            errors.append(f"invalid WAV {wav_path}: {error}")
            continue

        if channels != 1:
            errors.append(f"{wav_path.name}: expected mono, found {channels} channels")
        if sample_width != 3:
            errors.append(
                f"{wav_path.name}: expected 24-bit PCM, found {sample_width * 8}-bit"
            )

        summaries.append(
            {
                "number": take.get("take_number"),
                "status": status,
                "path": wav_path.resolve(),
                "sample_rate": sample_rate,
                "duration": frames / sample_rate if sample_rate else 0.0,
                "peak_dbfs": take.get("peak_dbfs"),
            }
        )

    if errors:
        for error in errors:
            print(f"error: {error}", file=sys.stderr)
        return 1

    approved = [item for item in summaries if item["status"] == "approved"]
    if args.approved_paths:
        for item in approved:
            print(item["path"])
        return 0

    print(f"request: {manifest.get('request_id')}")
    print(f"takes: {len(summaries)}")
    print(f"approved: {len(approved)}")
    print(f"candidates: {sum(item['status'] == 'candidate' for item in summaries)}")
    print(f"rejected: {sum(item['status'] == 'rejected' for item in summaries)}")
    for item in summaries:
        peak = item["peak_dbfs"]
        peak_text = f"{peak:.1f} dBFS" if isinstance(peak, (int, float)) else "unknown peak"
        print(
            f"take {item['number']}: {item['status']}, {item['sample_rate']} Hz, "
            f"{item['duration']:.2f} s, {peak_text}, {item['path']}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
