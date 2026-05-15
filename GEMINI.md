# Stitch: Minimalist Modal Editor

Stitch is a C11 modal text editor that combines Nano's clean bottom-bar aesthetic with Vim's modal efficiency. It follows a strictly modular, component-based architecture to ensure industry-standard maintainability and scalability.

## Architecture

The codebase is organized into four primary domains, each following a **Handler-Component** pattern:

- **Core (`src/core/`, `include/stitch/core/`)**: Manages system-level terminal state and memory. Handlers coordinate low-level `ncursesw` initialization and input normalization.
- **Buffer (`src/buffer/`, `include/stitch/buffer/`)**: Manages text data and file I/O. Uses isolated operations for line manipulation, insertion, and deletion.
- **UI (`src/ui/`, `include/stitch/ui/`)**: Responsible for rendering using isolated components (Status Bar, Message Bar, Text Grid). Uses `wnoutrefresh()` and `doupdate()` for efficient screen synchronization.
- **Editor (`src/editor/`, `include/stitch/editor/`)**: Implements modal logic and command execution. Each command and mode is isolated into its own component file.

## Technical Standards

### Context Injection
Stitch avoids global state. All application data is encapsulated in a unified `StitchState` context, which is instantiated in `main.c` and passed by pointer to all handlers.

### Deep Modularization
Every specific behavior is isolated to its own file within dedicated subdirectories:
- `src/editor/commands/`: Individual command implementations (save, quit, search).
- `src/editor/modes/`: Modal behavior (Normal, Insert, Command).
- `src/ui/components/`: Modular UI widgets.
- `src/buffer/operations/`: Atomic buffer manipulations.

## Building and Running

### Build Requirements
- GCC or Clang with C11 support.
- POSIX-compliant environment.
- **ncursesw** library.

### Commands
- **Build**: `make` (Recursively compiles all modular components)
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

## Development Conventions

### Standards
- **Language**: Strict C11 (`-std=c11`).
- **Portability**: Target POSIX.1-2008 and X/Open Extended.
- **Naming**: Strict domain-based prefixing (e.g., `ui_`, `buffer_`, `core_`, `cmd_`).

### UI & Aesthetics
- **Organic Warmth Palette**: Sage (Normal), Terracotta (Insert), Ochre (Command).
- **Background**: Standard Black background for the text grid; Deep Earth background for the status bar.
- **Clean Interface**: No tildes or EOF indicators.
