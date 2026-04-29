# Contributing to Stitch

Thank you for your interest in contributing to Stitch! We maintain high engineering standards to ensure the engine remains lean, predictable, and performant.

## Code of Conduct
By participating in this project, you agree to maintain a professional and respectful environment.

## How to Contribute

### Reporting Bugs
- Check the [Issue Tracker](https://github.com/StitchNative/Stitch/issues) for existing reports.
- Use a clear, descriptive title.
- Provide a minimal reproduction script if possible.
- Include your environment details (Node.js version, OS, Terminal emulator).

### Suggesting Enhancements
- Open an issue to discuss the change before implementation.
- Explain the use case and the expected performance impact.

### Pull Request Process
1. Fork the repository and create your branch from `main`.
2. Ensure the code adheres to our [Engineering Standards](#engineering-standards).
3. Add or update tests for any new logic.
4. Verify all tests pass: `npm test`.
5. Use concise, imperative commit messages following our [Commit Convention](#commit-message-convention).
6. Submit your PR and await review.

### Commit Message Convention
We use a structured commit format to keep history readable. Every commit must follow the pattern: `type(scope): description`.

**Types:**
- `feat`: A new feature or capability.
- `fix`: A bug fix.
- `docs`: Documentation only changes.
- `test`: Adding missing tests or correcting existing tests.
- `perf`: A code change that improves performance.
- `refactor`: A code change that neither fixes a bug nor adds a feature.
- `chore`: Updating build tasks, configurations, or dependencies.

**Mandatory Scopes:**
A scope must be provided to specify the module being changed. Acceptable scopes are:
- `core`: Engine orchestrator and diffing logic.
- `vram`: Bit-packed memory and buffer management.
- `driver`: Terminal I/O and ANSI sequence generation.
- `layout`: Mathematical constraints and positioning.
- `kernel`: Component identity and hooks-based state.
- `build`: Build scripts, dependencies, or configuration.
- `repo`: Global documentation or project-wide metadata.

**Example:** `feat(kernel): add support for async hooks`

## Engineering Standards

To maintain V8 JIT friendliness and zero-cost abstractions, all code must follow these strict rules:

### 1. Performance & Memory
- **Zero Allocations:** Avoid creating objects (`{}`, `[]`, `new`) or string concatenations in hot paths (render/diff cycles).
- **TypedArrays:** Use `Uint32Array` for all grid and memory-intensive data.
- **Monomorphic Logic:** Keep object shapes consistent to prevent V8 "Hidden Class" transitions.

### 2. Mathematical Integrity
- **Strict Integer Math:** All layout and rendering logic must use 32-bit integer math. Coerce all calculations using bitwise OR (`| 0`).
  ```javascript
  const offset = ((total - margin) / 2) | 0;
  ```

### 3. Code Style
- **Strict Mode:** Every file MUST start with `'use strict';`.
- **Modularity:** Keep a strict separation between `vram`, `driver`, `core`, `layout`, and `kernel`.
- **Naming:** Use descriptive, technical names. Avoid marketing language or hyperbole.

### 4. Testing
- Every module must have a corresponding `.test.js` file in the `tests/` directory.
- We use the native `node:test` runner.
