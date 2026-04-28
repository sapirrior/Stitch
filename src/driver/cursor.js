'use strict';

const { ESC } = require('./constants');

// Cursor Cache: 256x256 cache
const CURSOR_CACHE = new Array(256 * 256);

/**
 * Moves cursor to (x, y). ANSI uses 1-based indexing (row, col).
 * @param {number} x - 0-indexed column.
 * @param {number} y - 0-indexed row.
 * @returns {string}
 */
function moveCursor(x, y) {
  const ix = x | 0;
  const iy = y | 0;
  if (ix < 256 && iy < 256) {
    const index = (iy << 8) | ix;
    let cached = CURSOR_CACHE[index];
    if (cached === undefined) {
      cached = CURSOR_CACHE[index] = `${ESC}${(iy + 1) | 0};${(ix + 1) | 0}H`;
    }
    return cached;
  }
  return `${ESC}${(iy + 1) | 0};${(ix + 1) | 0}H`;
}

module.exports = {
  moveCursor,
};
