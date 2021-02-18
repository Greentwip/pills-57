//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_title_main.c
//	create		:	1999/03/08
//	modify		:	1999/12/01
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#ifndef LINUX_BUILD
#include <math.h>
#endif

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "audio.h"
#include "joy.h"
#include "static.h"
#include "tex_func.h"

#include "evsworks.h"
#include "record.h"
#include "dm_title_main.h"
#include "main_story.h"
#include "msgwnd.h"

#include "util.h"
#include "debug.h"
#include "dbg_font.h"
#include "local.h"

#define _getKeyTrg(playerNo) ((int)joyupd[main_joy[(playerNo)]])

static int title_demo_flg = 0;  // �f���ɔ�Ԃ��A��������ɔ�Ԃ��̃t���O
static int title_demo_no = 0;   // �ǂ̃f���ɔ�Ԃ�
static int title_manual_no = 0; // �ǂ̑�������ɔ�Ԃ�

static int title_exit_flag = 0;
static int title_mode_type;  // ��������ϐ�

static int title_fade_count;
static int title_fade_step;

//////////////////////////////////////////////////////////////////////////////
// �^�C�g���������֐�

void dm_title_init(void)
{
	int i, j;

	title_exit_flag = 0;
	title_mode_type = 0; // ��������̏�����

	title_fade_count = (main_old == MAIN_MENU ? 255 : 0);
	title_fade_step = -8;

	evs_seqence = FALSE;

	init_title((u32)_codeSegmentEnd, main_old != MAIN_MENU);
}

//////////////////////////////////////////////////////////////////////////////
// �^�C�g���������C���֐�

static const u8 _stageTbl[] = {
	STORY_M_ST1, STORY_M_ST2, STORY_M_ST3, STORY_M_ST4,
	STORY_M_ST5, STORY_M_ST6, STORY_M_ST7, STORY_M_ST8,
	STORY_M_ST9,
	STORY_W_ST1, STORY_W_ST2, STORY_W_ST3, STORY_W_ST4,
	STORY_W_ST5, STORY_W_ST6, STORY_W_ST7, STORY_W_ST8,
	STORY_W_ST9,
};

