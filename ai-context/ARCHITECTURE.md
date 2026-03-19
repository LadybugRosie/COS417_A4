# Architecture

## Repository Layout

Top level:

- `readme.md`: assignment handout
- `xv6/`: teaching OS source tree

Important `xv6` subtrees:

- `xv6/kernel/`: kernel code
- `xv6/user/`: user programs and tests
- `xv6/Makefile`: build and QEMU entry points
- `xv6/test-xv6.py`: helper test runner

## Assignment-Relevant Flow

### 1. User space syscall entry

User programs call:

- `mmap`
- `munmap`
- `getmmapinfo`

Declarations are in `xv6/user/user.h`.
Assembly stubs are generated from `xv6/user/usys.pl`.

### 2. Syscall dispatch

`xv6/kernel/syscall.c` maps syscall numbers to:

- `sys_getmmapinfo`
- `sys_mmap`
- `sys_munmap`

The actual syscall handlers live in `xv6/kernel/sysproc.c`.

### 3. Process state

`xv6/kernel/proc.h` defines `struct proc`.

This is where per-process mmap metadata belongs. A correct solution usually needs:

- mapping count
- virtual address range metadata
- linkage from each process-visible mapping to shared backing state

`xv6/kernel/proc.c` manages lifecycle points that matter to mmap:

- `allocproc()`: initialize mmap state
- `kfork()`: inherit mappings and update sharing references
- `kexit()` / `freeproc()`: drop references and release resources
- `growproc()`: prevent `sbrk` growth into mappings
- `mmapinfo()`: report current mapping state

### 4. Virtual memory and faults

`xv6/kernel/trap.c` handles user traps.

For page faults:

- load fault: `scause == 13`
- store fault: `scause == 15`

These faults are already routed to `vmfault()` in `xv6/kernel/vm.c`.

`vmfault()` currently handles lazy heap allocation for `sbrk`.
A complete assignment solution extends it to:

- detect whether the fault address is inside an mmap region
- locate the correct mapping and page index
- allocate a backing page only if not already realized
- map the page into the current process page table
- reuse already-realized pages for other processes sharing the mapping

### 5. Address-space replacement

`xv6/kernel/exec.c` replaces the user address space during `exec`.

Mappings must not survive `exec`, so any mmap state and shared references associated with the old process image must be cleared before or during replacement.

## Existing Lazy Allocation Path

The repo already supports lazy `sbrk`.

Current logic:

- `sys_sbrk()` may increase `p->sz` without allocating pages
- a later page fault enters `usertrap()`
- `usertrap()` calls `vmfault()`
- `vmfault()` allocates a zeroed physical page and maps it into the process

For mmap, the same trap path should be reused, but the backing decision is different:

- `sbrk` pages are private and indexed by fault address below `p->sz`
- `mmap` pages are region-based and may be shared across processes

## Conceptual Data Model

The cleanest design is to separate:

### Process mapping entry

Tracks:

- start virtual address
- rounded-up length
- flags
- pointer or identifier for shared backing object

### Shared mapping object

Tracks:

- total page count
- number of referencing processes
- realized physical page pointers per page index
- synchronization for concurrent faults / unmaps

This structure satisfies:

- shared lazy realization
- late fault handling
- cleanup on final dereference

## Collision Rules

The implementation must enforce:

- mappings cannot overlap each other
- mappings cannot overlap current or future heap space
- heap growth cannot pass into an existing mapping
- mappings cannot overlap trapframe/trampoline area
- `MAP_FIXED` must either use exactly the requested address or fail
- non-`MAP_FIXED` mappings should prefer high addresses to preserve heap growth space

## Test Structure

The primary spec is in `xv6/user/mmaptests.c`.

Task groups:

- `task3`: metadata and placement behavior
- `task4`: unmapping and cleanup
- `task5`: realization and true sharing behavior

Use these tests as behavioral checkpoints while implementing.
