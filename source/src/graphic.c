
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <libmus.h>
#include <libmus_data.h>
#include <assert.h>

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "graphic.h"
#include "static.h"

#include "main.h"
#include "cpu_task.h"

//////////////////////////////////////////////////////////////////////////////
// ｸﾞﾗﾌｨｯｸﾏｲｸﾛｺｰﾄﾞ
u32 gfx_ucode[][2] = {
	{ (u32)gspF3DEX2_fifoTextStart,		(u32)gspF3DEX2_fifoDataStart	},
	{ (u32)gspS2DEX_fifoTextStart,		(u32)gspS2DEX_fifoDataStart		},
};

OSMesgQueue gfx_msgQ;					// ﾒｯｾｰｼﾞQ
OSMesg		gfx_msgbuf[GFX_MESGS_MAX];	// ﾒｯｾｰｼﾞﾊﾞｯﾌｧ
NNScClient	gfx_client;					// ｸﾗｲｱﾝﾄ
u32			wb_flag = 0;				// Wﾊﾞｯﾌｧｲﾝﾃﾞｸｽ
GRAPHIC_NO	graphic_no = GFX_NULL;		// ｸﾞﾗﾌｨｯｸ no.
OSThread	gfxThread;					// Gfxｽﾚｯﾄﾞ

OSMesgQueue *sched_gfxMQ;	// ｽｹｼﾞｭｰﾗへの GfxﾒｯｾｰｼﾞQ

GFXMsg		gfx_msg;
GFXMsg		gfx_msg_no;
u64			gfxThreadStack[STACKSIZE/sizeof(u64)];
NNScTask	gfx_task[GFX_GTASK_NUM];
Gfx			gfx_glist[GFX_GTASK_NUM][GFX_GLIST_LEN];
Gfx*		gp;
u32			gfx_gtask_no;
u32			pendingGFX = 0;

//////////////////////////////////////////////////////////////////////////////
// 各種メインへの参照

#include "dm_game_main.h"
#include "dm_manual_main.h"
#include "dm_title_main.h"
#include "main_menu.h"
extern void graphic_story(void);

//////////////////////////////////////////////////////////////////////////////
// ｸﾞﾗﾌｨｯｸﾊﾟﾗﾒｰﾀの初期化
void gfxInit(u8* gfxdlistStart_ptr)
{
	// ｸﾞﾗﾌｨｯｸﾀｽｸ終了ﾒｯｾｰｼﾞの設定
	gfx_msg.gen.type	= NN_SC_DONE_MSG;		// ﾀｽｸ終了とﾌﾚｰﾑﾊﾞｯﾌｧ切り替えﾒｯｾｰｼﾞ
	gfx_msg_no.gen.type = NN_SC_GTASKEND_MSG;	// ﾀｽｸ終了のみのﾒｯｾｰｼﾞ
	graphic_no			= GFX_NULL;				// ｸﾞﾗﾌｨｯｸｽﾚｯﾄﾞ分岐ﾌﾗｸﾞの初期化
	gfx_gtask_no		= 0;
}

//////////////////////////////////////////////////////////////////////////////
// ｸﾞﾗﾌｨｯｸｽﾚｯﾄﾞ

static void gfxproc_onRetrace();
static void gfxproc_onDoneSwap();
static void gfxproc_onDoneTask();
static void gfxproc_onPreNMI();

