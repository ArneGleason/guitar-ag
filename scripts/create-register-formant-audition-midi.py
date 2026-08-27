#!/usr/bin/env python3
"""Generate the focused Plan 0094 open-string register audition MIDI."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
TICKS_PER_SECOND = 960
OUTPUT_PATH = Path("tests/midi/register-formant-open-strings.mid")


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
            break

    return bytes(result)


def event(delta_ticks: int, payload: bytes) -> bytes:
    return varlen(delta_ticks) + payload


def text_event(delta_ticks: int, text: str) -> bytes:
    encoded = text.encode("ascii")
    return event(delta_ticks, bytes([0xFF, 0x01]) + varlen(len(encoded)) + encoded)


def build_track() -> bytes:
    notes = [(40, "E2"), (45, "A2"), (50, "D3"), (55, "G3"), (59, "B3"), (64, "E4")]
    note_duration = int(0.85 * TICKS_PER_SECOND)
    gap = int(0.35 * TICKS_PER_SECOND)
    velocity = 100

    data = bytearray()
    title = b"Guitar AG register formant open strings"
    data += event(0, b"\xff\x03" + varlen(len(title)) + title)
    data += event(0, b"\xff\x51\x03" + (500000).to_bytes(3, byteorder="big"))
    data += event(0, b"\xff\x58\x04\x04\x02\x18\x08")
    data += text_event(0, "E2 A2 D3 G3 B3 E4; 0.85 s notes; 0.35 s gaps")

    pending_delta = 0
    for midi_note, name in notes:
        data += text_event(pending_delta, name)
        data += event(0, bytes([0x90, midi_note, velocity]))
        data += event(note_duration, bytes([0x80, midi_note, 64]))
        pending_delta = gap

    data += event(pending_delta, b"\xff\x2f\x00")
    return bytes(data)


def main() -> int:
    track = build_track()
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    chunk = b"MTrk" + struct.pack(">I", len(track)) + track
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_PATH.write_bytes(header + chunk)
    print(f"Wrote {OUTPUT_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

