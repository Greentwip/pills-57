
#include "code_entry.h"
#include "util.h"

#define CODE_IMPL(seg, func) _##seg##SegmentStart, mainproc, _##seg##SegmentBssStart, _##seg##SegmentBssEnd,

extern void mainproc(void *);
MSegDecl(code)

CODE_ENTRY _codeEntryTbl[] = {
	CODE_IMPL(code, mainproc)
};

