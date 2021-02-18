//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_game_main.c
//	create		:	1999/08/02
//	modify		:	1999/12/06
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>
#ifndef LINUX_BUILD
#include <math.h>
#endif

#include "calc.h"
#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "musdrv.h"

#include "aiset.h"
#include "aidata.h"

#include "evsworks.h"
#include "dm_game_main.h"
#include "record.h"
#include "cpu_task.h"

#include "vr_init.h"
#include "dm_virus_init.h"

#include "util.h"
#include "tex_func.h"
#include "char_anime.h"
#include "game_etc.h"
#include "msgwnd.h"
#include "main_story.h"

#ifdef DEBUG
#include "aidebug.h"
#endif

#include "debug.h"
#include "replay.h"
#include "passwd.h"
#include "local.h"

// �ŏI��o���ɖ����ɂȂ��`
#if !defined(NINTENDO)
#define _ENABLE_CONTROL // ��������
#endif // NINTENDO

//////////////////////////////////////////////////////////////////////////////
//{### �}�N��

#define _getKeyTrg(playerNo) ((int)joyupd[main_joy[(playerNo)]])
#define _getKeyRep(playerNo) ((int)joycur[main_joy[(playerNo)]])
#define _getKeyLvl(playerNo) ((int)joynew[main_joy[(playerNo)]])

//## �q�[�v�̐擪
#define HEAP_START _codeSegmentEnd

//## ���g���C�R�C���̍ő吔
#define COINMAX 3

//## �f�o�b�O���[�h�ɓ��鎞�ɉ����{�^��
#define CONT_DEBUG (DM_KEY_CU | DM_KEY_CD)
#define CONT_CONFIG (DM_KEY_CL | DM_KEY_CR)

/*
//## �w�肵���X�e�[�^�X���b�o�t���ǂ������`�F�b�N
#define PLAYER_IS_CPU(state, playerNo) \
	((state)->player_type == PUF_PlayerCPU || \
	 PLAYER_IS_DEBUG((state), (playerNo)))

//## �w�肵���X�e�[�^�X���f�o�b�O�p�b�o�t���ǂ����`�F�b�N
#define PLAYER_IS_DEBUG(state, playerNo) \
	((state)->player_type != PUF_PlayerCPU && \
	 (playerNo) == 0 && aiDebugP1 >= 0)
*/
/* XXXblythe stupid gcc can't parse the above macros properly */
#define PLAYER_IS_CPU(state, playerNo) ((state)->player_type == PUF_PlayerCPU || PLAYER_IS_DEBUG((state), (playerNo)))

#define PLAYER_IS_DEBUG(state, playerNo) ((state)->player_type != PUF_PlayerCPU && (playerNo) == 0 && aiDebugP1 >= 0)


#define MARIO_STAND_X (218 + 32)
#define MARIO_STAND_Y ( 20 + 64)
#define MARIO_THROW_X (MARIO_STAND_X - 32 +  0)
#define MARIO_THROW_Y (MARIO_STAND_Y - 64 + 32)

#define _setDarkCapPrim(gp) gDPSetPrimColor((gp), 0,0, 96,96,96,150)

#define JOYrTIMEvm 13500 // DrM�ްю����۰דǂ݂Ƃ����ݸ�( vsMEN )
//#define JOYrTIMEvc 10500 //             ..               ( vsCPU )

//## �p�l���e�̃A���t�@�l
#define PANEL_SHADOW_ALPHA 160

//## �r�̓����x
#define BOTTLE_ALPHA 192

//## �ŏI�X�e�[�W�ԍ�
#define LAST_STAGE_NO 8

//## EX�X�e�[�W�ԍ�
#define EX_STAGE_NO 9

//## �V�[�P���X�̉���
#define SEQ_VOL_LOW 0x40
#define SEQ_VOL_HI  0x80

//## �X�R�A�̍ő�l
#define SCORE_MAX_VAL 9999900

//## �X�R�A�A�^�b�N�̃^�C�����~�b�g
#define SCORE_ATTACK_TIME_LIMIT (3 * 60 * FRAME_PAR_SEC)

//## �E�B���X���x���}�b�N�X
#define VIRUS_LEVEL_MAX 99

//## �V�U�����O�t���O
#define SCISSOR_FLAG 2

//////////////////////////////////////////////////////////////////////////////
//{### ���ʃf�[�^

//## �E�C���X�̃A�j���[�V�����p�^�[���ԍ�
u8 virus_anime_table[][4] = {
	{ virus_a1, virus_a2, virus_a3, virus_a4 }, // �ԗp
	{ virus_b1, virus_b2, virus_b3, virus_b4 }, // ���p
	{ virus_c1, virus_c2, virus_c3, virus_c4 }, // �p
};

//////////////////////////////////////////////////////////////////////////////
//{### ���ʃ��[�`��

//## ���_���Z�o
static u32 dm_calc_score(game_state *state, int level)
{
	u32 score = 0;
	int i;

	for(i = state->erase_virus_count_old; i < state->erase_virus_count; i++) {
		score += Score1p[level][MIN(i, SCORE_MAX - 1)] * 100;
	}

	state->erase_virus_count_old = state->erase_virus_count;

	return score;
}

//## ���_�����Z
static void dm_add_score(game_state *state, u32 score)
{
	state->game_score += score;

	// �J���X�g
	if(state->game_score > SCORE_MAX_VAL) {
		state->game_score = SCORE_MAX_VAL;
	}

	// �n�C�X�R�A�̍X�V
	if(evs_high_score < state->game_score) {
		evs_high_score = state->game_score;
	}
}

//## ���_�֐�
u32 dm_make_score(game_state *state)
{
	int level;
	u32 score;

	if(evs_story_flg) {
		level = MIN(2, evs_story_level);
	}
	else {
		switch(evs_gamesel) {
		case GSL_1PLAY:
		case GSL_1DEMO:
			level = state->cap_def_speed;
			break;

		case GSL_2PLAY:
		case GSL_VSCPU:
		case GSL_2DEMO:
			level = state->cap_def_speed;
			break;

		case GSL_4PLAY:
		case GSL_4DEMO:
			level = state->cap_def_speed;
			break;

		default:
			DBG0(true, "�s���ȃQ�[�����[�h\n");
			break;
		}
	}

	score = dm_calc_score(state, level);
	dm_add_score(state, score);

	return score;
}

//## �U���������֐�
void dm_attack_se(game_state *state, int player_no)
{
	// 2P���̍U���r�d�e�[�u��
	static s8 dm_chaine_se_table_vs[] = {
		SE_gCombo1P, SE_gCombo2P
	};
	// 4P���̍U���r�d�e�[�u��
	static s8 dm_chaine_se_table_4p[4][3] = {
		{ SE_gCombo4P, SE_gCombo3P, SE_gCombo2P },
		{ SE_gCombo1P, SE_gCombo4P, SE_gCombo3P },
		{ SE_gCombo2P, SE_gCombo1P, SE_gCombo4P },
		{ SE_gCombo3P, SE_gCombo2P, SE_gCombo1P },
	};
//	static s8 dm_chaine_se_table_4p[4] = {
//		SE_gCombo1P, SE_gCombo2P, SE_gCombo3P, SE_gCombo4P,
//	};
	int i;

	if(state->chain_line < 2) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
		dm_snd_play_in_game(SE_gCombo1P);
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
		dm_snd_play_in_game(dm_chaine_se_table_vs[player_no]);
		break;

	case GSL_4PLAY:
		for(i = 0; i < 3; i++) {
			if(state->chain_color[3] & (1 << i)) {
				dm_snd_play_in_game(dm_chaine_se_table_4p[player_no][i]);
			}
		}
//		dm_snd_play_in_game(dm_chaine_se_table_4p[player_no]);
		break;
	}
}

//## �ςݏグ�x���������֐�
void dm_warning_h_line(game_state *state, game_map *map)
{
	int i, j, flg = 0;

	// ��̂R�i�������ׂȂ�
	for(i = 1; i < 4; i++) {
		for(j = 0; j < GAME_MAP_W; j++) {
			if(get_map_info(map, j, i) == cap_flg_on) {
				// ��̂R�i�ɉ�������������
				flg = 1;
				break;
			}
		}
		if(flg) {
			break;
		}
	}

	if(flg) {
		if(state->warning_se_flag == 0) {
			// �炵�Ă��Ȃ����ēx�ς݂�������
//			dm_snd_play_in_game(SE_gWarning); // �x�� SE �Đ�
			state->warning_se_flag++;
		}
	}
	else {
		// �r�b�g������
		state->warning_se_flag = 0;
	}
}

//## �������ݒ�֐�
static int set_down_flg(game_map *map)
{
	int i, j, ret;
	unsigned int k, l, flg[2];

	for(i = 0;i < 120;i++) {	// �����t���O�̐ݒ�
		map[i].capsel_m_flg[cap_down_flg] = cap_flg_on;	// �����t���O�̐ݒ�
	}

	for(j = 14;j >= 0;j--)	// �ŉ��i�̂P��̒i����n�߂�
	{
		for(i = 0;i < 8;i++)
		{
			k = (j << 3) + i;	// �z��ԍ��̌v�Z
			if((map + k)->capsel_m_flg[cap_disp_flg])
			{
				// �\������Ă���
				if((map + k)->capsel_m_flg[cap_virus_flg] < 0)
				{
					// �E�C���X�łȂ�
					if((map + k)->capsel_m_g > capsel_d && (map + k)->capsel_m_g < capsel_b)
					{
						// �������J�v�Z���̏ꍇ
						if((map + k)->capsel_m_g == capsel_l)
						{
							// �ŉ��i�ł͂Ȃ�
							for(l = 0;l < 2;l++) {
								flg[l] = cap_flg_on;
								if(get_map_info(map,(map + k+ l)->pos_m_x,(map + k+ l)->pos_m_y + 1) == cap_flg_on)
								{
									// ��Q�����������ꍇ
									if((map + k + l + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
										// ���̏�Q���͗����o���Ȃ�
										flg[l] = cap_flg_off;
									}
								}
							}

							if(!flg[0] || !flg[1])
							{
								// �����Ƃ����ɗ����邱�Ƃ��o���Ȃ�
								for(l = 0;l < 2;l++)
								{
									(map + k + l)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// �����t���O�𗧏���
								}
							}
						} else if((map + k)->capsel_m_g == capsel_r) {
							for(l = 0;l < 2;l++) {
								flg[l] = cap_flg_on;
								if(get_map_info(map,(map + k - l)->pos_m_x,(map + k - l)->pos_m_y + 1) == cap_flg_on)
								{
									// ��Q�����������ꍇ
									if((map + k - l + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
										// ���̏�Q���͗����o���Ȃ�
										flg[l] = cap_flg_off;
									}
								}
							}
							if(!flg[0] || !flg[1])
							{
								// �����Ƃ����ɗ����邱�Ƃ��o���Ȃ�
								for(l = 0;l < 2;l++)
								{
									(map + k - l)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// �����t���O������
								}
							}
						}
					} else {
						// �{�[���̏ꍇ
						if(get_map_info(map,(map + k)->pos_m_x, (map + k)->pos_m_y + 1) == cap_flg_on)
						{
							// ���̂̉��ɉ���������
							if((map + k + 8)->capsel_m_flg[cap_down_flg] != cap_flg_on) {
								(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̐ݒ�
							}
						}
					}
				} else {
					(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̐ݒ�
				}
			} else {
				(map + k)->capsel_m_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̐ݒ�
			}
		}
	}

	// �����t���O�̏󋵂�Ԃ�
	for(i = ret = 0;i < 128;i++) {
		if(map[i].capsel_m_flg[cap_disp_flg]) {
			// �\���t���O�������Ă���
			if(map[i].capsel_m_flg[cap_down_flg]) {
				// �����t���O�������Ă���
				ret++;
			}
		}
	}

	return ret;
}

//## �������֐�
void go_down(game_state *state, game_map *map , int cout)
{
	int i, j, p;
	unsigned int k, se_flg = 0;

	state->erase_anime_count++;

	// �E�F�C�g���Ԍo��?
	if(state->erase_anime_count < cout) {
		return;
	}

	state->erase_anime_count = 0;

	// �ŉ��i�̂P��̒i����n�߂�
	for(j = GAME_MAP_H; j >= 0; j--) {

		for(i = 0; i < GAME_MAP_W; i++) {

			// �z��ԍ��̌v�Z
			k = j * GAME_MAP_W + i;

			// �����t���O�������Ă���?
			if((map + k)->capsel_m_flg[cap_down_flg]) {

				// ������ɏ����R�s�[
				set_map(map ,(map + k)->pos_m_x,(map + k)->pos_m_y + 1,(map + k)->capsel_m_g,(map + k)->capsel_m_p);

				// �R�s�[�����̍폜
				clear_map(map,(map + k)->pos_m_x, (map + k)->pos_m_y);

				// ��������炷
				se_flg = 1;
			}
		}
	}

	if(se_flg) {
		// �����r�d�Đ�
		dm_snd_play_in_game(SE_gFreeFall);
	}

	if(state->flg_game_over) {
		// ���^�C�A���
		if(state->cnd_training == dm_cnd_training) {
			// ���K��
			for(i = 0; i < GAME_MAP_W * GAME_MAP_H; i++) {
				if(map[i].capsel_m_flg[cap_disp_flg])
				{
					// �\���t���O�������Ă���
					if(!map[i].capsel_m_flg[cap_down_flg])
					{
						// �����t���O�������Ă��Ȃ�
						if(map[i].capsel_m_p < 3) {
							// �Â��Ȃ��ꍇ
							map[i].capsel_m_p += 3;
						}
					}
				}
			}
		}
	}

	j = set_down_flg(map);	// �����t���O�̍Đݒ�

	// �����I��?
	if(j == 0) {
		if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
			if(!state->flg_game_over) {	// ���^�C�A���Ă��Ȃ�
				state->mode_now = dm_mode_erase_chack;	// ���Ŕ���
			}
			else if(state->flg_game_over) {	// ���^�C�A���Ă���(���K��)
				state->mode_now = dm_mode_tr_erase_chack;	// ���Ŕ���
			}
			state->cap_speed_count = 0;
		}
		else {
			if(!state->flg_game_over) {	// ���^�C�A���Ă��Ȃ�
				state->mode_now = dm_mode_cap_set;	// �J�v�Z���Z�b�g
			}
			else if(state->flg_game_over) {	// ���^�C�A���Ă���(���K��)
				state->mode_now = dm_mode_tr_cap_set;	// �J�v�Z���Z�b�g
			}
		}
	}
}

//## �}�b�v����ŃA�j���[�V�����֐�
void erase_anime(game_map *map)
{
	int i;

	for(i = 0; i < 128; i++) {
		// �\���t���O�������Ă���
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			continue;
		}

		// ���ŃA�j���[�V������Ԃ�����
		if(!map[i].capsel_m_flg[cap_condition_flg]) {
			continue;
		}

		// ���ŃA�j���[�V�����̐i�s
		map[i].capsel_m_g++;

		// �E�C���X�̏ꍇ
		if(map[i].capsel_m_flg[cap_virus_flg] >= 0) {
			if(map[i].capsel_m_g > erase_virus_b) {
				// �}�b�v���̍폜
				clear_map(map,map[i].pos_m_x,map[i].pos_m_y);
			}
		}
		// �E�C���X�ȊO�̏ꍇ
		else {
			if(map[i].capsel_m_g > erase_cap_b) {
				// �}�b�v���̍폜
				clear_map(map,map[i].pos_m_x,map[i].pos_m_y);
			}
		}
	}
}

//## ������ԃJ�v�Z������]�֐�
static void throw_rotate_capsel(game_cap *cap)
{
	static const int rotate_table[] = { 1, 3, 4, 2 };
	static const int rotate_mtx[] = {
		capsel_l, capsel_u, capsel_r, capsel_d, capsel_l, capsel_u
	};
	int i, vec, save;

	// �J�v�Z�����c�ɂȂ��Ă����ꍇ
	if(cap->pos_x[0] == cap->pos_x[1]) {
		cap->pos_x[1]++;
		cap->pos_y[1]++;
		save = cap->capsel_p[0];
		cap->capsel_p[0] = cap->capsel_p[1];
		cap->capsel_p[1] = save;
		vec = 1;
	}
	// �J�v�Z�������ɂȂ��Ă����ꍇ
	else {
		// ���W�ύX
		cap->pos_x[1]--;
		cap->pos_y[1]--;
		vec = -1;
	}

	// �O���t�B�b�N�̓���ւ�
	for(i = 0;i < 2;i++) {
		save = rotate_table[cap->capsel_g[i]];
		save += vec;
		cap->capsel_g[i] = rotate_mtx[save];
	}
}

//## �J�v�Z�����E�ړ��֐�
void translate_capsel(game_map *map, game_state *state, int move_vec, int joy_no)
{
	int vec = 0;
	game_cap *cap;

	cap = &state->now_cap;

	// �������Ă��Ȃ� || ��\��
	if(cap->pos_y[0] <= 0 || !cap->capsel_flg[cap_disp_flg]) {
		return;
	}

	if(move_vec == cap_turn_r) {
		// �E�ړ��̏ꍇ
		if(cap->pos_x[0] == cap->pos_x[1]) {
			// �J�v�Z�����c���т̏ꍇ
			if(cap->pos_x[0] < 7) {
				if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0]) != cap_flg_on) {
					if(cap->pos_y[1] == 0) {
						// �ړ���ɏ�Q���������ꍇ
						vec = 1;
					} else {
						if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[1]) != cap_flg_on) {
							// �ړ���ɏ�Q���������ꍇ
							vec = 1;
						}
					}
				}
			}
		} else {
			// �J�v�Z���������т̏ꍇ
			if(cap->pos_x[1] < 7 && (get_map_info(map,cap->pos_x[1] + 1,cap->pos_y[0]) != cap_flg_on)) {
				// �ړ���ɏ�Q���������ꍇ
				vec = 1;
			}
		}
	} else if(move_vec == cap_turn_l) {
		// ���ړ��̏ꍇ
		if(cap->pos_x[0] == cap->pos_x[1]) {
			// �J�v�Z�����c���т̏ꍇ
			if(cap->pos_x[0] > 0) {
				if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on) {
					if(cap->pos_y[1] == 0) {
						// �ړ���ɏ�Q���������ꍇ
						vec = -1;
					} else {
						if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[1]) != cap_flg_on) {
							// �ړ���ɏ�Q���������ꍇ
							vec = -1;
						}
					}
				}
			}
		} else {
			if(cap->pos_x[0] > 0) {
				if(get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on) {
					// �ړ������ɏ�Q���������ꍇ
					vec = -1;
				}
			}
		}
	}

	if(vec != 0) {
		dm_snd_play_in_game(SE_gLeftRight);	// �ړ�SE�Đ�
		state->cap_move_se_flg = 0;
		cap->pos_x[0] += vec;
		cap->pos_x[1] += vec;
	} else {
		if(state->cap_move_se_flg == 0) {
			// �ړ��ł��Ȃ��Ă����͖炷
			state->cap_move_se_flg = 1;
			dm_snd_play_in_game(SE_gLeftRight);	// �ړ�SE�Đ�
		}
		if(move_vec == cap_turn_r) {
			joyCursorFastSet(R_JPAD, joy_no);
		} else if(move_vec == cap_turn_l) {
			joyCursorFastSet(L_JPAD, joy_no);
		}
	}
}

//## �J�v�Z�����E��]�֐�
void rotate_capsel(game_map *map, game_cap *cap, int move_vec)
{
	static unsigned int rotate_table[] = {1,3,4,2};
	static unsigned int rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};
	int vec = 0;
	unsigned int i, save;

	if(cap->pos_y[0] > 0)	// �m�d�w�s�̈ʒu�ł͉�]�����Ȃ�
	{
		if(cap->capsel_flg[cap_disp_flg]) {	// �\���t���O
			if(cap->pos_x[0] == cap->pos_x[1]) {	// �J�v�Z�����c�ɂȂ��Ă����ꍇ
				if(cap->pos_x[0] == 7 || (get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0]) == cap_flg_on)) {	// �E�ǂɒ�����Ă��������ɏ�Q�����������ꍇ
					if(cap->pos_x[0] != 0 && (get_map_info(map,cap->pos_x[0] - 1,cap->pos_y[0]) != cap_flg_on)) {	// ���ǂɒ�����Ă��Ȃ��ł��ꂽ�Ƃ���ɏ�Q�������������ꍇ
						cap->pos_x[0] --;
						vec = 1;
					}
				} else {
					cap->pos_x[1] ++;
					vec = 1;
				}
				if(vec != 0) {
					cap->pos_y[1] ++;
					if(move_vec == cap_turn_l) {
						// �p���b�g�̓���ւ�
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			} else {	// �J�v�Z�������ɂȂ��Ă����ꍇ
				if(cap->pos_y[0] == 1) {	// �ŏ�i�������ꍇ
					cap->pos_x[1] --;
					vec = -1;
				} else {
					if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] - 1) == cap_flg_on) {	// ��]���̏�ɉ����������ꍇ
						if(get_map_info(map,cap->pos_x[0] + 1,cap->pos_y[0] - 1) != cap_flg_on) {	// �ړ���ɉ������������ꍇ
							cap->pos_x[0] ++;
							vec = -1;
						}
					} else {	// ��]���̏�ɉ������������ꍇ
						// ���W�ύX
						cap->pos_x[1] --;
						vec = -1;
					}
				}
				if(vec != 0) {
					cap->pos_y[1] --;
					// �p���b�g�̓���ւ�
					if(move_vec == cap_turn_r) {
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			}
			if(vec != 0) {
				dm_snd_play_in_game(SE_gCapRoll);	// ��]SE�Đ�
				// �O���t�B�b�N�̓���ւ�
				for(i = 0;i < 2;i++) {
					save = rotate_table[cap->capsel_g[i]];
					save += vec;
					cap->capsel_g[i] = rotate_mtx[save];
				}
			}
		}
	}
}

//## �J�v�Z���}�K�W���쐬�֐�
static void dm_make_magazine()
{
	u8 old[2], now[2];
	int i;

	CapsMagazine[0] = random(0xffff) % MAGA_MAX;
	old[0] = (CapsMagazine[0] >> 4) % 3;
	old[1] = (CapsMagazine[0] >> 0) % 3;
	i = 1;

	while(i < MAGA_MAX) {
		CapsMagazine[i] = random(0xffff) % MAGA_MAX;
		now[0] = (CapsMagazine[i] >> 4) % 3;
		now[1] = (CapsMagazine[i] >> 0) % 3;

		if(now[0] != old[0] || now[1] != old[1]) {
			old[0] = now[0];
			old[1] = now[1];
			i++;
		}
	}
}

//## �J�v�Z���������ݒ�֐�(�����t���O�𗧂ĂȂ�)
static void dm_init_capsel(game_cap *cap, unsigned int left_cap_col, unsigned int right_cap_col)
{
	cap->pos_x[0] = 3;	// ���J�v�Z���w���W
	cap->pos_x[1] = 4;	// �E�J�v�Z���w���W
	cap->pos_y[0] = cap->pos_y[1] = 0;	// �J�v�Z���x���W
	cap->capsel_g[0] = capsel_l;		// �O���t�B�b�N�ԍ�
	cap->capsel_g[1] = capsel_r;		// �O���t�B�b�N�ԍ�
	cap->capsel_p[0] = left_cap_col;	// �F�i���j
	cap->capsel_p[1] = right_cap_col;	// �F�i�E�j
	cap->capsel_flg[cap_disp_flg] = cap_flg_on;	// �\���t���O���n�m��
	cap->capsel_flg[1] = cap->capsel_flg[2] = cap_flg_off;	// ���̂Ƃ��떢�g�p
}

//## �J�v�Z���������ݒ�֐�(�����t���O�𗧂Ă�)
static void dm_init_capsel_go(game_cap *cap, int left_cap_col, int right_cap_col)
{
	dm_init_capsel(cap, left_cap_col, right_cap_col);
	cap->capsel_flg[cap_down_flg] = 1;
}

//## �J�v�Z���ݒ�֐�
void dm_set_capsel(game_state *state)
{
	state->cap_move_se_flg = 0;
	dm_init_capsel_go(&state->now_cap,(CapsMagazine[state->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state->cap_magazine_cnt] % 3);
	state->cap_magazine_save = state->cap_magazine_cnt;	// �O�̃}�K�W���ʒu�̕ۑ�
	state->cap_magazine_cnt ++;
	if(state->cap_magazine_cnt >= 0xfe)
		state->cap_magazine_cnt = 1;

	dm_init_capsel(&state->next_cap,(CapsMagazine[state->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state->cap_magazine_cnt] % 3);
}

//## �J�v�Z�����x�㏸�ݒ�֐�
void dm_capsel_speed_up(game_state *state)
{
	static const u8 _speed[] = { 15, 20, 25 };

	state->cap_count ++;
	if(state->cap_count >= 10) {	// �X�s�[�h�A�b�v
		dm_snd_play_in_game(SE_gSpeedUp);	// �X�s�[�h�A�b�v�r�d�Đ�
		state->cap_count = 0;
		state->cap_speed ++;
		if(state->cap_speed > GameSpeed[3]) {	// ���x���E
			state->cap_speed = GameSpeed[3];
		}
	}

	// �ϋv���[�h�ł̑��x����
	if(evs_gamemode == GMD_TaiQ) {
		state->cap_speed = MIN(_speed[ state->game_level ], state->cap_speed);
	}
}

//## �Q�[���I�[�o�[(�ς݂�����)����֐�
int dm_check_game_over(game_state *state, game_map *map)
{
	if(state->cnd_static == dm_cnd_game_over) {
		return cap_flg_on; // �Q�[���I�[�o�[
	}
	else {
		return cap_flg_off; // �Q�[���I�[�o�[����Ȃ�
	}
}

//## �t���b�V���E�B���X�̎c�萔�𐔂���
static int update_flash_virus_count(game_state *state, game_map *map, bool flag)
{
	int count = 0;
	int i, j;

	for(i = 0; i < state->flash_virus_count; i++) {
		j = state->flash_virus_pos[i][0] + state->flash_virus_pos[i][1] * GAME_MAP_W;

		// (��\�� || ���ŏ�� || �E�B���X�ł͂Ȃ�) �̏ꍇ
		if(!map[j].capsel_m_flg[cap_disp_flg]
		||  map[j].capsel_m_flg[cap_condition_flg]
		||  map[j].capsel_m_flg[cap_virus_flg] < 0)
		{
			if(flag) {
				state->flash_virus_pos[i][2] = -1;
			}
			continue;
		}

		count++;
	}

	return count;
}

// �E�C���X�����X�V
static int dm_update_virus_count(game_state *state, game_map *map, bool flag)
{
	switch(evs_gamemode) {
	case GMD_FLASH:
		state->virus_number = update_flash_virus_count(state, map, flag);
		break;
	default:
		state->virus_number = get_virus_count(map);
		break;
	}
	return state->virus_number;
}

//## �E�C���X�]���֐�
static void dm_set_virus(game_state *state, game_map *map, virus_map *v_map, u8 *order)
{
	unsigned int x_pos, y_pos, p, s;
	int virus_max;

	if(state->cnd_now != dm_cnd_init) {
		return;
	}

	// �E�C���X���̊���o��
	virus_max = dm_get_first_virus_count(evs_gamemode, state);

#if 0
	while(1) {
		s = random(virus_max);
		if(*(order + s) != 0xff) {
			p = *(order + s);
			*(order + s) = 0xff;
			break;
		}
	}
	state->virus_order_number++;
	set_virus(map,
		(v_map + p)->x_pos, (v_map + p)->y_pos, (v_map + p)->virus_type,
		virus_anime_table[(v_map + p)->virus_type][state->virus_anime]);
#else
	while(state->virus_order_number < virus_max && (order[state->virus_order_number] & 0x80)) {
		state->virus_order_number++;
	}
	if(state->virus_order_number < virus_max) {
		p = order[state->virus_order_number];
		order[state->virus_order_number] |= 0x80;

		set_virus(map,
			v_map[p].x_pos, v_map[p].y_pos, v_map[p].virus_type,
			virus_anime_table[v_map[p].virus_type][state->virus_anime]);

		// �g���[�j���O���͈Â��F�ɐݒ�
		if(state->cnd_training == dm_cnd_training) {
			map[p].capsel_m_p += 3;
		}
	}
#endif

	if(state->virus_order_number >= virus_max) {
		// �E�C���X�o���I��
		state->cnd_now = dm_cnd_wait; // �ҋ@��Ԃ�
		state->mode_now = dm_mode_wait; // �ҋ@��Ԃ�
	}

	dm_update_virus_count(state, map, false);
}

//## �E�C���X�A�j���֐�
void dm_virus_anime(game_state *state, game_map *map)
{
	int i;

	// �J�E���g�̑���
	state->virus_anime_count++;

	// �A�j���[�V�����Ԋu�������
	if(state->virus_anime_count <= state->virus_anime_spead) {
		return;
	}

	// �J�E���g�̃N���A
	state->virus_anime_count = 0;

	// �A�j���[�V�����̐i�s
	state->virus_anime += state->virus_anime_vec;

	// �A�j���[�V�����̍ő�R�}�����z����
	if(state->virus_anime > state->virus_anime_max) {
		// �A�j���[�V�����i�s�����̔��]
		state->virus_anime_vec = -1;
	}
	// �A�j���[�V�����̍ŏ��R�}�����z����
	else if(state->virus_anime < 1) {
		// �A�j���[�V�����i�s�����̔��]
		state->virus_anime_vec = 1;
	}

	// �}�b�v��̃E�C���X���A�j���[�V����������
	for(i = 0; i < 128; i++) {
		// �\������Ă��邩�H
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			continue;
		}

		// ���ŏ�Ԃ��H
		if(map[i].capsel_m_flg[cap_condition_flg]) {
			continue;
		}

		// �E�C���X���H
		if(map[i].capsel_m_flg[cap_virus_flg] < 0) {
			continue;
		}

		// �O���t�B�b�N�ύX
		map[i].capsel_m_g = virus_anime_table[ map[i].capsel_m_flg[cap_virus_flg] ][ state->virus_anime ];
	}
}

//## �E�C���X(�J�v�Z��)���Ŋ֐�
void dm_capsel_erase_anime(game_state *state, game_map *map)
{
	int i;

	state->erase_anime_count++;

	// ���i�K
	if(state->erase_anime_count == dm_erace_speed_1) {
		erase_anime(map); // ���ŃA�j���[�V�����i�s
		state->erase_anime++;
	}
	// ���i�K
	else if(state->erase_anime_count >= dm_erace_speed_2) {
		erase_anime(map); // ���ŃA�j���[�V�����i�s
		set_down_flg(map);// �����ݒ�
		state->erase_anime = 0;
		state->mode_now = dm_mode_ball_down;

		// ���Œ���͂����ɗ�����
		state->erase_anime_count = dm_down_speed;
	}
}

//## �c�����������֐�
static void dm_make_erase_h_line(game_state *state, game_map *map, int start, int chain, int pos_x)
{
	int i, p;

	for(i = start; i < start + chain + 1; i++) {
		p = (i << 3) + pos_x;

		if(map[p].capsel_m_flg[cap_condition_flg] == cap_flg_on) {
			continue;
		}

		// ���ŏ�Ԃɂ���
		map[p].capsel_m_flg[cap_condition_flg] = cap_flg_on;

		// �J�v�Z���̏ꍇ
		if(map[p].capsel_m_flg[cap_virus_flg] < 0) {
			map[p].capsel_m_g = erase_cap_a;
		}
		// �E�C���X�̏ꍇ
		else {
			map[p].capsel_m_g = erase_virus_a;
			state->erase_virus_count++;    // ���ŃE�C���X���̑���
			state->chain_color[3] |= 0x08; // �E�C���X���܂ޘA��
			state->chain_color[3] |= 0x10 << map[p].capsel_m_flg[cap_col_flg];
		}
	}
}

//## �c�������菈���֐�
int dm_h_erase_chack(game_map *map)
{
	int i, j, p, chain, chain_start, chain_col;

	for(i = 0;i < 8;i++) {
		chain = 0;
		chain_col = chain_start = -1;
		for(j = 0;j < 16;j++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {	// �����\������Ă���
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					// �����F�łȂ�����
					if(chain >= 3) {
						// �S�ȏゾ����
						return cap_flg_on;
					}
					if(j > 12) {	// ���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
						break;
					} else {
						chain_start = j;									// �A���J�n�ʒu�̕ύX
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];	// �F�̓���ւ�
						chain = 0;
					}
				} else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;	// �A���ǉ�
					if(j == 15) {	// �ŉ��i�ɒB�����ꍇ
						if(chain >= 3) {
							// �S�ȏゾ����
							return cap_flg_on;
						}
					}
				}
			} else {
				if(chain >= 3) {
					// �S�ȏゾ����
					return cap_flg_on;
				}
				if(j > 12) {	// ���݈ʒu�������R�i�ȓ��̏ꍇ���~
					break;
				} else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return cap_flg_off;
}

//## �c��������Ə��Őݒ�֐�
void dm_h_erase_chack_set(game_state *state, game_map *map)
{
	int i, j, p, end, chain, chain_start, chain_col;

	for(i = 0; i < GAME_MAP_W; i++) {
		chain = 0;
		chain_col = chain_start = -1;

		for(j = end = 0; j < GAME_MAP_H; j++) {
			p = j * GAME_MAP_W + i;

			// �����\������Ă���
			if((map + p)->capsel_m_flg[cap_disp_flg]) {

				// �����F�łȂ�����
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					// �S�ȏゾ����
					if(chain >= 3) {
						// ���ŏ�Ԃɂ���
						dm_make_erase_h_line(state,map,chain_start,chain,i);

						if(state->chain_count == 0) {
							// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
							state->chain_color[3] |= 1 << chain_col;
						}
						state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
						state->chain_line++;             // ���ŗ񐔂̃J�E���g
					}

					// ���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
					if(j > 12) {
						end = 1;
					}
					else {
						// �A���J�n�ʒu�̕ύX
						chain_start = j;

						// �F�̓���ւ�
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];

						chain = 0;
					}
				}
				else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					// �A���ǉ�
					chain++;

					// �ŉ��i�ɒB�����ꍇ
					if(j == 15) {
						// �S�ȏゾ����
						if(chain >= 3) {
							// ���ŏ�Ԃɂ���
							dm_make_erase_h_line(state,map,chain_start,chain,i);

							if(state->chain_count == 0) {
								// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
								state->chain_color[3] |= 1 << chain_col;
							}
							state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
							state->chain_line++;             // ���ŗ񐔂̃J�E���g
						}
					}
				}
			}
			else {
				// �S�ȏゾ����
				if(chain >= 3) {
					// ���ŏ�Ԃɂ���
					dm_make_erase_h_line(state,map,chain_start,chain,i);

					if(state->chain_count == 0) {
						// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
						state->chain_color[3] |= 1 << chain_col;
					}
					state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
					state->chain_line++;             // ���ŗ񐔂̃J�E���g
				}

				// ���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
				if(j > 12) {
					end = 1;
				}
				else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}

			if(end) {
				break;
			}
		}
	}
}

//## �������������֐�
static void dm_make_erase_w_line(game_state *state, game_map *map, unsigned int start, unsigned int chain, unsigned int pos_y)
{
	int i, p;

	for(i = start; i < start + chain + 1; i++) {
		p = (pos_y << 3) + i;

		if(map[p].capsel_m_flg[cap_condition_flg] == cap_flg_on) {
			continue;
		}

		// ���ŏ�Ԃɂ���
		map[p].capsel_m_flg[cap_condition_flg] = cap_flg_on;

		// �J�v�Z���̏ꍇ
		if(map[p].capsel_m_flg[cap_virus_flg] < 0) {
			map[p].capsel_m_g = erase_cap_a;
		}
		// �E�C���X�̏ꍇ
		else {
			map[p].capsel_m_g = erase_virus_a;
			state->erase_virus_count++;    // ���ŃE�C���X���̑���
			state->chain_color[3] |= 0x08; // �E�C���X���܂ޘA��
			state->chain_color[3] |= 0x10 << map[p].capsel_m_flg[cap_col_flg];
		}
	}
}

//## ���������菈���֐�
int dm_w_erase_chack(game_map *map)
{
	int i, j, p, chain, chain_start, chain_col;

	// ������
	for(j = 0;j < 16;j++) {
		chain = 0;
		chain_col = chain_start = -1;
		for(i = 0;i < 8;i++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// �����F�łȂ�����
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {
					if(chain >= 3) {
						// �S�ȏゾ����
						return cap_flg_on;
					}
					if(i > 4) {
						break;
					} else {
						chain_start = i;									// �A���J�n�ʒu�̕ύX
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];	// �F�̓���ւ�
						chain = 0;											// �A�����N���A
					}
				} else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;
					if(i == 7) {
						if(chain >= 3) {
							return cap_flg_on;
						}
					}
				}
			} else {
				// �S�ȏゾ����
				if(chain >= 3) {
					// �S�ȏゾ����
					return cap_flg_on;
				}
				if(i > 4) {
					break;
				} else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return cap_flg_off;
}

//## ����������Ə��Őݒ�֐�
void dm_w_erase_chack_set(game_state *state, game_map *map)
{
	unsigned int p, end;
	int i, j, chain, chain_start, chain_col;

	// ������
	for(j = 0;j < 16;j++) {
		chain = 0;
		chain_col = chain_start = -1;

		for(i = end = 0;i < 8;i++) {
			p = (j << 3) + i;

			if((map + p)->capsel_m_flg[cap_disp_flg]) {

				// �����F�łȂ�����
				if((map + p)->capsel_m_flg[cap_col_flg] != chain_col) {

					// �S�ȏゾ����
					if(chain >= 3) {
						// ���ŏ�Ԃɂ���
						dm_make_erase_w_line(state,map,chain_start,chain,j);

						if(state->chain_count == 0) {
							// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
							state->chain_color[3] |= 1 << chain_col;
						}
						state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
						state->chain_line++;             // ���ŗ񐔂̃J�E���g
					}

					if(i > 4) {
						end = 1;
					}
					else {
						// �A���J�n�ʒu�̕ύX
						chain_start = i;

						// �F�̓���ւ�
						chain_col = (map + p)->capsel_m_flg[cap_col_flg];

						// �A�����N���A
						chain = 0;
					}
				}
				else if((map + p)->capsel_m_flg[cap_col_flg] == chain_col) {
					chain++;
					if(i == 7) {
						// �S�ȏゾ����
						if(chain >= 3) {
							// ���ŏ�Ԃɂ���
							dm_make_erase_w_line(state,map,chain_start,chain,j);

							if(state->chain_count == 0) {
								// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
								state->chain_color[3] |= 1 << chain_col;
							}
							state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
							state->chain_line++;             // ���ŗ񐔂̃J�E���g
						}
					}
				}
			}
			else {
				// �S�ȏゾ����
				if(chain >= 3) {
					// ���ŏ�Ԃɂ���
					dm_make_erase_w_line(state,map,chain_start,chain,j);

					if(state->chain_count == 0) {
						// �P��ڂ̏������F�̃r�b�g�𗧂Ă�
						state->chain_color[3] |= 1 << chain_col;
					}
					state->chain_color[chain_col]++; // ���ŐF�̃J�E���g
					state->chain_line++;             // ���ŗ񐔂̃J�E���g
				}

				if(i > 4) {
					end = 1;
				}
				else {
					chain = 0;
					chain_col = chain_start = -1;
				}
			}

			if(end) {
				break;
			}
		}
	}
}

