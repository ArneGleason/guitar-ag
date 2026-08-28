#!/usr/bin/env python3
"""Generate deterministic ringing exercises for one open-string MIDI note."""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
TICKS_PER_SECOND = 960
VELOCITY = 100


def varlen(value: int) -> bytes:
    buffer = value & 0x7F
    value >>= 7
    while value:
        buffer <<= 8
        buffer |= (value & 0x7F) | 0x80
        value >>= 7

    result = bytearray()
    while True:
        result.append(buffer & 0xFF)
        if buffer & 0x80:
            buffer >>= 8
        else:
            return bytes(result)


def event(delta_ticks: int, payload: bytes) -> bytes:
    return varlen(delta_ticks) + payload


def meta(kind: int, value: bytes) -> bytes:
    return bytes([0xFF, kind]) + varlen(len(value)) + value


def ticks(seconds: float) -> int:
    return round(seconds * TICKS_PER_SECOND)


def make_track(name: str, midi_note: int, notes: list[tuple[float, float]]) -> bytes:
    absolute_events: list[tuple[int, int, bytes]] = [
        (0, 0, meta(0x03, name.encode("ascii"))),
        (0, 1, b"\xff\x51\x03" + (500000).to_bytes(3, "big")),
        (0, 2, b"\xff\x58\x04\x04\x02\x18\x08"),
    ]
    for index, (start_seconds, duration_seconds) in enumerate(notes):
        start = ticks(start_seconds)
        absolute_events.append((start, 10 + index * 2, bytes([0x90, midi_note, VELOCITY])))
        absolute_events.append(
            (start + ticks(duration_seconds), 11 + index * 2, bytes([0x80, midi_note, 64]))
        )

    data = bytearray()
    previous_tick = 0
    for tick, _, payload in sorted(absolute_events, key=lambda item: (item[0], item[1])):
        data += event(tick - previous_tick, payload)
        previous_tick = tick
    data += event(0, b"\xff\x2f\x00")
    return bytes(data)


def write_midi(path: Path, name: str, midi_note: int, notes: list[tuple[float, float]]) -> None:
    track = make_track(name, midi_note, notes)
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + b"MTrk" + struct.pack(">I", len(track)) + track)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-directory", type=Path, required=True)
    parser.add_argument("--midi-note", type=int, default=64)
    parser.add_argument("--label", default="High E")
    args = parser.parse_args()
    if not 0 <= args.midi_note <= 127:
        parser.error("--midi-note must be in 0..127")

    exercises = {
        "ringing-independent.mid": [
            (1.0, 1.1),
            (2.6, 1.1),
            (4.2, 1.1),
            (5.8, 1.1),
            (7.4, 1.1),
            (9.0, 6.0),
        ],
        "ringing-alternate.mid": [
            (1.0 + index * 0.58, 0.575 if index < 11 else 5.0) for index in range(12)
        ],
    }

    for filename, notes in exercises.items():
        pattern = "independent" if "independent" in filename else "alternate"
        path = args.output_directory / filename
        write_midi(path, f"{args.label} ringing {pattern}", args.midi_note, notes)
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
