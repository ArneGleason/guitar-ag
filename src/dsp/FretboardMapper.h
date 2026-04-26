#pragma once

#include <array>

namespace guitar_ag
{

struct FretboardAssignment
{
    int stringIndex = 0;
    int fret = 0;
    bool wound = true;
};

class FretboardMapper
{
public:
    void reset() noexcept;

    [[nodiscard]] FretboardAssignment assignNote (int midiNoteNumber, int midiChannel) noexcept;
    void releaseNote (int midiNoteNumber, int midiChannel) noexcept;

private:
    static constexpr auto stringCount = 6;
    static constexpr auto maxFret = 24;

    struct ActiveString
    {
        int noteNumber = -1;
        int channel = -1;
        bool active = false;
    };

    struct Candidate
    {
        int stringIndex = 0;
        int fret = 0;
        bool wound = true;
        float score = 0.0f;
    };

    [[nodiscard]] Candidate findBestCandidate (int midiNoteNumber) const noexcept;
    [[nodiscard]] float scoreCandidate (int stringIndex, int fret) const noexcept;
    [[nodiscard]] bool isStringOccupied (int stringIndex) const noexcept;
    void rememberAssignment (const FretboardAssignment& assignment, int midiNoteNumber, int midiChannel) noexcept;
    void updatePositionMemory (int fret) noexcept;

    static constexpr std::array<int, stringCount> openNotes { 40, 45, 50, 55, 59, 64 };
    static constexpr std::array<bool, stringCount> woundStrings { true, true, true, false, false, false };

    std::array<ActiveString, stringCount> activeStrings {};
    float positionFret = 2.0f;
};

} // namespace guitar_ag
