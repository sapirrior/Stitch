'use strict';

const test = require('node:test');
const assert = require('node:assert');
const Arena = require('../src/kernel/arena');
const Reconciler = require('../src/kernel/reconciler');
const { bindHooks, useState } = require('../src/kernel/hooks');

test('Arena dynamic resizing', () => {
  const arena = new Arena(10);
  for (let i = 0; i < 20; i++) {
    arena.alloc();
  }
  assert.strictEqual(arena.capacity >= 20, true);
});

test('Reconciler stable hashing', () => {
  const arena = new Arena();
  const rec = new Reconciler(arena);
  
  const ptr1 = rec.claim('Component', 1, 0, null);
  const ptr2 = rec.claim('Component', 1, 0, null);
  const ptr3 = rec.claim('Component', 1, 1, null);
  
  assert.strictEqual(ptr1, ptr2);
  assert.notStrictEqual(ptr1, ptr3);
});

test('Hook state and epoch validation', () => {
  const arena = new Arena();
  const rec = new Reconciler(arena);
  bindHooks(rec);

  const ptr = rec.claim('Test', 0, 0, null);
  rec.enter(ptr);
  
  const [val, setVal] = useState(0);
  assert.strictEqual(val, 0);
  
  setVal(1);
  assert.strictEqual(arena.refs[ptr][0], 1);
  assert.strictEqual(arena.isDirty(ptr), true);
  
  // Simulate re-render or unmount (increment epoch for THIS slot)
  const metaPtr = (ptr << 2) | 0;
  arena.meta[(metaPtr + 1) | 0] = (arena.meta[(metaPtr + 1) | 0] + 1) | 0;
  
  // Try stale update
  setVal(2);
  assert.strictEqual(arena.refs[ptr][0], 1); // Value should NOT change
});

test('Memory safety (free)', () => {
  const arena = new Arena();
  const ptr = arena.alloc();
  arena.refs[ptr] = { data: 'leak' };
  
  arena.free(ptr);
  assert.strictEqual(arena.refs[ptr], null);
});
