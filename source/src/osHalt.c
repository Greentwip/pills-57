#include <rcp.h>
#include <os.h>
#include "osHalt.h"

OSPiHandle *osHaltHandle() {
	static OSPiHandle handle;
	static int initialized = 0;

	if(initialized) {
		return &handle;
	}

	handle.type        = DEVICE_TYPE_CART;
	handle.baseAddress = PHYS_TO_K1(HALT_REG_START_ADDR);
	handle.latency     = HALT_REG_latency;
	handle.pulse       = HALT_REG_pulse;
	handle.pageSize    = HALT_REG_pageSize;
	handle.relDuration = HALT_REG_relDuration;
	handle.domain      = PI_DOMAIN2;
	handle.speed       = 0;
	bzero(&handle.transferInfo, sizeof(handle.transferInfo));

	osEPiLinkHandle(&handle);
	initialized = 1;

	return &handle;
}

void osHaltSet(int flag) {
	osEPiWriteIo(osHaltHandle(), HALT_REG_START_ADDR,
		flag ? HALT_REG_ON : HALT_REG_OFF);
}

