'use strict';

import { pack } from './bitmask.js';
import { compose } from './composer.js';

/**
 * VRAM manages dual buffers for terminal cell data.
 * It provides methods for manipulating the back buffer and swapping with the front buffer.
 * Now supports mathematical Z-Order composition.
 */
class VRAM {
  /**
   * @param {number} width - Grid width.
   * @param {number} height - Grid height.
   */
  constructor(width, height) {
    this.width = width | 0;
    this.height = height | 0;
    this.size = (this.width * this.height) | 0;

    this.frontBuffer = new Uint32Array(this.size);
    this.backBuffer = new Uint32Array(this.size);
  }

  /**
   * Resizes the buffers.
   * @param {number} width 
   * @param {number} height 
   */
  resize(width, height) {
    this.width = width | 0;
    this.height = height | 0;
    this.size = (this.width * this.height) | 0;

    this.frontBuffer = new Uint32Array(this.size);
    this.backBuffer = new Uint32Array(this.size);
  }

  /**
   * Sets a cell in the back buffer.
   * @param {number} x 
   * @param {number} y 
   * @param {number} char 
   * @param {number} fg 
   * @param {number} bg 
   * @param {number} attr 
   */
  setCell(x, y, char, fg, bg, attr) {
    const ix = x | 0;
    const iy = y | 0;
    const w = this.width | 0;
    const h = this.height | 0;
    if (ix < 0 || ix >= w || iy < 0 || iy >= h) return;
    const index = (iy * w + ix) | 0;
    this.backBuffer[index] = pack(char, fg, bg, attr);
  }

  /**
   * Gets a cell from the back buffer.
   * @param {number} x 
   * @param {number} y 
   * @returns {number}
   */
  getCell(x, y) {
    const ix = x | 0;
    const iy = y | 0;
    const w = this.width | 0;
    const h = this.height | 0;
    if (ix < 0 || ix >= w || iy < 0 || iy >= h) return 0;
    return this.backBuffer[(iy * w + ix) | 0] | 0;
  }

  /**
   * Clears the back buffer.
   * @param {number} clearValue 
   */
  clear(clearValue = 0) {
    this.backBuffer.fill(clearValue | 0);
  }

  /**
   * Swaps the back buffer into the front buffer.
   */
  swap() {
    this.frontBuffer.set(this.backBuffer);
  }

  /**
   * Composes multiple VRAM layers into this VRAM's back buffer.
   * Composition is performed Top-to-Bottom for performance.
   * 
   * @param {VRAM[]} layers - Array of VRAM instances, ordered Bottom-to-Top.
   */
  compose(layers) {
    compose(this.backBuffer, this.size, layers);
  }
}

export default VRAM;
