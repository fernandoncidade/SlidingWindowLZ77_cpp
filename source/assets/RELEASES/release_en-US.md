RELEASE NOTES: Version 2026.11.22.0<br>
Date: November 22, 2026<br>

# Sliding Window LZ77 Visualizer & 7z/LZMA Extractor

This update thoroughly modernizes the foundation of the application and its companion utilities to deliver a more stable, faster, and more reliable experience on Windows 10 and Windows 11 (64-bit).

## Key Improvements and Updates

### 1. Unified Experience Exclusively for Windows 10 and Windows 11 (64-bit)
- The application has been consolidated into a single native 64-bit edition dedicated to Windows 10 and Windows 11, discontinuing maintenance of the legacy alternative variant.
- This unification eliminates differences between editions and ensures that every user receives the same standard of quality, compatibility, and performance.

### 2. Modernized Graphics and System Foundation
- The visual components and underlying application foundation have been upgraded to a newer generation, delivering greater overall stability, improved integration with modern Windows features, and readiness for future enhancements.
- The essential files distributed with the application are now organized more cleanly and consistently, avoiding environment conflicts and ensuring immediate execution without missing components.

### 3. Greater Reliability in Companion Utilities
- The supporting translation and version management utilities have been aligned with the same updated foundation used by the main application.
- An issue that could cause file conflicts when preparing these utilities simultaneously has been resolved, making the process safer and more predictable.

### 4. Improved Distribution, Installation, and Maintenance
- Packaging and installation have been streamlined and updated to target exclusively the native 64-bit edition for Windows 10 and Windows 11, removing obsolete items and ensuring that all components required for the program to run are included automatically.
- Temporary file cleanup and environment preparation routines have been refined to keep the workspace organized and free from leftovers of older editions.

### 5. Updated Documentation
- The project's informational documents have been revised in both Portuguese (Brazil) and English (US) to reflect the new Windows 10 and Windows 11 (64-bit) exclusive foundation, maintaining full consistency between both languages.

---

RELEASE NOTES: Version 2026.8.21.0<br>
Date: August 21, 2026<br>

# Sliding Window LZ77 Visualizer & 7z/LZMA Extractor

Welcome to the official Release Notes for **Sliding Window LZ77 Visualizer & 7z/LZMA Extractor**. This release consolidates a complete software engineering suite for data decompression, streaming analytics, and deep inspection of dictionary-based compression algorithms (Sliding Window LZ77) and the LZMA (Lempel-Ziv-Markov chain-Algorithm) architecture.

---

## 1. Project Overview

The **Sliding Window LZ77 Visualizer** is a high-performance desktop application engineered in modern **C++17** and powered by the **Qt6** framework (version 6.11.1). Its mission is to deliver both practical multi-format archive extraction capabilities and real-time analytical inspection of LZ77/LZMA dictionary match pointers, tracking:
- **Distance (Offset)**: Backward displacement within the sliding dictionary window.
- **Length**: Number of identical bytes replicated from the prior sliding buffer.
- **Literals**: Uncompressed raw bytes emitted when no dictionary match is found.
- **Uncompressed Size and Compression Metrics**: Global stream sizing and elapsed processing times.

---

## 2. Key Features and Release Highlights

### 2.1 Integrated LZMA SDK Extraction Engine
- **Full LZMA/LZMA2 Decompression Core**: Built on the official reference LZMA SDK implementation (`7zDec.c`, `LzmaDec.c`, `Lzma2Dec.c`, `Bra86.c`, `Ppmd7Dec.c`, `7zAlloc.c`, etc.).
- **Broad Archive Format Support**: Native extraction support for `.7z`, `.zip`, `.rar`, `.tar`, `.wim`, `.xz`, `.gz`, `.bz2`, `.cab`, `.iso`, `.lzma`, `.zst`, `.tgz`, `.tbz2`, and `.txz`.
- **Asynchronous Multithreaded Architecture**: Extraction and dictionary match harvesting execute entirely in a dedicated worker thread (`LzmaExtractor`), maintaining consistent 60 FPS GUI responsiveness when processing massive multi-gigabyte archives.
- **Smart Chunk & Batch Streaming**: Batched emission of match information prevents UI freezing and ensures low memory footprint under high compression throughput.

