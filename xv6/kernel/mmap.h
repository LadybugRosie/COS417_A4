// Maximum number of `mmap`s per process:
#define MAX_MMAPS 64

// Maximum number of 'pages' per mmap:
#define MAX_PAGES 512

// Flags for the `mmap` system call:
#define MAP_SHARED 0x0002
#define MAP_ANONYMOUS 0x0004
#define MAP_FIXED 0x0008

// Struct filled by the `getmmapinfo` system call:
struct mmapinfo {
    uint64 total_mmaps;               // Total number of mmap regions
    void* addr[MAX_MMAPS];            // Starting address of each mapping
    uint64 length[MAX_MMAPS];         // Size of each mapping
    uint64 n_loaded_pages[MAX_MMAPS]; // Number of pages physically loaded into
                                      // memory across all mappings
};

// Allowed flags for this assignment's mmap implementation.
#define MAP_SUPPORTED (MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED)

// Shared backing object for one mmap region.
//
// `phys_pages` stores realized physical page addresses as 32-bit values.
// xv6's physical memory fits below 4GB, so 32 bits are sufficient and let
// this structure fit comfortably in a single kalloc() page.
struct mmap_area {
  uint64 ref_count;            // Number of processes referencing this mapping
  uint64 length;               // Rounded-up mapping length in bytes
  uint64 page_count;           // Rounded-up mapping length in pages
  uint64 loaded_pages;         // Number of pages realized so far
  uint flags;                  // MAP_* flags for this region
  uint phys_pages[MAX_PAGES];  // Realized physical page addresses, 0 if absent
};

// Per-process mmap metadata.
struct proc_mmap {
  uint64 addr;                 // Start address in this process
  struct mmap_area *area;      // Shared backing object
};
