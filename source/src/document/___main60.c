// ///////////////////////////////////////////////////////////////////////////
//
//	Mode Select Program File
//
//	Author		: Katsuyuki Ohkura
//
//	Last Update	: 1999/11/23
//
//	modify		: 1999/12/11	Hiroyuki Watanabe
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"
#include "dm_code_data.h"

#include "hardcopy.h"
#include "o_segment.h"
#include "o_static.h"
#include "lv_sel_data.h"
#include "cap_pos.dat"

#define	CS_4P_SELECT_TEAM	0
#define	CS_4P_SELECT_MAN	1
#define	CS_4P_SELECT_LEVEL	2

#define	CS_STORY_SERECT_LEVEL	0
#define	CS_STORY_SERECT_STAGE	1

static s8 main_60_out_flg;

typedef struct {
	s16			bx;				// ��w���W
	s16			by;				// ��x���W
	u8 			pos;			// �ʒu
	u8			com_lv;			// �b�n�l���x��
	u8			stg_no;			// �X�e�[�W�ԍ�
	u8			mus_no;			// �Ȕԍ�
	s16			com_adj_y;		// �b�n�l���x���p�㉺�X�N���[��
	s16			stg_adj_y;		// �X�e�[�W�p�㉺�X�N���[���p
	s16			mus_adj_y;		// �ȗp�㉺�X�N���[���p
	u8			stg_tbl[16];	// �X�e�[�W�e�[�u��
	u8			stg_sel_num;	// �I���\�X�e�[�W��
} UNDER_ITEMS;

typedef struct {
	s8			move;			// �ړ����t���O
	int			count;			// �ړ��p�J�E���^
	float		sc[5];			// �X�P�[��
	int			pos_y[5];		// �x���W�ʒu
} Face_Data;

typedef struct {
	s16			bx;				// ��w���W
	s16			by;				// ��x���W
	u8			flag;			// �t���O
	s8			ok;				// ����ς�
	u8			play;			// �l�� or �b�n�l
	u8			lv;				// �I�����x��
	u8			speed;			// �I���X�s�[�h
	u8			chara;			// �I���L����
	u8			p_team;			// �`�[���Ґ�
	Face_Data	fd;				// ��f�[�^
} Player_Data;

typedef struct {
	u8			flow;			// ���ꐧ��
	u8			sub_flow;		// �T�u�̗��ꐧ��
	u8 			max_chara;		// �L�����N�^�[��
	u8			now_com;		// �b�n�l����p�J�E���^
	s8			coms[5];		// �b�n�l�ԍ��T��
	s16			adj_x;			// �S�̈ړ��p
	s16			mv_x;			// �ړ���
	u32			cnt;			// �A�j���p�J�E���^
	u8			max_lv;			// �P�o�p�l�`�w���x��
	u8			team;			//	�`�[���I��p
	Player_Data	pd[4];			// �v���C���[�f�[�^
	UNDER_ITEMS	ui;				// ���i����
} Mode_Sel;

Mode_Sel ms;

// ���i�p�l��
static PIC2 under_pl[3][3] = {
{{plate_env_bm0_0,plate_env_bm0_0tlut,256,42,0,0},				// �_�~�[
 {plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42,0,0},				// �_�~�[
 {plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,0,0}},				// �_�~�[
{{plate_env_bm0_0,plate_env_bm0_0tlut,256,42,0,0},				// �X�e�[�W�����y
 {plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42,0,0},				// �X�e�[�W�����y �Ñ�
 {plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,0,0}},				// �X�e�[�W�����y �Ï�
{{plate_e_scm_bm0_0,   plate_e_scm_bm0_0tlut  ,262,42,0,0},		// �X�e�[�W���b�n�l�k�u�����y
 {plate_e_sm262_bm0_0, plate_e_sm262_bm0_0tlut,262,42,0,0},		// �X�e�[�W���b�n�l�k�u�����y �Ñ�
 {plate_e_sm082_bm0_0, plate_e_sm082_bm0_0tlut,82, 34,0,0}},	// �X�e�[�W���b�n�l�k�u�����y �Ï�
 };

// ��Փx�i�Q�o�E�S�o�p�j
static PIC com_lv[3] = {
	p_hard_bm0_0,   p_hard_bm0_0tlut,
	p_normal_bm0_0, p_normal_bm0_0tlut,
	p_easy_bm0_0,   p_easy_bm0_0tlut,
};
// ���y
static PIC music[4][2] = {
	{{mus_fev_bm0_0,mus_fev_bm0_0tlut},
	{mus_fev2_bm0_0, mus_fev2_bm0_0tlut}},
	{{mus_chi_bm0_0,mus_chi_bm0_0tlut},
	{mus_chi2_bm0_0, mus_chi2_bm0_0tlut}},
	{{mus_diz_bm0_0,mus_diz_bm0_0tlut},
	{mus_diz2_bm0_0, mus_diz2_bm0_0tlut}},
	{{mus_off_bm0_0,mus_off_bm0_0tlut},
	{mus_off2_bm0_0, mus_off2_bm0_0tlut}},
};


// �X�e�[�W
static PIC battle_stage_pic[] = {
	menu_st_name_bm0_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm1_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm2_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm3_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm4_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm5_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm6_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm7_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm8_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm9_0, menu_st_name_bm0_0tlut,
	menu_st_name_bm10_0, menu_st_name_bm0_0tlut,
};

// �o�k or �b�n�l
static PIC2 plcom[4][2] = {
	pl_1p_bm0_0, pl_1p_bm0_0tlut, 16,8,0,0,
	com_1p_bm0_0,com_1p_bm0_0tlut,32,8,0,0,
	pl_2p_bm0_0, pl_2p_bm0_0tlut, 16,8,0,0,
	com_2p_bm0_0,com_2p_bm0_0tlut,32,8,0,0,
	pl_3p_bm0_0, pl_3p_bm0_0tlut, 16,8,0,0,
	com_3p_bm0_0,com_3p_bm0_0tlut,32,8,0,0,
	pl_4p_bm0_0, pl_4p_bm0_0tlut, 16,8,0,0,
	com_4p_bm0_0,com_4p_bm0_0tlut,32,8,0,0,
};
// ��t���[��
static PIC face_frame[4] = {
	frame_1p_bm0_0, frame_1p_bm0_0tlut,
	frame_2p_bm0_0, frame_2p_bm0_0tlut,
	frame_3p_bm0_0, frame_3p_bm0_0tlut,
	frame_4p_bm0_0, frame_4p_bm0_0tlut,
};
// �v���[�g�i�L�����N�^�[�Z���N�g�j
static PIC plate4p_char[4] = {
	plate4p_char_bm0_0,plate4p_char_bm0_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm1_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm2_0tlut,
	plate4p_char_bm0_0,plate4p_char_bm3_0tlut,
};
// �v���[�g�i���x���Z���N�g�j
static PIC plate4p_lv[4] = {
	plate4p_lv_bm0_0,plate4p_lv_bm0_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm1_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm2_0tlut,
	plate4p_lv_bm0_0,plate4p_lv_bm3_0tlut,
};
// �X�s�[�h�i�Q�o�p�j
static PIC cap_sp[2][3] = {
	cap_1_l_bm0_0,cap_1_l_bm0_0tlut,
	cap_1_m_bm0_0,cap_1_m_bm0_0tlut,
	cap_1_h_bm0_0,cap_1_h_bm0_0tlut,
	cap_2_l_bm0_0,cap_2_l_bm0_0tlut,
	cap_2_m_bm0_0,cap_2_m_bm0_0tlut,
	cap_2_h_bm0_0,cap_2_h_bm0_0tlut,
};
// �X�s�[�h�i�S�o�p�E�p�^�[���f�[�^�j
static u8 *speed4p_pat[3] = {
	speed4p_low_bm0_0,
	speed4p_med_bm0_0,
	speed4p_hi_bm0_0,
};
// �X�s�[�h�i�S�o�p�E�p���b�g�f�[�^�j
static u16 *speed4p_pal[4] = {
	speed4p_low_bm0_0tlut,
	speed4p_low_bm1_0tlut,
	speed4p_low_bm2_0tlut,
	speed4p_low_bm3_0tlut,
};

//	?P or ?COM
static PIC team_4p_player[4][2] = {
	{{dm_ts_1p_bm0_0,dm_ts_1p_bm0_0tlut},{dm_ts_1p_com_bm0_0,dm_ts_1p_bm0_0tlut}},
	{{dm_ts_2p_bm0_0,dm_ts_2p_bm0_0tlut},{dm_ts_2p_com_bm0_0,dm_ts_2p_bm0_0tlut}},
	{{dm_ts_3p_bm0_0,dm_ts_3p_bm0_0tlut},{dm_ts_3p_com_bm0_0,dm_ts_3p_bm0_0tlut}},
	{{dm_ts_4p_bm0_0,dm_ts_4p_bm0_0tlut},{dm_ts_4p_com_bm0_0,dm_ts_4p_bm0_0tlut}},
};

//	�`�[�������f�[�^
static	u8	team_data[4][4] = {
	{TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY2,TEAM_ENEMY3},
	{TEAM_MARIO,TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY},
	{TEAM_MARIO,TEAM_MARIO,TEAM_MARIO,TEAM_ENEMY},
	{TEAM_MARIO,TEAM_ENEMY,TEAM_ENEMY,TEAM_ENEMY}
};

//	��Փx�v���[�g( STORY�p )
static u8 *difficulty_plate[3] = {
	menu_st_level_easy_bm0_0,
	menu_st_level_normal_bm0_0,
	menu_st_level_hard_bm0_0,
};

// ��
static PIC faces[] = {
	st_face_01_bm0_0, st_face_01_bm0_0tlut,
	st_face_02_bm0_0, st_face_02_bm0_0tlut,
	st_face_03_bm0_0, st_face_03_bm0_0tlut,
	st_face_04_bm0_0, st_face_04_bm0_0tlut,
	st_face_05_bm0_0, st_face_05_bm0_0tlut,
	st_face_06_bm0_0, st_face_06_bm0_0tlut,
	st_face_07_bm0_0, st_face_07_bm0_0tlut,
	st_face_08_bm0_0, st_face_08_bm0_0tlut,
	st_face_09_bm0_0, st_face_09_bm0_0tlut,
	st_face_10_bm0_0, st_face_10_bm0_0tlut,
	st_face_11_bm0_0, st_face_11_bm0_0tlut,
	st_face_12_bm0_0, st_face_12_bm0_0tlut,
};

// �g�k�Ή��e�N�X�`���\��
static void disp_tex_4bt_sc( u8 *pat, u16 *pal, u16 sizex, u16 sizey, s16 x, s16 y, f32 scx, f32 scy, u8 flag )
{
	s32 pos_x,pos_y,w,h,dsdx,dtdy;

	pos_x	= (s32)(x + (sizex - (f32)sizex * scx) * .5);
	w		= (s32)((f32)sizex * scx);
	dsdx	= (s32)(1024.0 / scx);

	pos_y	= (s32)(y + (sizey - (f32)sizey * scy) * .5);
	h		= (s32)((f32)sizey * scy);
	dtdy	= (s32)(1024.0 / scy);

	gDPLoadTLUT_pal256( gp++,pal );
	gDPLoadTextureTile_4b( gp++, pat, G_IM_FMT_CI,sizex,sizey,0,0,sizex-1,sizey-1,
					0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle( gp++, (pos_x)<<2, (pos_y)<<2, (pos_x+w)<<2, (pos_y+h)<<2,
					G_TX_RENDERTILE, 0, 0, dsdx, dtdy );
}


static	void reset_face_size( Face_Data *fd )
{
	fd->count		= 0;		//	�ړ�(�㉺�̃X�N���[��)�p�J�E���^�̃N���A
	fd->move		= 0;		//	�ړ�(�㉺�̃X�N���[��)���t���O�̃N���A
	fd->sc[0]		= 0.25;		//	�Q��̃X�P�[���l�̐ݒ�(1/4�̑傫��)
	fd->sc[1]		= 0.5;		//	�P��̃X�P�[���l�̐ݒ�(1/2�̑傫��)
	fd->sc[2]		= 1.0;		//	�����̃X�P�[���l�̐ݒ�(���ʂ̑傫��)
	fd->sc[3]		= 0.5;		//	�P���̃X�P�[���l�̐ݒ�(1/2�̑傫��)
	fd->sc[4]		= 0.25;		//	�Q���̃X�P�[���l�̐ݒ�(1/4�̑傫��)
	fd->pos_y[0]	= -54;		//	�Q��̊�x���W�̐ݒ�
	fd->pos_y[1]	= -36;		//	�P��̊�x���W�̐ݒ�
	fd->pos_y[2]	= 0;		//	�����̊�x���W�̐ݒ�
	fd->pos_y[3]	= 36;		//	�P���̊�x���W�̐ݒ�
	fd->pos_y[4]	= 54;		//	�Q���̊�x���W�̐ݒ�
}

static	void set_face_size( Player_Data *pd,Face_Data *fd,u32 cnt )
{
	float sc_s[] = {0.125, 0.149, 0.163, 0.176, 0.191, 0.204, 0.218, 0.232, 0.246, 0.250};	//	�S�i�ڂ���R�i�ڂ܂ł̃X�P�[���l
	float sc_m[] = {0.250, 0.288, 0.316, 0.343, 0.371, 0.399, 0.427, 0.454, 0.482, 0.500};	//	�R�i�ڂ���Q�i�ڂ܂ł̃X�P�[���l
	float sc_b[] = {0.500, 0.566, 0.621, 0.677, 0.732, 0.788, 0.843, 0.899, 0.954, 1.000};	// 	�Q�i�ڂ���P�i�ڂ܂ł̃X�P�[���l

	//	��̃X�N���[������
	switch( pd->flag ){
	case 0:		// �҂�
		break;
	case 1:		// ��Ɉړ�( �X�N���[�������͉� )
		if( !(cnt % 1) ){					//	�Q�C���g( �P�t���[�� )�łP�X�N���[��
			fd->count ++;						//	�����J�E���^�̑���
			fd->pos_y[0] += 2;					//	�Q��̊�x���W�̑���
			fd->pos_y[1] += 4;					//	�P��̊�x���W�̑���
			fd->pos_y[2] += 4;					//	�����̊�x���W�̑���
			fd->pos_y[3] += 2;					//	�P���̊�x���W�̑���
			fd->pos_y[4] += 1;					//	�Q���̊�x���W�̑���
			fd->sc[0] = sc_m[fd->count];		//	�Q��̃X�P�[���l�̐ݒ�
			fd->sc[1] = sc_b[fd->count];		//	�P��̃X�P�[���l�̐ݒ�
			fd->sc[2] = sc_b[9-fd->count];		//	�����̃X�P�[���l�̐ݒ�
			fd->sc[3] = sc_m[9-fd->count];		//	�P���̃X�P�[���l�̐ݒ�
			fd->sc[4] = sc_s[9-fd->count];		//	�Q���̃X�P�[���l�̐ݒ�
			if( fd->count >= 9 ){				//	�����I������
				pd->flag = 0;					//	�X�N���[�������t���O�̃N���A( ��~ )
				reset_face_size( fd );			//	���W�E�X�P�[���̏�����
			}
		}
		break;
	case 2:		// ���Ɉړ�( �X�N���[�������͏� )
		if( !(cnt % 1) ){					//	�Q�C���g( �P�t���[�� )�łP�X�N���[��
			fd->count ++;						//	�����J�E���^�̑���
			fd->pos_y[0] -= 1;					//	�Q��̊�x���W�̑���
			fd->pos_y[1] -= 2;					//	�P��̊�x���W�̑���
			fd->pos_y[2] -= 4;					//	�����̊�x���W�̑���
			fd->pos_y[3] -= 4;					//	�P���̊�x���W�̑���
			fd->pos_y[4] -= 2;					//	�Q���̊�x���W�̑���
			fd->sc[0] = sc_s[9-fd->count];		//	�Q��̃X�P�[���l�̐ݒ�
			fd->sc[1] = sc_m[9-fd->count];		//	�Q��̃X�P�[���l�̐ݒ�
			fd->sc[2] = sc_b[9-fd->count];		//	�����̃X�P�[���l�̐ݒ�
			fd->sc[3] = sc_b[fd->count];		//	�P���̃X�P�[���l�̐ݒ�
			fd->sc[4] = sc_m[fd->count];		//	�Q���̃X�P�[���l�̐ݒ�
			if( fd->count >= 9 ){				//	�����I������
				pd->flag = 0;					//	�X�N���[�������t���O�̃N���A( ��~ )
				reset_face_size( fd );			//	���W�E�X�P�[���̏�����
			}
		}
		break;
	}
}

// STORY �p������
static void init_story_play( void )
{
	if( evs_one_game_flg ){
		if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){
			//	�X�e�[�W�Z���N�g��I�Ԃ��Ƃ��o�����ꍇ
			ms.flow			= CS_STORY_SERECT_STAGE;	//	�X�e�[�W�Z���N�g
			ms.adj_x		= -320;						//	��ʂw���W
		}else{
			ms.flow			= CS_STORY_SERECT_LEVEL;	//	��Փx�I��(�ʏ�)
			ms.adj_x		= 0;						//	��ʂw���W
		}
	}else{
		//	���[�h�Z���N�g���痈���ꍇ
		ms.flow			= CS_STORY_SERECT_LEVEL;	//	��Փx�I��(�ʏ�)
		ms.adj_x		= 0;						//	��ʂw���W
	}
	ms.mv_x			= 20;	//	�؂�ւ�(�X�e�[�W�Z���N�g <-> ��Փx�I��)�̃X�N���[�����x
	ms.sub_flow		= 0;	//	�������䏈�����L�[���͂ɐݒ�
	ms.cnt			= 0;	//	�����J�E���^���N���A

	//	LEVEL �݂̂̊���W
	ms.pd[0].bx		= 110;	//	��Փx�I��`��̊�ɂȂ���W( ms.adj_x + ms.pd[0].bx )
	ms.pd[0].by		= 47;	// 	�x���W�͌Œ�
	ms.pd[0].lv 	= evs_mem_data[evs_select_name_no[0]].state_old.p_st_lv;	//	�O��I��������Փx�̐ݒ�

	//	LEVEL & STAGE �̊���W
	ms.pd[1].bx		= 383;	//	�X�e�[�W�Z���N�g�`��̊�ɂȂ���W( ms.adj_x + ms.pd[1].bx )
	ms.pd[1].by		= 47;	// 	�x���W�͌Œ�
	ms.pd[1].flag	= 0;	//	�X�e�[�W�Z���N�g�̎��A��Փx�ݒ肩�X�e�[�W�ݒ�̂ǂ�����s���Ă��邩�̃t���O(0:��Փx�ݒ� 1:�X�e�[�W�ݒ� )
	ms.pd[1].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_st_st_lv;		//	�O��I��������Փx�̐ݒ�
	ms.pd[1].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_st_st - 1;	//	�O��I�������X�e�[�W�ԍ��̐ݒ�

	evs_one_game_flg = 0;	// �X�e�[�W�Z���N�g�t���O�̃N���A
}

