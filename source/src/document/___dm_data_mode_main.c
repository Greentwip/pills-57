/*--------------------------------------
	filename	:	dm_data_mode_main.c

	create		:	1999/09/27
	modify		:	1999/12/01

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define F3DEX_GBI
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"

#include	"o_segment.h"
#include	"hardcopy.h"

#include	"dm_nab_include.h"

/*--------------------------------------
	�����O���t�B�b�N�f�[�^�Ǎ��݊֐�
--------------------------------------*/
void	dm_data_mode_num_load( void )
{

	//	��A�̐����`��֐����g�p����O�Ɉ��ĂԂ���
	load_TexTile_4b( dm_data_mode_num_bm0_0,8,160,0,0,7,159);
	load_TexPal( dm_data_mode_num_bm0_0tlut );
}

/*--------------------------------------
	�������O���t�B�b�N�f�[�^�Ǎ��݊֐�
--------------------------------------*/
void	dm_data_mode_mini_num_load( void )
{

	//	��A�̐����`��֐����g�p����O�Ɉ��ĂԂ���
	load_TexBlock_4b( dm_data_mode_mini_num_bm0_0,80,16);
	load_TexPal( dm_data_mode_mini_num_bm0_0tlut );
}

/*--------------------------------------
	���_�`���p�֐�( X���W�͉E�[�̍��W�ɂ���)
--------------------------------------*/
void	dm_data_mode_disp_score( u32 score,s16 x_pos, s16 y_pos,s8 h_size )
{
	s8	i,num[7];
	s8	size;

	size = h_size >> 1;


	num[6] = score / 1000000;
	num[5] = (score % 1000000) / 100000;
	num[4] = (score % 100000) / 10000;
	num[3] = (score % 10000) / 1000;
	num[2] = (score % 1000) / 100;
	num[1] = (score % 100) / 10;
	num[0] = score % 10;

	for( i = 0;i < 7;i++ ){
		x_pos -= 8;
		draw_ScaleTex( x_pos,y_pos - size,8,16,8,h_size,0,num[i] << 4 );
	}
}


/*--------------------------------------
	�����`��֐�(�w�茅�������`�悵�Ȃ�)( X���W�͉E�[�̍��W�ɂ���)
--------------------------------------*/
void	dm_data_mode_disp_num_no_sift( s16 d_num,s16 x_pos, s16 y_pos,s8 keta,s8 h_size )
{
	s32	max;
	s8	i,num[5];
	s8	size;

	size = h_size >> 1;

	for(i = 1,max = 10;i < keta;i++){
		max *= 10;
	}

	if( d_num >= max ){
		d_num = max - 1;
	}

	num[4] = d_num / 10000;
	num[3] = (d_num % 10000) / 1000;
	num[2] = (d_num % 1000) / 100;
	num[1] = (d_num % 100) / 10;
	num[0] = d_num % 10;

	for( i = 0;i < keta;i++ ){
		x_pos -= 8;
		draw_ScaleTex( x_pos,y_pos - size,8,16,8,h_size,0,num[i] << 4 );
	}
}

/*--------------------------------------
	�������`��֐�(�w�茅�������`�悵�Ȃ�)( X���W�͉E�[�̍��W�ɂ���)
--------------------------------------*/
void	dm_data_mode_disp_mini_num_no_sift( s16 d_num,s16 x_pos, s16 y_pos,s8 keta,s8 h_size )
{
	s32	max;
	s8	i,num[5];
	s8	size;

	size = h_size >> 1;

	for(i = 1,max = 10;i < keta;i++){
		max *= 10;
	}

	if( d_num >= max ){
		d_num = max - 1;
	}

	num[4] = d_num / 10000;
	num[3] = (d_num % 10000) / 1000;
	num[2] = (d_num % 1000) / 100;
	num[1] = (d_num % 100) / 10;
	num[0] = d_num % 10;

	for( i = 0;i < keta;i++ ){
		x_pos -= 7;
		draw_ScaleTex( x_pos,y_pos - size,8,16,8,h_size,num[i] << 3,0 );
	}
}


/*--------------------------------------
	�X�g�[���[���[�h�Z�[�u�f�[�^�\�[�g�E����ւ��֐�
--------------------------------------*/
void	dm_story_sort_set( game_state *state,u32 time ,u8 player_no ,u8 g_level,u8 c_stage )
{
	u8	i,j;
	u8	save;
	mem_story	rank[6],rank_save;
	mem_story	*rank_p;
	mem_char *mp = &evs_mem_data[player_no];

	//	STORY���[�h�̃v���C��
	mp -> mode_play[DAM_M_STORY] ++;
	mp -> mode_play[DAM_M_STORY] += state -> game_retry;

	if( mp -> mode_max_chain[DAM_M_STORY] < state -> max_chain_line ){
		 mp -> mode_max_chain[DAM_M_STORY] = state -> max_chain_line;
	}

	rank_p = rank;
	for(i = 0;i < 5;i++){
		*(rank_p + i) = mp -> story_data[i];	//	�f�[�^�̈ړ�
	}
	(rank_p + 5) -> g_level = g_level;
	(rank_p + 5) -> c_stage	= c_stage;
	(rank_p + 5) -> retry	= state -> game_retry;
	(rank_p + 5) -> score	= state -> game_score;
	(rank_p + 5) -> time	= time;

	//	�\�[�g
	for( i = 0;i < 5;i++ ){
		for(j = i + 1;j < 6;j++ ){
			if( (rank_p + i) -> score < (rank_p + j) -> score ){
				//	����ւ�
				rank_save = *(rank_p + i);
				*(rank_p + i) = *(rank_p + j);
				*(rank_p + j) = rank_save;
			}else	if( (rank_p + i) -> score == (rank_p + j) -> score ){
				//	�����_�̏ꍇ�^�C���ŋ���
				if( (rank_p + i) -> time > (rank_p + j) -> time ){
					//	����ւ�
					rank_save = *(rank_p + i);
					*(rank_p + i) = *(rank_p + j);
					*(rank_p + j) = rank_save;
				}else	if( (rank_p + i) -> time == (rank_p + j) -> time ){
					//	���^�C���̏ꍇ�Q�[�����x���ŋ���
					if( (rank_p + i) -> g_level < (rank_p + j) -> g_level ){
						//	����ւ�
						rank_save = *(rank_p + i);
						*(rank_p + i) = *(rank_p + j);
						*(rank_p + j) = rank_save;
					}else	if( (rank_p + i) -> g_level == (rank_p + j) -> g_level ){
						//	���Q�[�����x���̏ꍇ�N���A�X�e�[�W�ŋ���
						if( (rank_p + i) -> c_stage < (rank_p + j) -> c_stage ){
							//	����ւ�
							rank_save = *(rank_p + i);
							*(rank_p + i) = *(rank_p + j);
							*(rank_p + j) = rank_save;
						}else	if( (rank_p + i) -> c_stage == (rank_p + j) -> c_stage ){
							//	���N���A�X�e�[�W�̏ꍇ���g���C�񐔂ŋ���
							if( (rank_p + i) -> retry > (rank_p + j) -> retry ){
								//	����ւ�
								rank_save = *(rank_p + i);
								*(rank_p + i) = *(rank_p + j);
								*(rank_p + j) = rank_save;
							}//	�܂��������ꍇ�͌������Ȃ�
						}
					}
				}
			}
		}
	}
	//	����ւ����f�[�^���Z�b�g
	for(i = 0;i < 5;i++){
		mp -> story_data[i] = *(rank_p + i);
	}

}

