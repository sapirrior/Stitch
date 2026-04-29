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
    this._boundExit = this._onExit.bind(this);
    this._boundException = this._onException.bind(this);
  }

  /**
   * Starts the engine.
   */
  start() {
    this.driver.write(HIDE_CURSOR);
    this.driver.write(CLEAR_SCREEN);
    this.driver.flush();
    process.stdout.on('resize', this._boundResize);
    process.on('SIGINT', this._boundExit);
    process.on('SIGTERM', this._boundExit);
    process.on('uncaughtException', this._boundException);
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
    process.removeListener('SIGINT', this._boundExit);
    process.removeListener('SIGTERM', this._boundExit);
    process.removeListener('uncaughtException', this._boundException);
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
   * Handles process exit signals.
   * @private
   */
  _onExit() {
    this.stop();
    process.exit(0);
  }

  /**
   * Handles uncaught exceptions.
   * @private
   * @param {Error} err
   */
  _onException(err) {
    this.stop();
    console.error(err);
    process.exit(1);
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
