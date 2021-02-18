
#include <ultra64.h>
#include "def.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "joy.h"
#include "graphic.h"
#include "evsworks.h"
#include "vr_init.h"
#include "aiset.h"
#include "aidata.h"
#include "font.h"
#include "record.h"
#include "dm_game_main.h"
#include "main_story.h"
#include "debug.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////////////
// ��ڲ԰�Ƽ�ײ��
//	modify	Hiroyuki Watanabe
//	1999/11/01
///////////////////////////////
int main11( void )
{
	MAIN_NO no = MAIN_TITLE;
	int i;

	for(i = 0; i < 4; i++) {
		game_state_data[i].virus_level  = 10;           // �E�C���X���x��
		game_state_data[i].cap_def_speed= SPEED_MID;    // �������x
		game_state_data[i].charNo       = i;            // �L�����N�^�[�ԍ�
		game_state_data[i].player_no    = i;            // �v���C���[�ԍ�
		game_state_data[i].player_type  = PUF_PlayerMAN;// �l
		game_state_data[i].think_type   = i;            // �v�l�ԍ�
		game_state_data[i].think_level  = PMD_CPU1;     // CPU LEVEL
		game_state_data[i].game_level   = 1;            // �Q�[�����x��
	}

	evs_select_name_no[0] = evs_select_name_no[1] = DM_MEM_GUEST;

	switch(EepRom_Init()) {
	case EepRomErr_NotInit:
		EepRom_InitFirst(NULL, NULL);
		break;

	case EepRomErr_BadSum:
		no = MAIN_CSUM_ERROR;
		break;
	}

	return no;
}

//////////////////////////////////////////////////////////////////////////////
// �X�g�[���[���[�h:�G�L�����v�l�̒���

