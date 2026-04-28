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

cp "Vortex example progect/src/main.cpp" "$EXAMPLE_DIR/src/main.cpp"
cp "Vortex example progect/src/subsystems.cpp" "$EXAMPLE_DIR/src/subsystems.cpp"
cp "Vortex example progect/src/autons.cpp" "$EXAMPLE_DIR/src/autons.cpp"
cp "Vortex example progect/include/main.h" "$EXAMPLE_DIR/include/main.h"
cp "Vortex example progect/include/subsystems.hpp" "$EXAMPLE_DIR/include/subsystems.hpp"
cp "Vortex example progect/include/autons.hpp" "$EXAMPLE_DIR/include/autons.hpp"
cp "Vortex example progect/README.md" "$EXAMPLE_DIR/README.md"

python3 - "$EXAMPLE_DIR/project.pros" <<'PY'
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
text = path.read_text()
text = text.replace('"project_name": "Vortex"', '"project_name": "Vortex example progect"')
path.write_text(text)
PY

(cd "$EXAMPLE_DIR" && zip -r "$OUTPUT_DIR/$EXAMPLE_ZIP" .)

echo "Created $OUTPUT_DIR/$EXAMPLE_ZIP"
