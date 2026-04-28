'use strict';

/**
 * Buffered output driver for libstitch.
 * Minimizes process.stdout.write calls by batching strings.
 * Uses Array.join('') to avoid Cons String flattening overhead.
 */
class StdoutDriver {
  constructor() {
    this.buffer = [];
    this.length = 0;
    this.stream = process.stdout;
  }

  /**
   * Appends a string to the buffer.
   * @param {string} str 
   */
  write(str) {
    this.buffer.push(str);
    this.length += str.length;
    // Auto-flush if buffer gets too large (e.g., 16KB)
    if (this.length > 16384) {
      this.flush();
    }
  }

  /**
   * Flushes the buffer to stdout.
   */
  flush() {
    if (this.buffer.length > 0) {
      this.stream.write(this.buffer.join(''));
      this.buffer.length = 0;
      this.length = 0;
    }
  }
}

module.exports = StdoutDriver;
