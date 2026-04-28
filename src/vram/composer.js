'use strict';

const { CHAR_MASK } = require('./bitmask');

/**
 * Composes multiple VRAM layers into a target buffer.
 * Composition is performed Top-to-Bottom for performance.
 * A cell is filled by the first non-transparent (char != 0) layer encountered.
 * 
 * @param {Uint32Array} target - The target back buffer.
 * @param {number} size - Size of the buffers.
 * @param {VRAM[]} layers - Array of VRAM instances, ordered Bottom-to-Top.
 */
function compose(target, size, layers) {
  const s = size | 0;
  target.fill(0);

  // Track which cells are already filled
  const filled = new Uint8Array(s);
  let remaining = s | 0;

  // Iterate from Top (last) to Bottom (first)
  for (let i = (layers.length - 1) | 0; i >= 0 && remaining > 0; i = (i - 1) | 0) {
    const layer = layers[i];
    const layerBuffer = layer.backBuffer;
    const layerSize = layer.size | 0;
    
    const limit = (layerSize < s ? layerSize : s) | 0;

    for (let j = 0; j < limit; j = (j + 1) | 0) {
      if (filled[j] === 0) {
        const val = layerBuffer[j] | 0;
        if ((val & CHAR_MASK) !== 0) {
          target[j] = val;
          filled[j] = 1;
          remaining = (remaining - 1) | 0;
        }
      }
    }
  }
}

module.exports = {
  compose,
};