//## �c�����菈���֐�
void dm_h_ball_chack(game_map *map)
{
	unsigned int i, j, p, p2;

	for(i = 0;i < 8;i++) {
		for(j = 0;j < 16;j++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// �\������Ă���
				if((map + p)->capsel_m_g == capsel_u) {
					// �㑤�̃J�v�Z����������
					p2 = ((j + 1) << 3) + i;
					if((map + p2)->capsel_m_g != capsel_d) {	// �P�i���𒲂ׂ�
						(map + p)->capsel_m_g = capsel_b;
					}
				} else if((map + p)->capsel_m_g == capsel_d) {
					// �����̃J�v�Z����������
					if((map + p)->pos_m_y == 1) {	// �ŏ�i�ŉ����̃J�v�Z����
						// �����I�ɗ��ɂ���
						(map + p)->capsel_m_g = capsel_b;
					} else {
						p2 = ((j - 1) << 3) + i;
						if((map + p2)->capsel_m_g != capsel_u) {	// �P�i��𒲂ׂ�
							(map + p)->capsel_m_g = capsel_b;
						}
					}
				}
			}
		}
	}
}

//## �������菈���֐�
void dm_w_ball_chack(game_map *map)
{
	unsigned int i, j, p;

	for(j = 0;j < 16;j++) {
		for(i = 0;i < 8;i++) {
			p = (j << 3) + i;
			if((map + p)->capsel_m_flg[cap_disp_flg]) {
				// �\������Ă���
				if((map + p)->capsel_m_g == capsel_l) {
					// �����̃J�v�Z���̏ꍇ
					if((map + p + 1)->capsel_m_g != capsel_r) {	// ��E�𒲂ׂ�
						(map + p)->capsel_m_g = capsel_b;
					}
				} else if((map + p)->capsel_m_g == capsel_r) {
					// �E���̃J�v�Z���̏ꍇ
					if((map + p - 1)->capsel_m_g != capsel_l) {	// ����𒲂ׂ�
						(map + p)->capsel_m_g = capsel_b;
					}
				}
			}
		}
	}
}

//## �Q�[���I�[�o�[�̎��̈Â��Ȃ�֐�
int dm_black_up(game_state *state, game_map *map)
{
	u8	i,p;

	if(state->flg_retire && !state->flg_game_over) {
		// ���^�C�A�t���O�������Ă��邪�A�Q�[���I�[�o�[�t���O�͗����Ă��Ȃ����

		state->erase_anime_count ++;
		if(state->erase_anime_count >= dm_black_up_speed) {	// ���Ԋu�����
			state->erase_anime_count = 0;

			p = (state->black_up_count - 1) << 3;
			for(i = 0;i < 8;i++) {
				if((map + p + i)->capsel_m_flg[cap_disp_flg]) {
					// �J�v�Z��(�E�C���X)���\������Ă�����
					(map + p + i)->capsel_m_p += 3;	// �Â��F�ɐݒ�
				}
			}
			state->black_up_count --;
			if(state->black_up_count == 0) {	// �ŏ�i�܂ŏ������I�������I��
				state->flg_game_over = 1;
				return cap_flg_on;	// �I��
			}
		}
	}
	return cap_flg_off;	// �܂���ƒ�
}

//## ����ݒ�
int dm_broken_set(game_state *state, game_map *map)
{
	unsigned int i, j, chack;
	u16 work[DAMAGE_MAX][2];
	u8 work_b[DAMAGE_MAX][6];
	int ret = 0;

	// ���ꔻ��
	if(state->cap_attack_work[0][0] != 0x0000) {
		// ��Q����

		if(state->chain_line_max < state->chain_line) {	// �ő���ŗ񐔂̕ۊ�
			state->chain_line_max = state->chain_line;
		}

		state->chain_line = 0;		// ���Ń��C�����̃��Z�b�g
		state->chain_count = 0;		// �A�����̃��Z�b�g
		state->erase_virus_count = 0;	// ���ŃE�C���X���̃��Z�b�g
		state->erase_virus_count_old = 0;

		// ���ŐF�̃��Z�b�g
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}

		for(i = 0, j = 7; i < 16; i += 2, j--) {
			chack = state->cap_attack_work[0][0] & (0x0003 << i);
			if(chack != 0) {
				// �J�v�Z���Z�b�g
				set_map(map, j, 1, capsel_b, (chack >> i) - 1);
			}
		}

		// ��������
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// �����ݒ�
		set_down_flg(map);

		// ����f�[�^�̃V�t�g
		for(i = 0; i < DAMAGE_MAX; i++) {
			work[i][0] = state->cap_attack_work[i][0]; // �f�[�^�̕ۑ�
			work[i][1] = state->cap_attack_work[i][1]; // �f�[�^�̕ۑ�
			state->cap_attack_work[i][0] = 0x0000; // �f�[�^�N���A
			state->cap_attack_work[i][1] = 0x0000; // �f�[�^�N���A
		}
		for(i = 0, j = 1; i < DAMAGE_MAX - 1; i++, j++) {
			state->cap_attack_work[i][0] = work[j][0];
			state->cap_attack_work[i][1] = work[j][1];
		}

		// ����
		ret = 1;
	}

	return ret;
}

//## �E�B���X�������̓��_�\�����W�����߂�
static void dm_calc_erase_score_pos(game_state *state, game_map *map, int pos[2])
{
	int count, x, y;

	// ���W�n���N���A
	pos[0] = pos[1] = 0;

	count = 0;

	for(y = 0; y < GAME_MAP_H; y++) {
		for(x = 0; x < GAME_MAP_W; x++) {
			game_map *mp = &map[x + y * GAME_MAP_W];

			// ���ŃA�j���[�V�������łȂ���Β��f
			if(!mp->capsel_m_flg[cap_condition_flg]) {
				continue;
			}

			pos[0] += mp->pos_m_x;
			pos[1] += mp->pos_m_y;
			count++;
		}
	}

	if(count > 0) {
		pos[0] = pos[0] * state->map_item_size / count;
		pos[1] = pos[1] * state->map_item_size / count;
	}
}

//## ����J�v�Z���̕`��ʒu�����߂�
static int dm_calc_capsel_pos(game_state *state, int *xx, int *yy)
{
	game_cap *n_cap = &state->now_cap;
	float aa, bb, cc, dd, ee, ff, ii;
	int gg, hh;
	int i, j, k, m, n, q;
	int curve = 36;

	// �\���t���O�������Ă��Ȃ���Β��f
	if(!n_cap->capsel_flg[cap_disp_flg]) {
		return FALSE;
	}

	// ������
	if(state->mode_now == dm_mode_throw) {

		if(state->cap_def_speed == SPEED_LOW) {
			gg = (state->cap_speed_count >> 2);
		}
		else {
			gg = (state->cap_speed_count >> 1);
		}

		gg = gg % 4;
		gg = ONES_ooAB + gg;
		hh = 0;
		if(ONES_8_AB == gg || ONES_8_BA == gg) {
			hh = 5;
		}

		// �����J�v�Z�����c�������ꍇ
		if(state->now_cap.pos_x[0] == state->now_cap.pos_x[1]) {
			if(gg == ONES_ooAB || gg == ONES_ooBA) {
				throw_rotate_capsel(&state->now_cap);
			}
		}
		// �����J�v�Z�������������ꍇ
		else {
			if(gg == ONES_8_AB || gg == ONES_8_BA) {
				throw_rotate_capsel(&state->now_cap);
			}
		}

		// �ʂ�`��
		if(state->cap_speed_count < (FlyingCnt[state->cap_def_speed] / 3)) {
			// 1/3 �o����
			ii = curve / (FlyingCnt[state->cap_def_speed] / 3) * state->cap_speed_count;
		}
		else if(state->cap_speed_count < (FlyingCnt[state->cap_def_speed] / 3 << 1)) {
			// 2/3 ���Ԉʒu
			ii = curve;
		}
		else {
			ii = (FlyingCnt[state->cap_def_speed] << 1) / 3;
			ii = state->cap_speed_count - ii;
			ii = curve - (ii * 2);
		}

		// ���e�n�_ x,y
		aa = (state->map_x + (state->map_item_size * n_cap->pos_x[0]));
		bb = (state->map_y + (state->map_item_size * (n_cap->pos_y[0] - 1)));

		// �J�n�n�_ x,y
		cc = MARIO_THROW_X; // state->map_x + 101;
		dd = MARIO_THROW_Y; // state->map_y + 16;

		// ����ʒu x,y
		ee = aa+(((cc-aa)/(FlyingCnt[state->cap_def_speed]-1))*(FlyingCnt[state->cap_def_speed] - state->cap_speed_count));
		ff = bb+(((dd-bb)/(FlyingCnt[state->cap_def_speed]-1))*(FlyingCnt[state->cap_def_speed] - state->cap_speed_count));

		for(i = 0; i < 2; i++) {
			xx[i] = (u32)ee + hh + (state->map_item_size * (n_cap->pos_x[i] - 3));
			yy[i] = ((u32)ff + hh - (u16)ii) + 1 + (state->map_item_size * (n_cap->pos_y[i] + 1));
		}
	}
	// �����҂�
	else if(n_cap->pos_y[0] < 1) {
		// �J�[�\���ʒu�̊���o��
		for(i = 0; i < 2; i++) {
			xx[i] = state->map_x + (state->map_item_size * n_cap->pos_x[i]);
			yy[i] = state->map_y + (state->map_item_size * n_cap->pos_y[i]) - 10;
		}
	}
	// ������
	else if(n_cap->pos_y[0] > 0) {
		// �J�[�\���ʒu�̊���o��
		for(i = 0; i < 2; i++) {
			xx[i] = state->map_x + (state->map_item_size * n_cap->pos_x[i]);
			yy[i] = state->map_y + (state->map_item_size * n_cap->pos_y[i]) + 1;
		}
	}
	// �`�悹���ɏI��
	else {
		return FALSE;
	}

	return TRUE;
}

//## ����J�v�Z����`��
static void dm_draw_capsel_by_gfx(game_state *state, int *xx, int *yy)
{
	game_cap *cap = &state->now_cap;
	STexInfo *tex, *pal;
	int i, type;

	gSPDisplayList(gp++, normal_texture_init_dl);

	// �J�v�Z���̑傫���𔻕�
	if(state->map_item_size == cap_size_10) {
		type = 0;
	}
	else {
		type = 1;
	}

	// �J�v�Z���̃e�N�X�`����ǂݍ���
	tex = dm_game_get_capsel_tex(type);
	load_TexBlock_4b(tex->addr[1], tex->size[0], tex->size[1]);

	for(i = 0; i < 2; i++) {

		// �J�v�Z���̃p���b�g��ǂݍ���
		pal = dm_game_get_capsel_pal(type, cap->capsel_p[i]);
		load_TexPal(pal->addr[0]);

		// �J�v�Z����`��
		draw_Tex(xx[i], yy[i], state->map_item_size, state->map_item_size,
			0, cap->capsel_g[i] * state->map_item_size);
	}
}

//## ����J�v�Z����CPU�ŕ`��
static void dm_draw_capsel_by_cpu(game_state *state, int *xx, int *yy)
{
	game_cap *cap = &state->now_cap;
	STexInfo *inf;
	int i, x, y, type, bits, texel;
	int texStep, cfbStep;
	u16 *pal, *cfb;
	u8 *tex;

	// �J�v�Z���̑傫���𔻕�
	if(state->map_item_size == cap_size_10) {
		type = 0;
	}
	else {
		type = 1;
	}

	for(i = 0; i < 2; i++) {

		// ��ʒ[����͂ݏo��ꍇ�͒��f
		if(yy[i] < 0 || yy[i] + state->map_item_size > SCREEN_HT) {
			continue;
		}
		if(xx[i] < 0 || xx[i] + state->map_item_size > SCREEN_WD) {
			continue;
		}

		// �p���b�g�̃A�h���X�擾
		inf = dm_game_get_capsel_pal(type, cap->capsel_p[i]);
		pal = (u16 *)inf->addr[0];

		// �e�N�Z���̃A�h���X�擾
		inf = dm_game_get_capsel_tex(type);
		tex = (u8 *)inf->addr[1];
		tex += (cap->capsel_g[i] * state->map_item_size * inf->size[0]) >> 1;
		texStep = (inf->size[0] - state->map_item_size) >> 1;

		// �t���[���o�b�t�@�̃A�h���X���擾
		cfb = &fbuffer[wb_flag ^ 1][yy[i] * SCREEN_WD + xx[i]];
		cfbStep = SCREEN_WD - state->map_item_size;

		// �c���[�v
		for(y = 0; y < state->map_item_size; y++) {

			// �����[�v
			for(x = 0; x < state->map_item_size; x += 2) {

				// �e�N�X�`���f�[�^���P�o�C�g�擾
				bits = *tex;

				// �����h�b�g��`��
				if(texel = bits >> 4) {
					cfb[0] = pal[texel];
				}

				// ��h�b�g��`��
				if(texel = bits & 0x0f) {
					cfb[1] = pal[texel];
				}

				tex++;
				cfb += 2;
			}

			tex += texStep;
			cfb += cfbStep;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### �E�B���X��������
#if 0
/*
typedef struct {
	int	level;		// �Q�[�����x�� 0:easy, 1:normal, 2:hard
	int	cup_cnt;	// �J�v�Z���𗎂Ƃ����ƂɃJ�E���g����
	int	virus;		// �E�C���X�������������J�E���g����
	int	time;		// �P�t���[���P�ʂ̎��ԁi�`�F�b�N���ŃJ�E���g���Ă���j
	int	bottom_up;
} TAIKYU_PARAM;

//## ������
static void init_taikyu_param(TAIKYU_PARAM *st, int level)
{
	st->level = level;
	st->cup_cnt = 0;
	st->virus = 0;
	st->time = 0;
	st->bottom_up = FALSE;
}

//## ����
//
//	�E�C���X�������������
//	�O�񑝉������Ƃ����A
//		�P�D�E�C���X�����ȏ�������i easy:10   normal:7   hard:5 �j
//		�Q�D���Ԃ����b�o�߂����i easy:40   normal:30   hard:20 �j
//		�R�D�J�v�Z���������Ƃ����i easy:20   normal:15   hard:10 �j
//
// return:
//		TRUE	�E�C���X����i�A�b�v
static int	virus_add_check(TAIKYU_PARAM *st)
{
	static	int	virus_chk[] = { 10, 7 ,5 };
	static	int	time_chk[] = { 40*FRAME_PAR_SEC, 30*FRAME_PAR_SEC ,20*FRAME_PAR_SEC };
	static	int	cup_chk[] = { 20, 15 ,10 };

	if(evs_gamemode != GMD_TaiQ) {
		return FALSE;
	}

	if ( virus_chk[st->level] <= st->virus ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	if ( time_chk[st->level] <= st->time ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	if ( cup_chk[st->level] <= st->cup_cnt ) {
		st->cup_cnt = 0;
		st->virus = 0;
		st->time = 0;
		return st->bottom_up = TRUE;
	}

	st->time++;
	return FALSE;
}
*/
#endif

//////////////////////////////////////////////////////////////////////////////
//{### �\����

//## �^�C���A�^�b�N�̌��ʕ\��
typedef struct {
	int level;
	bool bonus;
	int flow;
	int frame;
	int cnt;

	u32 time;
	int combo;
	int virus;
	u32 score, scoreDelta;
	u32 result;
} _TimeAttackResult;

//## �X�R�A�̈ꌅ
typedef struct {
	int pos[2];
	int number;
	int color;
	float time;
} _ScoreNum;

//## �X�R�A
typedef struct {
	_ScoreNum numbers[8];
	int index;
} _ScoreNums;

//## ���\����
typedef struct {
	int *xtbl, *ytbl;
	int frame[16];
} _StarForce;

typedef struct {
	// ���v���C�Đ����t���O
	int replayFlag;

	// �����_���̃V�[�h�l
	int randSeed;
	int randSeed2;




	// �X�^�[�`��ʒu�̊i�[�ꏊ
	int star_pos_x[12];
	int star_pos_y[12];
	int star_count;
	_StarForce starForce;

	// �X�R�A�G�t�F�N�g
	_ScoreNums scoreNums[4];

	int retry_type[4];   // ���g���C���j���[�̎��
	int retry_select[4]; // ���g���C�̃J�[�\���ʒu
	int retry_result[4]; // ���g���C�̑I������
	int retry_coin;      // ���g���C�p�̃R�C���̖���

	// �����I�ɑ���J�v�Z����`�悷��J�E���^
	int force_draw_capsel_count[4];

	// �J�[�e�����㉺�����邽�߂̃J�E���^
	int curtain_count;
	int curtain_step;

	// �r�A�p�l�����ړ�������Ƃ��̃J�E���^
	#define FRAME_MOVE_MAX 20
	int frame_move_count;
	int frame_move_step;

	// ���s���̊�A�j���[�V�����J�E���^
	int face_anime_count[4];

	// �J�E���g�_�E�������̐���
	int count_down_ctrl;

	int eep_rom_flg;    // EEP �������ݗp�t���O
	int bgm_bpm_flg[2]; // BGM�̑�����ς���t���O
	int last_3_se_cnt;  // 
	int last_3_se_flg;  // �E�C���X���R�ȉ��ɂȂ����Ƃ���SE��炷���߂̃t���O
	int started_game_flg; // �Q�[�����J�n���ꂽ���Ƃ������t���O

	// ����E�B���X
	float big_virus_wait;
	float big_virus_timer;
	float big_virus_pos[3][2]; // ����E�C���X�̍��W ( x, y, degree )
	float big_virus_rot[3];
	float big_virus_scale[3];  // �X�P�[��
	int   big_virus_flg[3];    // ���ŃA�j���[�V��������񂾂��ݒ肷��t���O
	bool  big_virus_no_wait;   // �m�[�E�F�C�g�t���O
	int   big_virus_stop_count;// ��~�J�E���g
	int   big_virus_blink_count;// �_�ŃJ�E���g
	u8    big_virus_count[3];  // �E�C���X���̊e�F�ʂ̌��̕ϐ�

	// �f���̃^�C�}�[, �f���t���O
	int demo_timer;
	int demo_flag;

	// �\�����_�ŏ����J�E���^
	int blink_count;

	// �J�v�Z���ςݏオ�莞�� SE �Đ��J�E���^
	int warning_se_count;

	// �I�u�W�F�N�gDL
	void *objSeg;

	// �e��e�N�X�`��
	STexInfo *texAL; // ����
	STexInfo *texLS; // ���x���Z���N�g
	STexInfo *texP1; // ��l�p
	STexInfo *texP2; // ��l�p
	STexInfo *texP4; // �l�l�p
	STexInfo *texItem; // �J�v�Z��
	STexInfo *texKaSa; // �J�T�}��

	// �E�B���X�L�����A�j���[�V����
	SAnimeState virus_anime_state[3];

	// �E�B���X���Ŏ��̉��A�j���[�V����
	SAnimeSmog virus_smog_state[3];

	// �O���t�B�b�N�X���b�h�̃v���C�I���e�B
	int graphic_thread_pri;

	// ���������s�����Ƃ��ɁA1��� �w�i�݂̂̕`����s�����߂̃t���O
	u8  *bg_snap_buf;
	bool bg_snapping;

	// �G�t�F�N�g�f�[�^�̃A�h���X
	u8 *effect_data_buf;
	int effect_timer[4];

	int touch_down_wait; // ���ٶ�߾ق̉��ɉ�������Ƃ��̳��Ă����Z

	int win_count[4];    // �������J�E���^
	int vs_win_count[2]; // 2PLAY & VSCOM �������J�E���^(�R���łP������)
	int vs_win_total[2]; // 2PLAY & VSCOM �������J�E���^(�\���p)

	int vs_4p_player_count; // �v���C���[�̎Q����
	int vs_4p_team_flg;     // �`�[���΍R��̃t���O
	int vs_4p_team_bits[2]; // �`�[���΍R��̔���p�t���O

	int story_4p_name_num[4];     // �X�g�[���[�� 4P ���� �`�[���̃����o�[��
	int story_4p_stock_cap[4][4]; // �X�g�[���[�� 4P ���� �X�g�b�N�J�v�Z���p�z��(���������Ƃ��� -1 ������)

	// �X�^�b�t���[���p
	SMsgWnd msgWnd;

	// �R�C��
	int coin_count;
	int coin_time[COINMAX];

	// �R�[�q�[�u���[�N����
	int coffee_break_flow;
	int coffee_break_timer;
	int coffee_break_level;
	int coffee_break_shard;

	// �ϋv���[�h���A��i�V�t�g����t���O
	bool bottom_up_flag;

	// �|�[�Y�A�f�o�b�O�A�R���t�B�O �v����
	int query_play_pause_se;
	int query_pause_player_no;
	int query_debug_player_no;
	int query_config_player_no;

	// �^�C���A�^�b�N�̌��ʕ\��
	_TimeAttackResult timeAttackResult[2];

	// �p�X���[�h
	SMsgWnd passWnd;
	int passAlphaStep;
	#define PASSWORD_LENGTH 20
	u8 passBuf[PASSWORD_LENGTH * 2 + 2];

	// EEPROM�������ݒ��̌x�����b�Z�[�W
	RecWritingMsg writingMsg;
} Game;

typedef struct {
	Game game;
	game_state state[4];
	game_map map[4][GAME_MAP_W * (GAME_MAP_H + 1)];
	u32 highScore;
	u32 timer;
} GameBackup;

typedef struct {
	Mtx mtxBuf[GFX_GTASK_NUM][32];
	Vtx vtxBuf[GFX_GTASK_NUM][128];
} Geometry;

//////////////////////////////////////////////////////////////////////////////
//{### �ϐ�

// �q�[�v�g�b�v
static void *heapTop;

// �E�H�b�`
Game *watchGame;

// ���v���C���̃o�b�N�A�b�v�o�b�t�@
GameBackup *gameBackup[2];

// �W�I���g���[
Geometry *gameGeom;

// �v�l�J�n�t���O
int dm_think_flg = 0;

// �����n�_�ɃJ�v�Z����\������t���O
static int visible_fall_point[4] = { 0,0,0,0 };

#ifdef NN_SC_PERF
static int _disp_meter_flag = 0;
#endif

// �r�̒��̏��
game_map game_map_data[4][GAME_MAP_W * (GAME_MAP_H + 1)];

// �E�C���X�ݒ�p�z��
virus_map virus_map_data[4][GAME_MAP_W * GAME_MAP_H];

// �e�v���C���[�̏�ԋy�ѐ���
game_state game_state_data[4];

// �E�C���X�ݒ菇�p�z��
u8 virus_map_disp_order[4][96];

// �|�[�Y���A�Q�[���I�����̃p�l���̍��ڐ�
#define PAUSE_NEX_END        0
#define PAUSE_NEX_TRY_END    1
#define CONTINUE_REP_END     2
#define CONTINUE_REP_TRY_END 3
#define CONTINUE_REP_NEX     4
#define CONTINUE_REP_NEX_END 5
static const u8 _retryMenu_itemCount[] = {
	2, // �|�[�Y��     [������] [�I���]
	3, //              [������] [������x] [�I���]
	2, // �Q�[���I���� [���v���C] [�I���]
	3, //              [���v���C] [������x] [�I���]
	2, //              [���v���C] [������]
	3, //              [���v���C] [������] [�I���]
};

// �ϋv���[�h : �E�B���X��]�E�F�C�g�͈̔�
static const float _big_virus_def_wait[] = { 12.5, 10, 7.5 };
static const float _big_virus_min_wait[] = { 5, 4.5, 4 };
static const float _big_virus_max_wait[] = { 12.5, 10, 7.5 };

// �X�R�A�̐F
static const u8 _scoreNumsColor[SCORE_MAX][3] = {
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
	{ 255, 255, 255 },
};

// ���Z
#if LOCAL==JAPAN
#include "tech.mes"
#elif LOCAL==AMERICA
#include "tech_am.mes"
#elif LOCAL==CHINA
#include "tech_zh.mes"
#endif

#if (LOCAL == CHINA)
#include "record_zh.mes"
#else
#include "record.mes"
#endif

//////////////////////////////////////////////////////////////////////////////
//{### �`����W

//////////////////////////////////////
//## �X�g�[���[���[�h

// �X�^�[ (�X�g�[���[���[�h)
#define _posStP4StarY 13
static const int _posStP4StarX[] = { 205, 230, 255, 280 };

// �X�^�[
static const int _posStStar[][2] = {
	{ 128, 72 }, { 172, 72 },
};

//////////////////////////////////////
//## ���x���Z���N�g���[�h

// �^�C��
static const int _posLsTime[] = { 39, 95 };

//////////////////////////////////////
//## ��l�p

// �X�^�[
static const int _posP2StarX[2] = { 128, 172 };
static const int _posP2StarY[][3] = {
	{ 72, -1, -1 }, { 80, 64, -1 }, { 88, 72, 56 },
};

//////////////////////////////////////
//## �l�l�p

// �{�g��
static const int _posP4Bottle[][2] = {
	{ 16, 37 }, { 88, 37 }, { 160, 37 }, { 232, 37 },
};

// �{�g���̒��g (�{�g������̑��΍��W)
static const int _posP4BottleIn[] = { 4, 3 };

// �L�����p�l��
static const int _posP4CharBase[][2] = {
	{ 16, 186 }, { 88, 186 }, { 160, 186 }, { 232, 186 },
};

// �X�^�[ (�c�̐�)
#define _posP4TeamStarY 13
static const int _posP4TeamStarX[][2][3] = {
	{ {  45,  -1,  -1, },
	  { 191,  -1,  -1, } },
	{ {  34,  57,  -1, },
	  { 179, 202,  -1, } },
	{ {  22,  45,  68, },
	  { 167, 190, 214, } },
};

// �X�^�[ (�l��)
#define _posP4CharStarY 13
static const int _posP4CharStarX[][4][3] = {
	{ {  41,  -1,  -1, },
	  { 113,  -1,  -1, },
	  { 185,  -1,  -1, },
	  { 257,  -1,  -1, } },
	{ {  30,  53,  -1, },
	  { 102, 125,  -1, },
	  { 174, 197,  -1, },
	  { 246, 269,  -1, } },
	{ {  18,  42,  65, },
	  {  90, 114, 137, },
	  { 162, 186, 209, },
	  { 234, 258, 281, } },
};

// �X�g�b�N�J�v�Z��
static const int _posP4StockCap[][2] = {
	{ 95, 24 }, { 240, 24 },
};

//////////////////////////////////////
//## �X�g�[���[���[�h�Ɠ�l�p�ŋ��ʂ���̕`�敨�ʒu�f�[�^

// �E�B���X��
static const int _posP2VirusNum[][2] = { { 138, 210 }, { 182, 210 } };

// �L�����N�^�̘g
static const int _posP2CharFrm[][2] = { { 138, 149 }, { 181, 149 } };

//////////////////////////////////////////////////////////////////////////////
//{### �O�����J�p�֐�

// �q�[�v
void **dm_game_heap_top()
{
	return &heapTop;
}

// �E�B���X�L�����A�j���[�V����
SAnimeState *get_virus_anime_state(int index)
{
	Game *st = watchGame;
	return &st->virus_anime_state[index];
}

// �E�B���X���Ŏ��̉��A�j���[�V����
SAnimeSmog *get_virus_smog_state(int index)
{
	Game *st = watchGame;
	return &st->virus_smog_state[index];
}

// �}�g���b�N�X�o�b�t�@
Mtx *dm_get_mtx_buf()
{
	return gameGeom->mtxBuf[gfx_gtask_no];
}

// �o�[�e�b�N�X�o�b�t�@
Vtx *dm_get_vtx_buf()
{
	return gameGeom->vtxBuf[gfx_gtask_no];
}

//////////////////////////////////////////////////////////////////////////////
//{### �G�t�F�N�g

#define EFFECT_WIN_WAIT  180
#define EFFECT_LOSE_WAIT 180
#define EFFECT_DRAW_WAIT 180
#define EFFECT_PAUSE_WAIT 180
#define EFFECT_GAMEOVER_WAIT 180
#define EFFECT_NEXTSTAGE_WAIT 180
#define EFFECT_RETIRE_WAIT 180

//## �G�t�F�N�g��������
static void effectAll_init()
{
	Game *st = watchGame;
	int i;

	for(i = 0; i < evs_playcnt; i++) {
		st->effect_timer[i] = 0;
	}

	initEtcWork((u32)st->effect_data_buf, evs_playcnt);
}

//## �G�t�F�N�g�̏�����
static void _effectX_init(int playerNo, int timer)
{
	Game *st = watchGame;

	st->effect_timer[playerNo] = timer;
}

//## �����G�t�F�N�g�̏�����
static void effectWin_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_WIN_WAIT);
}

//## �s�k�G�t�F�N�g�̏�����
static void effectLose_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_LOSE_WAIT);
}

//## �����G�t�F�N�g�̏�����
static void effectDraw_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_DRAW_WAIT);
}

//## �{�[�Y�G�t�F�N�g�̏�����
static void effectPause_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_PAUSE_WAIT);
	init_pause_disp();
}

//## �Q�[���I�[�o�[�G�t�F�N�g�̏�����
static void effectGameOver_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_GAMEOVER_WAIT);
}

//## �l�N�X�g�X�e�[�W�G�t�F�N�g�̏�����
static void effectNextStage_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_NEXTSTAGE_WAIT);
}

//## ���^�C�A�G�t�F�N�g�̏�����
static void effectRetire_init(int playerNo)
{
	_effectX_init(playerNo, EFFECT_RETIRE_WAIT);
}

//## �G�t�F�N�g�̌v�Z����
static bool _effectX_calc(int playerNo)
{
	Game *st = watchGame;

	if(st->effect_timer[playerNo]) {
		st->effect_timer[playerNo]--;
	}

	return st->effect_timer[playerNo] != 0;
}

//## �����G�t�F�N�g�̌v�Z����
static bool effectWin_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## �s�k�G�t�F�N�g�̌v�Z����
static bool effectLose_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## �����G�t�F�N�g�̌v�Z����
static bool effectDraw_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## �|�[�Y�G�t�F�N�g�̌v�Z����
static bool effectPause_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## �Q�[���I�[�o�[�G�t�F�N�g�̌v�Z����
static bool effectGameOver_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## �l�N�X�g�X�e�[�W�G�t�F�N�g�̌v�Z����
static bool effectNextStage_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//## ���^�C�A�G�t�F�N�g�̌v�Z����
static bool effectRetire_calc(int playerNo)
{
	return _effectX_calc(playerNo);
}

//////////////////////////////////////////////////////////////////////////////
//{### ���[�`��

//## �^�C���A�^�b�N�̌��ʕ\�� : �c�莞�ԁA�A�����A�������E�B���X�A�X�R�A ��ݒ�
static void timeAttackResult_set(_TimeAttackResult *st,
	int level, bool bonus,
	u32 time, int combo, int virus, u32 score)
{
	// �Q�[�����x��
	st->level = level;

	// �{�[�i�X�̗L��
	st->bonus = bonus;

	// �����̗���
	st->flow = 0;

	// �t���[���J�E���^
	st->frame = 0;

	// �c�莞�ԁA�A�����A�������E�B���X
	st->time  = time;
	st->combo = combo;
	st->virus = virus;

	// �X�R�A
	st->score = score;
	st->scoreDelta = 0;
}

//## �^�C���A�^�b�N�̌��ʕ\�� : ������
static void timeAttackResult_init(_TimeAttackResult *st)
{
	timeAttackResult_set(st, 0, false, 0, 0, 0, 0);
	st->result = 0;
}

//## �^�C���A�^�b�N�̌��ʕ\�� : �I���m�F
static bool timeAttackResult_isEnd(_TimeAttackResult *st)
{
	return st->flow == 99;
}

//## �^�C���A�^�b�N�̌��ʕ\�� : �X�V
static bool timeAttackResult_update(_TimeAttackResult *st, bool se)
{
	bool res = true;
	int i, j;

	switch(st->flow) {
	case 0:
		if(st->frame < 120) {
			st->frame++;
		}
		else {
			st->flow = 10;
			st->frame = 0;
			st->cnt = 0;
		}
		break;

	case 10:
//		if(se && (st->frame & 7) == 0) {
//			dm_snd_play_in_game(SE_mLeftRight);
//		}

		if(st->time == 0) {
			st->flow = 20;
			st->frame = 0;
		}
		else {
			static int _n[] = { 10, 20, 30 };
			i = MIN(FRAME_PAR_MSEC, st->time);

			st->time -= i;
			if(se) {
				if(st->time == 0) {
					dm_snd_play_in_game(SE_gCoin);
				} else {
					if ( st->cnt % 10 == 0 )
						dm_snd_play_in_game(SE_mLeftRight);
				}
			}

			j = _n[st->level];
			if ( st->cnt >= 200 ) {
				j *= 3;
			} else if ( st->cnt >= 100 ) {
				j *= 2;
			}
			if(st->bonus) {
				st->score = MIN(SCORE_MAX_VAL, st->score + j);
				st->scoreDelta += j;
			}
			st->cnt++;
		}

		st->frame++;
		break;

	case 20:
		if(st->frame < 60) {
			st->frame++;
		}
		else {
			st->flow = 99;
			st->frame = 0;
		}
		break;

	case 99:
		res = !timeAttackResult_isEnd(st);
		break;
	}

	return res;
}

//## �^�C���A�^�b�N�̌��ʕ\�� : �X�L�b�v
static void timeAttackResult_skip(_TimeAttackResult *st)
{
	while(timeAttackResult_update(st, false));
}

//## �^�C���A�^�b�N�̌��ʕ\�� : �l��ݒ� & ���ʂ��擾
static u32 timeAttackResult_result(_TimeAttackResult *st,
	int level, bool bonus,
	u32 time, int combo, int virus, u32 score)
{
	timeAttackResult_set(st, level, bonus, time, combo, virus, score);

	timeAttackResult_skip(st);
	st->result = st->score;

	timeAttackResult_set(st, level, bonus, time, combo, virus, score);

	return st->result;
}

//## �X�R�A�\�� : ������
static void scoreNums_init(_ScoreNums *st)
{
	_ScoreNum *num;
	int i;

	for(i = 0; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[i];
		num->pos[0] = 0;
		num->pos[1] = 0;
		num->number = 0;
		num->color = 0;
		num->time = 1;
	}

	st->index = 0;
}

//## �X�R�A�\�� : �X�V
static void scoreNums_update(_ScoreNums *st)
{
	_ScoreNum *num, *pre;
	float speed = 1.0 / 40.0;
	int i, j;

	for(i = 0, j = st->index; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[j];
		pre = &st->numbers[WrapI(0, ARRAY_SIZE(st->numbers), j - 1)];

		if(pre->time > 0.20) {
			num->time = MIN(1, num->time + speed);
		}

		j = (j + 1) % ARRAY_SIZE(st->numbers);
	}
}

//## �X�R�A�\�� : �ݒ�
static void scoreNums_set(_ScoreNums *st, u32 score, int erase, int x, int y)
{
	int i, color, columns, column[16];

	color = CLAMP(0, SCORE_MAX - 1, erase - 1);
	columns = 0;

	while(score != 0) {
		column[columns] = score % 10;
		score /= 10;
		columns++;
	}

	x -= (7 * columns + 1) / 2;
	y -= 6;

	for(i = columns - 1; i >= 0; i--) {
		_ScoreNum *num = &st->numbers[st->index];

		num->pos[0] = x;
		num->pos[1] = y;
		num->number = column[i];
		num->color = color;
		num->time = 0;

		x += 7;
		st->index = WrapI(0, ARRAY_SIZE(st->numbers), st->index + 1);
	}
}

//## �Q�[���X�e�[�^�X���o�b�N�A�b�v
static void backup_game_state(int bufNo)
{
	Game *st = watchGame;
	GameBackup *bak = gameBackup[bufNo];
	int i, j;

	// Game �\����
	bak->game = *st;

	// �v���C���[���̃X�e�[�^�X
	assert(sizeof(bak->state) == sizeof(game_state_data));
	for(i = 0; i < ARRAY_SIZE(bak->state); i++) {
		bak->state[i] = game_state_data[i];
	}

	// �}�b�v���
	assert(sizeof(bak->map) == sizeof(game_map_data));
	for(i = 0; i < ARRAY_SIZE(bak->map); i++) {
		for(j = 0; j < ARRAY_SIZE(bak->map[i]); j++) {
			bak->map[i][j] = game_map_data[i][j];
		}
	}

	// �n�C�X�R�A
	bak->highScore = evs_high_score;

	// �v���C����
	bak->timer = evs_game_time;
}

//## �Q�[���X�e�[�^�X���C��
static void resume_game_state(int bufNo)
{
	Game *st = watchGame;
	GameBackup *bak = gameBackup[bufNo];
	int i, j;

	// Game �\����
	*st = bak->game;

	// �v���C���[���̃X�e�[�^�X
	for(i = 0; i < ARRAY_SIZE(bak->state); i++) {
		game_state_data[i] = bak->state[i];
	}

	// �}�b�v���
	for(i = 0; i < ARRAY_SIZE(bak->map); i++) {
		for(j = 0; j < ARRAY_SIZE(bak->map[i]); j++) {
			game_map_data[i][j] = bak->map[i][j];
		}
	}

	// �n�C�X�R�A
	evs_high_score = bak->highScore;

	// �v���C����
	evs_game_time = bak->timer;
}

//## ���v���C��Ԃ�
static void set_replay_state()
{
	Game *st = watchGame;

	if(!st->replayFlag) {
		backup_game_state(1);
	}

	resume_game_state(0);
	st->replayFlag = 1;

//	replay_play_init();
}

//## ���v���C��Ԃ���A��
static void reset_replay_state()
{
	Game *st = watchGame;

	if(st->replayFlag) {
		resume_game_state(1);
		st->replayFlag = 0;
	}

//	replay_record_init(evs_playcnt);
}

//## �^��E�Đ� �����s
static void start_replay_proc()
{
	Game *st = watchGame;

	if(st->replayFlag) {
		replay_play_init();
	}
	else {
		replay_record_init(evs_playcnt);
	}
}

//## �ςݏグ�x������炷
void dm_warning_h_line_se()
{
	Game *st = watchGame;
	int i, warning;

	for(i = warning = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait
		&& game_state_data[i].warning_se_flag)
		{
			warning++;
		}
	}

	if(warning) {
		st->warning_se_count++;

		// �x�� SE �Đ�
		if(st->warning_se_count == 1) {
			dm_snd_play_in_game(SE_gWarning);
		}
		else if(st->warning_se_count >= FRAME_PAR_SEC * 5) {
			st->warning_se_count = 0;
		}
	}
	else {
		st->warning_se_count = 0;
	}
}

//## �J�E���g�_�E���r�d���Đ�
static void dm_play_count_down_se()
{
	Game *st = watchGame;
	int i, j;

	if(evs_gamemode != GMD_TIME_ATTACK) {
		return;
	}

	for(i = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait) {
			break;
		}
	}

	if(i == evs_playcnt) {
		return;
	}

	i = SCORE_ATTACK_TIME_LIMIT - evs_game_time;
	j = i / FRAME_PAR_SEC;
	i %= FRAME_PAR_SEC;

	if(i == 0 && j >= 1 && j <= 10) {
		dm_snd_play_in_game(SE_gCount1);
	}
}

