'use strict';

import test from 'node:test';
import assert from 'node:assert';
import { performance } from 'node:perf_hooks';
import VRAM from '../src/vram/buffer.js';
import diff from '../src/core/diff.js';

class MockDriver {
  constructor() {
    this.buffer = [];
    this.length = 0;
  }
  write(str) {
    this.buffer.push(str);
    this.length += str.length;
  }
  flush() {
    this.buffer.length = 0;
    this.length = 0;
  }
}

test('Stress Test: 10,000 frames', () => {
  const width = 80;
  const height = 24;
  const vram = new VRAM(width, height);
  const driver = new MockDriver();
  
  const start = performance.now();
  
  for (let frame = 0; frame < 10000; frame++) {
    // Simulate some activity: update 10 random cells
    for (let i = 0; i < 10; i++) {
      const x = (Math.random() * width) | 0;
      const y = (Math.random() * height) | 0;
      const char = 65 + ((Math.random() * 26) | 0);
      const fg = (Math.random() * 16) | 0;
      const bg = (Math.random() * 16) | 0;
      vram.setCell(x, y, char, fg, bg, 0);
    }
    
    diff(vram, driver);
    driver.flush();
  }
  
  const end = performance.now();
  const duration = end - start;
  
  console.log(`Stress test completed in ${duration.toFixed(2)}ms (${(duration / 10000).toFixed(4)}ms/frame)`);
  
  // Assert that it's reasonably fast. 
  // 10,000 frames in 1 second is 0.1ms/frame.
  // We'll be generous and allow 5 seconds (0.5ms/frame).
  assert.ok(duration < 5000, `Execution time too high: ${duration}ms`);
});

test('Stability Test: Memory and Performance', () => {
  const width = 80;
  const height = 24;
  const vram = new VRAM(width, height);
  const driver = new MockDriver();

  const frameTimes = [];

  for (let frame = 0; frame < 5000; frame++) {
    // Full frame update every 100 frames to stress diffing
    const updateAll = (frame % 100 === 0);
    
    if (updateAll) {
      for (let y = 0; y < height; y++) {
        for (let x = 0; x < width; x++) {
          vram.setCell(x, y, 65, 7, 0, 0);
        }
      }
    } else {
      vram.setCell((Math.random() * width) | 0, (Math.random() * height) | 0, 66, 2, 0, 0);
    }

    const s = performance.now();
    diff(vram, driver);
    driver.flush();
    const e = performance.now();
    frameTimes.push(e - s);
  }

  // Check if frame times are stable (not increasing significantly)
  const firstHalf = frameTimes.slice(0, 2500);
  const secondHalf = frameTimes.slice(2500);
  
  const avg1 = firstHalf.reduce((a, b) => a + b, 0) / firstHalf.length;
  const avg2 = secondHalf.reduce((a, b) => a + b, 0) / secondHalf.length;
  
  console.log(`Average Frame Time (First Half): ${avg1.toFixed(4)}ms`);
  console.log(`Average Frame Time (Second Half): ${avg2.toFixed(4)}ms`);
  
  // Allow a small margin for system variance, but no 2x jumps.
  assert.ok(avg2 < avg1 * 2, `Performance degraded significantly: ${avg1} -> ${avg2}`);
});
