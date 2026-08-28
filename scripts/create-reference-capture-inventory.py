#!/usr/bin/env python3
"""Create the phased Guitar AG reference-capture starter inventory."""

from __future__ import annotations

import argparse
import ctypes
import json
import os
from datetime import datetime
from pathlib import Path


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
    takes: int = 3,
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
            "guitar": "identify the guitar in take notes",
            "pickup": "keep one pickup/settings combination fixed; identify it in notes",
            "comparison_group": phase_id,
        },
    }


def pick_instructions(
    string_name: str,
    direction: str,
    dynamics: str,
    depth: str,
    muting: str,
    pick: str,
) -> str:
    mute_text = {
        "ringing": "Let the string ring normally.",
        "hand-damped": (
            "Lightly touch the string at several non-harmonic fretting-hand positions, "
            "away from the pick, so sustained pitch is suppressed."
        ),
        "foam-damped": (
            "Weave soft foam or cloth through the strings near the fretboard, away "
            "from the pick, to damp propagation."
        ),
    }[muting]
    return (
        "Keep guitar, pickup, controls, interface gain, picking location, and the "
        f"specified pick fixed within the comparison group. {mute_text} For each take, "
        f"leave about one second of untouched input, then play eight isolated {dynamics} "
        f"{direction}strokes on {string_name} with {depth} pick depth and about half a "
        "second between strokes. Let the final event decay. Record up to three batches "
        "and approve at least one consistent, representative batch. Note the exact guitar, "
        f"pickup setting, and {pick} in the selected-take notes."
    )


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

    phase_1 = "phase-1-pick-contact-baseline"
    phase_1_title = "Phase 1 — Minimum pick-contact baseline"
    phase_1_why = (
        "Separates register and damping effects before expanding direction, force, or material."
    )
    baseline_pick = "one ordinary medium plastic pick"
    for prefix, string_name in (("low-e", "open low E"), ("high-e", "open high E")):
        for muting in ("ringing", "hand-damped", "foam-damped"):
            items.append(
                request(
                    f"pick-baseline-{prefix}-{muting}",
                    f"{string_name.title()} medium downstroke — {muting}",
                    phase_1,
                    phase_1_title,
                    phase_1_why,
                    pick_instructions(
                        string_name, "down", "medium", "shallow", muting, baseline_pick
                    ),
                    technique="pick_contact",
                    string_name=string_name,
                    direction="down",
                    dynamics="medium",
                    pick_depth="shallow",
                    pick=baseline_pick,
                    muting=muting,
                )
            )

    phase_2 = "phase-2-pick-response-axes"
    phase_2_title = "Phase 2 — Direction, force, depth, and middle strings"
    phase_2_why = "Measures the smallest response axes needed to scale one believable pick model."
    phase_2_specs = [
        ("d-string", "open D", "down", "medium", "shallow"),
        ("b-string", "open B", "down", "medium", "shallow"),
        ("low-e-up", "open low E", "up", "medium", "shallow"),
        ("high-e-up", "open high E", "up", "medium", "shallow"),
        ("low-e-light", "open low E", "down", "light", "shallow"),
        ("low-e-hard", "open low E", "down", "hard", "shallow"),
        ("high-e-light", "open high E", "down", "light", "shallow"),
        ("high-e-hard", "open high E", "down", "hard", "shallow"),
        ("low-e-deep", "open low E", "down", "medium", "deep"),
        ("high-e-deep", "open high E", "down", "medium", "deep"),
    ]
    for slug, string_name, direction, dynamics, depth in phase_2_specs:
        items.append(
            request(
                f"pick-response-{slug}",
                f"{string_name.title()} {dynamics} {direction}stroke — {depth}",
                phase_2,
                phase_2_title,
                phase_2_why,
                pick_instructions(
                    string_name, direction, dynamics, depth, "ringing", baseline_pick
                ),
                technique="pick_contact",
                string_name=string_name,
                direction=direction,
                dynamics=dynamics,
                pick_depth=depth,
                pick=baseline_pick,
                muting="ringing",
            )
        )

    phase_3 = "phase-3-pick-material"
    phase_3_title = "Phase 3 — Flexible versus stiff pick material"
    phase_3_why = "Tests whether compliance and edge texture need separable model controls."
    for pick_slug, pick_name in (
        ("flexible", "one clearly flexible/thin pick"),
        ("stiff", "one clearly stiff/thick pick"),
    ):
        for prefix, string_name in (("low-e", "open low E"), ("high-e", "open high E")):
            items.append(
                request(
                    f"pick-material-{pick_slug}-{prefix}-ringing",
                    f"{string_name.title()} — {pick_slug} pick ringing",
                    phase_3,
                    phase_3_title,
                    phase_3_why,
                    pick_instructions(
                        string_name, "down", "medium", "shallow", "ringing", pick_name
                    ),
                    technique="pick_contact",
                    string_name=string_name,
                    direction="down",
                    dynamics="medium",
                    pick_depth="shallow",
                    pick=pick_name,
                    muting="ringing",
                )
            )
        items.append(
            request(
                f"pick-material-{pick_slug}-low-e-hand-damped",
                f"Open Low E — {pick_slug} pick hand-damped",
                phase_3,
                phase_3_title,
                phase_3_why,
                pick_instructions(
                    "open low E", "down", "medium", "shallow", "hand-damped", pick_name
                ),
                technique="pick_contact",
                string_name="open low E",
                direction="down",
                dynamics="medium",
                pick_depth="shallow",
                pick=pick_name,
                muting="hand-damped",
            )
        )

    phase_4 = "phase-4-finger-and-surface"
    phase_4_title = "Phase 4 — Finger, nail, lift, and slide references"
    phase_4_why = "Seeds later motion-driven finger and fret-contact models after pick contact is calibrated."
    for attack, title_part in (("finger_flesh", "Finger flesh"), ("fingernail", "Fingernail")):
        for prefix, string_name in (("low-e", "open low E"), ("high-e", "open high E")):
            items.append(
                request(
                    f"finger-attack-{attack}-{prefix}",
                    f"{title_part} attack — {string_name}",
                    phase_4,
                    phase_4_title,
                    phase_4_why,
                    f"Keep the guitar, pickup, controls, interface gain, and plucking location fixed. "
                    f"For each take, leave about one second of untouched input, then make eight isolated "
                    f"medium attacks on {string_name} using {title_part.lower()} only, with about half a "
                    "second between attacks. Record up to three batches and approve at least one consistent batch.",
                    technique=attack,
                    string_name=string_name,
                    dynamics="medium",
                    muting="ringing",
                )
            )

    for direction in ("up", "down"):
        items.append(
            request(
                f"finger-slide-d-string-{direction}",
                f"Wound D-string finger slide — {direction}",
                phase_4,
                phase_4_title,
                phase_4_why,
                "Use the wound D string with the normal fretting finger pressure used in playing. "
                f"Make six isolated five-fret slides {direction}, allowing silence between gestures. "
                "Keep slide distance and speed as consistent as practical, then approve at least one representative batch.",
                technique="finger_slide",
                string_name="D string, five-fret span",
                direction=direction,
                dynamics="medium",
            )
        )

    items.append(
        request(
            "finger-lift-d-string",
            "Wound D-string finger lift",
            phase_4,
            phase_4_title,
            phase_4_why,
            "Fret one mid-neck note on the wound D string, pick it normally, then release the fretting "
            "finger with a natural lift after about one second. Make six isolated gestures per take. "
            "Keep the fret, pressure, and timing consistent and approve at least one representative batch.",
            technique="finger_lift",
            string_name="D string, one documented mid-neck fret",
            dynamics="medium",
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
                "preferred_sample_rates_hz": [48000, 96000],
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
        "inventory_id": "guitar-ag-reference-inventory-v1",
        "created_at": created_at,
        "title": "Guitar AG Reference Capture Inventory",
        "instructions": (
            "Complete Phase 0 and Phase 1 first, then stop and ask Codex to analyze the "
            "approved sessions before proceeding. Later phases are a roadmap, not homework."
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
