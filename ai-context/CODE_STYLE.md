# Code Style

## General Expectations

Follow existing `xv6` style unless there is a strong reason not to.

- Keep functions small and direct.
- Prefer straightforward C over abstraction-heavy patterns.
- Match nearby naming and formatting.
- Avoid introducing unnecessary helper layers.

## Formatting

- Use simple C formatting consistent with surrounding files.
- Keep comments short and technical.
- Prefer one blank line between logical blocks, not dense comment-heavy sections.
- Use ASCII only.

## Kernel Editing Conventions

- Preserve xv6 naming where possible: `kfork`, `kexit`, `uvmalloc`, `vmfault`, etc.
- Prefer explicit integer and pointer handling over clever macros.
- Be careful with page alignment:
  - `PGROUNDUP`
  - `PGROUNDDOWN`
  - `PGSIZE`
- When manipulating page tables, keep permission bits obvious and local.

## State Management

- Be explicit about which state is per-process versus shared across processes.
- Initialize all new process fields in `allocproc()`.
- Make lifecycle cleanup symmetrical:
  - allocate or attach in `mmap` / `fork`
  - detach in `munmap`, `exec`, `exit`, `freeproc`
- Avoid duplicating ownership of physical pages across multiple structures unless clearly justified.

## Error Handling

- On syscall failure, return the value required by the assignment:
  - `mmap`: `0`
  - `munmap`: non-zero error, usually `-1`
  - `getmmapinfo`: `-1` on copyout failure
- Validate arguments early:
  - flags
  - zero length
  - alignment for `MAP_FIXED`
  - address bounds
  - mapping limits
- On partial failure, unwind allocations immediately and conservatively.

## Concurrency / Correctness

- If shared mmap objects are introduced, give them clear locking or reference-count rules.
- Keep fault-time behavior race-safe. Two processes faulting the same shared page should not create two different backing pages that both survive.
- Do not rely on process-local metadata alone for shared backing semantics.

## Testing Discipline

Prefer this order:

1. targeted `mmaptests` case
2. `mmaptests -g task3`
3. `mmaptests -g task4`
4. `mmaptests -g task5`
5. full `usertests`

When a regression appears, inspect the corresponding test in `xv6/user/mmaptests.c` before changing code.

## Practical Guidance For Future Codex Sessions

- Read before editing:
  - `readme.md`
  - `xv6/kernel/mmap.h`
  - `xv6/kernel/proc.h`
  - `xv6/kernel/proc.c`
  - `xv6/kernel/sysproc.c`
  - `xv6/kernel/trap.c`
  - `xv6/kernel/vm.c`
  - relevant parts of `xv6/user/mmaptests.c`
- Treat `mmaptests` as executable specification.
- Prefer minimal diffs that preserve non-mmap behavior.