/*--------------------------------------
	LEVEL �Z�[�u�f�[�^�\�[�g�E����ւ��֐�
--------------------------------------*/
void	dm_level_sort_set( game_state *state,u8 player_no )
{
	u8	i,j;
	u8	save;
	mem_level	rank[6],rank_save;
	mem_level	*rank_p;
	mem_char *mp = &evs_mem_data[player_no];


	//	LEVEL���[�h�̃v���C��
	mp -> mode_play[DAM_M_LEVEL] ++;
	mp -> mode_play[DAM_M_LEVEL] += state -> game_retry;

	if( mp -> mode_max_chain[DAM_M_LEVEL] < state -> max_chain_line ){
		 mp -> mode_max_chain[DAM_M_LEVEL] = state -> max_chain_line;
	}

	rank_p = rank;
	for(i = 0;i < 5;i++){
		*(rank_p + i) = mp -> level_data[i];	//	�f�[�^�̈ړ�
	}
	(rank_p + 5) -> c_level	= state -> virus_level;
	(rank_p + 5) -> retry	= state -> game_retry;
	(rank_p + 5) -> score	= state -> game_score;
	(rank_p + 5) -> speed	= state -> cap_def_speed;
	//	���x�� 100 �ȏ�ɂ��Ȃ�
	if( (rank_p + 5) -> c_level > 99 ){
		(rank_p + 5) -> c_level = 99;
	}

	for( i = 0;i < 5;i++ ){
		for(j = i + 1;j < 6;j++ ){
			if( (rank_p + i) -> c_level < (rank_p + j) -> c_level ){
				rank_save = *(rank_p + i);
				*(rank_p + i) = *(rank_p + j);
				*(rank_p + j) = rank_save;
			}else	if( (rank_p + i) -> c_level == (rank_p + j) -> c_level ){
				//	�����x���̏ꍇ�J�v�Z�����x�ŋ���
				if( (rank_p + i) -> speed < (rank_p + j) -> speed ){
					rank_save = *(rank_p + i);
					*(rank_p + i) = *(rank_p + j);
					*(rank_p + j) = rank_save;
				}else	if( (rank_p + i) -> speed == (rank_p + j) -> speed ){
					//	���J�v�Z�����x�̏ꍇ���_�ŋ���
					if( (rank_p + i) -> score < (rank_p + j) -> score ){
						rank_save = *(rank_p + i);
						*(rank_p + i) = *(rank_p + j);
						*(rank_p + j) = rank_save;
					}else	if( (rank_p + i) -> score == (rank_p + j) -> score ){
						//	�����_�̏ꍇ���g���C�񐔂ŋ���
						if( (rank_p + i) -> retry > (rank_p + j) -> retry ){
							rank_save = *(rank_p + i);
							*(rank_p + i) = *(rank_p + j);
							*(rank_p + j) = rank_save;
						}//	�܂��������ꍇ�͌������Ȃ�
					}
				}
			}
		}
	}

	//	����ւ����f�[�^���Z�b�g
	for(i = 0;i < 5;i++){
		mp -> level_data[i] = *(rank_p + i);
	}
}

/*--------------------------------------
	VSCPU �Z�[�u�f�[�^�ݒ�֐�
--------------------------------------*/
void	dm_vscom_set( game_state *state,s16 win,s16 lose,u8 player_no,s8 char_no )
{
	mem_char *mp = &evs_mem_data[player_no];

	//	VSCOM���[�h�̃v���C��
	mp -> mode_play[DAM_M_VSCOM] ++;
	mp -> mode_play[DAM_M_VSCOM] += state -> game_retry;

	if( mp -> mode_max_chain[DAM_M_VSCOM] < state -> max_chain_line ){
		 mp -> mode_max_chain[DAM_M_VSCOM] = state -> max_chain_line;
	}

	 mp -> vscom_data[char_no][0] += win;
	 mp -> vscom_data[char_no][1] += lose;
}
/*--------------------------------------
	2PLAY �Z�[�u�f�[�^�ݒ�֐�
--------------------------------------*/
void	dm_vsman_set( game_state *state,s16 win,s16 lose,u8 player_no,s8 enemy_no )
{
	mem_char *mp = &evs_mem_data[player_no];

	//	VSMAN���[�h�̃v���C��
	mp -> mode_play[DAM_M_VSMAN] ++;
	mp -> mode_play[DAM_M_VSMAN] += state -> game_retry;

	if( mp -> mode_max_chain[DAM_M_VSMAN] < state -> max_chain_line ){
		 mp -> mode_max_chain[DAM_M_VSMAN] = state -> max_chain_line;
	}

	 mp -> vs_data[enemy_no][0] += win;
	 mp -> vs_data[enemy_no][1] += lose;

}


/*--------------------------------------
	�X�g�[���[���[�h���ʃ\�[�g�֐�
--------------------------------------*/
void	dm_data_mode_story_sort(void)
{
	u8	i,j,rank[8];
	u8	save;
	u32	score;
	mem_char *mp = evs_mem_data;

	for( i = 0;i < 8;i++ ){
		rank[i] = i;
	}

	//	�\�[�g
	for(i = 0;i < 7;i++ ){
		for( j = i + 1;j < 8;j++ ){
			if( (mp + rank[i]) -> story_data[0].score < (mp + rank[j]) -> story_data[0].score ){
				//	�f�[�^����
				save = rank[i];
				rank[i] = rank[j];
				rank[j] = save;
			}else	if( (mp + rank[i]) -> story_data[0].score == (mp + rank[j]) -> story_data[0].score ){
				//	�������ꍇ�̓N���A���ԂŃ\�[�g
				if( (mp + rank[i]) -> story_data[0].time > (mp + rank[j]) -> story_data[0].time ){
					//	�f�[�^����
					save = rank[i];
					rank[i] = rank[j];
					rank[j] = save;
				}
			}
		}
	}
	//	�\���p���ʐݒ�
	score = (mp + rank[0]) -> story_data[0].score;
	for(i = 0,save = 0;i < 8;i++ ){
		if( (mp + rank[i]) -> story_data[0].score != score ){
			score = (mp + rank[i]) -> story_data[0].score;
			save++;
		}
		dm_dmr_r[DAM_M_STORY][i] = save;
	}

	//	�l�f�[�^�̕\���p���ʐݒ�
	for(i = 0;i < 8;i++){
		score = (mp + i) -> story_data[0].score;
		for(j = save = 0;j < 5;j++){
			if( (mp + i) -> story_data[j].score != score ){
				score = (mp + i) -> story_data[j].score;
				save ++;
			}
			dm_dmr_sl_r[DAM_M_STORY][i][j] = save;

		}
	}

	//	���ڃR�s�[�����f�[�^�͎g�킸�A�ԐڎQ�Ƃ��s��(��̂���)
	for(i = 0;i < 8;i++){
		dm_dmr_d[DAM_M_STORY][i] = rank[i];
		//	���ԕ\���̌v�Z���ɂ���Ă���

		dm_dm_a_time[i][0] = evs_mem_data[i].story_data[0].time / 3600;	//	��
		dm_dm_a_time[i][1] = (evs_mem_data[i].story_data[0].time % 3600) / 60;	//	�b
		for( j = 0;j < 5;j++ ){
			dm_dm_p_time[i][j][0] = evs_mem_data[i].story_data[j].time / 3600;	//	��
			dm_dm_p_time[i][j][1] = (evs_mem_data[i].story_data[j].time % 3600) / 60;	//	�b
		}
	}
}

/*--------------------------------------
	���x���Z���N�g���ʃ\�[�g�֐�
--------------------------------------*/
void	dm_data_mode_level_sort(void)
{
	u8	i,j,rank[8];
	u8	save,c_level;
	mem_char *mp = evs_mem_data;

	for( i = 0;i < 8;i++ ){
		rank[i] = i;
	}

	//	�\�[�g
	for(i = 0;i < 7;i++ ){
		for( j = i + 1;j < 8;j++ ){
			if( (mp + rank[i]) -> level_data[0].c_level < (mp + rank[j]) -> level_data[0].c_level ){
				//	�f�[�^����
				save = rank[i];
				rank[i] = rank[j];
				rank[j] = save;
			}
		}
	}
	//	�\���p���ʂ�����
	c_level = (mp + rank[0]) -> level_data[0].c_level;
	for(i = 0, save = 0;i < 8;i++ ){
		if( (mp + rank[i]) -> level_data[0].c_level != c_level ){
			c_level = (mp + rank[i]) -> level_data[0].c_level;
			save++;
		}
		dm_dmr_r[DAM_M_LEVEL][i] = save;
	}

	//	�l�̕\���p���ʂ�����
	for( i = 0;i < 8;i++){
		c_level = (mp + i) -> level_data[0].c_level;
		for(j = save = 0;j < 5;j++){
			if( (mp + i) -> level_data[j].c_level != c_level ){
				c_level = (mp + i) -> level_data[j].c_level;
				save++;
			}
			dm_dmr_sl_r[DAM_M_LEVEL][i][j] = save;
		}
	}

	//	���ڃR�s�[�����f�[�^�͎g�킸�A�ԐڎQ�Ƃ��s��(��̂���)
	for(i = 0;i < 8;i++){
		dm_dmr_d[DAM_M_LEVEL][i] = rank[i];
	}
}
/*--------------------------------------
	VSCOM ���ʃ\�[�g�֐�
--------------------------------------*/
void	dm_data_vscom_sort(void)
{
	u8	i,j,rank[8];
	u8	save,ave;
	mem_char *mp = evs_mem_data;

	//	VSCOM �����v�Z
	for(i = 0;i < 8;i++){
		rank[i] = i;
		dm_vscom_a_ave[i][0] = dm_vscom_a_ave[i][1] = dm_vscom_a_ave[i][2] = 0;
		for(j = 0;j < 12;j++){
			dm_vscom_a_ave[i][1] += (mp + i) -> vscom_data[j][0];	//	�������̉��Z
			dm_vscom_a_ave[i][2] += (mp + i) -> vscom_data[j][1];	//	�s�k���̉��Z

		}
		if( dm_vscom_a_ave[i][1] != 0 || dm_vscom_a_ave[i][2] != 0 ){	//	���������Ă����ꍇ
			dm_vscom_a_ave[i][0] = ( dm_vscom_a_ave[i][1] / (dm_vscom_a_ave[i][1] + dm_vscom_a_ave[i][2])) * 100; //	��������o��
		}
	}

	//	�\�[�g
	for( i = 0;i < 7;i++ ){
		for( j = i + 1;j < 8;j++ ){
			if( dm_vscom_a_ave[ rank[i] ][0] < dm_vscom_a_ave[rank[j]][0] ){
				//	�f�[�^����
				save = rank[i];
				rank[i] = rank[j];
				rank[j] = save;
			}
		}
	}
	//	�\�[�g
	ave = dm_vscom_a_ave[ rank[0] ][0];
	for( i = 0, save = 0;i < 8;i++ ){
		if( dm_vscom_a_ave[ rank[i] ][0] != ave ){
			ave = dm_vscom_a_ave[ rank[i] ][0];
			save++;
		}
		dm_dmr_r[DAM_M_VSCOM][i] = save;
	}

	//	���ڃR�s�[�����f�[�^�͎g�킸�A�ԐڎQ�Ƃ��s��(��̂���)
	for(i = 0;i < 8;i++){
		dm_dmr_d[DAM_M_VSCOM][i] = rank[i];
	}

}

