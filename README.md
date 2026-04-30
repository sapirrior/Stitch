# Stitch

A terminal-native UI engine built for the 2D integer grid. 

Stitch provides a hooks-based immediate-mode API for the terminal. It utilizes bit-packed memory and strict zero-allocation hot paths to operate within the constraints of the V8 JIT compiler.

## Core Philosophy

- **Zero Allocations:** The render and diffing loops are designed to avoid object creation, minimizing Garbage Collection (GC) overhead during screen updates.
- **Bit-Packed Memory:** Terminal cells are packed into single 32-bit integers (`Uint32Array`), storing the Unicode character, foreground color, background color, and styling attributes.
- **Integer-Only Math:** All layout and rendering operations use strict 32-bit integer math and Integer Linear Programming (ILP) concepts to maintain V8's SMI fast path and prevent floating-point rounding errors.

## Features

- **Immediate Mode API:** Procedural "paint" model for UI development.
- **Persistent State (Hooks):** Includes a Reconciler and Memory Arena to support state management (`useState`, `useEffect`) within the immediate-mode loop.
- **XOR Diffing:** Cell-matrix diffing ensures only changed characters are sent to the terminal, minimizing I/O overhead.
- **Pre-calculated Caching:** Uses static lookup tables (e.g., a 2048-entry SGR styling table) and caches to avoid string concatenations in hot paths.

## Architecture

- `src/kernel/`: Manages component identity and state via an Arena and Reconciler.
- `src/core/`: The main engine orchestrator and XOR-based symmetric difference algorithm.
- `src/vram/`: Dual-buffer grid management, Top-to-Bottom layer composition, and bitmasking logic.
- `src/layout/`: Integer-based proportional splitting and constraint utilities.
- `src/driver/`: ANSI escape sequence generation and buffered stdout communication.

## Installation

Install the engine directly from the source repository:

```bash
npm install github:StitchNative/Stitch
```

## Quick Start

```javascript
import { Engine, VRAM } from 'stitch';

const engine = new Engine();
engine.start();

engine.render((vram) => {
  // Draw an 'A' at (10, 5) with Red Foreground
  vram.setCell(10, 5, 65, 1, 0, 0);
  
  // Draw a horizontal line
  for (let x = 0; x < 20; x++) {
    vram.setCell(x, 0, 45, 7, 0, 0);
  }
});
```

## Development
Run tests using Node's native test runner:
```bash
npm test
```

Refer to [CONTRIBUTING.md](CONTRIBUTING.md) for engineering standards.