void gfxproc(void *arg)
{
	short *msg_type = NULL;
	int preNMI = FALSE;

	pendingGFX = 0;

	// viﾘﾄﾚｰｽにおけるﾒｯｾｰｼﾞQのｸﾘｴｲﾄ
	osCreateMesgQueue(&gfx_msgQ, gfx_msgbuf, GFX_MESGS_MAX);
	nnScAddClient((NNSched *)arg, &gfx_client, &gfx_msgQ);

	// ｸﾞﾗﾌｨｯｸﾒｯｾｰｼﾞQの取得
	sched_gfxMQ = nnScGetGfxMQ((NNSched*)arg);

	// ｸﾞﾗﾌｨｯｸ処理ﾙｰﾌﾟ ... SHVCでのNMI(V-SYNC)処理にあたる部分
	while(1) {
		osRecvMesg(&gfx_msgQ, (OSMesg *)&msg_type, OS_MESG_BLOCK);

#if 0
/*
		if(preNMI) {
			u32 i, color, *ptr, *ptrEnd;

			if(cpuTask_getTaskCount()) {
				color = 0xf801f801;
			}
			else {
				color = 0x003f003f;
			}

			ptr = (u32 *)fbuffer[wb_flag^1];
			ptrEnd = ptr + SCREEN_WD * 32;
			for(; ptr < ptrEnd; ptr++) {
				*ptr = color;
			}
		}
*/
#endif

		switch(*msg_type) {

		// ﾘﾄﾚｰｽﾒｯｾｰｼﾞ処理
		case NN_SC_RETRACE_MSG:
			gfxproc_onRetrace();
			break;

		// 一画面分のｸﾞﾗﾌｨｯｸﾀｽｸ終了ﾒｯｾｰｼﾞ
		case NN_SC_DONE_MSG:
			gfxproc_onDoneSwap();
			break;

		// ｸﾞﾗﾌｨｯｸﾀｽｸ終了ﾒｯｾｰｼﾞ
		case NN_SC_GTASKEND_MSG:
			gfxproc_onDoneTask();
			break;

		// PRE NMIﾒｯｾｰｼﾞ
		case NN_SC_PRE_NMI_MSG:
			// PRE NMIﾒｯｾｰｼﾞの処理をここに書いてください
			preNMI = TRUE;
			gfxproc_onPreNMI();
			break;

		default:
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// リトレースの処理

static void gfxproc_onRetrace()
{
#if defined(NN_SC_PERF)
	nnScPushPerfMeter(NN_SC_PERF_GRPHg);
#endif

	switch(graphic_no) {

	case GFX_STORY:
		if(pendingGFX < 2) {
			graphic_story();
		}
		break;

	// 新タイトル
	case GFX_TITLE:
		if(pendingGFX < 2) {
			dm_title_graphic();
		}
		break;

	// 操作説明
	case GFX_MANUAL:
		if(pendingGFX < 2) {
			dm_manual_graphic();
		}
		break;

	case GFX_GAME:
		if(pendingGFX < 1) {
			dm_game_graphic();
		}
		break;

	case GFX_MENU:
		if(pendingGFX < 2) {
			graphic_menu();
		}
		break;

	case GFX_TECHMES:
		if(pendingGFX < 2) {
			graphic_techmes();
		}
		break;

	case GFX_BOOT_ERROR:
		if(pendingGFX < 2) {
			graphic_boot_error();
		}
		break;

	case GFX_NULL:
		break;
	}

#if defined(NN_SC_PERF)
	nnScPopPerfMeter();
#endif
}

//////////////////////////////////////////////////////////////////////////////
// スワップ直後の処理

static void gfxproc_onDoneSwap()
{
	pendingGFX--;

	switch(graphic_no) {
	case GFX_GAME:
//		dm_game_graphic_onDoneSawp();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// タスク完了直後の処理

static void gfxproc_onDoneTask()
{
	switch(graphic_no) {
	case GFX_GAME:
		dm_game_graphic_onDoneTask();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRE NMI の処理

static void gfxproc_onPreNMI()
{
	// !!重要!! Yスケールを1.0に戻すこと
	osViSetYScale(1.0);

	// ﾀｽｸを作成しないようにする
	pendingGFX += 2;

	// 画面をブラックアウト
	osViBlack(TRUE);

	// メインスレッドを停止
	stopMainThread();

	// 全サウンドを停止
	MusStop(MUSFLAG_SONGS|MUSFLAG_EFFECTS, 0);
}

//////////////////////////////////////////////////////////////////////////////
// ｸﾞﾗﾌｨｯｸｽﾚｯﾄﾞの作成と起動
void gfxCreateGraphicThread(NNSched* sched)
{
	osCreateThread(&gfxThread, GRAPHIC_THREAD_ID, gfxproc,(void *) sched, gfxThreadStack+STACKSIZE/sizeof(u64), GRAPHIC_THREAD_PRI);
	osStartThread(&gfxThread);
}

//////////////////////////////////////////////////////////////////////////////
// ｽｹｼﾞｭｰﾗからのﾒｯｾｰｼﾞ待ち
//	return :ｽｹｼﾞｭｰﾗからのﾒｯｾｰｼﾞの種類
//		OS_SC_RETRACE_MSG	 : ﾘﾄﾚｰｽﾒｯｾｰｼﾞ
//		OS_SC_PRE_NMI_MSG	 :	PRE NMIﾒｯｾｰｼﾞ
short gfxWaitMessage(void)
{
	short* msg_type = NULL;
	(void)osRecvMesg(&gfx_msgQ,(OSMesg *)&msg_type, OS_MESG_BLOCK);
	return *msg_type;
}

//////////////////////////////////////////////////////////////////////////////
// ｽｹｼﾞｭｰﾗにｸﾞﾗﾌｨｯｸﾀｽｸ起動ﾒｯｾｰｼﾞを送る
//	NNScTask *gtask		:ﾀｽｸ構造体
//	Gfx*	  gp :ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄのﾎﾟｲﾝﾀ
//	s32		  glsit_size:ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄｻｲｽﾞ
//	u32		  ucode_type:ﾏｲｸﾛｺｰﾄﾞの種類(graphic.h参照)
//	u32		  flag		:ﾌﾚｰﾑﾊﾞｯﾌｧ変更ﾌﾗｸﾞ
void gfxTaskStart(NNScTask *gtask,Gfx* gp, s32 glist_size, u32 ucode_type, u32 flag)
{

	gtask->list.t.data_ptr			= (u64*)gp;
	gtask->list.t.data_size			=		glist_size;
	gtask->list.t.type				=		M_GFXTASK;
	gtask->list.t.flags				=		OS_TASK_LOADABLE | OS_TASK_DP_WAIT;
	gtask->list.t.ucode_boot		= (u64*)rspbootTextStart;
	gtask->list.t.ucode_boot_size	= ((s32)rspbootTextEnd - (s32)rspbootTextStart);
	gtask->list.t.ucode				= (u64*)gfx_ucode[ucode_type][0];
	gtask->list.t.ucode_data		= (u64*)gfx_ucode[ucode_type][1];
	gtask->list.t.ucode_data_size	=		SP_UCODE_DATA_SIZE;
	gtask->list.t.dram_stack		= (u64*)dram_stack;
	gtask->list.t.dram_stack_size	=		SP_DRAM_STACK_SIZE8;
	gtask->list.t.output_buff		= (u64*)&rdp_output[0];
	gtask->list.t.output_buff_size	= (u64*)&rdp_output[GFX_RDP_OUTPUT_SIZE];

	gtask->list.t.yield_data_ptr	= (u64*)gfxYieldBuf;
	gtask->list.t.yield_data_size	=		OS_YIELD_DATA_SIZE;

	gtask->next						=		0;
	gtask->flags					=		flag;
	gtask->msgQ						=		&gfx_msgQ;

	// ﾀｽｸ終了ﾒｯｾｰｼﾞの設定
	if(flag & NN_SC_SWAPBUFFER) {
		// １画面分のﾀｽｸが終了した場合
		gtask->msg = (OSMesg)&gfx_msg;
		pendingGFX++;
	}
	else {
		// １画面分のﾀｽｸが終了していない場合
		gtask->msg = (OSMesg)&gfx_msg_no;
	}

	// 次回表示のﾌﾚｰﾑﾊﾞｯﾌｧ指定 ...nnsched.c にて osViSwapBuffer(task->framebuffer);
	gtask->framebuffer = fbuffer[wb_flag];

	// ｸﾞﾗﾌｨｯｸﾀｽｸの起動
	osSendMesg(sched_gfxMQ,(OSMesg*)gtask, OS_MESG_BLOCK);

	// ﾌﾚｰﾑﾊﾞｯﾌｧの切り替え
	if(flag & NN_SC_SWAPBUFFER) wb_flag ^= 1;

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ, ﾀｽｸﾊﾞｯﾌｧ切り替え
	gfx_gtask_no++;
	gfx_gtask_no %= GFX_GTASK_NUM;

}

//////////////////////////////////////////////////////////////////////////////
// F3DEX,S2DEX: RSP,RDPの初期化, ﾌﾚｰﾑﾊﾞｯﾌｧ/Zﾊﾞｯﾌｧのｸﾘｱ
static Vp vp[] = {
	{
		SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	// 320x240	scale
		SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	//			translate
	},
};

void F3RCPinitRtn(void)
{
	static int rdpinit_flag = TRUE;									// RDP初期化ﾌﾗｸﾞ

	gSPSegment(gp++, 0, 0x0);										// RSPｾｸﾞﾒﾝﾄﾚｼﾞｽﾀの設定, CPU仮想ｱﾄﾞﾚｽ用
	gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3SetupRSP_dl));			// RSPの設定
	gSPViewport(gp++, &vp[0]);
	if(TRUE==rdpinit_flag) {										// RDPの初期化（１回のみ）
		gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3RDPinit_dl));
		rdpinit_flag = 0;
	}
	gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3SetupRDP_dl));			// RDPの設定
	gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0,0, 320-1, 240-1);
}

void F3ClearFZRtn(u8 f )
{
	gDPSetCycleType(gp++, G_CYC_FILL);
	gDPSetColorImage(gp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, osVirtualToPhysical(fbuffer[wb_flag]));	// ﾌﾚｰﾑﾊﾞｯﾌｧのｸﾘｱ
	if(f) {
		gDPSetFillColor(gp++, (GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1)));
		gDPFillRectangle(gp++, 0, 0, 320-1, 240-1);
	}
	gDPPipeSync(gp++);
	gDPSetCycleType(gp++, G_CYC_1CYCLE);

}

void S2RDPinitRtn(u8 f)
{
	gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(S2RDPinit_dl));
	if(f) {
		gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1 );
	} else {
		gDPSetScissor(gp++, G_SC_NON_INTERLACE, 12, 8, 320-12-1, 240-8-1 );
	}
};

void S2ClearCFBRtn(u8 f)
{
	gDPSetColorImage(gp ++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, osVirtualToPhysical(fbuffer[wb_flag]));
	if(f) {
		gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(S2ClearCFB_dl));
		gDPFillRectangle(gp++, 0, 0, 320-1, 240-1);
	}
};