//## ����J�v�Z�����������֐�
void dm_capsel_down(game_state *state, game_map *map)
{
	static const int black_color[] = { 0, 3 };
	Game *st = watchGame;
	game_cap *cap;
	unsigned int i, j;

	cap = &state->now_cap;

	if(cap->pos_y[0] > 0) {	// �������̑��x�ݒ�
		i  = FallSpeed[state->cap_speed];	// �J�v�Z���������x�̐ݒ�
		if(cap->pos_y[0] >= 1 && cap->pos_y[0] <= 3) {
			i += BonusWait[cap->pos_y[0] - 1][state->cap_def_speed];	// ��߾ق̍����ɂ���ްŽ���Ă����Z
		}
		j = 0;
		if(cap->pos_y[0] > 0) {
			if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] + 1)) {
				// �J�v�Z���̉��ɃA�C�e�����������ꍇ
//				j = TouchDownWait[evs_playcnt-1];
				j = st->touch_down_wait;
			}
		}
		state->cap_speed_max = i + j;
	}
	else {	// �����O�̑��x�ݒ�(�ꗥ30)
		state->cap_speed_max = 30;
	}

	// �����J�E���^�̑���
	state->cap_speed_count += state->cap_speed_vec;

	// ��莞�ԉ߂����H
	if(state->cap_speed_count < state->cap_speed_max) {
		return;
	}

	// �J�E���^�N���A
	state->cap_speed_count = 0;

	// ��\���̏ꍇ�͏����𒆒f
	if(!cap->capsel_flg[cap_disp_flg]) {
		return;
	}

	// �P�i����
	if(cap->pos_y[0] > 0) {	// ��i�ڂ͋����I�ɗ��Ƃ�

		// �����J�v�Z���ƃ}�b�v���̔���
		if(cap->pos_x[0] == cap->pos_x[1]) {	// �����J�v�Z���͏c���H
			// �J�v�Z�����c�̏ꍇ
			if(get_map_info(map,cap->pos_x[0],cap->pos_y[0] + 1)) {
				// �J�v�Z���̉��ɃA�C�e�����������ꍇ

				cap->capsel_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̃N���A
			}
		}
		else {
			// �J�v�Z�������̏ꍇ
			for(j = 0;j < 2;j++) {
				if(get_map_info(map,cap->pos_x[j],cap->pos_y[j] + 1)) {
					// �J�v�Z���̉��ɃA�C�e�����������ꍇ
					cap->capsel_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̃N���A
					break;
				}
			}
		}
	}

	// �ŉ��i����
	for(i = 0;i < 2;i++) {
		if(cap->pos_y[i] == 16) {
			cap->capsel_flg[cap_down_flg] = cap_flg_off;	// �����t���O�̃N���A
			break;
		}
	}

	// ��������
	if(cap->capsel_flg[cap_down_flg]) {	// �����t���O�������Ă��邩�H
		// ��������
		for(i = 0;i < 2;i++) {
			if(cap->pos_y[i] < 16) {	// �O�̂���
				cap->pos_y[i] ++;
			}
		}
		for(i = 0;i < 2;i++) {
			// ���������ꏊ�ɃA�C�e��������
			if(get_map_info(map,cap->pos_x[i], cap->pos_y[i])) {
				// �Q�[���I�[�o�[�ݒ�
				state->cnd_static = dm_cnd_game_over;

				// �l�N�X�g�J�v�Z���̕\�����~ // �����t���O�̃N���A
				state->next_cap.capsel_flg[cap_disp_flg] = cap_flg_off;
				cap->capsel_flg[cap_down_flg] = cap_flg_off;
				break;
			}
		}
	}

	// �����ł��Ȃ�
	if(!cap->capsel_flg[cap_down_flg]) {
		dm_snd_play_in_game(SE_gTouchDown); // ���n�r�d�Đ�
		state->mode_now = dm_mode_down_wait; // �o�E���h
		cap->capsel_flg[cap_disp_flg] = cap_flg_off; // �\���t���O�̃N���A

#if 0 && defined(DEBUG)
	if(PLAYER_IS_CPU(state, state->player_no) &&
		(cap->pos_x[0] != state->ai.aiEX-1 || cap->pos_y[0] != state->ai.aiEY))
	{
		PRTFL();
		osSyncPrintf("�ړI�n�ɗ����ł��Ȃ�����(player(%d), cap(%d,%d), ai(%d,%d)\n",
			state->player_no, cap->pos_x[0], cap->pos_y[0], state->ai.aiEX-1, state->ai.aiEY);
	}
#endif

		for(i = 0;i < 2;i++) {
			if(cap->pos_y[i] != 0) {
				// �}�b�v�f�[�^�ɕύX
				set_map(map,cap->pos_x[i], cap->pos_y[i], cap->capsel_g[i],
					cap->capsel_p[i] + black_color[state->flg_game_over]);
			}
		}
	}
}

//## �U���ʒu���v�Z����֐�
static int dm_make_attack_pattern(unsigned int max)
{
	Game *st = watchGame;
	int pattern = 0;

	// �S�̏ꍇ
	if(max >= 4) {
		pattern = 0x55 << random(2);
	}
	// �R�̏ꍇ
	else if(max >= 3) {
		pattern = 0x15 << random(4);
	}
	// �Q�̏ꍇ
	else if(max >= 2) {
		pattern = 0x11 << random(4);
	}

	return pattern;
}

//## �J�v�Z���𗎂Ƃ�����
static int dm_set_attack_2p(game_state *state)
{
	Game *st = watchGame;
	game_state *enemy;
	int i, x, chainCount, pattern;

	if(state->chain_line < 2) {
		return FALSE;
	}

	// �G�̃X�e�[�^�X���擾
	enemy = &game_state_data[state->player_no ^ 1];

	// �A�������擾
	chainCount = MIN(GAME_MAP_W / 2, state->chain_line);

	for(i = 0; i < 1; i++) { // <= 2p�̏ꍇ��,�X�g�b�N�o�b�t�@��1����
		// �����҂��J�v�Z������������A���̌��Ԃ�T��
		if(enemy->cap_attack_work[i][0] != 0) {
			int flag;

			pattern = 0;

			for(x = 0; x < GAME_MAP_W; x++) {
				if(enemy->cap_attack_work[i][0] & (3 << (x << 1))) {
					pattern |=  1 << x;
					flag = x & 1;
				}
			}

			for(x = 0; x < GAME_MAP_W; x++) {
				if((x & 1) == flag) {
					pattern ^= 1 << x;
				}
			}

			if(pattern == 0) {
				continue;
			}
		}
		// �J�v�Z���̗��Ƃ��ꏊ���擾
		else {
			pattern = dm_make_attack_pattern(chainCount);
		}

		enemy->cap_attack_work[i][1] = state->player_no;

		for(x = 0; x < GAME_MAP_W; x++) {
			int c;

			// ���Ƃ��ꏊ�łȂ���Β��f
			if((pattern & (1 << x)) == 0) {
				continue;
			}

			// ���Ƃ��ĂȂ��J�v�Z����T��
			while(state->chain_color[0] ||
				state->chain_color[1] || state->chain_color[2])
			{
				c = random(3);

				if(state->chain_color[c]) {
					// �J�v�Z���𗎂Ƃ�
					state->chain_color[c]--;
					enemy->cap_attack_work[i][0] |= (c + 1) << (x << 1);
					break;
				}
			}
		}

		break;
	}

	return TRUE;
}

//## �U���ݒ� (4P�p)
static int dm_set_attack_4p(game_state *state)
{
	static const char attack_table[][3] = {
		{ 3, 2, 1 }, { 0, 3, 2 }, { 1, 0, 3 }, { 2, 1, 0 },
	};
	Game *st = watchGame;
	game_state *enemy;
	int c, i, j, chainCount, work[3];
	int attackWork[3];
	int attackFlag, stockFlag;

	if(state->chain_line < 2) {
		return FALSE;
	}

	for(i = 0; i < ARRAY_SIZE(attackWork); i++) {
		attackWork[i] = 0;
	}
	attackFlag = stockFlag = 0;
	chainCount = 0;

	// �U����A�U���J�v�Z���F�A�����߂�
	for(i = 0; i < 3; i++) {

		// �A�����A�ŏ��ɏ������F�ɂ���čU�������I��
		if(!(state->chain_color[3] & (1 << i))) {
			continue;
		}

		// �G�̃X�e�[�^�X���擾
		enemy = &game_state_data[attack_table[state->player_no][i]];

		// �����`�[���̏ꍇ
		if(enemy->team_no == state->team_no) {
			// ���^�C�A���Ă����璆�f
			if(enemy->flg_retire) {
				continue;
			}
			stockFlag |= (1 << attack_table[state->player_no][i]);
		}
		else {
			// ���^�C�A���� && (!�g���[�j���O�� || !�U������g���[�j���O)
			if(enemy->flg_retire &&
				(enemy->cnd_training != dm_cnd_training || !enemy->flg_training))
			{
				continue;
			}
			attackFlag |= (1 << attack_table[state->player_no][i]);
		}

		// ���łɍU���J�v�Z���F�����߂��ꍇ�͒��f
		if(chainCount > 0) {
			continue;
		}

		// �A�������擾
		chainCount = MIN(GAME_MAP_W / 2, state->chain_line);

		// �X�g�b�N���g�p����
		for(j = 0; j < ARRAY_SIZE(st->story_4p_stock_cap[state->team_no]); j++) {

			// �X�g�b�N���g�p�ł��邩?
			if(chainCount >= GAME_MAP_W / 2) {
				break;
			}

			// �X�g�b�N���������ꍇ�g�p
			if(st->story_4p_stock_cap[state->team_no][j] == -1) {
				continue;
			}

			// �X�g�b�N�F�̒ǉ�
			state->chain_color[ st->story_4p_stock_cap[state->team_no][j] ]++;

			// �X�g�b�N�F�̏���
			st->story_4p_stock_cap[state->team_no][j] = -1;

			chainCount++;
		}

		// �U���J�v�Z���F������
		j = 0;
		while(j < chainCount &&
			state->chain_color[0] + state->chain_color[1] + state->chain_color[2] > 0)
		{
			c = random(3);

			if(state->chain_color[c] > 0) {
				state->chain_color[c]--;
				attackWork[c]++;
				j++;
			}
		}
	}

	// �^�[�Q�b�g�����ׂă��^�C�A���Ă���
	if(attackFlag + stockFlag == 0) {
		return FALSE;
	}

	// �U������
	for(i = 0; i < 4; i++) {
		int x, pattern;

		// �U������łȂ���Β��f
		if((attackFlag & (1 << i)) == 0) {
			continue;
		}

		// �G�̃X�e�[�^�X���擾
		enemy = &game_state_data[i];

		// �U���G�t�F�N�g
		add_attack_effect(state->player_no,
			_posP4CharBase[state->player_no][0], _posP4CharBase[state->player_no][1],
			_posP4CharBase[enemy->player_no][0], _posP4CharBase[enemy->player_no][1]);

		// �A���J�v�Z������ۑ�
		for(j = 0; j < ARRAY_SIZE(work); j++) {
			work[j] = attackWork[j];
		}

		for(j = 0; j < DAMAGE_MAX; j++) {

			// �󂫂�������Β��f
			if(enemy->cap_attack_work[j][0] != 0x0000) {
				continue;
			}
			// �J�v�Z���̗��Ƃ��ꏊ���擾
			else {
				pattern = dm_make_attack_pattern(chainCount);
			}

			// �U�������v���C���[�ԍ�
			enemy->cap_attack_work[j][1] = state->player_no;

			for(x = 0; x < GAME_MAP_W; x++) {
				if((pattern & (1 << x)) == 0) {
					continue;
				}

				// ���W����
				while(work[0] + work[1] + work[2] > 0) {
					c = random(3);

					if(work[c] > 0) {
						work[c]--;
						enemy->cap_attack_work[j][0] |= (c + 1) << (x << 1);
						break;
					}
				}
			}
		}
	}

	// �X�g�b�N�̃V�t�g
	for(i = j = 0; i < 4; i++) {
		if(st->story_4p_stock_cap[state->team_no][i] == -1) continue;
		st->story_4p_stock_cap[state->team_no][j] = st->story_4p_stock_cap[state->team_no][i];
		j++;
	}
	for(; j < 4; j++) {
		st->story_4p_stock_cap[state->team_no][j] = -1;
	}

	// �X�g�b�N�v�Z
	for(i = 0; i < 4; i++) {

		// �X�g�b�N����łȂ���Β��f
		if((stockFlag & (1 << i)) == 0) {
			continue;
		}

		// �����̃X�e�[�^�X���擾
		enemy = &game_state_data[attack_table[state->player_no][i]];

		// �A���J�v�Z������ۑ�
		for(j = 0; j < ARRAY_SIZE(work); j++) {
			work[j] = attackWork[j];
		}

		for(j = c = 0; j < 4; j++) {
			// �����X�g�b�N���݂����璆�f
			if(st->story_4p_stock_cap[state->team_no][j] != -1) {
				continue;
			}

			for(; c < 3; c++) {
				if(work[c] > 0) {
					work[c]--;
					st->story_4p_stock_cap[state->team_no][j] = c;
					break;
				}
			}
		}
	}

	return TRUE;
}

//## �v���C���[���̃J�v�Z�������ʒuON/OFF ���Q�[���̃��[�N�ɔ��f
static void load_visible_fall_point_flag()
{
	int i, j;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		j = 1;
		break;

	case GSL_2PLAY:
		j = 2;
		break;

	default:
		j = 0;
		break;
	}

	for(i = 0; i < j; i++) {
		mem_char *mc = &evs_mem_data[ evs_select_name_no[i] ];
		visible_fall_point[i] = (mc->mem_use_flg & DM_MEM_CAP) ? 1 : 0;
	}
}

//## �����ʒuON/OFF ���Z�[�u�f�[�^�ɔ��f
static void save_visible_fall_point_flag()
{
	int i, j;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		j = 1;
		break;

	case GSL_2PLAY:
		j = 2;
		break;

	default:
		j = 0;
		break;
	}

	for(i = 0; i < j; i++) {
		mem_char *mc = &evs_mem_data[ evs_select_name_no[i] ];
		mc->mem_use_flg &= ~DM_MEM_CAP;
		if(visible_fall_point[i]) mc->mem_use_flg |=  DM_MEM_CAP;
	}
}

//## ���g���C���j���[: �J�[�\���ʒu�Ȃǂ�������
static void retryMenu_init(int playerNo, int type)
{
	Game *st = watchGame;

	st->retry_type[playerNo] = type;
	st->retry_select[playerNo] = 0;
	st->retry_result[playerNo] = -1;

	switch(st->retry_type[playerNo]) {
	case CONTINUE_REP_END:
	case CONTINUE_REP_TRY_END:
	case CONTINUE_REP_NEX:
	case CONTINUE_REP_NEX_END:
		st->retry_select[playerNo]++;
		break;
	}
}

//## ���g���C���j���[: �|�[�Y���S��������
static void retryMenu_initPauseLogo(int playerNo)
{
	retryMenu_init(playerNo, PAUSE_NEX_END);
}

//## ���g���C���j���[: ���͏���
static int retryMenu_input(int playerNo)
{
	Game *st = watchGame;
	int rep = _getKeyRep(playerNo);
	int trg = _getKeyTrg(playerNo);
	int select, vy = 0;
	int sound = -1, result = -1;

	select = st->retry_select[playerNo] =
		WrapI(0, _retryMenu_itemCount[st->retry_type[playerNo]], st->retry_select[playerNo]);

	if(rep & DM_KEY_UP) vy--;
	if(rep & DM_KEY_DOWN) vy++;
	select =
		WrapI(0, _retryMenu_itemCount[st->retry_type[playerNo]], select + vy);

	if(select != st->retry_select[playerNo]) {
		st->retry_select[playerNo] = select;
		sound = SE_mUpDown;
	}
	else if(trg & DM_KEY_OK) {
		result = st->retry_result[playerNo];
	}

	if(sound >= 0) {
		dm_snd_play_in_game(sound);
	}

	return result;
}

//## ���g���C���j���[: �`�揈��
static void retryMenu_drawPause(int playerNo, Gfx **gpp, bool dispMenu)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	switch(st->retry_type[playerNo]) {
	case PAUSE_NEX_END:
	case PAUSE_NEX_TRY_END:
		st->retry_result[playerNo] = disp_pause_logo(&gp, playerNo, 0,
			dispMenu ? st->retry_select[playerNo] : -1, st->retry_type[playerNo]);
		break;
	}

	*gpp = gp;
}

//## ���g���C���j���[: �`�揈��
static void retryMenu_drawContinue(int playerNo, Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	switch(st->retry_type[playerNo]) {
	case CONTINUE_REP_END:
	case CONTINUE_REP_TRY_END:
	case CONTINUE_REP_NEX:
	case CONTINUE_REP_NEX_END:
		if(evs_gamesel == GSL_2PLAY && evs_gamemode == GMD_TIME_ATTACK) {
			st->retry_result[playerNo] = disp_continue_logo_score(&gp, playerNo,
				st->retry_select[playerNo], st->retry_type[playerNo] - 2);
		}
		else {
			st->retry_result[playerNo] = disp_continue_logo(&gp, playerNo,
				st->retry_select[playerNo], st->retry_type[playerNo] - 2);
		}
		break;
	}

	*gpp = gp;
}

//## �r�̒��g����i�グ�鎞�A�V���ɗN���o��E�B���X��ݒ�
static void set_bottom_up_virus(game_state *state, game_map *map)
{
	int i, j, x, endI, color;
	u8	work[GAME_MAP_W];

	for( x = 0; x < GAME_MAP_W; x++ ) work[x] = 0;

	endI = GAME_MAP_W * GAME_MAP_H;
	i = endI - GAME_MAP_W;
	x = 0;

	// ��i��̗�̃J�v�Z���̉��ɂ́A�K���E�C���X��z�u
	for(; i < endI; i++, x++) {
		if(!map[i].capsel_m_flg[cap_disp_flg]) {
			// ��\��������
			continue;
		}
		if(map[i].capsel_m_flg[cap_virus_flg] >= 0) {
			// �E�B���X������
			continue;
		}
		// �����ɃE�C���X��u��
		work[x] = 1;
	}

	// 75%�̊m���Ŕz�u
	j = 0;
	for( x = 0; x < GAME_MAP_W; x++ ) {
		if ( random(100) < 75 ) work[x] = 1;
		if ( work[x] != 0 ) j++;
	}
	// �����P���z�u����Ȃ�������A�����I�ɂP��`
	if ( j == 0 ) work[random(GAME_MAP_W)] = 1;

	endI += GAME_MAP_W;
	x = 0;

	// �E�C���X��z�u����
	for(; i < endI; i++, x++) {
		int mask[3], flag[3], pos[3];

		// �u���Ȃ�
		if( work[x] == 0 ) continue;

		mask[0] = mask[1] = mask[2] = 0;
		flag[0] = 1;
		flag[1] = (x > 1);
//		flag[2] = (x < GAME_MAP_W - 2);
		pos[0] = i - GAME_MAP_W*2;
		pos[1] = i - 2;
//		pos[2] = i + 2;

		// ���͂̐F
		for(j = 0; j < 2; j++) {
			if(!flag[j]) {
				// �g�O
				continue;
			}
			if(map[ pos[j] ].capsel_m_flg[cap_disp_flg]) {
				// �F���L�^
				mask[ map[ pos[j] ].capsel_m_flg[cap_col_flg] ]++;
			}
		}

		// �O�F�Ƃ��g���Ă���
		if( mask[0] && mask[1] && mask[2] ) continue;

		// �F������
		do {
			color = random(3);
		} while ( mask[color] );

		// �E�B���X��ݒ�
		set_virus(map, x, GAME_MAP_H + 1, color, virus_anime_table[color][state->virus_anime]);
	}
}

//## �r�̒��g����i�グ�� & �V����т���������
static bool bottom_up_bottle_items(game_map *map)
{
	int i, j, size;
	bool flow = false;

	for(i = 0; i < GAME_MAP_W; i++) {
		if(map[i].capsel_m_flg[cap_disp_flg]) {
			// �V����т���
			flow = true;
		}
	}
	size = GAME_MAP_W * GAME_MAP_H;

	for(i = 0, j = GAME_MAP_W; i < size; i++, j++) {
		map[i] = map[j];
		map[i].pos_m_y--;
	}

	size += GAME_MAP_W;

	for(j = 0; i < size; i++, j++) {
		bzero(&map[i], sizeof(game_map));
		map[i].pos_m_x = j;
		map[i].pos_m_y = GAME_MAP_H + 1;
	}

	return flow;
}

//## �ϋv���[�h�́A�A�����{�[�i�X�E�F�C�g�����Z
static void add_taiQ_bonus_wait(game_state *state)
{
	static const int _bonus[] = {
		 30, // �E�B���X�A���������C��
		120, // 2�A��
		150, // 3�A��
		180, // 4�A��
		210, // 5�A��
		240, // 6�A���ȏ�
	};
	Game *st = watchGame;
	int i, count;

	if(evs_gamemode != GMD_TaiQ || state->chain_line < 2) {
		return;
	}

	// �{�[�i�X�E�F�C�g�����Z
	st->big_virus_wait += 0.25 * (state->chain_line - 1);
	st->big_virus_wait = MIN(st->big_virus_wait, _big_virus_max_wait[state->game_level]);

	count = 0;
	count += _bonus[0] * (state->erase_virus_count + state->chain_line);
	for(i = 1; i < state->chain_count; i++) {
		count += _bonus[ MIN(i, ARRAY_SIZE(_bonus) - 1) ];
	}

PRT1("��~���� += %d\n", count);

	// �E�B���X�̉�]��~���Ԃ�ݒ�
	st->big_virus_stop_count += count;
}

//## �������t����������
static bool dm_check_one_game_finish()
{
	int i;

	for(i = 0; i < evs_playcnt; i++) {
		DM_GAME_CONDITION cnd = game_state_data[i].cnd_static;
		if(cnd != dm_cnd_win && cnd != dm_cnd_lose && cnd != dm_cnd_draw) {
			break;
		}
	}

	return i == evs_playcnt;
}

//////////////////////////////////////////////////////////////////////////////
//{### �Z�[�u

//## �Q�[���p EEP �������݊֐��R�[���o�b�N
static void dm_game_eep_write_callback(void *args)
{
	Game *st = (Game *)args;

	// EEPROM�������ݒ��̌x�����b�Z�[�W
	RecWritingMsg_setStr(&st->writingMsg, _mesWriting);
	RecWritingMsg_setPos(&st->writingMsg,
		(SCREEN_WD - msgWnd_getWidth(&st->writingMsg.msgWnd)) / 2,
		(SCREEN_HT - msgWnd_getHeight(&st->writingMsg.msgWnd)) - 32);
	RecWritingMsg_start(&st->writingMsg);

	// �X���[�v�^�C�}�[
	setSleepTimer(500);
}

//## �Q�[���p EEP �������݊֐�
static void dm_game_eep_write(void *arg)
{
	Game *st = watchGame;
	EepRomErr err;

	if(st->eep_rom_flg) {
		err = EepRom_WriteAll(dm_game_eep_write_callback, st);
		st->eep_rom_flg = 0;

		PRTFL();
		EepRom_DumpErrMes(err);
	}
}

//## �X�g�[���[���[�h�ł̏������A���̖ʂɐi�ނ��ǂ����𔻕�
static bool dm_query_next_stage(int level, int stage, int retry)
{
	bool next = false;

	if(stage < LAST_STAGE_NO) {
		next = true;
	}
	else if(stage == LAST_STAGE_NO) {
		if(level > 2 || (level > 0 && retry == 0)) {
			next = true;
		}
	}

	return next;
}

//## �Z�[�u�Q�I�[��
static void dm_save_all()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	int i;

	if(st->replayFlag || st->eep_rom_flg) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_4PLAY: {
		int stage;

		// �X�g�[���[���[�h
		if(!evs_story_flg) break;

		stage = evs_story_no;
		if(state->cnd_static == dm_cnd_win) {
			stage++;
		}

		// �Z�[�u�f�[�^�Z�b�g
		dm_story_sort_set(evs_select_name_no[0],
			(story_proc_no >= STORY_W_OPEN ? 1 : 0),
			evs_story_level, state->game_score,
			evs_game_time, stage, evs_one_game_flg);
		st->eep_rom_flg = 1;
		} break;

	case GSL_2PLAY:
		// �L�^�̕ۑ�
		for(i = 0; i < 2; i++) {
			switch(evs_gamemode) {
			case GMD_NORMAL:
				dm_vsman_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			case GMD_FLASH:
				dm_vm_fl_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			case GMD_TIME_ATTACK:
				dm_vm_ta_set(evs_select_name_no[i], st->vs_win_count[i], st->vs_win_count[i ^ 1]);
				break;
			}
		}
		st->eep_rom_flg = 1;
		st->vs_win_count[0] = st->vs_win_count[1] = 0;
		break;

	case GSL_VSCPU:
		if(!evs_story_flg) {
			// �L�^�̕ۑ�
			switch(evs_gamemode) {
			case GMD_NORMAL:
				dm_vscom_set(evs_select_name_no[0], st->vs_win_count[0], st->vs_win_count[1]);
				break;
			case GMD_FLASH:
				dm_vc_fl_set(evs_select_name_no[0], st->vs_win_count[0], st->vs_win_count[1]);
				break;
			}
			st->eep_rom_flg = 1;
			st->vs_win_count[0] = st->vs_win_count[1] = 0;
		}
		// �X�g�[���[���[�h
		else {
			mem_char *mc = &evs_mem_data[evs_select_name_no[0]];
			game_config *cfg = &mc->config;
			int charNo = (story_proc_no < STORY_W_OPEN ? 0 : 1);
			int stage = evs_story_no;

			// ���B�X�e�[�W�����A���j���[�̃J�[�\���ʒu�ɔ��f
			cfg->st_st = CLAMP(0, LAST_STAGE_NO - 1, stage - 1);

			if(state->cnd_static == dm_cnd_win) {
				if(stage == EX_STAGE_NO && state->game_retry == 0) {
					evs_secret_flg[charNo] = 1;
				}

				if(dm_query_next_stage(evs_story_level, stage, state->game_retry)) {
					stage++;
				}
				else if(stage == EX_STAGE_NO) {
					// �L�^�ł`�k�k��\�����邽�߂̃C���N�������g
					stage++;
				}
			}

			// �Z�[�u�f�[�^�Z�b�g
			dm_story_sort_set(evs_select_name_no[0], charNo,
				evs_story_level, state->game_score,
				evs_game_time, stage, evs_one_game_flg);
			st->eep_rom_flg = 1;
		}
		break;

	case GSL_1PLAY:
		// �Z�[�u�f�[�^�Z�b�g
		state = &game_state_data[0];
		switch(evs_gamemode) {
		case GMD_NORMAL: {
			int level = state->virus_level;

			if(state->cnd_static == dm_cnd_win) {
				if(level >= 21) {
					evs_level_21 = 1;
				}
				if(level < VIRUS_LEVEL_MAX) {
					level++;
				}
			}
			dm_level_sort_set(evs_select_name_no[0],
				state->cap_def_speed, state->game_score, level);

			// ���B�������x�������j���[�̃J�[�\���ʒu�ɔ��f������
			evs_mem_data[evs_select_name_no[0]].config.p1_lv = MIN(21, level);
			} break;

		case GMD_TaiQ:
			dm_taiQ_sort_set(evs_select_name_no[0],
				state->game_level,
				state->game_score,
				evs_game_time);
			break;

		case GMD_TIME_ATTACK:
			dm_timeAt_sort_set(evs_select_name_no[0],
				state->game_level,
				state->game_score,
				evs_game_time,
				state->total_erase_count);
			break;
		}
		st->eep_rom_flg = 1;
		break;
	}

	// �Z�[�u
	cpuTask_sendTask(dm_game_eep_write, 0);
}

//////////////////////////////////////////////////////////////////////////////
//{### �Q�[���i�s

#ifdef _ENABLE_CONTROL
//## �f�o�b�O���쏈��
static void dm_query_debug_player(game_state *state)
{
	Game *st = watchGame;
	int trg = _getKeyTrg(state->player_no);
	int i;

	if(st->query_debug_player_no >= 0 || st->query_config_player_no >= 0) {
		return;
	}

	// �f�o�b�N�p����
	if((trg & CONT_DEBUG) == CONT_DEBUG) {
		st->query_debug_player_no = state->player_no;
	}
	else if((trg & CONT_CONFIG) == CONT_CONFIG) {
		st->query_config_player_no = state->player_no;
	}
}
#endif // _ENABLE_CONTROL

//## �|�[�Y���쏈��
static void dm_query_pause_player(game_state *state)
{
	Game *st = watchGame;
	int trg = _getKeyTrg(state->player_no);
	int i;

	if(st->query_pause_player_no >= 0) {
		return;
	}

	switch(state->cnd_static) {
	case dm_cnd_wait:
	case dm_cnd_pause:
		break;

	case dm_cnd_lose:
		if(!st->replayFlag || state->cnd_now != dm_cnd_tr_chack) {
			return;
		}
		break;

	default:
		return;
	}

	if(state->cnd_now == dm_cnd_init) return;
	if(state->player_type != PUF_PlayerMAN) return;

	if(!st->replayFlag) {
		if(trg & DM_KEY_START) {
			st->query_play_pause_se = 1;
			st->query_pause_player_no = state->player_no;
		}
	}
	else {
		if(trg & DM_ANY_KEY) {
			st->query_pause_player_no = state->player_no;
		}
	}
}

//## �Q�[�����쏈��(�P�o�p)
static DM_GAME_RET dm_game_main_cnt_1P(game_state *state, game_map *map, int player_no)
{
	Game *st = watchGame;
	int i, j;

	// �|�[�Y�v��
	dm_query_pause_player(state);

#ifdef _ENABLE_CONTROL
	// �f�o�b�O����v��
	dm_query_debug_player(state);
#endif // _ENABLE_CONTROL

	// �L�����A�j���[�V�������X�V
	animeState_update(&state->anime);

	// �X�R�A�G�t�F�N�g���X�V
	scoreNums_update(&st->scoreNums[player_no]);

	// �^�C���A�^�b�N�̏ꍇ�A�������Ԃ��߂��Ă��邩�`�F�b�N
	if(evs_gamemode == GMD_TIME_ATTACK &&
		evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
	{
		if(state->cnd_static == dm_cnd_wait) {
			// �������Ԃ��߂��Ă����̂ŃQ�[���I�[�o�[
			return dm_ret_game_over;
		}
	}

#ifdef _ENABLE_CONTROL
	// ���̃X�e�[�W��
	if(_getKeyTrg(0) & DM_KEY_Z) {
		if(state->cnd_static == dm_cnd_wait) {
			return dm_ret_clear;
		}
	}

//	if(_getKeyTrg(0) & DM_KEY_L) {
//		// �R�C���ǉ�
//		if(st->retry_coin < COINMAX) {
//			st->retry_coin++;
//		}
//	}
#endif // _ENABLE_CONTROL

	// ������������
	switch(state->mode_now) {
	// �E�C���X�z�u
	case dm_mode_init:
		dm_set_virus(state, map, virus_map_data[player_no], virus_map_disp_order[player_no]);
		return dm_ret_virus_wait;

	// �����̑҂�
	case dm_mode_wait:
		return dm_ret_virus_wait;

	// ������
	case dm_mode_throw:
		state->cap_speed_count++;

		// �J�v�Z�����r�����ɓ�������
		if(state->cap_speed_count == FlyingCnt[state->cap_def_speed]) {
			dm_init_capsel_go(&state->now_cap,
				(CapsMagazine[state->cap_magazine_save] >> 4) % 3,
				CapsMagazine[state->cap_magazine_save] % 3);

			// �J�v�Z�������J�n
			state->mode_now = dm_mode_down;
			state->cap_speed_count = 30;
			dm_capsel_down(state,map);
		}
		break;

	// ���n�E�F�C�g
	case dm_mode_down_wait:
		// �Q�[���I�[�o�[�̏ꍇ
		if(dm_check_game_over(state,map)) {
			for(i = 0; i < 3; i++) {
				// ���ł��Ă��Ȃ������狐��E�C���X�΂��A�j���[�V�����Z�b�g
				if(st->virus_anime_state[i].animeSeq->animeNo != ASEQ_LOSE) {
					animeState_set(&st->virus_anime_state[i], ASEQ_WIN);
				}
			}
			// ����E�B���X��~�J�E���g���N���A
			if(evs_gamemode == GMD_TaiQ) {
				st->big_virus_stop_count = 0;
			}
			// �ς݂�����(�Q�[���I�[�o�[)
			return dm_ret_game_over;
		}
		else {
			// ���ł��������ꍇ
			if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
				// ���ŊJ�n
				state->mode_now = dm_mode_erase_chack;
				state->cap_speed_count = 0;
			}
			else {
				// �J�v�Z���Z�b�g���w�肵�Ă���
				state->mode_now = dm_mode_cap_set;
			}
		}
		break;

	// ���Ŕ���
	case dm_mode_erase_chack:
		state->cap_speed_count++;

		// ���n�E�F�C�g�������
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}
		state->cap_speed_count = 0;

		// ���ŊJ�n
		state->mode_now = dm_mode_erase_anime;

		// �c�������� // ����������
		dm_h_erase_chack_set(state,map);
		dm_w_erase_chack_set(state,map);

		// �c�������� // ����������
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// �e�F�̃E�C���X���̎擾
		i = state->virus_number;
		i -= get_virus_color_count(map, &st->big_virus_count[0],
			&st->big_virus_count[1] ,&st->big_virus_count[2]);
		state->virus_number -= i;

		// �E�B���X���������擾
		state->total_erase_count += i;

		for(i = 0; i < 3; i++) {
			// �e�F�̃E�C���X���S�ł����ꍇ
			if(st->big_virus_count[i] == 0) {
				// �܂����Ńt���O�������Ă��Ȃ��ꍇ
				if(!st->big_virus_flg[i]) {
					// ���Ńt���O�������Ă�
					st->big_virus_flg[i] = 1;

					// ����E�C���X���ŃA�j���[�V�����Z�b�g
					animeState_set(&st->virus_anime_state[i], ASEQ_LOSE);
					animeSmog_start(&st->virus_smog_state[i]);

					if(state->virus_number != 0) {
						// ����E�C���X�_���[�W
						dm_snd_play_in_game(SE_gVirusStruggle);

						// ����E�C���X���� SE �Z�b�g
						dm_snd_play_in_game(SE_gVrsErsAl);
					}
				}
			}
			else if(state->chain_color[3] & (0x10 << i)) {
				if(st->big_virus_stop_count == 0) {
					// ����E�C���X����A�j���[�V�����Z�b�g
					animeState_set(&st->virus_anime_state[i], ASEQ_DAMAGE);

					// ����E�C���X�_���[�W
					dm_snd_play_in_game(SE_gVirusStruggle);
				}
			}
		}

		// ���ŐF�̃��Z�b�g
		state->chain_color[3] &= 0x0f;

		// ���_�v�Z & �X�R�A�G�t�F�N�g
		{
			int pos[2];
			dm_calc_erase_score_pos(state, map, pos);
			scoreNums_set(&st->scoreNums[player_no],
				dm_make_score(state), state->erase_virus_count,
				state->map_x + state->map_item_size / 2 + pos[0],
				state->map_y + state->map_item_size / 2 + pos[1]);
		}

		// �E�C���X�S��
		if(state->virus_number == 0 && evs_gamemode != GMD_TaiQ) {
			// �X�e�[�W�N���A
			state->cnd_now = dm_cnd_stage_clear;
			state->mode_now = dm_mode_stage_clear;

			// �ő���ŗ񐔂̕ۊ�
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			return dm_ret_clear;
		}
		// �E�C���X���c��R�ȉ��̂Ƃ��x������炷
		else if(state->virus_number <= 3 && evs_gamemode != GMD_TaiQ) {
			// �P��Ȃ炵����Ȍ�炳�Ȃ�
			if(!st->last_3_se_flg) {
				st->last_3_se_flg = 1;

				// �E�C���X�c��R�r�d�Đ�
				dm_snd_play_in_game(SE_gReach);
			}

			// ���y���x�A�b�v�̃t���O�𗧂Ă�
			if(st->bgm_bpm_flg[0] == 0) {
				st->bgm_bpm_flg[0] = 1; // �Q�񏈗����Ȃ����߂̃t���O
				st->bgm_bpm_flg[1] = 1; // �������s���t���O�𗧂Ă�
			}
		}

		state->chain_count++;

		if(state->chain_line < 2) {
			if(state->chain_color[3] & 0x08) {
				// ���ŐF�̃��Z�b�g
				state->chain_color[3] &= 0xF7;

				// �E�C���X���܂ޏ��łr�d�Đ�
				dm_snd_play_in_game(SE_gVrsErase);
			}
			else {
				// �J�v�Z���݂̂̏��łr�d�Đ�
				dm_snd_play_in_game(SE_gCapErase);
			}
		}
		else {
			if(state->chain_color[3] & 0x08) {
				// ���ŐF�̃��Z�b�g
				state->chain_color[3] &= 0xF7;
			}
			i = MIN(2, state->chain_line - 2);
			dm_snd_play_in_game(SE_getAttack(state->charNo, i));
		}
		break;

	// ���ŃA�j���[�V����
	case dm_mode_erase_anime:
		dm_capsel_erase_anime(state,map);
		break;

	// �J�v�Z��(��)����
	case dm_mode_ball_down:
		go_down(state,map,dm_down_speed);
		break;

	// �V�J�v�Z���Z�b�g
	case dm_mode_cap_set:

		// �ϋv���[�h�́A�A�����{�[�i�X�E�F�C�g�����Z
		add_taiQ_bonus_wait(state);

		// �ϋv���[�h�̏���
		if(st->bottom_up_flag && st->big_virus_stop_count == 0) {
			state->bottom_up_scroll = 0;
			state->mode_now = dm_mode_bottom_up;
			set_bottom_up_virus(state, map);
			dm_snd_play_in_game(SE_gBottomUp);
			break;
		}

		// ���v�A���񐔂��X�V
		if(state->chain_count > 1) {
			state->total_chain_count = MIN(99,
				state->total_chain_count + state->chain_count - 1);
		}

		// �ς݂�����`�F�b�N
		dm_warning_h_line(state, map);

		// �V�J�v�Z���Z�b�g
		dm_set_capsel(state);

		// �����J�v�Z�����x�̌v�Z
		dm_capsel_speed_up(state);

		// �U���r�d�Đ�
		dm_attack_se(state, player_no);

		// mario throw
		animeState_set(&state->anime, ASEQ_ATTACK);

		// �����O�v�l�ǉ��ʒu
		if(PLAYER_IS_CPU(state, player_no)) {
			aifMakeFlagSet(state);
		}

		// �ő���ŗ񐔂̕ۊ�
		if(state->chain_line_max < state->chain_line) {
			state->chain_line_max = state->chain_line;
		}

		// �J�v�Z������������
		state->mode_now = dm_mode_throw;
//		state->mode_now = dm_mode_down;

		// �J�E���g�̃��Z�b�g // �������x�̃��Z�b�g
		state->cap_speed_count = 0;
		state->cap_speed_max = 0;

		// ���Ń��C�����̃��Z�b�g // �A�����̃��Z�b�g
		state->chain_line = 0;
		state->chain_count = 0;

		// ���ŃE�C���X���̃��Z�b�g
		state->erase_virus_count = 0;
		state->erase_virus_count_old = 0;

		// ���ŐF�̃��Z�b�g
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}
		break;

	// �r�̒��g����i�グ��
	case dm_mode_bottom_up:
		// �X�N���[���l���X�V
		state->bottom_up_scroll++;
		if(state->bottom_up_scroll < state->map_item_size) {
			break;
		}
		state->bottom_up_scroll = 0;

		st->bottom_up_flag = false;
		st->big_virus_wait = MAX(_big_virus_min_wait[state->game_level], st->big_virus_wait - 0.5);

		// �n�[�t�J�v�Z����������
		state->mode_now = dm_mode_ball_down;

		if(bottom_up_bottle_items(map)) {
			// �V����т����̂ŁA�Q�[���I�[�o�[
			return dm_ret_game_over;
		}

		// �e�F�̃E�C���X���̎擾
		state->virus_number = get_virus_color_count(map,
			&st->big_virus_count[0], &st->big_virus_count[1] ,&st->big_virus_count[2]);

		// ���ł��Ă����F�̃E�B���X�����������ꍇ�A���Ƌ��ɓo�ꂳ����B
		for(i = 0; i < 3; i++) {
			if(st->big_virus_count[i] != 0 && st->big_virus_flg[i]) {
				// ����E�C���X�A�j���[�V�����Z�b�g
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
				animeSmog_start(&st->virus_smog_state[i]);
				st->big_virus_flg[i] = 0;
			}
			else if(st->virus_anime_state[i].animeSeq->animeNo == ASEQ_WIN) {
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
			}
		}
		break;

