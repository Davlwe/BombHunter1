#!/usr/bin/env bash
# Build Bomb Hunter for the web (Emscripten / WebAssembly)
#
# Prerequisites:
#   brew install emscripten    # macOS
#   or: https://emscripten.org/docs/getting_started/downloads.html
#
# Output:  build-web/BombHunter.html  +  .js  +  .wasm  +  .data
# Open BombHunter.html in a browser (served via localhost) to play.

set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# ── Locate the Emscripten toolchain file ──
TOOLCHAIN=""
for candidate in \
    /opt/homebrew/opt/emscripten/libexec/cmake/Modules/Platform/Emscripten.cmake \
    /usr/local/opt/emscripten/libexec/cmake/Modules/Platform/Emscripten.cmake; do
    if [ -f "$candidate" ]; then
        TOOLCHAIN="$candidate"
        break
    fi
done

# Also check EMSDK if set
if [ -z "$TOOLCHAIN" ] && [ -n "${EMSDK:-}" ] && [ -f "$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ]; then
    TOOLCHAIN="$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
fi

if [ -z "$TOOLCHAIN" ]; then
    echo "ERROR: Could not find Emscripten.cmake toolchain file."
    echo "Install Emscripten:  brew install emscripten"
    exit 1
fi

echo "Using toolchain: $TOOLCHAIN"

# ── Configure ──
cmake -B build-web \
    -DWEB_BUILD=ON \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
    -DCMAKE_BUILD_TYPE=Release

# ── Build ──
NPROC=$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)
cmake --build build-web -j"$NPROC"

echo ""
echo "============================================"
echo "  Build complete!"
echo ""
echo "  To play locally:"
echo "    cd build-web && python3 -m http.server 8000"
echo ""
echo "  Then open:  http://localhost:8000/BombHunter.html"
echo "============================================"