int dm_title_main(NNSched *sched)
{
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	int i, j;
	int sec_count = (main_old == MAIN_MENU) ? 99 : 0;
	int out_flg = 0; // ���[�v�E�o�p�t���O

	// ���b�Z�[�W�L���[�̍쐬
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// �N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);

	// �V���b�t��
	sgenrand( osGetCount() );

	// �ϐ�������
	dm_title_init();

	// �R���g���[���ڑ��󋵊m�F
	evs_playmax = joyResponseCheck();

	PRT0("Start Title\n");

	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

	// ���C�����[�v
	while(out_flg == 0) {
		joyProcCore();

		// ���g���[�X���b�Z�[�W
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		// �t�F�[�h�C���E�A�E�g�̐���
		title_fade_count = CLAMP(0, 255, title_fade_count + title_fade_step);

		switch(title_mode_type) {
		case 0:
			// �^�C���A�E�g����
			sec_count++;
			if(title_exit_flag == -1) {
				title_mode_type = 7;
				break;
			}

			// �V�[�P���X���Đ�
			if(sec_count == 100) {
				dm_seq_play(SEQ_Title);
			}

			// �L�[��t����
			if(title_exit_flag == 1) {
				title_mode_type = 6;
//				dm_snd_play(SE_mGameStart);

#if !defined(NINTENDO)
				for(i = 0; i < MAXCONTROLLERS; i++) {
					if(joynew[i] & DM_KEY_Z) {

						// �B���L�������g�p�\�ɂ���
						evs_secret_flg[0] = evs_secret_flg[1] = 1;

						// �S�X�e�[�W���e
						for(i = 0; i < 9; i++) {
							mem_char *mc = &evs_mem_data[i];
							mc->clear_stage[0][0] = mc->clear_stage[0][1] = 7;
							mc->clear_stage[1][0] = mc->clear_stage[1][1] = 7;
							mc->clear_stage[2][0] = mc->clear_stage[2][1] = 7;
							mc->clear_stage[3][0] = mc->clear_stage[3][1] = 7;
						}

						// �ςȉ����Đ�
						dm_snd_play_strange_sound();

						title_mode_type = 6;
						break;
					}
				}
#endif
			}
			break;

		case 6: case 7:
			title_fade_step = -title_fade_step;

			if(title_mode_type == 6) {
				// �Q�[����
				out_flg = 1;
			}
			else if(title_mode_type == 7) {
				// �f����
				out_flg = 2;
			}
			break;
		}

		// �I�[�f�B�I�̃��C������
		dm_audio_update();

		// �O���t�B�b�N�̐ݒ�
		graphic_no = GFX_TITLE;
	}

	// ���y��~
	dm_seq_stop();

	// �O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while(!dm_audio_is_stopped() || title_fade_count < 255) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();

		// �t�F�[�h�C���E�A�E�g�̐���
		title_fade_count = CLAMP(0, 255, title_fade_count + title_fade_step);
	}

	// �\����~
	graphic_no = GFX_NULL;

	// �O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while(pendingGFX != 0) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// �N���C�A���g�̖���
	nnScRemoveClient(sched, &client);

	if(out_flg == 1) {
		return MAIN_MENU;
	}
	else if(out_flg == 2) {
		if(title_demo_flg == 0) {
			// �w�i���w��
			story_proc_no = _stageTbl[rand() % ARRAY_SIZE(_stageTbl)];

			// �Q�[���f���̏ꍇ
			if(title_demo_no == 0) {
				// 1P�f����
				evs_gamesel = GSL_1DEMO;
			}
			else if(title_demo_no == 1) {
				// 2P�f����
				evs_gamesel = GSL_2DEMO;
			}
			else if(title_demo_no == 2) {
				// 4P�f����
				evs_gamesel = GSL_4DEMO;
			}
			evs_gamemode = GMD_NORMAL;
			title_demo_no++;

			// ���݂�4P�f���͍s��Ȃ�
			if(title_demo_no > 2) {
				title_demo_no = 0;
			}

			//�f���Ƒ�����������݂ɍs��
			title_demo_flg ^= 1;

			return MAIN_12;
		}
		else if(title_demo_flg == 1) {
			// ��������̏ꍇ
			evs_manual_no = title_manual_no;
			title_manual_no++;
			if(title_manual_no > 3) {
				title_manual_no = 0;
			}

			//�f���Ƒ�����������݂ɍs��
			title_demo_flg ^= 1;

			return MAIN_MANUAL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// �^�C�g���`�惁�C���֐�

void dm_title_graphic(void)
{
	NNScTask *gt;

	// �ި���ڲؽ��ޯ̧�����ޯ̧�̎w��
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	gSPSegment(gp++, 0, 0x0);

	// RSP �����ݒ�
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	// �`��͈͂̎w��
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	title_exit_flag = demo_title(&gp, evs_level_21 ? 1 : 0);

	// �t�F�[�h�t�B���^
	{
		int c, a;
		c = 255;
		a = CLAMP(0, 255, (title_fade_count - 127) * 1.2 + 127);
		if(a > 0) {
			FillRectRGBA(&gp, 0, 0, SCREEN_WD, SCREEN_HT, c, c, c, a);
		}
	}

#if defined(NN_SC_PERF)
	nnScWritePerfMeter(&gp, 32, 180);
#endif

#if defined(ROM_VERSION)
	DbgCon_Open(&gp, NULL, 32, 32);
	DbgCon_Format("%s\n%s\n", _debugVersionDate, _debugVersionTime);
//	DbgCon_Format("OS %s patch %d\n", OS_MAJOR_VERSION, OS_MINOR_VERSION);
	DbgCon_Close();
#endif

	/*------------------*/
	// �ި���ڲؽč쐬�I��, ���̨������̊J�n
	gDPFullSync(gp++);			// �ި���ڲؽčŏI����
	gSPEndDisplayList(gp++);	// �ި���ڲؽďI�[
	osWritebackDCacheAll();		// dinamic�����Ă��ׯ��
	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////

typedef struct {
	SMsgWnd msgWnd;
} SMainBootError;

#if LOCAL==JAPAN
#include "dm_title.mes"
#elif LOCAL==AMERICA
#include "dm_title_am.mes"
#elif LOCAL==CHINA
#include "dm_title_zh.mes"
#endif

int main_boot_error(void *arg)
{
	SMainBootError *st = (SMainBootError *)ALIGN_16(_codeSegmentEnd);
	void *heap = st + 1;

	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	bool loop = true;

	// ���[�N���N���A
	bzero(st, sizeof(SMainBootError));

	// ���b�Z�[�W�L���[�̍쐬
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// �N���C�A���g�ɓo�^
	nnScAddClient((NNSched *)arg, &client, &msgQ);

	switch(main_no) {
	case MAIN_CONT_ERROR:
#if LOCAL==JAPAN
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 96);
#elif LOCAL==AMERICA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 96);
#elif LOCAL==CHINA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 96);
#endif
		msgWnd_addStr(&st->msgWnd, _mesBootContErr);
		break;

	case MAIN_TV_ERROR:
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 64, 114);
		msgWnd_addStr(&st->msgWnd, _mesBootTvErr);
		break;

	case MAIN_CSUM_ERROR:
#if LOCAL==JAPAN
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#elif LOCAL==AMERICA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#elif LOCAL==CHINA
		msgWnd_init(&st->msgWnd, &heap, 64, 5, 40, 108);
#endif
		msgWnd_addStr(&st->msgWnd, _mesBootCSumErr);
		break;
	}

	msgWnd_skip(&st->msgWnd);

	while(loop) {
		joyProcCore();

		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		msgWnd_update(&st->msgWnd);

		switch(main_no) {
		case MAIN_CSUM_ERROR:
			if(_getKeyTrg(0) & A_BUTTON) {
				loop = false;
				dm_snd_play(SE_mDataErase);
			}
			break;
		}

		dm_audio_update();

		graphic_no = GFX_BOOT_ERROR;
	}

	// �\����~
	graphic_no = GFX_NULL;

	// �O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while(pendingGFX != 0) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// �N���C�A���g�̖���
	nnScRemoveClient((NNSched *)arg, &client);

	switch(main_no) {
	case MAIN_CSUM_ERROR:
		EepRom_WriteAll(NULL, NULL);
		break;
	}

	return MAIN_TITLE;
}

void graphic_boot_error()
{
	SMainBootError *st = (SMainBootError *)ALIGN_16(_codeSegmentEnd);
	int i;

	gp = gfx_glist[gfx_gtask_no];

	F3RCPinitRtn();
	F3ClearFZRtn(1);

	msgWnd_draw(&st->msgWnd, &gp);

	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();

	gfxTaskStart(&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////
