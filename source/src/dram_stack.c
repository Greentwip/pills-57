#include <ultra64.h>

/* dram stack buffer */
#ifdef LINUX_BUILD
u64 dram_stack[SP_DRAM_STACK_SIZE64] __attribute__((aligned(16)));
#else
u64 dram_stack[SP_DRAM_STACK_SIZE64];
#endif
