'use strict';

/**
 * Integer-based layout computation for Stitch.
 * All functions are pure mathematical functions using 32-bit integer math.
 */

/**
 * Calculates the start position for centering an item within a container.
 * @param {number} itemSize 
 * @param {number} containerSize 
 * @returns {number}
 */
export function center(itemSize, containerSize) {
  return (((containerSize | 0) - (itemSize | 0)) / 2) | 0;
}

/**
 * Calculates the start position for end-aligning an item.
 * @param {number} itemSize 
 * @param {number} containerSize 
 * @returns {number}
 */
export function alignEnd(itemSize, containerSize) {
  return ((containerSize | 0) - (itemSize | 0)) | 0;
}

/**
 * Clamps a value between min and max.
 * @param {number} val 
 * @param {number} min 
 * @param {number} max 
 * @returns {number}
 */
export function clamp(val, min, max) {
  const v = val | 0;
  const mi = min | 0;
  const ma = max | 0;
  if (v < mi) return mi;
  if (v > ma) return ma;
  return v;
}
