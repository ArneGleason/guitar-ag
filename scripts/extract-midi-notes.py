#!/usr/bin/env python3
"""Extract MIDI note events to CSV without third-party dependencies."""

from __future__ import annotations

import argparse
import csv
import struct
from collections import defaultdict, deque
from dataclasses import dataclass
from pathlib import Path


@dataclass
class TempoChange:
    tick: int
    seconds: float
    microseconds_per_quarter: int


@dataclass
class NoteStart:
    tick: int
    track: int
    channel: int
    note: int
    velocity: int


@dataclass
class NoteEvent:
    track: int
    channel: int
    note: int
    velocity: int
    start_tick: int
    end_tick: int


def read_varlen(data: bytes, offset: int) -> tuple[int, int]:
    value = 0

    while True:
        byte = data[offset]
        offset += 1
        value = (value << 7) | (byte & 0x7f)

        if not byte & 0x80:
            return value, offset


def ticks_to_seconds(tick: int, tempo_changes: list[TempoChange], ticks_per_quarter: int) -> float:
    current = tempo_changes[0]

    for change in tempo_changes[1:]:
        if change.tick > tick:
            break

        current = change

    ticks_after_change = tick - current.tick
    seconds_per_tick = current.microseconds_per_quarter / 1_000_000.0 / ticks_per_quarter
    return current.seconds + ticks_after_change * seconds_per_tick


def parse_track(track_data: bytes, track_index: int) -> tuple[list[NoteEvent], list[tuple[int, int]]]:
    offset = 0
    tick = 0
    running_status = None
    active: dict[tuple[int, int], deque[NoteStart]] = defaultdict(deque)
    notes: list[NoteEvent] = []
    tempos: list[tuple[int, int]] = []

    while offset < len(track_data):
        delta, offset = read_varlen(track_data, offset)
        tick += delta

        status = track_data[offset]

        if status < 0x80:
            if running_status is None:
                raise ValueError(f"running status used before status byte in track {track_index}")
            status = running_status
        else:
            offset += 1

            if status < 0xf0:
                running_status = status

        if status == 0xff:
            meta_type = track_data[offset]
            offset += 1
            length, offset = read_varlen(track_data, offset)
            payload = track_data[offset:offset + length]
            offset += length

            if meta_type == 0x51 and length == 3:
                tempos.append((tick, int.from_bytes(payload, byteorder="big")))

            if meta_type == 0x2f:
                break

            continue

        if status in (0xf0, 0xf7):
            length, offset = read_varlen(track_data, offset)
            offset += length
            continue

        event_type = status & 0xf0
        channel = (status & 0x0f) + 1

        data_lengths = {
            0x80: 2,
            0x90: 2,
            0xa0: 2,
            0xb0: 2,
            0xc0: 1,
            0xd0: 1,
            0xe0: 2,
        }
        data_length = data_lengths.get(event_type)

        if data_length is None:
            raise ValueError(f"unsupported MIDI status 0x{status:02x} in track {track_index}")

        payload = track_data[offset:offset + data_length]
        offset += data_length

        if event_type == 0x90 and payload[1] > 0:
            active[(channel, payload[0])].append(
                NoteStart(tick=tick, track=track_index, channel=channel, note=payload[0], velocity=payload[1]))
        elif event_type == 0x80 or (event_type == 0x90 and payload[1] == 0):
            key = (channel, payload[0])

            if active[key]:
                start = active[key].popleft()
                notes.append(NoteEvent(track=start.track,
                                       channel=start.channel,
                                       note=start.note,
                                       velocity=start.velocity,
                                       start_tick=start.tick,
                                       end_tick=tick))

    return notes, tempos


def parse_midi(path: Path) -> tuple[int, list[NoteEvent], list[TempoChange]]:
    data = path.read_bytes()
    offset = 0

    if data[offset:offset + 4] != b"MThd":
        raise ValueError("missing MThd header")

    offset += 4
    header_length = struct.unpack(">I", data[offset:offset + 4])[0]
    offset += 4
    midi_format, track_count, division = struct.unpack(">HHH", data[offset:offset + 6])
    offset += header_length

    if division & 0x8000:
        raise ValueError("SMPTE MIDI timing is not supported")

    all_notes: list[NoteEvent] = []
    raw_tempos = [(0, 500000)]

    for track_index in range(track_count):
        if data[offset:offset + 4] != b"MTrk":
            raise ValueError(f"missing MTrk header for track {track_index}")

        offset += 4
        track_length = struct.unpack(">I", data[offset:offset + 4])[0]
        offset += 4
        track_data = data[offset:offset + track_length]
        offset += track_length

        notes, tempos = parse_track(track_data, track_index)
        all_notes.extend(notes)
        raw_tempos.extend(tempos)

    raw_tempos = sorted(set(raw_tempos), key=lambda item: item[0])
    tempo_changes: list[TempoChange] = []
    last_tick = 0
    seconds = 0.0
    current_tempo = raw_tempos[0][1]

    for tick, tempo in raw_tempos:
        if tick > last_tick:
            seconds += (tick - last_tick) * current_tempo / 1_000_000.0 / division
        tempo_changes.append(TempoChange(tick=tick, seconds=seconds, microseconds_per_quarter=tempo))
        last_tick = tick
        current_tempo = tempo

    all_notes.sort(key=lambda note: (note.start_tick, note.track, note.channel, note.note))
    return division, all_notes, tempo_changes


def midi_note_name(note: int) -> str:
    names = ("C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B")
    octave = note // 12 - 1
    return f"{names[note % 12]}{octave}"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("midi_file", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    midi_file = args.midi_file.expanduser().resolve()
    output = args.output.expanduser().resolve()
    ticks_per_quarter, notes, tempo_changes = parse_midi(midi_file)

    output.parent.mkdir(parents=True, exist_ok=True)

    with output.open("w", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=[
            "track",
            "channel",
            "note",
            "note_name",
            "velocity",
            "start_tick",
            "end_tick",
            "duration_ticks",
            "start_seconds",
            "end_seconds",
            "duration_seconds",
        ])
        writer.writeheader()

        for note in notes:
            start_seconds = ticks_to_seconds(note.start_tick, tempo_changes, ticks_per_quarter)
            end_seconds = ticks_to_seconds(note.end_tick, tempo_changes, ticks_per_quarter)
            writer.writerow({
                "track": note.track,
                "channel": note.channel,
                "note": note.note,
                "note_name": midi_note_name(note.note),
                "velocity": note.velocity,
                "start_tick": note.start_tick,
                "end_tick": note.end_tick,
                "duration_ticks": note.end_tick - note.start_tick,
                "start_seconds": f"{start_seconds:.6f}",
                "end_seconds": f"{end_seconds:.6f}",
                "duration_seconds": f"{end_seconds - start_seconds:.6f}",
            })

    print(f"Extracted {len(notes)} note events")
    print(f"Wrote {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