// STORY �p���C��
static void calc_story_play( void )
{
	switch( ms.flow ){
	case	CS_STORY_SERECT_LEVEL:	//	���x���I���̂�
		switch( ms.sub_flow ){
			case	0:
				ms.cnt ++;									//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
				if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[0].lv < 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[0].lv ++;						//	�J�[�\����ړ�
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[0].lv > 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[0].lv --;						//	�J�[�\�����ړ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_R ){
					if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){
						//	�N���A���Ă����ꍇ�X�e�[�W�Z���N�g�֍s��
						ms.sub_flow = 253;					//	�X�N���[�������ɐݒ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );			//	SE�ݒ�
					ms.sub_flow = 254;						//	����
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	�L�����Z��( ���[�h�Z���N�g�ɖ߂� )
				}
				break;
			case 253:		// �t���[���A�E�g( �X�N���[������ )
				if( ms.adj_x - ms.mv_x > -320 ){
					ms.adj_x -= ms.mv_x;
				} else {
					ms.adj_x -= ms.mv_x;
					ms.cnt = 0;							//	�����J�E���^�̃N���A
					ms.sub_flow = 0;					//	�������䏈�����L�[���͂ɐݒ�
					ms.pd[1].flag = 0;					//	�X�e�[�W�Z���N�g�̓�Փx�ݒ菈���ɕύX�B
					ms.flow = CS_STORY_SERECT_STAGE;	//	�������X�e�[�W�Z���N�g�ɐ؂�ւ���
				}
				break;
			case 254:
				main_no = MAIN_50;						//	�X�g�[���[���[�h��
				main_60_out_flg = 0;					// ���C�����[�v�E�o
				break;
			case 255:
				main_no = MAIN_MODE_SELECT;				//	���[�h�Z���N�g�ɖ߂�
				main_60_out_flg = 0;					//	���C�����[�v�E�o
				break;
		}
		break;
	case	CS_STORY_SERECT_STAGE:	//	���x���I�� & �X�e�[�W�Z���N�g
		switch( ms.sub_flow ){
			case	0:	//	��Փx�ݒ�
				ms.cnt ++;									//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
				if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[1].lv < 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[1].lv ++;						//	�J�[�\����ړ�
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[1].lv > 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[1].lv --;						//	�J�[�\�����ړ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_RIGHT ){
					dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
					ms.pd[1].flag = 1;						//	�����������X�e�[�W�ݒ�ɐ؂�ւ���( �\���n )
					ms.sub_flow = 1;						//	�����������X�e�[�W�ݒ�ɐ؂�ւ���( �����n )
				}else	if( joyupd[main_joy[0]] & DM_KEY_L ){
					ms.sub_flow = 253;						//	�X�N���[�������ɐݒ�(�m�[�}�����[�h(�ʏ��Փx�ݒ�)�֍s��)
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );			//	SE�ݒ�
					ms.sub_flow = 254;						//	����
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	�L�����Z��( ���[�h�Z���N�g�ɖ߂� )
				}
				break;
			case	1:	//	�X�e�[�W�ݒ�
				ms.cnt ++;
				if( joycur[main_joy[0]] & DM_KEY_DOWN ){
					if( ms.pd[1].speed > 1 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[1].speed --;					//	�X�e�[�W�_�E����ړ�
					}
				}else	if( joycur[main_joy[0]] & DM_KEY_UP ){
					if( ms.pd[1].speed < 10 + evs_secret_flg ){	//	�s�[�`�P���g�p�\�ȏꍇ�́A�s�[�`�P�Ƃ��ΐ�ł���B
						dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						ms.pd[1].speed ++;					//	�X�e�[�W�A�b�v���ړ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_LEFT ){
					dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
					ms.pd[1].flag = 0;						//	�����������Փx�ݒ�ɐ؂�ւ���( �\���n )
					ms.sub_flow = 0;						//	�����������Փx�ݒ�ɐ؂�ւ���( �����n )
				}else	if( joyupd[main_joy[0]] & DM_KEY_L ){
					ms.sub_flow = 253;						//	�X�N���[�������ɐݒ�(�m�[�}�����[�h(�ʏ��Փx�ݒ�)�֍s��)
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
					dm_set_menu_se( SE_mDecide );
					ms.sub_flow = 254;	//	����
				}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
					ms.sub_flow = 255;						//	�L�����Z��( ���[�h�Z���N�g�ɖ߂� )
				}
				break;
			case 253:		// �t���[���C��
				if( ms.adj_x + ms.mv_x < 0 ){
					ms.adj_x += ms.mv_x;
				} else {
					ms.adj_x += ms.mv_x;
					ms.cnt = 0;							//	�����J�E���^�̃N���A
					ms.sub_flow = 0;					//	�������䏈�����L�[���͂ɐݒ�
					ms.flow = CS_STORY_SERECT_LEVEL;	//	�������Փx�ݒ�(�ʏ�)�ɐ؂�ւ���
				}
				break;
			case 254:		//	����
				main_no = MAIN_12;						// �Q�[���O�ݒ��
				main_60_out_flg = 0;					// ���C�����[�v�E�o
				break;
			case 255:
				main_no = MAIN_MODE_SELECT;				//	���[�h�Z���N�g�ɖ߂�
				main_60_out_flg = 0;					// ���C�����[�v�E�o
				break;
		}
		break;
	}
}

// STORY �p�`��
static void disp_story_play( void )
{
	s16	i,j;

	//	�w�i
	gSPDisplayList( gp++, Texture_TE_dl );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	�c��
	{
		for(j = 0;j < 16;j++)	//	����
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// �^�C�g��
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	���[�h�Z���N�g
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	�{�^��(�`�F���� �a�F�L�����Z��)

	//	��Փx�݂̂̕`��
	load_TexPal( menu_st_stage_dodai_bm0_0tlut );	//	�p���b�g�f�[�^�̓Ǎ���(  �قƂ�ǋ��� )
	for(i = 0;i < 4;i++ ){
		load_TexTile_4b( &menu_st_level_dodai_bm0_0[i * 1600],100,32,0,0,99,31 );
		draw_Tex(ms.adj_x + ms.pd[0].bx,ms.pd[0].by + (i << 5),100,32,0,0 );
	}
	load_TexTile_4b( &menu_st_level_dodai_bm0_0[1600 << 2],100,2,0,0,99,1 );
	draw_Tex(ms.adj_x + ms.pd[0].bx,ms.pd[0].by + (32 << 2),100,2,0,0 );

	//	�J�[�\��( EASY.NORMAL.HARD )
	load_TexBlock_4b( difficulty_plate[ms.pd[0].lv],80,16 );	//	ms.pd[0].lv ���J�[�\���ʒu �� ��Փx�̒l������(0:EASY 1:NORMAL 2:HARD )
	draw_Tex(ms.adj_x + ms.pd[0].bx + 9,ms.pd[0].by + 101 - ms.pd[0].lv * 33,80,16,0,0 );


	//	��Փx�ƃX�e�[�W�ݒ�̕`��
	for(i = 0;i < 8;i++ ){
		load_TexTile_4b( &menu_st_stage_dodai_bm0_0[i * 1552],194,16,0,0,193,15 );
		draw_Tex(ms.adj_x + ms.pd[1].bx,ms.pd[1].by + (i << 4),194,16,0,0 );
	}
	load_TexTile_4b( &menu_st_stage_dodai_bm0_0[1552 << 3],194,2,0,0,193,1 );
	draw_Tex(ms.adj_x + ms.pd[1].bx,ms.pd[1].by + (16 << 3),194,2,0,0 );

	//	�J�[�\��( EASY.NORMAL.HARD )
	load_TexBlock_4b( difficulty_plate[ms.pd[1].lv],80,16 );	//	ms.pd[1].lv ���J�[�\���ʒu �� ��Փx�̒l������(0:EASY 1:NORMAL 2:HARD )
	draw_Tex(ms.adj_x + ms.pd[1].bx + 9,ms.pd[1].by + 101 - ms.pd[1].lv * 33,80,16,0,0 );



	if( evs_mem_data[evs_select_name_no[0]].mem_use_flg & DM_MEM_STORY_CLEAR ){	//	�����A�N���A���Ă����ꍇ�A���̕`����s��
		//	RL �L�[
		load_TexTile_4b(menu_st_rl_key_bm0_0,40,13,0,0,39,12 );
		draw_Tex(ms.adj_x + ms.pd[0].bx - 5,ms.pd[0].by + 137 ,20,13,0,0 );		//	�q�{�^��
		draw_Tex(ms.adj_x + ms.pd[1].bx + 84,ms.pd[1].by + 137,20,13,20,0 );	//	�k�{�^��

		//	�؂�ւ����
		j = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 8 ) + 84;
		load_TexTile_4b( menu_st_arrow_b_bm0_0 ,92,8,0,0,91,7 );
		draw_Tex( ms.adj_x + ms.pd[0].bx + 16,ms.pd[0].by + 142,8,8,0,0 );				//	LEVEL�� ���[
		draw_ScaleTex( ms.adj_x + ms.pd[0].bx + 24,ms.pd[0].by + 142,80,8,j,8,2,0 );	//	LEVEL�� ����

		draw_TexFlip( ms.adj_x + ms.pd[1].bx + 75,ms.pd[1].by + 142,8,8,0,0,flip_on,flip_off );	//	STAGE�� �E�[
		draw_ScaleTex( ms.adj_x + ms.pd[1].bx + 75 - j,ms.pd[1].by + 142,80,8,j,8,2,0 );		//	STAGE�� ����

		load_TexTile_4b( menu_st_arrow_a_bm0_0 ,24,16,0,0,23,15 );	//	��󕔕�
		draw_Tex( ms.adj_x + ms.pd[0].bx + 24 + j,ms.pd[0].by + 134,24,16,0,0 );						//	LEVEL�� ���[
		draw_TexFlip( ms.adj_x + ms.pd[1].bx + 51 - j,ms.pd[1].by + 134,24,16,0,0,flip_on,flip_off );	//	STAGE�� �E�[

		//	����
		load_TexTile_4b( menu_st_stage_str_bm0_0 ,84,10,0,0,83,9 );	//	STAGE SELECT
		draw_Tex( ms.adj_x + ms.pd[0].bx + 20 ,ms.pd[0].by + 137,84,10,0,0 );

		load_TexBlock_4b( menu_st_level_str_bm0_0 ,80,10 );	//	NORMAL GAME
		draw_Tex( ms.adj_x + ms.pd[1].bx - 4 ,ms.pd[1].by + 137,80,10,0,0 );


	}

	//	��
	load_TexPal( faces[ms.pd[1].speed].pal );
	load_TexBlock_4b( faces[ms.pd[1].speed].pat ,48,48);
	draw_Tex(ms.adj_x + ms.pd[1].bx + 130,ms.pd[1].by + 69,48,48,0,0 );

	//	�X�e�[�W�ԍ�
	load_TexPal( lv_num_bm0_0tlut );
	load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );
	if( ms.pd[1].speed > 9 ){
		draw_Tex(ms.adj_x + ms.pd[1].bx + 157,ms.pd[1].by + 39,8,12,0,(ms.pd[1].speed / 10) * 12 );
		draw_Tex(ms.adj_x + ms.pd[1].bx + 165,ms.pd[1].by + 39,8,12,0,(ms.pd[1].speed % 10) * 12 );
	}else{
		draw_Tex(ms.adj_x + ms.pd[1].bx + 161,ms.pd[1].by + 39,8,12,0,ms.pd[1].speed * 12 );
	}


	//	�������炵��������
	gDPSetRenderMode( gp++,G_RM_XLU_SURF, G_RM_XLU_SURF2 );
	gDPSetPrimColor( gp++,0,0,255,255,255,127);

	//	����( ms.pd[1].flag �� 0 �̏ꍇ	�X�e�[�W�ݒ葤���Â��Ȃ�A�P�̏ꍇ�A��Փx�ݒ葤���Â��Ȃ�B
	load_TexPal( menu_st_stage_dodai_bm0_0tlut );
	for(i = 0;i < 2;i++ ){
		load_TexBlock_4b( &menu_st_shadow_bm0_0[i * 2016],96,42 );
		draw_Tex(ms.adj_x + ms.pd[1].bx + 98 - 	ms.pd[1].flag * 95,ms.pd[1].by + 14 + i * 42,96,42,0,0 );
	}
	load_TexBlock_4b( &menu_st_shadow_bm0_0[2016 << 1],96,28 );
	draw_Tex(ms.adj_x + ms.pd[1].bx + 98 - ms.pd[1].flag * 95,ms.pd[1].by + 14 + (42 << 1),96,28,0,0 );

	j = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	//	�����
	gDPSetPrimColor(gp++,0,0,j,j,j,0xff);
	load_TexPal( arrow_r_bm0_0tlut );
	load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );
	//	��Փx�݂̂̏ꍇ
	draw_Tex( ms.adj_x + ms.pd[0].bx + 2 + ((ms.cnt / 15) % 2),ms.pd[0].by + 104 - ms.pd[0].lv * 33,8,11,0,0 );

	//	�X�e�[�W�Z���N�g���܂ޏꍇ
	switch( ms.pd[1].flag )
	{
	case	0:
		draw_Tex( ms.adj_x + ms.pd[1].bx + 2 + ((ms.cnt / 15) % 2),ms.pd[1].by + 104 - ms.pd[1].lv * 33,8,11,0,0 );
		break;
	case	1:
		//	�㉺���
		load_TexPal( arrow_u_bm0_0tlut );
		load_TexBlock_4b( arrow_u_bm0_0,16,7 );
		//	����
		if( ms.pd[1].speed < 10 + evs_secret_flg ){
			draw_Tex( ms.adj_x + ms.pd[1].bx + 159,ms.pd[1].by + 30 - ((ms.cnt / 15) % 2),16,7,0,0 );
		}
		//	�����
		if( ms.pd[1].speed > 1 ){
			draw_TexFlip(ms.adj_x + ms.pd[1].bx + 159,ms.pd[1].by + 53 + ((ms.cnt / 15) % 2),16,7,0,0,flip_off,flip_on );
		}
		break;
	}
}


// �P�o(���x���Z���N�g)�p������
static void init_1p_play( void )
{
	ms.flow			= 0;		//	�����̏��������x���E�X�s�[�h�ݒ�ɂ���
	ms.cnt			= 0;		//	�����J�E���^�̃N���A
	ms.ui.mus_no	= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_m;	//	�O��ݒ肵�����y�ԍ��̃Z�b�g
	ms.ui.mus_adj_y	= 0;		//	���y�\���x���W�̏�����
	ms.ui.bx		= 96;		//	���y�v���[�g�̂w���W�̐ݒ�
	ms.ui.by		= 181;		//	���y�v���[�g�̂x���W�̐ݒ�

	if( evs_mem_data[evs_select_name_no[0]].level_data[0].c_level >= 20 ){	//	�N���A���x���� 21 �ȏゾ�����ꍇ
		ms.max_lv	= 21;		//	�I���\�ō����x���� 21 �ɂ���
	}else{						//	�N���A���x���� 20 �ȉ��̏ꍇ
		ms.max_lv	= 20;		//	�I���\�ō����x���� 20 �ɂ���
	}

	ms.pd[0].flag	= 0;		//	������������x���ݒ肵�Ă���B
	ms.pd[0].ok		= 0;		//	����t���O�̃N���A
	ms.pd[0].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_lv;	//	�O��ݒ肵�����x���ɐݒ肵�Ă���
	ms.pd[0].lv		= ( ms.pd[0].lv > ms.max_lv )?ms.max_lv:ms.pd[0].lv;		//	�ݒ肳�ꂽ���x�����I���\�ō����x���𒴂����ꍇ�A�I���\�ō����x���ɂ��Ă���
	ms.pd[0].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_1p_sp;	//	�O��ݒ肵�����x�ɐݒ肵�Ă���
	ms.pd[0].bx		= 96;		//	���x���I���v���[�g�̂w���W�̐ݒ�
	ms.pd[0].by		= 47;		//	���x���I���v���[�g�̂x���W�̐ݒ�
}

// �P�o�p���C��
static void calc_1p_play( void )
{
	int i;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;

	switch( ms.flow ){
		case 0:		// ���x���E�X�s�[�h�ݒ�
			pd = &ms.pd[0];
			if( !pd->ok ){
				if( joycur[main_joy[0]] & CONT_UP ){
					if( !pd->flag ){							//	���x���ݒ葤�̎�
						if( pd -> lv < ms.max_lv ){				//	�I���\�ō����x����艺�̏ꍇ
							pd -> lv ++;						//	���x���㏸
							dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						}
					} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
						if( pd -> speed < 2 ){					//	�ō����x��艺�������ꍇ
							pd -> speed ++;						//	����
							dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						}
					}
				}else	if( joycur[main_joy[0]] & CONT_DOWN ){
					if( !pd->flag ){							//	���x���ݒ葤�̎�
						if( pd -> lv > 0 ){						//	�Œ჌�x�����ゾ�����ꍇ
							pd -> lv --;						//	���x�����~
							dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						}
					} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
						if( pd -> speed > 0 ){					//	�Œᑬ�x���ゾ�����ꍇ
							pd -> speed --;						//	����
							dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
						}
					}
				}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
					if( pd -> flag < 1 ){						//	���x���ݒ葤�̎�
						pd -> flag ++;							//	�J�v�Z���X�s�[�h�ݒ葤�ɕύX
						dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
					}
				}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
					if( pd -> flag > 0 ){						//	�J�v�Z���X�s�[�h�ݒ葤�̎�
						pd -> flag --;							//	���x���ݒ葤�ɕύX
						dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	����
					pd->ok = 1;									//	����t���O�𗧂Ă�
					ms.flow = 1;								//	�������������y�ݒ�ɕύX
					dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
					break;
				}else	if( joyupd[main_joy[0]] & CONT_B ){		//	�L�����Z��
					main_no = MAIN_MODE_SELECT;					//	���[�h�Z���N�g�ɖ߂�
					main_60_out_flg = 0;						//	���C�����[�v�E�o
				}
			} else {
				// �L�����Z��
				if( joyupd[main_joy[0]] & CONT_B ){
					pd->ok = 0;						//	�������
				}
			}
			break;
		case 1:		// ���y�ݒ�
			pd = &ms.pd[0];
			if( ui->mus_adj_y == 0 ){	//	���y�O���t�B�b�N���X�N���[�����łȂ��ꍇ
				if( joynew[main_joy[0]] & CONT_UP ){
					if( ui->mus_no > 0 ){						//	���y�ԍ����O����̏ꍇ
						ui->mus_no --;							//	���y�ԍ��̌���
						ui->mus_adj_y = -16;					//	�X�N���[���l�̐ݒ�
						dm_set_menu_se( SE_mLeftRight );		//	SE�ݒ�
					}
				}else	if( joynew[main_joy[0]] & CONT_DOWN ){
					if( ui->mus_no < 3 ){						//	���y�ԍ����R��艺�̏ꍇ
						ui->mus_no ++;							//	���y�ԍ��̑���
						ui->mus_adj_y = 16;						//	�X�N���[���l�̐ݒ�
						dm_set_menu_se( SE_mLeftRight );		//	SE�ݒ�
					}
				}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	����
					main_no = MAIN_12;							//	�Q�[���O�ݒ��
					main_60_out_flg = 0;						//	���C�����[�v�E�o
					ms.flow = 2;								//	�������������Ȃ�(���[�v�E�o�̂���)
					dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
					break;
				}
			}
			// �L�����Z��
			if( joyupd[main_joy[0]] & CONT_B ){
				pd->ok = 0;										//	����t���O���N���A����
				ms.flow = 0;									//	�������������x���E�X�s�[�h�ݒ�ɕύX
			}
			break;
		case 2:		// ��������
			break;
	}
	ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	���y�I���O���t�B�b�N�̃X�N���[������
	ms.cnt ++;		//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
}

