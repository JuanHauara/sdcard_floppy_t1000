# Project: Toshiba T1000LE floppy emulator on STM32F411

## Mission
Build a minimal floppy emulator firmware for a Toshiba T1000LE using STM32F411 + microSD, taking only the useful parts from FlashFloppy and discarding the rest.

This is not a generic Gotek-compatible product.
This is not a broad-compatibility floppy emulator.
This is a focused embedded firmware project for one machine only.

## Development workflow
- Create a new empty STM32CubeIDE project for STM32F411.
- Configure and enable required peripherals using STM32CubeIDE UI.
- Use STM32CubeIDE only for:
  - project generation
  - peripheral configuration
  - build
  - debug
- Use VSCode with ChatGPT GPT-5.4 for implementation work.
- Do not preserve the original FlashFloppy project structure unless it is useful.
- The new project is the main codebase.
- Only copy selected files from FlashFloppy into the new project.
- Prefer copying only the minimum required source files rather than importing the whole original repository.

## Target hardware
- MCU: STM32F411
- Storage: microSD
- Host: Toshiba T1000LE floppy interface
- Prototype board: Blackpill STM32F411 + external microSD board
- Initial firmware target: one fixed floppy image on microSD

## Existing local libraries to reuse
The project must reuse these existing libraries from another STM32F411 project:

- `libraries/board_gpio`
  - Used to manage MCU pins.
- `libraries/debug_log`
  - Used to print debug messages through a virtual COM port over MCU USB.
- `libraries/sd_card`
  - FatFs wrapper plus SPI SD-card low-level interface.
  - Used to initialize the SD card and perform block-level read/write operations.
  - Intended to implement the STM32CubeIDE-generated FATFS "User-defined" disk I/O layer, typically `user_diskio.c`, by replacing function bodies while keeping Cube-generated function signatures.

Assume these libraries already exist and should be integrated into the new project instead of reinventing equivalent code.

## Non-goals
Do not optimize for:
- USB host
- LCD/OLED display
- buttons
- rotary encoder
- buzzer
- multiple image selection
- generic configuration UI
- broad machine compatibility
- preserving the full original FlashFloppy feature set
- carrying legacy code that is not needed

If a subsystem only exists to support those features, prefer not copying it at all.

## Primary goals, in order
1. Create a clean STM32CubeIDE-based project for STM32F411.
2. Integrate existing local libraries:
   - `board_gpio`
   - `debug_log`
   - `sd_card`
3. Bring up clocks, GPIO, timers, SPI, debug and microSD.
4. Mount FAT filesystem using STM32CubeIDE-generated FATFS integration plus local `sd_card` library.
5. Open one fixed floppy image from microSD.
6. Reuse only the minimum useful FlashFloppy source files.
7. Implement enough floppy emulation for Toshiba T1000LE bring-up.
8. Achieve stable read-only operation first.
9. Add write support only after read path is stable.

## Engineering priorities
- Deterministic behavior over feature richness.
- Small, understandable code over clever code.
- Minimal imported code from FlashFloppy.
- No unnecessary legacy code.
- Prefer explicit state machines.
- Prefer simple interfaces between layers.
- Avoid premature abstraction.
- Preserve observability and debugability.
- Keep timing-critical paths obvious.

## Scope assumptions
Assume:
- The Toshiba pinout will be provided or already exists in project docs.
- The floppy image is a standard DOS-compatible 720KB or 1.44MB image.
- The first milestone only needs one static mounted image.
- The project may reuse parts of FlashFloppy, but only by selectively copying useful files into the new project.

Do not assume:
- standard Gotek pin mapping
- stock FlashFloppy board support
- stock FlashFloppy storage backend wiring
- display/buttons/UI are needed
- this project must remain upstream-compatible
- the whole FlashFloppy repository should be imported

## Coding style
- Language: C
- File names: snake_case, lowercase only
- Functions: snake_case
- Macros/constants: upper snake case
- Prefer short, explicit modules with clear ownership
- Avoid large monolithic files when creating new code
- Add comments where timing, polarity, or hardware behavior is non-obvious
- Keep comments technical and concise

