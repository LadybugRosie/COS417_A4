# Repo Prompt

This repository contains COS417 Spring 2026 Assignment 4, which extends `xv6` with a reduced `mmap` implementation.

When working in this repo, assume the goal is to complete or debug the assignment in `readme.md` and preserve existing `xv6` behavior.

## Primary Objective

Implement anonymous shared memory mappings in `xv6` with these semantics:

- Support `mmap(void *addr, uint64 length, uint flags)`.
- Required flags: `MAP_SHARED | MAP_ANONYMOUS`.
- Optional flag: `MAP_FIXED`.
- Return mapped virtual address on success, `0` on failure.
- Support `munmap(void *addr)` for exact-start unmapping of a full mapping.
- Support `getmmapinfo(struct mmapinfo *)`.
- Realize pages lazily on page fault.
- Preserve sharing semantics across `fork()`.
- Prevent collisions between `mmap` regions, the heap (`sbrk` / `p->sz`), and the trapframe/trampoline region.
- Clean up shared mapping resources on `munmap`, `exit`, and `exec`.

## Current State Of The Repo

This is starter / partial-assignment code, not a finished solution.

- `sys_mmap()` and `sys_munmap()` are still stubs in `xv6/kernel/sysproc.c`.
- `getmmapinfo()` has a basic implementation in `xv6/kernel/proc.c`.
- `trap.c` already routes load/store page faults to `vmfault()`.
- `vmfault()` in `xv6/kernel/vm.c` currently only handles lazy `sbrk` pages below `p->sz`.
- `proc.h` has placeholder mmap-related per-process fields, but they are not sufficient for correct shared lazy mappings across forks.
- `mmaptests` is included in the build and encodes most assignment requirements.

## Most Important Files

- `readme.md`: assignment handout and requirements.
- `xv6/kernel/mmap.h`: mmap constants, flags, `struct mmapinfo`, suggested mapping struct.
- `xv6/kernel/proc.h`: process data structures.
- `xv6/kernel/proc.c`: process lifecycle, fork, exit, wait, `getmmapinfo`.
- `xv6/kernel/sysproc.c`: syscall entry points for `mmap`, `munmap`, `getmmapinfo`, `sbrk`.
- `xv6/kernel/trap.c`: user trap path and page-fault handling.
- `xv6/kernel/vm.c`: page table helpers, `copyin/copyout`, `vmfault`.
- `xv6/kernel/exec.c`: must clear mappings on `exec`.
- `xv6/user/mmaptests.c`: authoritative behavioral tests for mmap tasks.

## Working Model

Use a two-level design:

- Per-process mapping metadata:
  - virtual start address
  - length
  - flags
  - reference to a shared kernel mapping object
- Shared kernel mapping object:
  - refcount across processes
  - number of pages
  - per-page physical backing pointers
  - locking if needed

That split is necessary because:

- virtual placement is process-specific metadata
- realized pages must be shared across related processes
- cleanup must happen only when the last reference disappears

## Expected Validation Flow

Typical validation order:

1. `mmaptests -g task3`
2. `mmaptests -g task4`
3. `mmaptests -g task5` or `mmaptests`
4. full `usertests`

`xv6/user/mmaptests.c` groups:

- `task3`: metadata, placement, collision, fork inheritance
- `task4`: `munmap`, cleanup, exit/exec resource release
- `task5`: fault-time realization, zero-fill, shared visibility, bounds, orphan handling

## Notes For Future Codex Sessions

- Start by reading `readme.md` and `xv6/user/mmaptests.c`.
- Do not assume the current mmap data structures are correct.
- Be careful not to break lazy `sbrk`, normal `fork`, or `exec`.
- Prefer small, testable kernel changes with clear invariants.
