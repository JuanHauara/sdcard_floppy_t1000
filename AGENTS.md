# Project: Toshiba T1000LE floppy emulator on STM32F411

## Mission
Build a minimal floppy emulator firmware for a Toshiba T1000LE using STM32F411 + microSD, starting from a clean STM32CubeIDE project and selectively copying only the useful pieces from the original FlashFloppy source tree into `libraries/floppy_emu/`.

This is not a generic Gotek-compatible product.
This is not a broad-compatibility floppy emulator.
This is a focused embedded firmware project for one machine only.

## Current project status
The active STM32CubeIDE project already exists and is compiling correctly.

The following libraries are already integrated and working:
- `libraries/board_gpio`
- `libraries/debug_log`
- `libraries/sd_card`
- `libraries/utils`
- `FATFS`

These libraries are already proven in another project and should be reused as-is unless there is a strong technical reason not to.

The following peripherals are already configured and working in STM32CubeIDE:
- `SPI1`
  - Configured for microSD
  - Current baudrate: 328.125 Kbit/s
- `TIM2`
  - Configured to run at 1 MHz
  - Used by `libraries/utils` for `delay_us()` and `get_us()`
- `USB_OTG_FS`
  - Configured in device-only mode
  - Used for debug messages through virtual COM port via `libraries/debug_log`

Do not redesign or replace these already working foundations unless strictly necessary.

## Workspace layout
The workspace contains two code trees:

### 1. Active project tree
This is the real firmware project.
All final firmware code must live here.

### 2. `original_flashfloppy/`
This folder contains the original FlashFloppy source tree.
It exists only as a reference source.
It must be treated as read-only.
Do not modify it.
Do not move files out of it.
Only inspect files there and copy selected files into `libraries/floppy_emu/` inside the active project.

## Very important workflow rule
When reusing FlashFloppy code:
- never move files from `original_flashfloppy/`
- always copy them
- destination must be inside `libraries/floppy_emu/` unless there is a very strong reason otherwise
- preserve `original_flashfloppy/` as an untouched baseline
- prefer copying the minimum required files only
- do not import the full FlashFloppy project into the active build

## Development workflow
- STM32CubeIDE is used for:
  - project generation
  - peripheral configuration
  - build
  - debug
- VSCode with ChatGPT GPT-5.4 is used for:
  - code inspection
  - architecture decisions
  - selecting useful FlashFloppy files
  - implementing firmware
  - adapting imported code
- The assistant must reason across both trees:
  - active project tree
  - `original_flashfloppy/`
- The assistant must explicitly state source and destination paths when recommending copied files.

## Target hardware
- MCU: STM32F411
- Prototype board: Blackpill STM32F411
- Storage: microSD
- Host: Toshiba T1000LE floppy interface
- Initial firmware target: one fixed floppy image on microSD

## Existing local libraries to reuse
Reuse these existing local libraries in the active project:

### `libraries/board_gpio`
MCU GPIO/pin handling library.

### `libraries/debug_log`
Debug output over virtual COM port via MCU USB.

### `libraries/sd_card`
FatFs wrapper plus SPI SD-card low-level interface.
Initializes the SD card and performs block-level read/write.
Intended to implement STM32CubeIDE-generated FATFS "User-defined" disk I/O layer, typically `user_diskio.c`, by replacing function bodies while preserving Cube-generated signatures.

When handling microSD and files, prefer using these existing APIs:
- `sd_card_status_t sd_card_init(void);`
- `sd_card_status_t sd_card_deinit(void);`
- `bool sd_card_is_ready(void);`

- `sd_card_status_t sd_card_file_open(sd_card_file_t *file, const char *path, uint32_t flags);`
- `sd_card_status_t sd_card_file_close(sd_card_file_t *file);`
- `sd_card_status_t sd_card_file_read(sd_card_file_t *file, void *buf, uint32_t len, uint32_t *out_read);`
- `sd_card_status_t sd_card_file_write(sd_card_file_t *file, const void *buf, uint32_t len, uint32_t *out_written);`
- `sd_card_status_t sd_card_file_seek(sd_card_file_t *file, uint32_t offset);`
- `sd_card_status_t sd_card_file_tell(sd_card_file_t *file, uint32_t *out_pos);`
- `sd_card_status_t sd_card_file_size(sd_card_file_t *file, uint32_t *out_size);`
- `sd_card_status_t sd_card_file_sync(sd_card_file_t *file);`

These APIs may be modified if strictly necessary, but always prefer keeping the storage layer simple and easy to maintain.

### `libraries/utils`
Provides utility functions for delays and timebase access.

When timing is needed, prefer using these existing APIs:
- `void delay_ms(uint32_t ms);`
- `void delay_us(uint32_t us);`
- `uint32_t get_ms(void);`
- `uint32_t get_us(void);`

Do not reinvent equivalent delay/time helpers if these functions already solve the problem.

## Integration policy for FlashFloppy
The imported FlashFloppy code should become a controlled local library:

- destination folder: `libraries/floppy_emu/`
- only copy the minimum useful files
- imported files may then be adapted for this project
- prefer a small and understandable `floppy_emu` library over preserving original project structure

When reviewing files inside `original_flashfloppy/`, always classify them into one of these categories:
- copy as-is into `libraries/floppy_emu/`
- copy then adapt into `libraries/floppy_emu/`
- use only as reference
- do not copy

Do not leave this implicit.

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
- carrying unused legacy code into the active project

If a subsystem only exists to support those features, prefer not copying it at all.

