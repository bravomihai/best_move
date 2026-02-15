# Turn-Based Grid Game AI (Minimax + Alpha-Beta)

Engine for computing the optimal move in a two-player, turn-based grid game using **Minimax with Alpha-Beta pruning**.

The entry point is:

```c
move best_move(const char* pf);
```

It reads a game configuration from file and returns the best move for the current player.

---

## Game Model

### Board
- Rectangular grid: `H x W`
- Rows: `A-Z`, then `a-z` (if needed)
- Columns: numeric (1-based)
- Entities:
  - `A` – Player A
  - `B` – Player B
  - `m` – Monster
  - `oX` – Object with index `X`

---

## Player Stats

Each player has:

| Stat | Meaning |
|------|----------|
| H | Health |
| A | Attack |
| D | Defense |
| s | Remaining movement points |
| S | Max movement |

---

## Move Types

| Type | Meaning |
|------|----------|
| `m` | Move |
| `a` | Attack |
| `p` | Pass |

---

## Evaluation Strategy

The AI uses:

- **Minimax search**
- **Alpha-Beta pruning**
- Depth-limited search (`depth = 10`)
- Static evaluation function based on:
  - Health
  - Attack
  - Defense
  - Max stamina

Terminal states:
- Win → `+1e8`
- Loss → `-1e8`

---

## Core Components

### State Representation

Game state is encoded as a string:
```
"A A1 B H5 m C3 o0 D4 ..."
```

Utility functions:
- `encode()`
- `decode()`
- `next_states()`
- `static_eval()`
- `minimax()`

---

## Input File Format

```
H W NextPlayer
H A D s S   (Player A)
H A D s S   (Player B)
n           (number of item types)
dH dA dD dS (for each item)
<state string>
```

Example:

```
8 8 A
100 20 10 30 30
100 15 12 30 30
2
10 0 0 5
0 5 0 0
A A1 B H8 m D5 o0 C3
```

---

## Compilation

```bash
gcc -O2 -Wall main.c -o game_ai
```

Make sure `poe.h` is available and defines:
- `game_state`
- `player`
- `move`
- `item`

---

## How It Works

1. Read input file
2. Generate all possible next states
3. Evaluate each using Minimax
4. Return move with highest score

---

## Algorithm Flow

```
best_move()
  └── next_states()
        └── minimax()
              ├── recursive expansion
              ├── alpha-beta pruning
              └── static_eval()
```

---

## Features

- Manhattan distance movement
- Item pickup with stat modification
- Monster combat with reward
- Opponent combat system
- Depth-based search with pruning
- Deterministic best move selection

---

## Notes

- Maximum branching factor assumed: 30 states
- Memory for next states allocated dynamically
- Designed for competitive AI decision logic
- Intended to be integrated into a larger game engine
