#!/usr/bin/env python3
"""Generate a Guitar AG auto-strum audition MIDI file."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 96
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
BEAT = TICKS_PER_QUARTER
BAR = BEAT * 4

AUDITION_MIDI_PATH = Path("tests/midi/guitar-ag-auto-strum-audition.mid")


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


def delta_event(delta_ticks: int, payload: bytes) -> bytes:
    return varlen(delta_ticks) + payload


def midi_channel(channel: int) -> int:
    if channel < 1 or channel > 16:
        raise ValueError(f"MIDI channel must be 1-16, got {channel}")

    return channel - 1


def note_on(note: int, velocity: int, channel: int = 1) -> bytes:
    return bytes([0x90 | midi_channel(channel), note, max(0, min(127, velocity))])


def note_off(note: int, channel: int = 1) -> bytes:
    return bytes([0x80 | midi_channel(channel), note, 64])


def text_event(event_type: int, text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, event_type]) + varlen(len(encoded)) + encoded


class Track:
    def __init__(self) -> None:
        self.events: list[tuple[int, int, bytes]] = []
        self.order = 0

    def add(self, tick: int, payload: bytes) -> None:
        self.events.append((tick, self.order, payload))
        self.order += 1

    def marker(self, tick: int, text: str) -> None:
        self.add(tick, text_event(0x06, text))

    def text(self, tick: int, text: str) -> None:
        self.add(tick, text_event(0x01, text))

    def note(self, start: int, note: int, duration: int, velocity: int, channel: int = 1) -> None:
        self.add(start, note_on(note, velocity, channel))
        self.add(start + duration, note_off(note, channel))

    def block_chord(self, start: int, notes_low_to_high: list[int], duration: int, velocity: int) -> None:
        for note in notes_low_to_high:
            self.add(start, note_on(note, velocity))

        for note in notes_low_to_high:
            self.add(start + duration, note_off(note))

    def render(self) -> bytes:
        data = bytearray()
        previous_tick = 0

        for tick, _, payload in sorted(self.events, key=lambda item: (item[0], item[1])):
            data += delta_event(tick - previous_tick, payload)
            previous_tick = tick

        data += delta_event(0, b"\xFF\x2F\x00")
        return bytes(data)


def next_bar(tick: int) -> int:
    return ((tick + BAR - 1) // BAR) * BAR


def add_block_chord_reference(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "1 Block chords: exact simultaneous note-ons")
    track.text(tick, "Use Strum Speed 0% versus 40-100%; Pick Stroke Down/Up/Alternate changes the generated string order.")

    chords = [
        ("E major", [40, 47, 52, 56, 59, 64], 88),
        ("G major", [43, 47, 50, 55, 59, 67], 86),
        ("A major", [45, 52, 57, 61, 64], 84),
        ("D major", [50, 57, 62, 66], 84),
    ]

    for name, notes, velocity in chords:
        track.text(tick, f"{name}: all notes start on the same tick")
        track.block_chord(tick, notes, BAR, velocity)
        tick += BAR

    return next_bar(tick + BEAT)


def add_repeated_block_groove(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "2 Repeated block groove: VST creates the stroke")
    track.text(tick, "The note-ons are stacked; repeated blocks are separated by one tick of note-off cleanup.")

    progression = [
        [40, 47, 52, 55, 59, 64],
        [43, 47, 50, 55, 59, 67],
        [45, 52, 57, 61, 64],
        [50, 57, 62, 66],
    ]

    for bar_index, notes in enumerate(progression):
        for stroke_index in range(4):
            start_tick = tick + stroke_index * BEAT
            velocity = 92 if stroke_index % 2 == 0 else 78
            duration = BEAT - 2
            track.block_chord(start_tick, notes, duration, velocity)

        tick += BAR

    return next_bar(tick + BEAT)


def add_partial_strums(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "3 Partial-string blocks: skipped strings and smaller grips")
    track.text(tick, "These check that same-sample dyads/triads still follow assigned string order rather than pitch only.")

    grips = [
        [40, 52, 59],
        [45, 57, 64],
        [50, 59, 67],
        [40, 55, 64],
        [43, 55, 67],
        [45, 61, 69],
    ]

    for index, notes in enumerate(grips):
        track.block_chord(tick, notes, int(BEAT * 0.80), 82 + (index % 3) * 5)
        tick += int(BEAT * 1.10)

    return next_bar(tick + BEAT)


def add_single_note_control(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "4 Single-note control: Auto Strum should not change this")
    track.text(tick, "Single-note runs should still use Pick Stroke and Player Feel exactly as before.")

    notes = [40, 45, 50, 55, 59, 64, 59, 55, 50, 45, 40, 43, 47, 50, 55, 59]

    for index, note in enumerate(notes):
        track.note(tick, note, int(BEAT * 0.32), 80 + (index % 4) * 5)
        tick += int(BEAT * 0.42)

    return next_bar(tick + BEAT)


def add_final_chord(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "5 Final held block chord")
    track.block_chord(tick, [40, 47, 52, 56, 59, 64], BAR * 2, 82)
    track.marker(tick + BAR * 3, "End")
    return tick + BAR * 3


def build_track() -> bytes:
    track = Track()
    track.add(0, text_event(0x03, "Guitar AG auto-strum audition"))
    track.add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, byteorder="big"))
    track.add(0, b"\xFF\x58\x04\x04\x02\x18\x08")
    track.text(0, "Suggested setup: Pick Stroke Alternate, Strum Speed 35-70%, Player Feel around Pro/50%.")

    tick = BAR
    tick = add_block_chord_reference(track, tick)
    tick = add_repeated_block_groove(track, tick)
    tick = add_partial_strums(track, tick)
    tick = add_single_note_control(track, tick)
    add_final_chord(track, tick)

    return track.render()


def write_midi(path: Path) -> None:
    track = build_track()
    header = b"MThd" + struct.pack(">IHHH", 6, 0, 1, TICKS_PER_QUARTER)
    chunk = b"MTrk" + struct.pack(">I", len(track)) + track
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + chunk)


def main() -> int:
    write_midi(AUDITION_MIDI_PATH)
    print(f"Wrote {AUDITION_MIDI_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
