#!/usr/bin/env python3
"""Create the phased Guitar AG endpoint-evaluation capture inventory."""

from __future__ import annotations

import argparse
import ctypes
import json
import os
from datetime import datetime
from pathlib import Path


GUITAR_DESCRIPTION = "EVH Wolfgang Special"
PICKUP_DESCRIPTION = "neck humbucker; retain the established guitar volume/tone settings"
PICKING_LOCATION_DESCRIPTION = "halfway between the bridge and neck pickups"
STRING_SET_DESCRIPTION = "likely a .009-gauge electric-guitar set; exact brand and gauges unknown"
STRING_CONDITION_DESCRIPTION = (
    "installed for a long time and somewhat grimy from finger contact, but not heavily played or obviously fatigued"
)
PICK_DESCRIPTION = (
    "generic medium-gauge celluloid-style plastic pick; exact brand/material/thickness unknown; well worn and broken in"
)


def default_documents_directory() -> Path:
    if os.name == "nt":
        buffer = ctypes.create_unicode_buffer(260)
        if ctypes.windll.shell32.SHGetFolderPathW(None, 5, None, 0, buffer) == 0:
            return Path(buffer.value)
    return Path.home() / "Documents"


def request(
    request_id: str,
    title: str,
    phase_id: str,
    phase_title: str,
    why: str,
    instructions: str,
    *,
    technique: str,
    string_name: str = "",
    direction: str = "",
    dynamics: str = "",
    pick_depth: str = "",
    pick: str = "",
    muting: str = "",
    performance_pattern: str = "",
    stroke_count: int = 0,
    timing: str = "",
    takes: int = 2,
) -> dict[str, object]:
    return {
        "request_id": request_id,
        "title": title,
        "phase_id": phase_id,
        "phase_title": phase_title,
        "why": why,
        "instructions": instructions,
        "requested_take_count": takes,
        "context": {
            "technique": technique,
            "string": string_name,
            "direction": direction,
            "dynamics": dynamics,
            "pick_depth": pick_depth,
            "pick": pick,
            "muting": muting,
            "performance_pattern": performance_pattern,
            "stroke_count": stroke_count,
            "timing": timing,
            "guitar": GUITAR_DESCRIPTION,
            "pickup": PICKUP_DESCRIPTION,
            "picking_location": PICKING_LOCATION_DESCRIPTION,
            "string_set": STRING_SET_DESCRIPTION,
            "string_condition": STRING_CONDITION_DESCRIPTION,
            "setup_certainty": "descriptive player estimate; gauge, string brand, exact age, and pick composition are not confirmed",
            "comparison_group": phase_id,
        },
    }


def low_e_exercise_instructions(muting: str, pattern: str) -> str:
    common = (
        "Use open low E with one ordinary medium plastic pick, medium force, and shallow "
        "pick depth. Keep guitar, pickup, volume/tone controls, interface gain, picking "
        "location, and pick fixed for the entire low-E exercise. Leave about one second "
        "of untouched input before the first stroke. Natural timing is wanted; do not use "
        "a metronome. Long hand movement before starting and extra silence are fine. "
    )
    notes = (
        "Approve one representative batch. Record a second batch only if the first has a "
        "duff, handling noise, or obviously inconsistent force. Note the exact guitar, "
        "pickup setting, control settings, and pick brand/material/thickness in the "
        "selected-take notes."
    )

    if muting == "ringing" and pattern in ("independent_down", "independent_up"):
        direction = "downstroke" if pattern == "independent_down" else "upstroke"
        return common + (
            f"Play four independent {direction}s. After each of the first three strokes, "
            "let the string ring for 1.5 to 2 seconds, deliberately stop it, then leave "
            "about half a second of quiet reset time. After the fourth stroke, let the "
            "string ring naturally for 6 to 8 seconds before stopping it. "
        ) + notes

    if muting == "ringing" and pattern == "alternate_down_first":
        return common + (
            "Let the string keep ringing while you play 12 continuous alternate-picked "
            "strokes at a comfortable natural tempo, starting with down: D U D U D U D U "
            "D U D U. Do not stop the string between strokes. After the final upstroke, "
            "let the string ring for 4 to 6 seconds before stopping it. "
        ) + notes

    damping = (
        "Lightly touch the string at stable non-harmonic fretting-hand positions, away "
        "from the pick, so sustained pitch is suppressed without changing the picking "
        "location. "
    )
    if muting == "hand-damped" and pattern in ("independent_down", "independent_up"):
        direction = "downstrokes" if pattern == "independent_down" else "upstrokes"
        return common + damping + (
            f"Play six {direction}, allowing each short damped event to finish and leaving "
            "roughly 0.6 to 0.8 seconds between strokes. Comfortable consistency matters "
            "more than exact spacing. "
        ) + notes

    if muting == "hand-damped" and pattern == "alternate_down_first":
        return common + damping + (
            "Play 12 continuous alternate-picked strokes at a comfortable natural tempo, "
            "starting with down: D U D U D U D U D U D U. "
        ) + notes

    raise ValueError(f"unsupported low-E exercise: {muting}/{pattern}")


