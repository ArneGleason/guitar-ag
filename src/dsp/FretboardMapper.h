#pragma once

#include <array>

namespace guitar_ag
{

struct FretboardAssignment
{
    int stringIndex = 0;
    int fret = 0;
    bool wound = true;
    float woundAmount = 1.0f;
};

class FretboardMapper
{
public:
    void reset() noexcept;

    [[nodiscard]] FretboardAssignment assignNote (int midiNoteNumber,
                                                  int midiChannel,
                                                  int preferredStringIndex = -1,
                                                  float preferredStringBonus = 0.0f,
                                                  bool allowPreferredOccupied = false) noexcept;
    void releaseNote (int midiNoteNumber, int midiChannel) noexcept;
    [[nodiscard]] static int getFretForString (int midiNoteNumber, int stringIndex) noexcept;

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
        float woundAmount = 1.0f;
        float score = 0.0f;
    };

    [[nodiscard]] Candidate findBestCandidate (int midiNoteNumber,
                                               int preferredStringIndex,
                                               float preferredStringBonus,
                                               bool allowPreferredOccupied) const noexcept;
    [[nodiscard]] float scoreCandidate (int stringIndex,
                                        int fret,
                                        int preferredStringIndex,
                                        float preferredStringBonus,
                                        bool allowPreferredOccupied) const noexcept;
    [[nodiscard]] bool isStringOccupied (int stringIndex) const noexcept;
    void rememberAssignment (const FretboardAssignment& assignment, int midiNoteNumber, int midiChannel) noexcept;
    void updatePositionMemory (int fret) noexcept;

    static constexpr std::array<int, stringCount> openNotes { 40, 45, 50, 55, 59, 64 };
    static constexpr std::array<bool, stringCount> woundStrings { true, true, true, false, false, false };
    static constexpr std::array<float, stringCount> woundAmounts { 1.0f, 0.67f, 0.33f, 0.0f, 0.0f, 0.0f };

    std::array<ActiveString, stringCount> activeStrings {};
    float positionFret = 2.0f;
};

} // namespace guitar_ag
