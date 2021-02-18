#if !defined(_OS_HALT_H_)
#define _OS_HALT_H_

#define HALT_REG_START_ADDR  0x0ff70000
#define HALT_REG_SIZE        0x32
#define HALT_REG_latency     0x05
#define HALT_REG_pulse       0x0c
#define HALT_REG_pageSize    0x0d
#define HALT_REG_relDuration 0x02

#define HALT_REG_ON  0xffffffff
#define HALT_REG_OFF 0x00000000

extern OSPiHandle *osHaltHandle();
extern void osHaltSet(int flag);

#endif // _OS_HALT_H_
