'use strict';

import test from 'node:test';
import assert from 'node:assert';
import { moveCursor, sgr, RESET } from '../src/driver/ansi.js';

test('moveCursor sequence', () => {
  assert.strictEqual(moveCursor(0, 0), '\x1b[1;1H');
  assert.strictEqual(moveCursor(10, 5), '\x1b[6;11H');
});

test('sgr sequence', () => {
  // FG 1 (Red), BG 0 (Black), Attr 1 (Bold)
  // Expected: \x1b[0;1;31;40m
  assert.strictEqual(sgr(1, 0, 1), '\x1b[0;1;31;40m');

  // Bright FG 10 (Bright Green), Bright BG 12 (Bright Blue), No Attr
  // Expected: \x1b[0;92;104m
  assert.strictEqual(sgr(10, 12, 0), '\x1b[0;92;104m');
});

test('RESET constant', () => {
  assert.strictEqual(RESET, '\x1b[0m');
});
