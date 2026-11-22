<a name="inicio"></a><a name="sumario"></a><a name="top"></a>
# User Manual - Sliding Window LZ77 & LZMA Visualizer

Comprehensive, step-by-step user guide for the **Sliding Window LZ77 & LZMA Visualizer**.

---

## 📑 Table of Contents
1. [Introduction](#chapter-1)
2. [Supported Archive Formats](#chapter-2)
3. [Graphical Interface & Components](#chapter-3)
4. [Step-by-Step Extraction & Analysis Guide](#chapter-4)
5. [Data & Metrics Interpretation](#chapter-5)
6. [Language Switching & Internationalization](#chapter-6)
7. [Logging, Configuration & Diagnostics](#chapter-7)
8. [Troubleshooting](#chapter-8)

---

<a name="chapter-1"></a><a name="1-introduction"></a><a name="capitulo-1"></a>
## [1. Introduction](#inicio)

The **Sliding Window LZ77 & LZMA Visualizer** is a graphical tool designed to decompress archives and visually inspect how the LZ77 and LZMA algorithms identify redundancies in data streams, replacing repetitive sequences with short `(distance, length)` reference pointers.

---

<a name="chapter-2"></a><a name="2-supported-archive-formats"></a><a name="capitulo-2"></a>
## [2. Supported Archive Formats](#inicio)

The application supports the full range of archive containers handled by the [7-Zip (ip7z/7zip)](https://github.com/ip7z/7zip) project:

| Category | Supported Extensions |
|---|---|
| **High-Ratio Compression Archives** | `.7z`, `.xz`, `.lzma`, `.zst` |
| **Popular & Standard Archives** | `.zip`, `.rar`, `.tar`, `.tgz`, `.tbz2`, `.txz`, `.gz`, `.bz2`, `.cab` |
| **Disk Images & Containers** | `.wim`, `.iso`, `.dmg`, `.vhd`, `.vhdx`, `.vdi`, `.vmdk`, `.fat`, `.ntfs` |
| **Distribution Packages** | `.msi`, `.deb`, `.rpm`, `.cpio`, `.arj`, `.lzh`, `.chm` |

---

<a name="chapter-3"></a><a name="3-graphical-interface--components"></a><a name="capitulo-3"></a>
## [3. Graphical Interface & Components](#inicio)

Upon launching the application, the main window provides the following key sections:

1. **Menu Bar:**
   - **File:** Open any supported archive format or exit the application.
   - **Settings > Language:** Instant switching between **Português (Brasil)** and **English (US)**.
   - **About:** Direct access to **About** (`ABOUT.md` / `SOBRE.md`), **Readme** (`README.md` / `LEIAME.md`), **Manual** (`MANUAL.md`), and **Documentation** (tabbed dialog to inspect all licenses, permissions, and third-party component notices).
2. **Archive Selection Panel:**
   - File path line edit displaying the selected archive's absolute location.
   - **"Browse..."** Button: Opens the native OS file picker with filters for all supported file formats.
   - Primary Action Button **"Start Hook Extraction"**: Triggers asynchronous background decompression and repetition capturing.
   - **"Cancel"** Button: Safely aborts ongoing extraction.
3. **Real-Time Metrics Cards:**
   - **Total Matches:** Count of redundant sequences identified in the sliding dictionary.
   - **Total Literals:** Count of single bytes emitted without previous matching history.
   - **Uncompressed Size:** Accumulated uncompressed byte size.
   - **Execution Time:** Decompression duration in milliseconds.
4. **Match Stream Table:**
   - Real-time tabular visualization containing:
     - **Output Offset:** Byte position in the uncompressed output stream.
     - **Distance ($d$):** Lookback offset in the sliding window buffer.
     - **Length ($l$):** Length in bytes of the repeated sequence.
     - **Short Reference:** Compact representation `(d, l)`.
5. **Status and Progress Bar:**
   - Displays ongoing status (*Ready*, *Extracting and analyzing stream...*, *Completed*).
   - Visual progress bar showing percentage completion.

---

<a name="chapter-4"></a><a name="4-step-by-step-extraction--analysis-guide"></a><a name="capitulo-4"></a>
## [4. Step-by-Step Extraction & Analysis Guide](#inicio)

### Step 1: Select Compressed Archive
- Click **"Browse..."** or navigate to **File > Open Archive File...**.
- Select a valid compressed archive (e.g., `.7z`, `.rar`, `.zip`, `.tar`, `.wim`, `.xz`, etc.).

### Step 2: Start Analysis
- Click **"Start Hook Extraction"**.
- The start button is temporarily disabled, and the **"Cancel"** button becomes active.

### Step 3: Monitor Live Stream
- The match table will populate in progressive asynchronous batches.
- Compression metric cards update continuously as data blocks decode.

### Step 4: Completion
- Upon completion, a summary dialog reports final totals and elapsed time.

---

<a name="chapter-5"></a><a name="5-data--metrics-interpretation"></a><a name="capitulo-5"></a>
## [5. Data & Metrics Interpretation](#inicio)

- **Short Pointer `(d, l)`:** Larger length $l$ indicates larger matched repetition. Smaller distance $d$ indicates recent appearance in the sliding window.
- **Theoretical Compression Ratio:** Higher match count with lower literal count indicates high redundancy and superior compression density.

---

<a name="chapter-6"></a><a name="6-language-switching--internationalization"></a><a name="capitulo-6"></a>
## [6. Language Switching & Internationalization](#inicio)

1. Open the menu at **Settings > Language**.
2. Select **Português-BR** or **English-US**.
3. All interface texts, tables, buttons, and document dialogs translate immediately at runtime without application restart.

---

<a name="chapter-7"></a><a name="7-logging-configuration--diagnostics"></a><a name="capitulo-7"></a>
## [7. Logging, Configuration & Diagnostics](#inicio)

All application logs and user preferences persist in standard OS local storage:

- **Base Directory:** `C:\Users\<user>\AppData\Local\LZ77_SlidingWindow\`
- **Logs:** `%LOCALAPPDATA%\LZ77_SlidingWindow\logs\slidingwindowlz77_YYYY-MM-DD.log`
- **Language Config:** `%LOCALAPPDATA%\LZ77_SlidingWindow\config\language.json`

---

<a name="chapter-8"></a><a name="8-troubleshooting"></a><a name="capitulo-8"></a>
## [8. Troubleshooting](#inicio)

1. **"The specified file does not exist"**: Check that the archive path is accessible and not locked.
2. **`.rar` or `.wim` archives do not stream extraction**: Ensure 7-Zip is installed in standard path (`C:\Program Files\7-Zip\7z.exe`) or available in system `PATH`. The visualizer automatically falls back to direct analysis if the external tool is absent.
3. **Aborting Large Tasks**: For very large files, click **"Cancel"** at any moment to stop decompression cleanly.

---

Version: v2026.11.22.0<br>
Technical revision date: November 22, 2026<br>
Author: Fernando Nillsson Cidade<br>
