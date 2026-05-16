# Stitch: Minimalist Modal Editor

## Core Philosophy (DO NOT DELETE)
The ultimate goal of Stitch is to be a **"gentle modal text editor"**. It is designed to be simple, minimal, yet powerful enough for most daily tasks. Inspired heavily by the approachability of GNU Nano, it provides a clean interface and straightforward workflow. Stitch explicitly avoids the goal of becoming a hyper-complex, omnipotent environment. It is not meant to be everything to everyone; rather, it provides exactly what is needed for fast, focused text editing without the bloat.

Stitch is a C11 modal text editor that combines a clean bottom-bar aesthetic with traditional modal efficiency. It follows a strictly modular, component-based architecture to ensure industry-standard maintainability and scalability.

## Architecture

The codebase is organized into four primary domains, following a **Handler-Component** pattern. Global state is strictly forbidden; all application data is encapsulated in a `StitchState` context and injected via pointers.

- **Core (`src/core/`)**: System-level terminal state, input normalization, and memory management.
- **Buffer (`src/buffer/`)**: Text data management and file I/O. Atomic manipulations (insert, delete, line split) are isolated in `operations/`.
- **UI (`src/ui/`)**: Rendering logic using modular components (Status Bar, Message Bar, Text Grid) and `ncursesw`.
- **Editor (`src/editor/`)**: Modal logic and command execution. Each mode and command is an isolated component.

## Technical Standards

### Scale & Performance
- **Large File Support**: All coordinates, line counts, and buffer lengths use `size_t`, ensuring safety for files >2GB.
- **Amortized O(1) Editing**: Lines utilize exponential capacity growth (doubling on overflow) to minimize reallocations during active typing.
- **Memory Safety**: All allocations must use `editorMalloc` and `editorRealloc` wrappers for centralized error handling.

### Modularization
Every specific behavior is isolated to its own file:
- `src/editor/commands/`: Individual command implementations (e.g., `cmd_save.c`, `cmd_search.c`).
- `src/editor/modes/`: Modal behavior (Normal, Insert, Command).
- `src/ui/components/`: Modular UI widgets.
- `src/buffer/operations/`: Atomic buffer manipulations.

### Quality of Life (QoL)
- **Undo/Redo Engine**: Atomic operations are tracked in a lightweight doubly-linked list, bound to `u` and `U`.
- **Dynamic Line Numbers**: Toggleable via `:number` and `:nonumber`.
- **Help Overlay**: Accessible via `:h` or `:help`. Displays a clean, structured guide to core keybindings, dismissed with `Esc`.
- **Mouse Support**: Basic click-to-move support for intuitive cursor positioning within the text grid.
- **Bracket Matching**: Real-time highlighting of matching `()`, `[]`, and `{}` pairs.
- **Mode Switching**: The `escdelay` is minimized to 50ms to ensure instantaneous transitions between Insert and Normal modes.

## Building and Running

### Build Requirements
- GCC or Clang with C11 support.
- POSIX-compliant environment.
- **ncursesw** library.

### Commands
- **Build**: `make` (Produces a zero-warning build).
- **Run**: `./build/stitch [filename]`
- **Clean**: `make clean`

## Development Conventions

### Standards
- **Language**: Strict C11 (`-std=c11`).
- **Portability**: Target POSIX.1-2008 and `_XOPEN_SOURCE_EXTENDED` for ncursesw.
- **Naming**: Strict domain-based prefixing (e.g., `ui_`, `buffer_`, `core_`, `cmd_`).

### UI & Aesthetics
- **Design Philosophy**: Ultra-minimalist "Monochrome-Plus."
- **Aesthetic**: Uses standard terminal transparency/backgrounds. Color is strictly reserved for functional state indicators (Mode and Position) using the "Organic Warmth" palette (Sage Green, Terracotta Orange, Ochre Yellow).
- **Clean Interface**: Sparse and functional, with no decorative backgrounds.
- **Modified Indicator**: `*` directly attached to the filename in the status bar.