//	// �R�C���l��
//	case dm_mode_get_coin:
//		state->mode_now = dm_mode_stage_clear;
//		break;

	case dm_mode_clear_wait:
	case dm_mode_gover_wait:
		if(state->mode_now == dm_mode_clear_wait) {
			if(effectNextStage_calc(player_no)) break;
		}
		else {
			if(effectGameOver_calc(player_no)) break;
		}

		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			switch(state->mode_now) {
			case dm_mode_clear_wait:
				state->cnd_now = dm_cnd_clear_result;
				state->mode_now = dm_mode_clear_result;
				break;
			case dm_mode_gover_wait:
				state->cnd_now = dm_cnd_gover_result;
				state->mode_now = dm_mode_gover_result;
				break;
			}
		}
		break;

	case dm_mode_clear_result:
	case dm_mode_gover_result:
		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			timeAttackResult_skip(&st->timeAttackResult[player_no]);
		}

		timeAttackResult_update(&st->timeAttackResult[player_no], true);
		dm_add_score(state, st->timeAttackResult[player_no].scoreDelta);
		st->timeAttackResult[player_no].scoreDelta = 0;

		if(timeAttackResult_isEnd(&st->timeAttackResult[player_no])) {
			switch(state->mode_now) {
			case dm_mode_clear_result:
				state->cnd_now = dm_cnd_stage_clear;
				state->mode_now = dm_mode_stage_clear;
				break;
			case dm_mode_gover_result:
				state->cnd_now = dm_cnd_game_over;
				state->mode_now = dm_mode_game_over;
				break;
			}
		}
		break;

	// �X�e�[�W�N���A���o
	// �Q�[���I�[�o�[���o
	case dm_mode_stage_clear:
	case dm_mode_game_over:
		if(state->mode_now == dm_mode_stage_clear) {
			if(effectNextStage_calc(player_no)) break;
		}
		else {
			if(effectGameOver_calc(player_no)) break;
		}

		// �p�X���[�h��\��
		i = DM_KEY_L | DM_KEY_R | DM_KEY_LEFT | DM_KEY_Z;
		if(st->passAlphaStep < 0 && (_getKeyLvl(0) & i) == i) {
			static char _guest[] = { 0, 0, 0, 0 };
			u32 mode, level, time;
			char *name;

			switch(evs_gamemode) {
			case GMD_NORMAL:
				mode = 0;
				level = state->virus_level;
				break;
			case GMD_TIME_ATTACK:
				mode = 1;
				level = state->game_level;
				break;
			case GMD_TaiQ:
				mode = 2;
				level = state->game_level;
				break;
			}

			if(evs_select_name_no[0] == DM_MEM_GUEST) {
				name = _guest;
			}
			else {
				name = evs_mem_data[evs_select_name_no[0]].mem_name;
			}

			if(st->passBuf[0] == 0) {
				make_passwd((u16 *)st->passBuf, mode, level, state->cap_def_speed,
					state->game_score / 10, evs_game_time / FRAME_PAR_MSEC, name);
				st->passBuf[PASSWORD_LENGTH * 2 + 0] = '~';
				st->passBuf[PASSWORD_LENGTH * 2 + 1] = 'z';
			}

			msgWnd_clear(&st->passWnd);
			msgWnd_addStr(&st->passWnd, _mesPassword);
			msgWnd_addStr(&st->passWnd, st->passBuf);
			msgWnd_skip(&st->passWnd);

			st->passAlphaStep = -st->passAlphaStep;
			break;
		}
		else if(_getKeyTrg(0) != 0 && st->passAlphaStep > 0) {
			st->passAlphaStep = -st->passAlphaStep;
		}

		switch(retryMenu_input(player_no)) {
		case CONTINUE_RET_REPLAY:
			// ���v���C�̏���
			set_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_replay;

		case CONTINUE_RET_CONTINUE:
			// ���v���C��Ԃ����Z�b�g
			reset_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_next_stage;

		case CONTINUE_RET_RETRY:
			// ���v���C��Ԃ����Z�b�g
			reset_replay_state();

			if(state->game_retry < 999) {
				// ���g���C���̒ǉ�
				state->game_retry++;
			}

			if(state->virus_level >= 22 && st->retry_coin > 0) {
				// �R�C��������
				st->retry_coin--;

				// �R�C���g�p��
				dm_snd_play_in_game(SE_gCoin);
			}
			else {
				// ���g���C
				dm_snd_play_in_game(SE_mDecide);
			}
			return dm_ret_retry;

		case CONTINUE_RET_EXIT:
			// �I��
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;
		}
		break;

	// �|�[�Y����
	case dm_mode_pause:
		break;

	case dm_mode_pause_retry:
		switch(retryMenu_input(player_no)) {
		case PAUSE_RET_CONTINUE:
			// �|�[�Y����
			if(st->query_pause_player_no < 0) {
				st->query_pause_player_no = player_no;
			}
			dm_snd_play_in_game(SE_mDecide);
//			return dm_ret_pause;
			break;

		case PAUSE_RET_RETRY:
			// ���g���C���̒ǉ�
			if(state->game_retry < 999) {
				state->game_retry++;
			}

			if(state->virus_level >= 22 && st->retry_coin > 0) {
				// �R�C��������
				st->retry_coin--;

				// �R�C���g�p��
				dm_snd_play_in_game(SE_gCoin);
			}
			else {
				// ���g���C
				dm_snd_play_in_game(SE_mDecide);
			}
			return dm_ret_retry;

		case PAUSE_RET_EXIT:
			// �I��
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;
		}
		break;

	// �������Ȃ�
	case dm_mode_no_action:
		break;
	}

	return dm_ret_null;
}

//## �Q�[�����쏈��(���l���p)
static DM_GAME_RET dm_game_main_cnt(game_state *state, game_map *map, int player_no)
{
	Game *st = watchGame;
	int i, j, out, trg;

	// �L�[���̎擾
	trg = _getKeyTrg(player_no);

	// PAUSE �v�� or PAUSE �����v������
	dm_query_pause_player(state);

#ifdef _ENABLE_CONTROL
	// �f�o�b�O����v��
	dm_query_debug_player(state);
#endif // _ENABLE_CONTROL

	// �L�����A�j���[�V�������X�V
	animeState_update(&state->anime);

	// �X�R�A�G�t�F�N�g���X�V
	scoreNums_update(&st->scoreNums[player_no]);

	// ���オ�菈��
	if(state->cnd_static != dm_cnd_wait && state->cnd_static != dm_cnd_pause) {
		dm_black_up(state, map);
	}

	// �^�C���A�^�b�N�̏ꍇ�A�������Ԃ��߂��Ă��邩�`�F�b�N
	if(evs_gamemode == GMD_TIME_ATTACK &&
		evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
	{
		if(state->cnd_static == dm_cnd_wait) {
			// �������Ԃ��߂��Ă����̂ŃQ�[���I�[�o�[
			return dm_ret_game_over;
		}
	}

#ifdef _ENABLE_CONTROL
	// ��������
	if(trg & DM_KEY_Z) {
		if(state->cnd_static == dm_cnd_wait) {
			return dm_ret_clear;
		}
	}
#endif // _ENABLE_CONTROL

	switch(state->mode_now) {
	// �E�C���X�z�u
	case dm_mode_init:
		dm_set_virus(state, map, virus_map_data[player_no], virus_map_disp_order[player_no]);
		return dm_ret_virus_wait;

	// �E�F�C�g
	case dm_mode_wait:
		return dm_ret_virus_wait;

	// ���n�E�F�C�g
	case dm_mode_down_wait:

		// �ς݂����蔻��
		if(dm_check_game_over(state, map)) {
			// �ς݂�����(�Q�[���I�[�o�[)
			return dm_ret_game_over;
		}
		else {
			if(dm_h_erase_chack(map) != cap_flg_off || dm_w_erase_chack(map) != cap_flg_off) {
				// ���^�C�A���Ă��Ȃ�
				if(!state->flg_game_over) {
					// ���Ŕ���
					state->mode_now = dm_mode_erase_chack;
				}
				// ���^�C�A���Ă���(���K��)
				else if(state->flg_game_over) {
					// ���Ŕ���
					state->mode_now = dm_mode_tr_erase_chack;
				}
				state->cap_speed_count = 0;
			}
			else {
				// ���^�C�A���Ă��Ȃ�
				if(!state->flg_game_over) {
					// �J�v�Z���Z�b�g
					state->mode_now = dm_mode_cap_set;
				}
				// ���^�C�A���Ă���(���K��)
				else if(state->flg_game_over) {
					// �J�v�Z���Z�b�g
					state->mode_now = dm_mode_tr_cap_set;
				}
			}
		}
		break;

	// ���Ŕ���
	case dm_mode_erase_chack: {
		int erase;

		state->cap_speed_count++;
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}

		// ���ŊJ�n
		state->mode_now = dm_mode_erase_anime;
		state->cap_speed_count = 0;

		// �������� // ��������
		dm_h_erase_chack_set(state, map);
		dm_w_erase_chack_set(state, map);
		dm_h_ball_chack(map);
		dm_w_ball_chack(map);

		// �E�C���X���̎擾
		erase = state->virus_number;
		erase -= dm_update_virus_count(state, map, true);

		// �E�B���X���������擾
		state->total_erase_count += erase;

		// ���_�v�Z
		{
			int pos[2];
			dm_calc_erase_score_pos(state, map, pos);
			scoreNums_set(&st->scoreNums[player_no],
				dm_make_score(state), state->erase_virus_count,
				state->map_x + state->map_item_size / 2 + pos[0],
				state->map_y + state->map_item_size / 2 + pos[1]);
		}

		// �E�C���X����(�t���b�V���E�B���X�S��)
		if(state->virus_number == 0) {
			// �ő���ŗ񐔂̕ۊ�
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			return dm_ret_clear;
		}
		else if(state->virus_number <= st->last_3_se_cnt) {
			// �E�C���X���c��R�ȉ��̂Ƃ��x������炷
			if(!st->last_3_se_flg) {
				// �P��Ȃ炵����Ȍ�炳�Ȃ�
				st->last_3_se_flg = 1;

				// �E�C���X�c��R�r�d�Đ�
				dm_snd_play_in_game(SE_gReach);
			}

			// ���y���x�A�b�v�̃t���O�𗧂Ă�
			if(st->bgm_bpm_flg[0] == 0) {
				st->bgm_bpm_flg[0] = 1; // �Q��s��Ȃ����߂̃t���O
				st->bgm_bpm_flg[1] = 1; // �������s���t���O�𗧂Ă�
			}
		}

		state->chain_count++;

		switch(evs_gamesel) {
		case GSL_2PLAY: // 2�o�̎�
		case GSL_2DEMO: // 2�o�f��
		case GSL_VSCPU: // VSCPU�̂Ƃ�
			if(state->chain_line < 2) {

				if(state->chain_color[3] & 0x08) {
					// �E�C���X���ł̃��Z�b�g
					state->chain_color[3] &= 0xF7;

					// �E�C���X���܂ޏ��łr�d�Đ�
					if(evs_gamemode == GMD_FLASH && erase) {
						dm_snd_play_in_game(SE_gEraseFlash);
					}
					else {
						dm_snd_play_in_game(SE_gVrsErase);
					}
				}
				else {
					// �J�v�Z���݂̂̏��łr�d�Đ�
					dm_snd_play_in_game(SE_gCapErase);
				}
			}
			else {
				j = MIN(2, state->chain_line - 2);
				dm_snd_play_in_game(SE_getAttack(state->charNo, j));

				if(state->chain_color[3] & 0x08) {
					// �E�C���X���ł̃��Z�b�g
					state->chain_color[3] &= 0xF7;
				}
			}
			break;

		// 4�o or 4�o�f�� �̎�
		case GSL_4PLAY:
		case GSL_4DEMO:
			if(state->chain_color[3] & 0x08) {
				// �E�C���X���܂ޏ��ł̃��Z�b�g
				state->chain_color[3] &= 0xF7;

				// �E�C���X���܂ޏ��łr�d�Đ�
				if(evs_gamemode == GMD_FLASH && erase) {
					dm_snd_play_in_game(SE_gEraseFlash);
				}
				else {
					dm_snd_play_in_game(SE_gVrsErase);
				}
			}
			else {
				// �J�v�Z���݂̂̏��łr�d�Đ�
				dm_snd_play_in_game(SE_gCapErase);
			}
		}
		} break;

	// ���ŃA�j���[�V����
	case dm_mode_erase_anime:
		dm_capsel_erase_anime(state, map);
		break;

		// �J�v�Z��(��)����
	case dm_mode_ball_down:
		go_down(state,map,dm_down_speed);
		break;

	// �V�J�v�Z���Z�b�g
	case dm_mode_cap_set:
		out = 1;

		// ���v�A���񐔂��X�V
		if(state->chain_count > 1) {
			state->total_chain_count = MIN(99,
				state->total_chain_count + state->chain_count - 1);
		}

		// �U���r�d�Đ�
		dm_attack_se(state, player_no);

		// �ς݂�����`�F�b�N
		dm_warning_h_line(state, map);

		switch(evs_gamesel) {
		// 2�o or 2�o�f�� or VSCPU �̎�
		case GSL_2DEMO:
		case GSL_2PLAY:
		case GSL_VSCPU:
			// �^�C���A�^�b�N���͍U�����s��Ȃ�
			if(evs_gamemode == GMD_TIME_ATTACK) {
				break;
			}

			// �U������
			i = dm_set_attack_2p(state);

			// �U���A�j���[�V�����Z�b�g
			if(i) {
				animeState_set(&state->anime, ASEQ_ATTACK);
			}

			// ���ꔻ��
			if(dm_broken_set(state, map)) {
				// ����A�j���[�V�����Z�b�g(����)
				animeState_set(&state->anime, ASEQ_DAMAGE);

				// �E�C���X���܂ޏ��łr�d�Đ�
				dm_snd_play_in_game(SE_getDamage(state->charNo));

				// ������������
				state->mode_now = dm_mode_ball_down;
				out = 0;
				if(state->ai.aiState & AIF_DAMAGE) {
					state->ai.aiState |= AIF_DAMAGE2;
				}
				else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;

		// 4�o or 4�o�f�� �̎�
		case GSL_4PLAY:
		case GSL_4DEMO:
			// �U������
			i = dm_set_attack_4p(state);

			// �U���A�j���[�V����
			if(i) {
				animeState_set(&state->anime, ASEQ_ATTACK);
			}

			// ���ꔻ��
			if(dm_broken_set(state, map)) {
				// ����A�j���[�V������ݒ�
				animeState_set(&state->anime, ASEQ_DAMAGE);

				// ������������
				state->mode_now = dm_mode_ball_down;
				out = 0;

				if(state->ai.aiState & AIF_DAMAGE) {
					state->ai.aiState |= AIF_DAMAGE2;
				}
				else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;
		}

		if(out) {
			dm_set_capsel(state);

			// �����J�v�Z�����x�̌v�Z
			dm_capsel_speed_up(state);

			// �ő���ŗ񐔂̕ۊ�
			if(state->chain_line_max < state->chain_line) {
				state->chain_line_max = state->chain_line;
			}

			// ���Ń��C����, �A����, ���ŃE�C���X��, �����Z�b�g
			state->chain_line = 0;
			state->chain_count = 0;
			state->erase_virus_count = 0;
			state->erase_virus_count_old = 0;

			// ���ŐF�̃��Z�b�g
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// �J�v�Z������������
			state->mode_now = dm_mode_down;

			// �����O�v�l�ǉ��ʒu
			if(PLAYER_IS_CPU(state, player_no)) {
				aifMakeFlagSet(state);
			}
		}
		break;

	case dm_mode_clear_wait:
		effectNextStage_calc(player_no);
		break;

	case dm_mode_gover_wait:
		effectGameOver_calc(player_no);
		break;

	case dm_mode_retire_wait:
		effectRetire_calc(player_no);
		break;

	case dm_mode_clear_result:
	case dm_mode_gover_result:
	case dm_mode_retire_result:
		if(_getKeyTrg(player_no) & DM_ANY_KEY) {
			timeAttackResult_skip(&st->timeAttackResult[player_no]);
		}

		timeAttackResult_update(&st->timeAttackResult[player_no], true);
		dm_add_score(state, st->timeAttackResult[player_no].scoreDelta);
		st->timeAttackResult[player_no].scoreDelta = 0;
		break;

	// WIN & RETRY
	// LOSE & RETRY
	// DRAW & RETRY
	case dm_mode_win_retry:
	case dm_mode_lose_retry:
	case dm_mode_draw_retry:
		if(state->mode_now == dm_mode_win_retry) {
			if(effectWin_calc(player_no)) break;
		}
		else if(state->mode_now == dm_mode_lose_retry) {
			if(effectLose_calc(player_no)) break;
		}
		else if(state->mode_now == dm_mode_draw_retry) {
			if(effectDraw_calc(player_no)) break;
		}

		switch(retryMenu_input(player_no)) {
		case CONTINUE_RET_REPLAY:
			// ���v���C�̏���
			set_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_replay;

		case CONTINUE_RET_CONTINUE:
			// ���v���C��Ԃ����Z�b�g
			reset_replay_state();

			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_end;

		case CONTINUE_RET_RETRY:
			// ���v���C��Ԃ����Z�b�g
			reset_replay_state();

			// ���g���C���̒ǉ�
			if(state->game_retry < 999) {
				state->game_retry++;
			}
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_retry;

		case CONTINUE_RET_EXIT:
			// �I��
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_game_end;
		}
		break;

	// WIN���o
	case dm_mode_win:
		if(!effectWin_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// LOSE���o
	case dm_mode_lose:
		if(!effectLose_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// DRAW���o
	case dm_mode_draw:
		if(!effectDraw_calc(player_no)) {
//			if(trg & DM_ANY_KEY) {
//				return dm_ret_end;
//			}
		}
		break;

	// ���K�m�F
	case dm_mode_tr_chaeck:
		// �U��������
		if(trg & DM_KEY_START) {
			if(!st->replayFlag) {
				return dm_ret_tr_a;
			}
		}
//		// �U�������
//		else if(trg & DM_KEY_Z) {
//			return dm_ret_tr_b;
//		}
		break;

	// �g���[�j���O������
	case dm_mode_training:
		// �Q�[���I�[�o�[�t���O�𗧂Ă�(�O�̂���)
		state->flg_game_over = 1;

		clear_map_all(map); // �}�b�v���폜
		state->virus_number = 0; // �E�C���X���̃N���A
		state->virus_order_number = 0; // �z�u���̃E�B���X�ԍ�
		state->virus_anime_spead = v_anime_def_speed_4p;
		state->warning_se_flag = 0; // �x�����t���O���N���A

		// �J�v�Z���������x�̐ݒ�
		state->cap_speed = GameSpeed[state->cap_def_speed];
		state->cap_speed_max = 0; // �����ݒ肳���
		state->cap_speed_vec = 1; // �����J�E���^�����l
		state->cap_magazine_cnt = 1; // �}�K�W���c�Ƃ�1�ɂ���
		state->cap_count = state->cap_speed_count = 0; // �J�E���^�̏�����

		// �J�v�Z�����̏����ݒ�
		dm_set_capsel(state);

		// �E�C���X�A�j���[�V�����̐ݒ�
		state->erase_anime = 0; // ���ŃA�j���[�V�����R�}���̏�����
		state->erase_anime_count = 0; // ���ŃA�j���[�V�����J�E���^�̏�����
		state->erase_virus_count = 0; // ���ŃE�C���X�J�E���^�̏�����
		state->erase_virus_count_old = 0;

		// �A�����̏�����
		state->chain_count = state->chain_line = 0;

		// �U���J�v�Z���f�[�^�̏�����
		for(i = 0; i < 4; i++) {
			state->chain_color[i] = 0;
		}

		// �픚�J�v�Z���f�[�^�̏�����
		for(i = 0; i < DAMAGE_MAX; i++) {
			state->cap_attack_work[i][0] = 0; // �U������
			state->cap_attack_work[i][1] = 0; // �U������
		}

//		state->mode_now = dm_mode_down; // �ʏ폈����
//		state->cnd_now = dm_cnd_wait; // �ʏ��Ԃ�

		state->mode_now = dm_mode_init;
		state->cnd_now = dm_cnd_init;

		// �E�B���X�z�u����������
		for(i = 0; i < ARRAY_SIZE(virus_map_disp_order[player_no]); i++) {
			virus_map_disp_order[player_no][i] &= ~0x80;
		}

		// �t���b�V���E�B���X�F����������
		for(i = 0; i < state->flash_virus_count; i++) {
			state->flash_virus_pos[i][2] = state->flash_virus_bak[i];
		}

		break;

	case dm_mode_tr_erase_chack:
		state->cap_speed_count++;
		if(state->cap_speed_count < dm_bound_wait) {
			break;
		}

		state->cap_speed_count = 0;
		state->mode_now = dm_mode_erase_anime;

		dm_h_erase_chack_set(state,map); // �c��������
		dm_w_erase_chack_set(state,map); // ����������
		dm_h_ball_chack(map); // �c��������
		dm_w_ball_chack(map); // ����������

		state->chain_count++;

		// �E�C���X���̎擾
		dm_update_virus_count(state, map, true);

		if(state->chain_color[3] & 0x08) {
			// �E�C���X�t���O
			state->chain_color[3] &= 0xF7;

			// �E�C���X���܂ޏ��łr�d�Đ�
			dm_snd_play_in_game(SE_gVrsErase);
		}
		else {
			// �J�v�Z���݂̂̏��łr�d�Đ�
			dm_snd_play_in_game(SE_gCapErase);
		}
		break;

	case dm_mode_tr_cap_set:
		out = 1;

		// �ς݂�����`�F�b�N
		dm_warning_h_line(state, map);

		// �U������
		if(state->flg_training) {
			// �U���r�d�Đ�
			dm_attack_se(state, player_no);

			// �U������
			dm_set_attack_4p(state);

			// �U���A�j���[�V������ݒ�
			animeState_set(&state->anime, ASEQ_ATTACK);

			// ���ꔻ��
			if(dm_broken_set(state, map)) {
				// ������������
				state->mode_now = dm_mode_ball_down;
				out = 0;
			}
		}
		else {
			for(j = 0; j < DAMAGE_MAX; j++) {
				state->cap_attack_work[j][0] = 0; // �U������
				state->cap_attack_work[j][1] = 0; // �U������
			}
		}

		if(out) {
			dm_set_capsel(state);

			// �����J�v�Z�����x�̌v�Z
			dm_capsel_speed_up(state);

			// ���Ń��C����, �A����, ���ŃE�C���X��, �����Z�b�g
			state->chain_line = 0;
			state->chain_count = 0;
			state->erase_virus_count = 0;
			state->erase_virus_count_old = 0;

			// ���ŐF�̃��Z�b�g
			for(i = 0; i < 4; i++) {
				state->chain_color[i] = 0;
			}

			// �J�v�Z������������
			state->mode_now = dm_mode_down;
		}
		break;

	// �|�[�Y����
	case dm_mode_pause:
		break;

	case dm_mode_pause_retry:
		switch(retryMenu_input(player_no)) {
		case PAUSE_RET_CONTINUE:
			// �|�[�Y����
			if(st->query_pause_player_no < 0) {
				st->query_pause_player_no = player_no;
			}
			dm_snd_play_in_game(SE_mDecide);
//			return dm_ret_pause;
			break;

		case PAUSE_RET_RETRY:
			// ���g���C���̒ǉ�
			if(state->game_retry < 999) {
				state->game_retry++;
			}
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_retry;

		case PAUSE_RET_EXIT:
			// �I��
			dm_snd_play_in_game(SE_mDecide);
			return dm_ret_game_end;
		}
		break;

	// �������Ȃ�
	case dm_mode_no_action:
		break;
	}

	return dm_ret_null;
}

//## PAUSE ON �̐ݒ�
static void dm_set_pause_on(
	game_state *state, // �Q�[�����
	int master // �|�[�YON�����l
	)
{
	Game *st = watchGame;
	int p = state->player_no;

	// PAUSE ����������
	effectPause_init(p);

	state->cnd_static = dm_cnd_pause;    // ��ΓI�R���f�B�V������ PAUSE �ɐݒ�
	state->cnd_old    = state->cnd_now;  // ���݂̏�Ԃ̕ۑ�
	state->mode_old   = state->mode_now; // ���݂̏����̕ۑ�

	// ���x���Z���N�g
	if(evs_gamesel == GSL_1PLAY) {
		state->mode_now = dm_mode_pause_retry; // ������ PAUSE & RETRY �ɐݒ�
		state->cnd_now  = dm_cnd_pause_re;     // ��Ԃ� PAUSE & RETRY �ɐݒ�

		// LEVEL 21�ȉ� �� RETRY �p�R�C��������ꍇ
		if(state->virus_level < 22 || st->retry_coin > 0) {
			// RETRY �\�ɐݒ�
			retryMenu_init(0, PAUSE_NEX_TRY_END);
		}
		// �������Ȃ��āA���g���C�s�̏ꍇ
		else {
			retryMenu_init(0, PAUSE_NEX_END);
		}
	}
	// �|�[�Y��v�������l�����g���C�E���f�I�����𓾂�
	else if(p == master) {
		state->mode_now = dm_mode_pause_retry; // PAUSE & RETRY �����ɐݒ�
		state->cnd_now  = dm_cnd_pause_re;     // PAUSE & RETRY ��Ԃɐݒ�

		// �X�g�[���[���[�h��EXTRA�ȊO�̏ꍇ
//		if(evs_story_flg && !evs_one_game_flg) {
		if(evs_story_flg) {
			// PAUSE & RETRY & SCORE ��Ԃɐݒ�
			state->cnd_now = dm_cnd_pause_re_sc;

			// �s�[�`�P��
			if(evs_story_no == EX_STAGE_NO) {
				// ���g���C�s��
				retryMenu_init(p, PAUSE_NEX_TRY_END);
			}
			else {
				// ���g���C�\
				retryMenu_init(p, PAUSE_NEX_TRY_END);
			}
		}
		else {
			// ���g���C�\
			retryMenu_init(p, PAUSE_NEX_TRY_END);
		}
	}
	else {
		retryMenu_initPauseLogo(p);
		state->cnd_now  = dm_cnd_pause;  // PAUSE ��Ԃɐݒ�
		state->mode_now = dm_mode_pause; // PAUSE �����ɐݒ�
	}
}

//## PAUSE OFF �̐ݒ�
static void dm_set_pause_off(
	game_state *state // �Q�[�����
	)
{
	state->cnd_static = dm_cnd_wait; // ��ΓI�R���f�B�V������ʏ��Ԃɐݒ�
	state->cnd_now    = state->cnd_old; // �O�̏�Ԃɖ߂�
	state->mode_now   = state->mode_old; // �O�̏����ɖ߂�

	// ���K�m�F���������ꍇ
	// ��ΓI�R���f�B�V������s�k�ɐݒ�
	if(state->mode_now == dm_mode_tr_chaeck) {
		state->cnd_static = dm_cnd_lose;
	}
}

//## PAUSE ���� & ���ʂ̐ݒ�
static void dm_set_pause_and_volume(game_state **state, int count)
{
	Game *st = watchGame;
	int pause = st->query_pause_player_no;
	int sound = st->query_play_pause_se ? SE_gPause : -1;
	int i;

	st->query_pause_player_no = -1;
	st->query_play_pause_se = 0;

	// �|�[�Y��v�������l�����Ȃ��ꍇ�͒��f
	if(pause < 0) {
		return;
	}

	if(state[pause]->cnd_static == dm_cnd_wait) {
		// ���ʌ���
		dm_seq_set_volume(SEQ_VOL_LOW);

		// PAUSE ON!!!
		if(!st->replayFlag) {
			for(i = 0; i < count; i++) {
				dm_set_pause_on(state[i], pause);
			}
		}
		// ���v���C���畜�A
		else {
			resume_game_state(1);
		}

		pause = -1;
	}
	// PAUSE �����v��
	else if(state[pause]->cnd_static == dm_cnd_pause) {
		// ���ʉ�
		dm_seq_set_volume(SEQ_VOL_HI);

		// PAUSE OFF!!!
		for(i = 0; i < count; i++) {
			dm_set_pause_off(state[i]);
		}

		pause = -1;
	}
	else if(st->replayFlag && state[pause]->cnd_now == dm_cnd_tr_chack) {
		// ���ʌ���
		dm_seq_set_volume(SEQ_VOL_LOW);

		// ���v���C���畜�A
		resume_game_state(1);
	}

	// �|�[�YSE���Đ�
	if(pause < 0 && sound >= 0) {
		dm_snd_play_in_game(sound);
	}
}
static void dm_set_pause_and_volume_1p(game_state *state)
{
	game_state *stateArray[1] = { state };
	dm_set_pause_and_volume(stateArray, 1);
}

#ifdef _ENABLE_CONTROL
//## �f�o�b�O���[�h�̐ݒ�
static void dm_set_debug_mode()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	bool dbg = (st->query_debug_player_no >= 0);
	bool cfg = (st->query_config_player_no >= 0);

	st->query_debug_player_no = -1;
	st->query_config_player_no = -1;

	switch(state->mode_now) {
	// �|�[�Y����
	case dm_mode_pause_retry:
	case dm_mode_pause:
		// �f�o�b�N�p����
		if(dbg) {
			joyflg[0] |= DM_KEY_L | DM_KEY_R;
			state->cnd_now = dm_cnd_debug;
			state->mode_now = dm_mode_debug;
		}
		else if(cfg) {
			state->cnd_now = dm_cnd_debug_config;
			state->mode_now = dm_mode_debug_config;
		}
		break;

	// �f�o�b�N�p����
	case dm_mode_debug:
		if(dbg) {
			joyflg[0] &= ~(DM_KEY_L | DM_KEY_R);
			state->cnd_now = dm_cnd_pause;
			state->mode_now = dm_mode_pause;
		}
		break;

	case dm_mode_debug_config:
		if(cfg) {
			state->cnd_now = dm_cnd_pause;
			state->mode_now = dm_mode_pause;
		}
		break;
	}
}
#endif // _ENABLE_CONTROL

//## ����E�B���X�̍��W�v�Z
void dm_calc_big_virus_pos(game_state *state)
{
	Game *st = watchGame;
	int i, stopFlag;

	// �g��E�k��
	for(i = 0; i < 3; i++) {
		SAnimeState *ani = &st->virus_anime_state[i];
		float vec = 1;

		switch(ani->animeSeq->animeNo) {
		case ASEQ_LOSE:
			if(ani->frameCount >= 90) {
				vec = -1;
			}
			break;
		}
		st->big_virus_scale[i] = CLAMP(0, 1, st->big_virus_scale[i] + (vec / 60.0));
	}

	// �E�B���X��]��~���̓_�ŏ���
#if 1
	do {
		enum { DARK = 127 };
		static const struct {
			short limit, blink;
		} _tbl[] = {
			{ FRAME_PAR_SEC * 0.5, FRAME_PAR_SEC / 15 },
			{ FRAME_PAR_SEC * 1.0, FRAME_PAR_SEC / 10 },
			{ FRAME_PAR_SEC * 2.0, FRAME_PAR_SEC /  5 },
		};
		int color = DARK;

		for(i = 0; i < ARRAY_SIZE(_tbl); i++) {
			if(st->big_virus_stop_count < _tbl[i].limit) {
				static const short _clr[] = { 255, DARK };
				color = _clr[(st->big_virus_stop_count / _tbl[i].blink) & 1];
				break;
			}
		}

		for(i = 0; i < 3; i++) {
			SAnimeState *anim = &st->virus_anime_state[i];
			anim->color[0] = anim->color[1] = anim->color[2] = color;
		}

		st->big_virus_blink_count++;
	} while(0);
#else
/*
	do {
		int range = MIN(160, st->big_virus_stop_count + 1);
		int count = (st->big_virus_blink_count %= range);
		int color = count * 511 / range;

		if(color > 255) {
			color = 511 - color;
		}
		color = 255 - color / 2;

		for(i = 0; i < 3; i++) {
			SAnimeState *anim = &st->virus_anime_state[i];
			anim->color[0] = anim->color[1] = anim->color[2] = color;
		}

		st->big_virus_blink_count++;
	} while(0);
*/
#endif

	// �ʏ��Ԃ�����
	if(state->cnd_static != dm_cnd_wait) {
		return;
	}

	// �E�B���X��~�J�E���^���f�N�������g
	if(st->big_virus_stop_count > 0) {
		if(st->big_virus_no_wait) {
			st->big_virus_stop_count = 0;
			st->big_virus_no_wait = false;
		}
		else {
			st->big_virus_stop_count--;
		}
	}

	// ��]���s��������
	for(i = stopFlag = 0; i < 3; i++) {
		SAnimeState *ani = &st->virus_anime_state[i];

		switch(ani->animeSeq->animeNo) {
		case ASEQ_LOSE:
			switch(evs_gamemode) {
			case GMD_TaiQ:
				break;
			default:
				if(!animeSeq_isEnd(ani->animeSeq)) stopFlag++;
				break;
			}
			break;

		case ASEQ_DAMAGE:
			switch(evs_gamemode) {
			case GMD_TaiQ:
				break;
			default:
				stopFlag++;
				break;
			}
			break;

		case ASEQ_NORMAL:
			break;

		case ASEQ_WIN:
			if(state->cnd_static != dm_cnd_wait) {
				stopFlag++;
			}
			break;
		}
	}

	// ��~�J�E���^�[���L��?
	if(st->big_virus_stop_count > 0) {
		stopFlag++;
	}

	// �E�B���X��]����
	if(!stopFlag && st->started_game_flg) {
		int top = -1, lost = 0;

		for(i = 0; i < 3; i++) {
			SAnimeState *ani = &st->virus_anime_state[i];

			if(!st->big_virus_flg[i]) {
				if(top < st->big_virus_rot[i]) {
					top = st->big_virus_rot[i];
				}
			}
			else {
				if(animeSeq_isEnd(ani->animeSeq)) {
					lost++;
				}
			}
		}

		if(evs_gamemode == GMD_TaiQ && lost == 3) {
			for(i = 0; i < 3; i++) {
				st->big_virus_rot[i] = i * 120 + 1;
			}
			st->bottom_up_flag = true;
		}
		else if(st->big_virus_no_wait) {
			st->big_virus_timer = st->big_virus_wait * (360 - top);
		}
		else if(st->big_virus_timer < st->big_virus_wait) {
			st->big_virus_timer += 1;
		}
	}

	if(st->big_virus_timer >= st->big_virus_wait) {
		int step;

		step = st->big_virus_timer / st->big_virus_wait;
		step = MIN(step, 4);

		st->big_virus_timer -= st->big_virus_wait * step;

		for(i = 0; i < 3; i++) {
			st->big_virus_rot[i] += step;

			if(st->big_virus_rot[i] >= 360) {
				st->big_virus_rot[i] -= 360;

				// �r�̒��g���{�g���A�b�v
				if(!st->big_virus_flg[i] && evs_gamemode == GMD_TaiQ) {
					st->bottom_up_flag = true;
					st->big_virus_timer = 0;
					animeState_set(&st->virus_anime_state[i], ASEQ_WIN);
				}
			}
		}
	}

	// �E�B���X�̍��W���v�Z
	for(i = 0; i < 3; i++) {
		st->big_virus_pos[i][0] = sinf(DEGREE(st->big_virus_rot[i])) *  20 +  61;
		st->big_virus_pos[i][1] = cosf(DEGREE(st->big_virus_rot[i])) * -20 + 171;
	}
}

//## �Q�[�������֐�(1P�p)
static DM_GAME_RET dm_game_main_1p()
{
	Game *st = watchGame;
	game_state *state = &game_state_data[0];
	game_map *map = game_map_data[0];
	DM_GAME_RET ret;
	int i, j;

	// �|�[�Y����
	dm_set_pause_and_volume_1p(state);

#ifdef _ENABLE_CONTROL
	// �f�o�b�O����
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// ���C������
	ret = dm_game_main_cnt_1P(state, map, 0);

	// �݂�����x�������Đ�
	dm_warning_h_line_se();

	// �E�B���X�L�����A�j���[�V�������X�V
	if(st->big_virus_stop_count == 0) {
		for(i = 0; i < 3; i++) {
			animeState_update(&st->virus_anime_state[i]);
			animeSmog_update(&st->virus_smog_state[i]);
		}
	}

	// ����E�B���X�̍��W�v�Z
	dm_calc_big_virus_pos(state);

	// �J�E���g�_�E��SE���Đ�
	dm_play_count_down_se();

	// �E�C���X�z�u����
	if(ret == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			if(state->mode_now == dm_mode_wait) {
				// �^�C�}�[�쓮 // �J�v�Z�������J�n
				st->started_game_flg = 1;
				state->mode_now = dm_mode_throw;
//				state->mode_now = dm_mode_down;

				// mario throw
				animeState_set(&state->anime, ASEQ_ATTACK);

				// ���v���C�� �Đ��E�^�� ���J�n
				start_replay_proc();
			}
		}
	}
	// �N���A
	else if(ret == dm_ret_clear) {
		u32 score;

		effectNextStage_init(state->player_no);

		// �N���A BGM �̐ݒ�
		dm_seq_play_in_game(SEQ_End_A);

		// �^�C�}�[�J�E���g��~
		st->started_game_flg = 0;

		// ��ΓI�R���f�B�V������������Ԃɂ���
		state->cnd_static = dm_cnd_win;

		// ���̏���������
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			state->cnd_now  = dm_cnd_clear_wait;
			state->mode_now = dm_mode_clear_wait;

			timeAttackResult_result(&st->timeAttackResult[0], state->game_level, true,
				MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time),
				state->total_chain_count, state->total_erase_count, state->game_score);
			break;

		default:
			state->cnd_now  = dm_cnd_stage_clear;  // ��Ԃ� STAGE CLEAR �ɐݒ�
			state->mode_now = dm_mode_stage_clear; // ������ STAGE CLEAR �ɐݒ�
			break;
		}

		// ���j���[��ݒ�
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			// [���v���C] [������x] [�I���]
			retryMenu_init(0, CONTINUE_REP_TRY_END);
			break;

		case GMD_NORMAL:
			// [���v���C] [������]
			retryMenu_init(0, CONTINUE_REP_NEX_END);
			break;
		}

//		// �R�C���v�Z
//		if(state->virus_level >= 21 && (state->virus_level - 21) % 5 == 0) {
//			// (�ʏ탂�[�h && ���݂̃R�C���� COINMAX ���ȉ�) �̏ꍇ
//			if(evs_gamemode == GMD_NORMAL && st->retry_coin < COINMAX) {
//				// �R�C���l��
//				st->retry_coin++;
//
//				// �R�C���l����
//				dm_snd_play_in_game(SE_gCoin);
//
//				// �������R�C���l�����o�ɐݒ�
////				state->mode_now = dm_mode_get_coin;
//			}
//		}

		// �����A�j���[�V�����Z�b�g
		animeState_set(&state->anime, ASEQ_WIN);

		// �Z�[�u
		switch(evs_gamemode) {
		case GMD_NORMAL:
			dm_save_all();
			break;
		case GMD_TIME_ATTACK:
			score = game_state_data[0].game_score;
			game_state_data[0].game_score = st->timeAttackResult[0].result;
			dm_save_all();
			game_state_data[0].game_score = score;
			break;
		}
	}
	// �Q�[���I�[�o�[
	else if(ret == dm_ret_game_over) {
		effectGameOver_init(state->player_no);

		// ���������A�j���[�V�����Z�b�g
		animeState_set(&state->anime, ASEQ_LOSE);

		// GAME OVER ���ݒ�
		dm_seq_play_in_game(SEQ_End_C);

		state->virus_anime_spead = v_anime_speed; // �A�j���[�V�������x�𑁂�����
		state->cnd_static        = dm_cnd_lose;   // ��ΓI�R���f�B�V�����𕉂���Ԃɂ���

		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			state->cnd_now  = dm_cnd_gover_wait;
			state->mode_now = dm_mode_gover_wait;

			timeAttackResult_result(&st->timeAttackResult[0], state->game_level, false,
				0, state->total_chain_count, state->total_erase_count, state->game_score);
			break;
		default:
			state->cnd_now  = dm_cnd_game_over;  // ��Ԃ� GAME OVER �ɐݒ肷��
			state->mode_now = dm_mode_game_over; // ������ GAME OVER �ɐݒ肷��
			break;
		}

		// �^�C�}�[�J�E���g��~
		st->started_game_flg = 0;

		// LEVEL 21 �ȉ��̏ꍇ
		// RETRY �R�C���������Ă����ꍇ
		if(state->virus_level < 22 || st->retry_coin > 0) {
			// RETRY �\�ɐݒ�
			retryMenu_init(0, CONTINUE_REP_TRY_END);
		}
		// LEVEL 22 �ȏ�̏ꍇ
		else {
			// RETRY �s�ɐݒ�
			retryMenu_init(0, CONTINUE_REP_END);
		}

		// �Z�[�u
		dm_save_all();
	}
	// NEXT STAGE �̏ꍇ
	else if(ret == dm_ret_next_stage) {
		return dm_ret_next_stage;
	}
	// RETRY �̏ꍇ
	else if(ret == dm_ret_retry) {
		return dm_ret_retry;
	}
	// REPLAY �̏ꍇ
	else if(ret == dm_ret_replay) {
		return dm_ret_replay;
	}
	// �Q�[���I��
	else if(ret == dm_ret_end) {
		return dm_ret_game_over;
	}
	// ���S�I��
	else if(ret == dm_ret_game_end) {
		return dm_ret_game_end;
	}

	return dm_ret_null;
}

