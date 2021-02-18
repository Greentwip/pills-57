
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
// ���̨��ϲ�ۺ���
u32 gfx_ucode[][2] = {
	{ (u32)gspF3DEX2_fifoTextStart,		(u32)gspF3DEX2_fifoDataStart	},
	{ (u32)gspS2DEX_fifoTextStart,		(u32)gspS2DEX_fifoDataStart		},
};

OSMesgQueue gfx_msgQ;					// ү����Q
OSMesg		gfx_msgbuf[GFX_MESGS_MAX];	// ү�����ޯ̧
NNScClient	gfx_client;					// �ײ���
u32			wb_flag = 0;				// W�ޯ̧���޸�
GRAPHIC_NO	graphic_no = GFX_NULL;		// ���̨�� no.
OSThread	gfxThread;					// Gfx�گ��

OSMesgQueue *sched_gfxMQ;	// ���ޭ�ׂւ� Gfxү����Q

GFXMsg		gfx_msg;
GFXMsg		gfx_msg_no;
u64			gfxThreadStack[STACKSIZE/sizeof(u64)];
NNScTask	gfx_task[GFX_GTASK_NUM];
Gfx			gfx_glist[GFX_GTASK_NUM][GFX_GLIST_LEN];
Gfx*		gp;
u32			gfx_gtask_no;
u32			pendingGFX = 0;

//////////////////////////////////////////////////////////////////////////////
// �e�탁�C���ւ̎Q��

#include "dm_game_main.h"
#include "dm_manual_main.h"
#include "dm_title_main.h"
#include "main_menu.h"
extern void graphic_story(void);

//////////////////////////////////////////////////////////////////////////////
// ���̨�����Ұ��̏�����
void gfxInit(u8* gfxdlistStart_ptr)
{
	// ���̨������I��ү���ނ̐ݒ�
	gfx_msg.gen.type	= NN_SC_DONE_MSG;		// ����I�����ڰ��ޯ̧�؂�ւ�ү����
	gfx_msg_no.gen.type = NN_SC_GTASKEND_MSG;	// ����I���݂̂�ү����
	graphic_no			= GFX_NULL;				// ���̨���گ�ޕ����׸ނ̏�����
	gfx_gtask_no		= 0;
}

//////////////////////////////////////////////////////////////////////////////
// ���̨���گ��

static void gfxproc_onRetrace();
static void gfxproc_onDoneSwap();
static void gfxproc_onDoneTask();
static void gfxproc_onPreNMI();

