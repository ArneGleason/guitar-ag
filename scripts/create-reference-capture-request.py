#!/usr/bin/env python3
"""Create a focused Guitar AG reference-capture request JSON."""

from __future__ import annotations

import argparse
import ctypes
import json
import os
import re
from datetime import datetime
from pathlib import Path


def slugify(value: str) -> str:
    value = re.sub(r"[^a-z0-9]+", "-", value.lower()).strip("-")
    return value or "capture"


def default_documents_directory() -> Path:
    if os.name == "nt":
        buffer = ctypes.create_unicode_buffer(260)
        if ctypes.windll.shell32.SHGetFolderPathW(None, 5, None, 0, buffer) == 0:
            return Path(buffer.value)
    return Path.home() / "Documents"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create a request for Guitar AG Reference Capture."
    )
    parser.add_argument("--title", required=True)
    parser.add_argument("--instructions", required=True)
    parser.add_argument("--technique", default="pick_contact")
    parser.add_argument("--string", dest="string_name", default="")
    parser.add_argument("--direction", choices=("", "down", "up"), default="")
    parser.add_argument(
        "--dynamics", choices=("", "light", "medium", "hard"), default=""
    )
    parser.add_argument(
        "--pick-depth", choices=("", "shallow", "deep"), default=""
    )
    parser.add_argument("--pick", default="")
    parser.add_argument(
        "--muting",
        choices=("", "ringing", "hand-damped", "foam-damped"),
        default="",
    )
    parser.add_argument("--guitar", default="")
    parser.add_argument("--pickup", default="")
    parser.add_argument("--comparison-group", default="")
    parser.add_argument("--takes", type=int, default=8)
    parser.add_argument(
        "--capture-root",
        type=Path,
        default=default_documents_directory() / "Guitar AG Reference Captures",
    )
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()

    if not 1 <= args.takes <= 100:
        parser.error("--takes must be between 1 and 100")

    timestamp = datetime.now().astimezone()
    request_id = f"{timestamp:%Y%m%d-%H%M%S}-{slugify(args.title)}"
    session_directory = args.capture_root / "sessions" / request_id
    output = args.output or args.capture_root / "requests" / f"{request_id}.json"

    context = {
        "technique": args.technique,
        "string": args.string_name,
        "direction": args.direction,
        "dynamics": args.dynamics,
        "pick_depth": args.pick_depth,
        "pick": args.pick,
        "muting": args.muting,
        "guitar": args.guitar,
        "pickup": args.pickup,
        "comparison_group": args.comparison_group,
    }

    request = {
        "schema_version": 1,
        "request_id": request_id,
        "created_at": timestamp.isoformat(timespec="seconds"),
        "title": args.title,
        "instructions": args.instructions,
        "requested_take_count": args.takes,
        "session_directory": str(session_directory.resolve()),
        "context": context,
        "capture_requirements": {
            "signal": "clean electric-guitar DI",
            "format": "mono 24-bit WAV",
            "preferred_sample_rates_hz": [48000, 96000],
            "target_peak_dbfs_at_most": -12.0,
            "processing": "none",
        },
    }

    output = output.expanduser().resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(request, indent=2) + "\n", encoding="utf-8")
    print(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
