#!/usr/bin/env python3
"""Generate a Guitar AG feature-audition MIDI file."""

from __future__ import annotations

import struct
from pathlib import Path


TICKS_PER_QUARTER = 480
BPM = 120
MICROSECONDS_PER_QUARTER = 60_000_000 // BPM
BEAT = TICKS_PER_QUARTER
BAR = BEAT * 4
MPE_BEND_RANGE_SEMITONES = 48.0

AUDITION_MIDI_PATH = Path("tests/midi/guitar-ag-feature-audition.mid")


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


def delta_event(delta_ticks: int, payload: bytes) -> bytes:
    return varlen(delta_ticks) + payload


def midi_channel(channel: int) -> int:
    if channel < 1 or channel > 16:
        raise ValueError(f"MIDI channel must be 1-16, got {channel}")

    return channel - 1


def note_on(note: int, velocity: int, channel: int = 1) -> bytes:
    return bytes([0x90 | midi_channel(channel), note, velocity])


def note_off(note: int, channel: int = 1) -> bytes:
    return bytes([0x80 | midi_channel(channel), note, 64])


def controller(channel: int, number: int, value: int) -> bytes:
    return bytes([0xb0 | midi_channel(channel), number, max(0, min(127, value))])


def poly_aftertouch(channel: int, note: int, value: int) -> bytes:
    return bytes([0xa0 | midi_channel(channel), note, max(0, min(127, value))])


def channel_pressure(channel: int, value: int) -> bytes:
    return bytes([0xd0 | midi_channel(channel), max(0, min(127, value))])


def pitch_wheel(channel: int, normalized: float) -> bytes:
    clamped = max(-1.0, min(1.0, normalized))
    value = round(8192 + clamped * 8191)
    value = max(0, min(16383, value))
    return bytes([0xe0 | midi_channel(channel), value & 0x7f, (value >> 7) & 0x7f])


def marker_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xff, 0x06]) + varlen(len(encoded)) + encoded


def text_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xff, 0x01]) + varlen(len(encoded)) + encoded


def track_name_payload(text: str) -> bytes:
    encoded = text.encode("ascii", errors="replace")
    return bytes([0xff, 0x03]) + varlen(len(encoded)) + encoded


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

    def chord(
        self,
        start: int,
        notes: list[int],
        duration: int,
        velocity: int,
        channel: int = 1,
        strum_ticks: int = 0,
    ) -> None:
        for index, note in enumerate(notes):
            offset = index * strum_ticks
            self.note(start + offset, note, duration, velocity, channel)

    def mpe_chord(
        self,
        start: int,
        notes_and_channels: list[tuple[int, int, int]],
        duration: int,
        strum_ticks: int = 0,
    ) -> None:
        for index, (note, channel, velocity) in enumerate(notes_and_channels):
            offset = index * strum_ticks
            self.note(start + offset, note, duration, velocity, channel)

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

    def ramp_poly_aftertouch(
        self,
        start: int,
        duration: int,
        channel: int,
        note: int,
        start_value: int,
        end_value: int,
        steps: int = 12,
    ) -> None:
        for step in range(steps + 1):
            fraction = step / steps
            value = round(start_value + (end_value - start_value) * fraction)
            self.add(start + round(duration * fraction), poly_aftertouch(channel, note, value))

    def ramp_pitch_semitones(
        self,
        start: int,
        duration: int,
        channel: int,
        start_semitones: float,
        end_semitones: float,
        steps: int = 16,
    ) -> None:
        for step in range(steps + 1):
            fraction = step / steps
            semitones = start_semitones + (end_semitones - start_semitones) * fraction
            self.add(start + round(duration * fraction),
                     pitch_wheel(channel, semitones / MPE_BEND_RANGE_SEMITONES))

    def render(self) -> bytes:
        data = bytearray()
        previous_tick = 0

        for tick, _, payload in sorted(self.events, key=lambda item: (item[0], item[1])):
            data += delta_event(tick - previous_tick, payload)
            previous_tick = tick

        data += delta_event(0, b"\xff\x2f\x00")
        return bytes(data)


