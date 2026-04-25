#!/usr/bin/env python3
"""Generate canonical Guitar AG calibration MIDI files."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 120
TICKS_PER_SECOND = TICKS_PER_QUARTER * BPM // 60
MIDI_PATH = Path("tests/midi/single-note-calibration.mid")


def varlen(value: int) -> bytes:
    buffer = value & 0x7f
    value >>= 7

    while value:
        buffer <<= 8
        buffer |= ((value & 0x7f) | 0x80)
        value >>= 7

    result = bytearray()

    while True:
        result.append(buffer & 0xff)

        if buffer & 0x80:
            buffer >>= 8
        else:
            break

    return bytes(result)


def event(delta_ticks: int, payload: bytes) -> bytes:
    return varlen(delta_ticks) + payload


def note_on(note: int, velocity: int, channel: int = 0) -> bytes:
    return bytes([0x90 | channel, note, velocity])


def note_off(note: int, channel: int = 0) -> bytes:
    return bytes([0x80 | channel, note, 64])


def text_event(delta_ticks: int, text: str) -> bytes:
    encoded = text.encode("ascii")
    return event(delta_ticks, bytes([0xff, 0x01]) + varlen(len(encoded)) + encoded)


def build_track() -> bytes:
    notes = [
        (40, "E2"),
        (45, "A2"),
        (50, "D3"),
        (55, "G3"),
        (59, "B3"),
        (64, "E4"),
    ]

    long_duration = int(1.5 * TICKS_PER_SECOND)
    short_duration = int(0.12 * TICKS_PER_SECOND)
    gap = int(1.0 * TICKS_PER_SECOND)
    velocity = 100

    data = bytearray()
    data += event(0, b"\xff\x03" + varlen(len(b"Guitar AG single-note calibration")) + b"Guitar AG single-note calibration")
    data += event(0, b"\xff\x51\x03" + (500000).to_bytes(3, byteorder="big"))
    data += event(0, b"\xff\x58\x04\x04\x02\x18\x08")
    data += text_event(0, "Long notes: 1.5 s at velocity 100")

    pending_delta = 0

    for note, name in notes:
        data += text_event(pending_delta, f"long {name}")
        data += event(0, note_on(note, velocity))
        data += event(long_duration, note_off(note))
        pending_delta = gap

    data += text_event(pending_delta, "Short notes: 0.12 s at velocity 100")
    pending_delta = gap

    for note, name in notes:
        data += text_event(pending_delta, f"short {name}")
        data += event(0, note_on(note, velocity))
        data += event(short_duration, note_off(note))
        pending_delta = gap

    data += event(pending_delta, b"\xff\x2f\x00")
    return bytes(data)


def write_midi(path: Path) -> None:
    track = build_track()
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    chunk = b"MTrk" + struct.pack(">I", len(track)) + track
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + chunk)


def main() -> int:
    write_midi(MIDI_PATH)
    print(f"Wrote {MIDI_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