## Change policy
Before proposing code changes:
1. Inspect the current new STM32CubeIDE project structure.
2. Identify which existing local libraries are already present.
3. Inspect FlashFloppy only to locate the minimum useful source files.
4. Recommend exactly which files should be copied from FlashFloppy into the new project.
5. Do not suggest copying unused or speculative files.

For every meaningful change, state:
- files to modify/create/copy
- purpose of each file
- dependencies
- impact/risk
- whether the change is required now or can be deferred

When reviewing FlashFloppy, classify files into:
- copy as-is
- copy then adapt
- use only as reference
- do not copy

## Unknowns and uncertainty
Always separate conclusions into:
- confirmed
- probable
- unknown / must verify

Never invent:
- pin polarity
- timing values
- signal semantics
- FlashFloppy internal behavior
- Toshiba-specific electrical assumptions

If something is uncertain, say so clearly and ask to inspect the relevant file or documentation.

## Preferred architecture
Keep the new project split into these conceptual layers:

1. `platform`
   - STM32CubeIDE-generated startup and peripheral init
   - clocks
   - GPIO
   - timers
   - SPI
   - interrupt plumbing

2. `libraries`
   - `board_gpio`
   - `debug_log`
   - `sd_card`

3. `storage`
   - FatFs integration
   - fixed image mount/open
   - image read/write backend

4. `floppy_core`
   - track/side state
   - sector/image interpretation
   - ready/index/track0/write-protect/disk-change behavior
   - host-visible logical behavior

5. `host_if`
   - Toshiba-side signal adaptation
   - step handling
   - side select
   - motor/select/write gate/write data/read data lines
   - timing-critical signal I/O

6. `app`
   - startup sequence
   - init order
   - mount fixed image
   - run loop / service loop
   - debug status

This layering does not need to match stock FlashFloppy.

## FlashFloppy reuse policy
Use FlashFloppy only where it saves real engineering time, especially for:
- image handling
- floppy emulation concepts
- proven timing logic
- file/image parsing
- track/sector emulation logic if it fits this project

Do not carry over FlashFloppy subsystems that are unnecessary, especially:
- USB-specific code
- UI / display / button handling
- generic product infrastructure
- board support for unrelated hardware
- configuration systems not needed for one fixed image
- code included only for broad compatibility

Prefer a clean new codebase plus selected imported files over a large inherited codebase.

## Bring-up strategy
Work in short, verifiable phases.
Each phase must end in a concrete, testable result.

Recommended phase order:
1. Create clean STM32CubeIDE project.
2. Integrate `board_gpio`, `debug_log`, `sd_card`.
3. Bring up basic platform init.
4. Bring up debug output.
5. Bring up microSD low-level access.
6. Mount FAT.
7. Open one fixed image.
8. Copy the minimum useful FlashFloppy files.
9. Integrate floppy emulation core gradually.
10. Implement minimal host-visible floppy readiness behavior.
11. Implement read path.
12. Validate on Toshiba.
13. Only then implement write path.

## Debug strategy
Prefer adding lightweight instrumentation for:
- state transitions
- current track/side
- motor/select status
- image open success/failure
- read requests
- index generation
- ready/track0 transitions

If timing is sensitive, use compile-time debug guards.

## Patch strategy
Prefer small, reviewable patches.
Do not rewrite large portions of copied FlashFloppy code unless necessary.
If a larger rewrite is justified, explain why simpler options are worse.

## What to avoid
- Big speculative refactors
- UI work before read path works
- write support before read support is stable
- generic abstractions with no current need
- mixing storage logic with host signal handling
- hidden side effects across modules
- hardcoding assumptions without labeling them
- importing the full original FlashFloppy tree without clear need
- keeping legacy code "just in case"

## Expected assistant behavior
When helping in this repo:
- first inspect, then propose
- be implementation-oriented
- produce concrete code, not only theory
- keep patches minimal
- call out risks early
- prefer decisions over vague option dumps
- if multiple options exist, recommend one
- explicitly tell which original files to copy into the new project
- explicitly tell which original files not to copy

## First milestone definition
The first real success condition is:
- STM32F411 boots
- debug logging works
- microSD mounts
- one fixed image opens
- Toshiba sees a stable floppy device
- Toshiba can read from the emulated disk reliably

Everything else is secondary until this works.