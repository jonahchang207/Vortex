#!/usr/bin/env bash
set -euo pipefail

EXAMPLE_DIR="${1:-/tmp/vortex-example-project}"
EXAMPLE_ZIP="${2:-Vortex-Example-Project.zip}"
OUTPUT_DIR="${3:-$PWD}"

rm -rf "$EXAMPLE_DIR"
mkdir -p "$EXAMPLE_DIR"

rsync -a ./ "$EXAMPLE_DIR"/ \
  --exclude ".git" \
  --exclude ".github" \
  --exclude ".tmp_refs" \
  --exclude "bin" \
  --exclude ".d" \
  --exclude "vortex@*.zip" \
  --exclude "vortex-example.zip" \
  --exclude "Vortex-Example-Project.zip"

cp example/src/main.cpp "$EXAMPLE_DIR/src/main.cpp"
cp example/include/robot.hpp "$EXAMPLE_DIR/include/robot.hpp"
cp example/README.md "$EXAMPLE_DIR/README.md"

python3 - "$EXAMPLE_DIR/project.pros" <<'PY'
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
text = path.read_text()
text = text.replace('"project_name": "Vortex"', '"project_name": "Vortex Example"')
path.write_text(text)
PY

(cd "$EXAMPLE_DIR" && zip -r "$OUTPUT_DIR/$EXAMPLE_ZIP" .)

echo "Created $OUTPUT_DIR/$EXAMPLE_ZIP"
