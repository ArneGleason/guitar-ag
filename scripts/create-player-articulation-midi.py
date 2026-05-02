#!/usr/bin/env python3
"""Generate a Guitar AG player-articulation audition MIDI file."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 112
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
BEAT = TICKS_PER_QUARTER
BAR = BEAT * 4

AUDITION_MIDI_PATH = Path("tests/midi/guitar-ag-player-articulation-audition.mid")


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


def add_phrase(
    track: Track,
    start: int,
    notes: list[int],
    step_ticks: int,
    duration_ticks: int,
    velocities: list[int] | None = None,
    repeat: int = 1,
    gap_ticks: int = BEAT,
) -> int:
    tick = start

    for repeat_index in range(repeat):
        for index, note in enumerate(notes):
            velocity = velocities[index % len(velocities)] if velocities else 86 + (index % 4) * 6
            track.note(tick, note, duration_ticks, velocity)
            tick += step_ticks

        if repeat_index + 1 < repeat:
            tick += gap_ticks

    return tick


def add_reference_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "1 Picked reference: set Legato Articulation to 0% first")
    track.text(tick, "Same musical material later should lose pick attack as Legato Articulation rises.")

    picked_patterns = [
        [64, 67, 69, 72, 69, 67, 64],
        [59, 62, 64, 67, 64, 62, 59],
        [55, 57, 59, 62, 59, 57, 55],
    ]

    for pattern in picked_patterns:
        tick = add_phrase(track, tick, pattern, int(BEAT * 0.50), int(BEAT * 0.38), [72, 82, 92, 104])
        tick += int(BEAT * 0.75)

    return next_bar(tick + BEAT)


def add_hammer_on_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "2 Hammer-ons: ascending same-string legato candidates")
    track.text(tick, "Best around 30-70% Legato Articulation.")

    patterns = [
        [64, 67, 69, 72],
        [59, 62, 64, 67],
        [55, 57, 59, 62],
        [52, 55, 57, 59],
    ]

    for pattern in patterns:
        tick = add_phrase(track, tick, pattern, int(BEAT * 0.33), int(BEAT * 0.48), [80, 86, 92, 98], repeat=2, gap_ticks=int(BEAT * 0.45))
        tick += int(BEAT * 0.60)

    return next_bar(tick + BEAT)


def add_pull_off_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "3 Pull-offs: descending runs, open-string releases")
    track.text(tick, "Should become softer and grabbier above roughly 20-30%.")

    patterns = [
        [76, 72, 69, 67, 64],
        [72, 69, 67, 64],
        [67, 64, 62, 59],
        [59, 57, 55, 52, 50],
    ]

    for pattern in patterns:
        tick = add_phrase(track, tick, pattern, int(BEAT * 0.29), int(BEAT * 0.44), [104, 92, 84, 78, 72], repeat=2, gap_ticks=int(BEAT * 0.40))
        tick += int(BEAT * 0.65)

    return next_bar(tick + BEAT)


def add_mixed_lick_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "4 Mixed licks: hammer-on/pull-off turns and trills")
    track.text(tick, "Use 50-70% to check longer legato chains without full tapping.")

    licks = [
        [64, 67, 64, 69, 64, 67, 64],
        [67, 69, 67, 72, 69, 67, 64],
        [59, 62, 59, 64, 62, 59, 55],
        [55, 57, 59, 57, 55, 52, 50],
    ]

    for lick in licks:
        tick = add_phrase(track, tick, lick, int(BEAT * 0.25), int(BEAT * 0.39), [82, 90, 78, 96])
        tick += int(BEAT * 0.70)

    return next_bar(tick + BEAT)


def add_string_skipping_arpeggios(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "5 Guitar-like arpeggios: mix picked string changes with same-string legato")
    track.text(tick, "Chords/string changes should remain mostly picked; close same-string moves may articulate.")

    arpeggios = [
        [52, 55, 59, 64, 67, 64, 59, 55],
        [47, 52, 55, 59, 64, 67, 64, 59],
        [45, 52, 57, 61, 64, 69, 64, 61],
        [40, 47, 52, 55, 59, 64, 59, 55],
    ]

    for arpeggio in arpeggios:
        tick = add_phrase(track, tick, arpeggio, int(BEAT * 0.31), int(BEAT * 0.42), [74, 80, 86, 92, 98])
        tick += int(BEAT * 0.55)

    return next_bar(tick + BEAT)


def add_tapping_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "6 Right-hand tapping: high-fret tap and pull-off cycles")
    track.text(tick, "Use 80-100% Legato Articulation. Listen for sharper tap impact and lower pick scrape.")

    tap_cycles = [
        [64, 76, 67, 76, 69, 76, 67, 64],
        [59, 71, 62, 71, 64, 71, 62, 59],
        [52, 64, 55, 64, 59, 64, 55, 52],
        [55, 67, 59, 67, 62, 67, 59, 55],
    ]

    for cycle in tap_cycles:
        tick = add_phrase(track, tick, cycle, int(BEAT * 0.25), int(BEAT * 0.43), [76, 112, 82, 110, 88, 114, 82, 72], repeat=2, gap_ticks=int(BEAT * 0.50))
        tick += int(BEAT * 0.75)

    return next_bar(tick + BEAT)


def add_evh_style_section(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "7 Tap arpeggio flourish: full-style stress test")
    track.text(tick, "Fast wide intervals should favor tap/pull-off behavior at 100%.")

    flourish = [
        64, 76, 71, 76, 67, 76, 71, 76,
        62, 74, 69, 74, 65, 74, 69, 74,
        59, 71, 67, 71, 64, 71, 67, 71,
        55, 67, 62, 67, 59, 67, 62, 67,
    ]

    tick = add_phrase(track, tick, flourish, int(BEAT * 0.20), int(BEAT * 0.35), [74, 118, 88, 112], repeat=1)
    return next_bar(tick + BAR)


def build_track() -> bytes:
    track = Track()
    track.add(0, track_name_payload("Guitar AG player articulation audition"))
    track.add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, byteorder="big"))
    track.add(0, b"\xFF\x58\x04\x04\x02\x18\x08")
    track.text(0, "Audition with Legato Articulation at 0%, 35%, 65%, and 100%.")

    tick = BAR
    tick = add_reference_section(track, tick)
    tick = add_hammer_on_section(track, tick)
    tick = add_pull_off_section(track, tick)
    tick = add_mixed_lick_section(track, tick)
    tick = add_string_skipping_arpeggios(track, tick)
    tick = add_tapping_section(track, tick)
    tick = add_evh_style_section(track, tick)
    track.marker(tick, "End")

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
