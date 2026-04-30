'use strict';

import { Engine } from '../src/stitch.js';
import readline from 'node:readline';

/**
 * PRO TETRIS: Stitch Professional Example
 * 
 * Features:
 * - Double-width blocks (Square aspect ratio)
 * - High-fidelity Unicode borders
 * - Ghost Piece (Dimmed landing preview)
 * - Next Piece Preview
 * - Leveling system
 * - Zero-allocation hot paths
 */

const engine = new Engine();

const BOARD_WIDTH = 10;
const BOARD_HEIGHT = 20;

// Colors using 16-color ANSI palette
const COLORS = {
  I: 14, // Cyan
  J: 4,  // Blue
  L: 11, // Orange
  O: 3,  // Yellow
  S: 10, // Green
  T: 5,  // Magenta
  Z: 1   // Red
};

const TETROMINOS = {
  I: [[1, 1, 1, 1]],
  J: [[1, 0, 0], [1, 1, 1]],
  L: [[0, 0, 1], [1, 1, 1]],
  O: [[1, 1], [1, 1]],
  S: [[0, 1, 1], [1, 1, 0]],
  T: [[0, 1, 0], [1, 1, 1]],
  Z: [[1, 1, 0], [0, 1, 1]]
};

const SHAPES = Object.keys(TETROMINOS);

// Persistent State
const board = new Int32Array(BOARD_WIDTH * BOARD_HEIGHT);
let scene = 'menu'; // 'menu' | 'playing' | 'gameover'
let score = 0;
let level = 1;
let lines = 0;

let currentPiece = null;
let currentType = '';
let currentPos = { x: 3, y: 0 };
let nextType = SHAPES[(Math.random() * SHAPES.length) | 0];
let holdType = '';
let canHold = true;
let paused = false;

let frameCounter = 0;

function rotate(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  const newMatrix = Array.from({ length: cols }, () => new Array(rows).fill(0));
  for (let r = 0; r < rows; r++) {
    for (let c = 0; c < cols; c++) {
      newMatrix[c][rows - 1 - r] = matrix[r][c];
    }
  }
  return newMatrix;
}

function checkCollision(piece, pos) {
  for (let y = 0; y < piece.length; y++) {
    for (let x = 0; x < piece[y].length; x++) {
      if (piece[y][x]) {
        const bx = (pos.x + x) | 0;
        const by = (pos.y + y) | 0;
        if (bx < 0 || bx >= BOARD_WIDTH || by >= BOARD_HEIGHT) return true;
        if (by >= 0 && board[by * BOARD_WIDTH + bx]) return true;
      }
    }
  }
  return false;
}

function spawnPiece() {
  currentType = nextType;
  currentPiece = TETROMINOS[currentType];
  currentPos = { x: 3, y: 0 };
  nextType = SHAPES[(Math.random() * SHAPES.length) | 0];
  canHold = true;
  
  if (checkCollision(currentPiece, currentPos)) {
    scene = 'gameover';
  }
}

function lockPiece() {
  for (let y = 0; y < currentPiece.length; y++) {
    for (let x = 0; x < currentPiece[y].length; x++) {
      if (currentPiece[y][x]) {
        const bx = (currentPos.x + x) | 0;
        const by = (currentPos.y + y) | 0;
        if (by >= 0) {
          board[by * BOARD_WIDTH + bx] = COLORS[currentType];
        }
      }
    }
  }

  // Clear lines
  let linesCleared = 0;
  for (let y = BOARD_HEIGHT - 1; y >= 0; y--) {
    let isFull = true;
    for (let x = 0; x < BOARD_WIDTH; x++) {
      if (!board[y * BOARD_WIDTH + x]) {
        isFull = false;
        break;
      }
    }

    if (isFull) {
      linesCleared++;
      for (let sy = y; sy > 0; sy--) {
        for (let sx = 0; sx < BOARD_WIDTH; sx++) {
          board[sy * BOARD_WIDTH + sx] = board[(sy - 1) * BOARD_WIDTH + sx];
        }
      }
      for (let sx = 0; sx < BOARD_WIDTH; sx++) board[sx] = 0;
      y++;
    }
  }

  if (linesCleared > 0) {
    lines += linesCleared;
    score += (linesCleared * 100 * level);
    level = ((lines / 10) | 0) + 1;
  }
  spawnPiece();
}

