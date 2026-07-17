# TODO

## Build system

- [ ] `src/CMakeLists.txt` uses `pthread.h` directly but never declares a
      dependency on threads. It currently links only because MSYS2's UCRT64
      GCC bakes in winpthreads by default. Use `find_package(Threads REQUIRED)`
      and link `Threads::Threads` instead, so the build doesn't silently rely
      on toolchain-specific behavior (e.g. would likely break under MSVC or a
      different MinGW distribution).

## Architecture

- [ ] `GameOfLifeSimulation.c` is all global mutable state (`cellGrid`,
      `pauseMutex`, `gridMutex`, `paused`, `running`, `thread`, ...) rather than
      an encapsulated instance/handle. Fine for a single-instance app, but
      worth deciding deliberately rather than by default — consider what an
      instance-based design (a `Simulation` struct passed around, or an
      opaque handle returned by `initSimulation()`) would look like, and what
      it would cost/buy.

- [ ] `doOneTick()` heap-allocates a brand new grid (`mallocGrid()`) and frees
      the old one on every single tick, instead of ping-ponging between two
      preallocated buffers and swapping pointers. Not a performance problem
      at 50x50, but it's the classic double-buffering technique and directly
      relevant to the low-level-programming goal of this project.

- [ ] Callers of the grid currently have to remember to call `lockGrid()` /
      `unlockGrid()` themselves (`resetSimulation()`, `renderCellGrid()`), while
      `doOneTick()` locks internally — two different conventions for the same
      resource, and nothing stops a future call site from getting it wrong
      (e.g. wrapping `doOneTick()` in `lockGrid()`/`unlockGrid()` again would
      self-deadlock). Consider hiding the mutex and raw `cellGrid` pointer
      entirely behind an API (e.g. `getCellState(x, y)`, `toggleCell(x, y)`,
      or a locked-iteration/callback function) so the locking discipline can't
      be bypassed or duplicated by a caller.

## Testing & CI

- [ ] Unit tests — at minimum the tick rules (`getNeighbourCount`,
      `doOneTick`'s birth/survival/death logic) are pure enough to test without
      standing up SDL/nuklear.
- [ ] Headless integration/smoke test — run with `SDL_VIDEODRIVER=dummy` so it
      works without a real display in CI. Drive the real `initModules()` →
      a few frames of `displayGame()` → `disposeModules()` lifecycle and
      assert nothing crashes or hangs. Can go further and inject synthetic
      mouse events via `SDL_PushEvent` before `nk_sdl_handle_event` to test
      click-to-toggle through the real nuklear layout, at the cost of
      coupling the test to pixel/cell layout math.
- [ ] CI/CD — build (and run tests) on push, at least for one platform to
      start.

## Features

- [ ] UI control for simulation speed. `worker()` currently hardcodes 1 tick/
      second (`next.tv_sec += 1`); expose this as a configurable interval.
- [ ] Pattern save/load — load/save grid patterns from a file (either a
      simple custom format or the standard Life 1.06/RLE format). Good
      exercise in file I/O and parsing.
- [ ] Configurable grid dimensions — `HORIZONTAL_CELL_COUNT` /
      `VERTICAL_CELL_COUNT` are compile-time `#define`s (50x50); make grid
      size a runtime parameter. Touches the double-buffering and grid-API
      encapsulation work above, so probably sequence after those.
- [ ] Toroidal (wrap-around) edges toggle — currently edges are hard
      boundaries (`getNeighbourCount` treats off-grid as dead); wrap-around
      topology is a classic Game of Life variant and a small, self-contained
      change to the neighbor-counting logic.

## Tooling

- [ ] Add a sanitizer build option (debug CMake preset/flag for
      `-fsanitize=thread,address`). Given the exact class of bugs just fixed
      by hand (data races, use-after-free), having the compiler/runtime catch
      these automatically going forward is high-value and directly on-topic
      for the low-level-programming goal of this project.

## Docs

- [ ] README — what the project is, how to build it, how to run it.
