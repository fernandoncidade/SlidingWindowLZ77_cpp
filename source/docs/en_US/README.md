# Sliding Window LZ77 & LZMA Visualizer

A modern graphical application written in C++23 and Qt 6 developed for real-time decompression and interactive visualization of the Sliding Window LZ77 compression algorithm and LZMA Markov models across the full range of archive formats supported by the [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip) project.

---

## 🌟 Overview

The **Sliding Window LZ77 & LZMA Visualizer** allows opening, inspecting, and extracting compressed archives in various formats — including `.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs`, `.ext`, and other containers supported by the 7-Zip ecosystem — intercepting and reconstructing the repetition reference and literal stream in real time.

The project combines two deconstruction approaches:
1. **Native C Core with Hooks for `.7z`:** Direct decoding inside the official **LZMA SDK (Igor Pavlov)** C codebase, intercepting the Range Coder and Markovian probabilities inside the innermost decompression loop.
2. **Multi-Format Sliding Analysis Engine:** Continuous extraction and real-time LZ77 sliding window matching for other archive containers via 7-Zip backend integration.

During processing, the application captures and displays:
- **Match Repetition Pointers:** Distance in the sliding window history ($d$) and match length ($l$), represented as the short reference `(d, l)`.
- **Literal Symbols:** Raw bytes emitted directly without previous matching history in the window.
- **Global Metrics:** Total matches, total literals, accumulated uncompressed size, and elapsed decompression time.

---

## 🚀 Key Features