function move(dx, dy) {
  const nextPos = { x: currentPos.x + dx, y: currentPos.y + dy };
  if (!checkCollision(currentPiece, nextPos)) {
    currentPos = nextPos;
    return true;
  }
  if (dy > 0) lockPiece();
  return false;
}

function resetGame() {
  board.fill(0);
  score = 0;
  level = 1;
  lines = 0;
  holdType = '';
  paused = false;
  scene = 'playing';
  spawnPiece();
}

// Input Handling
readline.emitKeypressEvents(process.stdin);
if (process.stdin.isTTY) process.stdin.setRawMode(true);

process.stdin.on('keypress', (str, key) => {
  if (key.ctrl && key.name === 'c') {
    engine.stop();
    process.exit();
  }

  if (scene === 'menu') {
    if (key.name === 's' || key.name === 'return') resetGame();
  } else if (scene === 'gameover') {
    if (key.name === 'm' || key.name === 'return') scene = 'menu';
  } else if (scene === 'playing') {
    if (key.name === 'p') {
      paused = !paused;
    }

    if (paused) return;

    if (key.name === 'up') {
      const rotated = rotate(currentPiece);
      if (!checkCollision(rotated, currentPos)) currentPiece = rotated;
    } else if (key.name === 'down') {
      move(0, 1);
    } else if (key.name === 'left') {
      move(-1, 0);
    } else if (key.name === 'right') {
      move(1, 0);
    } else if (key.name === 'space') {
      while (move(0, 1));
    } else if (key.name === 'c' && canHold) {
      if (!holdType) {
        holdType = currentType;
        spawnPiece();
      } else {
        const temp = currentType;
        currentType = holdType;
        holdType = temp;
        currentPiece = TETROMINOS[currentType];
        currentPos = { x: 3, y: 0 };
      }
      canHold = false;
    }
  }
});

