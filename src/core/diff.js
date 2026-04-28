'use strict';

const { moveCursor, sgr } = require('../driver/ansi');
const { CHAR_MASK, FG_MASK, BG_MASK, ATTR_MASK, FG_SHIFT, BG_SHIFT, ATTR_SHIFT } = require('../vram/bitmask');

/**
 * Optimized cell-matrix diffing engine using Symmetric Difference (Bitwise XOR).
 * Compares front and back buffers and writes minimal ANSI sequences to the driver.
 * 
 * @param {VRAM} vram - The VRAM instance.
 * @param {StdoutDriver} driver - The output driver.
 */
function diff(vram, driver) {
  const { width, height, frontBuffer, backBuffer } = vram;

  let currentFg = -1;
  let currentBg = -1;
  let currentAttr = -1;

  for (let y = 0; y < height; y = (y + 1) | 0) {
    for (let x = 0; x < width; x = (x + 1) | 0) {
      const index = (y * width + x) | 0;
      const backVal = backBuffer[index] | 0;
      const frontVal = frontBuffer[index] | 0;

      // Symmetric Difference via Bitwise XOR
      if ((backVal ^ frontVal) !== 0) {
        // Start of a dirty run
        driver.write(moveCursor(x, y));

        // Consume consecutive dirty cells on the same line
        let runX = x | 0;
        while (runX < width) {
          const runIndex = (y * width + runX) | 0;
          const val = backBuffer[runIndex] | 0;
          
          // Unpack without object allocation
          const char = (val & CHAR_MASK) | 0;
          const fg = ((val >>> FG_SHIFT) & FG_MASK) | 0;
          const bg = ((val >>> BG_SHIFT) & BG_MASK) | 0;
          const attr = ((val >>> ATTR_SHIFT) & ATTR_MASK) | 0;

          // Update style if changed
          if (fg !== (currentFg | 0) || bg !== (currentBg | 0) || attr !== (currentAttr | 0)) {
            driver.write(sgr(fg, bg, attr));
            currentFg = fg | 0;
            currentBg = bg | 0;
            currentAttr = attr | 0;
          }

          // Write character
          if (char < 128) {
            driver.write(String.fromCharCode(char));
          } else {
            driver.write(String.fromCodePoint(char));
          }

          // Sync front buffer
          frontBuffer[runIndex] = val | 0;

          runX = (runX + 1) | 0;
          // Peek next cell to see if we should continue the run
          if (runX < width) {
            const nextIndex = (y * width + runX) | 0;
            if ((backBuffer[nextIndex] ^ frontBuffer[nextIndex]) === 0) {
              break;
            }
          }
        }
        // Update loop X to end of run
        x = (runX - 1) | 0;
      }
    }
  }
}

module.exports = diff;
