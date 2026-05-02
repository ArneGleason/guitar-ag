#include "FretboardMapper.h"

#include <limits>

namespace guitar_ag
{

void FretboardMapper::reset() noexcept
{
    for (auto& activeString : activeStrings)
        activeString = {};

    positionFret = 2.0f;
}

FretboardAssignment FretboardMapper::assignNote (int midiNoteNumber,
                                                 int midiChannel,
                                                 int preferredStringIndex,
                                                 float preferredStringBonus,
                                                 bool allowPreferredOccupied) noexcept
{
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

int FretboardMapper::getFretForString (int midiNoteNumber, int stringIndex) noexcept
{
    auto clampedString = stringIndex;

    if (clampedString < 0)
        clampedString = 0;
    else if (clampedString >= stringCount)
        clampedString = stringCount - 1;

    const auto fret = midiNoteNumber - openNotes[static_cast<size_t> (clampedString)];

    return fret >= 0 && fret <= maxFret ? fret : -1;
}

void FretboardMapper::releaseNote (int midiNoteNumber, int midiChannel) noexcept
{
    for (auto& activeString : activeStrings)
    {
        if (activeString.active && activeString.noteNumber == midiNoteNumber && activeString.channel == midiChannel)
            activeString = {};
    }
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

        const auto score = scoreCandidate (stringIndex,
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

    if (midiNoteNumber < openNotes.front())
        return { 0, 0, woundStrings.front(), woundAmounts.front(), 0.0f };

    return { stringCount - 1, maxFret, woundStrings.back(), woundAmounts.back(), 0.0f };
}

float FretboardMapper::scoreCandidate (int stringIndex,
                                       int fret,
                                       int preferredStringIndex,
                                       float preferredStringBonus,
                                       bool allowPreferredOccupied) const noexcept
{
    const auto fretDistance = static_cast<float> (fret) - positionFret;
    auto score = fretDistance * fretDistance;

    score += static_cast<float> (fret) * 0.030f;
    score += static_cast<float> (stringCount - 1 - stringIndex) * 0.010f;

    if (fret == 0)
        score -= 0.12f;

    if (fret >= 1 && fret <= 5)
        score -= 0.18f;

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
