# 💣 Bomb Hunter

A grid-based puzzle-strategy game built with [raylib](https://www.raylib.com/). Place bombs, lure monsters into the blast zone, and clear all 5 levels to win!

![Bomb Hunter](screenshot.png)

## 🌐 Play in Browser

**[▶ Play Now](https://YOUR_USERNAME.github.io/BombHunter/)** — no download, no install, works on any device.

*Or run locally after a web build: `cd build-web && python3 -m http.server 8000`*

## 🎮 How to Play

| Key | Action |
|---|---|
| `↑ ↓ ← →` / `W A S D` | Move |
| `E` | Place bomb on your current tile |
| `Q` | Detonate bomb (3×3 area) |

- Only **one bomb** can exist at a time.
- The explosion damages anything in a **3×3 area** (walls block it).
- Touching a monster **costs a life** and restarts the level.
- You have **3 lives**. Clear all monsters to advance.

## 🚀 Quick Start

You only need **CMake 3.15+** and a C++17 compiler. Raylib is downloaded and built automatically — nothing to install.

```bash
cmake -B build
cmake --build build
./build/BombHunter
```

> **Note for macOS:** Pass `-DUSE_SYSTEM_RAYLIB=ON` if you already have raylib installed via Homebrew and want faster builds.

## 🖥️ Platform Support

| Platform | Status |
|---|---|
| 🌐 **Web (WASM)** | ✅ Play in browser |
| macOS | ✅ Tested (Apple Silicon + Intel) |
| Linux | ✅ Supported |
| Windows | ✅ Supported |

## 📦 Download Pre-built Binaries

Pre-built binaries for macOS, Linux, and Windows are available on the [Releases](https://github.com/YOUR_USERNAME/BombHunter/releases) page.

## 🏗️ Building from Source

### Prerequisites
- **CMake** ≥ 3.15
- **C++17 compiler** (Clang, GCC, or MSVC)

### Linux — one-time setup
```bash
# Ubuntu / Debian
sudo apt install cmake g++ libx11-dev libxrandr-dev libxinerama-dev
        libxcursor-dev libxi-dev libgl1-mesa-dev

# Fedora
sudo dnf install cmake gcc-c++ libX11-devel libXrandr-devel
        libXinerama-devel libXcursor-devel libXi-devel mesa-libGL-devel
```

### Build
```bash
cmake -B build
cmake --build build -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
./build/BombHunter          # macOS / Linux
build\BombHunter.exe        # Windows
```

> Use `-DUSE_SYSTEM_RAYLIB=ON` if you have raylib already installed and want faster incremental builds.

### Web (Emscripten)

```bash
# Install Emscripten once
brew install emscripten

# Build for browser
./web-build.sh

# Serve locally
cd build-web && python3 -m http.server 8000
# Open http://localhost:8000/BombHunter.html
```

The web build outputs 4 files:
- `BombHunter.html` — page you open
- `BombHunter.js` — JavaScript glue
- `BombHunter.wasm` — WebAssembly binary
- `BombHunter.data` — game assets (levels)

## 🧱 Project Structure

```
BombHunter/
├── main.cpp              # Entry point
├── CMakeLists.txt        # Build config (cross-platform, auto-fetches raylib)
├── src/
│   ├── Game.h / .cpp     # Game state machine & top-level logic
│   ├── Player.h / .cpp   # Player movement & rendering
│   ├── Monster.h / .cpp  # Monster AI & rendering
│   ├── Bomb.h / .cpp     # Bomb placement, detonation, explosion
│   ├── Map.h / .cpp      # Grid map, tiles, collision
│   ├── Level.h / .cpp    # Level loading from .txt files
│   └── Utils.h / .cpp    # Shared helpers
└── levels/
    ├── level1.txt        # 20×15 grid: 0=floor, 1=wall, 2=player, 3=monster
    ├── level2.txt
    ├── level3.txt
    ├── level4.txt
    └── level5.txt
```

## 📄 License

MIT — see [LICENSE](LICENSE) for details.
