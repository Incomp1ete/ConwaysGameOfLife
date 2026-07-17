# TODO

## Correctness & Concurrency Bugs

- [ ] `initSimulation()` handles `pthread_create()` failure with
      `assert(0 && "Failed  to create thread")` (`GameOfLifeSimulation.c:197`).
      `assert` compiles to nothing when `NDEBUG` is defined, which CMake does
      automatically for Release builds — the exact build type the CI
      workflow uses. If `pthread_create` ever fails, execution continues
      with `thread` uninitialized, and `disposeSimulation()` later calls
      `pthread_join(thread, NULL)` on that garbage handle — undefined
      behavior, likely a crash or hang on shutdown. Needs real error
      handling instead of `assert`.

- [ ] `running` (`GameOfLifeSimulation.c:20`) is a plain `bool`, unlike
      `paused` which is correctly `atomic_bool`. It's written by both the
      main thread (`disposeSimulation()`) and the worker thread
      (unconditionally set `true` as the first line of `worker()`), and read
      outside of any mutex at the top of `worker()`'s loop — a data race
      (UB) regardless of timing. Worst case: if the worker thread hasn't
      reached its first line yet when `disposeSimulation()` runs, the
      worker's own `running = true` stomps the shutdown request, hanging
      `pthread_join()` forever. Make it `atomic_bool` like `paused`, or fold
      it into the mutex-protected pause state.

- [ ] `getLastMeasurement()` / `startTimer()` / `takeMeasurement()`
      (`PerformanceMeasuring.c`) use `clock()`, which measures CPU time, not
      wall-clock time. The renderer uses `SDL_RENDERER_PRESENTVSYNC`
      (`Client.c:33`), so most of a frame's real duration is spent blocked
      waiting for vsync — time `clock()` won't count. The "Frametime" label
      will read far lower than the actual frame time. Should use
      `clock_gettime(CLOCK_MONOTONIC, ...)` instead, the same call already
      used correctly for tick timing in `GameOfLifeSimulation.c`.

- [ ] `mallocGrid()` (`GameOfLifeSimulation.c:22`) never checks whether
      `malloc` returned `NULL` before writing through the pointer. Unlikely
      to matter at 50x50, but worth at least an `assert`/abort so a future
      larger/configurable grid size (see grid-dimensions TODO below) fails
      loudly instead of corrupting memory.

- [ ] `WINDOW_HEIGHT` (800, `GameOfLifeUI.h:8`) is smaller than the combined
      height of the toolbar row (30px) and the cell grid
      (`VERTICAL_CELL_COUNT * cell_size` = 50*16 = 800px) — 830px of content
      in an 800px window, so the bottom of the grid can be clipped or
      require scrolling.

- [ ] `Client.c` doesn't check the return values of `SDL_Init`,
      `SDL_CreateWindow`, `SDL_CreateRenderer`, or `nk_sdl_init`. On a
      machine without a usable display/driver (e.g. a headless CI runner,
      relevant to the headless smoke-test TODO below), these can return
      `NULL`, and the code will crash on a null dereference instead of
      failing with a diagnostic.

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