- **Wide Archive Format Support:** Full compatibility with all archive formats supported by 7-Zip (`.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, etc.).
- **Real-Time Pointer Visualization:** Interactive table displaying output offset, distance ($d$), length ($l$), and short reference pair `(d, l)`.
- **Data Persistence in User Local AppData:** Automatic storage of logs and configuration in `%LOCALAPPDATA%\LZ77_SlidingWindow\` (`logs/` and `config/language.json`).
- **Dynamic Internationalization (i18n):** Instant runtime language switching between **Portuguese (Brazil)** and **English (US)** from the menu bar (`Settings > Language`), without restarting.
- **Integrated Documentation Viewer:** Direct reading of `About`, `Readme`, `Manual`, and `Release Notes` dialogs from the menu bar (`About > ...`).
- **Native GUI Execution without Terminal Window:** Pure Windows 10/11 x64 GUI subsystem supporting both **MinGW-w64 x64** and **MSVC x64**.
- **Modular C++23 Architecture:** Strict separation into `source/config`, `source/interface`, `source/locale`, `source/logs`, `source/lzma_sdk`, and `source/assets/RELEASES`.

---

## 🛠️ Technologies Used

- **Languages:** C++23 (Frontend & Qt Logic) and C11 (LZMA SDK Core)
- **GUI Framework:** Qt 6.12.0 (Modules: `Core`, `Gui`, `Widgets`, `LinguistTools`)
- **Supported Compilers & Platforms (Windows 10/11 x64):**
  - **MinGW-w64 x64:** GCC/G++ `15.1.0` (`x86_64-posix-seh`, Qt `6.12.0/mingw_64`)
  - **MSVC x64:** Microsoft Visual C++ (Visual Studio 18 / MSVC toolset `14.52.37025` / `v145`, Windows SDK `10.0.28000.0`, Qt `6.12.0/msvc2022_64`)
- **Build System:** CMake 3.28+ and Ninja (`1.12+` / `1.13+`)
- **Development Environment:** Visual Studio Code with CMake Tools and C/C++ Extensions

---

## 📂 Project Directory Structure

```text
SlidingWindowLZ77_cpp/
├── .vscode/                     # VS Code workspace settings (tasks.json, c_cpp_properties.json, etc.)
├── cmake/                       # Auxiliary CMake scripts (minimal Qt deployment)
├── mocks/                       # Isolated auxiliary mock tools (translation compiler, version editor)
├── source/
│   ├── assets/                  # Icons, licenses, and release notes (RELEASES)
│   ├── config/                  # Asynchronous extractor worker and hooks (LzmaExtractor)
│   ├── docs/                    # Integrated bilingual documentation (pt_BR and en_US)
│   ├── interface/               # Main window and GUI dialogs (MainWindow, DocumentViewerDialog)
│   ├── locale/                  # Dynamic translation manager and .ts / .qm files
│   ├── logs/                    # Thread-safe logging manager (LogManager)
│   └── lzma_sdk/                # Official LZMA SDK source code (pure C) with injected hooks
├── CMakeLists.txt               # Main application CMake build script
├── CMakePresets.json            # Configuration and build presets for MinGW x64 and MSVC x64
├── main.cpp                     # Application entry point
├── README.md                    # Unified multilanguage Readme
├── ABOUT.md                     # Unified multilanguage theoretical and architectural details
└── MANUAL.md                    # Unified multilanguage User Manual
```

> [!NOTE]
> Runtime data and persistence folders (`logs/` and `config/`) are created in the user data path:  
> `C:\Users\<user>\AppData\Local\LZ77_SlidingWindow\`

---

## ⚙️ Building and Running

### Prerequisites
- Windows 10/11 (64-bit).
- **For MinGW x64:** Qt 6.12.0 (`mingw_64`) at `C:/Qt/6.12.0/mingw_64`, MinGW-w64 GCC `15.1.0` at `C:/Qt/Tools/mingw1510_64`, and Ninja at `C:/Qt/Tools/Ninja`.
- **For MSVC x64:** Visual Studio 18 (Community or Build Tools) with MSVC x64 toolset `14.52.37025` (`v145`), Windows SDK `10.0.28000.0`, Ninja `1.13+`, and Qt 6.12.0 (`msvc2022_64`) at `C:/Qt/6.12.0/msvc2022_64`.
- CMake 3.28 or higher.

### 1. Build via VS Code (Quick Tasks)
Press `Ctrl+Shift+P` > `Tasks: Run Task` and select:
- **`Qt6: Compilar e Executar LZ77Visualizer (MinGW)`** / **`(MSVC)`** to build the Release app and run.
- **`Qt6: Compilar LZ77Visualizer Debug (MinGW)`** / **`(MSVC)`** to build the Debug configuration.
- **`Qt6: Compilar e Executar Mocks (MinGW)`** / **`(MSVC)`** to build and run the auxiliary GUI tools.
- **`Qt6: Compilar Tudo - App e Mocks (MinGW)`** / **`(MSVC)`** to build both the main application and all auxiliary tools for that toolchain.
- **`Qt6: Compilar Tudo - MinGW e MSVC (App e Mocks)`** to rebuild both complete toolchains.

### 2. Build via Command Line (PowerShell)

#### MinGW 15.1.0 x64 Build (Release / Debug / Mocks):
```powershell
cmake --preset host-qt6-mingw-release
cmake --build --preset host-qt6-mingw-release

cmake --preset host-qt6-mingw-debug
cmake --build --preset host-qt6-mingw-debug

cmake --preset mocks-qt6-mingw-release
cmake --build --preset mocks-qt6-mingw-release
```
MinGW outputs: `build/build_LZ77Visualizer_mingw/LZ77Visualizer.exe`, `build/build_LZ77Visualizer_mingw_debug/LZ77Visualizer.exe`, and `build/build_mocks_mingw/`.

#### MSVC x64 Build (Release / Debug / Mocks):
```powershell
cmake --preset host-qt6-msvc-release
cmake --build --preset host-qt6-msvc-release

cmake --preset host-qt6-msvc-debug
cmake --build --preset host-qt6-msvc-debug

cmake --preset mocks-qt6-msvc-release
cmake --build --preset mocks-qt6-msvc-release
```
MSVC outputs: `build/build_LZ77Visualizer_msvc_ninja/Release/LZ77Visualizer.exe`, `build/build_LZ77Visualizer_msvc_ninja_debug/Debug/LZ77Visualizer.exe`, and `build/build_mocks_msvc_ninja/Release/`.

---

## 📄 License & Credits

- **LZMA SDK:** Public Domain / LGPL (Igor Pavlov).
- **Interface & Visualizer:** Developed in C++23 and Qt 6 under MIT License.

---

Version: v2026.11.22.0<br>
Technical revision date: November 22, 2026<br>
Author: Fernando Nillsson Cidade<br>