def high_e_exercise_instructions(pattern: str) -> str:
    common = (
        "Use open high E on the same EVH Wolfgang Special, neck humbucker, guitar controls, "
        "Focusrite gain, medium force, shallow pick depth, and picking location used for the "
        "accepted low-E set. Use the same worn medium celluloid-style pick. Leave about one "
        "second of untouched input before the first stroke. Natural timing is wanted; do not "
        "use a metronome. Long hand movement before starting and extra silence are fine. "
    )
    notes = (
        "Approve one representative batch. Record a second batch only if the first has a duff, "
        "handling noise, or obviously inconsistent force. Do not change gain merely to make high "
        "E look as large as low E; if a gain change is unavoidable, document it in take notes."
    )

    if pattern in ("independent_down", "independent_up"):
        direction = "downstrokes" if pattern == "independent_down" else "upstrokes"
        return common + (
            f"Play six independent {direction}. After each of the first five strokes, let the "
            "string ring for 1.5 to 2 seconds, deliberately stop it, then leave about half a "
            "second of quiet reset time. After the sixth stroke, let the string ring naturally "
            "for 5 to 8 seconds before stopping it. "
        ) + notes

    if pattern == "alternate_down_first":
        return common + (
            "Let the string keep ringing while you play 12 continuous alternate-picked strokes "
            "at a comfortable natural tempo, starting with down: D U D U D U D U D U D U. Do "
            "not stop the string between strokes. After the final upstroke, let it ring for 4 to "
            "6 seconds before stopping it. "
        ) + notes

    raise ValueError(f"unsupported high-E exercise: {pattern}")