// �P�o�p�`��
static void disp_1p_play( void )
{
	int i,j,k,l,mod,adj,col,st,en;
	char work1[10];
	Player_Data *pd;
	UNDER_ITEMS *ui;

	pd = &ms.pd[0];
	ui = &ms.ui;

	//	�w�i
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	�c��
	{
		for(j = 0;j < 16;j++)	//	����
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// ���n�֌W

	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );				//	���[�h�Z���N�g
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );				//	�{�^��(�`�F���� �a�F�L�����Z��)
	disp_tex_4bt( plate_1p_bm0_0, plate_1p_bm0_0tlut, 128,130,ms.pd[0].bx, ms.pd[0].by,SET_PTD );	//	���x���E�X�s�[�h�ݒ�v���[�g
	disp_tex_4bt( plate_env1_bm0_0,plate_env1_bm0_0tlut,128, 42, ui->bx,ui->by,SET_PTD );			//	���y�ݒ�v���[�g

	// �J�v�Z��
	disp_tex_4bt( cap_sp[0][pd->speed].pat,cap_sp[0][pd->speed].pal,56, 24, pd->bx+65,pd->by+32+(2-pd->speed)*33, SET_PTD );	//	���x�̃J�[�\��

	// �E�C���X
	for( k = 0; k < 3; k++ ){			//	�e�F���̃��[�v
		mod = SET_PTD;					//	�e�F���ڂ̕`��̎��́A�p���b�g�ƃO���t�B�b�N�����[�h����悤�ɐݒ肵�Ă���
		for( l = 0; l < 8; l++ ){		//	�����[�v
			for( j = 0; j < 13; j++ ){	//	�c���[�v
				col = (cap_pos[pd->lv][j] >> ( 14-l*2 )) & 0x3;	//	���W�f�[�^( l,j )�̔z�u�F�𒲂ׂ�( 0:����1:�� 2:�� 3:���F )
				if( (col > 0) && (col-1 == k ) ){				// 	�z�u������ŁA�z�u�F - 1 �� K �Ɠ����ꍇ�`�悷��
					disp_tex_4bt( &virus_bm0_0[k*20],virus_bm0_0tlut,8,5,pd->bx+17+l*5,pd->by+55+j*5,mod );
					mod = SET_D;		//	�e�F���ڈȍ~�̕`��̎��́A�p���b�g���O���t�B�b�N�����[�h���Ȃ��悤�ɐݒ肵�Ă���
				}
			}
		}
	}
	// ���x�����l
	adj = ( pd->lv < 10 )?4:0;
	itoa( pd->lv,work1,10 );
	for( j = 0; work1[j] != '\0'; j++ ){
		disp_tex_4bt( &lv_num_bm0_0[(work1[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+37+j*8+adj,pd->by+42,SET_PTD );
	}
	// ���x���Q�[�W
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+39+(20-pd->lv)*4,319,239);			//	�E�C���X���x���Q�[�W�̏㉺�́A�V�U�����O(�`��͈͎w��)�ōs���Ă���
	disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+6,pd->by+38,SET_PTD );
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);								//	�V�U�����O(�`��͈͎w��)�����ɖ߂�

	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;	//	���_�ŗp���l�̌v�Z

	// ���y�p�l��
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+18+16);	//	�X�N���[�������p�ɃV�U�����O(�`��͈͎w��)��ݒ肵�Ă���
	if( ms.flow == 1 || ui->mus_adj_y != 0 ){		//	�������������y�ݒ肩�X�N���[�����̏ꍇ( ui->mus_adj_y != 0 )
		for( i = 0; i < 4; i ++ ){					//	���y�O���t�B�b�N�𖈉�S�`�悷��( �V�U�����O�Ŕ͈͊O�͏����� )
				//	15�C���g�ŃA�j���[�V��������悤�ɂȂ��Ă���((ms.cnt/30)%2)
			disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+42,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
		}
	} else {	//	�������������x���E�X�s�[�h�ݒ�̏ꍇ�A�I�����Ă��鉹�y�ԍ�( ui->mus_no )�̃O���t�B�b�N������`�悷��(�A�j���[�V�������Ȃ�)
		disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+42,ui->by+18,SET_PTD );
	}

	gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+13,319,ui->by+13+16);	//	�V�U�����O(�`��͈͎w��)�����ɖ߂�
	gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
	switch( ms.flow ){
		case 0:
			// ���y�Ãp�l��
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42, ui->bx,ui->by,SET_PTD );	//	( ������������������ )

			// ��i�Ãp�l��
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			if( pd->ok ){	// ���܂�Ӗ�������
				disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			} else {
				//	pd->flag �� �O �̎��̓X�s�[�h�ݒ葤���A�P �̎��̓��x���ݒ葤���Â��Ȃ�( ������������������ )
				disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60, pd->by+14,SET_PTD );
			}
			// ��i�p���
			if( !pd->ok ){
				gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
				if( pd->flag ){
					disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2,pd->by+38+(2-pd->speed)*33,SET_PTD );
				} else {
					disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2,pd->by+34+(20-pd->lv)*4,SET_PTD );
				}
			}
			break;
		case 1:
			// ���i�p���
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
			if( ui->mus_no > 0 ){
				disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+62,ui->by+11-(ms.cnt/15)%2,SET_PTD );
			}
			if( ui->mus_no < 3 ){
				disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+62,ui->by+34+(ms.cnt/15)%2,SET_PTD );
			}
			// ��i�Ãp�l��
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			break;
		case 2:
			// ���y�Ãp�l��
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42, ui->bx,ui->by,SET_PTD );
			// ��i�Ãp�l��
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
			break;
	}

	// �n�j
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
	if( pd->ok ){
		disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95,pd->by+4,SET_PTD );
	}
}

