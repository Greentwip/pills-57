#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

#define GFX_MESGS_MAX 8
#define GFX_GLIST_LEN (1024*4)

typedef enum {
	GFX_NULL,
	GFX_STORY,
	GFX_TITLE,
	GFX_MANUAL,
	GFX_GAME,
	GFX_MENU,
	GFX_TECHMES,
	GFX_BOOT_ERROR,
} GRAPHIC_NO;

// ���̨��ϲ�ۺ���
enum {
	GFX_GSPCODE_F3DEX, // 0
	GFX_GSPCODE_S2DEX, // 1
};

// ���̨�������
#define GFX_GTASK_NUM 3

// ���̨�����ү���ލ\����
typedef union {
	struct {
		short	type;
	} gen;
	struct {
		short	type;
	} done;
	NNScMsg	  app;
} GFXMsg;

// parameter
extern GRAPHIC_NO	graphic_no;
extern OSMesgQueue	*sched_gfxMQ;
extern OSMesgQueue	gfx_msgQ;
extern Gfx			*gp;
extern u32			wb_flag;				// ��gfx_cfbdrawbuffer
extern NNScTask		gfx_task[];
extern Gfx			gfx_freebuf[GFX_FREEBUF_NUM][GFX_FREEBUF_SIZE];
extern Gfx			gfx_glist[GFX_GTASK_NUM][GFX_GLIST_LEN];
extern u32			gfx_gtask_no;
extern u32			pendingGFX ;

// buffer
extern u16 fbuffer[][SCREEN_WD*SCREEN_HT];		// double frame buffer
//extern u16 zbuffer[];    						// memory frame buffer
extern u64 rdp_output[];  						// RSP writes back RDP data
extern u64 dram_stack[];         				// used for matrix stack
extern u64 gfxYieldBuf[];						//

// functions
extern void		gfxInit(u8*);									// ���̨�����Ұ��̏�����
extern void		gfxCreateGraphicThread(NNSched*);				// ���̨���گ�ނ̍쐬�ƋN��
extern short	gfxWaitMessage(void);							// ���ޭ�ׂ����ү���ޑ҂�
extern void		gfxTaskStart(NNScTask*, Gfx*, s32, u32, u32);	// ���ޭ�ׂɸ��̨������N��ү���ނ𑗂�
extern void		F3RCPinitRtn();
extern void		F3ClearFZRtn(u8);
extern void		S2RDPinitRtn(u8);
extern void		S2ClearCFBRtn(u8);

#endif

