'use strict';

const ESC = '\x1b[';
const RESET = '\x1b[0m';
const HIDE_CURSOR = '\x1b[?25l';
const SHOW_CURSOR = '\x1b[?25h';
const CLEAR_SCREEN = '\x1b[2J\x1b[H';

module.exports = {
  ESC,
  RESET,
  HIDE_CURSOR,
  SHOW_CURSOR,
  CLEAR_SCREEN,
};
