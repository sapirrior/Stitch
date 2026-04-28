# libstitch

A high-performance, modular, terminal-native UI engine built for the grid.

## Core Philosophy
libstitch is designed purely for the 2D integer grid of the terminal. It eschews document-tree models (like React) in favor of a lean, immediate-mode architecture optimized for the V8 JIT compiler.

## Features
- **Immediate Mode API:** Simple "paint" model for UI development.
- **VRAM Buffer Diffing:** Optimized cell-matrix diffing to minimize TTY I/O.
- **Bit-Packed Memory:** Uses `Uint32Array` to store character codes, colors, and styles in a single 32-bit integer.
- **Zero Allocations:** Hot paths are designed to avoid garbage collection overhead.
- **Strict Integer Math:** All layout and rendering logic uses integer calculations.

## Architecture
- `src/vram/`: Manages the dual-buffer grid and bit-packing logic.
- `src/driver/`: Handles ANSI escape sequence generation and buffered stdout communication.
- `src/core/`: The heart of the engine, containing the diffing algorithm and orchestrator.
- `src/layout/`: Pure integer-based layout and constraint utilities.

## Usage

```javascript
const Engine = require('./src/core/engine');

const engine = new Engine();
engine.start();

engine.render((vram) => {
  // Draw an 'A' at (10, 5) with Red Foreground (1)
  vram.setCell(10, 5, 65, 1, 0, 0);
  
  // Draw a horizontal line
  for (let x = 0; x < 20; x++) {
    vram.setCell(x, 0, 45, 7, 0, 0);
  }
});

// engine.stop() to restore terminal state
```

## Development
Run tests using Node's native test runner:
```bash
node --test tests/
```

Refer to [CONTRIBUTING.md](CONTRIBUTING.md) for engineering standards.