//## �����������Z(2P)
static bool dm_add_win_2p(
	game_state *state // �v���C���[���
	)
{
	Game *st = watchGame;
	bool finish = false;
	int sound = SEQ_End_C;
	int x, y, p = state->player_no;

	// �����|�C���g�̑���
	st->win_count[p]++;

	// �X�g�[���[���[�h�̏ꍇ
	if(evs_story_flg) {
		// �X�^�[�O���t�B�b�N��ǉ�
		x = _posStStar[p][0];
		y = _posStStar[p][1];

		// �������t����
		finish = true;

		// �v���C���[���������ꍇ
		if(state->player_type == PUF_PlayerMAN) {
			sound = SEQ_End_A;
		}
	}
	// �ΐ�̏ꍇ
	else {
		// �X�^�[�O���t�B�b�N��ǉ�
		x = _posP2StarX[p];
		y = _posP2StarY[evs_vs_count - 1][st->win_count[p] - 1];

		// �������t����
		finish = (st->win_count[p] == evs_vs_count);

		// ��������ǉ�
		if(finish) {
			st->vs_win_total[p] = MIN( 99, st->vs_win_total[p] + 1);
			st->vs_win_count[p] = MIN(999, st->vs_win_count[p] + 1);
		}

		if(evs_gamesel != GSL_VSCPU ||
			state->player_type == PUF_PlayerMAN)
		{
			// �R�{�擾�������H
			if(finish) {
				// �R�{�擾�̏ꍇ
				sound = SEQ_End_B;
			}
			else {
				// �P�{�擾�̏ꍇ
				sound = SEQ_End_A;
			}
		}
	}

	st->star_pos_x[st->star_count] = x;
	st->star_pos_y[st->star_count] = y;
	st->star_count++;

	// �T�E���h���Đ�
	dm_seq_play_in_game(sound);

	return finish;
}

//## �����҂̐ݒ�(2P,4P)
static bool dm_set_win_2p(
	game_state *state, // �Q�[�����
	bool finish, // �������t������?
	bool menu    // ���j���[���ݒ肳��Ă��邩?
	)
{
	int p = state->player_no;

	// ��ΓI�R���f�B�V������������Ԃɂ���
	state->cnd_static = dm_cnd_win;

	// �������o�̏�����
	effectWin_init(p);

	// �����A�j���[�V������ݒ�
	animeState_set(&state->anime, ASEQ_WIN);

	// �����̗Y����
	dm_snd_play_in_game(SE_getWin(state->charNo));

	// MAN �̏ꍇ
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		state->cnd_now  = dm_cnd_win_retry;  // ��Ԃ� WIN & RETRY �ɐݒ�
		state->mode_now = dm_mode_win_retry; // ������ WIN & RETRY �ɐݒ�

		// �X�g�[���[���[�h�ȊO�̏ꍇ
		if(!evs_story_flg) {
			// ���s���t�����ꍇ���g���C��t�t���ɂ���
			if(finish) {
				retryMenu_init(p, CONTINUE_REP_TRY_END);
			}
			else {
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
		// �X�g�[���[���[�h�̏ꍇ
		else {
			// �X�g�[���[EXTRA
//			if(evs_one_game_flg) {
//				retryMenu_init(p, CONTINUE_REP_END);
//			}
//			else
			{
				state->cnd_now = dm_cnd_win_retry_sc;
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
	}
	// CPU �̏ꍇ
	else {
		state->cnd_now  = dm_cnd_win;  // ��Ԃ� WIN �ɐݒ肷��
		state->mode_now = dm_mode_win; // ������ WIN �ɐݒ肷��
	}

	return menu;
}

//## �s�k�҂̐ݒ�(2P,4P)
static bool dm_set_lose_2p(
	game_state *state, // �Q�[�����
	bool finish, // �������t������?
	bool menu    // ���j���[���ݒ肳��Ă��邩?
	)
{
	int p = state->player_no;

	// ��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
	state->cnd_static = dm_cnd_lose;

	// �����A�j���[�V������ݒ�
	animeState_set(&state->anime, ASEQ_LOSE);

	// �������o�̏�����
	effectLose_init(p);

	// MAN �̏ꍇ
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		state->cnd_now  = dm_cnd_lose_retry; // ��Ԃ� LOSE & RETRY �ɐݒ肷��
		state->mode_now = dm_mode_lose_retry; // ������ LOSE & RETRY �ɐݒ肷��

		// �X�g�[���[���[�h�̏ꍇ
		if(evs_story_flg) {
			// �X�g�[���[EXTRA
//			if(evs_one_game_flg) {
//				retryMenu_init(p, CONTINUE_REP_END);
//			}
//			else
			{
				// ��Ԃ� LOSE & RETRY & SCORE �ɐݒ肷��
				state->cnd_now = dm_cnd_lose_retry_sc;

				// �s�[�`�P��
				if(evs_story_no == EX_STAGE_NO) {
					// ���g���C�s�\�ɐݒ肷��
					retryMenu_init(p, CONTINUE_REP_TRY_END);
				}
				else {
					// ���g���C�\�ɐݒ肷��
					retryMenu_init(p, CONTINUE_REP_TRY_END);
				}
			}
		}
		// �X�g�[���[���[�h�ȊO�̏ꍇ
		else {
			// ���s���t�����ꍇ���g���C��t�t���ɂ���
			if(finish) {
				retryMenu_init(p, CONTINUE_REP_TRY_END);
			}
			else {
				retryMenu_init(p, CONTINUE_REP_NEX_END);
			}
		}
	}
	// CPU �̏ꍇ
	else {
		state->cnd_now = dm_cnd_lose;  // ��Ԃ� LOSE �ɐݒ肷��
		state->mode_now = dm_mode_lose; // ������ LOSE �ɐݒ肷��
	}

	return menu;
}

//## �����̐ݒ�(2P,4P)
static bool dm_set_draw_2p(
	game_state *state, // �Q�[�����
	bool menu // ���j���[���ݒ肳��Ă��邩?
	)
{
	int p = state->player_no;

	// ��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
	state->cnd_static = dm_cnd_draw;

	// DRAW ���o�����̏�����
	effectDraw_init(p);

	// �����A�j���[�V������ݒ�
	animeState_set(&state->anime, ASEQ_LOSE);

	// MAN �̏ꍇ
	if(!menu && state->player_type == PUF_PlayerMAN) {
		menu = true;

		// DRAW & ANY KEY
		state->cnd_now = dm_cnd_draw_retry;
		state->mode_now = dm_mode_draw_retry;
		retryMenu_init(p, CONTINUE_REP_NEX_END);
	}
	// CPU �̏ꍇ
	else {
		state->cnd_now = dm_cnd_draw; // DRAW
		state->mode_now = dm_mode_draw; // ������ DRAW �ɐݒ�
	}

	return menu;
}

//## �^�C���A�^�b�N�̌����ݒ�
static bool dm_set_time_attack_result_2p(game_state **state)
{
	Game *st = watchGame;
	bool menu, finish;
	u32 result[2];
	int i;

	menu = finish = false;

	result[0] = st->timeAttackResult[0].result;
	result[1] = st->timeAttackResult[1].result;

	for(i = 0; i < 2; i++) {
		// ����
		if(result[i] > result[i^1]) {
			finish = dm_add_win_2p(state[i]);
		}
	}

	// ����
	for(i = 0; i < 2; i++) {
		if(result[i] > result[i^1]) {
			menu = dm_set_win_2p(state[i], finish, menu);
		}
	}

	// �s�k
	for(i = 0; i < 2; i++) {
		if(result[i] < result[i^1]) {
			menu = dm_set_lose_2p(state[i], finish, menu);
		}
	}

	// ����
	for(i = 0; i < 2; i++) {
		if(result[i] == result[i^1]) {
			menu = dm_set_draw_2p(state[i], menu);
			dm_seq_play_in_game(SEQ_End_C);
		}
	}

	return finish;
}

//## �Q�[�������֐�(2P�p)
static DM_GAME_RET dm_game_main_2p()
{
	Game *st = watchGame;
	game_state *state[2];
	game_map *map[2];
	bool finish, menu;
	int i, x, y;
	int clear, gover;
	int ret[2];

	finish = menu = false;
	clear = gover = 0;

	// �e�[�u�����쐬
	for(i = 0; i < 2; i++) {
		state[i] = &game_state_data[i];
		map[i] = game_map_data[i];
	}

	// �|�[�Y����
	dm_set_pause_and_volume(state, 2);

#ifdef _ENABLE_CONTROL
	// �f�o�b�O����
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// ���C������
	for(i = 0; i < 2; i++) {
		ret[i] = dm_game_main_cnt(state[i], map[i], i);
	}

	// �݂�����x�������Đ�
	dm_warning_h_line_se();

	// �J�E���g�_�E��SE���Đ�
	dm_play_count_down_se();

	// �E�C���X�ݒ芮��
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 2; i++) {
				if(state[i]->mode_now != dm_mode_wait) {
					// �܂��z�u���I����Ă��Ȃ�
					break;
				}
			}

			// �z�u����?
			if(i == 2) {
				// �^�C�}�[�J�E���g�J�n
				st->started_game_flg = 1;

				for(i = 0; i < 2; i++) {
					//�J�v�Z�������J�n
					state[i]->mode_now = dm_mode_down;

					// �����O�v�l�ǉ��ʒu
					if(PLAYER_IS_CPU(state[i], i)) {
						aifMakeFlagSet(state[i]);
					}
				}

				// ���v���C�� �Đ��E�^�� ���J�n
				start_replay_proc();
			}
		}
	}
	else {
		int waitTA, resultTA;

		for(i = 0; i < 2; i++) {
			// �N���A
			if(ret[i] == dm_ret_clear) {
				if(evs_gamemode == GMD_TIME_ATTACK) {
					// �X�e�[�W�N���A���S�G�t�F�N�g��������
					effectNextStage_init(i);

					// �����̌��������̂�҂ݒ�
					state[i]->mode_now = dm_mode_clear_wait;
					state[i]->cnd_now = dm_cnd_clear_wait;
					state[i]->cnd_static = dm_cnd_clear_wait;

					// �^�C���A�^�b�N�̃X�R�A�ݒ�
					timeAttackResult_result(&st->timeAttackResult[i],
						state[i]->game_level, true,
						MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time),
						state[i]->total_chain_count,
						state[i]->total_erase_count,
						state[i]->game_score);

					// �����a�f�l(short ver)
					_dm_seq_play_in_game(1, SEQ_End_BS);
				}
				else {
					clear++;

					// ��ΓI�R���f�B�V������ WIN �ɐݒ�
					state[i]->cnd_static = dm_cnd_win;
				}
			}
			// �Q�[���I�[�o�[���̐ݒ�
			else if(ret[i] == dm_ret_game_over) {

				if(evs_gamemode == GMD_TIME_ATTACK) {
					if(evs_game_time >= SCORE_ATTACK_TIME_LIMIT) {
						// �Q�[���I�[�o�[���S�G�t�F�N�g��������
						effectGameOver_init(i);

						// �����̌��������̂�҂ݒ�
						state[i]->mode_now = dm_mode_gover_wait;
						state[i]->cnd_now = dm_cnd_gover_wait;
						state[i]->cnd_static = dm_cnd_gover_wait;
					}
					else {
						// ���^�C�A���S�G�t�F�N�g��������
						effectRetire_init(i);

						// �����̌��������̂�҂ݒ�
						state[i]->mode_now = dm_mode_retire_wait;
						state[i]->cnd_now = dm_cnd_retire_wait;
						state[i]->cnd_static = dm_cnd_retire_wait;
					}

					// �^�C���A�^�b�N�̃X�R�A�ݒ�
					timeAttackResult_result(&st->timeAttackResult[i],
						state[i]->game_level, false,
						0,
						state[i]->total_chain_count,
						state[i]->total_erase_count,
						state[i]->game_score);

					// �����a�f�l(short ver)
					_dm_seq_play_in_game(1, SEQ_End_CS);
				}
				else {
					gover++;

					// ��ΓI�R���f�B�V������ LOSE �ɐݒ�
					state[i]->cnd_static = dm_cnd_lose;
				}

				// �A�j���[�V�������x�𑁂�����
				state[i]->virus_anime_spead = v_anime_speed;

				state[i]->black_up_count = 16; // ���オ�菈���̃J�E���^�̐ݒ�
				state[i]->flg_retire     = 1;  // ���^�C�A�t���O�𗧂Ă�
			}
			// RETRY
			else if(ret[i] == dm_ret_retry) {
				return dm_ret_retry;
			}
			// REPLAY
			else if(ret[i] == dm_ret_replay) {
				return dm_ret_replay;
			}
			// �I��
			else if(ret[i] == dm_ret_end) {
				return dm_ret_game_over;
			}
			// ���S�I��
			else if(ret[i] == dm_ret_game_end) {
				return dm_ret_game_end;
			}
		}

		// �^�C���A�^�b�N�̌��������l�������߂�
		waitTA = resultTA = 0;
		for(i = 0; i < 2; i++) {
			switch(state[i]->cnd_now) {
			case dm_cnd_clear_wait:
			case dm_cnd_gover_wait:
			case dm_cnd_retire_wait:
				if(st->effect_timer[i] == 0) {
					waitTA++;
				}
				break;
			case dm_cnd_clear_result:
			case dm_cnd_gover_result:
			case dm_cnd_retire_result:
				if(timeAttackResult_isEnd(&st->timeAttackResult[i])) {
					resultTA++;
				}
				break;
			}
		}

		// �^�C���A�^�b�N�̌������t����
		if(waitTA == 2) {
			for(i = 0; i < 2; i++) {
				// �^�C���A�^�b�N�̃X�R�A�\��
				switch(state[i]->cnd_now) {
				case dm_cnd_clear_wait:
					state[i]->cnd_static = dm_cnd_clear_result;
					state[i]->cnd_now    = dm_cnd_clear_result;
					state[i]->mode_now   = dm_mode_clear_result;
					break;
				case dm_cnd_gover_wait:
					state[i]->cnd_static = dm_cnd_gover_result;
					state[i]->cnd_now    = dm_cnd_gover_result;
					state[i]->mode_now   = dm_mode_gover_result;
					break;
				case dm_cnd_retire_wait:
					state[i]->cnd_static = dm_cnd_retire_result;
					state[i]->cnd_now    = dm_cnd_retire_result;
					state[i]->mode_now   = dm_mode_retire_result;
					break;
				}
			}
		}
		// �^�C���A�^�b�N�̃X�R�A�\������������
		else if(resultTA == 2) {
			finish = dm_set_time_attack_result_2p(state);
		}
		else if(clear == 2 || gover == 2) {
			for(i = 0; i < 2; i++) {
				// ���������̐ݒ�
				menu = dm_set_draw_2p(state[i], menu);
			}
			// �T�E���h���Đ�
			dm_seq_play_in_game(SEQ_End_C);
		}
		else if(clear) {
			// �T�E���h�̐ݒ�ƃQ�[���I������̑O�ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					finish = dm_add_win_2p(state[i]);
				}
			}

			// �����҂̐ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
			}

			// �s�k�҂̐ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_win) {
					menu = dm_set_lose_2p(state[i], finish, menu);
				}
			}

		}
		else if(gover) {
			// �T�E���h�̐ݒ�ƃQ�[���I������̑O�ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_lose) {
					finish = dm_add_win_2p(state[i]);
				}
			}

			// �����҂̐ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static != dm_cnd_lose) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
			}

			// �s�k�҂̐ݒ�
			for(i = 0; i < 2; i++) {
				if(state[i]->cnd_static == dm_cnd_lose) {
					menu = dm_set_lose_2p(state[i], finish, menu);
				}
			}
		}

		// �Z�[�u
		if(finish) {
			dm_save_all();
		}
	}

	return dm_ret_null;
}

//## �Q�[�������֐�(4P�p)
static DM_GAME_RET dm_game_main_4p()
{
	Game *st = watchGame;
	game_state *state[4];
	game_map *map[4];
	DM_GAME_RET ret[4];
	int i;

	// �e�[�u�����쐬
	for(i = 0; i < 4; i++) {
		state[i] = &game_state_data[i];
		map[i] = game_map_data[i];
	}

	// �|�[�Y����
	dm_set_pause_and_volume(state, 4);

#ifdef _ENABLE_CONTROL
	// �f�o�b�O����
	dm_set_debug_mode();
#endif // _ENABLE_CONTROL

	// ���C������
	for(i = 0; i < 4; i++) {
		ret[i] = dm_game_main_cnt(state[i], map[i], i);
	}

	// �݂�����x�������Đ�
	dm_warning_h_line_se();

	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait &&
		ret[2] == dm_ret_virus_wait && ret[3] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 4; i++) {
				if(state[i]->mode_now != dm_mode_wait) {
					// �܂��z�u���I����Ă��Ȃ�
					break;
				}
			}

			// �E�C���X�ݒ芮��?
			if(i == 4) {
				// �^�C�}�[�J�E���g�J�n
				st->started_game_flg = 1;

				for(i = 0; i < 4; i++) {
					//�J�v�Z�������J�n
					state[i]->mode_now = dm_mode_down;

					// �����O�v�l�ǉ��ʒu
					if(PLAYER_IS_CPU(state[i], i)) {
						aifMakeFlagSet(state[i]);
					}
				}

				// ���v���C�� �Đ��E�^�� ���J�n
				start_replay_proc();
			}
		}
	}
	else {
		bool finish, menu;
		int clear, gover, sound, win_team;

		finish = menu = false;
		clear = gover = 0;

		for(i = 0; i < 4; i++) {
			// �E�B���X���Ĕz�u
			if(ret[i] == dm_ret_virus_wait &&
				state[i]->cnd_training == dm_cnd_training)
			{
				if(state[i]->mode_now == dm_mode_wait) {
					state[i]->mode_now = dm_mode_down;
				}
			}
			// �N���A
			else if(ret[i] == dm_ret_clear) {
				clear++;

				// ��ΓI�R���f�B�V������ WIN �ɐݒ肷��
				state[i]->cnd_static = dm_cnd_win;
			}
			// �Q�[���I�[�o�[���̐ݒ�
			else if(ret[i] == dm_ret_game_over) {
				// ���^�C�A��Ԃ���Ȃ��ꍇ
				if(!state[i]->flg_retire) {
					gover++;

					state[i]->cnd_now        = dm_cnd_retire;    // ���^�C�A��Ԃɂ���
					state[i]->cnd_training   = dm_cnd_retire;    // ���^�C�A��Ԃɂ���
					state[i]->cnd_static     = dm_cnd_lose;      // ��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
					state[i]->black_up_count = 16;               // ���オ��p�J�E���^�̃Z�b�g
					state[i]->flg_retire     = 1;                // ���^�C�A�t���O�𗧂Ă�

					// �b�o�t�Ȃ���K���Ȃ�
					if(state[i]->player_type == PUF_PlayerCPU) {
						state[i]->mode_now = dm_mode_no_action; // ������ NO ACTION (�������Ȃ�) �ɐݒ肷��
					}
					else {
						state[i]->cnd_now  = dm_cnd_tr_chack;   // ��Ԃ���K�m�F�ɐݒ肷��
						state[i]->mode_now = dm_mode_tr_chaeck; // ��������K�m�F�ɐݒ肷��
					}
				}
				// ���K���̏ꍇ
				else {
					state[i]->cnd_now  = dm_cnd_tr_chack;   // ��Ԃ���K�m�F�ɐݒ肷��
					state[i]->mode_now = dm_mode_tr_chaeck; // ��������K�m�F�ɐݒ肷��
				}

				// �A�j���[�V�������x�𑁂�����
				state[i]->virus_anime_spead = v_anime_speed_4p;
			}
			// ���g���C
			else if(ret[i] == dm_ret_retry) {
				return dm_ret_retry;
			}
			// REPLAY
			else if(ret[i] == dm_ret_replay) {
				return dm_ret_replay;
			}
			// �I��
			else if(ret[i] == dm_ret_end) {
				return dm_ret_game_over;
			}
			// ���S�I��
			else if(ret[i] == dm_ret_game_end) {
				return dm_ret_game_end;
			}
			// ���K�U������
			else if(ret[i] == dm_ret_tr_a) {
				state[i]->cnd_static   = dm_cnd_wait;      // ��ΓI�R���f�B�V������ �ʏ� �ɐݒ肷��
				state[i]->flg_training = 0;                // �U���i��e�j�����ɐݒ肷��
				state[i]->cnd_training = dm_cnd_training;  // ��Ԃ���K�ɐݒ肷��
				state[i]->mode_now     = dm_mode_training; // ��������K�ɐݒ肷��
			}
			// ���K�U������
			else if(ret[i] == dm_ret_tr_b) {
				state[i]->cnd_static   = dm_cnd_wait;      // ��ΓI�R���f�B�V������ �ʏ� �ɐݒ肷��
				state[i]->flg_training = 1;                // �U���i��e�j�L��ɐݒ肷��
				state[i]->cnd_training = dm_cnd_training;  // ��Ԃ���K�ɐݒ肷��
				state[i]->mode_now     = dm_mode_training; // ��������K�ɐݒ肷��
			}
		}

		// �N�����N���A����
		if(clear) {
			int clearBit = 0;

			for(i = 0; i < 4; i++) {
				if(state[i]->cnd_static == dm_cnd_win) {
					clearBit |= 1 << i;
					win_team = state[i]->team_no;
				}
			}

			// �o�g�����C�����ŁA�N���A�l�����Q�l�ȏ�̏ꍇ DRAW
			if(!st->vs_4p_team_flg && clear > 1) {
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// �T�E���h��ݒ�
				sound = SEQ_End_C;
			}
			// �`�[���o�g���ŁA�����̃`�[���ɃN���A�������̂����ꍇ DRAW
			else if(st->vs_4p_team_flg &&
				(clearBit & st->vs_4p_team_bits[TEAM_MARIO]) &&
				(clearBit & st->vs_4p_team_bits[TEAM_ENEMY]))
			{
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// �T�E���h��ݒ�
				sound = SEQ_End_C;
			}
			else {
				sound = SEQ_End_A;

				for(i = 0; i < 4; i++) {
					// �����`�[���̃J�E���g��ǉ�����
					// (�`�[���΍R��łȂ��ꍇ�̓`�[���ԍ����ʁX�Ȃ̂Ŗ��Ȃ�)
					if(state[i]->cnd_static == dm_cnd_win) {
						int team = state[i]->team_no;
						int win  = st->win_count[team];

						if(evs_story_flg) {
							st->star_pos_x[st->star_count] = _posStP4StarX[i];
							st->star_pos_y[st->star_count] = _posStP4StarY;
						}
						else if(st->vs_4p_team_flg) {
							st->star_pos_x[st->star_count] = _posP4TeamStarX[evs_vs_count - 1][team][win];
							st->star_pos_y[st->star_count] = _posP4TeamStarY;
						}
						else {
							st->star_pos_x[st->star_count] = _posP4CharStarX[evs_vs_count - 1][team][win];
							st->star_pos_y[st->star_count] = _posP4CharStarY;
						}

						st->star_count++;
						st->win_count[team]++;

						if(evs_story_flg) {
							finish = true;
							// SE�͂��ƂŌ��߂܂�
						}
						// �������t����(�R�{���)
						else if(st->win_count[team] == evs_vs_count) {
							// �������t�����t���O�𗧂Ă�
							finish = true;
							sound = SEQ_End_B;
						}
					}
				}

				// ���҂̐ݒ�
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no == win_team) {
						if(evs_story_flg && state[i]->player_type == PUF_PlayerMAN) {
							sound = SEQ_End_A;
						}
						menu = dm_set_win_2p(state[i], finish, menu);
					}
				}

				// �s�҂̐ݒ�
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no != win_team) {
						if(evs_story_flg && state[i]->player_type == PUF_PlayerMAN) {
							sound = SEQ_End_C;
						}
						menu = dm_set_lose_2p(state[i], finish, menu);
					}
				}
			}

			// BGM �̐ݒ�
			dm_seq_play_in_game(sound);
		}
		else if(gover) {
			bool end = false;
			int retire = 0, retireBit = 0;

			for(i = 0; i < 4; i++) {
				// ���^�C�A���Ă���
				if(state[i]->flg_retire) {
					retire++;
					retireBit |= 1 << i;
				}
			}

			// �S�����^�C�A(DRAW)
			if(retire == 4) {
				for(i = 0; i < 4; i++) {
					menu = dm_set_draw_2p(state[i], menu);
				}
				// �T�E���h���Đ�
				dm_seq_play_in_game(SEQ_End_C);
			}
			// �X�g�[���[�Ńv���C���[���s�k
			else if(evs_story_flg && (retireBit & 1)) {
				finish = true;
				menu = dm_set_lose_2p(state[0], finish, menu);
				for(i = 1; i < 4; i++) {
					menu = dm_set_win_2p(state[i], finish, menu);
				}
				// �T�E���h���Đ�
				dm_seq_play_in_game(SEQ_End_C);
			}
			// �o�g�����C����(1P vs 2P vs 3P vs 4P)�̏ꍇ
			// ���^�C�A�� �R�l�̂Ƃ�
			else if(!st->vs_4p_team_flg && retire == 3) {
				int team, win;

				end = true;
				sound = SEQ_End_A;

				for(i = 0; i < 4; i++) {
					if(state[i]->flg_retire) continue;

					team = state[i]->team_no;
					win  = st->win_count[team];

					if(evs_story_flg) {
						st->star_pos_x[st->star_count] = _posStP4StarX[i];
						st->star_pos_y[st->star_count] = _posStP4StarY;
					}
					else {
						st->star_pos_x[st->star_count] = _posP4CharStarX[evs_vs_count - 1][team][win];
						st->star_pos_y[st->star_count] = _posP4CharStarY;
					}
					st->star_count++;
					st->win_count[team]++;
					win_team = team;

					if(evs_story_flg) {
						if(state[i]->player_type != PUF_PlayerMAN) {
							sound = SEQ_End_C;
						}
						finish = true;
					}
					else if(st->win_count[team] == evs_vs_count) {
						// ���������܂����ɐݒ肷��
						finish = true;
						sound = SEQ_End_B;
					}

					break;
				}
			}
			// �`�[���΍R��̏ꍇ
			else if(st->vs_4p_team_flg) {
				int team, win;

				if((retireBit & st->vs_4p_team_bits[TEAM_MARIO]) == st->vs_4p_team_bits[TEAM_MARIO]) {
					team = TEAM_ENEMY;
					win  = st->win_count[team];

					st->win_count[team]++; // ENEMY �`�[���̎擾�|�C���g���Z
					win_team = team;       // �����`�[���̐ݒ������
					end = true;            // �P�����I���̐ݒ������
				}
				else if((retireBit & st->vs_4p_team_bits[TEAM_ENEMY]) == st->vs_4p_team_bits[TEAM_ENEMY]) {
					team = TEAM_MARIO;
					win  = st->win_count[team];

					st->win_count[team]++; // MARIO �`�[���̎擾�|�C���g���Z
					win_team = team;       // �����`�[���̐ݒ������
					end = true;            // �P�����I���̐ݒ������
				}

				// �X�^�[�O���t�B�b�N��ǉ�
				if(end) {
					st->star_pos_x[st->star_count] = _posP4TeamStarX[evs_vs_count - 1][team][win];
					st->star_pos_y[st->star_count] = _posP4TeamStarY;
					st->star_count++;

					// ���s�����܂����ꍇ�A�����I���̐ݒ������
					sound = SEQ_End_A;
					for(i = 0; i < 4; i++) {
						if(st->win_count[i] == evs_vs_count) {
							finish = true;
							sound = SEQ_End_B;
							break;
						}
					}
				}
			}

			if(end) {
				// ���҂̐ݒ�
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no == win_team) {
						menu = dm_set_win_2p(state[i], finish, menu);
					}
				}

				// �s�҂̐ݒ�
				for(i = 0; i < 4; i++) {
					if(state[i]->team_no != win_team) {
						menu = dm_set_lose_2p(state[i], finish, menu);
					}
				}

				dm_seq_play_in_game(sound);
			}
		}

		// �Z�[�u
		if(finish) {
			dm_save_all();
		}
	}

	return dm_ret_null;
}

//## �Q�[���f�������֐�(1P�p)
static DM_GAME_RET dm_game_demo_1p()
{
	Game *st = watchGame;
	DM_GAME_RET ret;
	int i;

	ret = dm_game_main_cnt_1P(&game_state_data[0], game_map_data[0], 0);

	// �݂�����x�������Đ�
	dm_warning_h_line_se();

	// ����E�C���X�A�j���[�V����
	for(i = 0; i < 3; i++) {
		animeState_update(&st->virus_anime_state[i]);
		animeSmog_update(&st->virus_smog_state[i]);
	}

	// ����E�B���X�̍��W�v�Z
	dm_calc_big_virus_pos( &game_state_data[0] );

	// �E�C���X�z�u����
	if(ret == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			//�J�v�Z�������J�n
			game_state_data[0].mode_now = dm_mode_throw;
//			game_state_data[0].mode_now = dm_mode_down;

			// mario throw
			animeState_set(&game_state_data[0].anime, ASEQ_ATTACK);

			if(PLAYER_IS_CPU(&game_state_data[0], 0)) {
				aifMakeFlagSet(&game_state_data[0]);
			}

			// �^�C�}�[�J�E���g�J�n
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// �f�����f
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//## �Q�[���f�������֐�(2P�p)
static DM_GAME_RET dm_game_demo_2p()
{
	Game *st = watchGame;
	DM_GAME_RET ret[2];
	int i;

	for(i = 0; i < 2; i++) {
		ret[i] = dm_game_main_cnt(&game_state_data[i], game_map_data[i], i);
	}

	// �݂�����x�������Đ�
	dm_warning_h_line_se();


	// �E�C���X�ݒ芮��
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait) {
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 2; i++) {
				//�J�v�Z�������J�n
				game_state_data[i].mode_now = dm_mode_down;

				// �����O�v�l�ǉ��ʒu
				if(PLAYER_IS_CPU(&game_state_data[i], i)) {
					aifMakeFlagSet(&game_state_data[i]);
				}
			}
			// �^�C�}�[�J�E���g�J�n
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// �f�����f
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//## �Q�[���f�������֐�(4P�p)
static DM_GAME_RET dm_game_demo_4p()
{
	Game *st = watchGame;
	DM_GAME_RET ret[4];
	int i;

	for(i = 0; i < 4; i++) {
		ret[i] = dm_game_main_cnt(&game_state_data[i], game_map_data[i], i);
	}

	// �݂�����x�������Đ�
	dm_warning_h_line_se();

	// �E�C���X�ݒ芮��
	if(ret[0] == dm_ret_virus_wait && ret[1] == dm_ret_virus_wait &&
		ret[2] == dm_ret_virus_wait && ret[3] == dm_ret_virus_wait)
	{
		if(st->count_down_ctrl < 0) {
			for(i = 0; i < 4; i++) {
				//�J�v�Z�������J�n
				game_state_data[i].mode_now = dm_mode_down;

				// �����O�v�l�ǉ��ʒu
				if(PLAYER_IS_CPU(&game_state_data[i], i)) {
					aifMakeFlagSet(&game_state_data[i]);
				}
			}
			// �^�C�}�[�J�E���g�J�n
			st->started_game_flg = 1;
		}
	}

	if(st->demo_timer) {
		st->demo_timer--;

		if(_getKeyTrg(0) & DM_ANY_KEY) {
			st->demo_timer = 0;
		}

		// �f�����f
		if(st->demo_timer == 0) {
			return dm_ret_next_stage;
		}
	}

	return dm_ret_null;
}

//////////////////////////////////////////////////////////////////////////////
//{### �O���t�B�b�N�f�[�^

//## �e��e�N�X�`���̃��x��
#include "texture/game/game_al.h"
#include "texture/game/game_p1.h"
#include "texture/game/game_p2.h"
#include "texture/game/game_p4.h"
#include "texture/game/game_ls.h"
#include "texture/game/game_item.h"
#include "texture/menu/menu_kasa.h"

//## �J�v�Z���̃O���t�B�b�N���擾
STexInfo *dm_game_get_capsel_tex(int sizeIndex)
{
	static const int cap_tex[] = { TEX_MITEM10R, TEX_MITEM08R };
	Game *st = watchGame;
	return st->texItem + cap_tex[sizeIndex];
}

//## �J�v�Z���̃p���b�g���擾
STexInfo *dm_game_get_capsel_pal(int sizeIndex, int colorIndex)
{
	static const int cap_pal[][6] = {
		{ TEX_MITEM10R, TEX_MITEM10Y, TEX_MITEM10B,
		  TEX_MITEM10RX,TEX_MITEM10YX,TEX_MITEM10BX },
		{ TEX_MITEM08R, TEX_MITEM08Y, TEX_MITEM08B,
		  TEX_MITEM08RX,TEX_MITEM08YX,TEX_MITEM08BX },
	};
	Game *st = watchGame;
	return st->texItem + cap_pal[sizeIndex][colorIndex];
}

//////////////////////////////////////////////////////////////////////////////
//{### �`�敨

//## �X�R�A�\�� : �X�R�A��`��
static void scoreNums_draw(_ScoreNums *st, Gfx **gpp)
{
	Game *g = watchGame;
	Gfx *gp = *gpp;
	_ScoreNum *num;
	STexInfo *texC = g->texAL + TEX_rank_figure;
	STexInfo *texA = g->texAL + TEX_rank_figure_alpha;
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 12;
	int i, x, y, alpha;
	float time, bound;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(st->numbers); i++) {
		num = &st->numbers[WrapI(0, ARRAY_SIZE(st->numbers), st->index + i)];
		time = num->time;

		if(time == 1.0) {
			continue;
		}

		if(time < 0.2) {
			alpha = time * (255 * 5);
		}
		else if(time < 0.8) {
			alpha = 255;
		}
		else {
			alpha = (1 - time) * (255 * 5);
		}

		bound = MIN(0.5, time) * (1.0 / 0.25) - 1.0;
		bound = (1.0 - bound * bound) * 8.0;

		gDPSetPrimColor(gp++, 0,0,
			_scoreNumsColor[num->color][0], _scoreNumsColor[num->color][1],
			_scoreNumsColor[num->color][2], alpha);

		StretchAlphaTexBlock(&gp, width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * num->number * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * num->number / 2, texA->size[0],
			num->pos[0], num->pos[1] - bound, width, height);
	}

	*gpp = gp;
}

//## ����������
static void starForce_init(_StarForce *star, int *xx, int *yy)
{
	int i;

	// ���W�e�[�u��
	star->xtbl = xx;
	star->ytbl = yy;

	// �t���[���J�E���^
	for(i = 0; i < ARRAY_SIZE(star->frame); i++) {
		star->frame[i] = 0;
	}
}

//## �����v�Z
static void starForce_calc(_StarForce *star, int count)
{
	int i;

	for(i = 0; i < count; i++) {
		if(star->frame[i] < 48) {
			star->frame[i]++;
		}
		else {
			star->frame[i] = WrapI(48, 48 + 60, star->frame[i] + 1);
		}
	}
}

//## ����`��
static void starForce_draw(_StarForce *star, Gfx **gpp, int count)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, j;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	texC = st->texAL + TEX_new_star;
	texA = st->texAL + TEX_new_star_alpha;

	// ����`��
	for(i = 0; i < count; i++) {
		if(star->frame[i] < 48) {
			gDPSetPrimColor(gp++, 0,0, 255,255,255, 255);
			tiStretchAlphaTexItem(&gp, texC, texA, false, 16, 0,
				star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1] / 16);
		}

		j = MIN(255, star->frame[i] << 3);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, j);

		j = MAX(0, star->frame[i] - 48) >> 2;
		tiStretchAlphaTexItem(&gp, texC, texA, false, 16, j + 1,
			star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1] / 16);
	}

	// ������`��
	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetPrimColor(gp++, 0,0, 255,255,200, 255);
	gDPSetTextureLUT(gp++, G_TT_NONE);

	for(i = 0; i < count; i++) {
		j = star->frame[i] >> 2;
		if(j >= 12) continue;

		texC = st->texAL + TEX_STARDUST01 + j;
		StretchTexTile4i(&gp,
			texC->size[0], texC->size[1], texC->addr[1],
			0, 0, texC->size[0], texC->size[1],
			star->xtbl[i], star->ytbl[i], texC->size[0], texC->size[1]);
	}

	*gpp = gp;
}

