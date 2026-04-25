#!/usr/bin/env bash
set -euo pipefail

DATA_DIR="${GUITAR_AG_REFERENCE_DIR:-/Users/arnegleason/code/reference-audio/Guitar-TECHS}"
ARCHIVE_DIR="$DATA_DIR/archives"
EXTRACT_DIR="$DATA_DIR/extracted"
BASE_URL="https://zenodo.org/records/14963133/files"

if [[ $# -eq 0 ]]; then
    set -- P1_singlenotes.zip
fi

mkdir -p "$ARCHIVE_DIR" "$EXTRACT_DIR" "$DATA_DIR/analysis"

for file_name in "$@"; do
    archive_path="$ARCHIVE_DIR/$file_name"
    url="$BASE_URL/$file_name?download=1"

    echo "Downloading $file_name"
    curl --fail --location --continue-at - --output "$archive_path" "$url"

    echo "Extracting $file_name"
    unzip -n "$archive_path" -d "$EXTRACT_DIR"
done

echo "Reference data directory: $DATA_DIR"
