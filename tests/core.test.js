'use strict';

import test from 'node:test';
import assert from 'node:assert';
import VRAM from '../src/vram/buffer.js';
import diff from '../src/core/diff.js';

class MockDriver {
  constructor() {
    this.output = '';
  }
  write(str) {
    this.output += str;
  }
  flush() {}
}

test('diffing logic - horizontal run', () => {
  const vram = new VRAM(10, 2);
  const driver = new MockDriver();

  // Set two consecutive cells on first line
  vram.setCell(0, 0, 65, 1, 0, 0); // 'A', Red
  vram.setCell(1, 0, 66, 1, 0, 0); // 'B', Red

  diff(vram, driver);

  // Should have moveCursor(0, 0), SGR, 'A', 'B'
  assert.match(driver.output, /1;1H/); // Move to 0,0 (1,1)
  assert.match(driver.output, /31/);   // Red FG
  assert.match(driver.output, /AB/);   // Text
});

test('diffing logic - partial update', () => {
  const vram = new VRAM(10, 2);
  const driver = new MockDriver();

  // Frame 1
  vram.setCell(0, 0, 65, 1, 0, 0);
  diff(vram, driver);
  driver.output = ''; // Reset mock for Frame 2

  // Frame 2: Change only one cell
  vram.setCell(1, 0, 66, 1, 0, 0);
  diff(vram, driver);

  assert.strictEqual(driver.output.includes('A'), false); // A should not be re-written
  assert.match(driver.output, /B/);
});

test('diffing logic - empty cell as space', () => {
  const vram = new VRAM(10, 1);
  const driver = new MockDriver();

  vram.setCell(0, 0, 0, 7, 0, 0); // Empty cell
  diff(vram, driver);

  assert.strictEqual(driver.output.includes(' '), true);
  assert.strictEqual(driver.output.includes('\0'), false);
});
