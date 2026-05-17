#include "FretboardMapper.h"

#include <algorithm>
#include <limits>

namespace guitar_ag
{

void FretboardMapper::reset() noexcept
{
    for (auto& activeString : activeStrings)
        activeString = {};

    openNotes = standardOpenNotes;
    positionFret = 2.0f;
}

FretboardAssignment FretboardMapper::assignNote (int midiNoteNumber,
                                                 int midiChannel,
                                                 int preferredStringIndex,
                                                 float preferredStringBonus,
                                                 bool allowPreferredOccupied) noexcept
{
    if (midiNoteNumber < openNotes[0])
        applyDropTuning (midiNoteNumber);

    const auto candidate = findBestCandidate (midiNoteNumber,
                                             preferredStringIndex,
                                             preferredStringBonus,
                                             allowPreferredOccupied);
    const FretboardAssignment assignment { candidate.stringIndex,
                                           candidate.fret,
                                           candidate.wound,
                                           candidate.woundAmount };

    rememberAssignment (assignment, midiNoteNumber, midiChannel);
    updatePositionMemory (assignment.fret);

    return assignment;
}

void FretboardMapper::assignNoteGroup (const int* midiNoteNumbers,
                                       const int* midiChannels,
                                       FretboardAssignment* assignments,
                                       int noteCount) noexcept
{
    if (midiNoteNumbers == nullptr || midiChannels == nullptr || assignments == nullptr || noteCount <= 0)
        return;

    const auto clampedNoteCount = std::min (noteCount, stringCount);
    auto lowestNote = midiNoteNumbers[0];

    for (auto noteIndex = 1; noteIndex < clampedNoteCount; ++noteIndex)
        lowestNote = std::min (lowestNote, midiNoteNumbers[noteIndex]);

    if (lowestNote < openNotes[0])
        applyDropTuning (lowestNote);

    std::array<std::array<Candidate, stringCount>, stringCount> candidates {};
    std::array<int, stringCount> candidateCounts {};

    for (auto noteIndex = 0; noteIndex < clampedNoteCount; ++noteIndex)
    {
        for (auto stringIndex = 0; stringIndex < stringCount; ++stringIndex)
        {
            const auto fret = midiNoteNumbers[noteIndex] - openNotes[static_cast<size_t> (stringIndex)];

            if (fret < 0 || fret > maxFret)
                continue;

            const auto candidateIndex = candidateCounts[static_cast<size_t> (noteIndex)]++;
            candidates[static_cast<size_t> (noteIndex)][static_cast<size_t> (candidateIndex)] = {
                stringIndex,
                fret,
                woundStrings[static_cast<size_t> (stringIndex)],
                woundAmounts[static_cast<size_t> (stringIndex)],
                scoreCandidate (midiNoteNumbers[noteIndex], stringIndex, fret, -1, 0.0f, false)
            };
        }

        if (candidateCounts[static_cast<size_t> (noteIndex)] == 0)
        {
            for (auto fallbackIndex = 0; fallbackIndex < clampedNoteCount; ++fallbackIndex)
                assignments[fallbackIndex] = assignNote (midiNoteNumbers[fallbackIndex], midiChannels[fallbackIndex]);

            return;
        }
    }

    std::array<Candidate, stringCount> current {};
    std::array<Candidate, stringCount> best {};
    std::array<bool, stringCount> usedStrings {};
    auto bestScore = std::numeric_limits<float>::max();

    const auto search = [&] (const auto& self, int noteIndex, float runningScore) noexcept -> void
    {
        if (noteIndex == clampedNoteCount)
        {
            if (runningScore < bestScore)
            {
                bestScore = runningScore;
                best = current;
            }

            return;
        }

        const auto count = candidateCounts[static_cast<size_t> (noteIndex)];

        for (auto candidateIndex = 0; candidateIndex < count; ++candidateIndex)
        {
            const auto candidate = candidates[static_cast<size_t> (noteIndex)][static_cast<size_t> (candidateIndex)];
            const auto stringIndex = static_cast<size_t> (candidate.stringIndex);

            if (usedStrings[stringIndex])
                continue;

            const auto nextScore = runningScore + candidate.score;

            if (nextScore >= bestScore)
                continue;

            usedStrings[stringIndex] = true;
            current[static_cast<size_t> (noteIndex)] = candidate;
            self (self, noteIndex + 1, nextScore);
            usedStrings[stringIndex] = false;
        }
    };

    search (search, 0, 0.0f);

    if (bestScore == std::numeric_limits<float>::max())
    {
        for (auto fallbackIndex = 0; fallbackIndex < clampedNoteCount; ++fallbackIndex)
            assignments[fallbackIndex] = assignNote (midiNoteNumbers[fallbackIndex], midiChannels[fallbackIndex]);

        return;
    }

    for (auto noteIndex = 0; noteIndex < clampedNoteCount; ++noteIndex)
    {
        const auto candidate = best[static_cast<size_t> (noteIndex)];
        assignments[noteIndex] = { candidate.stringIndex,
                                   candidate.fret,
                                   candidate.wound,
                                   candidate.woundAmount };
        rememberAssignment (assignments[noteIndex], midiNoteNumbers[noteIndex], midiChannels[noteIndex]);
        updatePositionMemory (assignments[noteIndex].fret);
    }
}

int FretboardMapper::getFretForString (int midiNoteNumber, int stringIndex) const noexcept
{
    auto clampedString = stringIndex;

    if (clampedString < 0)
        clampedString = 0;
    else if (clampedString >= stringCount)
        clampedString = stringCount - 1;

    const auto fret = midiNoteNumber - openNotes[static_cast<size_t> (clampedString)];

    return fret >= 0 && fret <= maxFret ? fret : -1;
}

std::array<FretboardMapper::StringState, FretboardMapper::stringCount> FretboardMapper::getStringStates() const noexcept
{
    std::array<StringState, stringCount> states {};

    for (auto stringIndex = 0; stringIndex < stringCount; ++stringIndex)
    {
        const auto openNote = openNotes[static_cast<size_t> (stringIndex)];
        const auto& activeString = activeStrings[static_cast<size_t> (stringIndex)];
        auto& state = states[static_cast<size_t> (stringIndex)];

        state.openNote = openNote;

        if (! activeString.active)
            continue;

        state.active = true;
        state.noteNumber = activeString.noteNumber;
        state.channel = activeString.channel;
        state.fret = activeString.noteNumber - openNote;
    }

    return states;
}

void FretboardMapper::applyDropTuning (int midiNoteNumber) noexcept
{
    // Tune string 0 down to the exact requested note so it lands at fret 0.
    // Persists until reset() — mirrors how a guitarist retunes and stays there.
    if (midiNoteNumber < openNotes[0])
        openNotes[0] = midiNoteNumber;
}

void FretboardMapper::releaseNote (int midiNoteNumber, int midiChannel) noexcept
{
    for (auto& activeString : activeStrings)
    {
        if (activeString.active && activeString.noteNumber == midiNoteNumber && activeString.channel == midiChannel)
            activeString = {};
    }
}

void FretboardMapper::releaseString (int stringIndex) noexcept
{
    if (stringIndex < 0 || stringIndex >= stringCount)
        return;

    activeStrings[static_cast<size_t> (stringIndex)] = {};
}

FretboardMapper::Candidate FretboardMapper::findBestCandidate (int midiNoteNumber,
                                                               int preferredStringIndex,
                                                               float preferredStringBonus,
                                                               bool allowPreferredOccupied) const noexcept
{
    Candidate best {};
    best.score = std::numeric_limits<float>::max();

    for (auto stringIndex = 0; stringIndex < stringCount; ++stringIndex)
    {
        const auto fret = midiNoteNumber - openNotes[static_cast<size_t> (stringIndex)];

        if (fret < 0 || fret > maxFret)
            continue;

        const auto score = scoreCandidate (midiNoteNumber,
                                           stringIndex,
                                           fret,
                                           preferredStringIndex,
                                           preferredStringBonus,
                                           allowPreferredOccupied);

        if (score < best.score)
            best = { stringIndex,
                     fret,
                     woundStrings[static_cast<size_t> (stringIndex)],
                     woundAmounts[static_cast<size_t> (stringIndex)],
                     score };
    }

    if (best.score < std::numeric_limits<float>::max())
        return best;

    if (midiNoteNumber < openNotes[0])
        return { 0, 0, woundStrings.front(), woundAmounts.front(), 0.0f };

    return { stringCount - 1, maxFret, woundStrings.back(), woundAmounts.back(), 0.0f };
}

float FretboardMapper::scoreCandidate (int midiNoteNumber,
                                       int stringIndex,
                                       int fret,
                                       int preferredStringIndex,
                                       float preferredStringBonus,
                                       bool allowPreferredOccupied) const noexcept
{
    const auto fretDistance = static_cast<float> (fret) - positionFret;
    auto score = fretDistance * fretDistance;

    score += static_cast<float> (fret) * 0.030f;

    // Register affinity: reward notes landing on strings that match their pitch register.
    // Low notes prefer low strings; high notes prefer high strings. This reduces spread
    // voicings for partial chords on lower/upper strings without overriding strong
    // position-memory preferences (weight 2.5 stays below a typical 4-fret memory pull).
    const auto guitarRangeLow  = static_cast<float> (openNotes[0]);
    const auto guitarRangeHigh = static_cast<float> (openNotes[stringCount - 1] + maxFret);
    const auto noteNorm   = (static_cast<float> (midiNoteNumber) - guitarRangeLow) / (guitarRangeHigh - guitarRangeLow);
    const auto stringNorm = static_cast<float> (stringIndex) / static_cast<float> (stringCount - 1);
    const auto registerMismatch = noteNorm - stringNorm;
    score += registerMismatch * registerMismatch * 2.5f;

    if (fret == 0)
        score -= 0.12f;

    if (fret >= 1 && fret <= 5)
        score -= 0.18f;

    if (midiNoteNumber <= openNotes[2] + 2
        && fret >= 5
        && stringIndex != preferredStringIndex)
    {
        auto hasOpenPositionAlternative = false;

        for (auto alternateString = 0; alternateString < stringCount; ++alternateString)
        {
            if (alternateString == stringIndex)
                continue;

            const auto alternateFret = midiNoteNumber - openNotes[static_cast<size_t> (alternateString)];

            if (alternateFret >= 0 && alternateFret <= 3)
            {
                hasOpenPositionAlternative = true;
                break;
            }
        }

        // Keep bass-register notes such as A2-E3 on their natural low-fret
        // strings after high-position phrases, unless a chord preview already
        // chose this string explicitly.
        if (hasOpenPositionAlternative)
            score += 160.0f;
    }

    if (stringIndex == preferredStringIndex && preferredStringBonus > 0.0f)
        score -= preferredStringBonus;

    if (isStringOccupied (stringIndex) && ! (allowPreferredOccupied && stringIndex == preferredStringIndex))
        score += 1000.0f;

    return score;
}

bool FretboardMapper::isStringOccupied (int stringIndex) const noexcept
{
    return activeStrings[static_cast<size_t> (stringIndex)].active;
}

void FretboardMapper::rememberAssignment (const FretboardAssignment& assignment,
                                          int midiNoteNumber,
                                          int midiChannel) noexcept
{
    activeStrings[static_cast<size_t> (assignment.stringIndex)] = { midiNoteNumber, midiChannel, true };
}

void FretboardMapper::updatePositionMemory (int fret) noexcept
{
    const auto target = static_cast<float> (fret);

    if (target > positionFret)
        positionFret = 0.68f * positionFret + 0.32f * target;
    else
        positionFret = 0.94f * positionFret + 0.06f * target;

    if (positionFret < 1.0f)
        positionFret = 1.0f;
    else if (positionFret > 17.0f)
        positionFret = 17.0f;
}

} // namespace guitar_ag