void gfxproc(void *arg)
{
	short *msg_type = NULL;
	int preNMI = FALSE;

	pendingGFX = 0;

	// vi��ڰ��ɂ�����ү����Q�̸ش��
	osCreateMesgQueue(&gfx_msgQ, gfx_msgbuf, GFX_MESGS_MAX);
	nnScAddClient((NNSched *)arg, &gfx_client, &gfx_msgQ);

	// ���̨��ү����Q�̎擾
	sched_gfxMQ = nnScGetGfxMQ((NNSched*)arg);

	// ���̨������ٰ�� ... SHVC�ł�NMI(V-SYNC)�����ɂ����镔��
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

		// ��ڰ�ү���ޏ���
		case NN_SC_RETRACE_MSG:
			gfxproc_onRetrace();
			break;

		// ���ʕ��̸��̨������I��ү����
		case NN_SC_DONE_MSG:
			gfxproc_onDoneSwap();
			break;

		// ���̨������I��ү����
		case NN_SC_GTASKEND_MSG:
			gfxproc_onDoneTask();
			break;

		// PRE NMIү����
		case NN_SC_PRE_NMI_MSG:
			// PRE NMIү���ނ̏����������ɏ����Ă�������
			preNMI = TRUE;
			gfxproc_onPreNMI();
			break;

		default:
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// ���g���[�X�̏���

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

	// �V�^�C�g��
	case GFX_TITLE:
		if(pendingGFX < 2) {
			dm_title_graphic();
		}
		break;

	// �������
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
// �X���b�v����̏���

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
// �^�X�N��������̏���

static void gfxproc_onDoneTask()
{
	switch(graphic_no) {
	case GFX_GAME:
		dm_game_graphic_onDoneTask();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// PRE NMI �̏���

static void gfxproc_onPreNMI()
{
	// !!�d�v!! Y�X�P�[����1.0�ɖ߂�����
	osViSetYScale(1.0);

	// ������쐬���Ȃ��悤�ɂ���
	pendingGFX += 2;

	// ��ʂ��u���b�N�A�E�g
	osViBlack(TRUE);

	// ���C���X���b�h���~
	stopMainThread();

	// �S�T�E���h���~
	MusStop(MUSFLAG_SONGS|MUSFLAG_EFFECTS, 0);
}

//////////////////////////////////////////////////////////////////////////////
// ���̨���گ�ނ̍쐬�ƋN��
void gfxCreateGraphicThread(NNSched* sched)
{
	osCreateThread(&gfxThread, GRAPHIC_THREAD_ID, gfxproc,(void *) sched, gfxThreadStack+STACKSIZE/sizeof(u64), GRAPHIC_THREAD_PRI);
	osStartThread(&gfxThread);
}

//////////////////////////////////////////////////////////////////////////////
// ���ޭ�ׂ����ү���ޑ҂�
//	return :���ޭ�ׂ����ү���ނ̎��
//		OS_SC_RETRACE_MSG	 : ��ڰ�ү����
//		OS_SC_PRE_NMI_MSG	 :	PRE NMIү����
short gfxWaitMessage(void)
{
	short* msg_type = NULL;
	(void)osRecvMesg(&gfx_msgQ,(OSMesg *)&msg_type, OS_MESG_BLOCK);
	return *msg_type;
}

//////////////////////////////////////////////////////////////////////////////
// ���ޭ�ׂɸ��̨������N��ү���ނ𑗂�
//	NNScTask *gtask		:����\����
//	Gfx*	  gp :�ި���ڲؽĂ��߲��
//	s32		  glsit_size:�ި���ڲؽĻ���
//	u32		  ucode_type:ϲ�ۺ��ނ̎��(graphic.h�Q��)
//	u32		  flag		:�ڰ��ޯ̧�ύX�׸�
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

	// ����I��ү���ނ̐ݒ�
	if(flag & NN_SC_SWAPBUFFER) {
		// �P��ʕ���������I�������ꍇ
		gtask->msg = (OSMesg)&gfx_msg;
		pendingGFX++;
	}
	else {
		// �P��ʕ���������I�����Ă��Ȃ��ꍇ
		gtask->msg = (OSMesg)&gfx_msg_no;
	}

	// ����\�����ڰ��ޯ̧�w�� ...nnsched.c �ɂ� osViSwapBuffer(task->framebuffer);
	gtask->framebuffer = fbuffer[wb_flag];

	// ���̨������̋N��
	osSendMesg(sched_gfxMQ,(OSMesg*)gtask, OS_MESG_BLOCK);

	// �ڰ��ޯ̧�̐؂�ւ�
	if(flag & NN_SC_SWAPBUFFER) wb_flag ^= 1;

	// �ި���ڲؽ�, ����ޯ̧�؂�ւ�
	gfx_gtask_no++;
	gfx_gtask_no %= GFX_GTASK_NUM;

}

//////////////////////////////////////////////////////////////////////////////
// F3DEX,S2DEX: RSP,RDP�̏�����, �ڰ��ޯ̧/Z�ޯ̧�̸ر
static Vp vp[] = {
	{
		SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	// 320x240	scale
		SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,	//			translate
	},
};

void F3RCPinitRtn(void)
{
	static int rdpinit_flag = TRUE;									// RDP�������׸�

	gSPSegment(gp++, 0, 0x0);										// RSP������ڼ޽��̐ݒ�, CPU���z���ڽ�p
	gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3SetupRSP_dl));			// RSP�̐ݒ�
	gSPViewport(gp++, &vp[0]);
	if(TRUE==rdpinit_flag) {										// RDP�̏������i�P��̂݁j
		gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3RDPinit_dl));
		rdpinit_flag = 0;
	}
	gSPDisplayList(gp++, OS_K0_TO_PHYSICAL(F3SetupRDP_dl));			// RDP�̐ݒ�
	gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0,0, 320-1, 240-1);
}

void F3ClearFZRtn(u8 f )
{
	gDPSetCycleType(gp++, G_CYC_FILL);
	gDPSetColorImage(gp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, osVirtualToPhysical(fbuffer[wb_flag]));	// �ڰ��ޯ̧�̸ر
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
