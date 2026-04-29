# Stitch

A modular, terminal-native UI engine built for the 2D integer grid.

## Core Philosophy
Stitch is designed for the terminal grid. It utilizes an immediate-mode architecture and bit-packed memory to operate within the constraints of the V8 JIT compiler.

## Features
- **Immediate Mode API:** Procedural "paint" model for UI development.
- **VRAM Buffer Diffing:** Cell-matrix diffing to minimize TTY I/O.
- **Bit-Packed Memory:** Uses `Uint32Array` to store character codes, colors, and styles in single 32-bit integers.
- **Zero Allocations:** Hot paths avoid object creation to reduce garbage collection overhead.
- **Strict Integer Math:** Layout and rendering logic uses 32-bit integer calculations.

## Architecture
- `src/vram/`: Dual-buffer grid management and bit-packing logic.
- `src/driver/`: ANSI escape sequence generation and buffered stdout communication.
- `src/core/`: Engine orchestration and diffing algorithms.
- `src/layout/`: Integer-based layout and constraint utilities.

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
  // 'vram' is an instance of VRAM passed by the engine to the callback
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
npm test
```

Refer to [CONTRIBUTING.md](CONTRIBUTING.md) for engineering standards.
