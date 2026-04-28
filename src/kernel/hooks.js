'use strict';

/**
 * Stitch-Arena Hooks: State and effect management bound to the Arena.
 */

let activeReconciler = null;

/**
 * Binds hooks to a reconciler instance.
 * @param {Reconciler} reconciler
 */
export function bindHooks(reconciler) {
  activeReconciler = reconciler;
}

/**
 * useState hook.
 * @param {any} initialState
 * @returns {[any, function]}
 */
export function useState(initialState) {
  if (!activeReconciler || activeReconciler.currentContext === null) {
    throw new Error('useState must be called within a component render.');
  }

  const ptr = activeReconciler.currentContext | 0;
  const cursor = activeReconciler.hookCursor | 0;
  activeReconciler.hookCursor = (activeReconciler.hookCursor + 1) | 0;

  // We use a sub-allocation strategy for hooks within a component's refs slot
  // Each component's refs slot will hold an array of its own hook states
  if (activeReconciler.arena.refs[ptr] === null || activeReconciler.arena.refs[ptr] === undefined) {
    activeReconciler.arena.refs[ptr] = [];
  }

  const hookState = activeReconciler.arena.refs[ptr];
  
  if (hookState[cursor] === undefined) {
    hookState[cursor] = initialState;
  }

  const currentEpoch = activeReconciler.arena.getEpoch(ptr) | 0;

  const setState = (newValue) => {
    // Epoch validation to drop stale async updates
    if (activeReconciler.arena.getEpoch(ptr) !== currentEpoch) {
      return;
    }

    const nextValue = typeof newValue === 'function' ? newValue(hookState[cursor]) : newValue;
    
    if (hookState[cursor] !== nextValue) {
      hookState[cursor] = nextValue;
      activeReconciler.arena.setDirty(ptr);
    }
  };

  return [hookState[cursor], setState];
}

/**
 * useEffect hook.
 * @param {function} effect
 * @param {any[]} deps
 */
export function useEffect(effect, deps) {
  if (!activeReconciler || activeReconciler.currentContext === null) {
    throw new Error('useEffect must be called within a component render.');
  }

  const ptr = activeReconciler.currentContext | 0;
  const cursor = activeReconciler.hookCursor | 0;
  activeReconciler.hookCursor = (activeReconciler.hookCursor + 1) | 0;

  if (activeReconciler.arena.refs[ptr] === null || activeReconciler.arena.refs[ptr] === undefined) {
    activeReconciler.arena.refs[ptr] = [];
  }

  const hookState = activeReconciler.arena.refs[ptr];
  const prevDeps = hookState[cursor];

  let hasChanged = true;
  if (prevDeps && deps) {
    hasChanged = deps.some((dep, i) => dep !== prevDeps[i]);
  }

  if (hasChanged) {
    // In a real reconciler, effects are scheduled. 
    // Here we execute immediately for simplicity, but in a full TUI they would run after diff.
    effect();
    hookState[cursor] = deps;
  }
}