function draw(vram) {
  const cx = (vram.width / 2) | 0;
  const cy = (vram.height / 2) | 0;

  if (scene === 'menu') {
    vram.clear(0);
    drawBox(vram, cx - 18, cy - 8, 36, 16, 8);
    drawText(vram, "S T I T C H   T E T R I S", cx, cy - 5, 11, 1);
    
    drawText(vram, "CONTROLS", cx, cy - 2, 15, 1);
    drawText(vram, "ARROWS : MOVE & ROTATE", cx, cy, 7, 0);
    drawText(vram, "SPACE  : HARD DROP", cx, cy + 1, 7, 0);
    drawText(vram, "C      : HOLD PIECE", cx, cy + 2, 7, 0);
    drawText(vram, "P      : PAUSE GAME", cx, cy + 3, 7, 0);

    drawText(vram, "PRESS [S] TO START", cx, cy + 6, 14, 1);
    return;
  }

  // Draw Main Board (Double-width)
  const bx = cx - 10;
  const by = cy - 10;

  // Board Box
  drawBox(vram, bx - 1, by - 1, (BOARD_WIDTH * 2) + 2, BOARD_HEIGHT + 2, 8);

  // Side Panel: Hold Piece
  const hx = bx - 12;
  drawBox(vram, hx, by, 10, 6, 8);
  drawText(vram, "HOLD", hx + 5, by - 1, 15, 0);
  if (holdType) {
    const holdShape = TETROMINOS[holdType];
    for (let y = 0; y < holdShape.length; y++) {
      for (let x = 0; x < holdShape[y].length; x++) {
        if (holdShape[y][x]) {
          vram.setCell(hx + 2 + x * 2, by + 2 + y, 9608, COLORS[holdType], 0, 0);
          vram.setCell(hx + 3 + x * 2, by + 2 + y, 9608, COLORS[holdType], 0, 0);
        }
      }
    }
  }

  // Static Blocks
  for (let y = 0; y < BOARD_HEIGHT; y++) {
    for (let x = 0; x < BOARD_WIDTH; x++) {
      const color = board[y * BOARD_WIDTH + x];
      if (color) {
        vram.setCell(bx + x * 2, by + y, 9608, color, 0, 0);
        vram.setCell(bx + x * 2 + 1, by + y, 9608, color, 0, 0);
      }
    }
  }

  if (scene === 'playing' && currentPiece) {
    // Calculate Ghost Piece
    let ghostY = currentPos.y;
    while (!checkCollision(currentPiece, { x: currentPos.x, y: ghostY + 1 })) {
      ghostY++;
    }

    // Draw Ghost
    for (let y = 0; y < currentPiece.length; y++) {
      for (let x = 0; x < currentPiece[y].length; x++) {
        if (currentPiece[y][x]) {
          vram.setCell(bx + (currentPos.x + x) * 2, by + ghostY + y, 9617, COLORS[currentType], 0, 2); // Dimmed pattern
          vram.setCell(bx + (currentPos.x + x) * 2 + 1, by + ghostY + y, 9617, COLORS[currentType], 0, 2);
        }
      }
    }

    // Draw Active Piece
    for (let y = 0; y < currentPiece.length; y++) {
      for (let x = 0; x < currentPiece[y].length; x++) {
        if (currentPiece[y][x]) {
          vram.setCell(bx + (currentPos.x + x) * 2, by + currentPos.y + y, 9608, COLORS[currentType], 0, 0);
          vram.setCell(bx + (currentPos.x + x) * 2 + 1, by + currentPos.y + y, 9608, COLORS[currentType], 0, 0);
        }
      }
    }
  }

  // Side Panel: Next Piece
  const nx = bx + (BOARD_WIDTH * 2) + 4;
  drawBox(vram, nx, by, 10, 6, 8);
  drawText(vram, "NEXT", nx + 5, by - 1, 15, 0);
  const nextShape = TETROMINOS[nextType];
  for (let y = 0; y < nextShape.length; y++) {
    for (let x = 0; x < nextShape[y].length; x++) {
      if (nextShape[y][x]) {
        vram.setCell(nx + 2 + x * 2, by + 2 + y, 9608, COLORS[nextType], 0, 0);
        vram.setCell(nx + 3 + x * 2, by + 2 + y, 9608, COLORS[nextType], 0, 0);
      }
    }
  }

  // Side Panel: Stats
  drawText(vram, "SCORE", nx + 5, by + 8, 8, 0);
  drawText(vram, String(score).padStart(7, '0'), nx + 5, by + 9, 14, 1);
  drawText(vram, "LEVEL", nx + 5, by + 12, 8, 0);
  drawText(vram, String(level).padStart(2, '0'), nx + 5, by + 13, 11, 1);
  drawText(vram, "LINES", nx + 5, by + 16, 8, 0);
  drawText(vram, String(lines).padStart(3, '0'), nx + 5, by + 17, 10, 1);

  if (paused && scene === 'playing') {
    for (let y = cy - 2; y <= cy + 2; y++) {
      for (let x = cx - 8; x <= cx + 8; x++) {
        vram.setCell(x, y, 32, 0, 8, 0); // Dark grey background
      }
    }
    drawText(vram, "P A U S E D", cx, cy, 15, 1);
  }

  if (scene === 'gameover') {
    // Overlay Game Over
    for (let y = cy - 2; y <= cy + 2; y++) {
      for (let x = cx - 12; x <= cx + 12; x++) {
        vram.setCell(x, y, 32, 0, 1, 0);
      }
    }
    drawText(vram, "G A M E   O V E R", cx, cy - 1, 15, 1);
    drawText(vram, "PRESS [M] FOR MENU", cx, cy + 1, 7, 0);
  }
}

function drawBox(vram, x, y, w, h, color) {
  // Corners
  vram.setCell(x, y, 9484, color, 0, 0); // ┌
  vram.setCell(x + w - 1, y, 9488, color, 0, 0); // ┐
  vram.setCell(x, y + h - 1, 9492, color, 0, 0); // └
  vram.setCell(x + w - 1, y + h - 1, 9496, color, 0, 0); // ┘
  
  // Edges
  for (let i = 1; i < w - 1; i++) {
    vram.setCell(x + i, y, 9472, color, 0, 0); // ─
    vram.setCell(x + i, y + h - 1, 9472, color, 0, 0);
  }
  for (let i = 1; i < h - 1; i++) {
    vram.setCell(x, y + i, 9474, color, 0, 0); // │
    vram.setCell(x + w - 1, y + i, 9474, color, 0, 0);
  }
}

function drawText(vram, str, x, y, fg, attr) {
  const startX = (x - (str.length / 2)) | 0;
  for (let i = 0; i < str.length; i++) {
    vram.setCell(startX + i, y, str.charCodeAt(i), fg, 0, attr);
  }
}

engine.start();

function update() {
  if (scene === 'playing' && !paused) {
    frameCounter++;
    const speed = Math.max(2, 30 - (level * 2));
    if (frameCounter >= speed) {
      move(0, 1);
      frameCounter = 0;
    }
  }
}

function tick() {
  update();
  engine.render(draw);
  setTimeout(tick, 16);
}

tick();
