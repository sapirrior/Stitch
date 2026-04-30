'use strict';

import test from 'node:test';
import assert from 'node:assert';
import { moveCursor, sgr, RESET } from '../src/driver/ansi.js';

test('moveCursor sequence', () => {
  assert.strictEqual(moveCursor(0, 0), '\x1b[1;1H');
  assert.strictEqual(moveCursor(10, 5), '\x1b[6;11H');
  // Stability: Negative bounds should bypass cache and return valid (though clipped) sequence
  assert.strictEqual(moveCursor(-1, -1), '\x1b[0;0H');
});

test('sgr sequence', () => {
  // FG 1 (Red), BG 0 (Black), Attr 1 (Bold)
  assert.strictEqual(sgr(1, 0, 1), '\x1b[0;1;31;40m');

  // Bright FG 10 (Bright Green), Bright BG 12 (Bright Blue), No Attr
  assert.strictEqual(sgr(10, 12, 0), '\x1b[0;92;104m');

  // Stability: Out-of-range masking
  // 17 & 0xF = 1 (Red)
  // 16 & 0xF = 0 (Black)
  // 9 & 0x7 = 1 (Bold)
  assert.strictEqual(sgr(17, 16, 9), '\x1b[0;1;31;40m');
});

test('RESET constant', () => {
  assert.strictEqual(RESET, '\x1b[0m');
});

import StdoutDriver from '../src/driver/stdout.js';

test('StdoutDriver buffering', () => {
  const mockStream = {
    content: '',
    write(str) { this.content += str; }
  };
  const driver = new StdoutDriver();
  driver.stream = mockStream;

  driver.write('hello');
  driver.write(' ');
  driver.write('world');
  
  assert.strictEqual(mockStream.content, ''); // Should be buffered
  driver.flush();
  assert.strictEqual(mockStream.content, 'hello world');
});

test('StdoutDriver stability: non-string input', () => {
  const mockStream = {
    content: '',
    write(str) { this.content += str; }
  };
  const driver = new StdoutDriver();
  driver.stream = mockStream;

  driver.write(123);
  driver.write(true);
  assert.strictEqual(driver.length, 7);
  driver.flush();
  
  assert.strictEqual(mockStream.content, '123true');
});

test('StdoutDriver auto-flush', () => {
  const mockStream = {
    count: 0,
    write() { this.count++; }
  };
  const driver = new StdoutDriver();
  driver.stream = mockStream;

  const largeStr = 'a'.repeat(16385);
  driver.write(largeStr);
  
  assert.strictEqual(mockStream.count, 1); // Should have auto-flushed
});

test('StdoutDriver aggregate auto-flush', () => {
  const mockStream = {
    count: 0,
    write() { this.count++; }
  };
  const driver = new StdoutDriver();
  driver.stream = mockStream;

  // 16 small writes of 1024 chars = 16384 chars (no flush yet)
  for (let i = 0; i < 16; i++) {
    driver.write('a'.repeat(1024));
  }
  assert.strictEqual(mockStream.count, 0);

  // One more char to cross 16384
  driver.write('b');
  assert.strictEqual(mockStream.count, 1);
});