def build_items() -> list[dict[str, object]]:
    items: list[dict[str, object]] = []

    items.append(
        request(
            "setup-01-input-noise-floor",
            "Input noise floor",
            "phase-0-setup",
            "Phase 0 — Setup reference",
            "Measures the Focusrite/guitar electrical floor before interpreting quiet contact energy.",
            "With the guitar connected and all capture settings fixed, do not touch or play it. "
            "Record 10 seconds of untouched input per take. Keep one clean take with no bumps, "
            "cable movement, computer alerts, or room handling noise.",
            technique="input_noise_floor",
            takes=2,
        )
    )

    phase_1 = "phase-1-low-e-model-evaluation"
    phase_1_title = "Phase 1 — Low-E model-evaluation exercise"
    phase_1_why = (
        "Measures independent down/up attacks, natural alternate repicking, and the "
        "ringing-versus-hand-damped contrast before changing the physical model."
    )
    baseline_pick = PICK_DESCRIPTION
    exercise_specs = [
        (
            "low-e-eval-ringing-down",
            "Open Low E - ringing independent downstrokes",
            "ringing",
            "down",
            "independent_down",
            4,
            "1.5-2 s ring, deliberate stop, 0.5 s reset; final stroke rings 6-8 s",
        ),
        (
            "low-e-eval-ringing-up",
            "Open Low E - ringing independent upstrokes",
            "ringing",
            "up",
            "independent_up",
            4,
            "1.5-2 s ring, deliberate stop, 0.5 s reset; final stroke rings 6-8 s",
        ),
        (
            "low-e-eval-ringing-alternate",
            "Open Low E - ringing alternate repicking",
            "ringing",
            "alternate_down_first",
            "alternate_down_first",
            12,
            "comfortable natural tempo; continuous D U sequence; final ring 4-6 s",
        ),
        (
            "low-e-eval-hand-damped-down",
            "Open Low E - hand-damped downstrokes",
            "hand-damped",
            "down",
            "independent_down",
            6,
            "roughly 0.6-0.8 s between strokes; natural timing",
        ),
        (
            "low-e-eval-hand-damped-up",
            "Open Low E - hand-damped upstrokes",
            "hand-damped",
            "up",
            "independent_up",
            6,
            "roughly 0.6-0.8 s between strokes; natural timing",
        ),
        (
            "low-e-eval-hand-damped-alternate",
            "Open Low E - hand-damped alternate picking",
            "hand-damped",
            "alternate_down_first",
            "alternate_down_first",
            12,
            "comfortable natural tempo; continuous D U sequence",
        ),
    ]
    for (
        request_id,
        title,
        muting,
        direction,
        pattern,
        stroke_count,
        timing,
    ) in exercise_specs:
        items.append(
            request(
                request_id,
                title,
                phase_1,
                phase_1_title,
                phase_1_why,
                low_e_exercise_instructions(muting, pattern),
                technique=(
                    "pick_contact"
                    if pattern != "alternate_down_first"
                    else "alternate_repicking"
                ),
                string_name="open low E",
                direction=direction,
                dynamics="medium",
                pick_depth="shallow",
                pick=baseline_pick,
                muting=muting,
                performance_pattern=pattern,
                stroke_count=stroke_count,
                timing=timing,
                takes=2,
            )
        )

    phase_2 = "phase-2-high-e-endpoint-evaluation"
    phase_2_title = "Phase 2 — High-E endpoint exercise"
    phase_2_why = (
        "Measures the opposite string/register endpoint before deciding how low-E geometry, "
        "decay, and attack behavior should scale across the instrument."
    )
    high_e_specs = [
        (
            "high-e-eval-ringing-down",
            "Open High E - ringing independent downstrokes",
            "down",
            "independent_down",
            6,
            "1.5-2 s ring, deliberate stop, 0.5 s reset; final stroke rings 5-8 s",
        ),
        (
            "high-e-eval-ringing-up",
            "Open High E - ringing independent upstrokes",
            "up",
            "independent_up",
            6,
            "1.5-2 s ring, deliberate stop, 0.5 s reset; final stroke rings 5-8 s",
        ),
        (
            "high-e-eval-ringing-alternate",
            "Open High E - ringing alternate repicking",
            "alternate_down_first",
            "alternate_down_first",
            12,
            "comfortable natural tempo; continuous D U sequence; final ring 4-6 s",
        ),
    ]
    for request_id, title, direction, pattern, stroke_count, timing in high_e_specs:
        items.append(
            request(
                request_id,
                title,
                phase_2,
                phase_2_title,
                phase_2_why,
                high_e_exercise_instructions(pattern),
                technique=("pick_contact" if pattern != "alternate_down_first" else "alternate_repicking"),
                string_name="open high E",
                direction=direction,
                dynamics="medium",
                pick_depth="shallow",
                pick=baseline_pick,
                muting="ringing",
                performance_pattern=pattern,
                stroke_count=stroke_count,
                timing=timing,
                takes=2,
            )
        )

    return items


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Create the phased Guitar AG reference-capture inventory."
    )
    parser.add_argument(
        "--capture-root",
        type=Path,
        default=default_documents_directory() / "Guitar AG Reference Captures",
    )
    parser.add_argument("--force", action="store_true")
    args = parser.parse_args()

    capture_root = args.capture_root.expanduser().resolve()
    inventory_path = capture_root / "capture-inventory.json"
    if inventory_path.exists() and not args.force:
        parser.error(f"inventory already exists: {inventory_path}; use --force to replace definitions")

    created_at = datetime.now().astimezone().isoformat(timespec="seconds")
    requests_directory = capture_root / "requests"
    sessions_directory = capture_root / "sessions"
    requests_directory.mkdir(parents=True, exist_ok=True)
    sessions_directory.mkdir(parents=True, exist_ok=True)

    inventory_items: list[dict[str, object]] = []
    for order, item in enumerate(build_items(), start=1):
        request_id = str(item["request_id"])
        request_path = requests_directory / f"{request_id}.json"
        session_directory = sessions_directory / request_id
        request_document = {
            "schema_version": 1,
            "request_id": request_id,
            "created_at": created_at,
            "title": item["title"],
            "instructions": item["instructions"],
            "research_reason": item["why"],
            "requested_take_count": item["requested_take_count"],
            "session_directory": str(session_directory),
            "context": item["context"],
            "capture_requirements": {
                "signal": "clean electric-guitar DI",
                "format": "mono 24-bit WAV",
                "preferred_sample_rates_hz": [44100],
                "target_peak_dbfs_at_most": -12.0,
                "processing": "none",
            },
        }
        request_path.write_text(json.dumps(request_document, indent=2) + "\n", encoding="utf-8")
        inventory_items.append(
            {
                "order": order,
                "request_id": request_id,
                "phase_id": item["phase_id"],
                "phase_title": item["phase_title"],
                "title": item["title"],
                "why": item["why"],
                "request_file": str(request_path),
                "required_approved_takes": 1,
            }
        )

    phase_order = []
    for item in inventory_items:
        phase = {"phase_id": item["phase_id"], "title": item["phase_title"]}
        if phase not in phase_order:
            phase_order.append(phase)

    inventory = {
        "schema_version": 1,
        "inventory_id": "guitar-ag-reference-inventory-v3-high-e-endpoint",
        "created_at": created_at,
        "title": "Guitar AG Low/High-E Endpoint Evaluation Inventory",
        "instructions": (
            "Retain the approved Phase 0 noise floor and six completed low-E items when "
            "settings are unchanged. Complete only the three Phase 2 ringing high-E items, "
            "then stop for endpoint comparison. Do not capture A, D, G, or B yet."
        ),
        "phases": phase_order,
        "items": inventory_items,
    }
    capture_root.mkdir(parents=True, exist_ok=True)
    inventory_path.write_text(json.dumps(inventory, indent=2) + "\n", encoding="utf-8")
    print(inventory_path)
    print(f"items={len(inventory_items)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