def next_bar(tick: int) -> int:
    return ((tick + BAR - 1) // BAR) * BAR


def reset_expression(track: Track, tick: int, channels: range = range(1, 9)) -> None:
    for channel in channels:
        track.add(tick, pitch_wheel(channel, 0.0))
        track.add(tick, channel_pressure(channel, 0))
        track.add(tick, controller(channel, 1, 0))
        track.add(tick, controller(channel, 74, 0))


def add_open_string_ladder(track: Track, start: int) -> int:
    open_strings = [(40, "E2"), (45, "A2"), (50, "D3"), (55, "G3"), (59, "B3"), (64, "E4")]
    tick = start

    track.marker(tick, "1 Open strings: wound/plain identity and velocity response")

    for note, name in open_strings:
        track.text(tick, f"{name}: soft then assertive pick")
        track.note(tick, note, int(BEAT * 0.75), 42)
        tick += BEAT
        track.note(tick, note, int(BEAT * 1.15), 112)
        tick += int(BEAT * 1.55)

    return next_bar(tick + BEAT)


def add_strums_and_arpeggios(track: Track, start: int) -> int:
    tick = start
    progression = [
        ("Em open voicing", [40, 47, 52, 55, 59, 64], 84),
        ("G open voicing", [43, 47, 50, 55, 59, 67], 82),
        ("D partial voicing", [50, 57, 62, 66], 82),
        ("A partial voicing", [45, 52, 57, 61, 64], 86),
    ]

    track.marker(tick, "2 Strummed chords and arpeggios: polyphony, mapping, sustain")

    for name, notes, velocity in progression:
        track.text(tick, name)
        track.chord(tick, notes, int(BEAT * 1.15), velocity, strum_ticks=24)

        arpeggio_start = tick + int(BEAT * 1.55)
        for index, note in enumerate(notes):
            track.note(arpeggio_start + index * int(BEAT * 0.32), note, int(BEAT * 0.55), 70 + index * 4)

        tick += BAR * 2

    return next_bar(tick)


def add_short_riff(track: Track, start: int) -> int:
    tick = start
    riff = [
        (40, 0.35, 96), (40, 0.20, 70), (43, 0.45, 104), (45, 0.35, 92),
        (47, 0.30, 108), (45, 0.25, 76), (43, 0.35, 94), (40, 0.65, 112),
        (52, 0.30, 86), (55, 0.30, 88), (57, 0.30, 96), (59, 0.30, 102),
        (57, 0.25, 78), (55, 0.25, 82), (52, 0.45, 100), (47, 0.75, 92),
    ]

    track.marker(tick, "3 Short picked riff: release damping and fast note handling")

    for note, beats, velocity in riff:
        duration = max(80, int(BEAT * beats))
        track.note(tick, note, duration, velocity)
        tick += int(BEAT * 0.50)

    return next_bar(tick + BEAT)


def add_mod_wheel_vibrato(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "4 CC1 vibrato ramp: enable Mod Wheel To Depth/Speed in the VST")
    track.note(tick, 64, BAR * 4, 92)
    track.ramp_controller(tick, BAR * 3, 1, 1, 0, 112, steps=18)
    track.ramp_controller(tick + BAR * 3, BEAT * 2, 1, 1, 112, 0, steps=8)
    return next_bar(tick + BAR * 5)


def add_poly_aftertouch(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "5 Key/poly aftertouch: bend only the top note of a held chord")
    chord_notes = [52, 59, 64]
    track.chord(tick, chord_notes, BAR * 4, 82, strum_ticks=18)
    track.ramp_poly_aftertouch(tick + BEAT, BAR, 1, 64, 0, 112, steps=12)
    track.ramp_poly_aftertouch(tick + BAR * 2, BAR, 1, 64, 112, 0, steps=12)
    return next_bar(tick + BAR * 5)


def add_mpe_independent_bend(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "6 MPE pitch bend: bend the middle note while the chord holds")
    reset_expression(track, tick)
    notes = [(52, 2, 86), (55, 3, 84), (59, 4, 82)]
    track.mpe_chord(tick, notes, BAR * 5, strum_ticks=16)
    track.ramp_pitch_semitones(tick + BEAT, BAR, 3, 0.0, 2.0, steps=18)
    track.ramp_pitch_semitones(tick + BAR * 2, BAR, 3, 2.0, -1.0, steps=18)
    track.ramp_pitch_semitones(tick + BAR * 3, BAR, 3, -1.0, 0.0, steps=18)
    reset_expression(track, tick + BAR * 5)
    return next_bar(tick + BAR * 6)


def add_mpe_pressure_and_cc74(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "7 MPE pressure and CC74: pressure one note, brighten another")
    reset_expression(track, tick)
    notes = [(52, 2, 82), (59, 3, 82), (64, 4, 82)]
    track.mpe_chord(tick, notes, BAR * 5, strum_ticks=18)
    track.ramp_channel_pressure(tick + BEAT, BAR * 2, 3, 0, 118, steps=18)
    track.ramp_controller(tick + BEAT * 2, BAR * 2, 4, 74, 0, 118, steps=18)
    track.ramp_channel_pressure(tick + BAR * 3, BAR, 3, 118, 0, steps=12)
    track.ramp_controller(tick + BAR * 3, BAR, 4, 74, 118, 0, steps=12)
    reset_expression(track, tick + BAR * 5)
    return next_bar(tick + BAR * 6)


def add_mpe_lead_bends(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "8 MPE lead bends: slide-ins, small bends, and releases")
    reset_expression(track, tick)

    phrases = [
        (64, -1.0, 0.0, 92),
        (67, 0.0, 1.5, 98),
        (69, 0.0, 0.0, 84),
        (71, -0.5, 0.0, 90),
        (72, 0.0, 2.0, 106),
        (76, 0.0, -1.0, 96),
    ]

    for note, start_bend, end_bend, velocity in phrases:
        track.add(tick, pitch_wheel(2, start_bend / MPE_BEND_RANGE_SEMITONES))
        track.note(tick, note, int(BEAT * 1.15), velocity, channel=2)
        track.ramp_pitch_semitones(tick, int(BEAT * 0.35), 2, start_bend, 0.0, steps=8)

        if end_bend != 0.0:
            track.ramp_pitch_semitones(tick + int(BEAT * 0.45), int(BEAT * 0.45), 2, 0.0, end_bend, steps=8)
            track.ramp_pitch_semitones(tick + int(BEAT * 0.90), int(BEAT * 0.25), 2, end_bend, 0.0, steps=6)

        track.add(tick + int(BEAT * 1.20), pitch_wheel(2, 0.0))
        tick += int(BEAT * 1.35)

    reset_expression(track, tick)
    return next_bar(tick + BAR)


def add_finale(track: Track, start: int) -> int:
    tick = start
    track.marker(tick, "9 Finale: wide MPE chord with long sustain tail")
    reset_expression(track, tick)
    notes = [(40, 2, 88), (47, 3, 82), (52, 4, 80), (59, 5, 78), (64, 6, 84), (67, 7, 76)]
    track.mpe_chord(tick, notes, BAR * 5, strum_ticks=28)
    track.ramp_channel_pressure(tick + BAR, BAR * 2, 6, 0, 92, steps=12)
    track.ramp_controller(tick + BAR, BAR * 2, 7, 74, 0, 96, steps=12)
    reset_expression(track, tick + BAR * 5)
    track.marker(tick + BAR * 6, "End")
    return tick + BAR * 6


def build_track() -> bytes:
    track = Track()
    track.add(0, track_name_payload("Guitar AG feature audition"))
    track.add(0, b"\xff\x51\x03" + MICROSECONDS_PER_QUARTER.to_bytes(3, byteorder="big"))
    track.add(0, b"\xff\x58\x04\x04\x02\x18\x08")
    track.text(0, "Suggested setup: MPE Mode On, MPE Bend Range 48, Pressure/CC74 amounts nonzero.")
    reset_expression(track, 0)

    tick = BAR
    tick = add_open_string_ladder(track, tick)
    tick = add_strums_and_arpeggios(track, tick)
    tick = add_short_riff(track, tick)
    tick = add_mod_wheel_vibrato(track, tick)
    tick = add_poly_aftertouch(track, tick)
    tick = add_mpe_independent_bend(track, tick)
    tick = add_mpe_pressure_and_cc74(track, tick)
    tick = add_mpe_lead_bends(track, tick)
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