static void adjust_story_ai()
{
	int i, j;

	for(i = 0; i < evs_playcnt; i++) {
		game_state *state;
		AI_CHAR *aichar;

		state = &game_state_data[i];
		aichar = &ai_char_data[CHR_PLAIN1 + i];

		if(state->player_type != PUF_PlayerCPU) {
			continue;
		}

		*aichar = ai_char_data[state->think_type];
		state->think_type = CHR_PLAIN1 + i;

		switch(evs_story_level) {
		case 0:
			switch(state->charNo) {
			case DMC_MARIO: case DMC_WARIO:
				if(evs_story_no == 1) {
					*aichar = ai_char_data[CHR_NOKO];
					aichar->speed = AI_SPEED_VERYSLOW;
				}
				break;

			case DMC_ROBO:
				aichar->speed = AI_SPEED_SLOW;
				break;

			case DMC_NAZO:
				aichar->speed = AI_SPEED_VERYSLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				aichar->effect[2] = AI_CONDITION_Ignore;
				break;

			case DMC_VWARIO: case DMC_MMARIO:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				break;
			}
			break;

		case 1:
			switch(state->charNo) {
			case DMC_MARIO: case DMC_WARIO:
				if(evs_story_no == 1) {
					*aichar = ai_char_data[CHR_NOKO];
					aichar->speed = AI_SPEED_VERYSLOW;
				}
				else {
					aichar->speed = AI_SPEED_VERYSLOW;
					aichar->effect[1] = AI_CONDITION_Ignore;
				}
				break;

			case DMC_KURAGE:
				aichar->speed = AI_SPEED_VERYSLOW;
				break;

			case DMC_IKA:
				aichar->speed = AI_SPEED_SLOW;
				break;

			case DMC_FUSEN:
				aichar->speed = AI_SPEED_VERYSLOW;
				break;

			case DMC_KAERU:
				aichar->effect[0] = AI_CONDITION_Ignore;
				break;

			case DMC_ROBO:
				aichar->speed = AI_SPEED_VERYSLOW;
				break;

			case DMC_MAD:
				aichar->speed = AI_SPEED_VERYSLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				aichar->effect[1] = AI_CONDITION_Ignore;
				break;

			case DMC_NAZO:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				aichar->effect[2] = AI_CONDITION_Ignore;
				break;

			case DMC_VWARIO: case DMC_MMARIO:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				break;
			}
			break;

		case 2:
			switch(state->charNo) {
			case DMC_MARIO: case DMC_WARIO:
				if(evs_story_no == 1) {
					*aichar = ai_char_data[CHR_NOKO];
					aichar->speed = AI_SPEED_VERYSLOW;
				}
				else {
					aichar->speed = AI_SPEED_SLOW;
					aichar->effect[1] = AI_CONDITION_Ignore;
				}
				break;

			case DMC_YARI:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[1] = AI_CONDITION_Ignore;
				break;

			case DMC_KUMO:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[2] = AI_CONDITION_Ignore;
				break;

			case DMC_RINGO: case DMC_MAYU:
				aichar->speed = AI_SPEED_FAST;
				aichar->effect[0] = AI_CONDITION_Ignore;
				break;

			case DMC_KURAGE:
				aichar->speed = AI_SPEED_FAST;
				aichar->effect[1] = AI_CONDITION_Ignore;
				break;

			case DMC_IKA:
				aichar->speed = AI_SPEED_MAX;
				break;

			case DMC_ROBO:
				aichar->speed = AI_SPEED_SLOW;
				break;

			case DMC_MAD:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[0] = AI_CONDITION_Ignore;
				aichar->effect[1] = AI_CONDITION_Ignore;
				break;

			case DMC_NAZO:
				aichar->speed = AI_SPEED_FAST;
				aichar->wait_attack = 0;
				aichar->condition[2] = AI_CONDITION_Random;
				aichar->condition_param[2] = 15;
				aichar->effect[2] = AI_EFFECT_Waver;
				break;

			case DMC_VWARIO: case DMC_MMARIO:
				aichar->speed = AI_SPEED_FAST;
				aichar->effect[0] = AI_CONDITION_Ignore;
				break;
			}
			break;

		case 3:
			switch(state->charNo) {
			case DMC_MARIO: case DMC_WARIO:
				if(evs_story_no == 1) {
					*aichar = ai_char_data[CHR_NOKO];
					aichar->effect[1] = 0;
				}
				aichar->speed = AI_SPEED_VERYSLOW;
				break;

			case DMC_YARI:
				aichar->speed = AI_SPEED_SLOW;
				aichar->effect[1] = 0;
				break;

			case DMC_KUMO:
				aichar->speed = AI_SPEED_SLOW;
				break;

			case DMC_RINGO:
			case DMC_MAYU:
				aichar->speed = AI_SPEED_FAST;
				break;

			case DMC_KURAGE:
				aichar->speed = AI_SPEED_FAST;
				break;

			case DMC_IKA:
				aichar->speed = AI_SPEED_MAX;
				break;

			case DMC_FUSEN:
				aichar->speed = AI_SPEED_VERYFAST;
				break;

			case DMC_KAERU:
				aichar->speed = AI_SPEED_FAST;
				break;

			case DMC_NAZO:
				aichar->speed = AI_SPEED_MAX;
				aichar->effect[1] = 0;
				break;

			case DMC_VWARIO:
			case DMC_MMARIO:
				aichar->speed = AI_SPEED_MAX;
				aichar->effect[0] = 6;
				break;
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// [����]��[�ݒ�] �Ƃ̘A���ݒ�

int main12( void )
{
	game_state *state;
	int i, j, k, pe, sx, sy;
	int ret_main;

	static const u8 _seqTbl[] = {
		SEQ_Game_D >> 1, SEQ_Game_C >> 1,
		SEQ_Game_D >> 1, SEQ_Game_C >> 1,
		SEQ_Game_D >> 1, SEQ_Game_C >> 1,
		SEQ_Game_D >> 1, SEQ_Game_C >> 1,
		SEQ_Game_E >> 1, SEQ_Game_C >> 1,
	};
	static const u8 _demoSeqTbl[] = {
		SEQ_Fever >> 1, SEQ_Chill >> 1,
	};

	static const u8 StoryVirLv[][10] = {
		{ 0, 2, 3, 3, 4, 4, 5, 5, 6, 6, },//10,10,10 },
		{ 0, 5, 6, 6, 7, 7, 8, 8, 9, 9, },//13,13,13 },
		{ 0,11,12,12,13,13,14,14,15,15, },//16,16,16 },

//		{ 0, 2, 3, 4, 5, 6, 7, 8, 9, 9, },//10,10,10 },
//		{ 0, 5, 6, 7, 8, 9,10,11,12,12, },//13,13,13 },
//		{ 0, 8, 9,10,11,12,13,14,15,15, },//16,16,16 },

//		{ 0, 5, 6, 6, 7, 7, 8, 8, 9, 9, },//10,10,10 },
//		{ 0, 8, 9, 9,10,10,11,11,12,12, },//13,13,13 },
//		{ 0,11,12,12,13,13,14,14,15,15, },//16,16,16 },
	};

	static const s8 _charToAi[] = {
		CHR_MARIO,// �}���I
		CHR_MARIO,// �����I
		CHR_PUKU, // �}���s�[
		CHR_NOKO, // �����}��
		CHR_PUKU, // ��񂲂낤
		CHR_HANA, // �t�E�Z���܂���
		CHR_TERE, // �܂�J�G��
		CHR_CHOR, // �ӂ����炰
		CHR_PROP, // �C�J�^�R�e���O
		CHR_BOMB, // �L�O����
		CHR_PACK, // �n���}�[���{
		CHR_KAME, // �}�b�h���V�^�C��
		CHR_KUPP, // �Ȃ��̂���
		CHR_PEACH,// �o���p�C�A�����I
		CHR_PEACH,// ���^���}���I
	};

	static const s8 _stageToChar_tbl[][10] = {
		{ 0, DMC_WARIO, DMC_YARI, DMC_KUMO, DMC_MAYU, DMC_KURAGE, DMC_FUSEN, DMC_ROBO, DMC_NAZO, DMC_VWARIO },
		{ 0, DMC_MARIO, DMC_YARI, DMC_KUMO, DMC_RINGO, DMC_IKA, DMC_KAERU, DMC_ROBO, DMC_NAZO, DMC_MMARIO },
	};
	static const s8 _story4PChar_tbl[][4] = {
		{ DMC_MARIO, DMC_WARIO, DMC_MAD, DMC_ROBO }, // �}���I�A�����I�A��A���{
		{ DMC_WARIO, DMC_MARIO, DMC_MAD, DMC_ROBO }, // �����I�A�}���I�A��A���{
	};

	static const s8 _team_flg[][4] = {
		{ TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY2, TEAM_ENEMY3 },
		{ TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY2, TEAM_ENEMY3 },
		{ TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY2, TEAM_ENEMY3 },
//		{ TEAM_MARIO, TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY },
//		{ TEAM_MARIO, TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY },
//		{ TEAM_MARIO, TEAM_MARIO, TEAM_ENEMY, TEAM_ENEMY },
	};

	// �ްѻ���( �l���ABG�ԍ�, ��߾ٻ���, M̨����x, y , ϯ��� )
	static const u8 GameSize[] = {
		1,//{ 1, BG_1P	, MFITEM_LARGE, STD_MFieldX, STD_MFieldY, 3}, // 1p
		2,//{ 2, BG_VS	, MFITEM_LARGE, STD_MFieldX, STD_MFieldY, 3}, // 2p
		4,//{ 4, BG_VS4P, MFITEM_SMALL, VS4_MFieldX, VS4_MFieldY, 3}, // 4p
		2,//{ 2, BG_2PS1, MFITEM_LARGE, STD_MFieldX, STD_MFieldY, 1}, // vs
		1,//{ 1, BG_1P	, MFITEM_LARGE, STD_MFieldX, STD_MFieldY, 3}, // demo 1p
		2,//{ 2, BG_VS	, MFITEM_LARGE, STD_MFieldX, STD_MFieldY, 3}, // demo 2p
		4,//{ 4, BG_VS4P, MFITEM_SMALL, VS4_MFieldX, VS4_MFieldY, 3}, // demo 4p
	};

	evs_playcnt = GameSize[evs_gamesel];

	switch( evs_gamesel ) {
	case GSL_1PLAY:
		PRT0("GSL_1PLAY\n");
		// �l�͌�Őݒ肷��
		game_state_data[0].player_type = PUF_PlayerMAN;
		evs_story_flg = 0;
		ret_main = MAIN_GAME;
		break;

	case GSL_2PLAY:
		PRT0("GSL_2PLAY\n");
		ret_main = MAIN_GAME;
		evs_story_flg = 0;
		for(i = 0; i < 2; i++) {
			// �l�͌�Őݒ肷��
			game_state_data[i].player_type = PUF_PlayerMAN;
		}
		break;

	case GSL_4PLAY:
		PRT0("GSL_4PLAY\n");
		for(i = 0; i < 4; i++) {
			// �v�l�ԍ�
			game_state_data[i].think_type = _charToAi[ game_state_data[i].charNo ];
		}
		ret_main = MAIN_GAME;
		break;

	case GSL_VSCPU:
		PRT0("GSL_VSCPU\n");

		// �X�g�[���[���[�h�̏ꍇ
		if(evs_story_flg) {
			int warioFlag = (story_proc_no >= STORY_W_OPEN ? 1 : 0);

			// �Ƃ肠�����S���ݒ�
			for(i = 0; i < 4; i++) {
				state = &game_state_data[i];
				j = MIN(2, evs_story_level);

				// �E�C���X��
				state->virus_level = StoryVirLv[j][evs_story_no];

				// �J�v�Z���X�s�[�h
				state->cap_def_speed = SPEED_MID;

				// �L�����ԍ�
				state->charNo = _stageToChar_tbl[warioFlag][evs_story_no];

				// �l�͌�Őݒ肷��
				state->player_type = PUF_PlayerCPU;

				// CPU��Փx
				state->think_level = j;

				// �v�l�ԍ�
				state->think_type = _charToAi[ state->charNo ];

				// �`�[������
				state->team_no = _team_flg[j][i];

				// �Q�[����Փx
				state->game_level = j;
			}

			// �l�l�ΐ�X�e�[�W
			if(evs_story_level > 0 && evs_story_no == 7) {
				evs_gamesel = GSL_4PLAY;
				evs_playcnt = 4;

				for(i = 0; i < 4; i++) {
					state = &game_state_data[i];
					state->charNo = _story4PChar_tbl[warioFlag][i];
					state->think_type = _charToAi[ state->charNo ];
					if(evs_story_level < 3) {
						state->think_level = 0;
					}
				}

				// 1P�l
				game_state_data[0].player_type = PUF_PlayerMAN;
			}
			else {
				evs_playcnt = 2;

				// �v���C���[
				game_state_data[0].charNo = warioFlag;
				game_state_data[0].player_type = PUF_PlayerMAN;
			}

			// �v�l�𒲐�
			adjust_story_ai();

			// �V�[�P���X��ON
			evs_seqence = TRUE;
			evs_seqnumb = _seqTbl[evs_story_no];
		}
		else {
			game_state_data[0].player_type = PUF_PlayerMAN;

			// �Ƃ肠����2P�͂b�o�t
			game_state_data[1].player_type = PUF_PlayerCPU;

			// �v�l�ԍ�
			game_state_data[1].think_type = _charToAi[ game_state_data[1].charNo ];
		}
		ret_main = MAIN_GAME;
		break;

	case GSL_1DEMO:
		evs_playcnt = 1;
		evs_game_time = 0;
		evs_one_game_flg = 0;

		game_state_data[0].game_score    = 0;             // �X�R�A
		game_state_data[0].player_no     = 0;             // �v���C���[�ԍ�
		game_state_data[0].player_type   = PUF_PlayerCPU; // �b�o�t
		game_state_data[0].think_level   = PMD_CPU1;      // ��Փx
		game_state_data[0].virus_level   = 10;            // ���x��
		game_state_data[0].cap_def_speed = SPEED_MID;     // ���x
		game_state_data[0].charNo        = 0;             // �G�L����

		// �v�l�ԍ�
		game_state_data[0].think_type = _charToAi[ game_state_data[0].charNo ];

		evs_seqence = TRUE;
		evs_seqnumb = _demoSeqTbl[rand() & ARRAY_SIZE(_demoSeqTbl)];

		ret_main = MAIN_GAME;
		break;

	case GSL_2DEMO:
		evs_playcnt = 2;
		evs_story_flg = 0;
		evs_one_game_flg = 0;

		for(i = 0; i < 2; i++) {
			// �Q�X�g��I�񂾂��Ƃɂ���
			evs_select_name_no[i] = DM_MEM_GUEST;
			for(;;){
				// �b�o�t
				game_state_data[i].charNo = genrand(13);
				for( j = k = 0;j < 2;j++ ){
					if( i != j ){
						if( game_state_data[i].charNo == game_state_data[j].charNo ){
							k ++;
						}
					}
				}
				if( k == 0 ){
					break;
				}
			}

			game_state_data[i].game_score  = 0;             // �X�R�A
			game_state_data[i].player_no   = i;             // �v���C���[�ԍ�
			game_state_data[i].player_type = PUF_PlayerCPU; // �b�o�t

			// �v�l�ԍ�
			game_state_data[i].think_type = _charToAi[ game_state_data[i].charNo ];
			game_state_data[i].think_level = PMD_CPU1;      // ��Փx

			game_state_data[i].virus_level   = 10;        // ���x��
			game_state_data[i].cap_def_speed = SPEED_MID; // ���x
		}

		evs_seqence = TRUE;
		evs_seqnumb = _demoSeqTbl[rand() & ARRAY_SIZE(_demoSeqTbl)];

		ret_main = MAIN_GAME;
		break;

	case GSL_4DEMO:
		evs_playcnt = 4;
		evs_story_flg = 0;
		evs_one_game_flg = 0;

		for(i = 0; i < 4; i++) {
			for(;;) {
				// �b�o�t
				game_state_data[i].charNo = genrand(13);
				for(j = k = 0; j < 4; j++) {
					if(i != j) {
						if(game_state_data[i].charNo == game_state_data[j].charNo) {
							k++;
						}
					}
				}
				if(k == 0) {
					break;
				}
			}
			game_state_data[i].game_score  = 0;             // �X�R�A
			game_state_data[i].player_no   = i;             // �v���C���[�ԍ�
			game_state_data[i].player_type = PUF_PlayerCPU; // �b�o�t

			// �v�l�ԍ�
			game_state_data[i].think_type = _charToAi[ game_state_data[i].charNo ];
			game_state_data[i].think_level = PMD_CPU1;      // ��Փx

			game_state_data[i].team_no       = i;         // �`�[������
			game_state_data[i].virus_level   = 10;        // ���x��
			game_state_data[i].cap_def_speed = SPEED_MID; // ���x
		}

		evs_seqence = TRUE;
		evs_seqnumb = _demoSeqTbl[rand() & ARRAY_SIZE(_demoSeqTbl)];

		ret_main = MAIN_GAME;
		break;
	}

	return ret_main;
}

