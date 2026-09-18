# Pthreads terminal game

Collect six moving gold pieces while avoiding horizontally moving walls. The
program retains the original three-worker design:

- an input worker handles W/A/S/D and Q;
- a wall worker advances six wrapping obstacles;
- a gold worker advances six collectible objects;
- the main thread renders a synchronized snapshot.

Shared world data is protected by a Pthreads mutex. Lifecycle flags are atomic,
and a terminal-session guard restores canonical input, file flags, and cursor
visibility on every normal return path.

## Build and run

```bash
make
./build/terminal-game
./build/terminal-game --seed 3150
```

The optional seed makes initial positions reproducible for demonstrations.
Run the program in a Linux/WSL terminal of at least 49 columns by 18 rows.

## Demo

[Watch the original MP4 recording](assets/demo.mp4).

