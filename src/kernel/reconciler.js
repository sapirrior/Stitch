'use strict';

/**
 * Stitch-Arena Reconciler: Manages component identity and arena slot claiming.
 */
class Reconciler {
  /**
   * @param {Arena} arena
   */
  constructor(arena) {
    this.arena = arena;
    this.hashSlots = new Map(); // hash -> ptr
    this.currentContext = null;
    this.hookCursor = 0;
  }

  /**
   * Simple string hash for component names.
   * @param {string} str
   * @returns {number}
   */
  hash(str) {
    let hash = 0;
    for (let i = 0; i < str.length; i++) {
      hash = (Math.imul(31, hash) + str.charCodeAt(i)) | 0;
    }
    return hash | 0;
  }

  /**
   * Claims a slot in the arena for a component instance.
   * @param {string} name
   * @param {number} depth
   * @param {number} siblingIndex
   * @param {any} key
   * @returns {number} The arena pointer.
   */
  claim(name, depth, siblingIndex, key) {
    const nameHash = this.hash(name) | 0;
    const keyHash = (key ? this.hash(String(key)) : 0) | 0;
    const id = (nameHash ^ (depth | 0) ^ (siblingIndex | 0) ^ keyHash) | 0;

    let ptr = this.hashSlots.get(id);
    if (ptr === undefined) {
      ptr = this.arena.alloc() | 0;
      this.hashSlots.set(id, ptr);
    }

    return ptr | 0;
  }

  /**
   * Sets the active render context.
   * @param {number} ptr
   */
  enter(ptr) {
    this.currentContext = ptr | 0;
    this.hookCursor = 0;
  }

  /**
   * Clears the render context.
   */
  exit() {
    this.currentContext = null;
    this.hookCursor = 0;
  }
}

export default Reconciler;
