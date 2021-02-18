
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "joy.h"
#include "audio.h"
#include "graphic.h"
#include "evsworks.h"
#include "aiset.h"

#include "record.h"
#include "main_menu.h"
#include "main_story.h"
#include "local.h"

//////////////////////////////////////////////////////////////////////////////
// 変数

NNSched sched;
MAIN_NO main_no;
MAIN_NO main_old;

//////////////////////////////////////////////////////////////////////////////
// 外部関数宣言

extern int main11(void);
extern int main12(void);
extern void main_story(NNSched*);
extern void main60(NNSched*);

#include "dm_game_main.h"
#include "dm_manual_main.h"
#include "dm_title_main.h"

//////////////////////////////////////////////////////////////////////////////
// ﾒｲﾝﾌﾟﾛｼｰｼﾞｬ
void mainproc(void* arg)
{
	MAIN_NO main_bak;

	EepRom_DumpDataSize();

	// ｽｹｼﾞｭｰﾗｽﾚｯﾄﾞの作成
	switch ( osTvType ) {
	case OS_TV_NTSC:
		// NTSC
		nnScCreateScheduler(&sched, OS_VI_NTSC_LAN1, 1);
		main_no = MAIN_11;
		break;
	case OS_TV_MPAL:
		// MPAL
		nnScCreateScheduler(&sched, OS_VI_MPAL_LAN1, 1);
#if LOCAL==JAPAN
		main_no = MAIN_TV_ERROR;
#elif LOCAL==AMERICA
		// アメリカ版は、MPALも動作可能。
		main_no = MAIN_11;
#elif LOCAL==CHINA
		main_no = MAIN_11;
#endif
		break;
	case OS_TV_PAL:
		// PAL
		nnScCreateScheduler(&sched, OS_VI_PAL_LAN1, 1);
		main_no = MAIN_TV_ERROR;
		break;
	default:
		nnScCreateScheduler(&sched, OS_VI_NTSC_LAN1, 1);
		main_no = MAIN_TV_ERROR;
		break;
	}

	// サウンドドライバーを初期化
	dm_audio_init_driver(&sched);

	// graphic library の初期化, ｸﾞﾗﾌｨｯｸｽﾚｯﾄﾞの作成, 起動
	gfxInit(_gfxfreeSegmentStart);
	gfxCreateGraphicThread(&sched);

	// VIｲﾝﾀｰﾌｴｰｽの設定
	osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF);

	// ｺﾝﾄﾛｰﾗ初期化, ｽﾚｯﾄﾞの作成, 起動
	joyInit(MAXCONTROLLERS);
	evs_playmax = joyResponseCheck();
	if(evs_playmax == 0 && main_no == MAIN_11) {
		main_no = MAIN_CONT_ERROR;
	}

	// ＡＩのワークを初期化
	aifFirstInit();

	// 思考関数をアイドル処理に設定
	setIdleFunc(make_ai_main);

	// ｹﾞｰﾑﾒｲﾝﾙｰﾌﾟ
	while(1){
		main_bak = main_no;

		switch(main_no) {
		case MAIN_11: // ﾌﾟﾚｲﾔｰﾒﾓﾘｲﾆｼｬﾗｲｽﾞ
			main_no = main11();
			break;

		case MAIN_12: // 各ｹﾞｰﾑへ連絡
			main_no = main12();
			break;

		case MAIN_TITLE: // 新タイトル
			main_no = dm_title_main(&sched);
			break;

		case MAIN_MANUAL: // 操作説明
			main_no = dm_manual_main(&sched);
			break;

		case MAIN_GAME: // ゲーム
			main_no = dm_game_main(&sched);
			break;

		case MAIN_MENU:
			main_no = main_menu(&sched);
			break;

		case MAIN_TECHMES:
			main_no = main_techmes(&sched);
			break;

		case MAIN_CONT_ERROR:
		case MAIN_TV_ERROR:
		case MAIN_CSUM_ERROR:
			main_no = main_boot_error(&sched);
			break;

		case MAIN_STORY: // ストーリー
			main_story(&sched);

			if(story_proc_no == STORY_M_OPEN || story_proc_no == STORY_W_OPEN) {
				story_proc_no++;
				main_no = MAIN_STORY;
			}
			else if(story_proc_no == STORY_M_END || story_proc_no == STORY_M_END2
				|| story_proc_no == STORY_W_END || story_proc_no == STORY_W_END2)
			{
				main_no = MAIN_TECHMES;
			}
			else{
				main_no = MAIN_12;
			}
			break;

		default:
			break;
		}

		main_old = main_bak;
	}
}

