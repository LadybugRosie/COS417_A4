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

// Per-memory mapping state
struct memmap {
  int mid; // Mapping ID
  int alloced; // If non-zero, the memory has been allocated
  int shared; // If non-zero, the memory is shared
  int backed; // If non-zero, memory is file-backed

  uint64 ref_count; // Number of processes referencing this map
  uint64 page_count; // Number of pages in this mapping
  uint64 phys_pages[MAX_PAGES]; // If alloced non-zero, physical addresses of allocated pages

};