// �Q�o�p������
static void init_2p_play( u8 flag, u8 mode )
{
	int i,j;

	if( flag ){

		if( main_old == MAIN_GAME){		//	�Q�[������ݒ�ɗ����ꍇ
			ms.flow		= 1;			//	���x���E�X�s�[�h�E���y�ݒ�
		}else{							//	�Q�[���ȊO(���[�h�Z���N�g�A���O����)����ݒ�ɗ����ꍇ
			ms.flow		= 0;			//	�L�����I��
		}

		ms.sub_flow		= 0;			//	�������䏈����l�ԃL�[���͂ɐݒ�
		ms.adj_x		= 0;			//	��ʂw���W
		ms.mv_x			= 20;			//	�؂�ւ�(�L�����I�� <-> ���x���E�X�s�[�h�E���y�ݒ�)�̃X�N���[�����x�̐ݒ�
		ms.max_chara	= 11 + evs_secret_flg;	//	�ő�I���\�L�������̐ݒ�( �ʏ� 11 ���� �s�[�`���g�p�\�̏ꍇ(evs_secret_flg == 1) 12 )

		ms.ui.com_lv = 1;				//	CPU �̃��x���� NORMAL �ɐݒ肵�Ă���

		if( evs_gamesel == GSL_2PLAY ){	//	MAN VS MAN �̏ꍇ
			for( i = 0;i < 2;i++ ){
				ms.pd[i].chara	= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_no;	//	�O��g�����L�����N�^�[�ԍ��̐ݒ�
				ms.pd[i].lv		= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_lv;	//	�O��̃E�C���X���x���̐ݒ�
				ms.pd[i].speed	= evs_mem_data[evs_select_name_no[i]].state_old.p_vm_sp;	//	�O��̃E�C���X�X�s�[�h�̐ݒ�
			}
			ms.ui.stg_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vm_st;			//	�O��̔w�i�ԍ��̐ݒ�
			ms.ui.mus_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vm_m;			//	�O��̉��y�̐ݒ�
		}else	if( evs_gamesel == GSL_VSCPU ){	//	MAN VS CPU�̏ꍇ
			for( i = 0;i < 2;i++ ){
				ms.pd[i].chara	= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_no[i];	//	�O��g�����L�����N�^�[�ԍ��̐ݒ�
				ms.pd[i].lv		= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_lv[i];	//	�O��̃E�C���X���x���̐ݒ�
				ms.pd[i].speed	= evs_mem_data[evs_select_name_no[0]].state_old.p_vc_sp[i];	//	�O��̃E�C���X�X�s�[�h�̐ݒ�
			}
			ms.ui.com_lv = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_cp_lv;		//	�O��� CPU��Փx�̐ݒ�
			ms.ui.stg_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_st;			//	�O��̔w�i�ԍ��̐ݒ�
			ms.ui.mus_no = evs_mem_data[evs_select_name_no[0]].state_old.p_vc_m;			//	�O��̉��y�̐ݒ�
		}

		// �l�� or �b�n�l
		ms.pd[0].play	= 0;		//	�l���b�n�l�̔��ʃt���O�̐ݒ�
		ms.pd[1].play	= mode;		//	0:MAN 1:CPU

		// ���i���ݒ�( ���y�E�w�i�̐ݒ蕔�� )
		ms.ui.bx	= ( ms.pd[1].play )?30:33;	//	�w���W 2PLAY �� VSCPU �ō��W��ς��Ă���(�\�������قȂ邽��)
		ms.ui.by	= 181;						//	�x���W�̐ݒ�
		ms.ui.pos	= 0;						//	���i�p�l���J�[�\�������ʒu�����[�ɐݒ肷��
		ms.ui.com_adj_y = 0;					//	�b�o�t��Փx�I��\���̃X�N���[���ړ��l�̃N���A
		ms.ui.stg_adj_y = 0;					//	�w�i�I��\���̃X�N���[���ړ��l�̃N���A
		ms.ui.mus_adj_y = 0;					//	���y�I��\���̃X�N���[���ړ��l�̃N���A

		// �I���\�X�e�[�W�`�F�b�N
		for( i = 0, ms.ui.stg_sel_num = 0; i < 11; i ++ ){
			if( evs_clear_stage_flg[i] ){
				ms.ui.stg_tbl[ms.ui.stg_sel_num++] = i;
			}
		}

		// ��֌W
		for( i = 0;i < 2;i++ ){
			reset_face_size( &ms.pd[i].fd );	// ���W������
		}
	}
	// ��{�ʒu
	ms.pd[0].by = ms.pd[1].by = 47;		//	��{�x���W�͋��ʂŌŒ�
	ms.pd[1].bx = 161;					//	�Q�o���̊�{�w���W�̐ݒ�
	if( ms.flow == 0 ){
		ms.pd[0].bx = 97;				//	�L�����I���̏ꍇ�̂P�o���̊�{�w���W�̐ݒ�
	} else {
		ms.pd[0].bx = 33;				//	���x���E�X�s�[�h�ݒ�̏ꍇ�̂P�o���̊�{�w���W�̐ݒ�
	}
	// �J�E���^
	ms.cnt		= 0;					//	�����J�E���^�̃N���A

	for( i = 0; i < 2; i ++ ){
		ms.pd[i].flag			= 0;	//	���x���E�X�s�[�h�ݒ�̓����I�������x���ݒ�ɂ���
		ms.pd[i].ok				= 0;	//	����t���O�̃N���A
	}
}
// �Q�o�p���C��
static void calc_2p_play( void )
{
	int i,j,fl;
	float sc_s[] = {0.125, 0.149, 0.163, 0.176, 0.191, 0.204, 0.218, 0.232, 0.246, 0.250};	//	�S�i�ڂ���R�i�ڂ܂ł̃X�P�[���l
	float sc_m[] = {0.250, 0.288, 0.316, 0.343, 0.371, 0.399, 0.427, 0.454, 0.482, 0.500};	//	�R�i�ڂ���Q�i�ڂ܂ł̃X�P�[���l
	float sc_b[] = {0.500, 0.566, 0.621, 0.677, 0.732, 0.788, 0.843, 0.899, 0.954, 1.000};	// 	�Q�i�ڂ���P�i�ڂ܂ł̃X�P�[���l

	Face_Data   *fd;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	switch( ms.flow ){
		case 0:		// �L�����I��
			switch( ms.sub_flow ){
				case 0:			// �l�Ԑݒ�
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// �l�Ԃ̂ݑ����
							if( !pd->ok ){			// �I��
								if( !fd->move ){		//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
									if( joynew[main_joy[i]] & CONT_UP ){
										dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
										pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	�L�����ԍ����O�ȉ��̏ꍇ�A�ő�I���L�����̔ԍ��ɐݒ�
										fd->move = -1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
										pd->flag = 1;														//	�X�N���[�������t���O��������X�N���[���ɐݒ�
									}else	if( joynew[main_joy[i]] & CONT_DOWN  ){
										dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
										pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	�L�����ԍ����ő�I���L�����̔ԍ��ȏ�ɂȂ����ꍇ�A�O�ɐݒ�
										fd->move = 1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
										pd->flag = 2;														//	�X�N���[�������t���O���������X�N���[���ɐݒ�
									}else	if( joyupd[main_joy[i]] & DM_KEY_OK  ){				// ����
										for( j = 0,fl = 1; j < 2; j++ ){
											//	���肪�����ԂŁA����̑I�������L�����Ǝ����̑I�������L�����������������ꍇ�A�����t���O���N���A����( fl = 0 )
											if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
										}
										if( fl ){												//	�����̃L���������肵���ꍇ( ����ƈႤ�L�������A���肪�܂������ԂłȂ������ꍇ )
											dm_set_menu_se( SE_mDecide );						//	SE�ݒ�
											pd->ok = 1;											//	����t���O�𗧂Ă�

											for( j = 0,fl = 1; j < 2; j++ ){					//	�����Ԃ𒲂ׂ�
												//	�v���C���[���l�Ԃł܂����肵�Ă��Ȃ������ꍇ�A�����t���O���N���A����( fl = 0 )
												if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
											}
											if( fl ){											//	���������ɐi�߂�
												//	�Q�o���b�o�t�̏ꍇ�A�b�o�t�̃L�����N�^�[�I����( ms.sub_flow = 1 ),�Q�o���l�̏ꍇ�A��ʂ̃X�N���[��������( ms.sub_flow = 254 )
												ms.sub_flow = ( ms.pd[1].play == 0 )?254:1;
												break;
											}
										}
									}else	if( joyupd[main_joy[i]] & CONT_B ){	//	�L�����Z��
										ms.sub_flow = 255;		//	���C�����[�v�E�o�����ɐݒ�
									}
								}
								//	��̃X�N���[������
								set_face_size(pd,fd,ms.cnt);
							} else {
								// �L�����Z��
								if( joyupd[main_joy[i]] & CONT_B && !fd->move ){	//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
									pd->ok = 0;										//	����̉���
								}
							}
						}
					}
					break;
				case 1:			// �b�n�l�ݒ�
					pd = &ms.pd[1];
					fd = &ms.pd[1].fd;

					if( !fd->move ){	//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
						if( joynew[main_joy[0]] & CONT_UP  ){
							dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
							pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	�L�����ԍ����O�ȉ��̏ꍇ�A�ő�I���L�����̔ԍ��ɐݒ�
							fd->move = -1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
							pd->flag = 1;														//	�X�N���[�������t���O��������X�N���[���ɐݒ�
						}else	if( joynew[main_joy[0]] & CONT_DOWN ){
							dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
							pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	�L�����ԍ����ő�I���L�����̔ԍ��ȏ�ɂȂ����ꍇ�A�O�ɐݒ�
							fd->move = 1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
							pd->flag = 2;														//	�X�N���[�������t���O���������X�N���[���ɐݒ�
						}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){							//	����
							if( pd->chara != ms.pd[0].chara ){									//	�P�o���I�������L�����ƈႤ�ꍇ
								dm_set_menu_se( SE_mDecide );									//	SE�ݒ�
								pd->ok = 1;														//	����t���O�̐ݒ�
								ms.sub_flow = 254;												//	��ʂ̃X�N���[��������
							}
						}else	if( joyupd[main_joy[0]] & CONT_B ){								// �L�����Z��
							ms.pd[0].ok = 0;													//	�P�o�̌���t���O�m�N���A
							ms.sub_flow = 0;													//	�P�o(�l�ԁj�̃L�����I���ɖ߂�
						}
					}
					set_face_size(pd,fd,ms.cnt);												//	��̃X�N���[������
					break;
				case 253:								//	�t���[���C��( ��̃v���[�g����ʓ��ɓ����Ă��鏈�� )
					if( ms.adj_x - ms.mv_x > 0 ){		//	�w����W�܂ňړ����Ă��Ȃ��ꍇ
						ms.adj_x -= ms.mv_x;
					} else {							//	�w����W�܂ňړ������ꍇ
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	�������L�����I���ɕύX
					}
					break;
				case 254:								//	�t���[���A�E�g( ��̃v���[�g����ʊO�ɏo�čs������ )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = 1;					//	���C���̏��������x���E�X�s�[�h�ݒ��
						ms.sub_flow = 253;				//	�t���[���C��( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʓ��ɓ����Ă��� )�����ɕύX
						init_2p_play(0,0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
				case 255:								//	�v���C���[�l���I����
					main_no = MAIN_MODE_SELECT;			//	���[�h�Z���N�g�ɖ߂�
					main_60_out_flg = 0;				//	���C�����[�v�E�o
					break;
			}
			ms.cnt ++;									//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
			break;
		case 1:		// ���x���I��
			ui = &ms.ui;

			switch( ms.sub_flow ){
				case 0:		// �l�Ԑݒ�
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){		// �l�Ԃ̂ݑ����
							if( !pd->ok ){			// �I��
								if( joycur[main_joy[i]] & CONT_UP ){
									if( !pd->flag ){							//	���x���ݒ葤�̎�
										if( pd -> lv < 20 ){					//	�I���\�ō����x��( 20 )��艺�̏ꍇ
											pd -> lv ++;						//	���x���㏸
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										if( pd -> speed < 2 ){					//	�ō����x��艺�������ꍇ
											pd -> speed ++;						//	����
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}else	if( joycur[main_joy[i]] & CONT_DOWN	){
									if( !pd->flag ){							//	���x���ݒ葤�̎�
										if( pd -> lv > 0 ){						//	�Œ჌�x�����ゾ�����ꍇ
											pd -> lv --;						//	���x�����~
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										if( pd -> speed > 0 ){					//	�Œᑬ�x���ゾ�����ꍇ
											pd -> speed --;						//	����
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}else	if( joyupd[main_joy[i]] & CONT_RIGHT ){
									if( pd->flag < 1 ){							//	���x���ݒ葤�̎�
										pd->flag ++;							//	�J�v�Z���X�s�[�h�ݒ葤�ɕύX
										dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
									}
								}else	if( joyupd[main_joy[i]] & CONT_LEFT ){
									if( pd->flag > 0 ){							//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										pd->flag --;							//	���x���ݒ葤�ɕύX
										dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
									}
								}else	if( joyupd[main_joy[i]] & DM_KEY_OK ){	//	����
									pd->ok = 1;									//	����t���O�𗧂Ă�
									dm_set_menu_se( SE_mDecide );				//	SE�ݒ�

									for( j = 0,fl = 1; j < 2; j++ ){
										//	�v���C���[���l�Ԃł܂����肵�Ă��Ȃ������ꍇ�A�����t���O���N���A����( fl = 0 )
										if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
									}
									if( fl ){	//	�l�Ԃ��S�����肵���ꍇ
										//	�Q�o���b�o�t�̏ꍇ�A�b�o�t�̃L�����N�^�[�I����( ms.sub_flow = 1 ),�Q�o���l�̏ꍇ�A���y�ݒ菈����( ms.sub_flow = 2 )
										ms.sub_flow = ( ms.pd[1].play == 0 )?2:1;
										break;
									}
								}else	if( joyupd[main_joy[i]] & CONT_B ){		//	�L�����Z��
									ms.sub_flow = 254;							//	��ʂ̃X�N���[��������
								}
							} else {				// ����ς�
								if( joyupd[main_joy[i]] & CONT_B ){				//	�L�����Z��
									pd->ok = 0;									//	����t���O�̃N���A
								}
							}
						}
					}
					break;
				case 1:		// �b�n�l�ݒ�
					pd = &ms.pd[1];

					if( joycur[main_joy[0]] & CONT_UP ){
						if( !pd->flag ){							//	���x���ݒ葤�̎�
							if( pd -> lv < 20 ){					//	�I���\�ō����x��( 20 )��艺�̏ꍇ
								pd -> lv ++;						//	���x���㏸
								dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
							}
						} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							if( pd -> speed < 2 ){					//	�ō����x��艺�������ꍇ
								pd -> speed ++;						//	����
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joycur[main_joy[0]] & CONT_DOWN ){
						if( !pd->flag ){							//	���x���ݒ葤�̎�
							if( pd -> lv > 0 ){						//	�Œ჌�x�����ゾ�����ꍇ
								pd -> lv --;						//	���x�����~
								dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
							}
						} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							if( pd -> speed > 0 ){					//	�Œᑬ�x���ゾ�����ꍇ
								pd -> speed --;						//	����
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
						if( pd->flag < 1 ){							//	���x���ݒ葤�̎�
							pd->flag ++;							//	�J�v�Z���X�s�[�h�ݒ葤�ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
						if( pd->flag > 0 ){							//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							pd->flag --;							//	���x���ݒ葤�ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	����
						pd->ok = 1;									//	����t���O�𗧂Ă�
						ms.sub_flow = 2;							//	���y�ݒ菈����
						dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
					}else	if( joyupd[main_joy[0]] & CONT_B ){		//	�L�����Z���`�F�b�N
						ms.pd[0].ok = 0;							//	�P�o�̌���t���O����
						ms.sub_flow = 0;							//	�P�o(�l�ԁj�̃��x���E�X�s�[�h�ݒ�ɖ߂�
					}
					break;
				case 2:		// ���y�ݒ�
					pd = &ms.pd[0];

					if( !ms.pd[1].play ){		// ��l�Ƃ��l��
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 1 ){				//	�w�i�I�𑤂̎�
								ui -> pos ++;					//	���y�ݒ葤�ɕύX
								dm_set_menu_se( SE_mUpDown );	//	SE�ݒ�
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0 ){				//	���y�ݒ葤�̎�
								ui -> pos --;					//	�w�i�I�𑤂ɕύX
								dm_set_menu_se( SE_mUpDown );	//	SE�ݒ�
							}
						}
						switch( ui->pos ){
							case 0:				//	�X�e�[�W( �w�i�I�� )
								if( ui->stg_adj_y == 0 ){	//	�w�i�I���O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->stg_no > 0 ){					//	�w�i�ԍ����P�ȏ�̎�
											ui->stg_no --;						//	�w�i�ԍ��̌���
											ui->stg_adj_y = -16;				//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->stg_no < ui->stg_sel_num - 1){		//	�w�i�ԍ����I���\�ő�ԍ���艺�̎�
											ui->stg_no ++;						//	�w�i�ԍ��̑���
											ui->stg_adj_y = 16;					//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}
								break;
							case 1:				// ���y�I��
								if( ui->mus_adj_y == 0 ){	//	���y�O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->mus_no > 0 ){					//	���y�ԍ����O����̏ꍇ
											ui->mus_no --;						//	���y�ԍ��̌���
											ui->mus_adj_y = -16;				//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->mus_no < 3 ){					//	���y�ԍ����R��艺�̏ꍇ
											ui->mus_no ++;						//	���y�ԍ��̑���
											ui->mus_adj_y = 16;					//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}
								break;
						}
					} else {					// �Q�o���b�n�l
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 2 ){					//	�J�[�\���ʒu���E�[( ���y�I�� )�łȂ���
								ui -> pos ++;						//	�E�Ɉړ�
								dm_set_menu_se( SE_mUpDown );		//	SE�ݒ�
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0 ){					//	�J�[�\���ʒu�����[( �w�i�I�� )�łȂ���
								ui -> pos --;						//	���Ɉړ�
								dm_set_menu_se( SE_mUpDown );		//	SE�ݒ�
							}
						}
						switch( ui->pos ){
							case 0:				//	�X�e�[�W( �w�i�I�� )
								if( ui->stg_adj_y == 0 ){	//	�w�i�I���O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->stg_no > 0 ){					//	�w�i�ԍ����P�ȏ�̎�
											ui->stg_no --;						//	�w�i�ԍ��̌���
											ui->stg_adj_y = -16;				//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->stg_no < ui->stg_sel_num - 1 ){		//	�w�i�ԍ����I���\�ő�ԍ���艺�̎�
											ui->stg_no ++;						//	�w�i�ԍ��̑���
											ui->stg_adj_y = 16;					//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}
								break;
							case 1:				// �b�n�l���x��
								if( ui->com_adj_y == 0 ){	//	�b�n�l���x���I���O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->com_lv < 2 ){					//	��Փx�� NORMAL �ȉ��̎�
											ui->com_lv ++;						//	��Փx�̏㏸
											ui->com_adj_y = -16;				//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->com_lv > 0 ){					//	��Փx�� NORMAL �ȏ�̎�
											ui->com_lv --;						//	��Փx�̌���
											ui->com_adj_y = 16;					//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}
								break;
							case 2:				// ���y
								if( ui->mus_adj_y == 0 ){	//	���y�O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
									if( joynew[main_joy[0]] & CONT_UP ){
										if( ui->mus_no > 0 ){					//	���y�ԍ����O����̏ꍇ
											ui->mus_no --;						//	���y�ԍ��̌���
											ui->mus_adj_y = -16;				//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE
										}
									}else	if( joynew[main_joy[0]] & CONT_DOWN ){
										if( ui->mus_no < 3 ){					//	���y�ԍ����R��艺�̏ꍇ
											ui->mus_no ++;						//	���y�ԍ��̑���
											ui->mus_adj_y = 16;					//	�X�N���[���l�̐ݒ�
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}
								break;
						}
					}

					// �L�����Z���`�F�b�N
					for( i = 0; i < 2; i ++ ){
						if( !ms.pd[i].play && joyupd[main_joy[i]] & CONT_B ){	//	�{�^�����������v���C���[���l�������ꍇ
							ms.pd[i].ok = 0;		//	�{�^�����������v���C���[�̌���t���O�̉���
							ms.sub_flow = 0;		//	���x���E�X�s�[�h�ݒ菈���ɖ߂�
							break;
						}
					}
					// ����
					if( joyupd[main_joy[0]] & DM_KEY_OK && ui->stg_adj_y == 0 && ui->com_adj_y == 0 && ui->mus_adj_y == 0 ){
						dm_set_menu_se( SE_mDecide );	//	SE�ݒ�
						ms.sub_flow = 255;				//	�Q�[���ݒ菈����
						break;
					}
					break;
				case 253:								//	�t���[���C��( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʓ��ɓ����Ă��� )
					if( ms.adj_x - ms.mv_x > 0 ){		//	�w����W�܂ňړ����Ă��Ȃ��ꍇ
						ms.adj_x -= ms.mv_x;
					} else {							//	�w����W�܂ňړ������ꍇ
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	���������x���E�X�s�[�h�ݒ�ɕύX
					}
					break;
				case 254:								//	�t���[���A�E�g( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʊO�ɏo�čs�� )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = 0;					//	���C���̏������L�����N�^�I����
						ms.sub_flow = 253;				//	�t���[���C��( ��̃v���[�g����ʓ��ɓ����Ă��鏈�� )
						init_2p_play(0,0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
				case 255:		// �Q�[����
					main_no = MAIN_12;					//	�Q�[���O�ݒ��
					main_60_out_flg = 0;				//	���C�����[�v�E�o
					break;
			}
			ui->stg_adj_y = ( ui->stg_adj_y-1 > 0 )?ui->stg_adj_y-1:( ui->stg_adj_y+1 < 0 )?ui->stg_adj_y+1:0;	//	�w�i�I���O���t�B�b�N�̃X�N���[������
			ui->com_adj_y = ( ui->com_adj_y-1 > 0 )?ui->com_adj_y-1:( ui->com_adj_y+1 < 0 )?ui->com_adj_y+1:0;	//	CPU��Փx�I���O���t�B�b�N�̃X�N���[������
			ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	���y�I���O���t�B�b�N�̃X�N���[������
			ms.cnt ++;			//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
			break;
	}
}

// �Q�o�p�`��
static void disp_2p_play( void )
{
	int i,j,k,fl,adj,col,top,mx;
	int l,mod,st,en;
	char cwo[10];
	char work1[10];

	u8 *smb;
	u8 *sms;
	u16 *smb_p;
	u16 *sms_p;

	PIC2 *pc;
	Player_Data *pd;
	Face_Data	*fd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;
	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	// �w�i
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	�c��
	{
		for(j = 0;j < 16;j++)	//	����
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// �^�C�g��
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	���[�h�Z���N�g
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	�{�^��(�`�F���� �a�F�L�����Z��)

	switch( ms.flow ){
		case 0:		// �L�����I��
			for( i = 0; i < 2; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// �p�l��
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				disp_tex_4bt( plate4p_char[i].pat, plate4p_char[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				// ��
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+25,319,pd->by+121 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				top = ( pd->chara >= 2 )?pd->chara-2:ms.max_chara-( 2-pd->chara );	//	��ԍŏ��ɕ`�悷���̐ݒ�
				if( fd->move == 1 ){							//	������ɃX�N���[�����Ă���ꍇ
					top = ( top - 1 >= 0 )?top-1:ms.max_chara-1;
				} else if( fd->move == -1 ){					//	�������ɃX�N���[�����Ă���ꍇ
					top = ( top + 1 < ms.max_chara )?top+1:0;
				}
				for( j = 0; j < 5; j ++ ){
					for( k = 0, fl = 1; k < 4; k ++ ){
						if( ms.pd[k].ok && ms.pd[k].chara == ((top+j)%ms.max_chara) && i != k )fl = 0;	//	���肪���肵�Ă��ē����L���������ׂĂ���
					}
					if( !fl || (pd->ok && j != 2) ){					//	����̌��肵���L�����Ɠ����L�����̏ꍇ
						gDPSetPrimColor( gp++, 0,0, 100,100,100,255 );	//	�Â��`�悷��
					} else {
						gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );	//	���邭�`�悷��
					}
					disp_tex_4bt_sc( faces[(top+j)%ms.max_chara].pat, faces[(top+j)%ms.max_chara].pal, 48, 48, pd->bx+7+ms.adj_x, pd->by+49+fd->pos_y[j], 1.0, fd->sc[j], 0 );
				}
				// ��g
				if( (ms.sub_flow == 0 && !pd->ok && !pd->play) || (ms.sub_flow == 1 && !pd->ok) ){	//	�܂����肵�Ă��Ȃ��ꍇ
					j = ( ( ms.cnt/30 ) % 2 ) * 127 + 128;											//	�_�ł�����
				} else {																			//	���肵�Ă��Ȃ��ꍇ
					j = 255;																		//	�_�ł����Ȃ�
				}
				gDPSetPrimColor( gp++, 0,0, j,j,j,255 );
				disp_tex_4bt( face_frame[i].pat, face_frame[i].pal, 48, 48, ms.pd[i].bx+7+ms.adj_x, ms.pd[i].by+49,SET_PTD );
			}

			// ��O���f(�C���e���V�e�B�g�p)
			gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);
			gDPSetPrimColor( gp++,0, 0, 0, 0, 0, 255);
			gDPSetEnvColor( gp++, 196, 196, 196, 255);
			for( i = 0; i < 2; i ++ ){
				pd = &ms.pd[i];

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,48-1,24-1,
								0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+25)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+25+24)<<2,
								G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,47,23,
								0, G_TX_CLAMP, G_TX_MIRROR, G_TX_NOMASK,5, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+97)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+97+24)<<2,
								G_TX_RENDERTILE, 0, (32+8)<<5, 1<<10, 1<<10);
			}
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);

			switch( ms.sub_flow ){
				case 0:		// �l�ԑI��
				case 253:	// �t���[���C��
				case 255:	// �v���C���[�l���I����
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// �I��
								// ���
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {			// ����
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// �n�j
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// �b�n�l�͑҂�
							// �Ãp�l��
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// �b�n�l�I��
					pd = &ms.pd[0];
					// �Ãp�l��
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
					// �n�j
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
					disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );

					pd = &ms.pd[1];
					// ���
					gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
					disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
					disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
					break;
				case 254: 	// �t���[���A�E�g
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						// �Ãp�l��
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// �n�j
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;

		case 1:		// ���x���I��

			// ��i�p�l��
			disp_tex_4bt( plate_1p_bm0_0, plate_1p_bm0_0tlut, 128,130,ms.pd[0].bx+ms.adj_x, ms.pd[0].by,SET_PTD );	//	�P�o���x���E�X�s�[�h�ݒ�v���[�g�`��
			disp_tex_4bt( plate_2p_bm0_0, plate_2p_bm0_0tlut, 128,130,ms.pd[1].bx+ms.adj_x, ms.pd[1].by,SET_PTD );	//	�Q�o���x���E�X�s�[�h�ݒ�v���[�g�`��
			if( !ms.pd[1].play ){	//	�Q�o���l�`�m( �l�� )�̏ꍇ
				pc = under_pl[1];	//	���̃p�l���� �w�i�E���y �̃O���t�B�b�N�ɂ���
			} else {				//	�Q�o���b�o�t�̏ꍇ
				//	�Q�o���x���E�X�s�[�h�ݒ�v���[�g�̂Q�o�̕����� �b�n�l�o�t�s�d�q �ƕ`�悷��
				disp_tex_4bt( plate_computer_bm0_0, plate_computer_bm0_0tlut, 58,8,ms.pd[1].bx+11+ms.adj_x, ms.pd[1].by+4,SET_PTD );
				pc = under_pl[2];	//	���̃p�l���� �w�i�E�b�o�t���x���E���y �̃O���t�B�b�N�ɂ���
			}
			// ���i�p�l��
			disp_tex_4bt( pc[0].pat,pc[0].pal,pc[0].width,pc[0].height, ui->bx+ms.adj_x,ui->by,SET_PTD );

			for( i = 0; i < 2; i ++ ){

				pd = &ms.pd[i];

				// �E�C���X
				for( k = 0; k < 3; k++ ){			//	�e�F���̃��[�v
					mod = SET_PTD;					//	�e�F���ڂ̕`��̎��́A�p���b�g�ƃO���t�B�b�N�����[�h����悤�ɐݒ肵�Ă���
					for( l = 0; l < 8; l++ ){		//	�����[�v
						for( j = 0; j < 13; j++ ){	//	�c���[�v
							col = (cap_pos[pd->lv][j] >> ( 14-l*2 )) & 0x3;	//	���W�f�[�^( l,j )�̔z�u�F�𒲂ׂ�( 0:����1:�� 2:�� 3:���F )
							if( (col > 0) && (col-1 == k ) ){				// 	�z�u������ŁA�z�u�F - 1 �� K �Ɠ����ꍇ�`�悷��
								disp_tex_4bt( &virus_bm0_0[k*20],virus_bm0_0tlut,8,5,pd->bx+17+l*5+ms.adj_x,pd->by+55+j*5,mod );
								mod = SET_D;		//	�e�F���ڈȍ~�̕`��̎��́A�p���b�g���O���t�B�b�N�����[�h���Ȃ��悤�ɐݒ肵�Ă���
							}
						}
					}
				}
				// ���x�����l
				adj = ( pd->lv < 10 )?4:0;
				itoa( pd->lv,work1,10 );
				for( j = 0; work1[j] != '\0'; j++ ){
					disp_tex_4bt( &lv_num_bm0_0[(work1[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+37+j*8+adj+ms.adj_x,pd->by+42,SET_PTD );
				}
				// ���x���Q�[�W
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+39+(20-pd->lv)*4,319,239);						//	�E�C���X���x���Q�[�W�̏㉺�́A�V�U�����O(�`��͈͎w��)�ōs���Ă���
				disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+6+ms.adj_x,pd->by+38,SET_PTD );
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);											//	�V�U�����O(�`��͈͎w��)�����ɖ߂�

				// �J�v�Z��
				disp_tex_4bt( cap_sp[i][pd->speed].pat,cap_sp[i][pd->speed].pal,56, 24, pd->bx+65+ms.adj_x,pd->by+32+(2-pd->speed)*33, SET_PTD );
			}

			col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

			gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+18+16);
			if( ui->stg_adj_y == 0 ){
				st = ui->stg_no;
				en = ui->stg_no;
			} else if( ui->stg_adj_y > 0 ){
				st = ui->stg_no-1;
				en = ui->stg_no;
			} else {
				st = ui->stg_no;
				en = ui->stg_no+1;
			}
			if( !ms.pd[1].play ){		// ��l�Ƃ��l��
				// �X�e�[�W
				for( i = st; i <= en; i ++ ){
					disp_tex_4bt( battle_stage_pic[ui->stg_tbl[i]].pat,battle_stage_pic[ui->stg_tbl[i]].pal,64,16,ui->bx+31+ms.adj_x,ui->by+18+(i-ui->stg_no)*16+ui->stg_adj_y,SET_PTD );
				}
				// ���y
				if( (ms.sub_flow == 2 && ui->pos == 1 ) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+165+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+165+ms.adj_x,ui->by+18,SET_PTD );
				}
			} else {					// �Q�o���b�n�l
				// �X�e�[�W
				for( i = st; i <= en; i ++ ){
					disp_tex_4bt( battle_stage_pic[ui->stg_tbl[i]].pat,battle_stage_pic[ui->stg_tbl[i]].pal,64,16,ui->bx+12+ms.adj_x,ui->by+18+(i-ui->stg_no)*16+ui->stg_adj_y,SET_PTD );
				}
				// �b�n�l���x��
				for( i = 0; i < 3; i ++ ){
					disp_tex_4bt( com_lv[i].pat,com_lv[i].pal,80,16,ui->bx+91+ms.adj_x,ui->by+18+(i+ui->com_lv-2)*16+ui->com_adj_y,SET_PTD );
				}
				// ���y
				if( (ms.sub_flow == 2 && ui->pos == 2 ) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,ui->bx+192+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,ui->bx+192+ms.adj_x,ui->by+18,SET_PTD );
				}
			}

			// ���i�Ãp�l��
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
			gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
			if( ms.sub_flow != 2 ){		// ���i�I�����ȊO�Ȃ�ł��������
				disp_tex_4bt( pc[1].pat,pc[1].pal,pc[1].width,pc[1].height, ui->bx+ms.adj_x,ui->by,SET_PTD );
			} else {					// ���i�I�����͏��������
				if( !ms.pd[1].play ){
					disp_tex_4bt( pc[2].pat,pc[2].pal,pc[2].width,pc[2].height, ui->bx+5+(1-ui->pos)*124+ms.adj_x,ui->by+4,SET_PTD );
				} else {
					for( i = 0; i < 3; i++ ){
						if( i != ui->pos ){
							disp_tex_4bt( pc[2].pat,pc[2].pal,pc[2].width,pc[2].height, ui->bx+i*85+5+ms.adj_x,ui->by+4,SET_PTD );
						}
					}
				}
			}

			switch( ms.sub_flow ){
				case 0:		// �l�ԑI��
				case 253:	// �t���[���C��
				case 254: 	// �t���[���A�E�g
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){
								// ��i�Ãp�l��
								gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
								disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60+ms.adj_x, pd->by+14,SET_PTD );

								// ��i���
								gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
								if( pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2+ms.adj_x,pd->by+34+(20-pd->lv)*4,SET_PTD );
								}
							} else {
								// ��i�Ãp�l��
								gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
								disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
								// �n�j
								gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {
							// ��i�Ãp�l��
							gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
							disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// �b�n�l�I��
					// ��i�Ãp�l��
					pd = &ms.pd[0];
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx+ms.adj_x,pd->by,SET_PTD );
					// �n�j
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );

					// ��i�Ãp�l��
					pd = &ms.pd[1];
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_p_bm0_0,plate_l_p_bm0_0tlut,64,112, pd->bx+4+(1-pd->flag)*60+ms.adj_x, pd->by+14,SET_PTD );
					// ��i���
					if( !pd->ok ){
						gDPSetPrimColor( gp++, 0,0, col,col,col,255 );
						if( pd->flag ){
							disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+59+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
						} else {
							disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx-2+(ms.cnt/15)%2+ms.adj_x,pd->by+34+(20-pd->lv)*4,SET_PTD );
						}
					}
					break;
				case 2:		// ���i
					pd = &ms.pd[0];
					gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239);
					gDPSetPrimColor( gp++, 0,0, col,col,col,255 );

					// ���
					if( !ms.pd[1].play ){		// ��l�Ƃ��l��
						switch( ui->pos ){
							case 0:					// �X�e�[�W�p���
								if( ui->stg_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+58+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->stg_no < ui->stg_sel_num-1 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+58+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 1:					// ���y�p���
								if( ui->mus_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+185+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->mus_no < 3 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+185+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
						}
					} else {					// �Q�o���b�n�l
						switch( ui->pos ){
							case 0:					// �X�e�[�W�p���
								if( ui->stg_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+41+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->stg_no < ui->stg_sel_num-1 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+41+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 1:					// �b�n�l���x���p���
								if( ui->com_lv < 2 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+126+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->com_lv > 0 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+126+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
							case 2:					// ���y�p���
								if( ui->mus_no > 0 ){
									disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+211+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
								}
								if( ui->mus_no < 3 ){
									disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+211+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
								}
								break;
						}
					}
					// �Ãp�l��
					gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, pd->bx,pd->by,SET_PTD );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[1].bx,ms.pd[1].by,SET_PTD );

					// �n�j
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95,pd->by+4,SET_PTD );
					}
					break;
				case 255:	// �v���C���[�l���I����
					// �Ãp�l��
					gSPDisplayList( gp++, Normal_XNZ_Texture_dl);
					gDPSetPrimColor( gp++, 0,0, 255,255,255,127 );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[0].bx+ms.adj_x,ms.pd[0].by,SET_PTD );
					disp_tex_4bt( plate_l_a_bm0_0,plate_l_a_bm0_0tlut,128,130, ms.pd[1].bx+ms.adj_x,ms.pd[1].by,SET_PTD );

					// �n�j
					gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					for( i = 0; i < 2; i ++ ){
						pd = &ms.pd[i];
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+95+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;
	}
}