//## ���̉��n��`��
static void draw_star_base(Gfx **gpp, int x, int y, int cached)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;

	texC = st->texAL + TEX_new_star;
	texA = st->texAL + TEX_new_star_alpha;

	if(!cached) {
		gSPDisplayList(gp++, alpha_texture_init_dl);
	}

	tiStretchAlphaTexItem(&gp, texC, texA, cached, 16, 0,
		x, y, texC->size[0], texC->size[1] / 16);

	*gpp = gp;
}

//## 4P,PAUSE ���̍U���Ώۃp�l��
static void draw_4p_attack_guide_panel(Gfx **gpp, int playerCount, int playerNo, int x, int y)
{
	static const u8 _tbl[][4] = {
		{ 0, 5, 8, 10 },
		{ 0, 1, 6,  9 },
		{ 0, 1, 2,  4 },
		{ 0, 1, 2,  3 },
	};

	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, j;

	gSPDisplayList(gp++, normal_texture_init_dl);

	// ���n
	texC = st->texP4 + TEX_P4_CP_PANEL;
	tiStretchTexItem(&gp, texC, false,
		4, playerNo, x, y, texC->size[0], texC->size[1] / 4);

	gSPDisplayList(gp++, alpha_texture_init_dl);

	// �v���C���[�ԍ�
	for(i = 0; i < 3; i++) {
		float xx, yy;
		int a, b;
		xx = x + 36;
		yy = y + 5 + i * 13;

		a = game_state_data[playerNo].team_no;
		b = game_state_data[(playerNo + 1 + i) % 4].team_no;

		if(a == b) {
			texC = st->texP4 + TEX_stock_a + a;
			texA = st->texP4 + TEX_stock_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				xx, yy+1, texC->size[0], texC->size[1]);
		}
		else {
			texC = st->texP4 + TEX_ATTACK_P;
			texA = st->texP4 + TEX_ATTACK_P_ALPHA;
			j = _tbl[playerCount - 1][(playerNo + i + 1) % 4];
			tiStretchAlphaTexItem(&gp, texC, texA, false,
				11, j, xx, yy, texC->size[0], texC->size[1] / 11);
		}
	}

	*gpp = gp;
}

//## �Q�[���`��֐��}�b�v�`��
static void dm_map_draw(game_map *map, u8 col_no, s16 x_p, s16 y_p, s8 size)
{
	int i;

	for(i = 0; i < GAME_MAP_W * (GAME_MAP_H + 1); i++) {
		// �E�C���X(�J�v�Z��)�̕\���t���O���n�m�̂Ƃ�
		// �F�i�q�E�a�E�x�j���������Ƃ�
		if(map[i].capsel_m_flg[cap_disp_flg]
		&& map[i].capsel_m_p == col_no)
		{
			gSPTextureRectangle(gp++,
				map[i].pos_m_x * size + x_p << 2,
				map[i].pos_m_y * size + y_p << 2,
				map[i].pos_m_x * size + x_p + size << 2,
				map[i].pos_m_y * size + y_p + size << 2,
				G_TX_RENDERTILE,
				0, map[i].capsel_m_g * size << 5,
				1 << 10, 1 << 10);
		}
	}
}

//## �J�v�Z���̗����n�_������
static void dm_find_fall_point(
	game_map *map,
	game_cap *cap,
	int       fallPosY[2])
{
	int i, y, minY = 16;

	for(i = 0; i < 2; i++) {
		for(y = MAX(1, cap->pos_y[i]); y <= 16; y++) {
			if(get_map_info(map, cap->pos_x[i], y)) {
				minY = MIN(minY, y - 1);
				break;
			}
		}
	}

	fallPosY[0] = minY - (cap->pos_y[0] < cap->pos_y[1] ? 1 : 0);
	fallPosY[1] = minY - (cap->pos_y[0] > cap->pos_y[1] ? 1 : 0);
}

//## �E�B���X����`��
static void draw_virus_number(Gfx **gpp, u32 number, int x, int y, float sx, float sy)
{
	static const char _tbl[] = { 9,0,1,2,3,4,5,6,7,8 };
	Game *st = watchGame;
	STexInfo *texC = st->texAL + TEX_AL_VIRUS_NUMBER;
	STexInfo *texA = st->texAL + TEX_AL_VIRUS_NUMBER_ALPHA;
	int i, xx, yy, tmp[16], column = 0;
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 10;

	do {
		tmp[column++] = number % 10;
		number /= 10;
	} while(number);

	xx = column * -7;
	yy = height / -2;

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * _tbl[tmp[i]] * 2,
			texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * _tbl[tmp[i]] / 2,
			texA->size[0],
			x + xx * sx, y + yy * sx, width * sx, height * sx);
		xx += 14;
	}
}

//## �X�R�A�\���p�̐�����`��
static void draw_count_number(Gfx **gpp, int color, int column, u32 number, int x, int y)
{
	static const int _tex[] = { TEX_count_num, TEX_count_1p_num, TEX_count_2p_num };
	static const int _row[] = { 13, 12, 12 };
	Game *st = watchGame;
	STexInfo *texC = st->texP1 + _tex[color];
	STexInfo *texA = st->texP1 + TEX_count_num_alpha;
	int i, tmp[16];
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / _row[color];

	for(i = 0; i < column; i++) {
		tmp[i] = number % 10;
		number /= 10;
	}

	switch(column) {
	case -1:
		tmp[0] = 10;
		column = 1;
		break;
	case -2:
		tmp[0] = 11;
		column = 1;
		break;
	case -3:
		tmp[0] = 12;
		column = 1;
		break;
	}

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * tmp[i] * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * tmp[i] / 2, texA->size[0],
			x, y, width, height);
		x += 9;
	}
}

#if 0
/*
//## ���ԕ\���p�̐�����`��
static void draw_time_number(Gfx **gpp, u32 number, int column, int x, int y)
{
	Game *st = watchGame;
	STexInfo *texC = st->texP1 + TEX_P1_VS_TIME_NUMBER;
	STexInfo *texA = st->texP1 + TEX_P1_VS_TIME_NUMBER_ALPHA;
	int i, tmp[16];
	int width = MIN(texC->size[0], texA->size[0]);
	int height = texC->size[1] / 12;

	for(i = 0; i < column; i++) {
		tmp[i] = number % 10 + 2;
		number /= 10;
	}

	switch(column) {
	case -1:
		tmp[0] = 0;
		column = 1;
		break;
	case -2:
		tmp[0] = 1;
		column = 1;
		break;
	}

	for(i = column - 1; i >= 0; i--) {
		StretchAlphaTexBlock(gpp,
			width, height,
			(u8 *)texC->addr[1] + texC->size[0] * height * tmp[i] * 2, texC->size[0],
			(u8 *)texA->addr[1] + texA->size[0] * height * tmp[i] / 2, texA->size[0],
			x, y, width, height);
		x += 10;
	}
}
*/
#endif

//## ���Ԃ�\��
static void draw_time(Gfx **gpp, u32 time, int x, int y)
{
	static const u8 _pos[] = { 0, 17, 26 };
	static const s8 _col[] = { 2, -3, 2 };
	int i, t[3];
	u32 m, s;

	time /= FRAME_PAR_SEC;
	t[2] = time % 60;

	time /= 60;
	t[0] =  time % 100;

	t[1] = 0;

	for(i = 0; i < 3; i++) {
		draw_count_number(gpp, 0, _col[i], t[i], x + _pos[i], y);
	}
}

//## ���Ԃ�\��(�~���b�P��)
static void draw_time2(Gfx **gpp, u32 time, int x, int y)
{
	static const u8 _pos[] = { 0, 6, 13, 28, 35 };
	static const s8 _col[] = { 1, -3, 2, -3, 1 };
	int i, t[5];

	t[1] = t[3] = 0;

	time /= FRAME_PAR_MSEC;
	t[4] = time % 10;

	time /= 10;
	t[2] = time % 60;

	time /= 60;
	t[0] = time % 60;

	for(i = 0; i < 5; i++) {
		draw_count_number(gpp, 0, _col[i], t[i], x + _pos[i], y);
	}
}

//## push_any_key ��`��
void push_any_key_draw(int x_pos, int y_pos)
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	int width, alpha;

	alpha = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha = CLAMP(0, 255, alpha);

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	texC = st->texAL + TEX_AL_INFO;
	texA = st->texAL + TEX_AL_INFO_ALPHA;
	width = MIN(texC->size[0], texA->size[0]);

	StretchAlphaTexTile(&gp, width, texC->size[1],
		texC->addr[1], texC->size[0],
		texA->addr[1], texA->size[0],
		0, 0, width, texC->size[1],
		x_pos, y_pos, width, texC->size[1]);
}

//## �f�����S��\��
static void draw_demo_logo(Gfx **gpp, int x, int y)
{
	static const int _tex[][2] = {
		{ TEX_AL_TITLE_01, TEX_AL_TITLE_01_ALPHA },
		{ TEX_AL_TITLE_02, TEX_AL_TITLE_02_ALPHA },
		{ TEX_AL_LOGO, TEX_AL_LOGO_ALPHA },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width, height, alpha[3];

	alpha[0] = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha[0] = CLAMP(0, 255, alpha[0]);
	alpha[1] = 255 - alpha[0];
	alpha[2] = 255;

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(_tex); i++) {
		if(i == 2) {
			x = 14;
			y = 18;
		}
		texC = st->texAL + _tex[i][0];
		texA = st->texAL + _tex[i][1];
		width = MIN(texC->size[0], texA->size[0]);
		height = MIN(texC->size[1], texA->size[1]);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha[i]);
		StretchAlphaTexTile(&gp, width, height,
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			0, 0, width, height, x, y, width, height);
	}

	*gpp = gp;
}

//## ���v���C���S��\��
static void draw_replay_logo(Gfx **gpp, int x, int y)
{
	static const int _tex[][2] = {
		{ TEX_AL_INFO, TEX_AL_INFO_ALPHA },
		{ TEX_AL_REPLAY, TEX_AL_REPLAY_ALPHA },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width, height, alpha[2];

	alpha[0] = sins(st->blink_count << 10) * (255.f / 32768.f) + 127.f;
	alpha[0] = CLAMP(0, 255, alpha[0]);
	alpha[1] = 255 - alpha[0];

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);

	for(i = 0; i < ARRAY_SIZE(_tex); i++) {
		texC = st->texAL + _tex[i][0];
		texA = st->texAL + _tex[i][1];
		width = MIN(texC->size[0], texA->size[0]);
		height = MIN(texC->size[1], texA->size[1]);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha[i]);
		StretchAlphaTexTile(&gp, width, height,
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			0, 0, width, height, x, y, width, height);
	}

	*gpp = gp;
}

//-------------------------------------------------------------------------
//## �R�C���̏�����
//		int count	���߂Ɏ����Ă���R�C���̐�
static void _init_coin_logo(int count)
{
	Game *st = watchGame;
	int		i;

	st->coin_count = count;
	for ( i = 0; i < 4; i++ ) {
		if ( i < count ) {
			st->coin_time[i] = 0;		// �L��
		} else {
			st->coin_time[i] = -1;		// ����
		}
	}
}

//-------------------------------------------------------------------------
//## �R�C���̕\��
//		int count	�\������R�C����
static void _disp_coin_logo(Gfx **glp, int count)
{
	enum {
		COIN_XPOS = 237,
		COIN_YPOS =  81,
		COIN_XINC =  16,
		COIN_TIME =  30,
	};

	Game *st = watchGame;
	Gfx			*pgfx;
	STexInfo	*texC;
	int			i, a, tileW;
	float		fx, fy;
	static u32	texofs[] = { 0, 17, 34, 51 };

	pgfx = *glp;

	if ( st->coin_count < count ) {
		// �R�C����������
		for ( i = st->coin_count; i < count; i++ ) {
			if ( st->coin_time[i] == 0 ) {
				st->coin_time[i] = 0;
				st->coin_count++;
			} else {
				if ( st->coin_time[i] < 0 ) {
					st->coin_time[i] = COIN_TIME;
				} else {
					st->coin_time[i]--;
				}
			}
		}
	} else if ( st->coin_count > count ) {
		// �R�C����������
		for ( i = st->coin_count - 1; i >= count; i-- ) {
			if ( st->coin_time[i] >= COIN_TIME ) {
				st->coin_count--;
				st->coin_time[i] = -1;
			} else {
				st->coin_time[i]++;
			}
		}
		count = st->coin_count;
	}

	// �ݒ�
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(pgfx++,
		0,0,0,TEXEL0,   PRIMITIVE,0,TEXEL0,0,
		0,0,0,TEXEL0,   PRIMITIVE,0,TEXEL0,0);

	// �R�C���̕\��
	texC = st->texLS + TEX_coin_00;
	tileW = texC->size[0] / 4;
	for ( i = 0; i < count; i++ ) {
		fx = (float)( COIN_XPOS + COIN_XINC * i );
		fy = (float)( st->coin_time[i] - COIN_TIME/2 );
		fy = COIN_YPOS + ( fy * fy ) * 0.125 - ( (COIN_TIME/2) * (COIN_TIME/2) ) * 0.125;
		a = 255 - ( ( 255 * st->coin_time[i] ) / COIN_TIME );
		gDPSetPrimColor(pgfx++, 0, 0, 255, 255, 255, a);
		StretchTexTile4(&pgfx,
			texC->size[0], texC->size[1],
			texC->addr[0], texC->addr[1],
			tileW * ((st->coin_time[i] >> 1) & 0x03), 0, tileW, texC->size[1],
			fx, fy, tileW, texC->size[1]);
	}

	*glp = pgfx;
}

//## �t���b�V���E�B���X�̌����P�`��
static void draw_flash_virus_light(Gfx **gpp, bool cached, int x, int y, int color)
{
	static u8 tbl[] = { 0, 1, 2, 1 };
	static u8 col[][4] = {
		{255,   0,  80, 255},// Red
		{255, 200,   0, 255},// Yellow
		{100, 100, 255, 255},// Blue
	};
	Gfx *gp = *gpp;
	Game *st = watchGame;
	STexInfo *tex = st->texItem + TEX_F1 + tbl[(st->blink_count >> 1) & 3];

	if(!cached) {
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineLERP(gp++,
			ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
			ENVIRONMENT, 0, TEXEL0, 0, 
			ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE,
			ENVIRONMENT, 0, TEXEL0, 0);
		gDPSetTextureLUT(gp++, G_TT_NONE);
		gDPSetEnvColor(gp++, 255, 255, 255, 255);
	}

	gDPSetPrimColor(gp++, 0, 0, col[color][0], col[color][1], col[color][2], col[color][3]);
	tiStretchTexBlock(&gp, tex, cached, x, y, 20, 20);

	*gpp = gp;
}

//## �t���b�V���E�B���X�̌���`��
static void draw_flash_virus_lights(Gfx **gpp, game_state *state, game_map *map)
{
	Game *st = watchGame;
	bool cached = false;
	int i, dx, dy;

	switch(state->map_item_size) {
	case cap_size_8:
		dx = dy = -6;
		break;
	case cap_size_10:
		dx = dy = -5;
		break;
	}

	for(i = 0; i < state->flash_virus_count; i++) {

		// (��\�� || ���ŏ�� || �E�B���X�ł͂Ȃ�) �̏ꍇ
		if(state->flash_virus_pos[i][2] < 0) {
			continue;
		}

		draw_flash_virus_light(gpp, cached,
			dx + state->map_x + state->map_item_size *  state->flash_virus_pos[i][0],
			dy + state->map_y + state->map_item_size * (state->flash_virus_pos[i][1] + 1),
			state->flash_virus_pos[i][2]);

		cached = true;
	}
}

//## �^�C���A�^�b�N�̌��ʕ\�� : �`��
static void timeAttackResult_draw(_TimeAttackResult *st, Gfx **gpp, int x, int y)
{
	Game *g = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i;

	gSPDisplayList(gp++, alpha_texture_init_dl);

	// ���n
	texC = g->texP1 + TEX_totalscore_panel;
	texA = g->texP1 + TEX_totalscore_panel_alpha;
	StretchAlphaTexTile(&gp, texC->size[0], texC->size[1],
		texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
		0, 0, texC->size[0], texC->size[1],
		x, y, texC->size[0], texC->size[1]);

	// �c�莞��
	draw_time2(&gp, st->time + FRAME_PAR_MSEC - 1, x + 18, y + 14);

	// �A����
	draw_count_number(&gp, 0, 2, st->combo, x + 31, y + 35);

	// �������E�B���X
	draw_count_number(&gp, 0, 2, st->virus, x + 31, y + 56);

	// �g�[�^���X�R�A
	draw_count_number(&gp, 0, 7, st->score, x + 9, y + 80);

	*gpp = gp;
}

//## �X�g�[���[���[�h�̃p�l����`��
static void draw_story_board(Gfx **gpp, int x, int y, bool visBoard, bool visScore)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	Gfx *obj;
	STexInfo *texC, *texA;

	gSPDisplayList(gp++, alpha_texture_init_dl);

	if(visBoard) {
		// �p�l��
		texC = st->texP2 + TEX_top_ohanasi_panel;
		texA = st->texP2 + TEX_top_ohanasi_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texA->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			x + 120, y + 11, texC->size[0], texC->size[1]);

		// NORMAL or HARD
		texC = st->texP2 + TEX_top_gamelv_contents;
		texA = st->texP2 + TEX_top_gamelv_contents_alpha;
		tiStretchAlphaTexItem(&gp, texC, texA, false, 4, evs_story_level,
			x + 142, y + 54, texC->size[0], texC->size[1] / 4);
	}

	if(visScore) {
		// �X�R�A
		draw_count_number(&gp, 0, 7, game_state_data[0].game_score, x + 129, y + 25);
	}

	*gpp = gp;
}

//## �ΐ��ʂ̃p�l����`��
static void draw_vsmode_board(Gfx **gpp, int x, int y, bool visBoard, bool visScore)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, pos[2][2];

	gSPDisplayList(gp++, alpha_texture_init_dl);

	if(visBoard) {
		switch(evs_gamemode) {
		// GAME LV
		case GMD_FLASH:
		case GMD_TIME_ATTACK:
			texC = st->texP2 + TEX_top_gamelv_panel;
			texA = st->texP2 + TEX_top_gamelv_panel_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 95, y + 9, texC->size[0], texC->size[1]);

			texC = st->texP2 + TEX_top_gamelv_contents;
			texA = st->texP2 + TEX_top_gamelv_contents_alpha;
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 99, 184 };
				tiStretchAlphaTexItem(&gp, texC, texA, false, 4,
					game_state_data[i].game_level,
					x + _x[i], y + 11, texC->size[0], texC->size[1] / 4);
			}
			break;

		// VIRUS LV
		default:
			texC = st->texP2 + TEX_top_viruslv_panel;
			texA = st->texP2 + TEX_top_viruslv_panel_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 104, y + 9, texC->size[0], texC->size[1]);

			for(i = 0; i < 2; i++) {
				static const int _x[] = { 113, 189 };
				draw_count_number(&gp, 0, 2, game_state_data[i].virus_level, x + _x[i], y + 11);
			}
			break;
		}

		// SPEED
		texC = st->texP2 + TEX_top_speed_panel;
		texA = st->texP2 + TEX_top_speed_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			x + 104, y + 25, texC->size[0], texC->size[1]);

		texC = st->texP2 + TEX_top_speed_contents;
		texA = st->texP2 + TEX_top_speed_contents_alpha;
		for(i = 0; i < 2; i++) {
			static const int _x[] = { 108, 184 };
			tiStretchAlphaTexItem(&gp, texC, texA, false, 3,
				2 - game_state_data[i].cap_def_speed,
				x + _x[i], y + 27, texC->size[0], texC->size[1] / 3);
		}
	}

	switch(evs_gamesel) {
	// SUCORE xxxxxxx
	case GSL_VSCPU:
		if(visBoard) {
			texC = st->texP2 + TEX_top_score_panel_1p;
			texA = st->texP2 + TEX_top_score_panel_1p_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 112, y + 41, texC->size[0], texC->size[1]);
		}
		if(visScore) {
			draw_count_number(&gp, 0, 7, game_state_data[0].game_score, x + 144, y + 42);
		}
		break;

	// xxxxxxx SCORE xxxxxxx
	default:
		if(visBoard) {
			texC = st->texP2 + TEX_top_score_panel_2p;
			texA = st->texP2 + TEX_top_score_panel_2p_alpha;
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
				x + 88, y + 41, texC->size[0], texC->size[1]);
		}
		if(visScore) {
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 94, 162 };
				draw_count_number(&gp, i + 1, 7, game_state_data[i].game_score, x + _x[i], y + 42);
			}
		}
		break;
	}

	*gpp = gp;
}

//## �r��`��
static void _draw_bottle_10(Gfx **gpp, const int *xx, const int *yy, int count)
{
	static const int _rect[][4] = {
		{ 0,   0, 96,  20 },
		{ 0,  20, 96,  20 },
		{ 0,  40,  8, 160 }, { 88, 40, 8, 160 },
		{ 0, 200, 96,   8 },
	};
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int h, i, j;

	// �������`��̏���
	gSPDisplayList(gp++, normal_texture_init_dl);
	gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
	gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA);

	// �r�̉e��`��
	texC = st->texP1 + TEX_P1_BOTTLE_SHADOW;
	for(i = 0; i < texC->size[1]; i += 42) {
		for(j = 0; j < count; j++) {
			h = MIN(42, texC->size[1] - i);
			tiStretchTexTile(&gp, texC, j,
				0, i, texC->size[0], h,
				xx[j], yy[j] + i, texC->size[0], h);
		}
	}

	gSPDisplayList(gp++, normal_texture_init_dl);

	// �r��`��
	texC = st->texP1 + TEX_P1_BOTTLE;
	for(i = 0; i < ARRAY_SIZE(_rect); i++) {
		for(j = 0; j < count; j++) {
			tiStretchTexTile(&gp, texC, j,
				_rect[i][0], _rect[i][1], _rect[i][2], _rect[i][3],
				xx[j] + _rect[i][0], yy[j] + _rect[i][1], _rect[i][2], _rect[i][3]);
		}
	}

	*gpp = gp;
}
void dm_calc_bottle_2p() {
	Game *st = watchGame;
	int i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_WD / 2 / FRAME_MOVE_MAX;
	int mx[] = { dm_wold_x_vs_1p-i, dm_wold_x_vs_2p+i };

	for(i = 0; i < 2; i++) {
		game_state *state = &game_state_data[i];
		state->map_x = mx[i];     // ��w���W
		state->map_y = dm_wold_y; // ��x���W
	}
}
void dm_draw_bottle_2p(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	int i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_WD / 2 / FRAME_MOVE_MAX;
	int pos[] = { -i, i };
	int x[2], y[2];

	for(i = 0; i < 2; i++) {
		game_state *state = &game_state_data[i];
		x[i] = state->map_x - 8;
		y[i] = state->map_y - 30;
	}

	_draw_bottle_10(&gp, x, y, 2);

	*gpp = gp;
}

//## �E�B���X�L������`��
void dm_draw_big_virus(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	int i;

	for(i = 0; i < 3; i++) {
		animeState_initDL2(&st->virus_anime_state[i], &gp);

		animeState_draw(&st->virus_anime_state[i], &gp,
			st->big_virus_pos[i][0], st->big_virus_pos[i][1],
			st->big_virus_scale[i], st->big_virus_scale[i]);

		animeSmog_draw(&st->virus_smog_state[i], &gp,
			st->big_virus_pos[i][0], st->big_virus_pos[i][1],
			st->big_virus_scale[i], st->big_virus_scale[i]);
	}

	*gpp = gp;
}

//## �P�ۂ�`��
void dm_draw_KaSaMaRu(Gfx **gpp, Mtx **mpp, Vtx **vpp,
	int speaking, int x, int y, int dir, int alpha)
{
	static const int _pat[] = { 0, 1, 2, 3, 2, 1 };
	Game *st = watchGame;

	Gfx *gp = *gpp;
	Mtx *mp = *mpp;
	Vtx *vp = *vpp;

	STexInfo *texC, *texA;
	int i, width, height;
	float mf[4][4], angle;

	// ���n��
	guOrtho(mp, 0, SCREEN_WD, SCREEN_HT, 0, 1, 10, 1);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
	mp++;

	guTranslate(mp, 0, 0, -5);
	gSPMatrix(gp++, mp, G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
	mp++;

	// �h��
	angle = WrapF(0, 1, st->blink_count * (1.0 / 128.0)) * M_PI * 2;
	guRotateRPYF(mf, 0, (1 - dir) * 90, sinf(angle) * 4 * dir);

	// �P��
	i = WrapI(0, ARRAY_SIZE(_pat), ((st->blink_count & 127) * ARRAY_SIZE(_pat) * 12) >> 7);
	if(!speaking) i = 0;

	texC = st->texKaSa + TEX_kasamaru01 + _pat[i];
	texA = st->texKaSa + TEX_kasamaru_alpha;
	width = MIN(texC->size[0], texA->size[0]);
	height = MIN(texC->size[1], texA->size[1]);

	gSPDisplayList(gp++, alpha_texture_init_dl);
	gSPClearGeometryMode(gp++, 0|G_ZBUFFER|G_SHADE|G_SHADING_SMOOTH|G_CULL_FRONT|G_CULL_BACK|G_LIGHTING|G_FOG|G_TEXTURE_GEN|G_TEXTURE_GEN_LINEAR|G_CLIPPING|G_LOD|G_TEXTURE_ENABLE);
	gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
	gDPSetTexturePersp(gp++, G_TP_NONE);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, alpha);

	mf[3][0] = (dir > 0) ? x : x + width;
	mf[3][1] = y;
	guMtxF2L(mf, mp);
	gSPMatrix(gp++, mp, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
	mp++;

	RectAlphaTexTile(&gp, &vp, width, height,
		texC->addr[1], texC->size[0],
		texA->addr[1], texA->size[0],
		0, 0, width, height,
		0, 0, width, height);

	// ����
//	if(tutolWnd_isSpeaking(st->msgWnd) && (st->animeCount & 4)) {
//		gSPDisplayList(gp++, normal_texture_init_dl);
//
//		texC = st->texKaSa + TEX_kasamaru_mouth;
//		RectTexTile8(&gp, &vp,
//			texC->size[0], texC->size[1],
//			texC->addr[0], texC->addr[1],
//			0, 0, texC->size[0], texC->size[1],
//			24, 61, texC->size[0], texC->size[1]);
//	}

	// �Y
	*vpp = vp;
	*mpp = mp;
	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### �`�惁�C��

//## �Q�[���`��֐�(1P, ���l�� ����)
static void dm_game_graphic_common(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	STexInfo *tex;
	int i, size_flg = 0;

	if(state->map_item_size == cap_size_8) {
		size_flg = 1;
	}

	gSPDisplayList(gp++, normal_texture_init_dl);

	// �E�C���X�O���t�B�b�N�f�[�^�̓Ǎ���
	tex = dm_game_get_capsel_tex(size_flg);
	load_TexTile_4b(tex->addr[1],
		tex->size[0], tex->size[1], 0, 0,
		tex->size[0] - 1, tex->size[1] - 1);

	// �V�U�[
	gfxSetScissor(&gp, SCISSOR_FLAG,
		state->map_x, state->map_y + state->map_item_size,
		state->map_item_size * GAME_MAP_W,
		state->map_item_size * GAME_MAP_H);

	// �E�C���X�`��
	for(i = 0; i < 6; i++) {
		// �E�C���X�p���b�g�f�[�^�̓Ǎ���
		tex = dm_game_get_capsel_pal(size_flg, i);
		load_TexPal(tex->addr[0]);
		dm_map_draw(map, i, state->map_x, state->map_y - state->bottom_up_scroll, state->map_item_size);
	}
	gDPPipeSync(gp++);

	// �V�U�[����
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);

	// �����n�_�ɃJ�v�Z����`��
	if(!PLAYER_IS_CPU(state, player_no) &&
		visible_fall_point[player_no] &&
		state->mode_now == dm_mode_down &&
		state->now_cap.pos_y[0] > 0 &&
		state->now_cap.capsel_flg[cap_disp_flg])
	{
		game_cap *cap = &state->now_cap;
		int fallPosY[2], color, x, y, size;

		dm_find_fall_point(map, cap, fallPosY);
		color = (fallPosY[0] - cap->pos_y[0]) << 3;

		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		_setDarkCapPrim(gp++);

		for(i = 0; i < 2; i++) {
			tex = dm_game_get_capsel_pal(size_flg, cap->capsel_p[i]);
			load_TexPal(tex->addr[0]);

			size = state->map_item_size;
			x = cap->pos_x[i] * size + state->map_x;
			y = fallPosY  [i] * size + state->map_y;

			gSPTextureRectangle(gp++, x<<2, y<<2,
				(x + size)<<2, (y + size)<<2,
				G_TX_RENDERTILE,
				0, cap->capsel_g[i] * size << 5,
				1 << 10, 1 << 10);
		}
		gDPSetPrimColor(gp++, 0, 0, 255, 255, 255, 255);
		gDPSetRenderMode(gp++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	}
}

//## �Q�[���`��֐�
void dm_game_graphic_p(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	int i, xx[2], yy[2];
	int size_flg = 0;
	STexInfo *tex;

	// �|�[�Y���͕`�悵�Ȃ�
	if(state->cnd_static == dm_cnd_pause) {
		return;
	}

	if(state->map_item_size == cap_size_8) {
		size_flg = 1;
	}

	// 
	dm_game_graphic_common(state, player_no, map);
	gSPDisplayList(gp++, normal_texture_init_dl);

	// ����J�v�Z���`��
	if(dm_calc_capsel_pos(state, xx, yy)) {
		// �f���� || �J�v�Z�������҂� || �Q�[���̌������t����
		if(st->demo_flag || state->now_cap.pos_y[0] < 1 ||
			state->cnd_now != dm_cnd_pause && state->cnd_now != dm_cnd_wait)
		{
			// �J�v�Z����`��
			dm_draw_capsel_by_gfx(state, xx, yy);
		}
	}

	// NEXT�J�v�Z���`��
	if(state->next_cap.capsel_flg[cap_disp_flg]) {
		// �\���t���O�������Ă����ꍇ
		if(state->now_cap.pos_y[0] > 0) {
			// �������Ă���
			for(i = 0; i < 2; i++) {
				// �p���b�g���[�h
				tex = dm_game_get_capsel_pal(size_flg, state->next_cap.capsel_p[i]);
				load_TexPal(tex->addr[0]);

				// �J�v�Z���`��
				draw_Tex(
					(state->next_cap.pos_x[i] * state->map_item_size) + state->map_x,
					(state->next_cap.pos_y[i] * state->map_item_size) + state->map_y - 10,
					state->map_item_size, state->map_item_size,
					0, state->next_cap.capsel_g[i] * state->map_item_size);
			}
		}
	}
}

//## �Q�[���`��֐�(1P�p)
static void dm_game_graphic_1p(game_state *state, int player_no, game_map *map)
{
	Game *st = watchGame;
	int i, xx[2], yy[2];
	STexInfo *tex;

	// �|�[�Y���͕`�悵�Ȃ�
	if(state->cnd_static == dm_cnd_pause) {
		return;
	}

	// 
	dm_game_graphic_common(state, player_no, map);
	gSPDisplayList(gp++, normal_texture_init_dl);

	// ����J�v�Z���`��
	if(dm_calc_capsel_pos(state, xx, yy)) {
		i = 0;

		// �J�v�Z���z�u�� || ���ڂ̃J�v�Z�������҂�
		if(state->mode_now == dm_mode_init || state->mode_now == dm_mode_wait) {
			xx[1] -= xx[0];
			yy[1] -= yy[0];
			xx[1] += (xx[0] = MARIO_THROW_X);
			yy[1] += (yy[0] = MARIO_THROW_Y);
			i++;
		}
		// �f����
		else if(st->demo_flag) {
			i++;
		}

		// �J�v�Z����`��
		if(i) {
			dm_draw_capsel_by_gfx(state, xx, yy);
		}
	}

	// NEXT�J�v�Z���`��
		// �\���t���O�������Ă����ꍇ
			// �������Ă���
	if(state->next_cap.capsel_flg[cap_disp_flg] &&
		state->now_cap.pos_y[0] > 0 && state->cnd_static == dm_cnd_wait)
	{
		for(i = 0; i < 2; i++) {
			// �p���b�g���[�h
			tex = dm_game_get_capsel_pal(0, state->next_cap.capsel_p[i]);
			load_TexPal(tex->addr[0]);

			// �J�v�Z���`��
			draw_Tex(i * cap_size_10 + MARIO_THROW_X, MARIO_THROW_Y,
				cap_size_10, cap_size_10, 0,
				state->next_cap.capsel_g[i] * cap_size_10);
		}
	}
}

//## �Q�[���`��֐�(���o�֌W�p)
void dm_game_graphic_effect(game_state *state, int player_no, int type)
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	int i;

	// �t���b�V���E�B���X��`��
	switch(state->cnd_now) {
	case dm_cnd_wait:
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
		if(evs_gamemode == GMD_FLASH) {
			draw_flash_virus_lights(&gp, state, game_map_data[player_no]);
		}
		break;
	}

	// �X�R�A�G�t�F�N�g��`��
	if(evs_score_flag) {
		scoreNums_draw(&st->scoreNums[player_no], &gp);
	}

	switch(state->cnd_training) {
	case dm_cnd_training:		// ���K��
		if(state->cnd_static != dm_cnd_wait) {
			break;
		}
		texC = st->texP4 + TEX_PRACTICES;
		texA = st->texP4 + TEX_PRACTICES_ALPHA;
		gSPDisplayList(gp++, alpha_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
		i = sins(st->blink_count << 9) * (127.f / 32768.f) + 127.f;
		gDPSetPrimColor(gp++, 0,0, 255,255,255, i);
		i = sins(st->blink_count << 10) * (5.f / 32768.f);
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0],
			texA->addr[1], texA->size[0],
			state->map_x, state->map_y + i + 160,
			texC->size[0], texC->size[1]);
		break;
	}

	// TRY NEXT STATE ��`��
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
		switch(evs_gamemode) {
		case GMD_TIME_ATTACK:
			break;
		case GMD_TaiQ:
			disp_clear_logo(&gp, player_no, 0);
			break;
		default:
			disp_clear_logo(&gp, player_no, 1);
			break;
		}
		break;
	}

	// ALL CLEAR ��`��
	switch(state->cnd_now) {
	case dm_cnd_clear_wait:
		disp_allclear_logo(&gp, player_no, 0);
		break;
	}

	// GAME OVER ��`��
	switch(state->cnd_now) {
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_gover_wait:
		if(state->cnd_now == dm_cnd_game_over &&
			evs_gamemode == GMD_TIME_ATTACK)
		{
			break;
		}

		if(evs_gamemode == GMD_TIME_ATTACK &&
			evs_game_time >= SCORE_ATTACK_TIME_LIMIT)
		{
			disp_timeover_logo(&gp, player_no);
		}
		else {
			disp_gameover_logo(&gp, player_no);
		}
		break;
	}

	// ���^�C�A��`��
	switch(state->cnd_now) {
	case dm_cnd_retire:   // ���^�C�A
	case dm_cnd_tr_chack: // ���K�m�F��
		// [START BUTTON �ŗ��K�J�n] ��`��
		if(!st->replayFlag &&
			state->player_type == PUF_PlayerMAN)
		{
			i = sins(st->blink_count << 10) * (127.f / 32768.f) + 127.f;
			texC = st->texP4 + TEX_PRACTICE;
			texA = st->texP4 + TEX_PRACTICE_ALPHA;
			gSPDisplayList(gp++, alpha_texture_init_dl);
			gDPSetCombineMode(gp++, G_CC_ALPHATEXTURE_PRIM, G_CC_PASS2);
			gDPSetPrimColor(gp++, 0,0, 255,255,255, i);
			StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				state->map_x, state->map_y + 92,
				texC->size[0], texC->size[1]);
		}

		// ���^�C�A���S
		disp_retire_logo(&gp, player_no);
		break;

	case dm_cnd_retire_wait:
		disp_retire_logo(&gp, player_no);
		break;
	}

	// �^�C���A�^�b�N�̌��ʂ�\��
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
	case dm_cnd_clear_result:
	case dm_cnd_gover_result:
	case dm_cnd_retire_result:
		if(evs_gamemode == GMD_TIME_ATTACK) {
			int y = (evs_gamesel == GSL_1PLAY) ? 10 : 74;
			timeAttackResult_draw(&st->timeAttackResult[player_no], &gp,
				state->map_x, state->map_y + y);
		}
		break;
	}

	// WIN ��`��
	switch(state->cnd_now) {
	case dm_cnd_win:			// WIN
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
		disp_win_logo(&gp, player_no);
		break;
	}

	// LOSE ��`��
	switch(state->cnd_now) {
	case dm_cnd_lose:			// LOSE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
		disp_lose_logo(&gp, player_no);
		break;
	}

	// DRAW ��`��
	switch(state->cnd_now) {
	case dm_cnd_draw:			// DRAW
	case dm_cnd_draw_retry:		// DRAW & RETRY
		disp_draw_logo(&gp, player_no);
		break;
	}

	// �J�E���g�_�E����`��
	switch(state->cnd_now) {
	case dm_cnd_init:
	case dm_cnd_wait: {
		int frame = st->count_down_ctrl / evs_playcnt;
		int sound = frame / 48;

		if(st->count_down_ctrl >= 0) {
			if(disp_count_logo(&gp, player_no, frame)) {
				st->count_down_ctrl = -1;

				// �J�E���g�_�E���I��SE���Đ�
				if(player_no == 0) {
					dm_snd_play_in_game(SE_gCountEnd);
				}
			}
			else {
				if(player_no == 0 && frame % 48 == 20) {
					if(sound < 3) {
						// �J�E���g�_�E��SE���Đ�
						dm_snd_play_in_game(SE_gCount1);
					}
				}
				st->count_down_ctrl++;
			}
		}
		} break;
	}

	// PUSH ANY KEY ��`��
	switch(state->cnd_now) {
	case dm_cnd_clear_wait:
	case dm_cnd_gover_wait:
		if(evs_gamesel == GSL_1PLAY && evs_gamemode == GMD_TIME_ATTACK) {
			if(st->effect_timer[player_no] == 0) {
				push_any_key_draw(state->map_x + 8, state->map_y + 160);
			}
		}
		break;
	}

	// PAUSE ��`��
	switch(state->cnd_now) {
	case dm_cnd_pause:			// PAUSE
		retryMenu_drawPause(player_no, &gp, false);
		break;

	case dm_cnd_pause_re:		// PAUSE & RETRRY
	case dm_cnd_pause_re_sc:	// PAUSE & RETRRY & SCORE
		retryMenu_drawPause(player_no, &gp, true);
		break;
	}

	// 4P,PAUSE ���̍U���Ώۃp�l��
	switch(state->cnd_now) {
	case dm_cnd_pause:			// PAUSE
	case dm_cnd_pause_re:		// PAUSE & RETRRY
	case dm_cnd_pause_re_sc:	// PAUSE & RETRRY & SCORE
		if(evs_gamesel == GSL_4PLAY) {
			draw_4p_attack_guide_panel(&gp,
				st->vs_4p_player_count, player_no,
				state->map_x, state->map_y - 36);
		}
		break;
	}

	// [������][���g���C][�I���] ��`��
	switch(state->cnd_now) {
	case dm_cnd_stage_clear:	// TRY NEXT STAGE
	case dm_cnd_game_over:		// GAME OVER
	case dm_cnd_win_retry:		// WIN & RETRY
	case dm_cnd_win_retry_sc:	// WIN & RETRY & SCORE
	case dm_cnd_lose_retry:		// LOSE & RETRY
	case dm_cnd_lose_retry_sc:	// LOSE & RETRY & SCORE
	case dm_cnd_draw_retry:		// DRAW & RETRY
		if(st->effect_timer[player_no] == 0) {
			retryMenu_drawContinue(player_no, &gp);
		}
		break;
	}

	// �`��͈͂�߂�
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);
}

