#!/usr/bin/env python3
"""Tests for the dependency-free reference-capture file contract."""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
import wave
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]
CREATE_SCRIPT = REPO_ROOT / "scripts" / "create-reference-capture-request.py"
SUMMARY_SCRIPT = REPO_ROOT / "scripts" / "summarize-reference-capture-session.py"
INVENTORY_SCRIPT = REPO_ROOT / "scripts" / "create-reference-capture-inventory.py"


class ReferenceCaptureScriptsTest(unittest.TestCase):
    def test_request_and_approved_session_round_trip(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            request_path = root / "request.json"
            result = subprocess.run(
                [
                    sys.executable,
                    str(CREATE_SCRIPT),
                    "--title",
                    "Low E ringing",
                    "--instructions",
                    "Record eight isolated strokes.",
                    "--string",
                    "low E",
                    "--direction",
                    "down",
                    "--dynamics",
                    "medium",
                    "--muting",
                    "ringing",
                    "--takes",
                    "8",
                    "--capture-root",
                    str(root / "captures"),
                    "--output",
                    str(request_path),
                ],
                check=True,
                capture_output=True,
                text=True,
            )
            self.assertEqual(Path(result.stdout.strip()), request_path.resolve())

            request = json.loads(request_path.read_text(encoding="utf-8"))
            self.assertEqual(request["schema_version"], 1)
            self.assertEqual(request["requested_take_count"], 8)
            self.assertEqual(request["context"]["string"], "low E")
            self.assertEqual(request["context"]["muting"], "ringing")

            session_directory = Path(request["session_directory"])
            session_directory.mkdir(parents=True)
            wav_path = session_directory / "take-001.wav"
            with wave.open(str(wav_path), "wb") as wav_file:
                wav_file.setnchannels(1)
                wav_file.setsampwidth(3)
                wav_file.setframerate(48000)
                wav_file.writeframes(b"\0\0\0" * 4800)

            manifest_path = session_directory / "session.json"
            manifest_path.write_text(
                json.dumps(
                    {
                        "schema_version": 1,
                        "request_id": request["request_id"],
                        "takes": [
                            {
                                "take_number": 1,
                                "file": wav_path.name,
                                "status": "approved",
                                "peak_dbfs": -18.0,
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )

            summary = subprocess.run(
                [sys.executable, str(SUMMARY_SCRIPT), str(manifest_path)],
                check=True,
                capture_output=True,
                text=True,
            )
            self.assertIn("approved: 1", summary.stdout)
            self.assertIn("48000 Hz", summary.stdout)

            approved = subprocess.run(
                [
                    sys.executable,
                    str(SUMMARY_SCRIPT),
                    str(manifest_path),
                    "--approved-paths",
                ],
                check=True,
                capture_output=True,
                text=True,
            )
            self.assertEqual(Path(approved.stdout.strip()), wav_path.resolve())

    def test_zero_requested_takes_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            result = subprocess.run(
                [
                    sys.executable,
                    str(CREATE_SCRIPT),
                    "--title",
                    "Invalid",
                    "--instructions",
                    "Invalid",
                    "--takes",
                    "0",
                    "--capture-root",
                    temporary_directory,
                ],
                capture_output=True,
                text=True,
            )
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("between 1 and 100", result.stderr)

    def test_phased_inventory_is_stable_and_finite(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory) / "captures"
            result = subprocess.run(
                [sys.executable, str(INVENTORY_SCRIPT), "--capture-root", str(root)],
                check=True,
                capture_output=True,
                text=True,
            )
            self.assertIn("items=30", result.stdout)

            inventory_path = root / "capture-inventory.json"
            inventory = json.loads(inventory_path.read_text(encoding="utf-8"))
            self.assertEqual(inventory["schema_version"], 1)
            self.assertEqual(len(inventory["items"]), 30)
            self.assertEqual(len(inventory["phases"]), 5)
            self.assertIn("Phase 0 and Phase 1 first", inventory["instructions"])

            phase_one = [
                item
                for item in inventory["items"]
                if item["phase_id"] == "phase-1-pick-contact-baseline"
            ]
            self.assertEqual(len(phase_one), 6)
            self.assertTrue(all(item["required_approved_takes"] == 1 for item in phase_one))

            request_ids = [item["request_id"] for item in inventory["items"]]
            self.assertEqual(len(request_ids), len(set(request_ids)))
            for item in inventory["items"]:
                request_path = Path(item["request_file"])
                request = json.loads(request_path.read_text(encoding="utf-8"))
                self.assertEqual(request["request_id"], item["request_id"])
                self.assertEqual(request["research_reason"], item["why"])

            duplicate = subprocess.run(
                [sys.executable, str(INVENTORY_SCRIPT), "--capture-root", str(root)],
                capture_output=True,
                text=True,
            )
            self.assertNotEqual(duplicate.returncode, 0)
            self.assertIn("inventory already exists", duplicate.stderr)


if __name__ == "__main__":
    unittest.main()
