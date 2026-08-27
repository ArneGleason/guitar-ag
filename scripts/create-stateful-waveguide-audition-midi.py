#!/usr/bin/env python3
"""Generate the focused Plan 0090 stateful-waveguide listening fixture."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 120
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
OUTPUT_PATH = Path("tests/midi/guitar-ag-stateful-waveguide-audition.mid")


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


def meta(kind: int, text: str) -> bytes:
    encoded = text.encode("ascii")
    return bytes([0xFF, kind]) + varlen(len(encoded)) + encoded


def note_on(note: int, velocity: int) -> bytes:
    return bytes([0x90, note, velocity])


def note_off(note: int) -> bytes:
    return bytes([0x80, note, 64])


events: list[tuple[int, int, bytes]] = []
order = 0


def add(tick: int, payload: bytes) -> None:
    global order
    events.append((tick, order, payload))
    order += 1


def marker(tick: int, text: str) -> None:
    add(tick, meta(0x06, text))


def note(tick: int, midi_note: int, duration: int, velocity: int) -> None:
    add(tick, note_on(midi_note, velocity))
    add(tick + duration, note_off(midi_note))


add(0, meta(0x03, "Guitar AG Plan 0090 Stateful Waveguide"))
add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, "big"))

marker(0, "1 Isolated wound string E2")
note(0, 40, 960, 100)

marker(1920, "2 Isolated plain string E4")
note(1920, 64, 960, 100)

marker(3840, "3 Eight state-preserving F2 repicks")
for hit in range(8):
    note(3840 + hit * 240, 41, 154, 92 + (hit % 3) * 4)

marker(6720, "4 E2 to G2 same-string hammer-on candidate")
add(6720, note_on(40, 100))
add(7080, note_on(43, 92))
add(7081, note_off(40))
add(7800, note_off(43))

track = bytearray()
previous_tick = 0

for tick, _, payload in sorted(events, key=lambda item: (item[0], item[1])):
    track += varlen(tick - previous_tick) + payload
    previous_tick = tick

track += b"\x00\xFF\x2F\x00"
header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
track_chunk = b"MTrk" + struct.pack(">I", len(track)) + track

OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
OUTPUT_PATH.write_bytes(header + track_chunk)
print(f"Wrote {OUTPUT_PATH}")
