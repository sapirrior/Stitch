# Stitch: Minimalist Modal Editor

Stitch is a C11 modal text editor that combines Nano's clean bottom-bar aesthetic with Vim's modal efficiency. It follows a strictly modular, domain-driven architecture to ensure maintainability and clarity.

## Architecture

The codebase is organized into four primary domains:

- **Core (`src/core/`, `include/stitch/core/`)**: Handles low-level system concerns, including POSIX terminal raw mode, signal-based resize handling (`SIGWINCH`), and the alternate buffer.
- **Buffer (`src/buffer/`, `include/stitch/buffer/`)**: Manages the internal representation of the text. `engine.c` handles line storage and modification, while `io.c` manages atomic file operations.
- **UI (`src/ui/`, `include/stitch/ui/`)**: Responsible for rendering the editor state to the terminal using an append-buffer strategy. Includes the inverted status bar and interactive prompt system.
- **Editor (`src/editor/`, `include/stitch/editor/`)**: Implements the modal logic (Normal, Insert, Command), key dispatching, and the internal command parser. Supports shell execution via `:! <command>`.

## Building and Running

### Build Requirements
- GCC or Clang with C11 support.
- POSIX-compliant environment.

### Commands
- **Build**: `make`
- **Run**: `./build/stitch [filename]`
- **Clean**: `rm -rf build/objs/* build/stitch` (or implement `make clean` in Makefile)

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
- **Command Line**: The very bottom row remains visually neutral with no background colors, ensuring it doesn't distract from the editor content.

### Technical Patterns
- **Resize Handling**: Use `sigaction` with `SA_RESTART` disabled to ensure `read()` is interrupted by `SIGWINCH`, returning `KEY_RESIZE` for instant UI updates.
- **Rendering**: Use `struct abuf` (append buffer) to batch `write()` calls and prevent screen flicker.
- **Background Shell Execution**: Uses `fork()` and `waitpid(..., WNOHANG)` to execute shell commands silently in the background, reporting only the final exit status in the command line/message bar to maintain focus and responsiveness.
- **Memory**: Always check `malloc`/`realloc` results. Free all allocated memory in the appropriate domain destructor or at exit.