//////////////////////////////////////////////////////////////////////////////
//{### �v�l

//## �Q�[���p�L�[���쐬�����ݒ�֐�
static void key_cntrol_init(void)
{
	int i;

	for(i = 0; i < 4; i++) {
		joyflg[i] =(0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT | DM_KEY_A | DM_KEY_B | DM_KEY_CU | DM_KEY_CD | DM_KEY_CL | DM_KEY_CR);
		joygmf[i] = TRUE;
		joygam[i] = 0x0000;
	}
	joycur1 = evs_keyrept[0];
	joycur2 = evs_keyrept[1];
}

//## �Q�[���p�L�[���쐬�֐�
void dm_make_key(void)
{
	Game *st = watchGame;
	int i, count;

	// joygam[x] �� �ްїp�� ���쐬
	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_VSCPU:
		count = 1;
		break;

	case GSL_2PLAY:
		count = 2;
		break;

	// 4PLAY & DEMO
	default:
		count = 4;
		break;
	}

	for(i = 0; i < count; i++) {
		if(game_state_data[i].player_type != PUF_PlayerMAN) {
			continue;
		}

		if(!joygmf[main_joy[i]]) {
			continue;
		}

		joygam[i] =
			(_getKeyTrg(i) & (DM_KEY_A | DM_KEY_B)) |
			(_getKeyRep(i) & (DM_KEY_LEFT | DM_KEY_RIGHT)) |
//			(_getKeyLvl(i) & (DM_KEY_L | DM_KEY_R));
			(_getKeyTrg(i) & (DM_KEY_L | DM_KEY_R));

		if(!(_getKeyLvl(i) & (DM_KEY_LEFT | DM_KEY_RIGHT))) {
			joygam[i] = joygam[i] | (_getKeyLvl(i) & DM_KEY_DOWN);
		}
	}
}

//## �J�v�Z������֐�
void key_control_main(game_state *state, game_map *map, int player_no, int joy_no)
{
	Game *st = watchGame;
	game_cap *cap;
	u32 joybak;
	int i, xx[2], yy[2];

	// �v���C���[���̃J�v�Z�������ʒuON/OFF ���Q�[���̃��[�N�ɔ��f
	load_visible_fall_point_flag();

	// �u�����n�_�ɃJ�v�Z����`��v�� ON/OFF
	if(!PLAYER_IS_CPU(state, player_no) && (joyupd[joy_no] & (DM_KEY_CU|DM_KEY_CD|DM_KEY_CL|DM_KEY_CR))) {
		visible_fall_point[player_no] = !visible_fall_point[player_no];
	}

	// �����ʒuON/OFF ���Z�[�u�f�[�^�ɔ��f
	save_visible_fall_point_flag();

	// ������
	if(state->mode_now == dm_mode_throw) {

		// �J�v�Z����`��
		if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
			dm_draw_capsel_by_cpu(state, xx, yy);
		}

		// ���۰ׂ������Ȃ����̏��� :����߰Ă𒼑O���ް��ɂ���
		if(state->player_type == PUF_PlayerMAN) {
			if(joynew[joy_no] & DM_KEY_RIGHT) {
				joyCursorFastSet(DM_KEY_RIGHT,joy_no);
			}
			if(joynew[joy_no] & DM_KEY_LEFT) {
				joyCursorFastSet(DM_KEY_LEFT, joy_no);
			}
		}
	}
	// ����J�v�Z������
	else if(state->mode_now == dm_mode_down) {

		// �ʏ���
		if(state->cnd_static == dm_cnd_wait) {

			// �������v�l�ǉ��ʒu
			if(PLAYER_IS_CPU(state, player_no)) {
				joybak = joygam[player_no];
				aifKeyOut(state);

				// ���v���C��
				if(st->replayFlag) {
					joygam[player_no] = joybak;
				}
			}

			cap = &state->now_cap;

			// ����]
			if(joygam[player_no] & DM_ROTATION_L) {
				rotate_capsel(map,cap,cap_turn_l);
			}
			// �E��]
			else if(joygam[player_no] & DM_ROTATION_R) {
				rotate_capsel(map,cap,cap_turn_r);
			}

			// ���ړ�
			if(joygam[player_no] & DM_KEY_LEFT) {
				translate_capsel(map,state,cap_turn_l,joy_no);
			}
			// �E�ړ�
			else if(joygam[player_no] & DM_KEY_RIGHT) {
				translate_capsel(map,state,cap_turn_r,joy_no);
			}

			state->cap_speed_vec = 1;

			// ��������
			if((joygam[player_no] & DM_KEY_DOWN) && cap->pos_y[0] > 0) {
				i = FallSpeed[state->cap_speed];
				i = (i >> 1) + (i & 1);
				state->cap_speed_vec = i;
			}
		}
		else {
			// ���۰ׂ������Ȃ����̏��� :����߰Ă𒼑O���ް��ɂ���
			if(state->player_type == PUF_PlayerMAN) {
				if(joynew[joy_no] & DM_KEY_RIGHT) {
					joyCursorFastSet(DM_KEY_RIGHT,joy_no);
				}
				if(joynew[joy_no] & DM_KEY_LEFT) {
					joyCursorFastSet(DM_KEY_LEFT, joy_no);
				}
			}
		}

		// �J�v�Z����`��
		if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
			dm_draw_capsel_by_cpu(state, xx, yy);
		}

		dm_capsel_down(state, map);

		st->force_draw_capsel_count[player_no] = 2;
	}
	else {
		if(st->force_draw_capsel_count[player_no]) {
			int bak = state->now_cap.capsel_flg[cap_disp_flg];
			state->now_cap.capsel_flg[cap_disp_flg] = cap_flg_on;

			// �J�v�Z����`��
			if(!st->demo_flag && dm_calc_capsel_pos(state, xx, yy)) {
				dm_draw_capsel_by_cpu(state, xx, yy);
			}

			state->now_cap.capsel_flg[cap_disp_flg] = bak;
			st->force_draw_capsel_count[player_no]--;
		}

		if(state->player_type == PUF_PlayerMAN) {
			joyCursorFastSet(DM_KEY_RIGHT,joy_no);
			joyCursorFastSet(DM_KEY_LEFT, joy_no);
		}
	}
}

//## �v�l����֐�
void make_ai_main()
{
	int i;

	if(!dm_think_flg) {
		return;
	}

	switch(evs_gamesel) {
	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		for(i = 0; i < 2; i++) {
			if(PLAYER_IS_CPU(&game_state_data[i], i)) {
				if(game_state_data[i].cnd_static == dm_cnd_wait) {
					aifMake(&game_state_data[i]);
				}
			}
		}
		break;
	case GSL_4PLAY:
	case GSL_4DEMO:
		for(i = 0; i < 4; i++) {
			if(PLAYER_IS_CPU(&game_state_data[i], i)) {
				if(game_state_data[i].cnd_static == dm_cnd_wait) {
					aifMake(&game_state_data[i]);
				}
			}
		}
		break;
	case GSL_1PLAY:
	case GSL_1DEMO:
		if(PLAYER_IS_CPU(&game_state_data[0], 0)) {
			aifMake(&game_state_data[0]);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### ��

//## ���ʌv�Z
void dm_effect_make(void)
{
	Game *st = watchGame;
	int i;

	// �_��
	st->blink_count++;

	// �r�A�p�l�����ړ�������Ƃ��̃J�E���^
	st->frame_move_count = CLAMP(0, FRAME_MOVE_MAX, st->frame_move_count +  st->frame_move_step);

	// �Q�[�����ԃJ�E���g���X�V
	for(i = 0; i < evs_playcnt; i++) {
		if(game_state_data[i].cnd_static == dm_cnd_wait) {
			if(st->started_game_flg && evs_game_time < DM_MAX_TIME) {
				evs_game_time++;
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### ������

#include "boot_data.h"

//## �q�[�v��������
void dm_game_init_heap()
{
	Game *st;
	int i;

	// �q�[�v�̐擪��ݒ�
	heapTop = (void *)HEAP_START;

	// �Q�[���\���̂�������
	st = watchGame = (Game *)ALIGN_16(heapTop);
	bzero(st, sizeof(Game));
	heapTop = watchGame + 1;

	// ���v���C���̃o�b�N�A�b�v�o�b�t�@���m��
	for(i = 0; i < ARRAY_SIZE(gameBackup); i++) {
		gameBackup[i] = (GameBackup *)ALIGN_16(heapTop);
		heapTop = gameBackup[i] + 1;
	}

	// �W�I���g���[
	gameGeom = (Geometry *)ALIGN_16(heapTop);
	heapTop = gameGeom + 1;
}

//## �Q�[���������֐�
void dm_game_init(int reinit)
{
	static const s16 map_x_table[][4] = {
		{dm_wold_x,       dm_wold_x,       dm_wold_x,       dm_wold_x      }, // 1P
		{dm_wold_x_vs_1p, dm_wold_x_vs_2p, dm_wold_x_vs_1p, dm_wold_x_vs_2p}, // 2P
		{dm_wold_x_4p_1p, dm_wold_x_4p_2p, dm_wold_x_4p_3p, dm_wold_x_4p_4p}, // 4P
	};
	static const u8 map_y_table[] = { dm_wold_y, dm_wold_y_4p };
	static const u8 size_table[] = { cap_size_10, cap_size_8 };
	Game *st = watchGame;
	game_state *state;
	int i, j, k;

	// �����_���V�[�h��ݒ�
	if(!reinit || !st->replayFlag) {
		st->replayFlag = 0;
		st->randSeed = (genrand(0xffff) + osGetTime()) * 0x00010001;
		st->randSeed2 = irandom() + osGetTime();
	}
	sgenrand(st->randSeed);
	randomseed(st->randSeed2);

	// �X�^�[��������
	if(!reinit) {
		// �X�^�[�`��ʒu�̊i�[�ꏊ
		for(i = 0; i < ARRAY_SIZE(st->star_pos_x); i++) {
			st->star_pos_x[i] = st->star_pos_y[i] = 0;
		}
		st->star_count = 0;

		// ����������
		starForce_init(&st->starForce, st->star_pos_x, st->star_pos_y);
	}

	// ���g���C�R�C��
	if(!reinit) {
		st->retry_coin = 0;
	}

	// �X�R�A
	for(i = 0; i < ARRAY_SIZE(st->scoreNums); i++) {
		scoreNums_init(&st->scoreNums[i]);
	}

	// ����J�v�Z�������`��J�E���^��������
	for(i = 0; i < ARRAY_SIZE(st->force_draw_capsel_count); i++) {
		st->force_draw_capsel_count[i] = 0;
	}

	// �J�[�e���̈ʒu��ݒ�
	st->curtain_count = CURTAIN_DOWN;
	st->curtain_step = -1;

	// ���s���̊�A�j���[�V�����J�E���^
	for(i = 0; i < ARRAY_SIZE(st->face_anime_count); i++) {
		st->face_anime_count[i] = 1;
	}

	// �J�E���g�_�E�������̐���
	st->count_down_ctrl = 0;

	// BGM�̑������P��ς��邽�߂̃t���O
	st->bgm_bpm_flg[0] = st->bgm_bpm_flg[1] = 0;

	// ���[�`����炷�t���O�̃N���A
	st->last_3_se_flg = 0;
	st->last_3_se_cnt = (evs_gamemode == GMD_FLASH) ? 1 : 3;

	// �^�C�����Z���J�n����t���O
	st->started_game_flg = (main_no == MAIN_GAME) ? 0 : 1;

	// ����E�B���X�̕`��ʒu
	for(i = 0; i < ARRAY_SIZE(st->big_virus_flg); i++) {
		// ���ŃA�j���[�V�����t���O
		st->big_virus_flg[i] = 0;

		// �ʒu��ݒ�
		st->big_virus_rot[i] = i * 120 + 1; // sin,cos�p���l
		st->big_virus_scale[i] = 1;
	}

	// �E�B���X�ړ��̃E�F�C�g&�^�C�}�[
	st->big_virus_wait = 10;
	st->big_virus_timer = 0;
	st->big_virus_no_wait = false;
	st->big_virus_stop_count = 0;
	st->big_virus_blink_count = 0;
	if(evs_gamemode == GMD_TaiQ) {
		st->big_virus_wait = _big_virus_def_wait[game_state_data[0].game_level];
	}

	// �e�F�̃E�C���X��
	for(i = 0; i < ARRAY_SIZE(st->big_virus_count); i++) {
		st->big_virus_count[i] = 0;
	}

	// �f���^�C�}�[��ݒ�
	switch(evs_gamesel) {
	case GSL_1DEMO: case GSL_2DEMO: case GSL_4DEMO:
		st->demo_timer = 1800;
		st->demo_flag = 1;
		break;
	default:
		st->demo_timer = 0;
		st->demo_flag = 0;
		break;
	}

	// �\�����_�ŏ����J�E���^
	st->blink_count = 0;

	// �J�v�Z���ςݏオ�莞�� SE �Đ��J�E���^
	st->warning_se_count = 0;

	// �������J�E���g�̏�����
	if(!reinit) {
		for(i = 0; i < 4; i++) {
			st->win_count[i] = 0;
		}
	}

	// �ϋv or �^�C���A�^�b�N�̏ꍇ�^�C�����N���A
	switch(evs_gamemode) {
	case GMD_TIME_ATTACK:
	case GMD_TaiQ:
		evs_game_time = 0;
		break;
	}

	// ���y�Đ�
	dm_seq_play_in_game(evs_seqnumb << 1);

	// �J�v�Z���쐬
	dm_make_magazine();

	// �E�C���X���\���T�C�Y�̏�����
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			// 4P�`�[���΍R��̍U���J�v�Z���X�g�b�N�̃N���A
			st->story_4p_stock_cap[i][j] = -1;
		}
	}

	// �ϋv���[�h���A��i�V�t�g����t���O
	st->bottom_up_flag = false;

	// �|�[�Y�A�f�o�b�O�A�R���t�B�O �v����
	st->query_play_pause_se = 0;
	st->query_pause_player_no = -1;
	st->query_debug_player_no = -1;
	st->query_config_player_no = -1;

	// �p�X���[�h
	st->passWnd.alpha = 0;
	st->passAlphaStep = -16;
	bzero(st->passBuf, sizeof(st->passBuf));

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		k = 0; // �A�C�e���̑傫��
		j = 0; // ��w���W
		break;
	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		k = 0; // �A�C�e���̑傫��
		j = 1; // ��w���W
		break;
	case GSL_4PLAY:
	case GSL_4DEMO:
		k = 1; // �A�C�e���̑傫��
		j = 2; // ��w���W
		break;
	}

	for(i = 0; i < 4; i++) {
		state = &game_state_data[i];

		// ���W�n�̐ݒ�
		state->map_x = map_x_table[j][i]; // ��w���W
		state->map_y = map_y_table[k];    // ��x���W
		state->map_item_size = size_table[k]; // ��A�C�e���T�C�Y
	}

	for(i = 0; i < 4; i++) {
		state = &game_state_data[i];

		// ��������ϐ��̐ݒ�
		state->player_no    = i;            // �v���C���[�ԍ�
		state->mode_now     = dm_mode_init; // ������������������[�h�ɐݒ�
		state->cnd_now      = dm_cnd_init;  // ��Ԃ���������Ԃɐݒ�
		state->cnd_training = dm_cnd_wait;  // �g���[�j���O��Ԃ�ʏ���(�g���[�j���O����)�ɐݒ�
		state->cnd_static   = dm_cnd_wait;  // ��ΓI��Ԃ�ʏ��Ԃɐݒ�
		state->virus_number = 0;            // �E�C���X���̃N���A
		state->virus_order_number = 0;      // �z�u���̃E�B���X�ԍ�

		// �x�����t���O���N���A
		state->warning_se_flag = 0;

		state->flg_retire    = 0; // ���^�C�A�t���O�̃N���A
		state->flg_game_over = 0; // GameOver�t���O�̃N���A
		state->flg_training  = 0; // Training�t���O�̃N���A

		// �J�v�Z���������x�̐ݒ�
		state->cap_speed        = GameSpeed[state->cap_def_speed]; // ���������J�E���g�̐ݒ�
		state->cap_speed_max    = 0; // �����ݒ肳���
		state->cap_speed_vec    = 1; // �����J�E���^�����l
		state->cap_magazine_cnt = 1; // �}�K�W���c�Ƃ�1�ɂ���
		state->cap_count        = 0; // �J�E���^�̏�����
		state->cap_speed_count  = 0;
		state->cap_move_se_flg  = 0; // �ړ����̂r�d�Đ��p�t���O�̃N���A

		// �J�v�Z�����̏����ݒ�
		dm_set_capsel(state);

		// �E�C���X�A�j���[�V�����̐ݒ�
		state->virus_anime       = 0; // �J�E���^�ƃA�j���V�����ԍ��̏�����
		state->virus_anime_count = 0;
		state->virus_anime_vec   = 1; // �A�j���[�V�����i�s�����̎w��
		state->erase_anime       = 0; // ���ŃA�j���[�V�����R�}���̏�����
		state->erase_anime_count = 0; // ���ŃA�j���[�V�����J�E���^�̏�����
		state->erase_virus_count = 0; // ���ŃE�C���X�J�E���^�̏�����
		state->erase_virus_count_old = 0;

		// �A�����̏�����
		state->chain_count = state->chain_line = 0;

		// �r�̒��g����i�グ�鎞�̃J�E���^
		state->bottom_up_scroll = 0;

		// �E�B���X������
		state->total_erase_count = 0;

		// ���v�A����
		state->total_chain_count = 0;

		switch(evs_gamesel)
		{
		case GSL_4PLAY:
		case GSL_4DEMO:
			// �S�o�̏ꍇ
			state->virus_anime_spead = v_anime_def_speed_4p;// �A�j���[�V�����Ԋu�̐ݒ�
			state->virus_anime_max = 0;                     // �E�C���X�A�j���[�V�����ő�R�}���̐ݒ�
			break;

		default:
			// ����ȊO�̏ꍇ
			state->virus_anime_spead = v_anime_def_speed;   // �A�j���[�V�����Ԋu�̐ݒ�
			state->virus_anime_max = 2;                     // �E�C���X�A�j���[�V�����ő�R�}���̐ݒ�
			break;
		}

		// �U���J�v�Z���f�[�^�̏�����
		for(j = 0; j < 4; j++) {
			state->chain_color[j] = 0;
		}

		// �픚�J�v�Z���f�[�^�̏�����
		for(j = 0; j < DAMAGE_MAX; j++) {
			state->cap_attack_work[j][0] = 0; // �U������
			state->cap_attack_work[j][1] = 0; // �U������
		}

		// �}�b�v���i�r�̒��j�̏�����
		init_map_all(game_map_data[i]);
	}

	// �E�C���X�f�[�^�̍쐬
	for(i = 0; i < evs_playcnt; i++) {
		dm_virus_init(evs_gamemode, &game_state_data[i], virus_map_data[i], virus_map_disp_order[i]);

		// �t���b�V���E�B���X�̈ʒu�����߂�
		if(evs_gamemode == GMD_FLASH) {
			make_flash_virus_pos(&game_state_data[i], virus_map_data[i], virus_map_disp_order[i]);
		}
		else {
			game_state_data[i].flash_virus_count = 0;
		}
	}

	// ���x���������������ꍇ�E�C���X�����R�s�[����
	for(i = 0; i < evs_playcnt - 1; i++) {
	for(j = i + 1; j < evs_playcnt; j++) {
		if(game_state_data[i].virus_level == game_state_data[j].virus_level) {
			// �E�B���X�̈ʒu���R�s�[����
			dm_virus_map_copy(virus_map_data[i], virus_map_data[j], virus_map_disp_order[i], virus_map_disp_order[j]);

			// �t���b�V���E�B���X�̈ʒu���R�s�[����
			if(evs_gamemode == GMD_FLASH) {
				game_state_data[j].flash_virus_count = game_state_data[i].flash_virus_count;
				bcopy(game_state_data[i].flash_virus_pos, game_state_data[j].flash_virus_pos, sizeof(game_state_data[1].flash_virus_pos));
				bcopy(game_state_data[i].flash_virus_bak, game_state_data[j].flash_virus_bak, sizeof(game_state_data[1].flash_virus_bak));
			}
			break;
		}
	}}

	// �G�t�F�N�g��������
	effectAll_init();

	aifGameInit();

	fool_mode = s_hard_mode = 0;
	if(evs_story_level == 3) {
		if(evs_story_no == LAST_STAGE_NO - 1) {
			fool_mode = TRUE;
		}
		else {
			s_hard_mode = TRUE;
		}
	}
}

//## �w�i�e�N�X�`����ǂݍ���
void dm_game_init_static()
{
	Game *st = watchGame;
	void *(*tbl)[2] = _romDataTbl;
	mem_char *mc;
	int i, j, score;

	// EEPROM �̏������ݏ����t���O
	st->eep_rom_flg = 0;

	// �O���t�B�b�N�X���b�h�̃v���C�I���e�B��ݒ�
	st->graphic_thread_pri = OS_PRIORITY_APPMAX;

	// ���ʃe�N�X�`���f�[�^��ǂݍ���
	st->texAL = tiLoadTexData(&heapTop, tbl[_game_al][0], tbl[_game_al][1]);
	st->texItem = tiLoadTexData(&heapTop, tbl[_game_item][0], tbl[_game_item][1]);

	// �J�T�}��
	if(main_no != MAIN_GAME) {
		st->texKaSa = tiLoadTexData(&heapTop, tbl[_menu_kasa][0], tbl[_menu_kasa][1]);
	}

	// �J�[�e���̈ʒu��ݒ�
//	st->curtain_count = CURTAIN_DOWN;
//	st->curtain_step = -1;

	// �G�t�F�N�g�f�[�^�����[�h
	st->effect_data_buf = heapTop;
	heapTop = (void *)ExpandGZip(
		tbl[_game_etc][0], st->effect_data_buf,
		(u8 *)tbl[_game_etc][1] - (u8 *)tbl[_game_etc][0]);

	// �ϐ��̏�����
	for(i = 0; i < 2; i++) {
		st->vs_win_count[i] = 0;
	}

	// �r�A�p�l�����ړ�������Ƃ��̃J�E���^
	st->frame_move_count = 0;
	st->frame_move_step = 1;

	// �J�v�Z�����n���̃E�F�C�g
	st->touch_down_wait = 2;

	// �R�C���O���t�B�b�N�A�j���[�V������������
	_init_coin_logo(0);

	// �R�[�q�[�u���[�N��
	st->coffee_break_flow = 0;
	st->coffee_break_timer = 0;
	st->coffee_break_level = 0;
	st->coffee_break_shard = 0;

	// ���v���C�f�[�^�o�b�t�@��������
	replay_record_init_buffer(&heapTop);
	replay_record_init(evs_playcnt);

	// �^�C���A�^�b�N�̌��ʕ\��
	for(i = 0; i < ARRAY_SIZE(st->timeAttackResult); i++) {
		timeAttackResult_init(&st->timeAttackResult[i]);
	}

	// �p�X���[�h
	msgWnd_init2(&st->passWnd, &heapTop, 256, 10, 5, 0, 0);
	st->passWnd.posX = (SCREEN_WD - st->passWnd.colStep * PASSWORD_LENGTH) / 2;
	st->passWnd.posY = (SCREEN_HT - st->passWnd.rowStep * 2) / 2;

	// EEPROM�������ݒ��̌x�����b�Z�[�W
	RecWritingMsg_init(&st->writingMsg, &heapTop);

	// �n�C�X�R�A���擾
	switch(evs_gamesel) {
	case GSL_1DEMO:
		evs_high_score = DEFAULT_HIGH_SCORE;
		break;

	case GSL_1PLAY: {
		mem_char *mc = &evs_mem_data[evs_select_name_no[0]];

		if(evs_select_name_no[0] == DM_MEM_GUEST) {
			evs_high_score = DEFAULT_HIGH_SCORE;
			break;
		}

		switch(evs_gamemode) {
		case GMD_NORMAL:
			evs_high_score = mc->level_data[game_state_data[0].cap_def_speed].score;
			break;
		case GMD_TaiQ:
			evs_high_score = mc->taiQ_data[game_state_data[0].game_level].score;
			break;
		case GMD_TIME_ATTACK:
			evs_high_score = mc->timeAt_data[game_state_data[0].game_level].score;
			break;
		}
		} break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
	case GSL_4PLAY:
	case GSL_4DEMO:
		if(!evs_story_flg) {
			evs_high_score = 0;
		}
		break;
	}

	// �O���t�B�b�N�f�[�^�̓Ǎ���(�w�i��)
	switch(evs_gamesel) {
	case GSL_1DEMO:
	case GSL_1PLAY:
		st->touch_down_wait = 1;

		// �}���I�A�j���[�V������������
		animeState_load(&game_state_data[0].anime, &heapTop, 15);
		animeState_set(&game_state_data[0].anime, ASEQ_DAMAGE);

		// �e�N�X�`���f�[�^��ǂݍ���(��l�p)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// �e�N�X�`���f�[�^��ǂݍ���(���x���Z���N�g�p)
		st->texLS = tiLoadTexData(&heapTop, tbl[_game_ls][0], tbl[_game_ls][1]);

		// �E�C���X�A�j���[�V������������
		for(i = 0; i < 3; i++) {
			animeState_load(&st->virus_anime_state[i], &heapTop, 16+i);
		}

		// ���A�j���[�V������������
		animeSmog_load(&st->virus_smog_state[0], &heapTop);
		for(i = 1; i < 3; i++) {
			animeSmog_init(&st->virus_smog_state[i], &st->virus_smog_state[0].animeState[0]);
		}

		// �R�[�q�[�u���[�N
		mc = &evs_mem_data[ evs_select_name_no[0] ];
#if LOCAL==JAPAN
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 64, 15);
#elif LOCAL==AMERICA
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 40, 15);
		st->msgWnd.centering = 1;
#elif LOCAL==CHINA
		msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 20, 15, 40, 15);
		st->msgWnd.centering = 1;
#endif
		msgWnd_addStr(&st->msgWnd, st_staffroll_txt);
		msgWnd_skip(&st->msgWnd);
		st->msgWnd.fontType = 1;
		st->msgWnd.contFlags = 0;
		heapTop = (void *)init_coffee_break((u32)heapTop, game_state_data[0].cap_def_speed);

		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		// �L�����A�j���[�V������������
		for(i = 0; i < 2; i++) {
			animeState_load(&game_state_data[i].anime, &heapTop, game_state_data[i].charNo);
		}

		// ��������������
		if(!evs_story_flg) {
			// �|�C���g�\���̐��l�����炩���߃Z�b�g���Ă���
			for(i = 0;i < 2;i++) {
				st->vs_win_total[i] = 0;
			}

			switch(evs_gamesel) {
			case GSL_2PLAY:
				for(i = 0; i < 2; i++) {
					st->vs_win_total[i] = evs_mem_data[evs_select_name_no[i]].vsman_data[0];
				}
				break;

			case GSL_VSCPU:
//				st->vs_win_total[0] = evs_mem_data[evs_select_name_no[0]].vscom_data[0];
//				st->vs_win_total[1] = evs_mem_data[evs_select_name_no[0]].vscom_data[1];
				break;
			}
		}

		// �e�N�X�`���f�[�^��ǂݍ���(��l�p)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// �e�N�X�`���f�[�^��ǂݍ���(��l�p)
		st->texP2 = tiLoadTexData(&heapTop, tbl[_game_p2][0], tbl[_game_p2][1]);

		break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		// �e�N�X�`���f�[�^��ǂݍ���(��l�p)
		st->texP1 = tiLoadTexData(&heapTop, tbl[_game_p1][0], tbl[_game_p1][1]);

		// �e�N�X�`���f�[�^��ǂݍ���(�l�l�p)
		st->texP4 = tiLoadTexData(&heapTop, tbl[_game_p4][0], tbl[_game_p4][1]);

		// �L�����A�j���[�V������������
		for(i = 0; i < 4; i++) {
			animeState_load(&game_state_data[i].anime, &heapTop, game_state_data[i].charNo);
		}

		// �`�[���̃����o�[���v�Z
		for(i = 0; i < 4; i++) {
			st->story_4p_name_num[i] = 0;
		}
		for(i = 0; i < 4; i++) {
			st->story_4p_name_num[ game_state_data[i].team_no ] ++;
		}

		// �v���C���[�̎Q���������߂�
		for(i = st->vs_4p_player_count = 0; i < 4; i++) {
			if(game_state_data[i].player_type == PUF_PlayerMAN) {
				st->vs_4p_player_count++;
			}
		}

//		if(evs_story_flg) {	// �X�g�[���[���[�h�̏ꍇ
//			// �`�[���΍R��
//			st->vs_4p_team_flg = 1;
//
//			// 1 VS 3	(MARIO : ENEMY)
//			st->vs_4p_team_bits[0] = 0x01;
//			st->vs_4p_team_bits[1] = 0x0e;
//		}
//		else {
		if(st->story_4p_name_num[0] == 1 && st->story_4p_name_num[1] == 1
		&&  st->story_4p_name_num[2] == 1 && st->story_4p_name_num[3] == 1)
		{
			// �o�g�����C����
			st->vs_4p_team_flg = 0;
		}
		else {
			// �`�[���΍R��
			st->vs_4p_team_flg = 1;

			// ����p�r�b�g�̍쐬
			st->vs_4p_team_bits[0] = st->vs_4p_team_bits[1] = 0;
			for(i = 0; i < 4; i++) {
				if(game_state_data[i].team_no == TEAM_MARIO) {
					st->vs_4p_team_bits[TEAM_MARIO] |= 1 << i;
				}
			}

			for(i = 0; i < 4; i++) {
				if(game_state_data[i].team_no == TEAM_ENEMY) {
					st->vs_4p_team_bits[TEAM_ENEMY] |= 1 << i;
				}
			}
		}
//		}
		break;
	}
}

//## �w�i�X�i�b�v�̏���
void dm_game_init_snap_bg()
{
	Game *st = watchGame;
	Gfx *gp;

	st->bg_snap_buf = (u8 *)ALIGN_64(heapTop);
	heapTop = st->bg_snap_buf + 328 * 240 * 2;

	st->bg_snapping = false;

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		break;
	default:
		heapTop = story_bg_init(story_proc_no, (u32)heapTop);
		break;
	}
}

//## �w�i���X�i�b�v
void dm_game_draw_snap_bg(Gfx **gpp, Mtx **mpp, Vtx **vpp, int drawPanelFlag)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;
	STexInfo *texC, *texA;
	int i, width;

	if(st->bg_snapping) {
		gDPSetColorImage(gp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 328,
			osVirtualToPhysical(st->bg_snap_buf));
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO: {
		static const short _bgPos[][2] = {
			{ 0, 0 }, { 80, 0 }, { 160, 0 }, { 240, 0 },
			{ 120, 120 }, { 200, 120 }, { 280, 120 },
		};
		static const short _panelPos[][2] = {
			{ 212, 103 }, { 212, 145 }, { 212, 187 },
		};
		static const char _bgTex[] = {
			TEX_bg_endless, TEX_bg_taikyu, TEX_bg_timeattack,
		};
		static const char _magTex[] = {
			TEX_magnifier_endless, TEX_magnifier_taikyu,
			TEX_magnifier_timeattack,
		};
		static const char _scrTex[] = {
			TEX_l_panel_endless, TEX_l_panel_endless, TEX_l_panel_timeattack,
		};
		static const char _panelTex[][3] = {
			{ TEX_r_panel_viruslv, TEX_r_panel_speed, TEX_r_panel_virus, },
			{ TEX_r_panel_level, TEX_r_panel_speed, TEX_r_panel_virus, },
			{ TEX_r_panel_level, TEX_r_panel_speed, TEX_r_panel_virus, },
		};
		enum { bottleX = 110, bottleY = 16 };
		int type;

		switch(evs_gamemode) {
		case GMD_NORMAL:
			type = 0;
			break;
		case GMD_TaiQ:
			type = 1;
			break;
		case GMD_TIME_ATTACK:
			type = 2;
			break;
		}

		gSPDisplayList(gp++, normal_texture_init_dl);

		// �w�i��`��
		texC = st->texLS + _bgTex[type];
		for(i = 0; i < ARRAY_SIZE(_bgPos); i++) {
			tiStretchTexTile(&gp, texC, false,
				0, 0, texC->size[0], texC->size[1],
				_bgPos[i][0], _bgPos[i][1],
				texC->size[0], texC->size[1]);
		}

		// �����Y
		texC = st->texLS + _magTex[type];
		tiStretchTexTile(&gp, texC, false,
			0, 0, texC->size[0], texC->size[1],
			0, 120, texC->size[0], texC->size[1]);

		if(drawPanelFlag) {
			gSPDisplayList(gp++, alpha_texture_init_dl);

			// ��������
			texC = st->texLS + TEX_mario_platform;
			texA = st->texLS + TEX_mario_platform_alpha;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexTile(&gp, width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				0, 0, width, texC->size[1],
				202, 12, width, texC->size[1]);

			// �X�R�A�p�l��
			texC = st->texLS + _scrTex[type];
			texA = st->texLS + TEX_l_panel_alpha;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexTile(&gp, width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texA->size[0],
				0, 0, width, texC->size[1],
				11, 17, width, texC->size[1]);

			// �p�l���Q
			texA = st->texLS + TEX_r_panel_alpha;
			for(i = 0; i < ARRAY_SIZE(_panelPos); i++) {
				texC = st->texLS + _panelTex[type][i];
				width = MIN(texC->size[0], texA->size[0]);
				StretchAlphaTexTile(&gp, width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texA->size[0],
					0, 0, width, texC->size[1],
					_panelPos[i][0], _panelPos[i][1],
					width, texC->size[1]);
			}

			// �ϋv�ƃ^�C�A�^�̃Q�[�����x��
			switch(evs_gamemode) {
			case GMD_TaiQ:
			case GMD_TIME_ATTACK:
				texC = st->texLS + TEX_newmode_level;
				texA = st->texLS + TEX_newmode_level_alpha;
				tiStretchAlphaTexItem(&gp, texC, texA, false,
					3, game_state_data[0].game_level, 232, 120, 48, 16);
				break;
			}

			// �Q�[���X�s�[�h
			texC = st->texLS + TEX_endless_speed;
			texA = st->texLS + TEX_endless_speed_alpha;
			tiStretchAlphaTexItem(&gp, texC, texA, false,
				3, game_state_data[0].cap_def_speed, 232, 162, 48, 16);
		}

		// �������`��̏���
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA / 2);

		// �r�̉e��`��
		texC = st->texP1 + TEX_P1_BOTTLE_SHADOW;
		for(i = 0; i < 2; i++) {
			tiStretchTexBlock(&gp, texC, false,
				bottleX, bottleY, texC->size[0], texC->size[1]);
		}

		// �r��`��
		texC = st->texP1 + TEX_P1_BOTTLE;
		for(i = 0; i < 2; i++) {
			tiCopyTexBlock(&gp, texC, false, bottleX, bottleY);
		}

		} break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		// �w�i��`��
		story_bg_proc(&gp);

		// �������`��̏���
		gSPDisplayList(gp++, normal_texture_init_dl);
		gDPSetCombineMode(gp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
		gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetPrimColor(gp++, 0,0, 255,255,255, BOTTLE_ALPHA);

		// �r�̉e��`��
		texC = st->texP4 + TEX_P4_BOTTLE_SHADOW;
		for(i = 0; i < 4; i++) {
			tiStretchTexBlock(&gp, texC, false,
				_posP4Bottle[i][0] + _posP4BottleIn[0],
				_posP4Bottle[i][1] + _posP4BottleIn[1],
				texC->size[0], texC->size[1]);
		}

		// �r��`��
		texC = st->texP4 + TEX_P4_BOTTLE;
		for(i = 0; i < 4; i++) {
			tiCopyTexBlock(&gp, texC, false, _posP4Bottle[i][0], _posP4Bottle[i][1]);
		}

		if(!drawPanelFlag) {
			break;
		}

		gSPDisplayList(gp++, normal_texture_init_dl);

		// �L�����p�l����`��
		texC = st->texP4 + TEX_P4_BOTTOMPANEL;
		for(i = 0; i < 4; i++) {
			tiStretchTexBlock(&gp, texC, false,
				_posP4CharBase[i][0], _posP4CharBase[i][1],
				texC->size[0], texC->size[1]);
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// Virus�̕�����`��
		if(evs_story_flg || !st->vs_4p_team_flg) {
			if(evs_gamemode == GMD_FLASH) {
				texC = st->texP4 + TEX_p4_panel_flash;
				texA = st->texP4 + TEX_p4_panel_flash_alpha;
			}
			else {
				texC = st->texP4 + TEX_P4_PANEL_VIRUS;
				texA = st->texP4 + TEX_P4_PANEL_VIRUS_ALPHA;
			}
			width = MIN(texC->size[0], texA->size[0]);
			for(i = 0; i < 4; i++) {
				StretchAlphaTexBlock(&gp, width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texA->size[0],
					_posP4CharBase[i][0] + 25,
					_posP4CharBase[i][1],
					width, texC->size[1]);
			}
		}

		// �X�g�[���[���[�̏ꍇ
		if(evs_story_flg) {
			// �^�C��, �X�e�[�W��, ���}�[�N, �̉��n
			texC = st->texP4 + TEX_P4_TOPPANEL_STORY;
			texA = st->texP4 + TEX_P4_TOPPANEL_STORY_ALPHA;
			width = MIN(texC->size[0], texA->size[0]);
			StretchAlphaTexBlock(&gp,
				width, texC->size[1],
				texC->addr[1], texC->size[0],
				texA->addr[1], texC->size[0],
				16, 11, width, texC->size[1]);
		}
		// �`�[���΍R��
		else if(st->vs_4p_team_flg) {
			// �`�[��AB��`��
			for(i = 0; i < 4; i++) {
				static const int tbl[][2] = {
					{ TEX_P4_TEAM_A, TEX_P4_TEAM_A_ALPHA },
					{ TEX_P4_TEAM_B, TEX_P4_TEAM_B_ALPHA },
				};
				int id = (game_state_data[i].team_no == TEAM_MARIO) ? 0 : 1;
				texC = st->texP4 + tbl[id][0];
				texA = st->texP4 + tbl[id][1];
				width = MIN(texC->size[0], texA->size[0]);
				StretchAlphaTexBlock(&gp,
					width, texC->size[1],
					texC->addr[1], texC->size[0],
					texA->addr[1], texC->size[0],
					_posP4CharBase[i][0] + 25,
					_posP4CharBase[i][1],
					width, texC->size[1]);
			}

			// �`�[���p�l��
			texC = st->texP4 + TEX_p4_toppanel_team_1 + evs_vs_count - 1;
			CopyTexBlock8(&gp,
				texC->addr[0], texC->addr[1],
				16, 8, texC->size[0], texC->size[1]);
		}
		// �o�g�����C����
		else {
			// ���}�[�N, �̉��n
			texC = st->texP4 + TEX_p4_toppanel_vs_1 + evs_vs_count - 1;
			CopyTexBlock8(&gp,
				texC->addr[0], texC->addr[1],
				16, 11, texC->size[0], texC->size[1]);
		}
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		story_bg_proc(&gp);

		if(!drawPanelFlag) {
			break;
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// STATE, TIME
		if(evs_story_flg) {
			texC = st->texP2 + TEX_bottom_stage_panel;
		}
		// POINT, TIME
		else {
			texC = st->texP2 + TEX_bottom_point_panel;
		}
		texA = st->texP2 + TEX_bottom_stage_point_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			114, 150, texC->size[0], texC->size[1]);

		// �X�e�[�W�ԍ�
		if(evs_story_flg) {
			draw_count_number(&gp, 0, 1, evs_story_no, 176, 152);
		}

		break;
	}

	*gpp = gp;
}

