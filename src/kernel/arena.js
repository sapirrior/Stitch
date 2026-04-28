'use strict';

/**
 * Stitch-Arena: A memory-safe, zero-allocation arena allocator optimized for V8.
 */
class Arena {
  /**
   * @param {number} initialCapacity
   */
  constructor(initialCapacity = 1024) {
    this.capacity = initialCapacity | 0;
    this.length = 0;

    // Meta: [Topology, Epoch, DirtyBit, ...reserved]
    // 4 slots per logical element for SMI efficiency
    this.meta = new Int32Array(this.capacity * 4);
    this.refs = new Array(this.capacity);
  }

  /**
   * Increases arena capacity.
   * Doubles capacity to minimize re-allocation frequency.
   */
  grow() {
    const oldCapacity = this.capacity | 0;
    this.capacity = (oldCapacity * 2) | 0;

    const oldMeta = this.meta;
    this.meta = new Int32Array(this.capacity * 4);
    this.meta.set(oldMeta);

    // refs is a standard array, we just need to ensure it's resized or handled by V8
    // We don't need .set() for standard Arrays, just increasing indices is fine.
  }

  /**
   * Allocates a slot in the arena.
   * @returns {number} The pointer (index) to the allocated slot.
   */
  alloc() {
    if (this.length >= ((this.capacity * 0.9) | 0)) {
      this.grow();
    }

    const ptr = this.length | 0;
    this.length = (this.length + 1) | 0;

    // Increment epoch for the new claim
    const metaPtr = (ptr << 2) | 0;
    this.meta[(metaPtr + 1) | 0] = (this.meta[(metaPtr + 1) | 0] + 1) | 0;

    return ptr;
  }

  /**
   * Explicitly frees a slot to prevent memory leaks.
   * @param {number} ptr
   */
  free(ptr) {
    const p = ptr | 0;
    if (p < 0 || p >= this.length) return;

    this.refs[p] = null;
    const metaPtr = (p << 2) | 0;
    this.meta[metaPtr] = 0; // Reset topology
    this.meta[(metaPtr + 2) | 0] = 0; // Reset dirty bit
  }

  /**
   * Marks a pointer as dirty.
   * @param {number} ptr
   */
  setDirty(ptr) {
    const metaPtr = (ptr << 2) | 0;
    this.meta[(metaPtr + 2) | 0] = 1;
  }

  /**
   * Checks if a pointer is dirty.
   * @param {number} ptr
   * @returns {boolean}
   */
  isDirty(ptr) {
    const metaPtr = (ptr << 2) | 0;
    return this.meta[(metaPtr + 2) | 0] === 1;
  }

  /**
   * Gets the current epoch of a slot.
   * @param {number} ptr
   * @returns {number}
   */
  getEpoch(ptr) {
    const metaPtr = (ptr << 2) | 0;
    return this.meta[(metaPtr + 1) | 0] | 0;
  }
}

export default Arena;
