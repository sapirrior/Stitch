'use strict';

const test = require('node:test');
const assert = require('node:assert');
const { pack, unpack } = require('../src/vram/bitmask');
const VRAM = require('../src/vram/buffer');

test('bitmask pack/unpack', () => {
  const char = 65; // 'A'
  const fg = 1;    // Red
  const bg = 4;    // Blue
  const attr = 1;  // Bold

  const packed = pack(char, fg, bg, attr);
  const unpacked = unpack(packed);

  assert.strictEqual(unpacked.char, char);
  assert.strictEqual(unpacked.fg, fg);
  assert.strictEqual(unpacked.bg, bg);
  assert.strictEqual(unpacked.attr, attr);
});

test('bitmask boundaries', () => {
  const char = 0x1FFFFF;
  const fg = 15;
  const bg = 15;
  const attr = 7;

  const packed = pack(char, fg, bg, attr);
  const unpacked = unpack(packed);

  assert.strictEqual(unpacked.char, char);
  assert.strictEqual(unpacked.fg, fg);
  assert.strictEqual(unpacked.bg, bg);
  assert.strictEqual(unpacked.attr, attr);
});

test('VRAM initialization and setCell', () => {
  const vram = new VRAM(10, 5);
  assert.strictEqual(vram.width, 10);
  assert.strictEqual(vram.height, 5);
  assert.strictEqual(vram.size, 50);

  vram.setCell(2, 2, 65, 1, 0, 0);
  const val = vram.getCell(2, 2);
  const unpacked = unpack(val);

  assert.strictEqual(unpacked.char, 65);
  assert.strictEqual(unpacked.fg, 1);
});

test('VRAM clear', () => {
  const vram = new VRAM(10, 5);
  vram.setCell(0, 0, 65, 1, 1, 1);
  vram.clear(0);
  assert.strictEqual(vram.getCell(0, 0), 0);
});

test('VRAM swap', () => {
  const vram = new VRAM(10, 5);
  vram.setCell(5, 2, 66, 2, 0, 0);
  vram.swap();
  assert.strictEqual(vram.frontBuffer[2 * 10 + 5], vram.backBuffer[2 * 10 + 5]);
});
