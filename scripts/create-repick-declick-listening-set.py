#!/usr/bin/env python3
"""Create an order-balanced Plan 0106 control/1 ms repick listening set."""

from __future__ import annotations

import argparse
import importlib.util
import json
import math
import sys
from pathlib import Path

import numpy as np


def load_audio_helpers(path: Path):
    spec = importlib.util.spec_from_file_location("low_e_listening_helpers", path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"could not load listening helpers: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def db(value: float) -> float:
    return 20.0 * math.log10(max(float(value), 1.0e-12))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--render-directory", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument(
        "--helpers",
        type=Path,
        default=Path(__file__).with_name("create-low-e-candidate-listening-set.py"),
    )
    args = parser.parse_args()
    helpers = load_audio_helpers(args.helpers.resolve())

    control_rate, control = helpers.read_pcm24(args.render_directory / "control.wav")
    candidate_rate, candidate = helpers.read_pcm24(args.render_directory / "crossfade-1ms.wav")
    if control_rate != candidate_rate or control.shape != candidate.shape:
        raise ValueError("control and candidate sources do not align")

    start = round(0.75 * control_rate)
    end = round(6.60 * control_rate)
    control = control[start:end].copy()
    candidate = candidate[start:end].copy()
    target_rms = 10.0 ** (-18.0 / 20.0)
    gains = []
    for samples in (control, candidate):
        rms = math.sqrt(float(np.mean(samples * samples)))
        safe_gain = 0.891 / max(float(np.max(np.abs(samples))), 1.0e-12)
        gains.append(min(target_rms / max(rms, 1.0e-12), safe_gain))
    control *= gains[0]
    candidate *= gains[1]

    args.output_directory.mkdir(parents=True, exist_ok=True)
    control_path = args.output_directory / "01-accepted-candidate-restart.wav"
    candidate_path = args.output_directory / "02-one-ms-modal-tail-crossfade.wav"
    helpers.write_pcm24(control_path, control_rate, control)
    helpers.write_pcm24(candidate_path, control_rate, candidate)

    pause = np.zeros((round(0.65 * control_rate), control.shape[1]), dtype=np.float64)
    helpers.write_pcm24(
        args.output_directory / "03-control-then-declicked.wav",
        control_rate,
        np.concatenate((control, pause, candidate), axis=0),
    )
    helpers.write_pcm24(
        args.output_directory / "04-declicked-then-control.wav",
        control_rate,
        np.concatenate((candidate, pause, control), axis=0),
    )
    report = {
        "sample_rate": control_rate,
        "channels": control.shape[1],
        "duration_seconds_each": len(control) / control_rate,
        "target_rms_dbfs": -18.0,
        "control_gain_db": db(gains[0]),
        "candidate_gain_db": db(gains[1]),
        "control_peak_dbfs": db(float(np.max(np.abs(control)))),
        "candidate_peak_dbfs": db(float(np.max(np.abs(candidate)))),
    }
    report_path = args.output_directory / "listening-set.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for path in (control_path, candidate_path, report_path):
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
