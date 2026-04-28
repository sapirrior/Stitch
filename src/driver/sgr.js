'use strict';

// SGR Lookup Table: 16 FG * 16 BG * 8 ATTR = 2048 entries
const SGR_TABLE = new Array(2048);

function buildSgr(fg, bg, attr) {
  let seq = '\x1b[0';
  if (attr & 1) seq += ';1'; // Bold
  if (attr & 2) seq += ';2'; // Dim
  if (attr & 4) seq += ';4'; // Underline

  if (fg < 8) seq += `;${30 + fg}`;
  else seq += `;${90 + (fg - 8)}`;

  if (bg < 8) seq += `;${40 + bg}`;
  else seq += `;${100 + (bg - 8)}`;

  return seq + 'm';
}

// Initialize SGR table
for (let fg = 0; fg < 16; fg++) {
  for (let bg = 0; bg < 16; bg++) {
    for (let attr = 0; attr < 8; attr++) {
      const index = (fg << 7) | (bg << 3) | attr;
      SGR_TABLE[index] = buildSgr(fg, bg, attr);
    }
  }
}

/**
 * Returns a pre-calculated SGR sequence.
 * @param {number} fg 
 * @param {number} bg 
 * @param {number} attr 
 * @returns {string}
 */
function sgr(fg, bg, attr) {
  const index = (fg << 7) | (bg << 3) | attr;
  return SGR_TABLE[index];
}

module.exports = {
  sgr,
};
