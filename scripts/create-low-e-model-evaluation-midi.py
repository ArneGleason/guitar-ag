#!/usr/bin/env python3
"""Generate deterministic low-E exercises matching Plan 0103's capture shapes."""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 120
TICKS_PER_SECOND = 960
LOW_E = 40
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


def note_on() -> bytes:
    return bytes([0x90, LOW_E, VELOCITY])


def note_off() -> bytes:
    return bytes([0x80, LOW_E, 64])


def meta(kind: int, value: bytes) -> bytes:
    return bytes([0xFF, kind]) + varlen(len(value)) + value


def ticks(seconds: float) -> int:
    return round(seconds * TICKS_PER_SECOND)


def make_track(name: str, notes: list[tuple[float, float]]) -> bytes:
    absolute_events: list[tuple[int, int, bytes]] = [
        (0, 0, meta(0x03, name.encode("ascii"))),
        (0, 1, b"\xff\x51\x03" + (500000).to_bytes(3, "big")),
        (0, 2, b"\xff\x58\x04\x04\x02\x18\x08"),
    ]
    for index, (start_seconds, duration_seconds) in enumerate(notes):
        start = ticks(start_seconds)
        absolute_events.append((start, 10 + index * 2, note_on()))
        absolute_events.append((start + ticks(duration_seconds), 11 + index * 2, note_off()))

    data = bytearray()
    previous_tick = 0
    for tick, _, payload in sorted(absolute_events, key=lambda item: (item[0], item[1])):
        data += event(tick - previous_tick, payload)
        previous_tick = tick
    data += event(0, b"\xff\x2f\x00")
    return bytes(data)


def write_midi(path: Path, name: str, notes: list[tuple[float, float]]) -> None:
    track = make_track(name, notes)
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + b"MTrk" + struct.pack(">I", len(track)) + track)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-directory", type=Path, required=True)
    args = parser.parse_args()

    exercises = {
        "ringing-independent.mid": (
            "Low E ringing independent",
            [
                (1.0, 1.6),
                (3.1, 1.6),
                (5.2, 1.6),
                (7.3, 1.6),
                (9.4, 1.6),
                (11.5, 8.0),
            ],
        ),
        "ringing-alternate.mid": (
            "Low E ringing alternate",
            [(1.0 + index * 0.42, 0.415 if index < 11 else 5.0) for index in range(12)],
        ),
        "damped-independent.mid": (
            "Low E damped independent",
            [(1.0 + index * 0.70, 0.12) for index in range(6)],
        ),
        "damped-alternate.mid": (
            "Low E damped alternate",
            [(1.0 + index * 0.55, 0.11) for index in range(6)],
        ),
    }

    for filename, (name, notes) in exercises.items():
        path = args.output_directory / filename
        write_midi(path, name, notes)
        print(path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