// �S�o�p������
static void init_4p_play( u8 flag )
{
	int i,j;

	if( flag ){
		if( main_old == MAIN_GAME){						//	�Q�[������ݒ�ɗ����ꍇ
			ms.flow			= CS_4P_SELECT_LEVEL;		//	���x���E�X�s�[�h�E���y�ݒ�
		}else{											//	�Q�[���ȊO(���[�h�Z���N�g�A���O����)����ݒ�ɗ����ꍇ
			ms.flow			= CS_4P_SELECT_TEAM;		//	�`�[���I��
		}

		ms.adj_x		= 0;							//	��ʂw���W
		ms.mv_x			= 20;							//	�؂�ւ��̃X�N���[�����x�̐ݒ�
		ms.max_chara	= 11 + evs_secret_flg;			//	�ő�I���\�L�������̐ݒ�( �ʏ� 11 ���� �s�[�`���g�p�\�̏ꍇ(evs_secret_flg == 1) 12 )
		ms.ui.pos	= 0;								//	���i�p�l���̃J�[�\�������ʒu�����[�ɐݒ肷��

		ms.ui.mus_no 	= evs_4p_state_old.p_4p_m;		//	�O��̉��y�̐ݒ�
		ms.ui.com_lv	= evs_4p_state_old.p_4p_c_lv;	//	�O��� CPU��Փx�̐ݒ�
		ms.team			= evs_4p_state_old.p_4p_t_m;	//	�O��̃Q�[�����[�h�̐ݒ� 0:�o�g�����C���� 1:�`�[���o�g��
		ms.sub_flow		= 0;							//	�������䏈����l�ԃL�[���͂ɐݒ�

		for( i = 0;i < 4;i++ ){
			ms.pd[i].chara	= evs_4p_state_old.p_4p_no[i];					//	�O��g�����L�����N�^�[�ԍ��̐ݒ�
			ms.pd[i].lv		= evs_4p_state_old.p_4p_lv[i];					//	�O��̃E�C���X���x���̐ݒ�
			ms.pd[i].speed	= evs_4p_state_old.p_4p_sp[i];					//	�O��̃E�C���X�X�s�[�h�̐ݒ�
			ms.pd[i].p_team	= evs_4p_state_old.p_4p_team[i];				//	�O��̃`�[���ԍ��̐ݒ�
			ms.pd[i].play	= game_state_data[i].player_state[PS_PLAYER];	//	�O��̃v���C���[���( 0: �l�� 1:�b�o�t )
		}
		ms.pd[0].by = ms.pd[1].by = ms.pd[2].by = ms.pd[3].by = 47;			//	�L�����I���x���W�̐ݒ�( �Œ� )
		ms.pd[0].bx	= 33;													//	�L�����I���P�o�w���W�̐ݒ�
		ms.pd[1].bx = 97;													//	�L�����I���Q�o�w���W�̐ݒ�
		ms.pd[2].bx = 161;													//	�L�����I���R�o�w���W�̐ݒ�
		ms.pd[3].bx = 225;													//	�L�����I���S�o�w���W�̐ݒ�

		for( i = 0, j = 0; i < 4; i ++ ){
			if( ms.pd[i].play ) ms.coms[j++] = i;							//	�b�o�t�̔z��ԍ���ݒ�
		}

		ms.coms[j]	= -1;								//	�b�o�t�ł��~��(����p)�̐ݒ�

		ms.ui.bx	= ( ms.coms[0] == -1 )?96:33;		//	���i�p�l���̂w���W�̐ݒ�( �b�o�t�������ꍇ 33 ���Ȃ��ꍇ 96 )
		ms.ui.by	= 181;								//	���i�p�l���̂x���W�̐ݒ�
		ms.ui.pos	= 0;								//	���i�p�l���J�[�\�������ʒu�����[�ɐݒ肷��
		ms.ui.com_adj_y = 0;							//	�b�o�t��Փx�I��\���̃X�N���[���ړ��l�̃N���A
		ms.ui.mus_adj_y = 0;							//	���y�I��\���̃X�N���[���ړ��l�̃N���A

	}

	ms.cnt		= 0;			//	�����J�E���^�̃N���A
	ms.now_com	= 0;			// �b�n�l����p�J�E���^�̃N���A

	for( i = 0; i < 4; i ++ ){
		ms.pd[i].flag			= 0;		//	���x���E�X�s�[�h�ݒ�̓����I�������x���ݒ�ɂ���
		ms.pd[i].ok				= 0;		//	����t���O�̃N���A
		reset_face_size( &ms.pd[i].fd );	//	��֌W���W�E�X�P�[��������
	}
}

