'use strict';

const test = require('node:test');
const assert = require('node:assert');
const { center, alignEnd, clamp } = require('../src/layout/compute');
const { split } = require('../src/layout/ilp');
const { Rect } = require('../src/layout/constraints');

test('layout compute - center', () => {
  assert.strictEqual(center(10, 100), 45);
  assert.strictEqual(center(3, 10), 3); // (10-3)/2 = 3.5 -> 3
});

test('layout compute - alignEnd', () => {
  assert.strictEqual(alignEnd(10, 100), 90);
});

test('layout compute - clamp', () => {
  assert.strictEqual(clamp(5, 0, 10), 5);
  assert.strictEqual(clamp(-1, 0, 10), 0);
  assert.strictEqual(clamp(15, 0, 10), 10);
});

test('layout compute - split (ILP)', () => {
  // Simple equal split
  const res1 = split(80, [1, 1, 1, 1]);
  assert.deepStrictEqual(res1, new Int32Array([20, 20, 20, 20]));
  assert.strictEqual(res1.reduce((a, b) => a + b, 0), 80);

  // Remainder distribution
  const res2 = split(81, [1, 1, 1, 1]);
  assert.deepStrictEqual(res2, new Int32Array([21, 20, 20, 20]));
  assert.strictEqual(res2.reduce((a, b) => a + b, 0), 81);

  // Complex weights
  const res3 = split(100, [1, 2, 1]);
  assert.deepStrictEqual(res3, new Int32Array([25, 50, 25]));
  assert.strictEqual(res3.reduce((a, b) => a + b, 0), 100);

  // Large remainder
  const res4 = split(10, [3, 3, 3]); // base 10*3/9 = 3. 3*3=9. remainder 1.
  assert.deepStrictEqual(res4, new Int32Array([4, 3, 3]));
  assert.strictEqual(res4.reduce((a, b) => a + b, 0), 10);
});

test('Rect - pad', () => {
  const r = new Rect(0, 0, 100, 50);
  const inner = r.pad(5, 10, 5, 10);
  assert.strictEqual(inner.x, 10);
  assert.strictEqual(inner.y, 5);
  assert.strictEqual(inner.width, 80);
  assert.strictEqual(inner.height, 40);
});

test('Rect - contains', () => {
  const r = new Rect(10, 10, 20, 20);
  assert.strictEqual(r.contains(15, 15), true);
  assert.strictEqual(r.contains(5, 5), false);
  assert.strictEqual(r.contains(30, 30), false);
});
