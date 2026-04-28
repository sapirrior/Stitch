'use strict';

import VRAM from '../vram/buffer.js';
import StdoutDriver from '../driver/stdout.js';
import { HIDE_CURSOR, SHOW_CURSOR, CLEAR_SCREEN, RESET } from '../driver/ansi.js';
import diff from './diff.js';
import Arena from '../kernel/arena.js';
import Reconciler from '../kernel/reconciler.js';
import { bindHooks } from '../kernel/hooks.js';

/**
 * libstitch Engine Orchestrator.
 */
class Engine {
  constructor() {
    this.driver = new StdoutDriver();
    this.width = process.stdout.columns || 80;
    this.height = process.stdout.rows || 24;
    this.vram = new VRAM(this.width, this.height);

    this.arena = new Arena();
    this.reconciler = new Reconciler(this.arena);
    bindHooks(this.reconciler);

    this._boundResize = this._onResize.bind(this);
  }

  /**
   * Starts the engine.
   */
  start() {
    this.driver.write(HIDE_CURSOR);
    this.driver.write(CLEAR_SCREEN);
    this.driver.flush();
    process.stdout.on('resize', this._boundResize);
  }

  /**
   * Stops the engine.
   */
  stop() {
    this.driver.write(RESET);
    this.driver.write(SHOW_CURSOR);
    this.driver.write(CLEAR_SCREEN);
    this.driver.flush();
    process.stdout.removeListener('resize', this._boundResize);
  }

  /**
   * Handles terminal resize events.
   * @private
   */
  _onResize() {
    this.width = process.stdout.columns || 80;
    this.height = process.stdout.rows || 24;
    this.vram.resize(this.width, this.height);
    this.driver.write(CLEAR_SCREEN);
  }

  /**
   * Performs a render cycle.
   * @param {function} drawFn - Function that receives (vram) and draws to the back buffer.
   */
  render(drawFn) {
    // 1. Clear back buffer (or let user handle it)
    // We'll provide a clean back buffer for immediate mode.
    this.vram.clear(0);

    // 2. Execute user draw logic
    drawFn(this.vram);

    // 3. Diff and flush
    diff(this.vram, this.driver);
    this.driver.flush();
  }
}

export default Engine;