// �S�o�p���C��
static void calc_4p_play( void )
{
	int i,j,k,fl;
	s8	work,work_ok;

	Face_Data   *fd;
	Player_Data *pd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;

	switch( ms.flow ){
		case CS_4P_SELECT_TEAM:		//	�`�[���I��
			switch( ms.sub_flow ){
				case	0:			//	�o�g�����C�������`�[���o�g����I�����鏈��
					ms.cnt ++;									//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
					if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
						if( ms.team < 1 ){							//	�o�g�����C�����I�𑤂̎�
							ms.team = 1;							//	�`�[���o�g���I�𑤂ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_UP ){
						if( ms.team > 0 ){							//	�`�[���o�g���I�𑤂̎�
							ms.team = 0;							//	�o�g�����C�����I�𑤂ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
						if( ms.team == 0 ){							//	�o�g�����C�����I�𑤂̏ꍇ
							ms.sub_flow = 254;						//	��ʂ̃X�N���[��������
						}else{										//	�`�[���o�g���I�𑤂̏ꍇ
							ms.sub_flow = 1;						//	�`�[������������
						}
						dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
					}else	if( joyupd[main_joy[0]] & DM_KEY_B ){
						ms.sub_flow = 255;							//	�L�����Z��( ���[�h�Z���N�g�ɖ߂� )
					}
					break;
				case	1:	//	�`�[�������̏���( �l�� )
					ms.cnt ++;			//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
					for( i = 0;i < 4;i++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// �l�Ԃ̂ݑ����
							if( !pd->ok ){			//	�I��
								if( joyupd[i] & DM_KEY_LEFT ){
									if( pd -> p_team ){						//	�s�d�`�l �a�̏ꍇ
										pd -> p_team = 0;					//	�s�d�`�l �`�ɕύX
										dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
									}
								}else	if( joyupd[i] & DM_KEY_RIGHT ){
									if( !pd -> p_team ){					//	�s�d�`�l �`�̏ꍇ
										pd -> p_team = 1;					//	�s�d�`�l �a�ɕύX
										dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
									}
								}else	if( joyupd[i] & DM_KEY_OK ){		//	����
									//	�I�������`�[���Ō���ł��邩���ׂ鏈��
									k = pd -> p_team;
									for(j = fl = 0;j < 4;j++){						//	�`�[���l���v�Z
										if( ms.pd[j].p_team == k && ms.pd[j].ok ){	//	�`�[��������(ms.pd[j].p_team == k)�Ō��肵�Ă����ꍇ( ms.pd[j].ok )
											fl ++;									//	�`�[���l������
										}
									}
									if( fl < 3 ){							//	�I�������`�[���̐l�����Q�l�ȉ��̏ꍇ
										dm_set_menu_se( SE_mDecide );		//	SE�ݒ�
										pd -> ok = 1;						//	����t���O�𗧂Ă�
									}else{									//	�I�������`�[���̐l�����R�l�ȏ�̏ꍇ
																			//	����ł��܂���
										dm_set_menu_se( SE_mError );		//	SE�ݒ�
									}
									//	���̏����Ɉڂ邽�߂̔��菈��
									for(j = 0,fl = 1;j < 4;j++){
										if( !ms.pd[j].play && !ms.pd[j].ok ){	//	�v���C���[( �l�� )�ł܂����܂��Ă��Ȃ��l�������ꍇ
											fl = 0;
										}
									}
									if( fl ){	// �v���C���[( �l�� )���S�Č��܂����ꍇ
										if( ms.coms[0] == -1 ){	//	CPU �����Ȃ��ꍇ
											ms.sub_flow = 254;	//	��ʂ̃X�N���[��������
										}else{					//	CPU ������ꍇ
											ms.sub_flow = 2;	//	CPU �̃`�[���ݒ��
										}
									}
								}else	if( joyupd[i] & DM_KEY_B ){
									if( main_joy[0] == i ){
										ms.sub_flow = 0;		//	�L�����Z��( �`�[���o�g�����o�g�����C�������I�����鏈���ɖ߂� )
									}
								}
							}else{
								if( joyupd[i] & DM_KEY_B ){		//	�L�����Z��
									pd -> ok = 0;				//	�����Ԃ̃L�����Z��
								}
							}
						}
					}
					break;
				case	2:	//	CPU�̃`�[���ݒ�
					ms.cnt ++;		//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
					pd = &ms.pd[ms.coms[ms.now_com]];
					fd = &ms.pd[ms.coms[ms.now_com]].fd;
					if( joyupd[main_joy[0]] & DM_KEY_LEFT ){
						if( pd -> p_team ){								//	�s�d�`�l �a�̏ꍇ
							pd -> p_team = 0;							//	�s�d�`�l �`�ɕύX
							dm_set_menu_se( SE_mLeftRight );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_RIGHT ){
						if( !pd -> p_team ){							//	�s�d�`�l �`�̏ꍇ
							pd -> p_team = 1;							//	�s�d�`�l �a�ɕύX
							dm_set_menu_se( SE_mLeftRight );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){		//	����
						//	�I�������`�[���Ō���ł��邩���ׂ鏈��
						k = pd -> p_team;
						for(j = fl = 0;j < 4;j++){						//	�`�[���l���v�Z
							if( ms.pd[j].p_team == k && ms.pd[j].ok ){	//	�`�[��������(ms.pd[j].p_team == k)�Ō��肵�Ă����ꍇ( ms.pd[j].ok )
								fl ++;									//	�`�[���l������
							}
						}
						if( fl < 3 ){							//	�I�������`�[���̐l�����Q�l�ȉ��̏ꍇ
							dm_set_menu_se( SE_mDecide );		//	SE�ݒ�
							pd -> ok = 1;						//	����t���O�𗧂Ă�
						}else{									//	�I�������`�[���̐l�����R�l�ȏ�̏ꍇ
																			//	����ł��܂���
							dm_set_menu_se( SE_mError );		//	SE�ݒ�
						}
						//	���̏����Ɉڂ邽�߂̔��菈��
						for(j = 0,fl = 1;j < 4;j++){
							if( !ms.pd[j].ok ){	//	���܂��Ă��Ȃ��l�������ꍇ
								fl = 0;
							}
						}

						if( ms.coms[ms.now_com+1] != -1 ){	//	�܂��b�o�t���c���Ă���
							ms.now_com ++;					//	�b�o�t�̔ԍ������炷
						} else {
							if( fl ){						//	�S�����肵�Ă���
								ms.sub_flow = 254;			//	��ʂ̃X�N���[��������
							}
							break;
						}
					}
					// �L�����Z��
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & DM_KEY_B ){
							if( i == 0 ){								//	���C���̃R���g���[���̏ꍇ
								if( ms.now_com > 0 ){					//	�b�o�t�̐ݒ���s���Ă����ꍇ
									ms.now_com --;						//	�b�o�t�̑O�̂b�o�t�ɂ��炷
									ms.pd[ms.coms[ms.now_com]].ok = 0;	//	�O�̂b�o�t�̌���t���O����
								} else {								//	�b�o�t�̐ݒ�ł͂Ȃ������ꍇ
									ms.pd[i].ok = 0;					//	����t���O����
									ms.now_com = 0;						//	�b�o�t�̔ԍ����N���A
									ms.sub_flow = 1;					//	�l�Ԃ̃`�[���I�������ɖ߂�
								}
							} else {
								ms.pd[i].ok = 0;						//	�������l�̌���t���O����
								for( j = 0; j < ms.now_com; j ++ ){
									ms.pd[ms.coms[j]].ok = 0;			//	�b�o�t�̌���t���O����
								}
								ms.now_com = 0;							//	�b�o�t�̔ԍ����N���A
								ms.sub_flow = 1;						//	�l�Ԃ̃`�[���I�������ɖ߂�
							}
							break;
						}
					}

					break;
				case 253:								//	�t���[���C��( �`�[���I���̃v���[�g����ʓ��ɓ����Ă��� )
					if( ms.adj_x - ms.mv_x > 0 ){		//	�w����W�܂ňړ����Ă��Ȃ��ꍇ
						ms.adj_x -= ms.mv_x;
					} else {							//	�w����W�܂ňړ������ꍇ
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = ms.team;			//	�`�[���I�����Q�[�����[�h�I��(�o�g�����C���� �� �`�[���o�g�� )�����ɕύX
					}
					break;
				case 254:								//	�t���[���A�E�g( �`�[���I���̃v���[�g����ʊO�ɏo�čs�� )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_MAN;		//	���C���̏������L�����N�^�I����
						ms.sub_flow = 253;				//	�t���[���C��( ��̃v���[�g����ʓ��ɓ����Ă��鏈�� )
						init_4p_play(0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
				case 255:								// �v���C���[�l���I����
					main_no = MAIN_MODE_SELECT;			//	���[�h�Z���N�g�ɖ߂�
					main_60_out_flg = 0;				//	���C�����[�v�E�o
					break;
			}
			break;
		case CS_4P_SELECT_MAN:		// �L�����I��
			switch( ms.sub_flow ){
				case 0:			// �l�Ԑݒ�
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						fd = &ms.pd[i].fd;
						if( !pd->play ){		// �l�Ԃ̂ݑ����
							if( !pd->ok ){			//	�I��
								if( !fd->move ){		//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
									if( joynew[i] & CONT_UP ){
										dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
										pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	�L�����ԍ����O�ȉ��̏ꍇ�A�ő�I���L�����̔ԍ��ɐݒ�
										fd->move = -1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
										pd->flag = 1;														//	�X�N���[�������t���O��������X�N���[���ɐݒ�
									}else	if( joynew[i] & CONT_DOWN  ){
										dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
										pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	�L�����ԍ����ő�I���L�����̔ԍ��ȏ�ɂȂ����ꍇ�A�O�ɐݒ�
										fd->move = 1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
										pd->flag = 2;														//	�X�N���[�������t���O���������X�N���[���ɐݒ�
									}else	if( joyupd[i] & DM_KEY_OK  ){				// ����
										for( j = 0,fl = 1; j < 4; j++ ){
											//	���肪�����ԂŁA����̑I�������L�����Ǝ����̑I�������L�����������������ꍇ�A�����t���O���N���A����( fl = 0 )
											if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
										}
										if( fl ){												//	�����̃L���������肵���ꍇ( ����ƈႤ�L�������A���肪�܂������ԂłȂ������ꍇ )
											dm_set_menu_se( SE_mDecide );						//	SE�ݒ�
											pd->ok = 1;											//	����t���O�𗧂Ă�

											for( j = 0,fl = 1; j < 4; j++ ){					//	�����Ԃ𒲂ׂ�
												//	�v���C���[���l�Ԃł܂����肵�Ă��Ȃ������ꍇ�A�����t���O���N���A����( fl = 0 )
												if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
											}
											if( fl ){											//	���������ɐi�߂�
												//	�b�o�t������ꍇ( ms.sub_flow = 1 ),�b�o�t�����Ȃ��ꍇ�A��ʂ̃X�N���[��������( ms.sub_flow = 254 )
												ms.sub_flow = ( ms.coms[0] == -1 )?254:1;
												break;
											}
										}
									}else	if( joyupd[i] & CONT_B  ){	//	�L�����Z��
										ms.sub_flow = 255;				//	��ʃX�N���[��������( �`�[���I���̏����ɖ߂� )
									}
								}
								//	��̃X�N���[������
								set_face_size(pd,fd,ms.cnt);
							} else {
								// �L�����Z��
								if( joyupd[i] & CONT_B && !fd->move ){	//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
									pd->ok = 0;							//	����̉���
								}
							}
						}
					}
					break;
				case 1:			// �b�n�l�ݒ�
					pd = &ms.pd[ms.coms[ms.now_com]];
					fd = &ms.pd[ms.coms[ms.now_com]].fd;

					if( !fd->move ){	//	��̃O���t�B�b�N���X�N���[�����łȂ��ꍇ( !fd->move )
						if( joynew[main_joy[0]] & CONT_UP  ){
							dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
							pd->chara = ( pd->chara - 1 >= 0 )?pd->chara - 1:ms.max_chara - 1;	//	�L�����ԍ����O�ȉ��̏ꍇ�A�ő�I���L�����̔ԍ��ɐݒ�
							fd->move = -1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
							pd->flag = 1;														//	�X�N���[�������t���O��������X�N���[���ɐݒ�
						}else	if( joynew[main_joy[0]] & CONT_DOWN ){
							dm_set_menu_se( SE_mLeftRight );									//	SE�ݒ�
							pd->chara = ( pd->chara + 1 < ms.max_chara )?pd->chara + 1:0;		//	�L�����ԍ����ő�I���L�����̔ԍ��ȏ�ɂȂ����ꍇ�A�O�ɐݒ�
							fd->move = 1;														//	�X�N���[�����t���O�̐ݒ�( �X�N���[�����������˂� )
							pd->flag = 2;														//	�X�N���[�������t���O���������X�N���[���ɐݒ�
						}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){
							for( j = 0,fl = 1; j < 4; j++ ){
								//	���肪�����ԂŁA����̑I�������L�����Ǝ����̑I�������L�����������������ꍇ�A�����t���O���N���A����( fl = 0 )
								if( ms.pd[j].ok && pd->chara == ms.pd[j].chara ) fl = 0;
							}
							if( fl ){
								dm_set_menu_se( SE_mDecide );		//	SE�ݒ�
								pd->ok = 1;							//	����t���O�𗧂Ă�

								//	���̏����Ɉڂ邽�߂̔��菈��
								for(j = 0,fl = 1;j < 4;j++){
									if( !ms.pd[j].ok ){	//	���܂��Ă��Ȃ��l�������ꍇ
										fl = 0;
									}
								}

								if( ms.coms[ms.now_com+1] != -1 ){	//	�܂��b�o�t���c���Ă���
									ms.now_com ++;					//	�b�o�t�̔ԍ������炷
								} else {
									if( fl ){						//	�S�����肵�Ă���
										ms.sub_flow = 254;			//	��ʂ̃X�N���[��������
									}
									break;
								}
							}
						}
											// �L�����Z��
						for( i = 0; i < 4; i ++ ){
							if( !ms.pd[i].play && joyupd[i] & CONT_B  ){
								if( i == 0 ){								//	���C���̃R���g���[���̏ꍇ
									if( ms.now_com > 0 ){					//	�b�o�t�̐ݒ���s���Ă����ꍇ
										ms.now_com --;						//	�b�o�t�̑O�̂b�o�t�ɂ��炷
										ms.pd[ms.coms[ms.now_com]].ok = 0;	//	�O�̂b�o�t�̌���t���O����
									} else {								//	�b�o�t�̐ݒ�ł͂Ȃ������ꍇ
										ms.pd[i].ok = 0;					//	����t���O����
										ms.now_com = 0;						//	�b�o�t�̔ԍ����N���A
										ms.sub_flow = 0;					//	�l�Ԃ̃L�����I�������ɖ߂�
									}
								} else {
									ms.pd[i].ok = 0;						//	�������l�̌���t���O����
									for( j = 0; j < ms.now_com; j ++ ){
										ms.pd[ms.coms[j]].ok = 0;			//	�b�o�t�̌���t���O����
									}
									ms.now_com = 0;							//	�b�o�t�̔ԍ����N���A
									ms.sub_flow = 0;						//	�l�Ԃ̃L�����I�������ɖ߂�
								}
								break;
							}
						}
					}
					//	��̃X�N���[������
					set_face_size(pd,fd,ms.cnt);

					break;
				case 253:								//	�t���[���C��( ��̃v���[�g����ʓ��ɓ����Ă��鏈�� )
					if( ms.adj_x - ms.mv_x > 0 ){		//	�w����W�܂ňړ����Ă��Ȃ��ꍇ
						ms.adj_x -= ms.mv_x;
					} else {							//	�w����W�܂ňړ������ꍇ
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	�������L�����I���ɕύX
					}
					break;
				case 254:								//	�t���[���A�E�g( ��̃v���[�g����ʊO�ɏo�čs������ )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_LEVEL;	//	���C���̏��������x���E�X�s�[�h�ݒ��
						ms.sub_flow = 253;				//	�t���[���C��( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʓ��ɓ����Ă��� )�����ɕύX
						init_4p_play(0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
				case 255:								//	�t���[���A�E�g( ��̃v���[�g����ʊO�ɏo�čs������ )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_TEAM;	//	���C���̏������`�[���I��
						ms.sub_flow = 253;				//	�t���[���C��( �`�[���I���̃v���[�g����ʓ��ɓ����Ă��� )�����ɕύX
						init_4p_play(0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
			}
			ms.cnt ++;	//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
			break;

		case CS_4P_SELECT_LEVEL:	// ���x���I��
			switch( ms.sub_flow ){
				case 0:		// �l�Ԑݒ�
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){		// �l�Ԃ̂ݑ����
							if( !pd->ok ){			// �I��
								if( joycur[i] & CONT_UP ){
									if( !pd->flag ){							//	���x���ݒ葤�̎�
										if( pd -> lv < 20 ){					//	�I���\�ō����x��( 20 )��艺�̏ꍇ
											pd -> lv ++;						//	���x���㏸
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										if( pd -> speed < 2 ){					//	�ō����x��艺�������ꍇ
											pd -> speed ++;						//	����
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}else	if( joycur[i] & CONT_DOWN ){
									if( !pd->flag ){							//	���x���ݒ葤�̎�
										if( pd -> lv > 0 ){						//	�Œ჌�x�����ゾ�����ꍇ
											pd -> lv --;						//	���x�����~
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										if( pd -> speed > 0 ){					//	�Œᑬ�x���ゾ�����ꍇ
											pd -> speed --;						//	����
											dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
										}
									}
								}else	if( joyupd[i] & CONT_RIGHT ){
									if( pd->flag < 1 ){							//	���x���ݒ葤�̎�
										pd->flag ++;							//	�J�v�Z���X�s�[�h�ݒ葤�ɕύX
										dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
									}
								}else	if( joyupd[i] & CONT_LEFT ){
									if( pd->flag > 0 ){							//	�J�v�Z���X�s�[�h�ݒ葤�̎�
										pd->flag --;							//	���x���ݒ葤�ɕύX
										dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
									}
								}else	if( joyupd[i] & DM_KEY_OK ){			//	����
									pd->ok = 1;									//	����t���O�𗧂Ă�
									dm_set_menu_se( SE_mDecide );				//	SE�ݒ�

									for( j = 0,fl = 1; j < 4; j++ ){
										//	�v���C���[���l�Ԃł܂����肵�Ă��Ȃ������ꍇ�A�����t���O���N���A����( fl = 0 )
										if( !ms.pd[j].play && !ms.pd[j].ok ) fl = 0;
									}
									if( fl ){	//	�l�Ԃ��S�����肵���ꍇ
										//	�b�o�t�������ꍇ�A�b�o�t�̃L�����N�^�[�I����( ms.sub_flow = 1 ),�b�o�t�����Ȃ��ꍇ�A���y�ݒ菈����( ms.sub_flow = 2 )
										ms.sub_flow = ( ms.coms[0] == -1 )?2:1;
										break;
									}
								}else	if( joyupd[i] & CONT_B ){				//	�L�����Z��
									ms.sub_flow = 254;							//	��ʂ̃X�N���[��������
								}
							} else {				// ����ς�
								// �L�����Z��
								if( joyupd[i] & CONT_B ){						//	�L�����Z��
									pd->ok = 0;									//	����t���O�̃N���A
								}
							}
						}
					}
					break;
				case 1:		// �b�n�l�ݒ�
					pd = &ms.pd[ms.coms[ms.now_com]];

					if( joycur[main_joy[0]] & CONT_UP ){
						if( !pd->flag ){							//	���x���ݒ葤�̎�
							if( pd -> lv < 20 ){					//	�I���\�ō����x��( 20 )��艺�̏ꍇ
								pd -> lv ++;						//	���x���㏸
								dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
							}
						} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							if( pd -> speed < 2 ){					//	�ō����x��艺�������ꍇ
								pd -> speed ++;						//	����
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joycur[main_joy[0]] & CONT_DOWN ){
						if( !pd->flag ){							//	���x���ݒ葤�̎�
							if( pd -> lv > 0 ){						//	�Œ჌�x�����ゾ�����ꍇ
								pd -> lv --;						//	���x�����~
								dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
							}
						} else {									//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							if( pd -> speed > 0 ){					//	�Œᑬ�x���ゾ�����ꍇ
								pd -> speed --;						//	����
								dm_set_menu_se( SE_mLeftRight );	//	SE
							}
						}
					}else	if( joyupd[main_joy[0]] & CONT_RIGHT ){
						if( pd->flag < 1 ){							//	���x���ݒ葤�̎�
							pd->flag ++;							//	�J�v�Z���X�s�[�h�ݒ葤�ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
						if( pd->flag > 0 ){							//	�J�v�Z���X�s�[�h�ݒ葤�̎�
							pd->flag --;							//	���x���ݒ葤�ɕύX
							dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
						}
					}else	if( joyupd[main_joy[0]] & DM_KEY_OK ){	//	����
						dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
						pd->ok = 1;									//	����t���O�𗧂Ă�

						//	���̏����Ɉڂ邽�߂̔��菈��
						for(j = 0,fl = 1;j < 4;j++){
							if( !ms.pd[j].ok ){	//	���܂��Ă��Ȃ��l�������ꍇ
								fl = 0;
							}
						}

						if( ms.coms[ms.now_com+1] != -1 ){	//	�܂��b�o�t���c���Ă���
							ms.now_com ++;					//	�b�o�t�̔ԍ������炷
						} else {
							if( fl ){						//	�S�����肵�Ă���
								ms.sub_flow = 2;			//	���y�ݒ菈����
							}
							break;
						}
					}
					// �L�����Z���`�F�b�N
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & CONT_B ){
							if( i == 0 ){								//	���C���̃R���g���[���̏ꍇ
								if( ms.now_com > 0 ){					//	�b�o�t�̐ݒ���s���Ă����ꍇ
									ms.now_com --;						//	�b�o�t�̑O�̂b�o�t�ɂ��炷
									ms.pd[ms.coms[ms.now_com]].ok = 0;	//	�O�̂b�o�t�̌���t���O����
								} else {								//	�b�o�t�̐ݒ�ł͂Ȃ������ꍇ
									ms.pd[i].ok = 0;					//	����t���O����
									ms.now_com = 0;						//	�b�o�t�̔ԍ����N���A
									ms.sub_flow = 0;					//	�l�Ԃ̃��x���E�X�s�[�h�I�������ɖ߂�
								}
							} else {
								ms.pd[i].ok = 0;						//	�������l�̌���t���O����
								for( j = 0; j < ms.now_com; j ++ ){
									ms.pd[ms.coms[j]].ok = 0;			//	�b�o�t�̌���t���O����
								}
								ms.now_com = 0;							//	�b�o�t�̔ԍ����N���A
								ms.sub_flow = 0;						//	�l�Ԃ̃��x���E�X�s�[�h�I�������ɖ߂�
							}
							break;
						}
					}
					break;
				case 2:		// ���y�ݒ�
					pd = &ms.pd[0];

					if( ms.coms[0] == -1 ){		//	�b�o�t�����Ȃ��ꍇ�̏���
						if( ui->mus_adj_y == 0 ){	//	���y�O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
							if( joynew[main_joy[0]] & CONT_UP ){
								if( ui->mus_no > 0 ){					//	���y�ԍ����O����̏ꍇ
									ui->mus_no --;						//	���y�ԍ��̌���
									ui->mus_adj_y = -16;				//	�X�N���[���l�̐ݒ�
									dm_set_menu_se( SE_mLeftRight );	//	SE
								}
							}else	if( joynew[main_joy[0]] & CONT_DOWN ){
								if( ui->mus_no < 3 ){					//	���y�ԍ����R��艺�̏ꍇ
									ui->mus_no ++;						//	���y�ԍ��̑���
									ui->mus_adj_y = 16;					//	�X�N���[���l�̐ݒ�
									dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
								}
							}
						}
					} else {					//	�b�o�t������ꍇ�̏���
						if( joyupd[main_joy[0]] & CONT_RIGHT ){
							if( ui -> pos < 1 ){						//	�b�o�t��Փx�I�𑤂̎�
								ui->pos = 1;							//	���y�ݒ葤�ɕύX
								dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
							}
						}else	if( joyupd[main_joy[0]] & CONT_LEFT ){
							if( ui -> pos > 0){							//	���y�ݒ葤�̎�
								ui->pos = 0;							//	�b�o�t��Փx�I�𑤂ɕύX
								dm_set_menu_se( SE_mUpDown );			//	SE�ݒ�
							}
						}
						if( !ui->pos ){		//	�b�o�t��Փx�I�𑤂̎�
							if( ui->com_adj_y == 0 ){	//	�b�n�l���x���I���O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
								if( joynew[main_joy[0]] & CONT_UP ){
									if( ui->com_lv < 2 ){					//	��Փx�� NORMAL �ȉ��̎�
										ui->com_lv ++;						//	��Փx�̏㏸
										ui->com_adj_y = -16;				//	�X�N���[���l�̐ݒ�
										dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
									}
								}else	if( joynew[main_joy[0]] & CONT_DOWN ){
									if( ui->com_lv > 0 ){					//	��Փx�� NORMAL �ȏ�̎�
										ui->com_lv --;						//	��Փx�̌���
										ui->com_adj_y = 16;					//	�X�N���[���l�̐ݒ�
										dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
									}
								}
							}
						} else {			//	���y�ݒ葤�̎�
							if( ui->mus_adj_y == 0 ){	//	���y�O���t�B�b�N���X�N���[�����Ă��Ȃ��Ƃ�
								if( joynew[main_joy[0]] & CONT_UP ){
									if( ui->mus_no > 0 ){					//	���y�ԍ����O����̏ꍇ
										ui->mus_no --;						//	���y�ԍ��̌���
										ui->mus_adj_y = -16;				//	�X�N���[���l�̐ݒ�
										dm_set_menu_se( SE_mLeftRight );	//	SE
									}
								}else	if( joynew[main_joy[0]] & CONT_DOWN ){
									if( ui->mus_no < 3 ){					//	���y�ԍ����R��艺�̏ꍇ
										ui->mus_no ++;						//	���y�ԍ��̑���
										ui->mus_adj_y = 16;					//	�X�N���[���l�̐ݒ�
										dm_set_menu_se( SE_mLeftRight );	//	SE�ݒ�
									}
								}
							}
						}
					}

					// �L�����Z���`�F�b�N
					for( i = 0; i < 4; i ++ ){
						if( !ms.pd[i].play && joyupd[i] & CONT_B ){	//	�{�^�����������v���C���[���l�������ꍇ
							ms.pd[i].ok = 0;						//	�{�^�����������v���C���[�̌���t���O�̉���
							for( j = 0; ms.coms[j] != -1; j ++ ){
								ms.pd[ms.coms[j]].ok = 0;			//	�b�o�t�̌���t���O����
							}
							ms.now_com = 0;							//	�b�o�t�̔ԍ����N���A
							ms.sub_flow = 0;						//	�l�Ԃ̃��x���E�X�s�[�h�I�������ɖ߂�
							break;
						}
					}
					// ����
					if( joyupd[main_joy[0]] & DM_KEY_OK && ui->com_adj_y == 0 && ui->mus_adj_y == 0 ){
						dm_set_menu_se( SE_mDecide );				//	SE�ݒ�
						ms.sub_flow = 255;							//	�Q�[���ݒ菈����
						break;
					}
					break;
				case 253:								//	�t���[���C��( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʓ��ɓ����Ă��� )
					if( ms.adj_x - ms.mv_x > 0 ){		//	�w����W�܂ňړ����Ă��Ȃ��ꍇ
						ms.adj_x -= ms.mv_x;
					} else {							//	�w����W�܂ňړ������ꍇ
						ms.adj_x -= ms.mv_x;
						ms.sub_flow = 0;				//	���������x���E�X�s�[�h�ݒ�ɕύX
					}
					break;
				case 254:								//	�t���[���A�E�g( ���x���E�X�s�[�h�ݒ�̃v���[�g����ʊO�ɏo�čs�� )
					if( ms.adj_x + ms.mv_x < 320 ){		//	��ʊO�ɏo�Ă����Ă��Ȃ��ꍇ
						ms.adj_x += ms.mv_x;
					} else {							//	��ʊO�ɏo�Ă������ꍇ
						ms.adj_x += ms.mv_x;
						ms.flow = CS_4P_SELECT_MAN;		//	���C���̏������L�����N�^�I����
						ms.sub_flow = 253;				//	�t���[���C��( ��̃v���[�g����ʓ��ɓ����Ă��鏈�� )
						init_4p_play(0);				//	�ݒ菉����( ���W�n�ƌ���֌W�̃t���O�̂݃N���A����� )
					}
					break;
				case 255:		// �Q�[����
					main_no = MAIN_12;					//	�Q�[���O�ݒ��
					main_60_out_flg = 0;				//	���C�����[�v�E�o
					break;
			}
			ui->com_adj_y = ( ui->com_adj_y-1 > 0 )?ui->com_adj_y-1:( ui->com_adj_y+1 < 0 )?ui->com_adj_y+1:0;	//	CPU��Փx�I���O���t�B�b�N�̃X�N���[������
			ui->mus_adj_y = ( ui->mus_adj_y-1 > 0 )?ui->mus_adj_y-1:( ui->mus_adj_y+1 < 0 )?ui->mus_adj_y+1:0;	//	���y�I���O���t�B�b�N�̃X�N���[������
			ms.cnt ++;			//	�����J�E���^�̑���( ���̓_�œ��ɗ��p����� )
			break;
	}
}
// �S�o�p�`��
static void disp_4p_play( void )
{
	int i,j,k,fl,adj,col,top,mx;
	s8	a_fl[2],b_fl[2];
	s8	cwo[10];
	PIC2 *pc;
	Player_Data *pd;
	Face_Data	*fd;
	UNDER_ITEMS *ui;

	ui = &ms.ui;
	col = (s16)(sinf( ((ms.cnt*10) % 360) * 3.14159265 /180.0 ) * 64.0) + 191;

	// �w�i
	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );

	load_TexPal(dm_bg_tile_pal_3_bm0_0tlut);
	load_TexTile_4b( &dm_bg_tile_data_bm0_0[0],20,20,0,0,19,19 );
	for(i = 0;i < 12;i++)	//	�c��
	{
		for(j = 0;j < 16;j++)	//	����
		{
			draw_Tex(j * 20,i * 20,20,20,0,0);
		}
	}

	// �^�C�g��
	disp_tex_4bt( title_2p_1_bm0_0, title_2p_1_bm0_0tlut, 160, 31, 32, 12,SET_PTD );	//	���[�h�Z���N�g
	disp_tex_4bt( title_2p_2_bm0_0, title_2p_2_bm0_0tlut,  80, 32,208, 12,SET_PTD );	//	�{�^��(�`�F���� �a�F�L�����Z��)

	switch( ms.flow ){
		case CS_4P_SELECT_TEAM:		//	�`�[���I��

			//	�p�l��
			load_TexPal( dm_ts_pal_bm0_0tlut );
			load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );

			switch( ms.team ){
			case	0:	//	NORMAL
				//	�v���[�g�`��
				draw_Tex( ms.adj_x + 42,47,236,17,0,0 );				//	�V�䕔�� NOMAL ��
				draw_Tex( ms.adj_x + 42,100,236,14,0,0 );				//	�V�䕔�� TEAM ��

				//	NOMAL ��
				load_TexTile_4b( dm_ts_waku_b_bm0_0,236,8,0,0,235,7 );
				draw_Tex( ms.adj_x + 42,64,236,7,0,0 );					//	��̕���	NOMAL ��
				draw_ScaleTex( ms.adj_x + 42,71,236,6,236,10,0,1);		// 	���Ԃ̕���	NOMAL ��
				draw_Tex( ms.adj_x + 42,81,236,7,0,1 );					//	���̕���	NOMAL ��

				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,88,236,4,0,0 );					//	������ NOMAL ��
				draw_Tex( ms.adj_x + 42,114,236,4,0,0 );				//	������ TEAM ��

				//	NORMAL �̕���
				load_TexTile_4b( dm_ts_team_normal_bm0_0,46,10,0,0,45,9 );
				draw_Tex( ms.adj_x + 137,51,45,10,0,0 );
				//	TEAM �̕���
				load_TexTile_4b( dm_ts_team_4p_bm0_0,30,10,0,0,29,9 );
				draw_Tex( ms.adj_x + 145,103,30,10,0,0 );

				//	vs �̕���
				load_TexBlock_4b( dm_ts_vs_bm0_0,16,10 );
				for( i = 0;i < 3;i++ ){
					draw_Tex( ms.adj_x + 92 + i * 60,71,16,10,0,0 );
				}
				//	1P vs 2P vs 3P vs 4P( �l( P ) �� CPU( COM ) )
				for( i = 0;i < 4;i++ ){
					load_TexPal( team_4p_player[i][0].pal );
					load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
					draw_Tex( ms.adj_x + 49 + i * 60,68,42,16,0,0 );

				}

				//	���ʂŕ���
				gSPDisplayList( gp++, Texture_NZ_dl);

				gDPSetPrimColor( gp++, 0,0, 128,128,128,128 );	//	�Â�
				load_TexPal( dm_ts_waku_a_b_bm0_0tlut );
				load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );
				draw_Tex( ms.adj_x + 42,100,236,14,0,0 );
				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,114,236,4,0,0 );

				// ���
				gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
				load_TexPal( arrow_u_bm0_0tlut );
				load_TexBlock_4b( arrow_u_bm0_0,16,7 );
				draw_TexFlip(ms.adj_x + 154 ,92 + (ms.cnt/15)%2,16,7,0,0, flip_off,flip_on );
				break;
			case	1:	//	TEAM
				//	�v���[�g�`��
				draw_Tex( ms.adj_x + 42,47,236,14,0,0 );				//	�V�䕔�� NOMAL ��
				draw_Tex( ms.adj_x + 42,73,236,17,0,0 );				//	�V�䕔�� TEAM ��

				load_TexTile_4b( dm_ts_waku_b_bm0_0,236,8,0,0,235,7 );
				draw_Tex( ms.adj_x + 42,90,236,7,0,0 );					//	��̕���	TEAM ��
				draw_ScaleTex( ms.adj_x + 42,97,236,6,236,97,0,1);		// 	���Ԃ̕���	TEAM ��
				draw_Tex( ms.adj_x + 42,194,236,7,0,1 );				//	���̕���	TEAM ��

				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,61,236,4,0,0 );					//	������ NOMAL ��
				draw_Tex( ms.adj_x + 42,201,236,4,0,0 );				//	������ TEAM ��

				//	NORMAL �̕���
				load_TexTile_4b( dm_ts_team_normal_bm0_0,46,10,0,0,45,9 );
				draw_Tex( ms.adj_x + 137,51,45,10,0,0 );
				//	TEAM �̕���
				load_TexTile_4b( dm_ts_team_4p_bm0_0,30,10,0,0,29,9 );
				draw_Tex( ms.adj_x + 145,77,30,10,0,0 );

				//	VS �̕���
				load_TexBlock_4b( dm_ts_vs_bm0_0,16,10 );
				draw_Tex( ms.adj_x + 152,136,16,10,0,0 );

				//	TEAM A ���n
				load_TexBlock_4b( dm_ts_team_a_bm0_0,64,50 );
				draw_Tex( ms.adj_x + 84,94,64,50,0,0 );
				load_TexBlock_4b( &dm_ts_team_a_bm0_0[1600],64,53 );
				draw_Tex( ms.adj_x + 84,144,64,53,0,0 );

				//	TEAM B ���n
				load_TexBlock_4b( dm_ts_team_b_bm0_0,64,50 );
				draw_Tex( ms.adj_x + 171,94,64,50,0,0 );
				load_TexBlock_4b( &dm_ts_team_b_bm0_0[1600],64,53 );
				draw_Tex( ms.adj_x + 171,144,64,53,0,0 );

				//	1P vs 2P vs 3P vs 4P( �l( P ) �� CPU( COM ) )

				//	TEAM A SIDE
				for(i = 0;i < 4;i++){
					if( !ms.pd[i].p_team ){	//	�I���`�[���� �s�d�`�l �`�̏ꍇ
						load_TexPal( team_4p_player[i][0].pal );
						load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
						draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
					}
				}

				//	TEAM B SIDE
				for(i = 0;i < 4;i++){
					if( ms.pd[i].p_team ){	//	�I���`�[���� �s�d�`�l �a�̏ꍇ
						load_TexPal( team_4p_player[i][0].pal );
						load_TexTile_4b( team_4p_player[i][ms.pd[i].play].pat,42,16,0,0,41,15 );
						draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
					}
				}

				//	���ʂŕ���
				gSPDisplayList( gp++, Texture_NZ_dl);

				gDPSetPrimColor( gp++, 0,0, 128,128,128,128 );	//	�Â�
				load_TexPal( dm_ts_waku_a_b_bm0_0tlut );		//	���F�p���b�g�̃��[�h
				load_TexTile_4b( dm_ts_waku_a_bm0_0,236,17,0,0,235,16 );
				draw_Tex( ms.adj_x + 42,47,236,14,0,0 );
				load_TexTile_4b( dm_ts_waku_c_bm0_0,236,4,0,0,235,3 );
				draw_Tex( ms.adj_x + 42,61,236,4,0,0 );

				load_TexTile_4b( team_4p_player[0][0].pat,42,16,0,0,41,15 );
				switch( ms.sub_flow ){
				case	255:	//	���[�h�Z���N�g�ɖ߂�
				case 	253:	//	�t���[���C��
				case	0:	//	�`�[������
				case	1:	//	�l�ԑI��
					//	TEAM A SIDE
					for(i =  0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	�v���C���[
								if( ms.pd[i].ok ){	//	���肵�Ă���ꍇ
									draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
								}
							}else{	//	CPU
								draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
							}
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	�v���C���[
								if( ms.pd[i].ok ){	//	���肵�Ă���ꍇ
									draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
								}
							}else{	//	CPU
								draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
							}
						}
					}
					break;
				case	2://	�b�o�t�I��
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i != ms.coms[ms.now_com] ){	//	�I�𒆂̂b�n�l�łȂ�������
									draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
								}
							}else{	//	�v���C���[�̏ꍇ
								draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
							}
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i != ms.coms[ms.now_com] ){	//	�I�𒆂̂b�n�l�łȂ�������
									draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
								}
							}else{	//	�v���C���[�̏ꍇ
								draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );	//	CPU
							}
						}
					}
					break;
				case	254://	FRAME OU�s( �t���[���A�E�g���͑S�Ăɍ��������� )
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							draw_Tex( ms.adj_x + 96,113 + i * 20,42,16,0,0 );
						}
					}
					//	TEAM B SIDE
					for(i = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							draw_Tex( ms.adj_x + 183,113 + i * 20,42,16,0,0 );
						}
					}
					break;
				}

				// ����
				gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
				if( ms.sub_flow == 0 ){
					load_TexPal( arrow_u_bm0_0tlut );
					load_TexBlock_4b( arrow_u_bm0_0,16,7 );
					draw_TexFlip(ms.adj_x + 154 ,65 + (ms.cnt/15)%2,16,7,0,0, flip_off,flip_off);
				}

				//	�E���
				load_TexPal( arrow_r_bm0_0tlut );
				load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );

				switch( ms.sub_flow ){
				case	1:	//	�l�ԑI��
					//	TEAM A SIDE
					for(i = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	�v���C���[
								if( !ms.pd[i].ok ){	//	������
									draw_Tex( ms.adj_x + 138 + (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0 );	//	���`��
								}
							}
						}
					}
					break;
				case	2:	//	CPU�I��
					//	TEAM A SIDE
					for(i = j = 0;i < 4;i++){
						if( !ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i == ms.coms[ms.now_com] ){	//	�I�𒆂�CPU�̏ꍇ
									draw_Tex( ms.adj_x + 138 + (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0 );	//	���`��
								}
							}
						}
					}
					break;
				}

				switch( ms.sub_flow ){
				case	1:	//	�l�ԑI��
					//	TEAM B SIDE
					for(i = j = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( !ms.pd[i].play ){	//	�v���C���[
								if( !ms.pd[i].ok ){	//	������
									draw_TexFlip( ms.adj_x + 174 - (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0,flip_on,flip_off );	//	���`��
								}
							}
						}
					}
					break;
				case	2:
					//	TEAM B SIDE
					for(i = j = 0;i < 4;i++){
						if( ms.pd[i].p_team ){
							if( ms.pd[i].play ){	//	CPU
								if( i == ms.coms[ms.now_com] ){	//	�I�𒆂�CPU�̏ꍇ
									draw_TexFlip( ms.adj_x + 174 - (ms.cnt/15)%2 ,115 + i * 20,8,11,0,0,flip_on,flip_off );	//	���`��
								}
							}
						}
					}
					break;
				}
				break;
			}

			break;
		case CS_4P_SELECT_MAN:		// �L�����I��
			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// �p�l��
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				disp_tex_4bt( plate4p_char[i].pat, plate4p_char[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				//	�`�[���\��
				if( ms.team ){
					load_TexPal( dm_ts_pal_bm0_0tlut );
					load_TexTile_4b( dm_ts_team_ab_bm0_0,44,13,0,0,43,12 );
					draw_Tex( pd->bx+ms.adj_x+36,pd->by+1,22,13,ms.pd[i].p_team * 22,0 );
				}

				// ��
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+25,319,pd->by+121 );
				gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
				top = ( pd->chara >= 2 )?pd->chara-2:ms.max_chara-( 2-pd->chara );
				if( fd->move == 1 ){
					top = ( top - 1 >= 0 )?top-1:ms.max_chara-1;
				} else if( fd->move == -1 ){
					top = ( top + 1 < ms.max_chara )?top+1:0;
				}
				for( j = 0; j < 5; j ++ ){
					for( k = 0, fl = 1; k < 4; k ++ ){
						if( ms.pd[k].ok && ms.pd[k].chara == ((top+j)%ms.max_chara) && i != k )fl = 0;
					}
					if( !fl || (pd->ok && j != 2) ){
						gDPSetPrimColor( gp++, 0,0, 100,100,100,255 );
					} else {
						gDPSetPrimColor( gp++, 0,0, 255,255,255,255 );
					}
					disp_tex_4bt_sc( faces[(top+j)%ms.max_chara].pat, faces[(top+j)%ms.max_chara].pal, 48, 48, pd->bx+7+ms.adj_x, pd->by+49+fd->pos_y[j], 1.0, fd->sc[j], 0 );
				}
				// ��g
				if( (!ms.sub_flow && !pd->ok && !pd->play) || (ms.sub_flow == 1 && i == ms.coms[ms.now_com]) ){
					j = ( ( ms.cnt/30 ) % 2 ) * 127 + 128;
				} else {
					j = 255;
				}
				gDPSetPrimColor( gp++, 0,0, j,j,j,255 );
				disp_tex_4bt( face_frame[i].pat, face_frame[i].pal, 48, 48, ms.pd[i].bx+7+ms.adj_x, ms.pd[i].by+49,SET_PTD );
			}

			// ��O���f
			gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);
			gDPSetPrimColor( gp++,0, 0, 0, 0, 0, 255);
			gDPSetEnvColor( gp++, 196, 196, 196, 255);
			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,48-1,24-1,
								0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+25)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+25+24)<<2,
								G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);

				gDPLoadTextureTile_4b( gp++, face4p_sm_bm0_0, G_IM_FMT_I,48,24,0,0,47,23,
								0, G_TX_CLAMP, G_TX_MIRROR, G_TX_NOMASK,5, G_TX_NOLOD, G_TX_NOLOD);
				gSPScisTextureRectangle( gp++, (pd->bx+7+ms.adj_x)<<2, (pd->by+97)<<2, (pd->bx+7+48+ms.adj_x)<<2, (pd->by+97+24)<<2,
								G_TX_RENDERTILE, 0, (32+8)<<5, 1<<10, 1<<10);
			}
			gSPDisplayList( gp++, Normal_XNZ_Texture_dl);

			switch( ms.sub_flow ){
				case 0:		// �l�ԑI��
				case 253:	// �t���[���C��
				case 255:	// �v���C���[�l���I����
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// �I��
								// ���
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {			// ����
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// �n�j
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// �b�n�l�͑҂�
							// �Ãp�l��
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					break;
				case 1:		// �b�n�l�I��
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];

						if( !pd->play ){		// �l��
							// �Ãp�l��
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
							// �n�j
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
							disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
						} else {				// �b�n�l
							if( i == ms.coms[ms.now_com] ){
								// ���
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+41-(ms.cnt/15)%2,SET_PTD );
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,pd->bx+26+ms.adj_x,pd->by+98+(ms.cnt/15)%2,SET_PTD );
							} else {
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								if( pd->ok ){
									// �n�j
									gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
									disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
								}
							}
						}
					}
					break;
				case 254: 	// �t���[���A�E�g
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// �Ãp�l��
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// �n�j
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
			}
			break;

		case CS_4P_SELECT_LEVEL:		// ���x���I��

			pd = &ms.pd[0];

			// ���i�p�l��
			if( ms.coms[0] == -1 ){
				mx = ui->bx;
				disp_tex_4bt( plate_env1_bm0_0,plate_env1_bm0_0tlut,128, 42, ui->bx+ms.adj_x,ui->by,SET_PTD );
			} else {
				mx = ui->bx + 124;
				disp_tex_4bt( com_music_bm0_0,com_music_bm0_0tlut,256, 42, ui->bx+ms.adj_x,ui->by,SET_PTD );
			}

			// �b�n�l���x��
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,ui->by+18,319,ui->by+34 );
			if( ms.coms[0] != -1 ){
				for( i = 0; i < 3; i ++ ){
					disp_tex_4bt( com_lv[i].pat,com_lv[i].pal,80,16,ui->bx+26+ms.adj_x,ui->by+18+(i+ui->com_lv-2)*16+ui->com_adj_y,SET_PTD );
				}
			}
			// ���y
			if( ms.coms[0] == -1 ){
				if( ms.sub_flow == 2 || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,mx+41+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				}else{
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,mx+41+ms.adj_x,ui->by+18,SET_PTD );
				}
			}else{
				if( (ms.sub_flow == 2 && ui->pos == 1) || ui->mus_adj_y != 0 ){
					for( i = 0; i < 4; i ++ ){
						disp_tex_4bt( music[i][(ms.cnt/30)%2].pat,music[i][(ms.cnt/30)%2].pal,48,16,mx+41+ms.adj_x,ui->by+18+(i-ui->mus_no)*16+ui->mus_adj_y,SET_PTD );
					}
				} else {
					disp_tex_4bt( music[ui->mus_no][0].pat,music[ui->mus_no][0].pal,48,16,mx+41+ms.adj_x,ui->by+18,SET_PTD );
				}
			}
			gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );

			for( i = 0; i < 4; i ++ ){
				pd = &ms.pd[i];
				fd = &ms.pd[i].fd;
				pc = &plcom[i][pd->play];

				// �p�l��
				disp_tex_4bt( plate4p_lv[i].pat, plate4p_lv[i].pal, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );

				// Player �� COM
				disp_tex_4bt( pc->pat, pc->pal, pc->width, pc->height, pd->bx+4+ms.adj_x, pd->by+4,SET_PTD );

				// �I���X�s�[�h
				disp_tex_4bt( speed4p_pat[pd->speed], speed4p_pal[i], 28, 24, pd->bx+33+ms.adj_x, pd->by+32+(2-pd->speed)*33,SET_PTD );

				// ���x���Q�[�W
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,pd->by+(20-pd->lv)*4+41,319,239);
				disp_tex_4bt( lv_guage_bm0_0,lv_guage_bm0_0tlut,8,84, pd->bx+15+ms.adj_x, pd->by+40,SET_PTD );
				gDPSetScissor( gp++,G_SC_NON_INTERLACE,0,0,319,239 );

				// ���x�����l
				adj = ( pd->lv < 10 )?4:0;
				itoa( pd->lv,cwo,10 );
				for( j = 0; cwo[j] != '\0'; j++ ){
					disp_tex_4bt( &lv_num_bm0_0[(cwo[j]-'0')*48],lv_num_bm0_0tlut,8,12,pd->bx+14+j*8+adj+ms.adj_x,pd->by+27,SET_PTD );
				}
			}
			//	�`�[���\��
			if( ms.team ){
				load_TexPal( dm_ts_pal_bm0_0tlut );
				load_TexTile_4b( dm_ts_team_ab_bm0_0,44,13,0,0,43,12 );
				for(i = 0;i < 4;i++){
					pd = &ms.pd[i];
					draw_Tex( pd->bx+ms.adj_x+36,pd->by+1,22,13,ms.pd[i].p_team * 22,0 );
				}
			}

			gDPSetRenderMode( gp++, G_RM_XLU_SURF,G_RM_XLU_SURF2 );

			switch( ms.sub_flow ){
				case 0:		// �l�Ԑݒ�
				case 253:	// �t���[���C��
				case 254:	// �t���[���A�E�g
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						if( !pd->play ){
							if( !pd->ok ){		// �I��
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_sms_bm0_0, plate4p_sms_bm0_0tlut, 28, 112, pd->bx+3+(1-pd->flag)*29+ms.adj_x, pd->by+14,SET_PTD );
								// ���
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								if( !pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+6+(ms.cnt/15)%2+ms.adj_x,pd->by+36+(20-pd->lv)*4,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11,pd->bx+25+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								}
							} else {			// ����
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								// �n�j
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
								disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
							}
						} else {				// �b�n�l�͑҂�
							// �Ãp�l��
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						}
					}
					// ���i�Ãp�l��
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
				case 1:		// �b�n�l�ݒ�
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];

						if( !pd->play ){
							// �Ãp�l��
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
							disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
							// �n�j
							gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
							disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
						} else {
							if( i == ms.coms[ms.now_com] ){
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_sms_bm0_0, plate4p_sms_bm0_0tlut, 28, 112, pd->bx+3+(1-pd->flag)*29+ms.adj_x, pd->by+14,SET_PTD );
								// ���
								gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
								if( !pd->flag ){
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11, pd->bx+6+(ms.cnt/15)%2+ms.adj_x,pd->by+36+(20-pd->lv)*4,SET_PTD );
								} else {
									disp_tex_4bt( arrow_r_bm0_0,arrow_r_bm0_0tlut,8,11,pd->bx+25+(ms.cnt/15)%2+ms.adj_x,pd->by+38+(2-pd->speed)*33,SET_PTD );
								}
							} else {
								// �Ãp�l��
								gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
								disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
								if( pd->ok ){
									// �n�j
									gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
									disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
								}
							}
						}
					}
					// ���i�Ãp�l��
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
				case 2:		// �b�n�l���x���E���y�ݒ�
					// ���
					gDPSetPrimColor( gp++,0, 0, col, col, col, 255);
					if( ms.coms[0] == -1 ){		// �b�n�l�����Ȃ��Ƃ��͂�����
						if( ui->mus_no > 0 ){
							disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
						}
						if( ui->mus_no < 3 ){
							disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
						}
					} else {					// �b�n�l������Ƃ��͂�����
						pd = &ms.pd[0];
						if( !ui->pos ){			// �b�n�l�k�u�p���
							if( ui->com_lv < 2 ){
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,ui->bx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
							}
							if( ui->com_lv > 0 ){
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,ui->bx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
							}
						} else {				// ���y�p���
							if( ui->mus_no > 0 ){
								disp_tex_4bt( arrow_u_bm0_0,arrow_u_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+11-(ms.cnt/15)%2,SET_PTD );
							}
							if( ui->mus_no < 3 ){
								disp_tex_4bt( arrow_d_bm0_0,arrow_d_bm0_0tlut,16,7,mx+61+ms.adj_x,ui->by+34+(ms.cnt/15)%2,SET_PTD );
							}
						}
						// �Ãp�l��
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate_e_p_bm0_0,plate_e_p_bm0_0tlut,128,34,ui->bx+5+(1-ui->pos)*124, ui->by+4,SET_PTD );
					}
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// �Ãp�l��
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// �n�j
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					break;
				case 255:		// �Q�[����
					for( i = 0; i < 4; i ++ ){
						pd = &ms.pd[i];
						// �Ãp�l��
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
						disp_tex_4bt( plate4p_smb_bm0_0, plate4p_smb_bm0_0tlut, 64, 130, pd->bx+ms.adj_x, pd->by,SET_PTD );
						// �n�j
						gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 255);
						disp_tex_4bt( menu_ok2_bm0_0, menu_ok2_bm0_0tlut,  16,	8,pd->bx+42+ms.adj_x,pd->by+4,SET_PTD );
					}
					// ���y�Ãp�l��
					gDPSetPrimColor( gp++,0, 0, 255, 255, 255, 127);
					if( ms.coms[0] == -1 ){
						disp_tex_4bt( music_sm_bm0_0,music_sm_bm0_0tlut,128,42,ui->bx+ms.adj_x,ui->by,SET_PTD );
					} else {
						disp_tex_4bt( plate_e_a_bm0_0,plate_e_a_bm0_0tlut,256,42, ui->bx+ms.adj_x,ui->by,SET_PTD );
					}
					break;
			}
			break;
	}
}

