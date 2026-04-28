# Contributing to libstitch

## Engineering Standards

To maintain high performance and V8 JIT friendliness, all contributions must adhere to these standards:

### 1. Strict Mode
Every JavaScript file must begin with `'use strict';`.

### 2. Zero Floating-Point Math
Core engine and layout calculations must use integer math. Use bitwise OR (`| 0`) to coerce results to 32-bit integers.
```javascript
const pos = ((container - item) / 2) | 0;
```

### 3. V8 JIT-Friendly Memory
- Use `TypedArrays` (specifically `Uint32Array`) for grid data.
- Avoid "Hidden Class" transitions by keeping objects monomorphic.
- **Zero Allocations in Hot Paths:** Do not use `new`, `[]`, `{}`, or string concatenation inside the `diff` or `render` loops.

### 4. Modular Architecture
- Keep modules focused on a single responsibility.
- Place files in their respective logical subfolders (`vram/`, `driver/`, `core/`, `layout/`).

### 5. Testing
- Create a corresponding test file in `tests/` for every new module.
- Use Node's native test runner (`node:test`).
- Ensure all tests pass before submitting changes.

### 6. Code Style
- Professional, formal tone in documentation and comments.
- No hyperbole (avoid "blazingly fast", "ultimate", etc.).
- Use descriptive variable names that reflect the bit-packed nature of the data where appropriate.
