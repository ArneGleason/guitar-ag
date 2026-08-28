#!/usr/bin/env python3
"""Generate the compact Plan 0097 production-tone promotion phrase."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 120
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
OUTPUT_PATH = Path("tests/midi/tone-promotion-phrase.mid")


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


events: list[tuple[int, int, bytes]] = []
order = 0


def add(tick: int, payload: bytes) -> None:
    global order
    events.append((tick, order, payload))
    order += 1


def marker(tick: int, text: str) -> None:
    add(tick, meta(0x06, text))


def note(tick: int, midi_note: int, duration: int, velocity: int) -> None:
    add(tick, bytes([0x90, midi_note, velocity]))
    add(tick + duration, bytes([0x80, midi_note, 64]))


def note_run(start: int, notes: list[int], step: int, duration: int, velocities: list[int]) -> None:
    for index, midi_note in enumerate(notes):
        note(start + index * step, midi_note, duration, velocities[index % len(velocities)])


add(0, meta(0x03, "Guitar AG Plan 0097 tone promotion phrase"))
add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, "big"))

marker(0, "1 Wound-string picked riff")
note_run(0, [40, 43, 45, 47, 50, 47, 45, 43], 180, 140, [88, 96, 84, 100])

marker(1680, "2 Cross-string E minor arpeggio")
note_run(1680, [40, 47, 52, 55, 59, 64, 59, 55, 52, 47], 160, 135, [78, 86, 94, 82])

marker(3600, "3 Upper-register melodic phrase")
note_run(3600, [52, 55, 59, 64, 67, 64, 62, 59, 55, 52], 192, 160, [82, 90, 98, 86])

marker(5760, "4 Held E minor chord")
for chord_index, midi_note in enumerate([40, 47, 52, 55, 59, 64]):
    note(5760, midi_note, 1152, 84 + chord_index * 2)

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