void main60(NNSched* sched)
{
	OSMesgQueue msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	int i;
	game_state_old_sub	*config;

	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	// create message queue for VI retrace
	nnScAddClient(sched, &client, &msgQ);			// add client to shceduler


	// �f�[�^�ǂݍ���
	auRomDataRead( (u32)_lv_sel_dataSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)(_lv_sel_dataSegmentRomEnd - _lv_sel_dataSegmentRomStart));

	for(i = 0;i < MAXCONTROLLERS;i ++){	//	�L�[���s�[�g�Ή��L�[�̐ݒ�
		joyflg[i] = 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT ;
	}
	joycur1 = 24;
	joycur2 = 6;
	joystp = FALSE;			// start button pause ... off.
	#ifdef STICK_3D_ON
	joystk = TRUE;			// joy stick key.
	#endif

	main_60_out_flg = 1;

	evs_seqence = TRUE;			//	���y�t���O�𗧂Ă�
	evs_seqnumb = SEQ_Menu;		//	�Ȃ̐ݒ�
	dm_init_se();				//	���ʉ��̏�����

	// ������
	switch( evs_gamesel ){
		case GSL_1PLAY:
			init_1p_play();
			break;
		case GSL_2PLAY:
			init_2p_play(1,0);
			break;
		case GSL_4PLAY:
			init_4p_play(1);
			break;
		case GSL_VSCPU:
			if( evs_story_flg ){
				init_story_play();
			}else{
				init_2p_play(1,1);
			}
			break;
	}

	//	��ʐ؂�ւ�菈��
	if( main_old == MAIN_GAME ){
		 HCEffect_Init(gfx_freebuf[0], HC_SPHERE);
	}else{
		 HCEffect_Init(gfx_freebuf[0], HC_CURTAIN);
	}

