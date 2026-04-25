#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
JUCE_PATH="${JUCE_PATH:-/Users/arnegleason/code/JUCE}"
CONFIG="${CONFIG:-Release}"
SOURCE="$PROJECT_ROOT/build/GuitarAG_artefacts/$CONFIG/VST3/Guitar AG.vst3"
DEST_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
DEST="$DEST_DIR/Guitar AG.vst3"
GIT_COMMIT="$(git -C "$PROJECT_ROOT" rev-parse --short HEAD 2>/dev/null || echo unknown)"
if [[ "$GIT_COMMIT" != "unknown" ]] && [[ -n "$(git -C "$PROJECT_ROOT" status --porcelain 2>/dev/null)" ]]; then
    GIT_COMMIT="$GIT_COMMIT-dirty"
fi

if [[ "${1:-}" == "--build" ]]; then
    cmake -S "$PROJECT_ROOT" -B "$PROJECT_ROOT/build" \
        -DJUCE_PATH="$JUCE_PATH" \
        -DCMAKE_BUILD_TYPE="$CONFIG"
    cmake --build "$PROJECT_ROOT/build" --config "$CONFIG"
elif [[ $# -gt 0 ]]; then
    echo "Usage: $0 [--build]"
    exit 2
fi

if [[ ! -d "$SOURCE" ]]; then
    echo "VST3 bundle not found: $SOURCE"
    echo "Run '$0 --build' first, or build the plugin before installing."
    exit 1
fi

mkdir -p "$DEST_DIR"
rsync -a --delete "$SOURCE/" "$DEST/"

echo "Installed: $DEST"
echo "Build: $CONFIG / commit $GIT_COMMIT"
