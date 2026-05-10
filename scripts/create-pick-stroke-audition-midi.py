#!/usr/bin/env python3
"""Generate a Guitar AG pick-stroke audition MIDI file."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 112
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
BEAT = TICKS_PER_QUARTER
BAR = BEAT * 4

AUDITION_MIDI_PATH = Path("tests/midi/guitar-ag-pick-stroke-audition.mid")


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


def marker_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x06]) + varlen(len(encoded)) + encoded


def text_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x01]) + varlen(len(encoded)) + encoded


def track_name_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x03]) + varlen(len(encoded)) + encoded


class Track:
    def __init__(self) -> None:
        self.events: list[tuple[int, int, bytes]] = []
        self.order = 0

    def add(self, tick: int, payload: bytes) -> None:
        self.events.append((tick, self.order, payload))
        self.order += 1

    def marker(self, tick: int, text: str) -> None:
        self.add(tick, marker_payload(text))

    def text(self, tick: int, text: str) -> None:
        self.add(tick, text_payload(text))

    def note(self, start: int, note: int, duration: int, velocity: int, channel: int = 1) -> None:
        self.add(start, note_on(note, velocity, channel))
        self.add(start + duration, note_off(note, channel))

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


def add_note_run(
    track: Track,
    start: int,
    notes: list[int],
    step_ticks: int,
    duration_ticks: int,
    velocities: list[int] | None = None,
    repeat: int = 1,
    gap_ticks: int = 0,
) -> int:
    tick = start

    for repeat_index in range(repeat):
        for index, note in enumerate(notes):
            velocity = velocities[index % len(velocities)] if velocities else 88
            track.note(tick, note, duration_ticks, velocity)
            tick += step_ticks

        if repeat_index + 1 < repeat:
            tick += gap_ticks

    return tick


def add_strum(
    track: Track,
    start: int,
    notes_low_to_high: list[int],
    duration: int,
    velocity: int,
    direction: str,
    strum_ticks: int = 18,
) -> None:
    notes = notes_low_to_high if direction == "down" else list(reversed(notes_low_to_high))

    for index, note in enumerate(notes):
        track.note(start + index * strum_ticks, note, duration, velocity)


def add_open_string_map(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "1 Open-string map: low-to-high then high-to-low")
    track.text(tick, "Use Pick Stroke Alternate. Low-to-high should match downstroke travel; high-to-low should match upstroke travel.")

    low_to_high = [40, 45, 50, 55, 59, 64]
    high_to_low = list(reversed(low_to_high))

    tick = add_note_run(track, tick, low_to_high, int(BEAT * 0.50), int(BEAT * 0.36), [78, 82, 86, 90, 94, 98])
    tick += BEAT
    tick = add_note_run(track, tick, high_to_low, int(BEAT * 0.50), int(BEAT * 0.36), [98, 94, 90, 86, 82, 78])

    return next_bar(tick + BEAT)


def add_same_string_tremolo(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "2 Same-string alternate: low E then high E tremolo")
    track.text(tick, "Repeated notes on one assigned string should still alternate down/up.")

    tremolo_velocities = [92, 78, 86, 80]
    tick = add_note_run(track, tick, [40] * 16, int(BEAT * 0.25), int(BEAT * 0.17), tremolo_velocities)
    tick += BEAT
    tick = add_note_run(track, tick, [64] * 16, int(BEAT * 0.25), int(BEAT * 0.17), tremolo_velocities)

    return next_bar(tick + BEAT)


def add_two_string_crossings(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "3 Two-string crossings: adjacent strings and direction changes")
    track.text(tick, "These expose whether Alternate follows string travel instead of blindly toggling.")

    patterns = [
        [40, 45] * 8,
        [45, 40] * 8,
        [59, 64] * 8,
        [64, 59] * 8,
    ]

    for pattern in patterns:
        tick = add_note_run(track, tick, pattern, int(BEAT * 0.25), int(BEAT * 0.18), [86, 82, 94, 78])
        tick += int(BEAT * 0.75)

    return next_bar(tick + BEAT)


def add_string_skips(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "4 String skips: economy direction through wide jumps")
    track.text(tick, "String skips should choose the direction of travel, not a strict down/up count.")

    skip_patterns = [
        [40, 50, 59, 45, 55, 64, 50, 59],
        [64, 55, 45, 59, 50, 40, 55, 45],
        [40, 55, 45, 64, 50, 59, 45, 55],
        [64, 50, 59, 40, 55, 45, 50, 40],
    ]

    for pattern in skip_patterns:
        tick = add_note_run(track, tick, pattern, int(BEAT * 0.31), int(BEAT * 0.24), [80, 90, 84, 96])
        tick += int(BEAT * 0.50)

    return next_bar(tick + BEAT)


def add_down_up_chords(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "5 Chord strums: downstroke then upstroke order")
    track.text(tick, "Open-position voicings are original audition material using common guitar shapes.")

    chords = [
        ("Em shape", [40, 47, 52, 55, 59, 64]),
        ("G shape", [43, 47, 50, 55, 59, 67]),
        ("D shape", [50, 57, 62, 66]),
        ("A shape", [45, 52, 57, 61, 64]),
    ]

    for name, notes in chords:
        track.text(tick, f"{name}: down then up")
        add_strum(track, tick, notes, int(BEAT * 1.15), 88, "down", strum_ticks=20)
        add_strum(track, tick + int(BEAT * 1.65), notes, int(BEAT * 1.05), 82, "up", strum_ticks=20)
        tick += BAR * 2

    return next_bar(tick)


def add_strum_groove(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "6 Back-and-forth strum groove: alternating chord travel")
    track.text(tick, "A compact rhythm-guitar check: down/up ordering should feel like hand motion.")

    progression = [
        [40, 47, 52, 55, 59, 64],
        [43, 47, 50, 55, 59, 67],
        [45, 52, 57, 61, 64],
        [50, 57, 62, 66],
    ]

    for bar_index in range(4):
        notes = progression[bar_index % len(progression)]

        for stroke_index in range(8):
            direction = "down" if stroke_index % 2 == 0 else "up"
            velocity = 92 if direction == "down" else 78
            add_strum(track,
                      tick + stroke_index * int(BEAT * 0.50),
                      notes,
                      int(BEAT * 0.34),
                      velocity,
                      direction,
                      strum_ticks=12)

        tick += BAR

    return next_bar(tick + BEAT)


def add_crosspicking_etude(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "7 Crosspicking etude: rolling chord strings")
    track.text(tick, "Original rolling patterns inspired by common flatpick and classical arpeggio exercises.")

    patterns = [
        [52, 59, 64, 59, 55, 64, 59, 52],
        [47, 55, 62, 55, 52, 62, 55, 47],
        [45, 52, 61, 52, 57, 64, 61, 52],
        [50, 57, 66, 57, 62, 66, 57, 50],
    ]

    for pattern in patterns:
        tick = add_note_run(track, tick, pattern, int(BEAT * 0.25), int(BEAT * 0.20), [78, 84, 92, 82])
        tick += int(BEAT * 0.50)

    return next_bar(tick + BEAT)


def add_pedal_riffs(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "8 Pedal riffs: rapid same-string picking into string changes")
    track.text(tick, "Original riff studies: low-string pedal tones, accents, and occasional string crossings.")

    riffs = [
        [40, 40, 43, 40, 45, 40, 43, 40, 47, 40, 45, 40, 43, 40, 40, 45],
        [45, 45, 48, 45, 50, 45, 52, 45, 50, 45, 48, 45, 47, 45, 45, 40],
        [52, 52, 55, 52, 59, 52, 55, 52, 64, 52, 59, 52, 55, 52, 50, 45],
    ]

    for riff in riffs:
        tick = add_note_run(track, tick, riff, int(BEAT * 0.25), int(BEAT * 0.18), [96, 78, 88, 80])
        tick += BEAT

    return next_bar(tick + BEAT)


def add_scale_fragment(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "9 Scale fragments: phrase-like alternate and economy blend")
    track.text(tick, "A melodic check across repeated notes, adjacent strings, and small position shifts.")

    fragments = [
        [52, 55, 57, 59, 57, 55, 52, 50],
        [55, 57, 59, 62, 59, 57, 55, 52],
        [59, 62, 64, 67, 64, 62, 59, 57],
        [64, 67, 69, 72, 69, 67, 64, 59],
    ]

    for fragment in fragments:
        tick = add_note_run(track, tick, fragment, int(BEAT * 0.25), int(BEAT * 0.20), [82, 88, 94, 86])
        tick += int(BEAT * 0.50)

    return next_bar(tick + BEAT)


def add_finale(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "10 Finale: mixed riff, strum, tremolo, final chord")

    add_strum(track, tick, [40, 47, 52, 55, 59, 64], int(BEAT * 1.20), 90, "down", strum_ticks=16)
    tick += int(BEAT * 1.50)
    tick = add_note_run(track, tick, [40, 45, 50, 55, 59, 64, 59, 55, 50, 45, 40], int(BEAT * 0.25), int(BEAT * 0.18), [86, 92])
    tick += int(BEAT * 0.75)
    tick = add_note_run(track, tick, [64] * 12, int(BEAT * 0.20), int(BEAT * 0.14), [96, 82, 88])
    tick += int(BEAT * 0.75)
    add_strum(track, tick, [40, 47, 52, 55, 59, 64], BAR * 2, 82, "down", strum_ticks=24)
    track.marker(tick + BAR * 3, "End")

    return tick + BAR * 3


def build_track() -> bytes:
    track = Track()
    track.add(0, track_name_payload("Guitar AG pick stroke audition"))
    track.add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, byteorder="big"))
    track.add(0, b"\xFF\x58\x04\x04\x02\x18\x08")
    track.text(0, "Suggested setup: Pick Stroke Alternate, Legato Articulation 0%, Pick Bite 20-60%, String Age to taste.")

    tick = BAR
    tick = add_open_string_map(track, tick)
    tick = add_same_string_tremolo(track, tick)
    tick = add_two_string_crossings(track, tick)
    tick = add_string_skips(track, tick)
    tick = add_down_up_chords(track, tick)
    tick = add_strum_groove(track, tick)
    tick = add_crosspicking_etude(track, tick)
    tick = add_pedal_riffs(track, tick)
    tick = add_scale_fragment(track, tick)
    add_finale(track, tick)

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