//## �X�i�b�v�����w�i��`��
void dm_game_draw_snapped_bg(Gfx **gpp)
{
	Game *st = watchGame;
	Gfx *gp = *gpp;

	CopyTexBlock16(&gp, st->bg_snap_buf, 0, 0, 328, 240);

	*gpp = gp;
}

//////////////////////////////////////////////////////////////////////////////
//{### �X�P�W���[������Ă΂�郁�C���֐�

//## �Q�[���������C���֐�
int dm_game_main(NNSched *sched)
{
	static bool dm_game_main2();
	static MAIN_NO dm_game_main3();

	Game *st;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	MAIN_NO ret;
	DM_GAME_RET gret;
	bool loop_flg = true;
	int i, j;

	// �Q�[���p�L�[���쐬������
	key_cntrol_init();

	// ���b�Z�[�W�L���[�̍쐬
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// �N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);

	// CPU�^�X�N�X���b�h���쐬
	cpuTask_createThread();

	// �q�[�v��������
	dm_game_init_heap();
	st = watchGame;

	// �O���t�B�b�N�f�[�^�̓Ǎ���(�w�i��)
	dm_game_init_static();

	// �w�i��荞�݂̏���
	dm_game_init_snap_bg();
	st->bg_snapping = true;

	// �Q�[���ُ̈�����
	dm_game_init(0);

	// �X�e�[�^�X���o�b�N�A�b�v
	backup_game_state(0);

	DBG3(1, "use heap %08x ( %08x - %08x )\n",
		(u32)heapTop - (u32)HEAP_START, HEAP_START, heapTop);

	// �v�l�J�n
	dm_think_flg = 1;

	// �ި���ڲؽ��ޯ̧�̎w��
	gp = gfx_glist[gfx_gtask_no];

	while(loop_flg || st->curtain_count != CURTAIN_DOWN)
	{
		s16 *msgtype;
		int gs, joybak;
		int debug_flag =
			(game_state_data[0].mode_now == dm_mode_debug) ||
			(game_state_data[0].mode_now == dm_mode_debug_config);

		// ���g���[�X���b�Z�[�W
		osRecvMesg(&msgQ,(OSMesg*)&msgtype, OS_MESG_BLOCK);

		if ( GameHalt ) {
			joyProcCore();
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		// �b�o�t��������
		if(!MQ_IS_EMPTY(&msgQ)) {
			nnp_over_time = 1;
		}

		if(*msgtype != NN_SC_RETRACE_MSG) {
			continue;
		}

		// ���C�����[�v�E�o���ɃJ�[�e�������
		if(!loop_flg) {
			if(st->curtain_step < 0) {
				st->curtain_step = -st->curtain_step;
			}
			continue;
		}

#ifdef NN_SC_PERF
		// �p�t�H�[�}���X���[�^�[�\������
		if(_getKeyTrg(0) & Z_TRIG) {
			nnp_over_time = 0;
			_disp_meter_flag ^= 1;
		}
		nnScPushPerfMeter(NN_SC_PERF_MAIN);
#endif

		joybak = joyupd[0];
		for(gs = 0; loop_flg && gs < evs_gamespeed; gs++) {
			if(gs) joyupd[0] = 0;

			// �^�̃��C��
			gret = dm_game_main2();
			loop_flg = (gret == dm_ret_null);
		}
		joyupd[0] = joybak;

#ifdef NN_SC_PERF
		nnScPopPerfMeter();	// NN_SC_PERF_MAIN
#endif
		// �f�����̓{�����[����������
		if(st->demo_flag) {
			dm_seq_set_volume(SEQ_VOL_LOW);
		}

		// �T�E���h�����̍X�V
		dm_audio_update();

		// �p�b�h����
		dm_game_graphic_onDoneSawp();

		if(GameHalt) {
			graphic_no = GFX_NULL;
		}
		else {
			graphic_no = GFX_GAME;
		}
	}

	// �v�l��~
	dm_think_flg = 0;

	// �O���t�B�b�N�X���b�h�̃v���C�I���e�B�����ɖ߂�܂ő҂�
	st->graphic_thread_pri = GRAPHIC_THREAD_PRI;
	while(st->graphic_thread_pri) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// �\����~
	graphic_no = GFX_NULL;

	// ���œh��Ԃ�
	memset(&fbuffer[wb_flag ^ 1], 0xff, SCREEN_WD * SCREEN_HT * 2);

	// �V�[�P���X���~
	dm_audio_stop();

	// �O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while(!dm_audio_is_stopped() || pendingGFX) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// ���ɂǂ̏����ɔ�Ԃ������� & �Z�[�u�f�[�^������
	ret = dm_game_main3(gret);

	// CPU�^�X�N�X���b�h��j��
	while(cpuTask_getTaskCount()) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}
	cpuTask_destroyThread();

	// �N���C�A���g�̖���
	nnScRemoveClient(sched, &client);

	return ret;
}

//## �Q�[���������C���֐�2
static DM_GAME_RET dm_game_main2()
{
	Game *st = watchGame;
	DM_GAME_RET ret;
	game_state *state;
	bool finish = false;
	int i;

	if(st->bgm_bpm_flg[1]) {
		// hi tempo version �̃V�[�P���X�ɐ؂�ւ���
		dm_seq_play_in_game((evs_seqnumb << 1) + 1);
		st->bgm_bpm_flg[1] = 0;
	}

	// ���ʌv�Z
	dm_effect_make();

	// EEPROM�������ݒ��̌x�����b�Z�[�W
	RecWritingMsg_calc(&st->writingMsg);

	switch(evs_gamesel) {
	case GSL_1PLAY:

		// �R�[�q�[�u���[�N��?
		if(st->coffee_break_flow > 0) {
			if(_getKeyLvl(0) & (DM_KEY_A | DM_KEY_B)) {
				st->msgWnd.scrSpeed = 1.0 / 8.0;
			}
			else {
				st->msgWnd.scrSpeed = 1.0 / 60.0;
			}

			msgWnd_update(&st->msgWnd);

			switch(st->coffee_break_flow) {
			// ���b�Z�[�W�X�N���[����
			case 1:
				if(_getKeyTrg(0) & START_BUTTON) {
					st->coffee_break_flow = 0;
				}
				else if(!msgWnd_isScroll(&st->msgWnd)) {
					st->coffee_break_flow = 2;
				}
				break;

			// 
			case 2:
				if(st->coffee_break_timer < 360) {
					st->coffee_break_timer++;
				}
				else if(_getKeyTrg(0) & DM_ANY_KEY) {
					st->coffee_break_flow = 0;
				}
				break;
			}

			if(st->coffee_break_flow == 0) {
				dm_seq_play_in_game(evs_seqnumb << 1);
			}
		}

		// �R�[�q�[�u���[�N��?
		if(st->coffee_break_flow > 0) {
			ret = dm_ret_null;
			break;
		}

		ret = dm_game_main_1p();

		switch(ret) {
		// �X�e�[�W�N���A
		case dm_ret_next_stage:

			// �R�[�q�[�u���[�N?
			i = game_state_data[0].virus_level;
			if(i == 21 || i == 24 || (i >= 30 && i % 5 == 0)) {
				st->coffee_break_flow = 1;
				st->coffee_break_level = (i < 30) ? 0 : (i < 40) ? 1 : 2;
				st->coffee_break_shard = (i < 30) ? ((i - 21) / 3) : ((i / 5) & 1);
			}

			if(st->coffee_break_flow == 1) {
				st->coffee_break_timer = 0;
				init_coffee_break_cnt();
				msgWnd_clear(&st->msgWnd);
				msgWnd_addStr(&st->msgWnd, st_staffroll_txt);
				msgWnd_skip(&st->msgWnd);
			}

			// ���̃E�B���X���x����
			if(game_state_data[0].virus_level < VIRUS_LEVEL_MAX) {
				game_state_data[0].virus_level++;
			}
			break;

		case dm_ret_retry:
			// �X�R�A���[���ɂ���
			game_state_data[0].game_score = 0;
			break;

		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}

		switch(ret) {
		case dm_ret_next_stage:
		case dm_ret_retry:
			ret = dm_ret_null;

			dm_game_init(1);
			animeState_set(&game_state_data[0].anime, ASEQ_DAMAGE);

			for(i = 0; i < 3; i++) {
				animeState_set(&st->virus_anime_state[i], ASEQ_NORMAL);
				animeSmog_stop(&st->virus_smog_state[i]);
			}

			// �X�e�[�^�X���o�b�N�A�b�v
			i = st->coffee_break_flow;
			st->coffee_break_flow = 0;
			backup_game_state(0);
			st->coffee_break_flow = i;

			if(st->coffee_break_flow > 0) {
				dm_seq_play_in_game(SEQ_Coffee);
			}
			break;
		}
		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
		ret = dm_game_main_2p();

		// �������t�������H
		for(i = 0; i < 2; i++) {
			if(evs_story_flg) {
				if(st->win_count[i] > 0) {
					finish = true;
				}
			}
			else {
				if(st->win_count[i] == evs_vs_count) {
					finish = true;
				}
			}
		}

		switch(ret) {
		case dm_ret_game_over:
			if(finish) break;

			ret = dm_ret_null;

			// �X�R�A���[���ɂ���
			if(evs_gamemode == GMD_TIME_ATTACK) {
				for(i = 0; i < 2; i++) {
					game_state_data[i].game_score = 0;
				}
			}

			// �Ď���
			dm_game_init(1);
			for(i = 0; i < 2; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// �X�e�[�^�X���o�b�N�A�b�v
			backup_game_state(0);
			break;

		// ���g���C�̏ꍇ
		case dm_ret_retry:
			ret = dm_ret_null;

			// �X�R�A���[���ɂ���
			for(i = 0; i < 2; i++) {
				game_state_data[i].game_score = 0;
			}

			// �Ď���
			dm_game_init(0);
			for(i = 0; i < 2; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// �X�e�[�^�X���o�b�N�A�b�v
			backup_game_state(0);
			break;

		// ���v���C�̏ꍇ
		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}
		break;

	case GSL_4PLAY:
		ret = dm_game_main_4p();

		// �������t�������H
		for(i = 0; i < 4; i++) {
			if(evs_story_flg) {
				if(st->win_count[i] > 0) {
					finish = true;
				}
			}
			else {
				if(st->win_count[i] == evs_vs_count) {
					finish = true;
				}
			}
		}

		switch(ret) {
		case dm_ret_game_over:
			if(finish) break;

			ret = dm_ret_null;

			// �Ď���
			dm_game_init(1);
			for(i = 0; i < 4; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// �X�e�[�^�X���o�b�N�A�b�v
			backup_game_state(0);
			break;

		case dm_ret_retry:
			ret = dm_ret_null;

			// �X�R�A���[���ɂ���
			for(i = 0; i < 2; i++) {
				game_state_data[i].game_score = 0;
			}

			// �Ď���
			dm_game_init(0);
			for(i = 0; i < 4; i++) {
				animeState_set(&game_state_data[i].anime, ASEQ_NORMAL);
			}

			// �X�e�[�^�X���o�b�N�A�b�v
			backup_game_state(0);
			break;

		case dm_ret_replay:
			ret = dm_ret_null;
			dm_game_init(1);
			break;
		}
		break;

	case GSL_1DEMO:
		ret = dm_game_demo_1p();
		break;

	case GSL_2DEMO:
		ret = dm_game_demo_2p();
		break;

	case GSL_4DEMO:
		ret = dm_game_demo_4p();
		break;
	}

	return ret;
}

//## ���ɂǂ̏����֔�Ԃ��ݒ肷��
static MAIN_NO dm_game_main3(DM_GAME_RET gret)
{
	Game *st = watchGame;
	MAIN_NO ret;
	int i;

	switch(evs_gamesel) {
	case GSL_4PLAY:
		// �X�g�[���[���[�h
		if(evs_story_flg) {
			evs_gamesel = GSL_VSCPU;
//			if(evs_one_game_flg) {
//				ret = MAIN_MENU;
//			}
//			else
			{
				if(game_state_data[0].cnd_static == dm_cnd_win && gret != dm_ret_game_end) {
					story_proc_no++;
					ret = MAIN_STORY;
				}
				// �Q�[���I�[�o�[
				else {
					ret = MAIN_MENU;
				}
			}
		}
		else {
			ret = MAIN_MENU;
		}
		break;

	case GSL_2PLAY:
		ret = MAIN_MENU;
		break;

	case GSL_VSCPU:
//		// �X�g�[���[���[�h && 1�����̂݃t���O
//		if(evs_story_flg && evs_one_game_flg) {
//			ret = MAIN_MENU;
//		}
//		// �X�g�[���[���[�h && !1�����̂݃t���O
//		else if(evs_story_flg && !evs_one_game_flg) {
		if(evs_story_flg) {
			// 
			if(game_state_data[0].cnd_static == dm_cnd_win && gret != dm_ret_game_end) {
				if(dm_query_next_stage(evs_story_level, evs_story_no, game_state_data[0].game_retry)) {
					// ���̃X�e�[�W��
//#define SHOW_CREDIT
#ifdef SHOW_CREDIT
                                        story_proc_no=EX_STAGE_NO + 1;
#else
					story_proc_no++;
#endif
				}
				else {
					// �G���f�B���O�i�̂͂��j
					story_proc_no += 2;
				}
				ret = MAIN_STORY;
			}
			else {
				// EX�X�e�[�W
				if(evs_story_no == EX_STAGE_NO) {
					// ���j���[��
					ret = MAIN_MENU;
				}
				else {
					// ���j���[��
					ret = MAIN_MENU;
				}
			}

			// ���Z���m���b�Z�[�W��ݒ�
			if(story_proc_no == STORY_M_END || story_proc_no == STORY_M_END2
			|| story_proc_no == STORY_W_END || story_proc_no == STORY_W_END2)
			{
				switch(evs_story_level) {
				case 0:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesEasyCont;
					}
					else {
						EndingLastMessage = _mesEasyNoCont;
					}
					break;

				case 1:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesNormalCont;
					}
					else {
						EndingLastMessage = _mesNormalNoCont;
					}
					break;

				case 2:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesHardCont;
					}
					else {
						EndingLastMessage = _mesHardNoCont;
					}
					break;

				case 3:
					if(game_state_data[0].game_retry != 0) {
						EndingLastMessage = _mesSHardCont;
					}
					else {
						EndingLastMessage = _mesSHardNoCont;
					}
					break;
				}
			}
		}
		else {
			ret = MAIN_MENU;
		}
		break;

	case GSL_1PLAY:
		ret = MAIN_MENU;
		break;

	case GSL_1DEMO:
	case GSL_2DEMO:
	case GSL_4DEMO:
		ret = MAIN_TITLE;
		break;
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
//{### �X�P�W���[������Ă΂��O���t�B�b�N�֐�

//## �Q�[���`�惁�C���֐�
void dm_game_graphic()
{
	static void dm_game_graphic2();
	Game *st = watchGame;
	int i;

	if(st->graphic_thread_pri != OS_PRIORITY_APPMAX) {
		// �v���C�I���e�B�����ɖ߂�
		osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);
		st->graphic_thread_pri = 0;
		return;
	}

	// EEPROM�������ݒ��̌x�����b�Z�[�W
	RecWritingMsg_draw(&st->writingMsg, &gp);

	// �ި���ڲؽč쐬�I��
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);

	// ���̨������̊J�n
	osWritebackDCacheAll();
	gfxTaskStart(
		&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX,
		st->bg_snapping ? 0 : NN_SC_SWAPBUFFER);
//		NN_SC_SWAPBUFFER);

	// �v���C�I���e�B�����ɖ߂�
	osSetThreadPri(NULL, GRAPHIC_THREAD_PRI);

	// �f�B�X�v���C���X�g���\�z
	dm_game_graphic2();

	// �I�[�f�B�I��荂���v���C�I���e�B�[��
	osSetThreadPri(NULL, OS_PRIORITY_APPMAX);
}

//## �Q�[���`�惁�C���֐�2
static void dm_game_graphic2()
{
	Game *st = watchGame;
	STexInfo *texC, *texA;
	Mtx *mp;
	Vtx *vp;
	int i, j, cached;
	int debug_flag =
		(game_state_data[0].mode_now == dm_mode_debug) ||
		(game_state_data[0].mode_now == dm_mode_debug_config);

	// �ި���ڲؽ��ޯ̧�̎w��
	gp = gfx_glist[gfx_gtask_no];
	mp = dm_get_mtx_buf();
	vp = dm_get_vtx_buf();

	// RSP �����ݒ�
	gSPSegment(gp++, 0, 0x0);

	// �t���[���o�b�t�@�̐ݒ�
	F3RCPinitRtn();
	gfxSetScissor(&gp, SCISSOR_FLAG, 0, 0, SCREEN_WD, SCREEN_HT);

	// �t���[���o�b�t�@�̃N���A
	F3ClearFZRtn(debug_flag);

	// �w�i�`��
	if(!debug_flag) {
//		if(st->coffee_break_flow == 0) {
//			dm_game_draw_snap_bg(&gp, &mp, &vp, TRUE);
//		}
		if(st->frame_move_count < FRAME_MOVE_MAX) {
			dm_game_draw_snap_bg(&gp, &mp, &vp, TRUE);
		}
		else if(st->coffee_break_flow == 0) {
			dm_game_draw_snapped_bg(&gp);
		}
	}

	// �r�A�p�l����`��
	switch(evs_gamesel) {
	case GSL_2PLAY: case GSL_VSCPU: case GSL_2DEMO: {
		int bak = st->frame_move_count;
		bool visBoard = st->frame_move_count < FRAME_MOVE_MAX;
		bool visScore = !st->bg_snapping;

		if(debug_flag) break;

		if(st->bg_snapping) {
			st->frame_move_count = FRAME_MOVE_MAX;
		}

		// �r����`��
		dm_calc_bottle_2p();
		if(visBoard) {
			dm_draw_bottle_2p(&gp);
		}

		// �p�l����`��
		i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
		if(evs_story_flg) {
			draw_story_board(&gp, 0, -i, visBoard, visScore);
		}
		else {
			draw_vsmode_board(&gp, 0, -i, visBoard, visScore);
		}

		st->frame_move_count = bak;
		} break;
	}

	// �J�v�Z����`��
	switch(evs_gamesel) {
	case GSL_2PLAY: case GSL_VSCPU: case GSL_2DEMO:
	case GSL_4PLAY: case GSL_4DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		for(i = 0; i < evs_playcnt; i++) {
			dm_virus_anime(&game_state_data[i], game_map_data[i]);
			dm_game_graphic_p(&game_state_data[i], i, game_map_data[i]);
		}
		break;
	}

	switch(evs_gamesel) {
	case GSL_1PLAY:
	case GSL_1DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		disp_logo_setup(&gp);

		// �R�[�q�[�u���[�N
		if(st->coffee_break_flow > 0) {
			draw_coffee_break(&gp, st->coffee_break_level, st->coffee_break_shard,
				st->coffee_break_flow > 1 ? 1 : 0);
			msgWnd_draw(&st->msgWnd, &gp);

			if(st->coffee_break_timer == 360) {
				push_any_key_draw(128, 192);
			}
			break;
		}

		// �}���I��`��
		animeState_initDL(&game_state_data[0].anime, &gp);
		animeState_draw(&game_state_data[0].anime, &gp, MARIO_STAND_X, MARIO_STAND_Y, 1, 1);

		// �J�v�Z����`��
		dm_virus_anime(&game_state_data[0], game_map_data[0]);
		dm_game_graphic_1p(&game_state_data[0], 0, game_map_data[0]);

		// �R�C����`��
		_disp_coin_logo(&gp, st->retry_coin);

		gSPDisplayList(gp++, alpha_texture_init_dl);

		// �ō����_��`��
		draw_count_number(&gp, 0, 7, evs_high_score, 29, 39);

		// ���ݓ��_��`��
		draw_count_number(&gp, 0, 7, game_state_data[0].game_score, 29, 67);

		gSPDisplayList(gp++, alpha_texture_init_dl);

		switch(evs_gamemode) {
		// �E�B���X���x������`��
		case GMD_NORMAL:
			draw_virus_number(&gp, game_state_data[0].virus_level, 254, 126, 1, 1);
			break;
		}

		switch(evs_gamemode) {
		// ���Ԃ�`��
		case GMD_NORMAL:
		case GMD_TaiQ:
			draw_time(&gp, evs_game_time, _posLsTime[0], _posLsTime[1]);
			break;

		// �^�C���A�^�b�N�̏ꍇ�J�E���g�_�E���^�C����`��
		case GMD_TIME_ATTACK:
			i = MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time);
			i += FRAME_PAR_MSEC - 1;
			draw_time2(&gp, i, _posLsTime[0], _posLsTime[1]);
			break;
		}

		// �E�B���X����`��
		switch(evs_gamemode) {
		case GMD_TaiQ:
			i = game_state_data[0].total_erase_count;
			break;
		default:
			i = game_state_data[0].virus_number;
			break;
		}
		draw_virus_number(&gp, i, 254, 210, 1, 1);

		// �E�B���X�L������`��
		dm_draw_big_virus(&gp);

		// WIN LOSE DRAW �A�������̕`��
		dm_game_graphic_effect(&game_state_data[0],0,0);

		// 
		if(st->big_virus_stop_count) {
			disp_timestop_logo(&gp, 0);
		}

		break;

	case GSL_2PLAY:
	case GSL_VSCPU:
	case GSL_2DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		disp_logo_setup(&gp);
		gSPDisplayList(gp++, alpha_texture_init_dl);

		// �E�B���X����`��
		for(i = 0; i < 2; i++) {
			draw_virus_number(&gp, game_state_data[i].virus_number,
				_posP2VirusNum[i][0], _posP2VirusNum[i][1], 1, 1);
		}

		// ���Ԃ�`��
		if(evs_gamemode == GMD_TIME_ATTACK) {
			i = MAX(0, (int)SCORE_ATTACK_TIME_LIMIT - (int)evs_game_time);
			i += FRAME_PAR_MSEC - 1;
			draw_time2(&gp, i, 154, 167);
		}
		else {
			draw_time(&gp, evs_game_time, 154, 167);
		}

		// �X�g�[���[���[�h�̏ꍇ
		if(evs_story_flg) {
//			// �p�l����`��
//			i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
//			draw_story_board(&gp, 0, -i, game_state_data[0].game_score, evs_story_level);

			// ���̉��n
			for(i = cached = 0; i < ARRAY_SIZE(_posStStar); i++) {
				if(st->win_count[i] == 0) {
					draw_star_base(&gp, _posStStar[i][0], _posStStar[i][1], cached);
					cached++;
				}
			}
		}
		else {
//			// �p�l����`��
//			i = (FRAME_MOVE_MAX - st->frame_move_count) * SCREEN_HT / 2 / FRAME_MOVE_MAX;
//			draw_vsmode_board(&gp, 0, -i);

			// ��������`��
			for(i = 0; i < 2; i++) {
				static const int _x[] = { 119, 182 };
				draw_count_number(&gp, 0, 2, st->vs_win_total[i], _x[i], 152);
			}

			// ���̉��n
			for(i = cached = 0; i < 2; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP2StarX[i], _posP2StarY[evs_vs_count - 1][j], cached);
					cached++;
				}
			}
		}

		// FLASH
		if(evs_gamemode == GMD_FLASH) {
			texC = st->texP2 + TEX_bottom_flash_panel;
		}
		// VIRUS
		else {
			texC = st->texP2 + TEX_bottom_virus_panel;
		}
		texA = st->texP2 + TEX_bottom_virus_flash_panel_alpha;
		StretchAlphaTexBlock(&gp, texC->size[0], texC->size[1],
			texC->addr[1], texC->size[0], texA->addr[1], texA->size[0],
			131, 181, texC->size[0], texC->size[1]);

		// ����`��
		starForce_calc(&st->starForce, st->star_count);
		starForce_draw(&st->starForce, &gp, st->star_count);

		// �L�����A�j����`��
		for(i = 0; i < 2; i++) {
			static const dir[] = { -1, 1 };

			animeState_initDL(&game_state_data[i].anime, &gp);
			animeState_draw(&game_state_data[i].anime, &gp,
				_posP2CharFrm[i][0], _posP2CharFrm[i][1], dir[i], 1);
		}

		// WIN LOSE DRAW �A�������̕`��
		for(i = 0; i < 2; i++) {
			dm_game_graphic_effect(&game_state_data[i], i, 0);
		}

		gSPDisplayList(gp++, alpha_texture_init_dl);

		break;

	case GSL_4PLAY:
	case GSL_4DEMO:
		if(debug_flag) break;
		if(st->bg_snapping) break;

		// �L�����A�j����`��
		for(i = 0; i < 4; i++) {
			animeState_initDL(&game_state_data[i].anime, &gp);
			animeState_draw(&game_state_data[i].anime, &gp,
				_posP4CharBase[i][0] + 20,
				_posP4CharBase[i][1] + 40, 1, 1);
		}

		disp_logo_setup(&gp);
		gSPDisplayList(gp++, alpha_texture_init_dl);

		// �E�B���X����`��
		for(i = 0; i < 4; i++) {
			draw_virus_number(&gp, game_state_data[i].virus_number,
				_posP4CharBase[i][0] + 50,
				_posP4CharBase[i][1] + 29, 1, 1);
		}

		// �X�g�[���[���[�̏ꍇ
		if(evs_story_flg) {
			// �^�C��
			draw_time(&gp, evs_game_time, 59, 18);

			// ���̉��n
			for(i = cached = 0; i < ARRAY_SIZE(_posStP4StarX); i++) {
				if(st->win_count[i] == 0) {
					draw_star_base(&gp, _posStP4StarX[i], _posStP4StarY, cached);
					cached++;
				}
			}
		}
		// �`�[���΍R��
		else if(st->vs_4p_team_flg) {
			static const int color2index[] = { 1, 0, 2 };
			int tx, ty, tw, th, color;

			// �X�g�b�N�J�v�Z����`��
			texC = st->texP4 + TEX_P4_TOPPANEL_STOCKLAMP;
			texA = st->texP4 + TEX_P4_TOPPANEL_STOCKLAMP_ALPHA;
			tw = texC->size[0] / 2;
			th = texC->size[1] / 3;

			for(i = 0; i < 2; i++) {
				for(j = 0; j < 4; j++) {
					color = st->story_4p_stock_cap[i][j];

					if(color < 0) {
						continue;
					}

					tx = tw * i;
					ty = th * color2index[color];

					StretchAlphaTexTile(&gp,
						texC->size[0], texC->size[1],
						texC->addr[1], texC->size[0],
						texA->addr[1], texA->size[0],
						tx, ty, tw, th,
						_posP4StockCap[i][0] + j * 9,
						_posP4StockCap[i][1],
						tw, th);
				}
			}

			// ���̉��n
			for(i = cached = 0; i < 2; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP4TeamStarX[evs_vs_count - 1][i][j], _posP4TeamStarY, cached);
					cached++;
				}
			}
		}
		// �o�g�����C����
		else {
			// ���̉��n
			for(i = cached = 0; i < 4; i++) {
				for(j = st->win_count[i]; j < evs_vs_count; j++) {
					draw_star_base(&gp, _posP4CharStarX[evs_vs_count - 1][i][j], _posP4CharStarY, cached);
					cached++;
				}
			}
		}

		// ����`��
		starForce_calc(&st->starForce, st->star_count);
		starForce_draw(&st->starForce, &gp, st->star_count);

		// WIN LOSE DRAW �A�������̕`��
		for(i = 0; i < 4; i++) {
			dm_game_graphic_effect(&game_state_data[i],i,1);
		}

		// �U���G�t�F�N�g��`��
		disp_attack_effect(&gp);

		break;
	}

	if(!st->bg_snapping) {
//	if(1) {
		// �f�o�b�O��ʂ�`��
		switch(game_state_data[0].mode_now) {
		case dm_mode_debug:
			DebugMode_Draw(&gp);
			DebugMode_Update();
			break;
		case dm_mode_debug_config:
			DebugConfig_Update();
			DebugConfig_Draw(&gp);
			break;
		}

		// �f���\�� (Dr.�}���I, DEMO PLAY, press any key)
		switch(evs_gamesel) {
		case GSL_1DEMO:
		case GSL_2DEMO:
		case GSL_4DEMO:
			draw_demo_logo(&gp, 106, 170);
			gDPPipeSync(gp++);
			break;
		}

		// ���v���C���S��\��
		if(st->replayFlag && !dm_check_one_game_finish()) {
			int x, y;

			switch(evs_gamesel) {
			case GSL_1PLAY:
			case GSL_4PLAY:
				x = 30; y = 20;
				break;

			case GSL_VSCPU:
			case GSL_2PLAY:
				x = 128; y = 144;
				break;
			}

			draw_replay_logo(&gp, x, y);
		}

		// �p�X���[�h
		if(st->passWnd.alpha > 0) {
			texC = st->texAL + TEX_popup_window;

			gSPDisplayList(gp++, normal_texture_init_dl);
			gDPSetCombineLERP(gp++,
				0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0,
				0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0);
			gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
			gDPSetPrimColor(gp++, 0,0, 255,255,200, st->passWnd.alpha);
			gDPSetTextureLUT(gp++, G_TT_NONE);

			gDPLoadTextureBlock_4b(gp++, texC->addr[1], G_IM_FMT_I, texC->size[0], texC->size[1], 0,
				G_TX_NOMIRROR, G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

			drawCursorPattern(&gp, texC->size[0], texC->size[1], 16, 16,
				st->passWnd.posX - 6, st->passWnd.posY - 6,
				st->passWnd.colStep * PASSWORD_LENGTH + 12,
				st->passWnd.rowStep * 3 + 12);

			msgWnd_update(&st->passWnd);
			msgWnd_draw(&st->passWnd, &gp);
		}
		st->passWnd.alpha = CLAMP(0, 255, st->passWnd.alpha + st->passAlphaStep);

		// �J�[�e����`��
		st->curtain_count = CLAMP(CURTAIN_UP, CURTAIN_DOWN, st->curtain_count + st->curtain_step);
		curtain_proc(&gp, st->curtain_count);
	}

#if defined(DEBUG)
	if(!st->bg_snapping && _disp_meter_flag) {
//	if(_disp_meter_flag) {
		for(i = 0; i < evs_playcnt; i++) {
			game_state *state = &game_state_data[i];
			int x, y, s;
			if(!PLAYER_IS_CPU(state, state->player_no)) continue;

			s = state->map_item_size >> 2;
			x = state->map_x + s + state->map_item_size * (state->ai.aiEX - 1);
			y = state->map_y + s + state->map_item_size * state->ai.aiEY;
			s = state->map_item_size - (s << 1);

			FillRectRGB(&gp, x, y, s, s, 255, 255, 255);
		}
	}
#endif

#ifdef NN_SC_PERF
	if(!st->bg_snapping && _disp_meter_flag) {
//	if(_disp_meter_flag) {
		nnScWritePerfMeter(&gp, 32, 180);
	}
#endif

	if(st->bg_snapping) {
		st->bg_snapping = false;
	}
}

//## �X���b�v����̏���
void dm_game_graphic_onDoneSawp()
{
	Game *st = watchGame;
	OSMesg msg;
	OSMesgQueue msgQ;
	OSTimer timer;
	int i, wait, stepCnt;

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_OVER, NN_SC_PERF_INFO_Count);
#endif

	// �L�[���͂܂ł̃E�F�C�g���擾
	wait = JOYrTIMEvm;

	// �^�C�}�[����̃��b�Z�[�W��҂�
	wait -= OS_CYCLES_TO_USEC(osGetTime());
	if(wait > 50) {
		osCreateMesgQueue(&msgQ, &msg, 1);
		osSetTimer(&timer, OS_USEC_TO_CYCLES(wait), 0, &msgQ, 0);
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// EEPRom�փA�N�Z�X���̓p�b�h��ǂ܂Ȃ�
	if(st->eep_rom_flg) {
		for(i = 0; i < MAXCONTROLLERS; i++) {
			joynew[i] = joyold[i] = joyupd[i] = joycur[i] = 0;
		}
	}
	else {
		joyProcCore();
	}

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_CPU, NN_SC_PERF_INFO_Count);
#endif

	// �L�[����
	for(stepCnt = 0; stepCnt < evs_gamespeed; stepCnt++) {
		bool pause = false;
		bool finish = dm_check_one_game_finish();

		dm_make_key();

		// �|�[�Y��?
		for(i = 0; i < evs_playcnt; i++) {
			if(game_state_data[i].cnd_static == dm_cnd_pause) {
				pause = true;
				break;
			}
		}

		if(!pause && st->started_game_flg) {
			aiCOM_MissTake();
		}

		// ���v���[�f�[�^���擾
		if(st->replayFlag && !pause && !finish && st->started_game_flg) {
			for(i = 0; i < evs_playcnt; i++) {
				joygam[i] = replay_play(i);
			}
			// ���v���C�o�b�t�@�[�����ӂꂽ��?
			for(i = 0; i < evs_playcnt; i++) {
				if(joygam[i] & CONT_START) {
					break;
				}
			}
			if(i < evs_playcnt) {
				dm_seq_set_volume(SEQ_VOL_LOW);
				resume_game_state(1);
			}
		}

		// �E�B���X��]����
		i = (L_TRIG | R_TRIG);
		st->big_virus_no_wait = (evs_gamemode == GMD_TaiQ) && (joygam[0] & i);

		switch(evs_gamesel) {
		case GSL_1PLAY:
			key_control_main(&game_state_data[0], game_map_data[0], 0, main_joy[0]);
			break;

		case GSL_VSCPU:
		case GSL_2PLAY:
			for(i = 0; i < 2; i++) {
				key_control_main(&game_state_data[i], game_map_data[i], i, main_joy[i]);
			}
			break;

		default:
			for(i = 0; i < 4; i++) {
				key_control_main(&game_state_data[i], game_map_data[i], i, main_joy[i]);
			}
			break;
		}

		// ���v���C�p�L�[�f�[�^���o��
		if(!st->replayFlag && !pause) {
			for(i = 0; i < evs_playcnt; i++) {
				replay_record(i, joygam[i]);
			}
		}
	}

#ifdef NN_SC_PERF
	nnScCountUpPerfMeter(NN_SC_PERF_OVER, NN_SC_PERF_INFO_Count);
#endif
}

//## �^�X�N��������̏���
void dm_game_graphic_onDoneTask()
{
}

//////////////////////////////////////////////////////////////////////////////
//{### �X�g�[���[���[�h all clear ��̃��b�Z�[�W

//## ���C��
int main_techmes(void *arg)
{
	Game *st;
	OSMesgQueue msgQ;
	OSMesg msgbuf[MAX_MESGS];
	NNScClient client;
	bool loop = true;
	int i;

	// ���b�Z�[�W�L���[�̍쐬
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);

	// �N���C�A���g�ɓo�^
	nnScAddClient((NNSched *)arg, &client, &msgQ);

	// �q�[�v��������
	dm_game_init_heap();
	st = watchGame;

	// �O���t�B�b�N�f�[�^�̓Ǎ���(�w�i��)
	dm_game_init_static();

	// �w�i
	heapTop = (void *)init_menu_bg((u32)heapTop, 0);

	// ���b�Z�[�W
	msgWnd_init2(&st->msgWnd, &heapTop, 0x1000, 18, 16, 52, 34);
	msgWnd_addStr(&st->msgWnd, EndingLastMessage);
	st->msgWnd.fontType = 1;
#if LOCAL==AMERICA
	st->msgWnd.centering = 1;
#elif LOCAL==CHINA
	st->msgWnd.centering = 1;
#endif

	// �Q�[���ُ̈�����
	i = evs_seqence;
	evs_seqence = 0;
	dm_game_init(0);
	evs_seqence = i;
	dm_seq_play_in_game(SEQ_Coffee);

	while(loop) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		joyProcCore();

		if ( GameHalt ) {
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}

		switch(st->coffee_break_flow) {
		case 0:
			if(st->coffee_break_timer == 255) {
				st->coffee_break_flow++;
			}
			else {
				st->coffee_break_timer = MIN(255, st->coffee_break_timer + 4);
			}
			break;
		case 1:
			if(msgWnd_isEnd(&st->msgWnd)) {
				st->coffee_break_flow++;
			}
			else {
				msgWnd_update(&st->msgWnd);
			}
			break;
		case 2:
			if(_getKeyTrg(0) & DM_ANY_KEY) {
				st->coffee_break_flow++;
			}
			break;
		case 3:
			if(st->coffee_break_timer == 0) {
				loop = false;
			}
			else {
				st->coffee_break_timer = MAX(0, st->coffee_break_timer - 4);
			}
			break;
		}

		dm_audio_update();
		graphic_no = GFX_TECHMES;
	}

	// �`�揈�����~
	graphic_no = GFX_NULL;

	// �V�[�P���X���~
	dm_audio_stop();

	// �O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while(!dm_audio_is_stopped() || pendingGFX) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		dm_audio_update();
	}

	// �N���C�A���g�̖���
	nnScRemoveClient((NNSched *)arg, &client);

	return MAIN_TITLE;
}

//## �`��
void graphic_techmes()
{
	Game *st = watchGame;
	Mtx *mp;
	Vtx *vp;
	int i;

	gp = gfx_glist[gfx_gtask_no];
	mp = dm_get_mtx_buf();
	vp = dm_get_vtx_buf();

	F3RCPinitRtn();
	F3ClearFZRtn(0);

	gDPSetEnvColor(gp++, 0,0,0, 255);
	gDPSetPrimColor(gp++, 0,0, 255,255,255, 255);

//	draw_ending_mess_bg(&gp);
	draw_menu_bg(&gp, 0, -SCREEN_HT / 2);

	dm_draw_KaSaMaRu(&gp, &mp, &vp, msgWnd_isSpeaking(&st->msgWnd), 200, 128, 1, 255);

	msgWnd_draw(&st->msgWnd, &gp);

	switch(st->coffee_break_flow) {
	case 2:
		push_any_key_draw(128, 192);
		break;
	}

	FillRectRGBA(&gp, 0,0,SCREEN_WD,SCREEN_HT, 0,0,0, 255 - st->coffee_break_timer);

	st->blink_count++;

	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();

	gfxTaskStart(&gfx_task[gfx_gtask_no], gfx_glist[gfx_gtask_no],
		(s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx),
		GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF
