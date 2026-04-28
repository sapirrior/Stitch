'use strict';

const test = require('node:test');
const assert = require('node:assert');
const VRAM = require('../src/vram/buffer');
const diff = require('../src/core/diff');

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
