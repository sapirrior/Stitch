'use strict';

/**
 * libstitch bitmasking utilities for cell data.
 * 
 * Cell Bit Layout (32 bits):
 * 00-20: Unicode Codepoint (21 bits)
 * 21-24: Foreground Color (4 bits, 16-color ANSI)
 * 25-28: Background Color (4 bits, 16-color ANSI)
 * 29-31: Attributes (3 bits: Bold, Dim, Underline)
 */

const CHAR_MASK = 0x1FFFFF;
const FG_MASK = 0xF;
const BG_MASK = 0xF;
const ATTR_MASK = 0x7;

const FG_SHIFT = 21;
const BG_SHIFT = 25;
const ATTR_SHIFT = 29;

/**
 * Packs cell data into a single 32-bit unsigned integer.
 * @param {number} char - Unicode codepoint.
 * @param {number} fg - Foreground color (0-15).
 * @param {number} bg - Background color (0-15).
 * @param {number} attr - Attributes (0-7).
 * @returns {number} Packed 32-bit integer.
 */
function pack(char, fg, bg, attr) {
  return (
    (char & CHAR_MASK) |
    ((fg & FG_MASK) << FG_SHIFT) |
    ((bg & BG_MASK) << BG_SHIFT) |
    ((attr & ATTR_MASK) << ATTR_SHIFT)
  ) >>> 0;
}

/**
 * Unpacks a 32-bit integer into its cell components.
 * @param {number} val - Packed 32-bit integer.
 * @returns {object} Object containing char, fg, bg, and attr.
 */
function unpack(val) {
  return {
    char: val & CHAR_MASK,
    fg: (val >>> FG_SHIFT) & FG_MASK,
    bg: (val >>> BG_SHIFT) & BG_MASK,
    attr: (val >>> ATTR_SHIFT) & ATTR_MASK,
  };
}

module.exports = {
  pack,
  unpack,
  CHAR_MASK,
  FG_MASK,
  BG_MASK,
  ATTR_MASK,
  FG_SHIFT,
  BG_SHIFT,
  ATTR_SHIFT,
};
