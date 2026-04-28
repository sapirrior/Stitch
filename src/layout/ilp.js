'use strict';

/**
 * Partitions totalSize into segments proportional to the weights in ratios.
 * Uses Integer Linear Programming (ILP) with remainder distribution to ensure
 * the sum of returned segments exactly equals totalSize.
 * 
 * @param {number} totalSize - The total integer space to split.
 * @param {number[]} ratios - Array of integer weight coefficients.
 * @returns {Int32Array} - Array of segment sizes.
 */
export function split(totalSize, ratios) {
  const len = ratios.length;
  const size = totalSize | 0;
  const result = new Int32Array(len);
  
  let totalWeight = 0;
  for (let i = 0; i < len; i++) {
    totalWeight = (totalWeight + (ratios[i] | 0)) | 0;
  }

  if (totalWeight === 0) return result;

  let allocated = 0;
  for (let i = 0; i < len; i++) {
    const s = ((size * (ratios[i] | 0)) / totalWeight) | 0;
    result[i] = s;
    allocated = (allocated + s) | 0;
  }

  let remainder = (size - allocated) | 0;
  // Distribute remainder to segments starting from the first one
  for (let i = 0; i < len && remainder > 0; i++) {
    result[i] = (result[i] + 1) | 0;
    remainder = (remainder - 1) | 0;
  }

  return result;
}