## Primary goals, in order
1. Preserve the clean STM32CubeIDE project.
2. Keep `original_flashfloppy/` untouched as reference.
3. Reuse existing working libraries and peripheral configuration.
4. Mount FAT filesystem through the existing storage stack.
5. Open one fixed floppy image from microSD.
6. Copy only the minimum useful FlashFloppy files into `libraries/floppy_emu/`.
7. Implement enough floppy emulation for Toshiba T1000LE bring-up.
8. Achieve stable read-only operation first.
9. Add write support only after read path is stable.

## Engineering priorities
- Deterministic behavior over feature richness.
- Small, understandable code over clever code.
- Minimal imported code from FlashFloppy.
- No unnecessary legacy code in the active project.
- Prefer explicit state machines.
- Prefer simple interfaces between layers.
- Avoid premature abstraction.
- Preserve observability and debugability.
- Keep timing-critical paths obvious.
- Prefer reusing existing `utils` and `sd_card` APIs instead of introducing new timing or storage abstractions.

## Scope assumptions
Assume:
- The Toshiba pinout will be provided or already exists in project docs.
- The floppy image is a standard DOS-compatible 720KB or 1.44MB image.
- The first milestone only needs one static mounted image.
- The assistant may inspect `original_flashfloppy/` and selectively reuse code from it.

Do not assume:
- standard Gotek pin mapping
- stock FlashFloppy board support
- stock FlashFloppy storage backend wiring
- display/buttons/UI are needed
- this project must remain upstream-compatible
- the whole FlashFloppy tree should be copied

## Coding style
- Language: C
- File names: snake_case, lowercase only
- Folder names: snake_case, lowercase only
- Functions: snake_case
- Macros/constants: upper snake case
- Prefer short, explicit modules with clear ownership
- Avoid large monolithic files when creating new code
- Add comments where timing, polarity, or hardware behavior is non-obvious
- Keep comments technical and concise

## Change policy
Before proposing changes:
1. Inspect the active project tree first.
2. Inspect `original_flashfloppy/` only to locate potentially useful source files.
3. Respect the existing working libraries and already configured peripherals.
4. Recommend exactly which files should be copied from `original_flashfloppy/` into `libraries/floppy_emu/`.
5. Do not suggest copying unused or speculative files.
6. When timing is needed, prefer the existing `utils` functions.
7. When file or storage access is needed, prefer the existing `sd_card` APIs.

For every meaningful recommendation, state:
- source file path
- destination file path
- whether to:
  - copy as-is
  - copy and adapt
  - use only as reference
  - ignore
- purpose of the file
- dependencies
- impact/risk
- whether the change is required now or can be deferred

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

If something is uncertain, say so clearly and request the exact file, schematic, or relevant source snippet.

## Preferred architecture in the active project
The active project should be organized conceptually into:

### `platform`
- STM32CubeIDE-generated startup and peripheral init
- clocks
- GPIO
- timers
- SPI
- interrupt plumbing

### `libraries`
- `board_gpio`
- `debug_log`
- `sd_card`
- `utils`
- `floppy_emu`

### `storage`
- fixed image mount/open
- image read/write backend
- thin integration over `sd_card`

### `floppy_core`
- track/side state
- sector/image interpretation
- ready/index/track0/write-protect/disk-change behavior
- host-visible logical behavior

### `host_if`
- Toshiba-side signal adaptation
- step handling
- side select
- motor/select/write gate/write data/read data lines
- timing-critical signal I/O

### `app`
- startup sequence
- init order
- mount fixed image
- run loop / service loop
- debug status

This structure does not need to match stock FlashFloppy.

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

Prefer a clean active codebase plus selected imported files over a large inherited codebase.

## Bring-up strategy
Work in short, verifiable phases.
Each phase must end in a concrete, testable result.

Recommended phase order:
1. Inspect active project tree and `original_flashfloppy/`.
2. Identify the minimum useful FlashFloppy files.
3. Copy the first minimal subset into `libraries/floppy_emu/`.
4. Integrate the copied code into the active build.
5. Use the existing `sd_card` layer to mount/open one fixed image.
6. Integrate floppy emulation core gradually.
7. Implement minimal host-visible floppy readiness behavior.
8. Implement read path.
9. Validate on Toshiba.
10. Only then implement write path.

## Debug strategy
Prefer adding lightweight instrumentation for:
- state transitions
- current track/side
- motor/select status
- image open success/failure
- read requests
- index generation
- ready/track0 transitions

Use `debug_log` for observability.
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
- importing the full original FlashFloppy tree into the active project
- keeping legacy code "just in case"
- modifying `original_flashfloppy/`
- replacing already working local libraries without clear reason
- adding new timing helpers if `utils` already covers the need
- adding new storage/file wrappers if `sd_card` already covers the need

## Expected assistant behavior
When helping in this workspace:
- first inspect, then propose
- reason across both trees
- be implementation-oriented
- produce concrete code, not only theory
- keep patches minimal
- call out risks early
- prefer decisions over vague option dumps
- explicitly tell which files in `original_flashfloppy/` to copy
- explicitly tell where they should go in `libraries/floppy_emu/`
- explicitly tell which files must not be copied
- treat `original_flashfloppy/` as read-only
- respect the existing working project base
- prefer using the existing `utils` and `sd_card` APIs

## First milestone definition
The first real success condition is:
- STM32F411 boots
- debug logging works
- microSD mounts
- one fixed image opens
- Toshiba sees a stable floppy device
- Toshiba can read from the emulated disk reliably

Everything else is secondary until this works.
