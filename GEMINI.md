# Stitch: A Gentle Modal Editor

Stitch is a text editor designed to bridge the gap between the simple, discoverable aesthetic of `nano` and the powerful modal workflow of `vim`. It is written in strict C11 for POSIX-compliant systems.

## Project Overview

- **Aesthetic:** Minimalist "Nano-style" interface. A single inverted status bar sits above a command/message line at the bottom. No top headers or cluttered help legends.
- **Philosophy:** "Gentle" modal editing. Discoverable state through a persistent mode indicator, familiar Ctrl-key shortcuts for system tasks, and modal keys (hjkl, etc.) for text manipulation.
- **Architecture:** Domain-Driven Modular design. The codebase is strictly separated into specialized domains to ensure maintainability and scalability.

## Architecture & Module Map

### `src/` (Source Code)
- **`main.c`**: Entry point. Orchestrates initialization, raw mode activation, and the primary event loop.
- **`core/`**: 
  - `terminal.c`: Manages the terminal lifecycle, `termios` raw mode, and manual escape sequence parsing for special keys (Arrows, Home, End, etc.).
- **`buffer/`**:
  - `engine.c`: The core text engine. Handles line allocation, character insertion/deletion, tab expansion, and line merging.
  - `io.c`: File system operations. Implements resilient `editorOpen` (handles new and existing files) and `editorSave` (includes interactive filename prompting).
- **`ui/`**:
  - `render.c`: The drawing engine. Uses an append-buffer strategy for flicker-free rendering. Dynamically positions the cursor for both text editing and command-line input.
  - `prompt.c`: Interactive UI logic. Captures user input in the command line for commands and file naming.
- **`editor/`**:
  - `modes/dispatcher.c`: Modal input router. Manages transitions between `NORMAL`, `INSERT`, and `COMMAND` modes.
  - `commands/parser.c`: Implementation of `:` style commands (e.g., `:w`, `:q`, `:e`).

### `include/stitch/` (Headers)
- **`types.h`**: Central definitions for `EditorConfig`, `Line`, `Mode`, and the `EditorKey` enum.
- **Domain Headers**: Each subdirectory in `src/` has a corresponding header in `include/stitch/` defining its public API.
- **`include/stitch.h`**: Umbrella header aggregating all domain-specific headers.

## Key APIs & Hardened Features

- **Resilient File Opening**: `int editorOpen(char *filename)` returns `0` on success and `-1` on failure. It handles `ENOENT` (file not found) by initializing an empty buffer with the target name, allowing for new file creation.
- **Interactive Saving**: `editorSave()` will automatically trigger an `editorPrompt` if the buffer currently has no associated filename.
- **Unified Key Reading**: All input (buffer and prompts) passes through `editorReadKey()`, ensuring consistent behavior for backspace, delete, and escape keys across the application.
- **Flicker-Free UI**: The rendering engine clears and redraws the screen in a single `write()` call to avoid terminal flicker.

## Building and Running

### Commands
- **Build**: `make` (Outputs binary to `build/stitch` and objects to `build/objs/`).
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

### Dependencies
- **Standard**: C11 compiler (`gcc` or `clang`).
- **OS**: POSIX-compliant system (uses `termios.h`, `sys/ioctl.h`, `unistd.h`).

## Development Conventions

- **Standards**: Strict C11 (`-std=c11`). Use `-Wall -Wextra -pedantic`.
- **Modularity**: Logic must reside in its specific domain. Avoid monolithic growth in `dispatcher.c`.
- **Memory Safety**: Manual memory management is required. All allocations must be checked for `NULL`. Use `die()` for fatal allocation failures to ensure terminal raw mode is safely disabled before exit.
- **UI Consistency**: Maintain the minimalist aesthetic. Do not use tildes (`~`) for empty lines. The status bar should always be clean and separated by `|`.
- **Cursor Management**: Always update `E.rx` (render x) to account for tab stops when calculating cursor positioning on screen.