#ifdef	DM_DEBUG_FLG
	osSyncPrintf("Start Main60 \n");
#endif

	graphic_no = GFX_60;

	// ���C�����[�v
	while( main_60_out_flg ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.
		if(	!HCEffect_Main() ){		//	��ʐ؂�ւ�����
			switch( evs_gamesel ){
				case GSL_1PLAY:
					calc_1p_play();	//	�PPLAY�p����
					break;
				case GSL_2PLAY:
					calc_2p_play();	//	�QPLAY�p����
					break;
				case GSL_4PLAY:
					calc_4p_play();	//	�SPLAY�p����
					break;
				case GSL_VSCPU:
					if( evs_story_flg ){
						calc_story_play();	//	STORY�p����
					}else{
						calc_2p_play();		//	�QPLAY�p����( ������CPU�p�̏������s���Ă��� )
					}
					break;
			}
		}
		dm_menu_se_play();	//	���ʉ��Đ�
		#ifndef THREAD_JOY
		joyProcCore();
		#endif
	}
	if( main_no == MAIN_12 ){											// �Q�[���ɍs���ꍇ�A�Ȃ̍Đ����~����
		auSeqPlayerStop(0);												// stop the audio.
		while( auSeqPlayerState(0) != AL_STOPPED ) {
			(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		}
	}
	graphic_no = GFX_NULL;												// graphics finish.
	while( pendingGFX != 0 ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	// code of charm. 980213.
	nnScRemoveClient(sched, &client);									// remove client to shceduler

	// �㏈��
	main_old = MAIN_60;													//	���݂̃��[�h�̕ۑ�
	switch( evs_gamesel ){
		case GSL_1PLAY:
			// �X�s�[�h
			config = &evs_mem_data[evs_select_name_no[0]].state_old;

			game_state_data[0].cap_def_speed			= ms.pd[0].speed;	//	�J�v�Z���������x�̐ݒ�
			config -> p_1p_sp							= ms.pd[0].speed;	//	�J�v�Z���������x�̐ݒ�( �ݒ�̕ۑ� )

			// �E�B���X
			game_state_data[0].virus_level				= ms.pd[0].lv;		//	�E�C���X���x���̐ݒ�
			config -> p_1p_lv 							= ms.pd[0].lv;		//	�E�C���X���x���̐ݒ�( �ݒ�̕ۑ� )

			//	���y
			config -> p_1p_m 							= ms.ui.mus_no;		//	�Ȕԍ��̐ݒ�( �ݒ�̕ۑ� )
			break;
		case GSL_2PLAY:
			for( i = 0; i < 2; i++ ){
				config = &evs_mem_data[evs_select_name_no[i]].state_old;

				// �L����
				config -> p_vm_no								= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �ݒ�̕ۑ� )

				game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �\���p )
				game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �v�l�p )

				// �X�s�[�h
				game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	�J�v�Z���������x�̐ݒ�
				config -> p_vm_sp								= ms.pd[i].speed;	//	�J�v�Z���������x�̐ݒ�( �ݒ�̕ۑ� )

				// �E�B���X
				game_state_data[i].virus_level					= ms.pd[i].lv;		//	�E�C���X���x���̐ݒ�
				config -> p_vm_lv								= ms.pd[i].lv;		//	�E�C���X���x���̐ݒ�( �ݒ�̕ۑ� )

				//	�w�i
				config -> p_vm_st								= ms.ui.stg_tbl[ms.ui.stg_no];	//	�w�i�ԍ��ݒ�( �ݒ�̕ۑ� )

				//	���y
				config -> p_vm_m 								= ms.ui.mus_no;		//	�Ȕԍ��̐ݒ�( �ݒ�̕ۑ� )
			}
			// �X�e�[�W
			evs_stage_no = ms.ui.stg_tbl[ms.ui.stg_no];			//	�w�i�ԍ��ݒ�
			break;
		case GSL_4PLAY:
			for( i = 0; i < 4; i ++ ){
				// �L����
				game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �\���p )
				game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �v�l�p )
				evs_4p_state_old.p_4p_no[i]						= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �ݒ�̕ۑ� )

				// �X�s�[�h
				game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	�J�v�Z���������x�̐ݒ�
				evs_4p_state_old.p_4p_sp[i]						= ms.pd[i].speed;	//	�J�v�Z���������x�̐ݒ�( �ݒ�̕ۑ� )

				// �E�B���X
				game_state_data[i].virus_level					= ms.pd[i].lv;		//	�E�C���X���x���̐ݒ�
				evs_4p_state_old.p_4p_lv[i]						= ms.pd[i].lv;		//	�E�C���X���x���̐ݒ�( �ݒ�̕ۑ� )

				// �b�n�l���x��
				if( ms.pd[i].play ){
					game_state_data[i].player_state[PS_CPU_LEVEL]	= ms.ui.com_lv;	//	CPU���x���̐ݒ�
				}

				//	�`�[������
				if( ms.team == 0){	//	�o�g�����C�����̏ꍇ
					game_state_data[i].player_state[PS_TEAM_FLG]	= i;	//	�`�[���ԍ��̐ݒ�( �S���قȂ�( 0,1,2,3 ) )
				}else{				//	�`�[����̏ꍇ
					game_state_data[i].player_state[PS_TEAM_FLG]	= ms.pd[i].p_team;	//	�`�[���ԍ��̐ݒ�( 0 �� 1 )
					evs_4p_state_old.p_4p_team[i]					= ms.pd[i].p_team;	//	�`�[���ԍ��̐ݒ�( �ݒ�̕ۑ� )
				}
			}
			evs_4p_state_old.p_4p_t_m	= ms.team;			//	�Q�[�����[�h( �`�[���킩�o�g�����C���� )
			evs_4p_state_old.p_4p_m 	= ms.ui.mus_no;		//	�Ȕԍ��̐ݒ�( �ݒ�̕ۑ� )
			evs_4p_state_old.p_4p_c_lv	= ms.ui.com_lv;		//	CPU���x���̐ݒ�( �ݒ�̕ۑ� )
			break;
		case GSL_VSCPU:
			config = &evs_mem_data[evs_select_name_no[0]].state_old;
			if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
				switch( ms.flow ){
				case	CS_STORY_SERECT_LEVEL:	//	�ʏ�̏ꍇ
					evs_story_level 	= ms.pd[0].lv;		//	���x���ݒ�( EASY/NORMAL/HARD )
					config -> p_st_lv 	= ms.pd[0].lv;		//	���x���ݒ�( �ݒ�̕ۑ� )
					evs_story_no 		= 0;				//	�X�g�[���[�ԍ����O( 1�� )�ɂ���
					evs_one_game_flg	= 0;				//	���ꏈ���t���O������
					break;
				case	CS_STORY_SERECT_STAGE:	//	�X�e�[�W�Z���N�g�̏ꍇ
					evs_story_level 		= ms.pd[1].lv;			//	���x���ݒ�
					evs_story_no 			= ms.pd[1].speed;		//	�X�e�[�W�ݒ�
					config -> p_st_st_lv 	= ms.pd[1].lv;			//	���x���ݒ�( �ݒ�̕ۑ� )
					config -> p_st_st 		= ms.pd[1].speed + 1;	//	�X�e�[�W�ݒ�( �ݒ�̕ۑ� )

					evs_one_game_flg = 1;							//	���ꏈ���t���O�𗧂Ă�
					break;
				}
				main_old = MAIN_60;
			}else{				//	�u�r�b�o�t�̏ꍇ
				for( i = 0; i < 2; i++ ){
					// �L����
					game_state_data[i].anime.cnt_charcter_no		= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �\���p )
					game_state_data[i].player_state[PS_THINK_FLG]	= ms.pd[i].chara;	//	�L�����N�^�ԍ��̐ݒ�( �v�l�p )

					// �X�s�[�h
					game_state_data[i].cap_def_speed				= ms.pd[i].speed;	//	�J�v�Z���������x�̐ݒ�

					// �E�B���X
					game_state_data[i].virus_level					= ms.pd[i].lv;		//	�E�C���X���x���̐ݒ�

					// �b�n�l���x��
					if( ms.pd[i].play ){
						game_state_data[i].player_state[PS_CPU_LEVEL]	= ms.ui.com_lv;	//	CPU���x���̐ݒ�
					}
					config -> p_vc_lv[i] 	= ms.pd[i].lv;					//	�E�C���X���x���̐ݒ�( �ݒ�̕ۑ� )
					config -> p_vc_sp[i] 	= ms.pd[i].speed;				//	�J�v�Z���������x�̐ݒ�( �ݒ�̕ۑ� )
					config -> p_vc_no[i] 	= ms.pd[i].chara;				//	�L�����N�^�ԍ��̐ݒ�( �ݒ�̕ۑ� )
				}
				config -> p_vc_cp_lv 		= ms.ui.com_lv;					//	CPU���x���̐ݒ�( �ݒ�̕ۑ� )
				config -> p_vc_st 			= ms.ui.stg_tbl[ms.ui.stg_no];	//	�w�i�ԍ��ݒ�( �ݒ�̕ۑ� )
				config -> p_vm_m 			= ms.ui.mus_no;					//	�Ȕԍ��̐ݒ�( �ݒ�̕ۑ� )


				// �X�e�[�W
				evs_stage_no = ms.ui.stg_tbl[ms.ui.stg_no];	//	�w�i�ԍ��ݒ�

			}
			break;
	}

	for( i = 0;i < 4;i++ ){
		game_state_data[i].game_score = 0;	//	���_�̏�����
		game_state_data[i].game_retry = 0;	//	���g���C�񐔂̏�����
	}

	evs_game_time = 0;	//	���ʃQ�[���^�C��������

	//	���y�ݒ�
	if( evs_one_game_flg ){
		evs_seqnumb = evs_story_no % 3;		//	�Ȕԍ��̐ݒ�
		evs_seqence = TRUE;					//	�a�f�l��炷
	}else{
		if ( ms.ui.mus_no < 3) {
			evs_seqnumb = ms.ui.mus_no;		//	�Ȕԍ��̐ݒ�
			evs_seqence = TRUE;				//	�a�f�l��炷
		} else {
			evs_seqence = FALSE;			//	�a�f�l��炳�Ȃ�
		}
	}
	if( main_no == MAIN_MODE_SELECT ){		//	���[�h�Z���N�g�ɖ߂�
		evs_one_game_flg	= 0;			//	���ꏈ���t���O������
	}


	return;
}

// �O���t�B�b�N
void graphic60(void)
{
	NNScTask *t;

	// graphic display list buffer. task buffer.
	gp = &gfx_glist[gfx_gtask_no][0];
	t  = &gfx_task[gfx_gtask_no];

	// RSP initialize.

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT,	osVirtualToPhysical(gfx_freebuf[0]));
	gSPSegment(gp++, BG_SEGMENT,	osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE]));

	F3RCPinitRtn();
	F3ClearFZRtn(0);


	S2RDPinitRtn(TRUE);

	// ���[�h�ʕ���
	switch( evs_gamesel ){
		case GSL_1PLAY:
			disp_1p_play();
			break;
		case GSL_2PLAY:
			disp_2p_play();
			break;
		case GSL_4PLAY:
			disp_4p_play();
			break;
		case GSL_VSCPU:
			if( evs_story_flg ){
				disp_story_play();
			}else{
				disp_2p_play();
			}
			break;
	}
	HCEffect_Grap(&gp);

	// end of display list & go the graphic task.
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();
	gfxTaskStart(t, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}



