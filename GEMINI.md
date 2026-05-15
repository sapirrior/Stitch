# Stitch: A Gentle Modal Editor

Stitch is a text editor designed to bridge the gap between the simple, discoverable aesthetic of `nano` and the powerful modal workflow of `vim`. It is written in C11 for POSIX-compliant systems.

## Project Overview

- **Aesthetic:** Clean, minimalist "Nano-style" interface with a persistent status bar above a command/message line. No header bars or cluttered legends.
- **Philosophy:** "Gentle" modal editing. Discoverable state and familiar Ctrl-key shortcuts for system tasks, while leveraging modal keys (hjkl, etc.) for text manipulation.
- **Architecture:** Domain-Driven Modular design. The codebase is separated into distinct domains: Core (Terminal), Buffer (Text Engine/IO), UI (Rendering/Prompts), and Editor (Modes/Commands).

## Architecture & Module Map

### `src/` (Source Code)
- **`main.c`**: Entry point. Orchestrates initialization and the main event loop.
- **`core/`**: Terminal lifecycle management (Raw mode, `termios` handling).
- **`buffer/`**:
  - `engine.c`: Low-level buffer manipulation (insertion, deletion, line merging).
  - `io.c`: File system operations (reading from and writing to disk).
- **`ui/`**:
  - `render.c`: The drawing engine. Handles rows, status bar, and flicker-free updates using escape sequences.
  - `prompt.c`: Logic for capturing user input in the command line (prompts, status messages).
- **`editor/`**:
  - `modes/dispatcher.c`: Modal input routing and state transitions.
  - `commands/parser.c`: Implementation of `:` style Vim commands.

### `include/stitch/` (Headers)
- **`types.h`**: Central definitions for `EditorConfig`, `Line`, `Mode`, and `EditorKey` enums.
- **Domain Headers**: Each subdirectory in `src/` has a corresponding header defining its public API.
- **`include/stitch.h`**: The umbrella header that includes all domain-specific headers.

## Building and Running

### Commands
- **Build**: `make` (Outputs binary to `build/stitch` and objects to `build/objs/`).
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

### Dependencies
- A C11 compatible compiler (e.g., `gcc` or `clang`).
- POSIX-compliant system (uses `termios.h`, `sys/ioctl.h`, `unistd.h`).

## Development Conventions

- **C Standard**: Strict C11 (`-std=c11`).
- **Modularity**: New features should be added to their respective domains. Avoid monolithic growth in `dispatcher.c` or `engine.c`.
- **Terminal Handling**: Always ensure `Raw Mode` is handled safely. The `die()` function in `terminal.c` is the standard way to exit on fatal errors, ensuring the terminal is restored.
- **Memory Management**: Manual memory management is required. All `malloc`/`realloc` calls must be checked for `NULL`. Use the `editorFreeBuffer` and `editorFreeLine` functions to prevent leaks.
- **UI Design**: Maintain the "Nano-style" minimalist look. The status bar should remain clean, and tildes (`~`) are avoided for empty lines.
- **Modal Logic**:
  - **Normal Mode**: Standard navigation and manipulation (hjkl, dd, x, etc.).
  - **Insert Mode**: Standard typing. `Esc` returns to Normal.
  - **Command Mode**: Activated via `:`. Handles file and system commands.