### 2.2 Real-Time Virtual Matches Table
- **Virtualized Model Architecture (`LzmaMatchesTableModel`)**: Custom `QAbstractTableModel` designed to stream millions of match records with zero lag and minimal RAM overhead.
- **Detailed Data Columns**:
  - `Index`: Sequential identifier of the match event.
  - `Stream Offset`: Byte position within the decompressed data stream.
  - `Distance`: Lookback distance in the sliding history buffer.
  - `Length`: Repeated slice byte length.
  - `Dictionary Window`: Relative displacement metric within the active dictionary window.

### 2.3 Modern UI with Native Qt6 Widgets
- **Strict Native Widget Philosophy**: All windows, dialogs, progress bars, and menu items use standard native Qt6 widgets without custom CSS overrides, guaranteeing seamless OS integration, accessibility, and native visual rendering.
- **Comprehensive About Menu**:
  - `About`: Architectural summary and project credits.
  - `Readme`: Quick-start guidelines, build requirements, and features.
  - `User Manual`: Step-by-step instructions with anchor-linked table of contents.
  - `Release Notes`: Comprehensive version history and technical changelog.
  - `Documentation & Licenses`: Dedicated tabbed viewer for third-party licenses and notices.

### 2.4 Dynamic Internationalization (i18n)
- **Dual Language Support**: Portuguese (`pt-BR`) and English (`en-US`).
- **On-the-Fly Language Switching**: Immediate interface translation via `Settings > Language` without restarting the application.

### 2.5 Structured Logging Subsystem
- **Centralized Log Manager (`LogManager`)**: Auto-creates timestamped session logs in `%LOCALAPPDATA%\SlidingWindowLZ77\logs`.
- **Multi-Level Event Tracing**: Logs execution events, file selection, extraction progress, errors, and application teardown.

### 2.6 Dual Toolchain Support (MinGW & MSVC)
- **MinGW (GCC 13.1.0)**: Optimized release builds with static C/C++ runtimes.
- **MSVC (Visual Studio 2022 / v14.51)**: Fully optimized 64-bit binaries with complete VC CRT deployment.
- **CMake Presets**: Unified single-command Ninja configuration, building, and runtime deployment.
- **Balanced Distribution Size**: Balanced package sizes (~64 MB MSVC and ~71 MB MinGW).

---

## 3. Mock Utilities and Developer Tools

The project includes a set of support utilities built and exported together:
1. **`CompileTranslationsGUI.exe`**: GUI tool to compile `.ts` language files into `.qm` binaries using Qt's `lrelease`.
2. **`CompileTranslationsTerminal.exe`**: Command-line translation compiler for automated build workflows.
3. **`find_VersionEditor.exe`**: Visual governance utility to batch inspect and synchronize version numbers, technical revision dates, and author information across all 15 documentation files and Inno Setup scripts.

---

## 4. Inno Setup Installers

- Fully customized installers in [`repo/SlidingWindowLZ77.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77.iss), [`repo/SlidingWindowLZ77_MinGW.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77_MinGW.iss), and [`repo/SlidingWindowLZ77_MSVC.iss`](file:///c:/Users/ferna/APLICATIVOS/CPP/SlidingWindowLZ77_cpp/repo/SlidingWindowLZ77_MSVC.iss).
- Features solid compression, high-res icon integration, desktop/Start menu shortcuts, and clean uninstallation.

---

Version: v2026.8.21.0<br>
Technical revision date: August 21, 2026<br>
Author: Fernando Nillsson Cidade<br>
