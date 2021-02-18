
#ifndef MAIN_H
#define MAIN_H

// main loop ID no.
typedef enum {
	MAIN_11,
	MAIN_12,
	MAIN_STORY,
	MAIN_TITLE,
	MAIN_MANUAL,
	MAIN_GAME,
	MAIN_MENU,
	MAIN_TECHMES,
	MAIN_CONT_ERROR,
	MAIN_TV_ERROR,
	MAIN_CSUM_ERROR,
} MAIN_NO;

extern void stopMainThread();
extern void setIdleFunc(void *func);

extern MAIN_NO main_no;
extern MAIN_NO main_old;

#endif
