#ifndef DEF_H
#define DEF_H

// segment for spec file
#define ST_SEGMENT  5 // Ohkura追加 // ストーリーモード用

// screen size
#define SCREEN_HT 240
#define SCREEN_WD 320

#define STACKSIZE           0x2000 //
#define GLIST_LEN           2048   //
#define GFX_FREEBUF_NUM     0x1b   // ﾌﾘｰｴﾘｱﾊﾞﾝｸ数
#define GFX_FREEBUF_SIZE    0x2000 //      ..     ( x 64bits )
#define GFX_RDP_OUTPUT_SIZE 1024*8 // RDP fifoﾊﾞｯﾌｧ

// thread priority
#define IDLE_THREAD_PRI    1
#define RMON_THREAD_PRI    OS_PRIORITY_RMON
#define GRAPHIC_THREAD_PRI 15
#define MAIN_THREAD_PRI    10

// thread id (for debug)
#define IDLE_THREAD_ID    1
#define RMON_THREAD_ID    2
#define MAIN_THREAD_ID    3
#define GRAPHIC_THREAD_ID 5

#ifdef _LANGUAGE_C
#endif // _LANGUAGE_C

#endif // DEF_H