/*--------------------------------------
	VSMAN ���ʃ\�[�g�֐�
--------------------------------------*/
void	dm_data_vsman_sort(void)
{
	u8	i,j,rank[8];
	u8	save,ave;
	mem_char *mp = evs_mem_data;

	//	VSMAN �����v�Z
	for(i = 0;i < 8;i++){
		rank[i] = i;
		dm_vsman_a_ave[i][0] = dm_vsman_a_ave[i][1] = dm_vsman_a_ave[i][2] = 0;
		for(j = 0;j < 8;j++){
			dm_vsman_a_ave[i][1] += (mp + i) -> vs_data[j][0];	//	�������̉��Z
			dm_vsman_a_ave[i][2] += (mp + i) -> vs_data[j][1];	//	�s�k���̉��Z

			dm_vsman_p_ave[i][j] = 0;
			if( (mp + i) -> vs_data[j][0] != 0 || (mp + i) -> vs_data[j][1] != 0 ){	//	���������Ă����ꍇ
				dm_vsman_p_ave[i][j] = ((mp + i) -> vs_data[j][0] / ( (mp + i) -> vs_data[j][0] + (mp + i) -> vs_data[j][1] )) * 100;
			}
		}
		if( dm_vsman_a_ave[i][1] != 0 || dm_vsman_a_ave[i][2] != 0 ){	//	���������Ă����ꍇ
			dm_vsman_a_ave[i][0] = ( dm_vsman_a_ave[i][1] / (dm_vsman_a_ave[i][1] + dm_vsman_a_ave[i][2])) * 100; //	��������o��
		}
	}

	//	�\�[�g
	for( i = 0;i < 7;i++ ){
		for( j = i + 1;j < 8;j++ ){
			if( dm_vsman_a_ave[ rank[i] ][0] < dm_vsman_a_ave[rank[j]][0] ){
				//	�f�[�^����
				save = rank[i];
				rank[i] = rank[j];
				rank[j] = save;
			}
		}
	}
	//	�\���p���ʂ̐ݒ�
	ave = dm_vsman_a_ave[ rank[0] ][0];
	for( i = 0,save = 0;i < 8;i++ ){
		if( dm_vsman_a_ave[ rank[i] ][0] != ave ){
			ave = dm_vsman_a_ave[ rank[i] ][0];
			save++;
		}
		dm_dmr_r[DAM_M_VSMAN][i] = save;
	}

	//	���ڃR�s�[�����f�[�^�͎g�킸�A�ԐڎQ�Ƃ��s��(��̂���)
	for(i = 0;i < 8;i++){
		dm_dmr_d[DAM_M_VSMAN][i] = rank[i];
	}

}


/*--------------------------------------
	�L�^�����������֐�
--------------------------------------*/
void	init_dm_data_mode(void)
{
	s16	i,j;

	for( i = 0;i < 2;i++ ){
		dm_data_mode_menu[i]		=	DAM_ALPHA_IN;	//	��������
		dm_data_mode_disp_menu[i]	=	0;
		dm_data_mode_count[i]		=	0;
	}
	dm_data_mode_cursor_pos[0]	=	0;	//	���j���[(���E)�̃J�[�\���ʒu
	dm_data_mode_cursor_pos[1]	=	1;	//	�l(�㉺)�̃J�[�\���ʒu
	dm_data_mode_c_flash_count[0] = 0;	//	���_�ŗp�̃A���t�@�l
	dm_data_mode_c_flash_count[1] = 0;	//	���_�ŗp�̃J�E���^
	dm_data_mode_flash_count  = 0;		//	�_�ŃJ�E���^�̏�����
	dm_data_mode_plate_size[0] = 16;	//	�v���[�g�̏����c��
	dm_data_mode_plate_size[1] = -4;	//	�g��
	dm_data_mode_personal_pos = 320;	//	�l�f�[�^�̏������W

	//	���j���[�̍��\�ݒ�
	for( i = 0,j = 24;i < 4;i++ ){
		dm_data_mode_menu_pos[i][0] = dm_data_mode_menu_pos[i][1] = j;
		j += 68;
	}

	//	�w���W�ƃ��j���[�ԍ��ݒ�
	for( i = 0;i < 9;i++ ){
		dm_data_mode_rank_pos[0].plate_pos[i][0] = 24;
		dm_data_mode_rank_pos[1].plate_pos[i][0] = 320;

	}
	//	�����\�����̐ݒ�
	dm_data_mode_rank_pos[0].menu_no = 0;	//	�X�g�[���[
	dm_data_mode_rank_pos[1].menu_no = 1;	//	���x���Z���N�g

	//	�x���W�̐ݒ�
	dm_data_mode_rank_pos[0].plate_pos[0][1] = 83;
	dm_data_mode_rank_pos[1].plate_pos[0][1] = 83;

	for( i = 1,j = 107;i < 9;i++ ){
		dm_data_mode_rank_pos[0].plate_pos[i][1] = j;
		dm_data_mode_rank_pos[1].plate_pos[i][1] = j;
		j += 16;
	}

	//	�e���ʌv�Z

	for( i = 0;i < 4;i++ ){
		for( j= 0;j < 8;j++ ){
			dm_dmr_d[i][j] = 0;
		}
	}

	dm_data_mode_story_sort();	//	�X�g�[���[���[�h�̃\�[�g
	dm_data_mode_level_sort();	//	���x�����[�h�̃\�[�g
	dm_data_vscom_sort();		//	VSCOM���[�h�̃\�[�g
	dm_data_vsman_sort();		//	2PLAY���[�h�̃\�[�g
}

