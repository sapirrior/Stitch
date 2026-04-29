'use strict';

/**
 * Layout constraints and box model utilities for Stitch.
 */

export class Rect {
  constructor(x = 0, y = 0, width = 0, height = 0) {
    this.x = x | 0;
    this.y = y | 0;
    this.width = width | 0;
    this.height = height | 0;
  }

  /**
   * Applies padding to a rect, returning a new inner rect.
   * @param {number} top 
   * @param {number} right 
   * @param {number} bottom 
   * @param {number} left 
   * @returns {Rect}
   */
  pad(top, right, bottom, left) {
    return new Rect(
      this.x + left,
      this.y + top,
      Math.max(0, this.width - left - right) | 0,
      Math.max(0, this.height - top - bottom) | 0
    );
  }

  /**
   * Checks if a point is within the rect.
   * @param {number} x 
   * @param {number} y 
   * @returns {boolean}
   */
  contains(x, y) {
    return (
      x >= this.x &&
      x < this.x + this.width &&
      y >= this.y &&
      y < this.y + this.height
    );
  }
}
