#if !defined(_CODE_ENTRY_H_)
#define _CODE_ENTRY_H_

typedef struct {
	void *start;
	void (*entry)(void *);
	void *bssStart;
	void *bssEnd;
} CODE_ENTRY;

#endif // _CODE_ENTRY_H_