/*--------------------------------------
	�L�^�������C���֐�
--------------------------------------*/
int		dm_data_mode_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	int	ret;
	s16	i,j,k;
	s8	out_flg = 1;	//	���[�v�E�o�p�t���O

	//	���b�Z�[�W�L���[�̍쐬
	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	���b�Z�[�W�p�m���蓖��
	//	�N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);

	//	�O���t�B�b�N�f�[�^�̓Ǎ���
	auRomDataRead((u32)_dm_data_mode_dataSegmentRomStart,(void *)_dm_data_mode_dataSegmentStart	, (u32)(_dm_data_mode_dataSegmentRomEnd - _dm_data_mode_dataSegmentRomStart));

	//	��ʐ؂�ւ�����������
//	init_change_scene();

	HCEffect_Init(gfx_freebuf[0], HC_CURTAIN);

	//	�L�^��ʏ�����
	init_dm_data_mode();

	//	SE ������
	dm_init_se();

	//	�O���t�B�b�N�̐ݒ�
	graphic_no = GFX_RANK;

	//	���C�����[�v
//	while( !check_change_scene() )

	while( out_flg )
	{
		#ifndef THREAD_JOY
		joyProcCore();
		#endif

		//	���g���[�X���b�Z�[�W
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);


		switch( dm_data_mode_menu[0] )
		{
		case	DAM_SCALE_CHANGE_OUT:
			//	�l�v���[�g�̈ړ�
			dm_data_mode_personal_pos += 16;
			if( dm_data_mode_personal_pos >= 320 ){
				dm_data_mode_personal_pos = 320;
				dm_data_mode_menu[0] = dm_data_mode_menu[1];
			}

			//	���j���[�̈ړ�
			if( dm_data_mode_rank_pos[0].menu_no != 0 ){
				if( dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] <= dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][1] ){
					dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] += 16;
					if( dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] >= dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][1] )
					{
						dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] = dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][1];
					}
				}
			}

			//	�l�[���v���[�g�̊g��k��
			dm_data_mode_plate_size[0] += dm_data_mode_plate_size[1];
			if( dm_data_mode_plate_size[0] <= 0 ){
				dm_data_mode_plate_size[1] = 4;
				dm_data_mode_disp_menu[0] = dm_data_mode_disp_menu[1];
			}else	if( dm_data_mode_plate_size[0] >= 16 )
			{
				dm_data_mode_plate_size[0] = 16;
			}
			break;
		case	DAM_SCALE_CHANGE_IN:
			//	�l�v���[�g�̈ړ�
			dm_data_mode_personal_pos -= 16;
			if( dm_data_mode_personal_pos <= 92 ){
				dm_data_mode_personal_pos = 92;
				dm_data_mode_menu[0] = dm_data_mode_menu[1];
			}
			//	���j���[�̈ړ�
			if( dm_data_mode_rank_pos[0].menu_no != 0 ){
				if( dm_data_mode_personal_pos <= dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][1] + 68 ){
					dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] -= 16;
					if( dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] <= 24 )
					{
						dm_data_mode_menu_pos[dm_data_mode_rank_pos[0].menu_no][0] = 24;
					}
				}
			}
			//	�l�[���v���[�g�̊g��k��
			dm_data_mode_plate_size[0] += dm_data_mode_plate_size[1];
			if( dm_data_mode_plate_size[0] <= 0 ){
				dm_data_mode_plate_size[1] = 4;
				dm_data_mode_disp_menu[0] = dm_data_mode_disp_menu[1];
			}else	if( dm_data_mode_plate_size[0] >= 16 )
			{
				dm_data_mode_plate_size[0] = 16;
			}
			break;
		case	DAM_SCROOL_CHANGE:
			//	�v���[�g�X�N���[��
			k = -8;
			for( i = 0;i < 2;i++ ){
				for( j = 0;j < 9;j++ ){
					if( j == 0 ){
						dm_data_mode_rank_pos[i].plate_pos[j][0] -= 16;
					}else{
						if( dm_data_mode_rank_pos[i].plate_pos[j - 1][0] < k ){
							dm_data_mode_rank_pos[i].plate_pos[j][0] -= 16;
						}
					}
				}
				k = 288;
			}

			//	���W�C��
			for( j = 0;j < 9;j++ ){
				if( dm_data_mode_rank_pos[1].plate_pos[j][0] <= 24 ){
					dm_data_mode_rank_pos[1].plate_pos[j][0] = 24;
				}
			}
			//	�X�N���[���I��
			if( dm_data_mode_rank_pos[1].plate_pos[8][0] == 24 ){
				for( i = 0;i < 9;i++ ){
					dm_data_mode_rank_pos[0].plate_pos[i][0] = 24;
					dm_data_mode_rank_pos[1].plate_pos[i][0] = 320;
//					dm_data_mode_rank_pos[0].name_no[i] = dm_data_mode_rank_pos[1].name_no[i];
				}
				dm_data_mode_rank_pos[0].menu_no = dm_data_mode_rank_pos[1].menu_no;	//	�\�����̕ύX
				dm_data_mode_menu[0] = DAM_ALL_MAIN;	//	�����̕ύX
			}
			break;
		case	DAM_ALPHA_OUT:
			break;
		case	DAM_ALPHA_IN:
			if(	!HCEffect_Main() ){		//	��ʐ؂�ւ��������I������܂�
				dm_data_mode_menu[0] = DAM_ALL_MAIN;
			}
			break;
		case	DAM_ALL_MAIN:	//	�ʏ�
			//	�J�[�\���̓_�ŗp
			dm_data_mode_flash_count ++;
			if( dm_data_mode_flash_count >= 60 ){
				dm_data_mode_flash_count = 0;
			}
			//	���̓_�ŗp
			dm_data_mode_c_flash_count[0]  = (s16)(sinf( DEGREE( dm_data_mode_c_flash_count[1] ) ) * 64 ) + 191;
			dm_data_mode_c_flash_count[1] += 10;
			if( dm_data_mode_c_flash_count[1] >= 360 ){
				dm_data_mode_c_flash_count[1] -= 360;
			}

			if( joycur[main_joy[0]] & R_JPAD ){	//	�E
				if( dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ] < 3 ){	//	�X�N���[��
					dm_set_menu_se( SE_mLeftRight );

					dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ] ++;

					dm_data_mode_rank_pos[1].menu_no = dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ];
					dm_data_mode_menu[0] = DAM_SCROOL_CHANGE;

				}
			}else	if( joycur[main_joy[0]] & L_JPAD ){	//	��
				if( dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ] > 0 ){	//	�X�N���[��
					dm_set_menu_se( SE_mLeftRight );

					dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ] --;

					dm_data_mode_rank_pos[1].menu_no = dm_data_mode_cursor_pos[ DAM_MENU_CURSOR ];
					dm_data_mode_menu[0] = DAM_SCROOL_CHANGE;
				}
			}else	if( joycur[main_joy[0]] & U_JPAD ){	//	��
				if( dm_data_mode_cursor_pos[ DAM_PLAYER_CURSOR ] > 1 ){
					dm_set_menu_se( SE_mUpDown );
					dm_data_mode_cursor_pos[ DAM_PLAYER_CURSOR ] --;
				}
			}else	if( joycur[main_joy[0]] & D_JPAD ){	//	��
				if( dm_data_mode_cursor_pos[ DAM_PLAYER_CURSOR ] < 8 ){
					dm_set_menu_se( SE_mUpDown );
					dm_data_mode_cursor_pos[ DAM_PLAYER_CURSOR ] ++;
				}
			}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
				dm_set_menu_se( SE_mDecide );
				dm_data_mode_plate_size[1] = -4;
				dm_data_mode_menu[0] = DAM_SCALE_CHANGE_IN;			//	�l�f�[�^��
				dm_data_mode_menu[1] = dm_data_mode_disp_menu[1] = DAM_MENU_PERSONAL;	//	�k����l�f�[�^�`���
			}else	if( joyupd[main_joy[0]] & B_BUTTON ){
				dm_data_mode_menu[0] = DAM_ALPHA_OUT;
				dm_data_mode_disp_menu[1] = DAM_ALPHA_OUT;
//				start_change_scene(3);					//	��ʐ؂�ւ������J�n
				out_flg = 0;
			}
			break;
		case	DAM_PERSONAL_MAIN:
			if( joyupd[main_joy[0]] & B_BUTTON ){
				dm_set_menu_se( SE_mDecide );

				dm_data_mode_plate_size[1] = -4;
				dm_data_mode_menu[0] = DAM_SCALE_CHANGE_OUT;	//	�S���f�[�^��
				dm_data_mode_menu[1] = dm_data_mode_disp_menu[1] = DAM_MENU_ALL;	//	�k����S���f�[�^�`���
			}
			break;
		}

		dm_menu_se_play();
	}

	//	���y��~
//	auSeqPlayerStop(0);

	//	�\����~
	graphic_no = GFX_NULL;

	//	�O���t�B�b�N�I���҂�
	while( pendingGFX != 0 ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	//	�N���C�A���g�̖���
	nnScRemoveClient(sched, &client);

	main_old = MAIN_RANK;
	return	MAIN_MODE_SELECT;	//	���[�h�Z���N�g
}

