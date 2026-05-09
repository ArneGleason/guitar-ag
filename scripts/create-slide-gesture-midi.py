#!/usr/bin/env python3
"""Generate a Guitar AG slide-gesture audition MIDI file."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 100
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
BEAT = TICKS_PER_QUARTER
BAR = BEAT * 4
MPE_BEND_RANGE_SEMITONES = 48.0

AUDITION_MIDI_PATH = Path("tests/midi/guitar-ag-slide-gesture-audition.mid")


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


def controller(channel: int, number: int, value: int) -> bytes:
    return bytes([0xB0 | midi_channel(channel), number, max(0, min(127, value))])


def channel_pressure(channel: int, value: int) -> bytes:
    return bytes([0xD0 | midi_channel(channel), max(0, min(127, value))])


def pitch_wheel(channel: int, normalized: float) -> bytes:
    clamped = max(-1.0, min(1.0, normalized))
    value = round(8192 + clamped * 8191)
    value = max(0, min(16383, value))
    return bytes([0xE0 | midi_channel(channel), value & 0x7F, (value >> 7) & 0x7F])


def marker_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x06]) + varlen(len(encoded)) + encoded


def text_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x01]) + varlen(len(encoded)) + encoded


def track_name_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xFF, 0x03]) + varlen(len(encoded)) + encoded


def pitch_norm(semitones: float) -> float:
    return semitones / MPE_BEND_RANGE_SEMITONES


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

    def mpe_chord(
        self,
        start: int,
        notes_and_channels: list[tuple[int, int, int]],
        duration: int,
        strum_ticks: int = 0,
    ) -> None:
        for index, (note, channel, velocity) in enumerate(notes_and_channels):
            offset = index * strum_ticks
            self.note(start + offset, note, duration - offset, velocity, channel)

    def ramp_pitch_semitones(
        self,
        start: int,
        duration: int,
        channel: int,
        start_semitones: float,
        end_semitones: float,
        steps: int = 24,
    ) -> None:
        for step in range(steps + 1):
            fraction = step / steps
            semitones = start_semitones + (end_semitones - start_semitones) * fraction
            self.add(start + round(duration * fraction), pitch_wheel(channel, pitch_norm(semitones)))

    def ramp_channel_pressure(
        self,
        start: int,
        duration: int,
        channel: int,
        start_value: int,
        end_value: int,
        steps: int = 12,
    ) -> None:
        for step in range(steps + 1):
            fraction = step / steps
            value = round(start_value + (end_value - start_value) * fraction)
            self.add(start + round(duration * fraction), channel_pressure(channel, value))

    def ramp_controller(
        self,
        start: int,
        duration: int,
        channel: int,
        number: int,
        start_value: int,
        end_value: int,
        steps: int = 12,
    ) -> None:
        for step in range(steps + 1):
            fraction = step / steps
            value = round(start_value + (end_value - start_value) * fraction)
            self.add(start + round(duration * fraction), controller(channel, number, value))

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


def reset_expression(track: Track, tick: int, channels: range = range(1, 9)) -> None:
    for channel in channels:
        track.add(tick, pitch_wheel(channel, 0.0))
        track.add(tick, channel_pressure(channel, 0))
        track.add(tick, controller(channel, 1, 0))
        track.add(tick, controller(channel, 74, 0))


def add_single_slide_to(track: Track, start: int) -> int:
    tick = start
    channel = 2
    track.marker(tick, "1 Slide To: one held note ramps up seven semitones and settles")
    track.text(tick, "MPE pitch bend curve on channel 2. Setup: MPE Mode On, MPE Bend Range 48 st.")
    reset_expression(track, tick)
    track.note(tick, 55, BAR * 4, 92, channel)
    track.ramp_pitch_semitones(tick + BEAT, BEAT * 2, channel, 0.0, 7.0, steps=36)
    track.ramp_pitch_semitones(tick + BAR * 3, BEAT, channel, 7.0, 7.0, steps=1)
    reset_expression(track, tick + BAR * 4)
    return next_bar(tick + BAR * 5)


def add_slide_ins(track: Track, start: int) -> int:
    tick = start
    channel = 2
    track.marker(tick, "2 Slide In: indefinite approach pitches into written notes")
    reset_expression(track, tick)

    gestures = [
        (57, -5.0, 92),
        (60, 4.0, 88),
        (62, -2.0, 96),
        (64, 7.0, 90),
    ]

    for note, start_bend, velocity in gestures:
        track.add(tick, pitch_wheel(channel, pitch_norm(start_bend)))
        track.note(tick, note, int(BEAT * 1.6), velocity, channel)
        track.ramp_pitch_semitones(tick, int(BEAT * 0.55), channel, start_bend, 0.0, steps=14)
        track.add(tick + int(BEAT * 1.7), pitch_wheel(channel, 0.0))
        tick += BEAT * 2

    reset_expression(track, tick)
    return next_bar(tick + BEAT)


def add_slide_throw(track: Track, start: int) -> int:
    tick = start
    channel = 2
    track.marker(tick, "3 Slide Throw: fast up-neck excursion and return on a held note")
    reset_expression(track, tick)
    track.note(tick, 59, BAR * 5, 98, channel)
    track.ramp_pitch_semitones(tick + int(BEAT * 0.75), BEAT, channel, 0.0, 12.0, steps=24)
    track.ramp_pitch_semitones(tick + int(BEAT * 1.75), int(BEAT * 1.25), channel, 12.0, 0.0, steps=28)
    track.ramp_pitch_semitones(tick + BAR * 3, int(BEAT * 0.75), channel, 0.0, -5.0, steps=16)
    track.ramp_pitch_semitones(tick + BAR * 3 + int(BEAT * 0.75), BEAT, channel, -5.0, 0.0, steps=20)
    reset_expression(track, tick + BAR * 5)
    return next_bar(tick + BAR * 6)


def add_slide_outs(track: Track, start: int) -> int:
    tick = start
    channel = 2
    track.marker(tick, "4 Slide Out: held notes move away before release")
    reset_expression(track, tick)

    track.note(tick, 57, BAR * 2, 92, channel)
    track.ramp_pitch_semitones(tick + BEAT, int(BEAT * 0.85), channel, 0.0, -12.0, steps=20)
    track.add(tick + BAR * 2 + int(BEAT * 0.10), pitch_wheel(channel, 0.0))

    second = tick + BAR * 3
    track.note(second, 64, BAR * 2, 94, channel)
    track.ramp_pitch_semitones(second + BEAT, int(BEAT * 0.70), channel, 0.0, 12.0, steps=18)
    track.add(second + BAR * 2 + int(BEAT * 0.10), pitch_wheel(channel, 0.0))

    reset_expression(track, second + BAR * 2 + BEAT)
    return next_bar(second + BAR * 3)


def add_independent_chord_slide(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "5 Independent MPE slide: only the top note of the chord moves")
    reset_expression(track, tick)
    notes = [(52, 2, 86), (55, 3, 84), (59, 4, 86)]
    track.mpe_chord(tick, notes, BAR * 5, strum_ticks=18)
    track.ramp_pitch_semitones(tick + BEAT, BEAT * 2, 4, 0.0, 5.0, steps=36)
    track.ramp_pitch_semitones(tick + BAR * 3, BEAT, 4, 5.0, 0.0, steps=24)
    reset_expression(track, tick + BAR * 5)
    return next_bar(tick + BAR * 6)


def add_manual_chord_slide_proxy(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "6 Chord slide proxy: identical MPE bends on all held chord tones")
    track.text(tick, "This stands in for the planned global Slide Offset lane until Phase 2 exists.")
    reset_expression(track, tick)
    notes = [(45, 2, 90), (52, 3, 86), (57, 4, 84), (61, 5, 82)]
    track.mpe_chord(tick, notes, BAR * 6, strum_ticks=20)

    for channel in range(2, 6):
        track.ramp_pitch_semitones(tick + BEAT, BEAT * 2, channel, 0.0, 5.0, steps=32)
        track.ramp_pitch_semitones(tick + BAR * 3, BEAT, channel, 5.0, -2.0, steps=20)
        track.ramp_pitch_semitones(tick + BAR * 4, BEAT, channel, -2.0, 0.0, steps=20)

    reset_expression(track, tick + BAR * 6)
    return next_bar(tick + BAR * 7)


def add_slide_with_expression(track: Track, start: int) -> int:
    tick = start
    channel = 2
    track.marker(tick, "7 Slide plus expression: pitch motion with pressure and CC74")
    track.text(tick, "Pressure and CC74 are not guitar slide; this checks that they can layer with pitch slides.")
    reset_expression(track, tick)
    track.note(tick, 64, BAR * 5, 96, channel)
    track.ramp_pitch_semitones(tick + BEAT, BEAT * 2, channel, 0.0, 7.0, steps=36)
    track.ramp_channel_pressure(tick + int(BEAT * 0.75), BAR * 2, channel, 0, 110, steps=20)
    track.ramp_controller(tick + BEAT, BAR * 2, channel, 74, 0, 96, steps=20)
    track.ramp_pitch_semitones(tick + BAR * 3, BEAT, channel, 7.0, 0.0, steps=24)
    track.ramp_channel_pressure(tick + BAR * 3, BEAT, channel, 110, 0, steps=12)
    track.ramp_controller(tick + BAR * 3, BEAT, channel, 74, 96, 0, steps=12)
    reset_expression(track, tick + BAR * 5)
    track.marker(tick + BAR * 6, "End")
    return tick + BAR * 6


def build_track() -> bytes:
    track = Track()
    track.add(0, track_name_payload("Guitar AG slide gesture audition"))
    track.add(0, b"\xFF\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, byteorder="big"))
    track.add(0, b"\xFF\x58\x04\x04\x02\x18\x08")
    track.text(0, "Suggested setup: MPE Mode On, MPE Bend Range 48 st, Pressure/CC74 amounts nonzero for section 7.")
    reset_expression(track, 0)

    tick = BAR
    tick = add_single_slide_to(track, tick)
    tick = add_slide_ins(track, tick)
    tick = add_slide_throw(track, tick)
    tick = add_slide_outs(track, tick)
    tick = add_independent_chord_slide(track, tick)
    tick = add_manual_chord_slide_proxy(track, tick)
    add_slide_with_expression(track, tick)

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
