# Stitch: Minimalist Modal Editor

Stitch is a C11 modal text editor that combines Nano's clean bottom-bar aesthetic with Vim's modal efficiency. It follows a strictly modular, domain-driven architecture to ensure maintainability and clarity.

## Architecture

The codebase is organized into four primary domains:

- **Core (`src/core/`, `include/stitch/core/`)**: Manages the terminal state using `ncursesw`. Handles raw mode, input normalization, and coordinate-safe key mapping.
- **Buffer (`src/buffer/`, `include/stitch/buffer/`)**: Manages the internal representation of the text. `engine.c` handles line storage and modification, while `io.c` manages atomic file operations.
- **UI (`src/ui/`, `include/stitch/ui/`)**: Responsible for rendering the editor state using `ncurses` windows and color pairs. Includes the inverted status bar and interactive prompt system.
- **Editor (`src/editor/`, `include/stitch/editor/`)**: Implements the modal logic (Normal, Insert, Command), key dispatching, and the internal command parser. Supports shell execution via `:! <command>`.

## Building and Running

### Build Requirements
- GCC or Clang with C11 support.
- POSIX-compliant environment.
- **ncursesw** library (wide-character support).

### Commands
- **Build**: `make`
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

## Development Conventions

### Standards
- **Language**: Strict C11 (`-std=c11`).
- **Portability**: Target POSIX.1-2008 (`-D_POSIX_C_SOURCE=200809L`) and X/Open Extended for ncurses support.
- **Error Handling**: Use `die()` for fatal failures. All interactive components must implement defensive null-checks and bounds-checking to prevent segmentation faults.

### UI & Aesthetics
- **Organic Warmth Palette**: Uses soft tones: Sage (Normal), Terracotta (Insert), and Ochre (Command).
- **Background**: Default editor uses a standard Black background for clarity.
- **Status Bar**: Uses a neutral Deep Earth background with high-contrast, color-coded "Mode Blocks" (Sage/Terra/Ochre).
- **Command Line**: The very bottom row remains visually neutral with no background colors.

### Technical Patterns
- **Memory Safety**: All allocations must use `editorMalloc`, `editorRealloc`, or `editorStrdup`.
- **Key Normalization**: All input is normalized in `editorReadKey` (e.g., mapping `\r`, `\n`, and `KEY_ENTER` to `\r`) to ensure consistent behavior across different terminal emulators.
- **Resize Handling**: Delegated to `ncurses`' internal detection. `getch()` returns `STITCH_KEY_RESIZE`, triggering a full screen invalidation via `clear()` and `touchwin()`.
- **Rendering**: Uses efficient screen synchronization via `wnoutrefresh()` and `doupdate()`.
- **Atomic Saving**: Files are saved using a "write-then-rename" strategy (writing to `.filename.tmp` first) to prevent data loss.
- **Background Shell Execution**: Uses `fork()` and `waitpid(..., WNOHANG)` to execute shell commands silently in the background.