/*--------------------------------------
	�L�^�`�惁�C���֐�
--------------------------------------*/
void	dm_data_mode_graphic(void)
{
	NNScTask*	gt;
	int			i,j,k,l;
	s8			size;
	rank_pos	*rank_p;

	// �ި���ڲؽ��ޯ̧�����ޯ̧�̎w��
	gp = &gfx_glist[gfx_gtask_no][0];
	gt = &gfx_task[gfx_gtask_no];

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT,	 osVirtualToPhysical(gfx_freebuf[0]));


	// RSP �����ݒ�
	F3RCPinitRtn();
	F3ClearFZRtn(0);

	//	�`��͈͂̎w��
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	�������ݒ�
	gSPDisplayList(gp++, Texture_TE_dl );

	//	�w�i�`��

	load_TexPal(dm_bg_tile_pal_2_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	�c��
	{
		for(j = 0;j < 16;j++)	//	����
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	//	�{�^�������̕`��
	load_TexPal(dm_mode_select_bt_ab_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_bt_ab_bm0_0,80,32);
	draw_Tex(208,12,80,32,0,0);

	//	���j���[���̕`��

	load_TexPal(dm_mode_select_menu_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_menu_bm0_0,160,25);
	draw_Tex(32,12,160,25,0,0);
	load_TexBlock_4b(&dm_mode_select_menu_bm0_0[2000],160,6);
	draw_Tex(32,37,160,6,0,0);


	//	���j���[�`��
	for( i = 0;i < 4;i++ ){
		load_TexPal( dm_data_mode_menu_p[i] );
		load_TexBlock_4b( dm_data_mode_menu_g[i],64,16 );
		draw_Tex( dm_data_mode_menu_pos[i][0],51,64,16,0,0 );
	}


	switch( dm_data_mode_disp_menu[0] ){
	case	DAM_MENU_ALL:
	case	DAM_ALPHA_OUT:
		gDPPipeSync(gp++);
		gDPSetRenderMode(gp++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
		j = ( ( dm_data_mode_flash_count/30 ) % 2 ) * 127 + 128;
		gDPSetPrimColor(gp++,0,0,j,j,j,255);
		break;
	}

	//	�J�[�\���`��
	load_TexPal( dm_data_mode_menu_cursor_bm0_0tlut );
	load_TexBlock_4b( dm_data_mode_menu_cursor_bm0_0,64,16 );
	draw_Tex( dm_data_mode_menu_pos[dm_data_mode_cursor_pos[DAM_MENU_CURSOR]][0],51,64,16,0,0 );

	//	���`��
	switch( dm_data_mode_disp_menu[0] )
	{
	case	DAM_MENU_ALL:	//	�S��
	case	DAM_ALPHA_OUT:	//	�E�o
		load_TexPal( arrow_r_bm0_0tlut );
		load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );
		gDPSetPrimColor(gp++,0,0,dm_data_mode_c_flash_count[0],dm_data_mode_c_flash_count[0],dm_data_mode_c_flash_count[0],255);
		if( dm_data_mode_cursor_pos[DAM_MENU_CURSOR] < 3 ){		//	�E���
			draw_Tex(dm_data_mode_menu_pos[dm_data_mode_cursor_pos[DAM_MENU_CURSOR]][0] + 64 + ( ( dm_data_mode_flash_count/15 ) % 2 ),53,8,11,0,0 );
		}
		if( dm_data_mode_cursor_pos[DAM_MENU_CURSOR]  > 0 ){	//	�����
			draw_TexFlip(dm_data_mode_menu_pos[dm_data_mode_cursor_pos[DAM_MENU_CURSOR]][0] - 8 - ( ( dm_data_mode_flash_count/15 ) % 2 ) ,53,8,11,0,0, flip_on,flip_off );
		}
		gDPPipeSync(gp++);
		gDPSetRenderMode(gp++,G_RM_TEX_EDGE,G_RM_TEX_EDGE2);
		gDPSetPrimColor(gp++,0,0,255,255,255,255);
		break;
	}


	//	�l�f�[�^�v���[�g�`��
	load_TexTile_4b( dm_data_mode_personal_bm0_0,204,16,0,0,203,15 );
	load_TexPal( dm_data_mode_personal_p[dm_data_mode_rank_pos[0].menu_no] );
	draw_Tex( dm_data_mode_personal_pos,51,204,16,0,0);


	switch( dm_data_mode_disp_menu[0] )
	{
	case	DAM_MENU_ALL:	//	�S��
	case	DAM_ALPHA_OUT:	//	�E�o

		if( dm_data_mode_disp_menu[0] == DAM_MENU_ALL ){
			l = 2;
		}else	if( dm_data_mode_disp_menu[0] == DAM_ALPHA_OUT ){
			l = 1;
		}

		//	�v���[�g�`��
		if( dm_data_mode_plate_size[0] > 0 )
		{
			load_TexTile_4b( dm_data_mode_rank_plate_bm0_0,138,16,0,0,137,15 );

			size = dm_data_mode_plate_size[0] >> 1;

			for(i = 0;i < l;i++){
				load_TexPal( dm_data_mode_rank_plate_p[dm_data_mode_rank_pos[i].menu_no] );
				for( j = 1;j < 9;j++ ){
					draw_ScaleTex( dm_data_mode_rank_pos[i].plate_pos[j][0],dm_data_mode_rank_pos[i].plate_pos[j][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
					draw_TexScaleFlip( dm_data_mode_rank_pos[i].plate_pos[j][0] + 136,dm_data_mode_rank_pos[i].plate_pos[j][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );
				}

				gDPPipeSync(gp++);
				gDPSetRenderMode(gp++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
				gDPSetPrimColor(gp++,0,0,255,255,255,196);

				load_TexPal( dm_data_mode_rank_plate_p[4] );
				draw_ScaleTex( dm_data_mode_rank_pos[i].plate_pos[0][0],dm_data_mode_rank_pos[i].plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
				draw_TexScaleFlip( dm_data_mode_rank_pos[i].plate_pos[0][0] + 136,dm_data_mode_rank_pos[i].plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );

				gDPPipeSync(gp++);
				gDPSetRenderMode(gp++,G_RM_TEX_EDGE,G_RM_TEX_EDGE2);
				gDPSetPrimColor(gp++,0,0,255,255,255,255);

				load_TexPal( dm_data_mode_d_plate_p[dm_data_mode_rank_pos[i].menu_no] );
				draw_ScaleTex( dm_data_mode_rank_pos[i].plate_pos[0][0],dm_data_mode_rank_pos[i].plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
				draw_TexScaleFlip( dm_data_mode_rank_pos[i].plate_pos[0][0] + 136,dm_data_mode_rank_pos[i].plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );
			}

			//	�ԍ�(����)�`��
			load_TexBlock_4b( dm_data_mode_rank_bm0_0,16,128 );
			for(i = 0;i < 2;i++){
				rank_p = &dm_data_mode_rank_pos[i];
				load_TexPal( dm_data_mode_rank_p[rank_p -> menu_no] );
				for( j = 1;j < 9;j++ ){
					draw_ScaleTex( rank_p -> plate_pos[j][0],rank_p -> plate_pos[j][1] - size,16,16,16,dm_data_mode_plate_size[0],0,dm_dmr_r[rank_p -> menu_no ][j - 1] << 4 );
				}
			}


			//	��ڕ`��
			for(i = 0;i < 2;i++){
				gDPPipeSync(gp++);
				gDPSetRenderMode(gp++,G_RM_TEX_EDGE,G_RM_TEX_EDGE2);
				gDPSetPrimColor(gp++,0,0,255,255,255,255);

				load_TexPal( dm_data_mode_d_p[dm_data_mode_rank_pos[i].menu_no] );

				rank_p = &dm_data_mode_rank_pos[i];
				switch( rank_p -> menu_no )
				{
				case	DAM_M_STORY://	�X�g�[���[
					for( j = 0;j < 4;j++ ){
						//	�e�N�X�`�����[�h
						load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_story_all[j][1] ],
												dm_data_mode_d_g_size[ dm_data_d_pos_story_all[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_story_all[j][1] ] - 1,15 );
						//	�e�N�X�`���`��
						draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_story_all[j][0],rank_p -> plate_pos[0][1] - size,
										dm_data_mode_d_g_size[ dm_data_d_pos_story_all[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_story_all[j][1]],dm_data_mode_plate_size[0],0,0 );
					}


					//	�Q�[�����x���`��
					load_TexPal( dm_data_mode_game_level_bm0_0tlut );
					load_TexTile_4b( dm_data_mode_game_level_bm0_0, 46,48,0,0,45,47 );
					for( j = 0;j < 8;j++ )
					{
						draw_ScaleTex( rank_p -> plate_pos[j + 1][0] + 209,rank_p -> plate_pos[j + 1][1] - size,46,16,46,dm_data_mode_plate_size[0],0,
												evs_mem_data[dm_dmr_d[DAM_M_STORY][j]].story_data[0].g_level << 4 );
					}
					//	�R���}�`��
					load_TexTile_4b( dm_data_mode_conma_bm0_0, 4,16,0,0,3,15 );
					for( j = 0;j < 8;j++ )
					{
						draw_ScaleTex( rank_p -> plate_pos[j + 1][0] + 179,rank_p -> plate_pos[j + 1][1] - size,4,16,4,dm_data_mode_plate_size[0],0,0 );
					}

					dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
					//	�����`��
					for( j = 0;j < 8;j++ )
					{
						//	���_
						dm_data_mode_disp_score(evs_mem_data[dm_dmr_d[DAM_M_STORY][j]].story_data[0].score,rank_p -> plate_pos[j + 1][0] + 153,rank_p -> plate_pos[j + 1][1],dm_data_mode_plate_size[0] );
						//	����(��)
						dm_data_mode_disp_num_no_sift(dm_dm_a_time[dm_dmr_d[DAM_M_STORY][j]][0],rank_p -> plate_pos[j + 1][0] + 177,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
						//	����(�b)
						dm_data_mode_disp_num_no_sift(dm_dm_a_time[dm_dmr_d[DAM_M_STORY][j]][1],rank_p -> plate_pos[j + 1][0] + 201,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0]);
					}
					break;
				case	DAM_M_LEVEL://	���x���Z���N�g
					for( j = 0;j < 3;j++ ){
						//	�e�N�X�`�����[�h
						load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_level_all[j][1] ],
												dm_data_mode_d_g_size[ dm_data_d_pos_level_all[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_level_all[j][1] ] - 1,15 );
						//	�e�N�X�`���`��
						draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_level_all[j][0],rank_p -> plate_pos[0][1] - size,
										dm_data_mode_d_g_size[ dm_data_d_pos_level_all[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_level_all[j][1]],dm_data_mode_plate_size[0],0,0 );
					}

					//	�Q�[���X�X�s�[�h�`��
					load_TexPal( dm_data_mode_game_spead_bm0_0tlut );
					load_TexTile_4b( dm_data_mode_game_spead_bm0_0, 24,48,0,0,23,47 );
					for( j = 0;j < 8;j++ )
					{
						draw_ScaleTex( rank_p -> plate_pos[j + 1][0] + 226,rank_p -> plate_pos[j + 1][1] - size,24,16,24,dm_data_mode_plate_size[0],0,
												evs_mem_data[dm_dmr_d[DAM_M_LEVEL][j]].level_data[0].speed << 4 );

					}

					//	LV�`��
					load_TexPal( dm_data_mode_rank_p[1] );
					load_TexBlock_4b( dm_data_mode_level_bm0_0,16,16 );
					for( j = 0;j < 8;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j + 1][0] + 133,rank_p -> plate_pos[j + 1][1] - size,16,16,16,dm_data_mode_plate_size[0],0,0);
					}

					dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
					//	�����`��
					for( j = 0;j < 8;j++ )
					{
						//	�N���A���x��
						dm_data_mode_disp_num_no_sift( evs_mem_data[dm_dmr_d[DAM_M_LEVEL][j]].level_data[0].c_level,rank_p -> plate_pos[j + 1][0] + 172,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
					}
					break;
				case	DAM_M_VSCOM://	�u�r�b�n�l
					for( j = 0;j < 4;j++ ){
						//	�e�N�X�`�����[�h
						load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_vscom_all[j][1] ],
												dm_data_mode_d_g_size[ dm_data_d_pos_vscom_all[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_vscom_all[j][1] ] - 1,15 );
						//	�e�N�X�`���`��
						draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_vscom_all[j][0],rank_p -> plate_pos[0][1] - size,
										dm_data_mode_d_g_size[ dm_data_d_pos_vscom_all[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_vscom_all[j][1]],dm_data_mode_plate_size[0],0,0 );
					}
					//	���`��
					load_TexPal( dm_data_mode_persent_y_bm0_0tlut );
					load_TexTile_4b( dm_data_mode_persent_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 131,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}
					//	���`��
					load_TexTile_4b( dm_data_mode_maru_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 163,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}
					//	�w�`��
					load_TexTile_4b( dm_data_mode_batu_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 219,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}

					dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
					//	�����`��
					for( j = 0;j < 8;j++ ){
						//	�V���E���c
						dm_data_mode_disp_num_no_sift( dm_vscom_a_ave[dm_dmr_d[DAM_M_VSCOM][j]][0],rank_p -> plate_pos[j + 1][0] + 128,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( dm_vscom_a_ave[dm_dmr_d[DAM_M_VSCOM][j]][1],rank_p -> plate_pos[j + 1][0] + 200,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( dm_vscom_a_ave[dm_dmr_d[DAM_M_VSCOM][j]][2],rank_p -> plate_pos[j + 1][0] + 256,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
					}

					break;
				case	DAM_M_VSMAN://	2PLAY
					for( j = 0;j < 4;j++ ){
						//	�e�N�X�`�����[�h
						load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_vs_all[j][1] ],
												dm_data_mode_d_g_size[ dm_data_d_pos_vs_all[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_vs_all[j][1] ] - 1,15 );
						//	�e�N�X�`���`��
						draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_vs_all[j][0],rank_p -> plate_pos[0][1] - size,
										dm_data_mode_d_g_size[ dm_data_d_pos_vs_all[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_vs_all[j][1]],dm_data_mode_plate_size[0],0,0 );

					}
					//	���`��
					load_TexPal( dm_data_mode_persent_g_bm0_0tlut );
					load_TexTile_4b( dm_data_mode_persent_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 131,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}
					//	���`��
					load_TexTile_4b( dm_data_mode_maru_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 163,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}
					//	�~�`��
					load_TexTile_4b( dm_data_mode_batu_bm0_0,12,16,0,0,11,15 );
					for( j = 1;j < 9;j++ ){
						draw_ScaleTex( rank_p -> plate_pos[j][0] + 219,rank_p -> plate_pos[j][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
					}

					dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
					//	�����`��
					for( j = 0;j < 8;j++ ){
						//	�V���E���c
						dm_data_mode_disp_num_no_sift( dm_vsman_a_ave[dm_dmr_d[DAM_M_VSMAN][j]][0],rank_p -> plate_pos[j + 1][0] + 128,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( dm_vsman_a_ave[dm_dmr_d[DAM_M_VSMAN][j]][0],rank_p -> plate_pos[j + 1][0] + 200,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( dm_vsman_a_ave[dm_dmr_d[DAM_M_VSMAN][j]][0],rank_p -> plate_pos[j + 1][0] + 256,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
					}

					break;
				}

				//	���O�`��
				//	�C���e���V�e�B���g������
//				gSPDisplayList(gp++, Intensity_XNZ_Texture_dl);
//				gDPSetEnvColor(gp++,64,64,64,mode_select_alpha);
//				gDPSetPrimColor(gp++,0,0,255,255,255,255);


				load_font_pal( 1 );
				k = dm_data_mode_plate_size[0];
				if( k >= 12 ){
					k = 12;
				}
				if( k > 0 ){
					for( j = 0;j < 8;j++ ){
						draw_font_scale_name_12( rank_p -> plate_pos[j + 1][0] + 34,rank_p -> plate_pos[j + 1][1],
							k,evs_mem_data[ dm_dmr_d[rank_p -> menu_no][j] ].mem_name );
					}
				}

				//	�������ݒ�
//				gSPDisplayList(gp++, prev_halftrans_Ab_dl );

			}

			gDPPipeSync(gp++);
			gDPSetRenderMode(gp++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
			j = ( ( dm_data_mode_flash_count/30 ) % 2 ) * 127 + 128;
			gDPSetPrimColor(gp++,0,0,j,j,j,255);

			//	�J�[�\���`��
			load_TexPal( dm_data_mode_menu_cursor_bm0_0tlut );
			load_TexTile_4b( dm_data_mode_plate_cursor_bm0_0,138,18,0,0,137,17 );
			for(i = 0;i < 2;i++){
				rank_p = &dm_data_mode_rank_pos[i];
				draw_ScaleTex( rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][0],
								rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][1] - size - 1,138,18,138,dm_data_mode_plate_size[0] + 2,0,0 );

				draw_TexScaleFlip( rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][0] + 136,
									rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][1] - size - 1,138,18,138,dm_data_mode_plate_size[0] + 2,0,0,138,18,flip_on,flip_off );
			}
			//	���`��
			if( dm_data_mode_plate_size[0] == 16 ){
				load_TexPal( arrow_u_bm0_0tlut );
				load_TexBlock_4b( arrow_u_bm0_0,16,7 );
				gDPSetPrimColor(gp++,0,0,dm_data_mode_c_flash_count[0],dm_data_mode_c_flash_count[0],dm_data_mode_c_flash_count[0],255);
				for(i = 0;i < 2;i++){
					rank_p = &dm_data_mode_rank_pos[i];
					if( dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] < 8 ){	//	�����
						draw_TexFlip(rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][0] + 130,
										rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][1] + 9 + ( ( dm_data_mode_flash_count/15 ) % 2 ),16,7,0,0, flip_off,flip_on );
					}
					if( dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR]  > 1 ){	//	����
						draw_Tex(rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][0] + 130,
										rank_p -> plate_pos[ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] ][1] - 16 - ( ( dm_data_mode_flash_count/15 ) % 2 ),16,7,0,0 );
					}
				}
			}
		}
		break;
	case	DAM_MENU_PERSONAL:	//	�l
		//	�v���[�g�`��
		rank_p = &dm_data_mode_rank_pos[0];
		if( dm_data_mode_plate_size[0] > 0 )
		{
			if( rank_p -> menu_no == DAM_M_VSMAN ){
				k = 8;
			}else{
				k = 6;
			}


			load_TexTile_4b( dm_data_mode_rank_plate_bm0_0,138,16,0,0,137,15 );

			size = dm_data_mode_plate_size[0] >> 1;

			if( rank_p -> menu_no != DAM_M_VSCOM ){

				load_TexPal( dm_data_mode_rank_plate_p[rank_p -> menu_no] );
				for( j = 1;j < k;j++ ){
					draw_ScaleTex( rank_p -> plate_pos[j][0],rank_p -> plate_pos[j][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
					draw_TexScaleFlip( rank_p -> plate_pos[j][0] + 136,rank_p -> plate_pos[j][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );
				}
			}

			gDPPipeSync(gp++);
			gDPSetRenderMode(gp++,G_RM_XLU_SURF,G_RM_XLU_SURF2);
			gDPSetPrimColor(gp++,0,0,255,255,255,196);

			load_TexPal( dm_data_mode_rank_plate_p[4] );
			draw_ScaleTex( rank_p -> plate_pos[0][0],rank_p -> plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
			draw_TexScaleFlip( rank_p -> plate_pos[0][0] + 136,rank_p -> plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );

			gDPPipeSync(gp++);
			gDPSetRenderMode(gp++,G_RM_TEX_EDGE,G_RM_TEX_EDGE2);
			gDPSetPrimColor(gp++,0,0,255,255,255,255);

			load_TexPal( dm_data_mode_d_plate_p[rank_p -> menu_no] );
			draw_ScaleTex( rank_p -> plate_pos[0][0],rank_p -> plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0 );
			draw_TexScaleFlip( rank_p -> plate_pos[0][0] + 136,rank_p -> plate_pos[0][1] - size,138,16,138,dm_data_mode_plate_size[0],0,0,138,16,flip_on,flip_off );


			if( rank_p -> menu_no == DAM_M_STORY || rank_p -> menu_no == DAM_M_LEVEL ){
				//	�ԍ�(����)�`��
				load_TexBlock_4b( dm_data_mode_rank_bm0_0,16,128 );
				load_TexPal( dm_data_mode_rank_p[rank_p -> menu_no] );
				for( j = 1;j < k;j++ ){
					draw_ScaleTex( rank_p -> plate_pos[j][0],rank_p -> plate_pos[j][1] - size,16,16,16,dm_data_mode_plate_size[0],0,
									dm_dmr_sl_r[rank_p -> menu_no][dm_dmr_d[rank_p -> menu_no][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]][j - 1] << 4 );
				}
			}

			//	��ڕ`��
			load_TexPal( dm_data_mode_d_p[rank_p -> menu_no] );

			switch( rank_p -> menu_no )
			{
			case	DAM_M_STORY://	�X�g�[���[
				for( j = 0;j < 5;j++ ){
					//	�e�N�X�`�����[�h
					load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_story_personal[j][1] ],
										dm_data_mode_d_g_size[ dm_data_d_pos_story_personal[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_story_personal[j][1] ] - 1,15 );
					//	�e�N�X�`���`��
					draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_story_personal[j][0],rank_p -> plate_pos[0][1] - size,
										dm_data_mode_d_g_size[ dm_data_d_pos_story_personal[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_story_personal[j][1]],dm_data_mode_plate_size[0],0,0 );
				}

				//	�Q�[�����x���`��
				load_TexPal( dm_data_mode_game_level_bm0_0tlut );
				load_TexTile_4b( dm_data_mode_game_level_bm0_0, 46,48,0,0,45,47 );
				for( j = 0;j < 5;j++ )
				{
					draw_ScaleTex( 161,rank_p -> plate_pos[j + 1][1] - size,46,16,46,dm_data_mode_plate_size[0],0,
									evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].g_level << 4 );

				}
				//	ALL �`��
				load_TexTile_4b( dm_data_mode_all_bm0_0, 24,16,0,0,23,16 );
				for( j = 0;j < 5;j++ )
				{
					if( evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].c_stage == 0xff ){
						draw_ScaleTex( 218,rank_p -> plate_pos[j + 1][1] - size,24,16,24,dm_data_mode_plate_size[0],0,0);

					}
				}
				//	�R���}�`��
				load_TexTile_4b( dm_data_mode_conma_bm0_0, 4,16,0,0,3,15 );
				for( j = 0;j < 5;j++ )
				{
					draw_ScaleTex( 131,rank_p -> plate_pos[j + 1][1] - size,4,16,4,dm_data_mode_plate_size[0],0,0 );
				}


				dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
				for( j = 0;j < 5;j++ )
				{
					//	���_
					dm_data_mode_disp_score(evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].score,
															105,rank_p -> plate_pos[j + 1][1],dm_data_mode_plate_size[0] );
					//	����(��)
					dm_data_mode_disp_num_no_sift(dm_dm_p_time[dm_dmr_d[DAM_M_STORY][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]][j][0],
															129,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
					//	����(�b)
					dm_data_mode_disp_num_no_sift(dm_dm_p_time[dm_dmr_d[DAM_M_STORY][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]][j][1],
															153,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
					//	�X�e�[�W
					if( evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].c_stage != 0xff ){
						dm_data_mode_disp_num_no_sift(evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].c_stage,
																238,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
					}
					//	���g���C
					dm_data_mode_disp_num_no_sift(evs_mem_data[ dm_dmr_d[DAM_M_STORY][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].story_data[j].retry
															,276,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
				}
				break;
			case	DAM_M_LEVEL://	���x���Z���N�g
				for( j = 0;j < 4;j++ ){
					//	�e�N�X�`�����[�h
					load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_level_personal[j][1] ],
											dm_data_mode_d_g_size[ dm_data_d_pos_level_personal[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_level_personal[j][1] ] - 1,15 );
					//	�e�N�X�`���`��
					draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_level_personal[j][0],rank_p -> plate_pos[0][1] - size,
									dm_data_mode_d_g_size[ dm_data_d_pos_level_personal[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_level_personal[j][1]],dm_data_mode_plate_size[0],0,0 );
				}

				//	�Q�[���X�X�s�[�h�`��
				load_TexPal( dm_data_mode_game_spead_bm0_0tlut );
				load_TexTile_4b( dm_data_mode_game_spead_bm0_0, 24,48,0,0,23,47 );
				for( j = 0;j < 5;j++ )
				{
					draw_ScaleTex( 137,rank_p -> plate_pos[j + 1][1] - size,24,16,24,dm_data_mode_plate_size[0],0,
										evs_mem_data[ dm_dmr_d[DAM_M_LEVEL][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR]-1 ] ].level_data[j].speed << 4 );
				}

				//	LV�`��
				load_TexPal( dm_data_mode_rank_p[1] );
				load_TexBlock_4b( dm_data_mode_level_bm0_0,16,16 );
				for( i = 1;i < 6;i++ ){
					draw_ScaleTex( 61,rank_p -> plate_pos[i][1] - size,16,16,16,dm_data_mode_plate_size[0],0,0 );
				}

				dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
				//	�����`��
				for( j = 0;j < 5;j++ )
				{
					//	���_
					dm_data_mode_disp_score(evs_mem_data[ dm_dmr_d[DAM_M_LEVEL][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].level_data[j].score,
																240,rank_p -> plate_pos[j + 1][1],dm_data_mode_plate_size[0] );
					//	�N���A���x��
					dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_LEVEL][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].level_data[j].c_level,
																100,rank_p -> plate_pos[j + 1][1],2,dm_data_mode_plate_size[0] );
					//	���g���C
					dm_data_mode_disp_num_no_sift(evs_mem_data[ dm_dmr_d[DAM_M_LEVEL][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].level_data[j].retry,
																280,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
				}

				break;
			case	DAM_M_VSCOM://	�u�r�b�n�l
				for( j = 0;j < 6;j++ ){
					//	�e�N�X�`�����[�h
					load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_vscom_personal[j][1] ],
											dm_data_mode_d_g_size[ dm_data_d_pos_vscom_personal[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_vscom_personal[j][1] ] - 1,15 );
					//	�e�N�X�`���`��
					draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_vscom_personal[j][0],rank_p -> plate_pos[0][1] - size,
									dm_data_mode_d_g_size[ dm_data_d_pos_vscom_personal[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_vscom_personal[j][1]],dm_data_mode_plate_size[0],0,0 );
				}
				//	VSCOM�p�v���[�g�̕`��
				load_TexPal( dm_data_mode_rank_plate_p[rank_p -> menu_no] );
				load_TexTile_4b( dm_data_mode_rank_plate_vs_bm0_0,138,22,0,0,137,21 );
				for( i = 0, k = 104;i < 6;i++,k += 21 ){
					//	�e�N�X�`���`��
					draw_ScaleTex( 23,k - size - 3,138,22,138,dm_data_mode_plate_size[0] + 6,0,0 );
				}
				for( i = 0,j = 1, k = 104;i < 5 + evs_secret_flg ;i++,k += 21 ){
					//	�e�N�X�`���`��
					draw_ScaleTex( 160,k - size - 3,138,22,138,dm_data_mode_plate_size[0] + 6,0,0 );
				}
				//	��`��
				for( i = 0,j = 0, k = 104;i < 6 ;i++,j += 2,k += 21 ){
					load_TexPal( face_4p_game_pal_data[j] );
					load_TexBlock_4b( face_4p_game_data[j],16,16 );
					draw_ScaleTex( 35,k - size,16,16,16,dm_data_mode_plate_size[0],0,0 );
				}
				for( i = 0,j = 1, k = 104;i < 5 + evs_secret_flg;i++,j += 2,k += 21 ){
					load_TexPal( face_4p_game_pal_data[j] );
					load_TexBlock_4b( face_4p_game_data[j],16,16 );
					draw_ScaleTex( 171,k - size,16,16,16,dm_data_mode_plate_size[0],0,0 );
				}

				load_TexPal( dm_data_mode_persent_y_bm0_0tlut );
				//	���`��
				load_TexTile_4b( dm_data_mode_maru_bm0_0,12,16,0,0,11,15 );
				for( i = 0,k = 104;i < 6 ;i++,k += 21 ){
					draw_ScaleTex( 62,k - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}
				for( i = 0,k = 104;i < 5 + evs_secret_flg;i++,k += 21 ){
					draw_ScaleTex( 199,k - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}
				//	�~�`��
				load_TexTile_4b( dm_data_mode_batu_bm0_0,12,16,0,0,11,15 );
				for( i = 0,k = 104;i < 6;i++,k += 21 ){
					draw_ScaleTex( 114,k - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}
				for( i = 0,k = 104;i < 5 + evs_secret_flg;i++,k += 21 ){
					draw_ScaleTex( 251,k - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}

				dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
				//	�����`��
				for( i = j = 0,k = 104;i < 6;i++,k += 21,j += 2 ){
					//	����
					dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSCOM][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]].vscom_data[j][0],99,k,3,dm_data_mode_plate_size[0] );
					//	����
					dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSCOM][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]].vscom_data[j][1],151,k,3,dm_data_mode_plate_size[0] );
				}
				for( i = 0,j = 1,k = 104;i < 5 + evs_secret_flg;i++,k += 21,j += 2 ){
					//	����
					dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSCOM][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]].vscom_data[j][0],236,k,3,dm_data_mode_plate_size[0] );
					//	����
					dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSCOM][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]].vscom_data[j][1],288,k,3,dm_data_mode_plate_size[0] );
				}

				break;
			case	DAM_M_VSMAN://	2PLAY
				for( j = 0;j < 4;j++ ){
					//	�e�N�X�`�����[�h
					load_TexTile_4b( dm_data_mode_d_g[ dm_data_d_pos_vs_personal[j][1] ],
											dm_data_mode_d_g_size[ dm_data_d_pos_vs_personal[j][1] ],16,0,0,dm_data_mode_d_g_size[ dm_data_d_pos_vs_personal[j][1] ] - 1,15 );
					//	�e�N�X�`���`��
					draw_ScaleTex( rank_p -> plate_pos[0][0] + dm_data_d_pos_vs_personal[j][0],rank_p -> plate_pos[0][1] - size,
									dm_data_mode_d_g_size[ dm_data_d_pos_vs_personal[j][1] ],16,dm_data_mode_d_g_size[ dm_data_d_pos_vs_personal[j][1]],dm_data_mode_plate_size[0],0,0 );
				}
				//	���`��
				load_TexPal( dm_data_mode_persent_g_bm0_0tlut );
				load_TexTile_4b( dm_data_mode_persent_bm0_0,12,16,0,0,11,15 );
				for( i = 1;i < 8;i++ ){
					draw_ScaleTex( 155,rank_p -> plate_pos[i][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}
				//	���`��
				load_TexTile_4b( dm_data_mode_maru_bm0_0,12,16,0,0,11,15 );
				for( i = 1;i < 8;i++ ){
					draw_ScaleTex( 187,rank_p -> plate_pos[i][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}
				//	�~�`��
				load_TexTile_4b( dm_data_mode_batu_bm0_0,12,16,0,0,11,15 );
				for( i = 1;i < 8;i++ ){
					draw_ScaleTex( 243,rank_p -> plate_pos[i][1] - size,12,16,12,dm_data_mode_plate_size[0],0,0 );
				}

				//	VS�`��
				load_TexPal( dm_data_mode_rank_p[3] );
				load_TexBlock_4b( dm_data_mode_vs_bm0_0,16,16 );
				for( i = 1;i < 8;i++ ){
					draw_ScaleTex( 35,rank_p -> plate_pos[i][1] - size,16,16,16,dm_data_mode_plate_size[0],0,0 );
				}

				dm_data_mode_num_load();	//	�����O���t�B�b�N���[�h
				//	�����`��
				for( i = j = 0;i < 8;i++ ){
					if( i != dm_dmr_d[DAM_M_VSMAN][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ){
						//	�V���E���c
						dm_data_mode_disp_num_no_sift( dm_vsman_p_ave[dm_dmr_d[DAM_M_VSMAN][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1]][j],152,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSMAN][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1 ] ].vs_data[j][0],224,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						//	����
						dm_data_mode_disp_num_no_sift( evs_mem_data[ dm_dmr_d[DAM_M_VSMAN][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1 ] ].vs_data[j][1],280,rank_p -> plate_pos[j + 1][1],3,dm_data_mode_plate_size[0] );
						j++;
					}
				}
				break;
			}
		}

		dm_data_mode_mini_num_load();	//	�����O���t�B�b�N���[�h
		//	�v���C��
		dm_data_mode_disp_mini_num_no_sift( evs_mem_data[ dm_dmr_d[rank_p -> menu_no][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].mode_play[rank_p -> menu_no],dm_data_mode_personal_pos + 131,59,3,16 );
		//	�A����
		dm_data_mode_disp_mini_num_no_sift( evs_mem_data[ dm_dmr_d[rank_p -> menu_no][ dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ].mode_max_chain[rank_p -> menu_no],dm_data_mode_personal_pos + 152,59,2,16 );

		//	���O�`��
		//	�C���e���V�e�B���g������
//		gSPDisplayList(gp++, Intensity_XNZ_Texture_dl);
//		gDPSetEnvColor(gp++,64,64,64,mode_select_alpha);
//		gDPSetPrimColor(gp++,0,0,255,255,255,255);

		//	�l�f�[�^���O
		load_font_pal( 1 );
		draw_font_scale_name_12( dm_data_mode_personal_pos + 5,59,12,
								evs_mem_data[ dm_dmr_d[rank_p -> menu_no][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1 ] ].mem_name );

		if( rank_p -> menu_no == DAM_M_VSMAN ){
			//	VSMAN�̏ꍇ
			k = dm_data_mode_plate_size[0];
			if( k >= 12 ){
				k = 12;
			}
			if( k > 0 ){
				for( i = j = 0;i < 8;i++ ){
					//	���O
					if( i != dm_dmr_d[DAM_M_VSMAN][dm_data_mode_cursor_pos[DAM_PLAYER_CURSOR] - 1] ){
						draw_font_scale_name_12( 58,rank_p -> plate_pos[j + 1][1],k,evs_mem_data[i].mem_name);
						j++;
					}
				}
			}
		}


		//	�������ݒ�
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );

		break;
	}

	//	��ʐ؂�ւ������֐�
  	HCEffect_Grap(&gp);

	// �ި���ڲؽč쐬�I��, ���̨������̊J�n
	gDPFullSync(gp++);			// �ި���ڲؽčŏI����
	gSPEndDisplayList(gp++);	// �ި���ڲؽďI�[
	osWritebackDCacheAll();		// dinamic�����Ă��ׯ��
	gfxTaskStart(gt, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);

}
