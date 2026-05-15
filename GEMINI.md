# Stitch: Minimalist Modal Editor

Stitch is a C11 modal text editor that combines Nano's clean bottom-bar aesthetic with Vim's modal efficiency. It follows a strictly modular, domain-driven architecture to ensure maintainability and clarity.

## Architecture

The codebase is organized into four primary domains:

- **Core (`src/core/`, `include/stitch/core/`)**: Handles low-level system concerns, including POSIX terminal raw mode, signal-based resize handling (`SIGWINCH`), and memory-safe allocation wrappers.
- **Buffer (`src/buffer/`, `include/stitch/buffer/`)**: Manages the internal representation of the text. `engine.c` handles line storage and modification, while `io.c` manages atomic file operations.
- **UI (`src/ui/`, `include/stitch/ui/`)**: Responsible for rendering the editor state to the terminal using an append-buffer strategy. Includes the inverted status bar, interactive prompt system, and non-blocking refresh logic.
- **Editor (`src/editor/`, `include/stitch/editor/`)**: Implements the modal logic (Normal, Insert, Command), key dispatching, and the internal command parser. Supports shell execution via `:! <command>` and a 10-item command history with arrow-key navigation.

## Building and Running

### Build Requirements
- GCC or Clang with C11 support.
- POSIX-compliant environment.

### Commands
- **Build**: `make`
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

## Development Conventions

### Standards
- **Language**: Strict C11 (`-std=c11`).
- **Portability**: Target POSIX.1-2008 (`-D_POSIX_C_SOURCE=200809L`).
- **Error Handling**: Use `die()` for fatal system or allocation failures. Recoverable I/O errors should return error codes and display a status message.

### UI & Aesthetics
- **Organic Warmth Palette**: Uses soft Truecolor (RGB) tones: Sage (Normal), Terracotta (Insert), and Ochre (Command).
- **No Header**: The top of the screen is reserved for text only.
- **Clean Empty Lines**: No tildes (`~`) or EOF indicators.
- **Balanced Status Bar**: A neutral Deep Earth background with high-contrast, color-coded "Mode Blocks" (Sage/Terra/Ochre) for instant state recognition.
- **Command Line**: The very bottom row remains visually neutral with no background colors.

### Technical Patterns
- **Memory Safety**: All allocations must use `editorMalloc`, `editorRealloc`, or `editorStrdup`. These wrappers provide centralized error handling via `die()`.
- **Atomic Saving**: Files are saved using a "write-then-rename" strategy (writing to `.filename.tmp` first) to prevent data loss during crashes or power failures.
- **Resize Handling**: `SIGWINCH` is handled via `sigaction` with `SA_RESTART` disabled. This ensures `read()` is interrupted, allowing `editorReadKey` to return `KEY_RESIZE` for instant UI updates.
- **Background Shell Execution**: Uses `fork()` and `waitpid(..., WNOHANG)` to execute shell commands silently. Only one background process is allowed at a time.
- **Command History**: A 10-item FIFO history is maintained for the command prompt, navigable via Up/Down arrow keys.
- **TTY Robustness**: Always check `isatty()` before applying terminal attributes or writing escape sequences to support piped/non-interactive environments.
