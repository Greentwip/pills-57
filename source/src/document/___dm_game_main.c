/*--------------------------------------
	filename	:	dm_game_main.c

	create		:	1999/08/02
	modify		:	1999/12/02

	created		:	Hiroyuki Watanabe
--------------------------------------*/
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
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"
#include "aiset.h"

#include "dm_nab_include.h"
#include "o_segment.h"
#include "record.h"

#define		ANIME_RESTRT_TIME	600	//	5�b
#define		WINMAX	3	//	���s����p�̏�����(�ʏ�͂R)

#ifdef	DM_DEBUG_FLG

#include	"tex/nab/test/debug_font.bft"	//	�f�o�b�N�p�t�H���g�f�[�^


u16	debug_font_def_pal[100][2];
u16	debug_font_def_pal_p;

s16	num_keta[4] = {4,4,4,4};
s16	param_pos[4] = {0,0,0,0};

u8	keyRept[4] = {0,0,0,0};

#include	"aidebug.h"

/*--------------------------------------
	�f�o�b�N�p�֐��Q
--------------------------------------*/
/*--------------------------------------
	�t�H���g�Ǎ��݊֐�(�f�o�b�N�p�̊֐��𗘗p����ꍇ�A�P��Ă�)
--------------------------------------*/
void	disp_font_load(void)
{
	//	�t�H���g�ǂݒ�����
	load_TexBlock_4b(debug_font_bm0_0,96,30);

	//	�Ǎ��݃p���b�g�ԍ��̏�����
	debug_font_def_pal_p = 0;
}

/*--------------------------------------
	�P�����`��֐�
--------------------------------------*/
void	disp_debug_font(s16	x_pos,s16 y_pos,s8 font)
{
	s8	ascii,x,y;
	//	�p���b�g�͂��łɌĂ΂�Ă�����̂Ƃ���

	ascii = font - 0x20;
	x = ascii % 16;
	y = ascii >> 4;

	//	�t�H���g��������
	draw_Tex( x_pos,y_pos,6,5,x * 6,y * 5);
}
/*--------------------------------------
	������`��֐�
--------------------------------------*/
void	disp_debug_string(STRTBL *str)
{
	s8	i,*str_dat;
	s16	x_pos,y_pos;

	x_pos = str -> x_pos;
	y_pos = str -> y_pos;
	str_dat = str -> str;

	debug_font_def_pal[debug_font_def_pal_p][0] = str -> color;	//	�`��F�̐ݒ�
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	while(1){
		if( *str_dat == '\0' ){
			break;
		}
		switch( *str_dat ){
		case	' ':	//	�X�y�[�X
			break;
		default:	//	�`��
			disp_debug_font( x_pos,y_pos,*str_dat );
		}
		x_pos += 6;
		str_dat ++;
	}
}
/*--------------------------------------
	�����`��֐�
--------------------------------------*/
void	disp_debug_num(NUMTBL *num)
{
	s8	i,start,zero_flg,ascii[5];
	s16	x_pos,y_pos,num_dat;

	x_pos = num -> x_pos;
	y_pos = num -> y_pos;
	num_dat = num -> num;



	debug_font_def_pal[debug_font_def_pal_p][0] = num -> color;	//	�`��F�̐ݒ�
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	if( num_dat < 0 ){	//	�}�C�i�X
		num_dat *= -1;
		disp_debug_font( x_pos,y_pos,0x2d );
		x_pos += 6;
	}
	ascii[0] = num_dat / 10000;							//	��
	ascii[1] = (num_dat % 10000) / 1000;				//	��
	ascii[2] = ((num_dat % 10000) % 1000) / 100;		//	�S
	ascii[3] = (((num_dat % 10000) % 1000) % 100) / 10;	//	�\
	ascii[4] = (((num_dat % 10000) % 1000) % 100) % 10;	//	��

	zero_flg = 0;
	start = 5 - (num -> flg & 0x0f);
	for(i =  start;i < 5;i++){
		if( ascii[i] == 0 ){	//	�O�̂Ƃ�
			if( !zero_flg ){
				if( num -> flg & NUM_ZERO ){	//	�O���߃t���O�������Ă�����
					disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
					x_pos += 6;
				}else{
					if( num -> flg & NUM_RIGHT ){	//	�E�񂹃t���O�������Ă�����
						x_pos += 6;
					}
				}
			}else{
				disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
				x_pos += 6;
			}
		}else{
			disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
			zero_flg = 1;
			x_pos += 6;
		}
	}
}
/*--------------------------------------
	�����`��֐�
--------------------------------------*/
void	disp_debug_param(u8 player_no,PARAMTBL *param, u8 ctrlno)
{
	s8	i,zero_flg,ascii[5];
	s16	x_pos,y_pos,num_dat;

	x_pos = param -> x_pos;
	y_pos = param -> y_pos;
	num_dat = *(param -> data);

	debug_font_def_pal[debug_font_def_pal_p][0] = param -> color;	//	�`��F�̐ݒ�
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	if( num_dat < 0 ){	//	�}�C�i�X
		num_dat *= -1;
		disp_debug_font( x_pos,y_pos,0x2d );
	}
	x_pos += 6;
	ascii[0] = num_dat / 10000;							//	��
	ascii[1] = (num_dat % 10000) / 1000;				//	��
	ascii[2] = (num_dat % 1000) / 100;					//	�S
	ascii[3] = (num_dat % 100) / 10;					//	�\
	ascii[4] = num_dat % 10;							//	��

	zero_flg = 0;
	for(i = 0;i < 5;i++){
		if( param_pos[player_no] == ctrlno && num_keta[player_no] == i ){	//	���쒆�̌��̏ꍇ
			disp_debug_font( x_pos,y_pos + 6,0x5e );

		}
		disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
		x_pos += 6;
	}
}

/*--------------------------------------
	�p�����[�^����֐�
--------------------------------------*/
void	cnt_debug_param( s8 player_no,PARAMTBL *param )
{
	s16	num[] = { 10000,1000,100,10,1 };

	if( joyupd[player_no] & LEVER_LEFT ){ //	���グ
		if( num_keta[player_no] > 0 ){
			num_keta[player_no] --;
		}
	}else	if( joyupd[player_no] & LEVER_RIGHT ){	//	������
		if( num_keta[player_no] < 4 ){
			num_keta[player_no] ++ ;
		}
	}else	if( joyupd[player_no] & LEVER_UP ){ //	���l�グ
		if( *(param -> data) + num[num_keta[player_no]] <= param -> max ){
			*(param -> data) += num[num_keta[player_no]];
		}
	}else	if( joyupd[player_no] & LEVER_DOWN ){ //	���l����
		if( *(param -> data) - num[num_keta[player_no]] >= param -> min ){
			*(param -> data) -= num[num_keta[player_no]];
		}
	}
}

/*--------------------------------------
	�f�o�b�N�`��֐�
--------------------------------------*/
void	cnt_debug_main(s8 player_no)
{
	cnt_debug_param(0,&dai_param_table[ param_pos[player_no] ]);
	if( joyold[player_no] & L_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] > 0 ) {
				param_pos[player_no] --;
			}
		}
		keyRept[player_no]++;
	} else if( joyold[player_no] & R_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] < 39 - 1 ) {
				param_pos[player_no] ++;
			}
		}
		keyRept[player_no]++;
	} else {
		keyRept[player_no] = 0;
	}

	if( joyold[player_no] & L_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] > 0 ) {
				param_pos[player_no] --;
			}
		}
		keyRept[player_no]++;
	} else if( joyold[player_no] & R_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] < 39 - 1 ) {
				param_pos[player_no] ++;
			}
		}
		keyRept[player_no]++;
	} else {
		keyRept[player_no] = 0;
	}
	dai_param_table[2].data = &(ai_param[daiLevel][daiMode].dt_LnOnLinVrs);
	dai_param_table[3].data = &(ai_param[daiLevel][daiMode].dt_LnEraseVrs);
	dai_param_table[4].data = &(ai_param[daiLevel][daiMode].dt_EraseLinP1);
	dai_param_table[5].data = &(ai_param[daiLevel][daiMode].dt_HeiEraseLinRate);
	dai_param_table[6].data = &(ai_param[daiLevel][daiMode].dt_WidEraseLinRate);
	dai_param_table[7].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_FallCap);
	dai_param_table[8].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_FallVrs);
	dai_param_table[9].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_Capsule);
	dai_param_table[10].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_Virus);
	dai_param_table[11].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_HeightP);
	dai_param_table[12].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Bottom);
	dai_param_table[13].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_FallCap);
	dai_param_table[14].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_FallVrs);
	dai_param_table[15].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Capsule);
	dai_param_table[16].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Virus);
	dai_param_table[17].data = &(ai_param[daiLevel][daiMode].dt_OnVirusP);
	dai_param_table[18].data = &(ai_param[daiLevel][daiMode].dt_RensaP);
	dai_param_table[19].data = &(ai_param[daiLevel][daiMode].dt_RensaMP);
	dai_param_table[20].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp2);
	dai_param_table[21].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp3);
	dai_param_table[22].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp4);
	dai_param_table[23].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp5);
	dai_param_table[24].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp6);
	dai_param_table[25].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp7);
	dai_param_table[26].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp8);
	dai_param_table[27].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp2);
	dai_param_table[28].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp3);
	dai_param_table[29].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp4);
	dai_param_table[30].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp5);
	dai_param_table[31].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp6);
	dai_param_table[32].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp7);
	dai_param_table[33].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp8);
	dai_param_table[34].data = &(ai_param[daiLevel][daiMode].dt_LnHighCaps);
	dai_param_table[35].data = &(ai_param[daiLevel][daiMode].dt_LnHighVrs);
	dai_param_table[36].data = &(ai_param[daiLevel][daiMode].dt_aiWall_F);

}
/*--------------------------------------
	�f�o�b�N�`��֐�
--------------------------------------*/
void	disp_debug_main_1p(void)
{
	int		i;

	disp_font_load();

	disp_debug_string( &daiLevelStr[daiLevel] );
	disp_debug_string( &daiModeStr[daiMode] );
	disp_debug_string( &daiDebugP1Str[debug_p1cpu] );

	for( i = 0; i < 26;i++) {
		disp_debug_string( &dai_str_table[i] );
	}

	for( i = 0; i < 39;i++) {
		disp_debug_param( 0,&dai_param_table[i],(u8)i );
	}
}

#endif

#ifdef NN_SC_PERF
static	s8	disp_meter_flg = 0;
#endif

//#include	"tex/nab/test_speed.bft"	//	�f�o�b�N�p�t�H���g�f�[�^

s8		testes_sound_no = 56;
s8		m_g;

#ifdef	DM_S2D_MAIN
		uObjSprite	s2d_dm_map_item[8 << 4];
#endif

static	uObjBg	s2d_dm_bg;						//	�w�i�\���p�ϐ�
static	uObjBg	s2d_demo_4p;					//	�X�g�[���[�S�o���Ɏg�p

static	game_anime	big_virus_anime[3];			//	����E�C���X�A�j������p�ϐ�
static	game_anime	disp_anime_data[2][2];		//	�A�j���[�V�����f�[�^�\���p�ϐ�
static	coin_pos	dm_retry_coin_pos[3];		//	���g���C�p�̃R�C���̕ϐ�
static	lv_score_pos	lv_sc_pos[10];			//	���_���o�p�ϐ�
static	s32	seq_save;							//	�Đ����y�ԍ��ۑ��p�ϐ�
static	s16	virsu_num_size[4][3];				//	�E�C���X���̑傫��
static	s16	big_virus_position[3][3];			//	����E�C���X�̍��W
static	u16	dm_rand_seed = 0;					//	�����_���̎�
static	s16	dm_start_time;						//	��P���̃^�C�}�[
static	u16	dm_demo_time;						//	�f���̃^�C�}�[
static	s16	dm_retry_alpha[2];					//	���g���C�̓_�ŗp�ϐ� 0:�J�[�\���̃A���t�@ 1:�J�[�\���̓_�ŗp�T�C���l
static	s16	dm_vsmode_win_point[2];				//	2PLAY & VSCOM �|�C���g�\���p�ϐ�
static	u16	*story_4p_demo_bg_tlut;				//	4P ���� �ΐ�O�̊�\���܍���I��p(�p���b�g�f�[�^)
static	s16	story_4p_demo_w[3];					//	�X�g�[���[�� 4P ���� �ΐ�O�̊�\���c��
static	s16	story_4p_demo_h[3];					//	�X�g�[���[�� 4P ���� �ΐ�O�̊�\���c��
static	s16	story_4p_demo_face_pos[5];			//	�X�g�[���[�� 4P ���� �ΐ�O�̊�\�� �w���W
static	s16	dm_anime_restart_count;				//	���s���t������A�j���[�V�������ĉғ������邽�߂̃J�E���^
static	s8	eep_rom_flg[2];						//	EEP �������ݗp�t���O
static	s8	dm_think_flg = 0;					//	�v�l�J�n�t���O
static	s8	dm_TouchDownWait;					//	���ٶ�߾ق̉��ɉ�������Ƃ��̳��Ă����Z
static	s8	dm_game_init_flg;					//	���������s�����Ƃ��ɁA�P��� �w�i�݂̂̕`����s�����߂̃t���O
static	s8	dm_peach_stage_flg;					//	�s�[�`�X�e�[�W���g�p����t���O
static	u8	dm_peach_get_flg[2];				//	�s�[�`�P�l���̉��o�p
static	u8	dm_demo_time_flg;					//	�f���̃t���O
static	s8	dm_retry_position[4][3];			//	���g���C�̃J�[�\���ʒu 0:�J�[�\���̏㉺�ʒu 1:�J�[�\���̍��E�ʒu 2:�J�[�\���̍��E�ړ��p�J�E���^
static	s8	dm_retry_pos[4];					//	���F���Ɋւ���ϐ�
static	u8	dm_retry_coin;						//	���g���C�p�̃R�C���̖���
static	u8	dm_retry_flg;						//	���g���C�\���ǂ����\���ɔ��f�����邽�߂̃t���O
static	u8	win_count[4];						//	�������J�E���^
static	u8	vs_win_count[2];					//	�������J�E���^(�R���łP������)
static	u8	vs_lose_count[2];					//	�s�k���J�E���^(�R�����łP������)
static	u8	win_anime_count[2];					//	�������J�E���^
static	u8	dm_game_bg_no;						//	VSCOM 2PLAY�̂Ƃ��̏㉺�̃^�C���ԍ�
static	u8	bgm_bpm_flg[2];						//	BGM�̑�����ς���t���O
static	u8	last_3_se_flg;						//	�E�C���X���R�ȉ��ɂȂ����Ƃ���SE��炷���߂̃t���O
static	s8	story_time_flg;						//	�X�g�[���[�̃^�C���J�E���g�̊J�n�t���O
static	s8	big_virus_count[4];					//	�E�C���X���̊e�F�ʂ̌��̕ϐ�
static	s8	big_virus_flg[3][2];				//	0:���ŃA�j���[�V��������񂾂��ݒ肷��t���O 1:���ŉ�����񂾂��ݒ肷��t���O
static	s8	story_4p_demo_flg[2];				//	�X�g�[���[�� 4P ���� �ΐ�O�̊�\��
static	s8	dm_game_4p_team_flg;				//	�`�[���΍R��̃t���O
static	u8	dm_game_4p_team_bit;				//	�`�[���΍R��̔���p�t���O
static	u8	dm_game_4p_team_bits[2];			//	�`�[���΍R��̔���p�t���O
static	u8	*story_4p_demo_bg;					//	4P ���� �ΐ�O�̊�\�������I��p(�O���t�B�b�N�f�[�^)
static	s8	story_4p_name_flg[2];				//	�X�g�[���[�� 4P ���� MARIO.ENEMY �����E�̂ǂ��炩�ɕ\�����邩�̃t���O
static	s8	story_4p_name_num[4];				//	�X�g�[���[�� 4P ���� �`�[���̃����o�[��
static	s8	story_4p_stock_cap[4][4];			//	�X�g�[���[�� 4P ���� �X�g�b�N�J�v�Z���p�z��( ���������Ƃ��� -1 ������)

u16	*capsel_pal[] = {	//	�J�v�Z���ƃE�C���X�̃p���b�g�z��(��)
	TLUT_mitem10r,
	TLUT_mitem10y,
	TLUT_mitem10b,
	TLUT_mitem10rx,
	TLUT_mitem10yx,
	TLUT_mitem10bx,
};

u16	*capsel_8_pal[] = {	//	�J�v�Z���ƃE�C���X�̃p���b�g�z��(��)
	TLUT_mitem08r,
	TLUT_mitem08y,
	TLUT_mitem08b,
	TLUT_mitem08rx,
	TLUT_mitem08yx,
	TLUT_mitem08bx
};

u16	**cap_pal[] = {	//	�J�v�Z���̃p���b�g
	capsel_pal,
	capsel_8_pal
};

u8	*cap_tex[] = {	//	�J�v�Z���̃O���t�B�b�N
	TEX_mitem10r_0_0,
	TEX_mitem08r_0_0
};

//	�E�C���X�̃A�j���[�V�����p�^�[���ԍ�
u8	virus_anime_table[][4] = {
	{virus_a1,virus_a2,virus_a3,virus_a4},	//	�ԗp
	{virus_b1,virus_b2,virus_b3,virus_b4},	// 	���p
	{virus_c1,virus_c2,virus_c3,virus_c4},	//	�p
};

//	2P���̍U���r�d�e�[�u��
s8	dm_chaine_se_table_vs[] = {
	dm_se_attack_2p,dm_se_attack_1p
};

//	4P���̍U���r�d�e�[�u��
s8	dm_chaine_se_table_4p[4][3] = {
	{dm_se_attack_4p,dm_se_attack_3p,dm_se_attack_2p},
	{dm_se_attack_1p,dm_se_attack_4p,dm_se_attack_3p},
	{dm_se_attack_2p,dm_se_attack_1p,dm_se_attack_4p},
	{dm_se_attack_3p,dm_se_attack_2p,dm_se_attack_1p},
};

//	�A�j���[�V�����f�[�^�̃����A�h���X
u8	*dm_anime_rom_address[][2] = {
	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	�}���I
	{_dm_anime_nokoSegmentRomStart,_dm_anime_nokoSegmentRomEnd},		//	�m�R�m�R
//	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	�m�R�m�R
	{_dm_anime_bomheiSegmentRomStart,_dm_anime_bomheiSegmentRomEnd},	//	�{����
	{_dm_anime_pukuSegmentRomStart,_dm_anime_pukuSegmentRomEnd},		//	�v�N�v�N
	{_dm_anime_choroSegmentRomStart,_dm_anime_choroSegmentRomEnd},		//	�`�����u�[
	{_dm_anime_heihoSegmentRomStart,_dm_anime_heihoSegmentRomEnd},		//	�v���y���w�C�z�[
	{_dm_anime_hanaSegmentRomStart,_dm_anime_hanaSegmentRomEnd},		//	�n�i�`����
//	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	�n�i�`����
	{_dm_anime_teresaSegmentRomStart,_dm_anime_teresaSegmentRomEnd},	//	�e���T
	{_dm_anime_pakuSegmentRomStart,_dm_anime_pakuSegmentRomEnd},		//	�p�b�N���t�����[
	{_dm_anime_kameSegmentRomStart,_dm_anime_kameSegmentRomEnd},		//	�J���b�N
	{_dm_anime_kuppaSegmentRomStart,_dm_anime_kuppaSegmentRomEnd},		//	�N�b�p
	{_dm_anime_peachSegmentRomStart,_dm_anime_peachSegmentRomEnd},		//	�s�[�`
	{_dm_anime_virusSegmentRomStart,_dm_anime_virusSegmentRomEnd},		//	�E�C���X
};

//	�w�i�f�[�^�̃����A�h���X
u8	*dm_bg_rom_address[] = {
	_bg_dat_01SegmentRomStart,
	_bg_dat_02SegmentRomStart,
	_bg_dat_03SegmentRomStart,
	_bg_dat_04SegmentRomStart,
	_bg_dat_05SegmentRomStart,
	_bg_dat_06SegmentRomStart,
	_bg_dat_07SegmentRomStart,
	_bg_dat_08SegmentRomStart,
	_bg_dat_09SegmentRomStart,
	_bg_dat_10SegmentRomStart
};

//	�A�j���[�V�����f�[�^�̃e�[�u��
game_anime_data	**game_anime_table[] = {
	game_anime_data_mario,		//	�}���I
	game_anime_data_noko,		//	�m�R�m�R
//	game_anime_data_mario,		//	�m�R�m�R
	game_anime_data_bomhei,		//	�{����
	game_anime_data_puku,		//	�v�N�v�N
	game_anime_data_choro,		//	�`�����u�[
	game_anime_data_heiho,		//	�v���y���w�C�z�[
	game_anime_data_hana,		//	�n�i�`����
//	game_anime_data_mario,		//	�m�R�m�R
	game_anime_data_teresa,		//	�e���T
	game_anime_data_paku,		//	�p�b�N���t�����[
	game_anime_data_kame,		//	�J���b�N
	game_anime_data_kuppa,		//	�N�b�p
	game_anime_data_peach,		//	�s�[�`
	game_anime_data_virus_r,	//	�ԃE�C���X
	game_anime_data_virus_y,	//	���E�C���X
	game_anime_data_virus_b,	//	�E�C���X
};

//	4P�p��O���t�B�b�N�f�[�^
u8	*face_4p_game_data[] = {
	face_4p_game_mario_bm0_0,
	face_4p_game_noko_bm0_0,
	face_4p_game_bomhei_bm0_0,
	face_4p_game_puku_bm0_0,
	face_4p_game_chor_bm0_0,
	face_4p_game_heiho_bm0_0,
	face_4p_game_hana_bm0_0,
	face_4p_game_tere_bm0_0,
	face_4p_game_pack_bm0_0,
	face_4p_game_kame_bm0_0,
	face_4p_game_kupp_bm0_0,
	face_4p_game_peach_bm0_0,
	face_4p_game_kinopio_bm0_0,
	face_4p_game_bomhei_2_bm0_0,
};

//	4P�p��p���b�g�f�[�^
u16	*face_4p_game_pal_data[] = {
	face_4p_game_mario_bm0_0tlut,
	face_4p_game_noko_bm0_0tlut,
	face_4p_game_bomhei_bm0_0tlut,
	face_4p_game_puku_bm0_0tlut,
	face_4p_game_chor_bm0_0tlut,
	face_4p_game_heiho_bm0_0tlut,
	face_4p_game_hana_bm0_0tlut,
	face_4p_game_tere_bm0_0tlut,
	face_4p_game_pack_bm0_0tlut,
	face_4p_game_kame_bm0_0tlut,
	face_4p_game_kupp_bm0_0tlut,
	face_4p_game_peach_bm0_0tlut,
	face_4p_game_kinopio_bm0_0tlut,
	face_4p_game_bomhei_2_bm0_0tlut,
};

//	�w�i�^�C���O���t�B�b�N�f�[�^
u8	*dm_bg_tile_data[] = {
	tile_01_bm0_0,
	tile_02_bm0_0,
	tile_03_bm0_0,
	tile_04_bm0_0,
	tile_05_bm0_0,
	tile_01_bm0_0,
	tile_07_bm0_0,
	tile_08_bm0_0,
	tile_08_bm0_0,
	tile_10_bm0_0,
};

//	�w�i�^�C���p���b�g�f�[�^
u16	*dm_bg_tile_pal_data[] = {
	tile_01_bm0_0tlut,
	tile_02_bm0_0tlut,
	tile_03_bm0_0tlut,
	tile_04_bm0_0tlut,
	tile_05_bm0_0tlut,
	tile_01_bm0_0tlut,
	tile_07_bm0_0tlut,
	tile_08_bm0_0tlut,
	tile_08_bm0_0tlut,
	tile_10_bm0_0tlut,
};


#ifdef	DM_S2D_MAIN
/*--------------------------------------
	S2DEX �p�X�v���C�g�ݒ�֐�
--------------------------------------*/
void	dm_set_sprite(uObjSprite *sp,s16 x_p,s16 y_p,s16 size_w,s16 size_h,u8 im_fmt,u8 im_siz,s16 wide,s16 t_x,s16 t_y,s8 pal_no )
{
	sp->s.objX = x_p << 2;
	sp->s.objY = y_p << 2;
	sp->s.imageW = size_w << 5;
	sp->s.imageH = size_h << 5;
	sp->s.scaleW = sp->s.scaleH = 1<<10;
	sp->s.paddingX = sp->s.paddingY = sp->s.imageFlags = 0;dm_4p_game_target_3p_bm0_0tlut
	sp->s.imageStride = GS_PIX2TMEM(wide,im_siz);
	sp->s.imageAdrs = GS_PIX2TMEM( ( t_y * wide ) + t_x,im_siz);
	sp->s.imageFmt = im_fmt;
	sp->s.imageSiz = im_siz;
	sp->s.imagePal = pal_no;
}
#endif
/*--------------------------------------
	���_�֐�
--------------------------------------*/
void	dm_score_make( game_state *state,u8 type )
{
	u8	i,sc_type;

	if( type ){
		sc_type = state -> player_state[1];
	}else{
		sc_type = state -> cap_def_speed;
	}

	for( i = 0;i < state -> erase_virus_count;i++ ){
		if( i < 6 ){
			state -> game_score += Score1p[sc_type][i] * 100;
		}else{
			state -> game_score += Score1p[sc_type][5] * 100;
		}
	}

	//	�J���X�g
	if( state -> game_score > 9999900 ){
		state -> game_score = 9999900;
	}
}

/*--------------------------------------
	LEVEL�p���_�ݒ�֐�
--------------------------------------*/
void	dm_level_score_make( game_state *state )
{
	s8	i,j,p;

	for( i = 0;i < 10;i++ ){
		if( !lv_sc_pos[i].flg ){	//	���g�p�̏ꍇ
			lv_sc_pos[i].mode = 1;
			lv_sc_pos[i].flg = 1;
			lv_sc_pos[i].score = 0;
			//	�����ړ��l�̃Z�b�g
			for(j = 0;j < 5;j++){
				lv_sc_pos[i].score_pos[j][1] = 12;
			}
			//	���Z���链�_�̌v�Z
			for(j = 0;j < state -> erase_virus_count;j++ ){
				if( j > 5 ){
					p = 5;
				}else{
					p = j;
				}
				lv_sc_pos[i].score += Score1p[state -> cap_def_speed][p] * 100;
			}
			//	���_�ʂ̐ݒ�
			if( lv_sc_pos[i].score < 1000 ){
				lv_sc_pos[i].num_max = 3;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 100;
				lv_sc_pos[i].num_g[1] = lv_sc_pos[i].num_g[2] = 0;
				for(j = 0;j < 3;j++){
					lv_sc_pos[i].score_pos[j][0] = 70 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}

			}else 	if( lv_sc_pos[i].score < 10000 ){
				lv_sc_pos[i].num_max = 4;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 1000;
				lv_sc_pos[i].num_g[1] = ( lv_sc_pos[i].score % 1000 ) / 100;
				lv_sc_pos[i].num_g[2] = lv_sc_pos[i].num_g[3] = 0;
				for(j = 0;j < 4;j++){
					lv_sc_pos[i].score_pos[j][0] = 62 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}
			}else{
				lv_sc_pos[i].num_max = 5;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 10000;
				lv_sc_pos[i].num_g[1] = ( lv_sc_pos[i].score % 10000 ) / 1000;
				lv_sc_pos[i].num_g[2] = ( lv_sc_pos[i].score % 1000 ) / 100;
				lv_sc_pos[i].num_g[3] = lv_sc_pos[i].num_g[4] = 0;
				for(j = 0;j < 5;j++){
					lv_sc_pos[i].score_pos[j][0] = 54 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}
			}

			break;
		}
	}
}

/*--------------------------------------
	LEVEL�p���_�֐�
--------------------------------------*/
void	dm_level_score_main( game_state *state )
{
	s8	i,j;

	for( i = 0;i < 10;i++ ){
		if( lv_sc_pos[i].flg ){	//	�g�p���̏ꍇ
			switch( lv_sc_pos[i].mode )
			{
			case	1:	//	�A���t�@�A�b�v
				for( j = 0;j < lv_sc_pos[i].num_max;j++ ){
					if( !lv_sc_pos[i].num_alpha_flg[j] ){
						lv_sc_pos[i].num_alpha[j] += 0x30;
						if( lv_sc_pos[i].num_alpha[j] >= 0xff ){
							lv_sc_pos[i].num_alpha[j] = 0xff;
							lv_sc_pos[i].num_alpha_flg[j] = 1;	//	���̌��̓��������I��
						}
						break;
					}
				}
				if( lv_sc_pos[i].num_alpha_flg[lv_sc_pos[i].num_max - 1] ){
					//	���̏����Ɉڂ�
					lv_sc_pos[i].mode = 2;
				}
				break;
			case	2:	//	������������
				for( j = lv_sc_pos[i].num_max - 1;j >= 0;j-- ){
					if( lv_sc_pos[i].score_pos[j][1] ){
						if( j == lv_sc_pos[i].num_max - 1 ){

							lv_sc_pos[i].num_alpha[j] -= 0x20;
							lv_sc_pos[i].score_pos[j][1] -= 2;
							if( lv_sc_pos[i].score_pos[j][1] <= 0 ){
								lv_sc_pos[i].score_pos[j][1] = 0;
							}
						}else{
							if( lv_sc_pos[i].score_pos[j + 1][1] <= 4 ){
								lv_sc_pos[i].num_alpha[j] -= 0x20;
								lv_sc_pos[i].score_pos[j][1] -= 2;
								if( lv_sc_pos[i].score_pos[j][1] <= 0 ){
									lv_sc_pos[i].score_pos[j][1] = 0;
								}
							}
						}
					}
				}

				//	�S�Ă̐������������I������瓾�_�̉��Z
				if(	lv_sc_pos[i].score_pos[0][1] == 0 )
				{
					lv_sc_pos[i].mode = 0;
					state -> game_score += lv_sc_pos[i].score;	//	���_���Z
					//	�J���X�g
					if( state -> game_score > 9999900 ){
						state -> game_score = 9999900;
					}
					if( evs_high_score < state -> game_score ){
						evs_high_score = state -> game_score;	//	�n�C�X�R�A�̍X�V
					}
					lv_sc_pos[i].flg = 0;						//	�g�p���t���O��OFF
				}
				break;
			}
		}
	}
}
/*--------------------------------------
	SE�ݒ�֐�
--------------------------------------*/
void	dm_set_se( u16 senum ){
	if ( evs_gamespeed < 6 ){	//	���������p����
		dm_set_menu_se(senum);
	}
}

/*--------------------------------------
	SE�Đ��֐�
--------------------------------------*/
void	dm_play_se( void )
{
	dm_menu_se_play();
}
/*--------------------------------------
	BGM �ݒ�֐�
--------------------------------------*/
void	dm_play_bgm_set( s8 bgm_no )
{
	evs_seSetPtr = evs_seGetPtr = 0;		//	SE �Đ����~
	evs_seqnumb = bgm_no;					//	�Đ�BGM�ݒ�
	auSeqPlayerStop(0);						//	BGM ��~
}

/*--------------------------------------
	�U���������֐�
--------------------------------------*/
void	dm_attack_se( game_state *state ,u8 player_no )
{
	u8	i,flg;

	if( state -> chain_line > 1 ){	//	�A�������Q�ȏ�
		switch( evs_gamesel ){
		case	GSL_1PLAY:
			dm_set_se( dm_se_attack_1p );
			break;
		case	GSL_2PLAY:
		case	GSL_VSCPU:
			dm_set_se( dm_chaine_se_table_vs[player_no] );
			break;
		case	GSL_4PLAY:
			for(i = 0;i < 3;i++){
				if( state -> chain_color[3] & (1 << i) ){
					dm_set_se( dm_chaine_se_table_4p[player_no][i] );
				}
			}
			break;
		}
	}
}
/*--------------------------------------
	�ςݏグ�x���������֐�
--------------------------------------*/
void	dm_warning_h_line( game_state *state,game_map *map )
{
	u8	i,j,flg = 0;

	for( i = 1;i < 4;i++ ){	//	��̂R�i�������ׂȂ�
		for( j = 0;j < 8;j++ ){
			if( get_map_info( map,j,i) == cap_flg_on ){	//	��̂R�i�ɉ�������������
				flg = 1;
				break;
			}
		}
		if( flg ){
			break;
		}
	}

	if( flg ){
		if( ( state -> warning_flg & 0x80 ) == 0 ){
			//	�炵�Ă��Ȃ����ēx�ς݂�������
			state -> warning_flg |= 0x80;	//	�r�b�g�𗧂Ă�( �A����h�~ )
			dm_set_se( dm_se_warning );	//	�x�� SE �Đ�
		}
	}else{
		state -> warning_flg &= 0x7f;	//	�r�b�g������
	}
}

/*--------------------------------------
	�������ݒ�֐�
--------------------------------------*/
s8	set_down_flg( game_map *map )
{
	u8	k,l,flg[2];
	s8	ret;
	s16	i,j;

	for(i = 0;i < 120;i++){	//	�����t���O�̐ݒ�
		(map + i) -> capsel_m_flg[cap_down_flg] = cap_flg_on;	//	�����t���O�̐ݒ�
	}

	for( j = 14;j >= 0;j-- )	//	�ŉ��i�̂P��̒i����n�߂�
	{
		for( i = 0;i < 8;i++ )
		{
			k = (j << 3) + i;	//	�z��ԍ��̌v�Z
			if( (map + k) -> capsel_m_flg[cap_disp_flg] )
			{
				//	�\������Ă���
				if( (map + k) -> capsel_m_flg[cap_virus_flg] < 0)
				{
					//	�E�C���X�łȂ�
					if( (map + k) -> capsel_m_g > capsel_d && (map + k) -> capsel_m_g < capsel_b)
					{
						//	�������J�v�Z���̏ꍇ
						if(  (map + k) -> capsel_m_g == capsel_l)
						{
							//	�ŉ��i�ł͂Ȃ�
							for(l = 0;l < 2;l++){
								flg[l] = cap_flg_on;
								if( get_map_info(map,(map + k+ l) -> pos_m_x,(map + k+ l) -> pos_m_y + 1) == cap_flg_on)
								{
									//	��Q�����������ꍇ
									if( (map + k + l + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
										//	���̏�Q���͗����o���Ȃ�
										flg[l] = cap_flg_off;
									}
								}
							}

							if( !flg[0] || !flg[1] )
							{
								//	�����Ƃ����ɗ����邱�Ƃ��o���Ȃ�
								for(l = 0;l < 2;l++)
								{
									(map + k + l) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�𗧏���
								}
							}
						}else	if( (map + k) -> capsel_m_g == capsel_r ){
							for(l = 0;l < 2;l++){
								flg[l] = cap_flg_on;
								if( get_map_info(map,(map + k - l) -> pos_m_x,(map + k - l) -> pos_m_y + 1) == cap_flg_on)
								{
									//	��Q�����������ꍇ
									if( (map + k - l + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
										//	���̏�Q���͗����o���Ȃ�
										flg[l] = cap_flg_off;
									}
								}
							}
							if( !flg[0] || !flg[1] )
							{
								//	�����Ƃ����ɗ����邱�Ƃ��o���Ȃ�
								for(l = 0;l < 2;l++)
								{
									(map + k - l) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	�����t���O������
								}
							}
						}
					}else{
						//	�{�[���̏ꍇ
						if( get_map_info(map,(map + k) -> pos_m_x, (map + k) -> pos_m_y + 1) == cap_flg_on)
						{
							//	���̂̉��ɉ���������
							if( (map + k + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
								(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̐ݒ�
							}
						}
					}
				}else{
					(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̐ݒ�
				}
			}else{
				(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̐ݒ�
			}
		}
	}

	//	�����t���O�̏󋵂�Ԃ�
	for(i = ret = 0;i < 128;i++){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
			//	�\���t���O�������Ă���
			if( (map + i) -> capsel_m_flg[cap_down_flg] ){
				//	�����t���O�������Ă���
				ret++;
			}
		}
	}

	return	ret;
}
/*--------------------------------------
	�������֐�
--------------------------------------*/
void	go_down( game_state *state,game_map *map ,u8 cout)
{

	u8	k,se_flg = 0;
	s8	black_color[] = {0,3};
	s16	i,j,p;

	state -> erase_anime_count ++;
	if( state -> erase_anime_count >= cout ){	//	�E�F�C�g���Ԍo��
		state -> erase_anime_count = 0;

		for( j = 14;j >= 0;j-- ){	//	�ŉ��i�̂P��̒i����n�߂�
			for( i = 0;i < 8;i++ ){
				k = (j << 3) + i;	//	�z��ԍ��̌v�Z
				if( (map + k) -> capsel_m_flg[cap_down_flg] ){
					//	�����t���O�������Ă���

					//	������ɏ����R�s�[
					set_map( map ,(map + k) -> pos_m_x,(map + k) -> pos_m_y + 1,(map + k) -> capsel_m_g,(map + k) -> capsel_m_p );
					//	�R�s�[�����̍폜
					clear_map(map,(map + k) -> pos_m_x, (map + k) -> pos_m_y);

					se_flg = 1;	//	��������炷
				}
			}
		}

		if( se_flg ){
			dm_set_se( dm_se_cap_a_down );	//	�����r�d�Đ�
		}

		if( state -> retire_flg[dm_game_over_flg] ){
			//	���^�C�A���
			if( state -> game_condition[2] == dm_cnd_training ){
				//	���K��
				for(i = 0;i < 128;i++){
					if( (map + i) -> capsel_m_flg[cap_disp_flg] )
					{
						//	�\���t���O�������Ă���
						if( !(map + i) -> capsel_m_flg[cap_down_flg] )
						{
							//	�����t���O�������Ă��Ȃ�
							if( (map + i) -> capsel_m_p < 3 ){
								//	�Â��Ȃ��ꍇ
								(map + i) -> capsel_m_p += 3;
							}
						}
					}
				}

			}
		}

		j = set_down_flg( map );	//	�����t���O�̍Đݒ�

		if( j == 0 ){	//	�����I��
			if( dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off ){
				if( !state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă��Ȃ�
					state -> game_mode[dm_mode_now] = dm_mode_erase_chack;	//	���Ŕ���
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă���(���K��)
					state -> game_mode[dm_mode_now] = dm_mode_tr_erase_chack;	//	���Ŕ���
				}
				state -> cap_speed_count = 0;
			}else{
				if( !state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă��Ȃ�
					state -> game_mode[dm_mode_now] = dm_mode_cap_set;	//	�J�v�Z���Z�b�g
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă���(���K��)
					state -> game_mode[dm_mode_now] = dm_mode_tr_cap_set;	//	�J�v�Z���Z�b�g
				}
			}
		}
	}
}

/*--------------------------------------
	�}�b�v����ŃA�j���[�V�����֐�
--------------------------------------*/
void	erase_anime( game_map *map )
{
	u8	i;

	for(i = 0;i < 128;i ++){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){	//	�\���t���O�������Ă���
			if( (map + i) -> capsel_m_flg[cap_condition_flg] ){	//	���ŃA�j���[�V������Ԃ�����
				(map + i) -> capsel_m_g ++;	//	���ŃA�j���[�V�����̐i�s
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 ){	//	�E�C���X�̏ꍇ
					if( (map + i) -> capsel_m_g > erase_virus_b ){
						clear_map( map,(map + i) -> pos_m_x,(map + i) -> pos_m_y );	//	�}�b�v���̍폜
					}
				}else{	//	�E�C���X�ȊO�̏ꍇ
					if( (map + i) -> capsel_m_g > erase_cap_b ){
						clear_map( map,(map + i) -> pos_m_x,(map + i) -> pos_m_y );	//	�}�b�v���̍폜
					}
				}
			}
		}
	}
}

/*--------------------------------------
	�J�v�Z�����E�ړ��֐�
--------------------------------------*/
void	translate_capsel( game_map *map,game_state *state,s8 move_vec,u8 joy_no)
{
	s8	vec = 0;
	game_cap *cap;

	cap = &state -> now_cap;

	if( cap->pos_y[0] > 0 ){
		if( cap -> capsel_flg[cap_disp_flg] ){	//	�\���t���O
			if( move_vec == cap_turn_r){
				//	�E�ړ��̏ꍇ
				if(cap -> pos_x[0] == cap -> pos_x[1]){
					//	�J�v�Z�����c���т̏ꍇ
					if( cap->pos_x[0] < 7 ){
						if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[0] ) != cap_flg_on ){
							if( cap -> pos_y[1] == 0 ){
								//	�ړ���ɏ�Q���������ꍇ
								vec = 1;
							}else{
								if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[1] ) != cap_flg_on ){
									//	�ړ���ɏ�Q���������ꍇ
									vec = 1;
								}
							}
						}
					}
				}else{
					//	�J�v�Z���������т̏ꍇ
					if( cap->pos_x[1] < 7 && (get_map_info( map,cap -> pos_x[1] + 1,cap -> pos_y[0] ) != cap_flg_on)){
						//	�ړ���ɏ�Q���������ꍇ
						vec = 1;
					}
				}
			}else	if( move_vec == cap_turn_l ){
				//	���ړ��̏ꍇ
				if(cap -> pos_x[0] == cap -> pos_x[1]){
					//	�J�v�Z�����c���т̏ꍇ
					if( cap->pos_x[0] > 0 ){
						if( get_map_info( map,cap -> pos_x[0] - 1,cap -> pos_y[0] ) != cap_flg_on ){
							if( cap -> pos_y[1] == 0 ){
								//	�ړ���ɏ�Q���������ꍇ
								vec = -1;
							}else{
								if( get_map_info( map,cap -> pos_x[0] - 1,cap -> pos_y[1] ) != cap_flg_on ){
									//	�ړ���ɏ�Q���������ꍇ
									vec = -1;
								}
							}
						}
					}
				}else{
					if( cap -> pos_x[0] > 0){
						if( get_map_info( map,cap -> pos_x[0] - 1,cap->pos_y[0] ) != cap_flg_on ){
							//	�ړ������ɏ�Q���������ꍇ
							vec = -1;
						}
					}
				}
			}

			if( vec != 0 ){
				dm_set_se( dm_se_cap_trance );	//	�ړ�SE�Đ�
				state -> cap_move_se_flg = 0;
				cap -> pos_x[0] += vec;
				cap -> pos_x[1] += vec;
			}else{
				if( state -> cap_move_se_flg == 0 ){
					//	�ړ��ł��Ȃ��Ă����͖炷
					state -> cap_move_se_flg = 1;
					dm_set_se( dm_se_cap_trance );	//	�ړ�SE�Đ�
				}
				if( move_vec == cap_turn_r){
					joyCursorFastSet( LEVER_RIGHT,joy_no );
				}else	if( move_vec == cap_turn_l){
					joyCursorFastSet( LEVER_LEFT,joy_no );
				}
			}
		}
	}
}
/*--------------------------------------
	�J�v�Z�����E��]�֐�
--------------------------------------*/
void	rotate_capsel( game_map *map,game_cap *cap,s8 move_vec )
{
	s8	vec = 0;
	u8	i,save;
	u8	rotate_table[] = {1,3,4,2};
	u8	rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};

	if( cap -> pos_y[0] > 0 )	//	�m�d�w�s�̈ʒu�ł͉�]�����Ȃ�
	{
		if( cap -> capsel_flg[cap_disp_flg] ){	//	�\���t���O
			if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	�J�v�Z�����c�ɂȂ��Ă����ꍇ
				if( cap -> pos_x[0] == 7 || ( get_map_info( map,cap->pos_x[0] + 1,cap->pos_y[0] ) == cap_flg_on) ){	//	�E�ǂɒ�����Ă��������ɏ�Q�����������ꍇ
					if( cap -> pos_x[0] != 0 && (get_map_info( map,cap->pos_x[0] - 1,cap->pos_y[0] ) != cap_flg_on) ){	//	���ǂɒ�����Ă��Ȃ��ł��ꂽ�Ƃ���ɏ�Q�������������ꍇ
						cap -> pos_x[0] --;
						vec = 1;
					}
				}else{
					cap -> pos_x[1] ++;
					vec = 1;
				}
				if( vec != 0){
					cap -> pos_y[1] ++;
					if( move_vec == cap_turn_l ){
						//	�p���b�g�̓���ւ�
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			}else{	//	�J�v�Z�������ɂȂ��Ă����ꍇ
				if( cap -> pos_y[0] == 1 ){	//	�ŏ�i�������ꍇ
					cap -> pos_x[1] --;
					vec = -1;
				}else{
					if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] - 1) == cap_flg_on ){	//	��]���̏�ɉ����������ꍇ
						if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[0] - 1) != cap_flg_on ){	//	�ړ���ɉ������������ꍇ
							cap -> pos_x[0] ++;
							vec = -1;
						}
					}else{	//	��]���̏�ɉ������������ꍇ
						//	���W�ύX
						cap -> pos_x[1] --;
						vec = -1;
					}
				}
				if( vec != 0 ){
					cap -> pos_y[1] --;
					//	�p���b�g�̓���ւ�
					if( move_vec == cap_turn_r ){
						save = cap->capsel_p[0];
						cap -> capsel_p[0] = cap->capsel_p[1];
						cap -> capsel_p[1] = save;
					}
				}
			}
			if( vec != 0 ){
				dm_set_se( dm_se_cap_rotate );	//	��]SE�Đ�
				//	�O���t�B�b�N�̓���ւ�
				for(i = 0;i < 2;i++){
					save = rotate_table[cap -> capsel_g[i]];
					save += vec;
					cap -> capsel_g[i] = rotate_mtx[save];
				}
			}
		}
	}
}

/*--------------------------------------
	�J�v�Z���}�K�W���쐬�֐�
--------------------------------------*/
void	dm_make_magazine( void )
{
	int	i;

	i = OS_CYCLES_TO_USEC(osGetTime()) + dm_rand_seed + RAND(5000);
	sgenrand(i);		// �����
	dm_rand_seed ++;
	if( dm_rand_seed >= 0xffff )
		dm_rand_seed = 0;

	for( i = 0;i < MAGA_MAX;i++ ){
		 CapsMagazine[i] = genrand( 0xffff )% MAGA_MAX;
	}
}
/*--------------------------------------
	�J�v�Z���������ݒ�֐�(�����t���O�𗧂ĂȂ�)
--------------------------------------*/
void	dm_init_capsel( game_cap *cap,u8 left_cap_col,u8 right_cap_col )
{
	cap->pos_x[0] = 3;	//	���J�v�Z���w���W
	cap->pos_x[1] = 4;	//	�E�J�v�Z���w���W
	cap->pos_y[0] = cap->pos_y[1] = 0;	//	�J�v�Z���x���W
	cap->capsel_g[0] = capsel_l;		//	�O���t�B�b�N�ԍ�
	cap->capsel_g[1] = capsel_r;		//	�O���t�B�b�N�ԍ�
	cap->capsel_p[0] = left_cap_col;	//	�F�i���j
	cap->capsel_p[1] = right_cap_col;	//	�F�i�E�j
	cap->capsel_flg[cap_disp_flg] = cap_flg_on;	//	�\���t���O���n�m��
	cap->capsel_flg[1] = cap->capsel_flg[2] = cap_flg_off;	//	���̂Ƃ��떢�g�p
}
/*--------------------------------------
	�J�v�Z���������ݒ�֐�(�����t���O�𗧂Ă�)
--------------------------------------*/
void	dm_init_capsel_go( game_cap *cap,s8 left_cap_col,s8 right_cap_col )
{
	dm_init_capsel(cap,left_cap_col,right_cap_col);
	cap -> capsel_flg[cap_down_flg] = 1;
}

/*--------------------------------------
	�J�v�Z���ݒ�֐�
--------------------------------------*/
void	dm_set_capsel( game_state *state )
{
	state -> cap_move_se_flg = 0;
	dm_init_capsel_go( &state -> now_cap,(CapsMagazine[state -> cap_magazine_cnt] >> 4) % 3,CapsMagazine[state ->cap_magazine_cnt] % 3);
	state -> cap_magazine_save = state -> cap_magazine_cnt;	//	�O�̃}�K�W���ʒu�̕ۑ�
	state -> cap_magazine_cnt ++;
	if( state -> cap_magazine_cnt >= 0xfe )
		state -> cap_magazine_cnt = 1;

	dm_init_capsel( &state -> next_cap,(CapsMagazine[state ->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state ->cap_magazine_cnt] % 3);
}
/*--------------------------------------
	�J�v�Z�����x�㏸�ݒ�֐�
--------------------------------------*/
void	dm_capsel_speed_up( game_state *state )
{
	state -> cap_count ++;
	if( state -> cap_count >= 10 ){	//	�X�s�[�h�A�b�v
		dm_set_se( dm_se_cap_speed_up );					//	�X�s�[�h�A�b�v�r�d�Đ�
		state -> cap_count = 0;
		state -> cap_speed ++;
		if( state -> cap_speed > GameSpeed[3] ){	//	���x���E
			state -> cap_speed = GameSpeed[3];
		}
	}
}

/*--------------------------------------
	����J�v�Z�����������֐�
--------------------------------------*/
void	dm_capsel_down( game_state *state,game_map *map )
{
	u8	i,j;
	s8	black_color[] = {0,3};
	game_cap *cap;

	cap = &state -> now_cap;


	if( cap -> pos_y[0] > 0 ){	//	�������̑��x�ݒ�
		i  = FallSpeed[state -> cap_speed];	//	�J�v�Z���������x�̐ݒ�
		if( cap -> pos_y[0] >= 1 && cap -> pos_y[0] <= 3 ){
			i += BonusWait[cap -> pos_y[0] - 1][state -> cap_def_speed];	// ��߾ق̍����ɂ���ްŽ���Ă����Z
		}
		j = 0;
		if( cap -> pos_y[0] > 0 ){
			if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] + 1) ){
				//	�J�v�Z���̉��ɃA�C�e�����������ꍇ
//				j = TouchDownWait[evs_playcnt-1];
				j = dm_TouchDownWait;
			}
		}
		state -> cap_speed_max = i + j;
	}else{	//	�����O�̑��x�ݒ�(�ꗥ30)
		state -> cap_speed_max = 30;
	}

	state -> cap_speed_count += state -> cap_speed_vec;	//	�����J�E���^�̑���
	if( state -> cap_speed_count >= state -> cap_speed_max ){	//	��莞�ԉ߂����H
		state -> cap_speed_count = 0;			//	�J�E���^�N���A

		if( cap -> capsel_flg[cap_disp_flg] ){
			//	�P�i����
			if( cap -> pos_y[0] > 0 ){	//	��i�ڂ͋����I�ɗ��Ƃ�

				//	�����J�v�Z���ƃ}�b�v���̔���
				if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	�����J�v�Z���͏c���H
					//	�J�v�Z�����c�̏ꍇ
					if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] + 1) ){
						//	�J�v�Z���̉��ɃA�C�e�����������ꍇ

						cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̃N���A
					}
				}else{
					//	�J�v�Z�������̏ꍇ
					for( j = 0;j < 2;j++ ){
						if( get_map_info( map,cap -> pos_x[j],cap -> pos_y[j] + 1) ){
							//	�J�v�Z���̉��ɃA�C�e�����������ꍇ
							cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̃N���A
							break;
						}
					}
				}
			}

			//	�ŉ��i����
			for( i = 0;i < 2;i++ ){
				if( cap -> pos_y[i] == 16 ){
					cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	�����t���O�̃N���A
					break;
				}
			}

			//	��������
			if( cap -> capsel_flg[cap_down_flg] ){	//	�����t���O�������Ă��邩�H
				//	��������
				for( i = 0;i < 2;i++ ){
					if( cap -> pos_y[i] < 16 ){	//	�O�̂���
						cap -> pos_y[i] ++;
					}
				}
				for( i = 0;i < 2;i++ ){
					if( get_map_info( map,cap -> pos_x[i],cap -> pos_y[i]) ){
						//	���������ꏊ�ɃA�C�e��������
						state -> game_condition[dm_static_cnd] = dm_cnd_game_over;	//	�Q�[���I�[�o�[�ݒ�
//						state -> game_condition[dm_mode_now] = dm_cnd_game_over;	//	�Q�[���I�[�o�[�ݒ�
						state -> next_cap.capsel_flg[cap_disp_flg] = cap_flg_off;	//	�l�N�X�g�J�v�Z���̕\�����~
						cap -> capsel_flg[cap_down_flg] = cap_flg_off;				//	�����t���O�̃N���A
						break;
					}
				}
			}
			if( !cap -> capsel_flg[cap_down_flg] ){
				//	�����ł��Ȃ�
				dm_set_se( dm_se_cap_down );					//	���n�r�d�Đ�
				state -> game_mode[dm_mode_now] = dm_mode_down_wait;	//	�o�E���h
				cap -> capsel_flg[cap_disp_flg] = cap_flg_off;	//	�\���t���O�̃N���A
				for( i = 0;i < 2;i++ ){
					if( cap -> pos_y[i] != 0 ){
						//	�}�b�v�f�[�^�ɕύX
						set_map( map,cap -> pos_x[i],cap -> pos_y[i],
									cap -> capsel_g[i],cap -> capsel_p[i] + black_color[state -> retire_flg[dm_game_over_flg]]);
					}
				}
			}
		}
	}
}
/*--------------------------------------
	�Q�[���I�[�o�[(�ς݂�����)����֐�
--------------------------------------*/
s8	dm_check_game_over( game_state *state,game_map *map )
{
	if( state -> game_condition[dm_static_cnd] == dm_cnd_game_over ){
		return	cap_flg_on;		//	�Q�[���I�[�o�[
	}else{
		return	cap_flg_off;	//	�Q�[���I�[�o�[����Ȃ�
	}
}

/*--------------------------------------
	�E�C���X�]���֐�
--------------------------------------*/
void	dm_set_virus( game_state *state,game_map *map,virus_map *v_map,u8 *order )
{
	u16	x_pos,y_pos;
	s8	virus_max;
	u8	p,s;

	if( state -> game_condition[dm_mode_now] == dm_cnd_init )
	{
		//	�E�C���X���̊���o��
		virus_max = state -> virus_level;
		//	����I�[�o�[��h��
		if ( virus_max > LEVLIMIT ){
			virus_max = LEVLIMIT;
		}
		//	�E�C���X���̊���o��
		virus_max = (virus_max + 1) << 2;

		while(1)
		{
			s = genrand(virus_max);
			if( *(order + s ) != 0xff ){
				p = *(order + s );
				 *(order + s ) = 0xff;
				break;
			}
		}
/*		while(1)
		{
			x_pos = genrand(8);
			y_pos = genrand(16) + 1;

			if( dm_virus_check(v_map,x_pos,y_pos) == TRUE )
				break;
		}
		//	�E�C���X�Z�b�g
		p = ((y_pos - 1) << 3) + x_pos;
		set_virus(map,x_pos,y_pos,(v_map + p) -> virus_type,virus_anime_table[(v_map + p) -> virus_type][state -> virus_anime] );
*/
		set_virus(map,(v_map + p) -> x_pos,(v_map + p) -> y_pos,(v_map + p) -> virus_type,virus_anime_table[(v_map + p) -> virus_type][state -> virus_anime] );
		//	�E�C���X���̍폜
//		(v_map + p) -> virus_type = -1;

		state -> virus_number ++;
		if( state -> virus_number >= virus_max )
		{
			//	�E�C���X�o���I��
			state -> game_condition[dm_mode_now] = dm_cnd_wait;	//	�ҋ@��Ԃ�
			state -> game_mode[dm_mode_now]		= dm_mode_wait;	//	�ҋ@��Ԃ�
		}
	}
}
/*--------------------------------------
	�E�C���X�A�j���֐�
--------------------------------------*/
void	dm_virus_anime( game_state *state,game_map *map )
{
	u8	i;

	state -> virus_anime_count ++;	// �J�E���g�̑���
	if( state -> virus_anime_count > state -> virus_anime_spead ){ // �A�j���[�V�����Ԋu�������
		state -> virus_anime_count = 0;	//	�J�E���g�̃N���A
		state -> virus_anime += state -> virus_anime_vec;		//	�A�j���[�V�����̐i�s
		if( state -> virus_anime > state -> virus_anime_max ){	//	�A�j���[�V�����̍ő�R�}�����z����
			state -> virus_anime_vec = -1;						//	�A�j���[�V�����i�s�����̔��]
		}else	if( state -> virus_anime < 1 ){					//	�A�j���[�V�����̍ŏ��R�}�����z����
			state -> virus_anime_vec = 1;						//	�A�j���[�V�����i�s�����̔��]
		}

		//	�}�b�v��̃E�C���X���A�j���[�V����������
		for( i = 0;i < 128;i++ ){
			if( (map + i) -> capsel_m_flg[cap_disp_flg] ){	//	�\������Ă��邩�H
				//	�\������Ă���
				if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){	//	���ŏ�Ԃ��H
					//	�ʏ��Ԃ�����
					if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 ){	//	�E�C���X���H
						//	�E�C���X������

						//	�O���t�B�b�N�ύX
						(map + i) -> capsel_m_g = virus_anime_table[ (map + i) -> capsel_m_flg[cap_virus_flg] ][ state -> virus_anime ];
					}
				}
			}
		}
	}
}
/*--------------------------------------
	�E�C���X(�J�v�Z��)���Ŋ֐�
--------------------------------------*/
void	dm_capsel_erase_anime( game_state *state,game_map *map )
{
	u8	i;

	state -> erase_anime_count ++;
	if( state -> erase_anime_count == dm_erace_speed_1 ){			//	���i�K
		erase_anime( map );	//	���ŃA�j���[�V�����i�s
		state -> erase_anime ++;
	}else	if( state -> erase_anime_count >= dm_erace_speed_2 ){	//	���i�K
		erase_anime( map );	//	���ŃA�j���[�V�����i�s
		set_down_flg( map );		//	�����ݒ�
		state -> erase_anime = 0;
		state -> game_mode[dm_mode_now] = dm_mode_ball_down;
		state -> erase_anime_count = dm_down_speed;	//	���Œ���͂����ɗ�����

	}

}

/*--------------------------------------
	�c�����������֐�
--------------------------------------*/
void	dm_make_erase_h_line( game_state *state,game_map *map,u8 start,u8 chain,u8 pos_x )
{
	u8	i,p;

	for( i = start;i < start + chain + 1;i++ ){
		p = (i << 3) + pos_x;
		(map + p) -> capsel_m_flg[cap_condition_flg] = cap_flg_on;	//	���ŏ�Ԃɂ���
		if( (map + p) -> capsel_m_flg[cap_virus_flg] < 0){
			//	�J�v�Z���̏ꍇ
			(map + p) -> capsel_m_g = erase_cap_a;
		}else{
			//	�E�C���X�̏ꍇ
			(map + p) -> capsel_m_g = erase_virus_a;
			state -> erase_virus_count ++;	//	���ŃE�C���X���̑���
			state -> chain_color[3] |= 0x08;	//	�E�C���X���܂ޘA��
			state -> chain_color[3] |= 0x10 << (map + p) -> capsel_m_flg[cap_col_flg] ;
		}
	}
}
/*--------------------------------------
	�c�������菈���֐�
--------------------------------------*/
s8	dm_h_erase_chack( game_map *map )
{
	s16	i,j;
	u8	p;
	s8	chain,chain_start,chain_col;

	for(i = 0;i < 8;i++){
		chain = 0;
		chain_col = chain_start = -1;
		for(j = 0;j < 16;j++){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){	//	�����\������Ă���
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					//	�����F�łȂ�����
					if( chain >= 3 ){
						//	�S�ȏゾ����
						return	cap_flg_on;
					}
					if(j > 12){	//	���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
						break;
					}else{
						chain_start = j;									//	�A���J�n�ʒu�̕ύX
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	�F�̓���ւ�
						chain = 0;
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col ){
					chain++;	//	�A���ǉ�
					if(j == 15){	//	�ŉ��i�ɒB�����ꍇ
						if( chain >= 3 ){
							//	�S�ȏゾ����
							return	cap_flg_on;
						}
					}
				}
			}else{
				if( chain >= 3 ){
					//	�S�ȏゾ����
					return	cap_flg_on;
				}
				if(j > 12){	//	���݈ʒu�������R�i�ȓ��̏ꍇ���~
					break;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return	cap_flg_off;
}
/*--------------------------------------
	�c��������Ə��Őݒ�֐�
--------------------------------------*/
void	dm_h_erase_chack_set( game_state *state,game_map *map )
{
	s16	i,j;
	u8	p,end;
	s8	chain,chain_start,chain_col;

	for(i = 0;i < 8;i++){
		chain = 0;
		chain_col = chain_start = -1;
		for(j = end = 0;j < 16;j++){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){	//	�����\������Ă���

				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					//	�����F�łȂ�����
					if( chain >= 3 ){
						//	�S�ȏゾ����
						dm_make_erase_h_line( state,map,chain_start,chain,i );	//	���ŏ�Ԃɂ���

						if( state -> chain_count == 0 ){
							state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
						}
						state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
						state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
					}
					if(j > 12){	//	���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
						end = 1;
					}else{
						chain_start = j;									//	�A���J�n�ʒu�̕ύX
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	�F�̓���ւ�
						chain = 0;
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col ){
					chain++;	//	�A���ǉ�
					if(j == 15){	//	�ŉ��i�ɒB�����ꍇ
						if( chain >= 3 ){
							//	�S�ȏゾ����
							dm_make_erase_h_line( state,map,chain_start,chain,i );	//	���ŏ�Ԃɂ���

							if( state -> chain_count == 0 ){
								state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
							}
							state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
							state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
						}
					}
				}
			}else{
				if( chain >= 3 ){
					//	�S�ȏゾ����
					dm_make_erase_h_line( state,map,chain_start,chain,i );	//	���ŏ�Ԃɂ���
					if( state -> chain_count == 0 ){
						state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
					}
					state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
					state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
				}
				if(j > 12){	//	���Ŕ��肪�o���Ȃ�����(�S�ȏ㖳��)
					end = 1;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
			if( end ){
				break;
			}
		}

	}
}
/*--------------------------------------
	�������������֐�
--------------------------------------*/
void	dm_make_erase_w_line( game_state *state,game_map *map,u8 start,u8 chain,u8 pos_y )
{
	u8	i,p;

	p = (pos_y << 3);

	for( i = start;i < start + chain + 1;i++ ){
		(map + p + i) -> capsel_m_flg[cap_condition_flg] = cap_flg_on;	//	���ŏ�Ԃɂ���
		if( (map + p + i) -> capsel_m_flg[cap_virus_flg] < 0){
			//	�J�v�Z���̏ꍇ
			(map + p + i) -> capsel_m_g = erase_cap_a;
		}else{
			//	�E�C���X�̏ꍇ
			(map + p + i) -> capsel_m_g = erase_virus_a;
			state -> erase_virus_count ++;	//	���ŃE�C���X���̑���
			state -> chain_color[3] |= 0x08;	//	�E�C���X���܂ޘA��
			state -> chain_color[3] |= 0x10 << (map + p + i) -> capsel_m_flg[cap_col_flg] ;
		}
	}
}
/*--------------------------------------
	���������菈���֐�
--------------------------------------*/
s8	dm_w_erase_chack( game_map *map )
{
	s16	i,j;
	u8	p;
	s8	chain,chain_start,chain_col;

	//	������
	for( j = 0;j < 16;j++ ){
		chain = 0;
		chain_col = chain_start = -1;
		for( i = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	�����F�łȂ�����
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					if( chain >= 3 ){
						//	�S�ȏゾ����
						return	cap_flg_on;
					}
					if(i > 4){
						break;
					}else{
						chain_start = i;									//	�A���J�n�ʒu�̕ύX
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	�F�̓���ւ�
						chain = 0;											//	�A�����N���A
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col){
					chain++;
					if(i == 7){
						if( chain >= 3 ){
							return	cap_flg_on;
						}
					}
				}
			}else{
				//	�S�ȏゾ����
				if( chain >= 3 ){
					//	�S�ȏゾ����
					return	cap_flg_on;
				}
				if( i > 4 ){
					break;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return	cap_flg_off;
}

/*--------------------------------------
	����������Ə��Őݒ�֐�
--------------------------------------*/
void	dm_w_erase_chack_set( game_state *state,game_map *map )
{
	s16	i,j;
	u8	p,end;
	s8	chain,chain_start,chain_col;

	//	������
	for( j = 0;j < 16;j++ ){
		chain = 0;
		chain_col = chain_start = -1;
		for( i = end = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	�����F�łȂ�����
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					if( chain >= 3 ){
						//	�S�ȏゾ����
						dm_make_erase_w_line( state,map,chain_start,chain,j );	//	���ŏ�Ԃɂ���

						if( state -> chain_count == 0 ){
							state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
						}
						state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
						state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
					}
					if(i > 4){
						end = 1;
					}else{
						chain_start = i;									//	�A���J�n�ʒu�̕ύX
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	�F�̓���ւ�
						chain = 0;											//	�A�����N���A
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col){
					chain++;
					if(i == 7){
						if( chain >= 3 ){
							//	�S�ȏゾ����
							dm_make_erase_w_line( state,map,chain_start,chain,j );	//	���ŏ�Ԃɂ���
							if( state -> chain_count == 0 ){
								state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
							}
							state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
							state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
						}
					}
				}
			}else{
				//	�S�ȏゾ����
				if( chain >= 3 ){
					//	�S�ȏゾ����
					dm_make_erase_w_line( state,map,chain_start,chain,j );	//	���ŏ�Ԃɂ���

					if( state -> chain_count == 0 ){
						state -> chain_color[3] |= 1 << chain_col;	//	�P��ڂ̏������F�̃r�b�g�𗧂Ă�
					}
					state -> chain_color[chain_col] ++;				//	���ŐF�̃J�E���g
					state -> chain_line ++;							//	���ŗ񐔂̃J�E���g
				}
				if( i > 4 ){
					end = 1;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
			if( end ){
				break;
			}
		}
	}
}
/*--------------------------------------
	�c�����菈���֐�
--------------------------------------*/
void	dm_h_ball_chack( game_map *map )
{
	u8	i,j,p,p2;

	for( i = 0;i < 8;i++ ){
		for( j = 0;j < 16;j++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	�\������Ă���
				if( (map + p) -> capsel_m_g == capsel_u ){
					//	�㑤�̃J�v�Z����������
					p2 = ((j + 1) << 3) + i;
					if( (map + p2 ) -> capsel_m_g != capsel_d ){	//	�P�i���𒲂ׂ�
						(map + p) -> capsel_m_g = capsel_b;
					}
				}else	if( (map + p) -> capsel_m_g == capsel_d ){
					//	�����̃J�v�Z����������
					if( (map + p) -> pos_m_y == 1 ){	//	�ŏ�i�ŉ����̃J�v�Z����
						//	�����I�ɗ��ɂ���
						(map + p) -> capsel_m_g = capsel_b;
					}else{
						p2 = ((j - 1) << 3) + i;
						if( (map + p2 ) -> capsel_m_g != capsel_u ){	//	�P�i��𒲂ׂ�
							(map + p) -> capsel_m_g = capsel_b;
						}
					}
				}
			}
		}
	}
}

/*--------------------------------------
	�������菈���֐�
--------------------------------------*/
void	dm_w_ball_chack( game_map *map )
{
	u8	i,j,p;

	for( j = 0;j < 16;j++ ){
		for( i = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	�\������Ă���
				if( (map + p) -> capsel_m_g == capsel_l ){
					//	�����̃J�v�Z���̏ꍇ
					if( (map + p + 1) -> capsel_m_g != capsel_r ){	//	��E�𒲂ׂ�
						(map + p) -> capsel_m_g = capsel_b;
					}
				}else	if( (map + p) -> capsel_m_g == capsel_r ){
					//	�E���̃J�v�Z���̏ꍇ
					if( (map + p - 1 ) -> capsel_m_g != capsel_l ){	//	����𒲂ׂ�
						(map + p) -> capsel_m_g = capsel_b;
					}
				}
			}
		}
	}
}
/*--------------------------------------
	�Q�[���I�[�o�[�̎��̈Â��Ȃ�֐�
--------------------------------------*/
s8	dm_black_up( game_state *state,game_map *map )
{
	u8	i,p;

	if( state -> retire_flg[dm_retire_flg] && !state -> retire_flg[dm_game_over_flg] ){
		//	���^�C�A�t���O�������Ă��邪�A�Q�[���I�[�o�[�t���O�͗����Ă��Ȃ����

		state -> erase_anime_count ++;
		if( state -> erase_anime_count >= dm_black_up_speed ){	//	���Ԋu�����
			state -> erase_anime_count = 0;

			p = (state -> work_flg - 1) << 3;
			for( i = 0;i < 8;i++ ){
				if( ( map + p + i ) -> capsel_m_flg[cap_disp_flg] ){
					//	�J�v�Z��(�E�C���X)���\������Ă�����
					( map + p + i ) -> capsel_m_p += 3;	//	�Â��F�ɐݒ�
				}
			}
			state -> work_flg --;
			if( state -> work_flg == 0){	//	�ŏ�i�܂ŏ������I�������I��
				state -> retire_flg[dm_game_over_flg] = 1;
				return	cap_flg_on;	//	�I��
			}
		}
	}
	return	cap_flg_off;	//	�܂���ƒ�
}
/*--------------------------------------
	����ݒ�
--------------------------------------*/
s8	dm_broken_set( game_state *state ,game_map *map )
{
	u16	i,j,chack;
	u16	work[DAMAGE_MAX][2];
	u8	work_b[DAMAGE_MAX][6];
	s8	ret = 0;


	//	���ꔻ��
#ifdef	DAMAGE_TYPE
	if( state -> cap_attack_work[0][0] != 0x0000 ){
		//	��Q����

		if( state -> max_chain_line < state -> chain_line ){	//	�ő���ŗ񐔂̕ۊ�
			state -> max_chain_line = state -> chain_line;
		}

		state -> chain_line = 0;		//	���Ń��C�����̃��Z�b�g
		state -> chain_count = 0;		//	�A�����̃��Z�b�g
		state -> erase_virus_count = 0;	//	���ŃE�C���X���̃��Z�b�g

		for( i = 0,j = 7;i < 16;i += 2,j--){
			chack = state -> cap_attack_work[0][0] & ( 0x0003 << i );
			if( chack != 0 ){
				//	�J�v�Z���Z�b�g
				set_map( map,j,1,capsel_b,(chack >> i) - 1);
			}
		}
		set_down_flg( map );		//	�����ݒ�

		//	����f�[�^�̃V�t�g
		for( i = 0;i < DAMAGE_MAX;i++ ){
			work[i][0] = state -> cap_attack_work[i][0];		//	�f�[�^�̕ۑ�
			work[i][1] = state -> cap_attack_work[i][1];		//	�f�[�^�̕ۑ�
			state -> cap_attack_work[i][0] = 0x0000;	//	�f�[�^�N���A
			state -> cap_attack_work[i][1] = 0x0000;	//	�f�[�^�N���A
		}
		for( i = 0,j = 1;i < DAMAGE_MAX - 1;i++,j++ ){
			state -> cap_attack_work[i][0] = work[j][0];
			state -> cap_attack_work[i][1] = work[j][1];
		}
		ret = 1;	//	����
	}
#endif
#ifndef	DAMAGE_TYPE
	if( state -> cap_attack_work[0][5] != 0x00 ){
		state -> chain_line = 0;		//	���Ń��C�����̃��Z�b�g
		state -> chain_count = 0;		//	�A�����̃��Z�b�g
		state -> erase_virus_count = 0;	//	���ŃE�C���X���̃��Z�b�g

		for( i = j = 0;i < 8;i++ ){
			if( state -> cap_attack_work[0][4] & (0x80 >> i) ){
				//	�J�v�Z���Z�b�g
				set_map( map,i,1,capsel_b,state -> cap_attack_work[0][j]);
				j ++;
			}
		}
		set_down_flg( map );		//	�����ݒ�

		//	����f�[�^�̃V�t�g
		for( i = 0;i < DAMAGE_MAX;i++ ){
			for( j = 0;j < 6;j++ ){
				work_b[i][j] = state -> cap_attack_work[i][j];
				state -> cap_attack_work[i][j] = 0;
			}
		}

		for( i = 0;i < DAMAGE_MAX - 1;i++ ){
			for( j = 0;j < 6;j++ ){
				state -> cap_attack_work[i][j] = work_b[i + 1][j];
			}
		}
		ret = 1;	//	����
	}
#endif
	return	ret;
}
/*--------------------------------------
	�U���ʒu���v�Z����֐�
--------------------------------------*/
u8	dm_make_attack_pattern( u8 max )
{
	u8	i,pattern = 0;
	u8	max_pattern[] = {0xaa,0x55};
	s8	flg;

	if( max >= 4 ){	//	�S�̏ꍇ
		pattern = max_pattern[genrand(2)];
	}else{	//	����ȉ��̏ꍇ
		while( max )
		{
			flg = 1;
			i = genrand(7);
			if( pattern & (1 << i ) ){	//	�����ꏊ�ɃJ�v�Z��������
				flg = 0;
			}
			if( i != 7 ){
				if( pattern & (1 << (i + 1) ) ){	//	�ׂɃJ�v�Z��������
					flg = 0;
				}
			}
			if( i != 0 ){
				if( pattern & (1 << (i - 1) ) ){	//	�ׂɃJ�v�Z��������
					flg = 0;
				}
			}
			if( flg ){	//	���E�ɃJ�v�Z��������������Z�b�g
				pattern |= ( 1 << i );
				max --;
			}
		}
	}
	return	pattern	;
}

/*--------------------------------------
	�U���ݒ�
--------------------------------------*/
s8	dm_attack_set_2p( game_state *state ,u8 player_no )
{
	game_state *enemy_state;
	u8	bom_pattern,bom_pattern_save;
	s8	i,j,k,l,m,n,flg,ret = 0;
	s8	stock_save[4];
	s8	attack_table[][3] = {
		{3,2,1},
		{0,3,2},
		{1,0,3},
		{2,1,0},
	};
	s8	col_work[3];
	u16	chain_count;

	chain_count = state -> chain_line;

	//	�U������
#ifdef	DAMAGE_TYPE
	if( state  -> chain_line > 1 )	//	�U��
	{
		if( chain_count >= 4 ){
			chain_count = 4;
		}

		bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
		enemy_state = &game_state_data[player_no ^ 1];
		for( i = 0;i < 1;i++ ){
			if( enemy_state -> cap_attack_work[i][0] == 0x0000 ){	//	�󂫂���������
				enemy_state -> cap_attack_work[i][1] = player_no;	//	�U�������v���C���[�ԍ�
				for( j = l = 0;j < 8;j++ ){
					if( bom_pattern & 1 ){	//	���W����
						for( k = l;k < 3;k++ ){
							if( state -> chain_color[k] > 0 ){
								state -> chain_color[k] --;
								enemy_state -> cap_attack_work[i][0] |= (1 + k) << (j << 1);
								l = k;
								break;
							}
						}
					}
					bom_pattern >>= 1;
				}
				ret = 1;
				break;
			}
		}
	}
#endif
	return	ret;
}

/*--------------------------------------
	�U���ݒ�( 4P�p )
--------------------------------------*/
void	dm_attack_set_4p( game_state *state ,u8 player_no ,s8 *c_r,s8 *c_y, s8 *c_b )
{
	game_state *enemy_state;
	u16	chain_count;
	u8	bom_pattern,bom_pattern_save;
	s8	i,j,k,l,m,n,flg,ret = 0;
	s8	stock_save[4];
	s8	attack_col[3];
	s8	col_work[3];
	s8	attack_table[][3] = {
		{3,2,1},
		{0,3,2},
		{1,0,3},
		{2,1,0},
	};


	chain_count = state -> chain_line;
	for(i = 0;i < 3;i++){
		attack_col[i] = 0;
	}

	//	�U������
#ifdef	DAMAGE_TYPE
	if( state  -> chain_line > 1 )	//	�U��
	{
		//	�S���U���̏ꍇ�͍ŏ��Ɍv�Z���Ă���
		if( chain_count >= 4 ){
			chain_count = 4;
			bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
		}

		for( i = 0;i < 3;i++ ){
			if( state -> chain_color[3] & (1 << i) ){	//	�U����������߂�
				flg = 0;
				enemy_state = &game_state_data[attack_table[player_no][i]];
				if( enemy_state -> player_state[PS_TEAM_FLG] != state -> player_state[PS_TEAM_FLG] ){	//	�����`�[���łȂ��ꍇ�U��
					if( !enemy_state -> retire_flg[dm_retire_flg] ){	//	���^�C�A���Ă��Ȃ�������
						flg = 1;
					}else{
						if( enemy_state -> game_condition[dm_training_flg] ){
							if( enemy_state -> retire_flg[dm_training_flg] ){
								flg = 1;
							}
						}
					}
					if( flg ){
						chain_count = state -> chain_line;
						if( chain_count >= 4 ){
							bom_pattern = bom_pattern_save;
							chain_count = 4;
						}else{
							for( j = 0;j < 4;j++ ){
								if( story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] != -1 ){
									//	�X�g�b�N���������ꍇ�g�p
									state -> chain_color[ story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] ]++;	//	�X�g�b�N�F�̒ǉ�
									story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = -1;	//	�X�g�b�N�F�̏���
									chain_count ++;
									if( chain_count >= 4 ){
										break;
									}
								}
							}
							//	�U���p�^�[���̍쐬
							bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
						}
						for(m = 0;m < 3;m++){
							col_work[m] = state -> chain_color[m];
						}
						for( m = 0;m < DAMAGE_MAX;m++ ){
							if( enemy_state -> cap_attack_work[m][0] == 0x0000 ){	//	�󂫂���������
								if( !enemy_state -> game_condition[dm_retire_flg] ){	//	���^�C�A���Ă��Ȃ�������
									set_shock( attack_table[player_no][i], enemy_state -> map_x + 10,207);	//	���炢���o�ݒ�
								}
								enemy_state -> cap_attack_work[m][1] = player_no;	//	�U�������v���C���[�ԍ�
								for( j = l = 0;j < 8;j++ ){
									if( bom_pattern & 1 ){	//	���W����
										for( k = l;k < 3;k++ ){
											if( col_work[k] > 0 ){
												col_work[k] --;
												enemy_state -> cap_attack_work[m][0] |= (1 + k) << (j << 1);
												l = k;
												break;
											}
										}
									}
									bom_pattern >>= 1;
								}
								attack_col[i] = 1;	//	�U������
								break;
							}
						}
					}
				}
			}
		}

		//	�X�g�b�N�̃V�t�g
		for(j = 0;j < 4;j++){
			stock_save[j] = story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j];
			story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = -1;
		}
		for(j = k = 0;j < 4;j++){
			if( stock_save[j] >= 0 ){
				story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][k] = stock_save[j];
				k++;
			}
		}
		//	�X�g�b�N�v�Z
		for( i = 0;i < 3;i++ ){
			if( state -> chain_color[3] & (1 << i) ){	//	�U������𒲂ׂ�
				flg = 0;
				enemy_state = &game_state_data[attack_table[player_no][i]];
				if( enemy_state -> player_state[PS_TEAM_FLG] == state -> player_state[PS_TEAM_FLG] ){	//	�����`�[���̏ꍇ�X�g�b�N�v�Z
					if( !enemy_state -> game_condition[dm_retire_flg] ){	//	���^�C�A���Ă��Ȃ�������
						for(m = 0;m < 3;m++){
							col_work[m] = state -> chain_color[m];
						}
						for( j = l = 0;j < 4;j++ ){
							if( story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG] ]][j] == -1 ){
								//	�����X�g�b�N������������
								for( k = l;k < 3;k++ ){
									if( col_work[k] > 0 ){
										col_work[k] --;
										story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = k;
										l = k;
										break;
									}
								}
							}
						}
					}
				}
			}
		}

	}

#endif
	*c_r = attack_col[0];	//	�ԍU��
	*c_y = attack_col[1];	//	���U��
	*c_b = attack_col[2];	//	�U��

}

/*--------------------------------------
	�Q�[�������֐�(�P�o�p)
--------------------------------------*/
s8	dm_game_main_cnt_1P( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j;
	s8	chain_flg[] = {0,0,1};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};
#ifdef	DM_DEBUG_FLG
	u16	debug_key = 0x0000 | DM_KEY_CU | DM_KEY_CD ;
	//| L_CBUTTONS | R_CBUTTONS;

	//	���̃X�e�[�W��
	if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){
		if( joyupd[main_joy[0]] & DM_KEY_L ){
			return	dm_ret_next_stage;
		}else	if( joyupd[main_joy[0]] & DM_KEY_R ){
			return	dm_ret_clear;
		}
	}
#endif

	//	�|�[�Y����
	if( joyupd[main_joy[0]] & DM_KEY_START ){
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){	//	�ʏ��Ԃ�����
			if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
				dm_set_se( dm_se_pause );				//	�|�[�Y�r�d�Đ�
				return	dm_ret_pause;
			}
		}
	}

//	dm_virus_anime( state,map );	//	�E�C���X�A�j���[�V����

	for(i = 0;i < dm_retry_coin;i++ ){
		//	�R�C����]
		if( !dm_retry_coin_pos[i].flg ){
			//	�R�C�����ʏ��Ԃ������ꍇ
			dm_retry_coin_pos[i].anime[0]++;
			if( dm_retry_coin_pos[i].anime[0] >= 20 ){
				dm_retry_coin_pos[i].anime[0] = 0;
				dm_retry_coin_pos[i].anime[1] ++;
				if( dm_retry_coin_pos[i].anime[1] > 3 ) {
					dm_retry_coin_pos[i].anime[1] = 0;
				}
			}
		}
	}


	if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	�ʏ��Ԃ�����

		dm_level_score_main( state );	//	���_�A�j������

		//	���Ԍv�Z
		if( story_time_flg ){
			if( evs_game_time < DM_MAX_TIME ){	//	99��59�b
				evs_game_time ++;
			}
		}

		//	����E�C���X�A�j���[�V����
		for(i = j = 0;i < 3;i++){
			if( big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	�����A�j���[�V�����̏ꍇ
				if( big_virus_anime[i].cnt_now_type[big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
					j++;
					if( big_virus_anime[i].cnt_now_frame == 20 ){
						if( !big_virus_flg[i][1] ){				//	����E�C���X���� SE ���Đ����Ă��Ȃ�
							big_virus_flg[i][1] = 1;			//	�Q��炳�Ȃ����߂̃t���O�Z�b�g
							dm_set_se( dm_se_big_virus_erace );	//	����E�C���X���� SE �Z�b�g
						}
					}
				}
			}else	if( big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
				j ++;
			}
		}
		//	��]�i�~��`���j����
		if( j == 0 ){
			big_virus_count[3] ++;
			if( big_virus_count[3] >= 10 ){
				big_virus_count[3] = 0;

				for(i = 0;i < 3;i++){
					big_virus_position[i][2] ++;
					if( big_virus_position[i][2] >= 360 ){
						big_virus_position[i][2] = 0;
					}
					big_virus_position[i][0] = (( 10 * sinf( DEGREE( big_virus_position[i][2] ) ) ) * -2 ) + 45;
					big_virus_position[i][1] = (( 10 * cosf( DEGREE( big_virus_position[i][2] ) ) ) * 2  ) + 155;
				}
			}
		}
//		chain_main( state,player_no ,state -> chain_line,chain_flg[flg],center[flg][player_no] );	//	�A�����o����
	}

	//	������������
	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:	//	�E�C���X�z�u
		dm_set_virus( state,map,virus_map_data[player_no],virus_map_disp_order[player_no] );
		return	dm_ret_virus_wait;
	case	dm_mode_wait:	//	�����̑҂�
		return	dm_ret_virus_wait;
	case	dm_mode_throw:	//	������
		state -> cap_speed_count ++;
		if( state -> cap_speed_count == FlyingCnt[state -> cap_def_speed]){	//	�J�v�Z�����r�����ɓ�������
			dm_init_capsel_go( &state -> now_cap,(CapsMagazine[state ->cap_magazine_save] >> 4) % 3,CapsMagazine[state ->cap_magazine_save] % 3);
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	�J�v�Z�������J�n
			state -> cap_speed_count = 30;					//	���i����
			dm_capsel_down( state,map );
		}
		break;
	case	dm_mode_down_wait:	//	���n�E�F�C�g
		if( dm_check_game_over( state,map ) ){	//	�Q�[���I�[�o�[�̏ꍇ
			for( i = 0;i < 3;i++ ){
				if( big_virus_anime[i].cnt_now_anime != ANIME_lose ){	//	���ł��Ă��Ȃ�������
					dm_anime_set( &big_virus_anime[i],ANIME_win );	//	����E�C���X�΂��A�j���[�V�����Z�b�g
				}
			}
			return	dm_ret_game_over;			//	�ς݂�����(�Q�[���I�[�o�[)
		}else{
			if( dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off ){	//	���ł��������ꍇ
				state -> game_mode[dm_mode_now] = dm_mode_erase_chack;	//	���ŊJ�n
				state -> cap_speed_count = 0;
			}else{
				state -> game_mode[dm_mode_now] = dm_mode_cap_set;	//	�J�v�Z���Z�b�g���w�肵�Ă���
			}
		}
		break;
	case	dm_mode_erase_chack:	//	���Ŕ���
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){	//	���n�E�F�C�g�������

			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;	//	���ŊJ�n

			dm_h_erase_chack_set( state,map );	//	�c��������
			dm_w_erase_chack_set( state,map );	//	����������
			dm_h_ball_chack( map );				//	�c��������
			dm_w_ball_chack( map );				//	����������


			//	�e�F�̃E�C���X���̎擾
			state -> virus_number = get_virus_color_count( map,&big_virus_count[0], &big_virus_count[1] ,&big_virus_count[2] );
			for( i = 0;i < 3;i++ )
			{
				if( big_virus_count[i] == 0 ){	//	�e�F�̃E�C���X���S�ł����ꍇ
					if( !big_virus_flg[i][0] ){		//	�܂����Ńt���O�������Ă��Ȃ��ꍇ
						big_virus_flg[i][0] = 1;	//	���Ńt���O�������Ă�
						dm_anime_set( &big_virus_anime[i],ANIME_lose );	//	����E�C���X���ŃA�j���[�V�����Z�b�g
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_damage );			//	����E�C���X�_���[�W
						}
					}
				}else{
					if( state -> chain_color[3] & (0x10 << i) ){
						dm_anime_set( &big_virus_anime[i],ANIME_damage );	//	����E�C���X����A�j���[�V�����Z�b�g
						dm_set_se( dm_se_big_virus_damage );				//	����E�C���X�_���[�W
					}
				}
			}
			state -> chain_color[3] &= 0x0f;	//	���ŐF�̃��Z�b�g


			if( state -> virus_number == 0 ){
				//	�E�C���X�S��
				dm_score_make( state,0 );									//	���_�v�Z
				if( evs_high_score < state -> game_score ){
					evs_high_score = state -> game_score;	//	�n�C�X�R�A�̍X�V
				}
				state -> game_condition[dm_mode_now]	= dm_cnd_stage_clear;	//	�X�e�[�W�N���A
				state -> game_mode[dm_mode_now] 		= dm_mode_stage_clear;
				//	�ő���ŗ񐔂̕ۊ�
				if( state -> max_chain_line < state -> chain_line ){
					state -> max_chain_line = state -> chain_line;
				}
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	�E�C���X���c��R�ȉ��̂Ƃ��x������炷
				if( !last_3_se_flg ){
					//	�P��Ȃ炵����Ȍ�炳�Ȃ�
					last_3_se_flg = 1;
					dm_set_se( dm_se_last_3 );			//	�E�C���X�c��R�r�d�Đ�
				}
				//	���y���x�A�b�v�̃t���O�𗧂Ă�
				if( bgm_bpm_flg[0] == 0){
					bgm_bpm_flg[0] = 1;					//	�Q�񏈗����Ȃ����߂̃t���O
					bgm_bpm_flg[1] = 1;					//	�������s���t���O�𗧂Ă�
				}
			}
			state ->chain_count ++;
			if( state ->chain_line < 2 ){
				if( state -> chain_color[3] & 0x08 ){
					dm_level_score_make( state );			//	���_�v�Z
					state -> chain_color[3] &= 0xF7;		//	���ŐF�̃��Z�b�g
					dm_set_se( dm_se_virus_erace );			//	�E�C���X���܂ޏ��łr�d�Đ�
				}else{
					dm_set_se( dm_se_cap_erace );			//	�J�v�Z���݂̂̏��łr�d�Đ�
				}
			}else{
				if( state -> chain_color[3] & 0x08 ){
					dm_level_score_make( state );			//	���_�v�Z
					state -> chain_color[3] &= 0xF7;		//	���ŐF�̃��Z�b�g
				}
				i = state ->chain_line - 2;
				if( i > 2 )
					i = 2;
				dm_set_se( dm_se_attack_a + i );
			}
		}
		break;
	case	dm_mode_erase_anime:	//	���ŃA�j���[�V����
		dm_capsel_erase_anime( state,map );
		break;
	case	dm_mode_ball_down:		//	�J�v�Z��(��)����
		go_down( state,map,dm_down_speed );
		break;
	case	dm_mode_cap_set:		//	�V�J�v�Z���Z�b�g
		dm_warning_h_line( state,map );	//	�ς݂�����`�F�b�N

		dm_set_capsel( state );							//	�V�J�v�Z���Z�b�g
		dm_capsel_speed_up( state );					//	�����J�v�Z�����x�̌v�Z
		dm_anime_set( &state -> anime,ANIME_attack );	//	�U���A�j���[�V�����Z�b�g(��)
		dm_attack_se( state,player_no );				//	�U���r�d�Đ�

		//	�����O�v�l�ǉ��ʒu
		if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){	// CPU �̏ꍇ
			aifMakeFlagSet( state );
		}

		//	�ő���ŗ񐔂̕ۊ�
		if( state -> max_chain_line < state -> chain_line ){
			state -> max_chain_line = state -> chain_line;
		}

		state -> game_mode[dm_mode_now] = dm_mode_throw;	//	�J�v�Z������������
		state -> cap_speed_count = 0;	//	�J�E���g�̃��Z�b�g
		state -> cap_speed_max = 0;		//	�������x�̃��Z�b�g
		state -> chain_line = 0;		//	���Ń��C�����̃��Z�b�g
		state -> chain_count = 0;		//	�A�����̃��Z�b�g
		state -> erase_virus_count = 0;	//	���ŃE�C���X���̃��Z�b�g
		for( i = 0;i < 4;i++ )
			state -> chain_color[i] = 0;		//	���ŐF�̃��Z�b�g
		break;
	case	dm_mode_get_coin:		//	�R�C���l��
		if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] < 60 ){
			dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] ++;	//	�ړ��l�̑���
			dm_retry_coin_pos[dm_retry_coin - 1].pos += dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0];	//	�R�C�����W�̕ύX

			//	�R�C���̉�]
			dm_retry_coin_pos[dm_retry_coin - 1].anime[0]++;
			if( dm_retry_coin_pos[dm_retry_coin - 1].anime[0] >= ( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] >> 2 ) ){
				dm_retry_coin_pos[dm_retry_coin - 1].anime[0] = 0;
				dm_retry_coin_pos[dm_retry_coin - 1].anime[1] ++;
				if( dm_retry_coin_pos[dm_retry_coin - 1].anime[1] > 3 ) {
					dm_retry_coin_pos[dm_retry_coin - 1].anime[1] = 0;
				}
			}

			if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 60 ){
				dm_retry_coin_pos[dm_retry_coin - 1].flg = 0;	//	��]�𓯂����ɂ���
				//	�R�C���̍��W�Ɖ�]�̒���
				for( i = 1;i < 3;i++ ){
					dm_retry_coin_pos[i].pos = dm_retry_coin_pos[0].pos;
					dm_retry_coin_pos[i].anime[0] = dm_retry_coin_pos[0].anime[0];
					dm_retry_coin_pos[i].anime[1] = dm_retry_coin_pos[0].anime[1];
				}
			}else	if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 30 ){
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = 1;
			}
		}
	case	dm_mode_stage_clear:	//	�X�e�[�W�N���A���o
		i = try_next_stage_main();
		if( i > 0){
			if( joyupd[main_joy[0]] & DM_ANY_KEY ){
				return	dm_ret_next_stage;
			}
		}
		break;
	case	dm_mode_game_over:			//	�Q�[���I�[�o�[���o
	case	dm_mode_game_over_retry:	//	�Q�[���I�[�o�[ & ���g���C
		i = game_over_main();
		if( i > 0 )
		{
			if( dm_retry_flg ){
				//	���g���C�\�̏ꍇ
				if( joyupd[main_joy[0]] & DM_KEY_UP ){
					if( dm_retry_position[0][0] > 0 )
						dm_retry_position[0][0] = 0;
					}else	if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
					if( dm_retry_position[0][0] < 1 )
						dm_retry_position[0][0] = 1;
				}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
					if( dm_retry_position[0][0] ){
						return	dm_ret_end;			//	�I��
					}else{
						if( state -> game_retry < 999 ){
							state -> game_retry ++;	//	���g���C���̒ǉ�
						}
						dm_set_se( dm_se_get_coin );			//	�R�C���g�p��
						if( state -> virus_level >= 22 ){
							dm_retry_coin_pos[dm_retry_coin - 1].flg = 1;			//	�����]�����Ȃ��悤�ɂ���
							dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = -1;	//	�ړ������̐ݒ�
							dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] = 0;	//	�J�E���^�̐ݒ�
							state -> game_mode[dm_mode_now] = dm_mode_use_coin;
						}else{
							return	dm_ret_retry;		//	���g���C
						}
					}
				}
			}else{
				//	�Q�[���I�[�o�[
				if( joyupd[main_joy[0]] & DM_ANY_KEY ){
					return	dm_ret_end;
				}
			}
		}
		break;
	case	dm_mode_use_coin:	//	�R�C���g�p�f��
		if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] < 30 ){
			dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] ++;	//	�ړ��l�̑���
			dm_retry_coin_pos[dm_retry_coin - 1].pos += dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0];	//	�R�C�����W�̕ύX
			//	�R�C���̉�]
			dm_retry_coin_pos[dm_retry_coin - 1].anime[0]++;
			if( dm_retry_coin_pos[dm_retry_coin - 1].anime[0] >= ( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] >> 2 ) ){
				dm_retry_coin_pos[dm_retry_coin - 1].anime[0] = 0;
				dm_retry_coin_pos[dm_retry_coin - 1].anime[1] ++;
				if( dm_retry_coin_pos[dm_retry_coin - 1].anime[1] > 3 ) {
					dm_retry_coin_pos[dm_retry_coin - 1].anime[1] = 0;
				}
			}
			if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 30 ){
				//	�����I��
//				dm_retry_coin_pos[dm_retry_coin - 1].flg = 0;	//	�����]������悤�ɂ���
				dm_retry_coin --;			// 	�R�C��������
				return	dm_ret_retry;		//	���g���C
			}
		}

		break;
	case	dm_mode_pause:	//	�|�[�Y����
		pause_main( player_no,center[flg][player_no] );
#ifdef	DM_DEBUG_FLG
		//	�f�o�b�N�p����
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	�f�o�b�N������
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
		}
#endif
		break;
	case	dm_mode_pause_retry:
		pause_main( player_no,center[flg][player_no] );

#ifdef	DM_DEBUG_FLG

		//	�f�o�b�N�p����
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	�f�o�b�N������
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
			break;
		}
#endif
		if( dm_retry_flg ){
			//	���g���C�\�̏ꍇ
			if(  joyupd[main_joy[0]] &  DM_KEY_START  ){
				return	dm_ret_pause;		//	�|�[�Y����
			}else	if( joyupd[main_joy[0]] & DM_KEY_UP ){
				if( dm_retry_position[0][0] > 0 )
					dm_retry_position[0][0] = 0;
				}else	if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
				if( dm_retry_position[0][0] < 1 )
					dm_retry_position[0][0] = 1;
			}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
				if( dm_retry_position[0][0] ){
					return	dm_ret_end;			//	�I��
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;	//	���g���C���̒ǉ�
					}
					if( state -> virus_level >= 22 ){
						dm_set_se( dm_se_get_coin );							//	�R�C���g�p��
						dm_retry_coin_pos[dm_retry_coin - 1].flg = 1;			//	�����]�����Ȃ��悤�ɂ���
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = -1;	//	�ړ������̐ݒ�
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] = 0;	//	�J�E���^�̐ݒ�
						state -> game_mode[dm_mode_now] = dm_mode_use_coin;
					}else{
						return	dm_ret_retry;		//	���g���C
					}
				}
			}
		}else{
			if(  joyupd[main_joy[0]] & DM_KEY_START ){
				return	dm_ret_pause;		//	�|�[�Y����
			}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
				if( dm_retry_position[0][0] ){
					return	dm_ret_game_end;	//	���S�I��
				}
			}
		}
		break;
#ifdef	DM_DEBUG_FLG
	case	dm_mode_debug:
		//	�f�o�b�N�p����
		cnt_debug_main( player_no );
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_pause;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_pause;						//	�f�o�b�N������
			evs_gamespeed = debug_gamespeed;
			if ( debug_p1cpu ) {
				game_state_data[0].player_state[0] = PUF_PlayerCPU;
				aiDebugP1 = debug_p1cpu - 1;
			} else {
				game_state_data[0].player_state[0] = PUF_PlayerMAN;
			}
		}
		break;
#endif
	case	dm_mode_no_action:	//	�������Ȃ�
		break;
	}
	return	0;
}

/*--------------------------------------
	�Q�[�������֐�(���l���p)
--------------------------------------*/
s8	dm_game_main_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j,out;
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s8	attack_col[] = {0,0,0};
	u16	key_data;
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};
#ifdef	DM_DEBUG_FLG
	u16	debug_key = 0x0000 | DM_KEY_CU | DM_KEY_CD ;
	//| L_CBUTTONS | R_CBUTTONS;
#endif

	//	�L�[���̎擾
	if( flg == 1 ){
		//	2P��
		key_data = joyupd[main_joy[player_no]];
	}else	if( flg == 2 ){
		//	4P��
		key_data = joyupd[player_no];
	}



	//	PAUSE �v�� or PAUSE �����v������
	if( key_data & DM_KEY_START ){
#ifndef	DM_DEBUG_FLG
		if( state -> player_state[PS_PLAYER] != PUF_PlayerCPU ){
			//	�v���C���[�̏ꍇ
			if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
				if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
					dm_set_se( dm_se_pause );				//	�|�[�Y�r�d�Đ�
					return	dm_ret_pause;
				}
			}
		}
#else
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
			if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
				dm_set_se( dm_se_pause );				//	�|�[�Y�r�d�Đ�
				return	dm_ret_pause;
			}
		}
#endif
	}

	//	��������
#ifdef	DM_DEBUG_FLG
	if( key_data & DM_KEY_R ){
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){
			return	dm_ret_clear;
		}
	}
#endif



	if( state -> game_condition[dm_mode_now] != dm_cnd_wait && state -> game_condition[dm_mode_now] != dm_cnd_pause ){
		dm_black_up( state,map );		//	���オ�菈��
	}

	if( state -> game_condition[dm_mode_now] == dm_cnd_wait ){	//	�ʏ��Ԃ�����
		bubble_main( player_no );	//	�U�����o����
//		chain_main( state ,player_no ,state -> chain_line,chain_flg[flg],center[flg][player_no] );	//	�A�����o����
		shock_main( player_no, state -> map_x + 10,207);		//	���炢���o����
		humming_main( player_no, state -> map_x + 20 );			//	�n�~���O���o����
	}

#ifdef	DM_DEBUG_FLG
//	if ( game_state_data[0].player_state[0] == PUF_PlayerCPU && win_count[0] < 10 && win_count[1] < 10 ) {
	if ( game_state_data[0].player_state[0] == PUF_PlayerCPU && win_count[0] < WINMAX && win_count[1] < WINMAX ) {
		switch( state -> game_mode[dm_mode_now] )
		{
		case	dm_mode_game_over:		//	�Q�[���I�[�o�[���o
		case	dm_mode_win:	//	WIN���o
		case	dm_mode_lose:	//	LOSE���o
		case	dm_mode_draw:	//	DRAW���o
//			joyupd[player_no] |= DM_ANY_KEY;
			key_data |= DM_ANY_KEY;
			break;
		}
	}

#endif

	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:		//	�E�C���X�z�u
		dm_set_virus( state,map,virus_map_data[player_no],virus_map_disp_order[player_no] );
		return	dm_ret_virus_wait;
	case	dm_mode_wait:		//	�E�F�C�g
		return	dm_ret_virus_wait;
	case	dm_mode_down_wait:	//	���n�E�F�C�g
		if( dm_check_game_over( state,map ) ){	//	�ς݂����蔻��
			return	dm_ret_game_over;			//	�ς݂�����(�Q�[���I�[�o�[)
		}else{
			if( dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off ){
				if( !state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă��Ȃ�
					state -> game_mode[dm_mode_now] = dm_mode_erase_chack;	//	���Ŕ���
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă���(���K��)
					state -> game_mode[dm_mode_now] = dm_mode_tr_erase_chack;	//	���Ŕ���
				}
				state -> cap_speed_count = 0;
			}else{
				if( !state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă��Ȃ�
					state -> game_mode[dm_mode_now] = dm_mode_cap_set;	//	�J�v�Z���Z�b�g
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	���^�C�A���Ă���(���K��)
					state -> game_mode[dm_mode_now] = dm_mode_tr_cap_set;	//	�J�v�Z���Z�b�g
				}
			}
		}
		break;
	case	dm_mode_erase_chack:	//	���Ŕ���
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){

			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;	//	���ŊJ�n

			dm_h_erase_chack_set( state,map );	//	�c��������
			dm_w_erase_chack_set( state,map );	//	����������
			dm_h_ball_chack( map );				//	�c��������
			dm_w_ball_chack( map );				//	����������

			state -> virus_number = get_virus_count( map ); //	�E�C���X���̎擾
			if( state -> virus_number == 0 ){
				//	�E�C���X����
				dm_score_make( state,1 );	//	���_�v�Z
				//	�ő���ŗ񐔂̕ۊ�
				if( state -> max_chain_line < state -> chain_line ){
					state -> max_chain_line = state -> chain_line;
				}
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	�E�C���X���c��R�ȉ��̂Ƃ��x������炷
				if( !last_3_se_flg ){
					//	�P��Ȃ炵����Ȍ�炳�Ȃ�
					last_3_se_flg = 1;
					dm_set_se( dm_se_last_3 );			//	�E�C���X�c��R�r�d�Đ�
				}
				//	���y���x�A�b�v�̃t���O�𗧂Ă�
				if( bgm_bpm_flg[0] == 0){
					bgm_bpm_flg[0] = 1;					//	�Q��s��Ȃ����߂̃t���O
					bgm_bpm_flg[1] = 1;					//	�������s���t���O�𗧂Ă�
				}
			}

			state ->chain_count ++;

			switch( evs_gamesel )
			{
			case	GSL_2PLAY:	//	2�o�̎�
			case	GSL_2DEMO:	//	2�o�f��
			case	GSL_VSCPU:	//	VSCPU�̂Ƃ�
				if( state ->chain_line < 2 ){

					if( state -> chain_color[3] & 0x08 ){
						dm_score_make( state,1 );				//	���_�v�Z
						state -> chain_color[3] &= 0xF7;		//	�E�C���X���ł̃��Z�b�g
						dm_set_se( dm_se_virus_erace );			//	�E�C���X���܂ޏ��łr�d�Đ�
					}else{
						dm_set_se( dm_se_cap_erace );			//	�J�v�Z���݂̂̏��łr�d�Đ�
					}
				}else{
					j = state ->chain_line - 2;
					if( j > 2 )
						j = 2;
					dm_set_se( chain_se[player_no] + j );

					if( state -> chain_color[3] & 0x08 ){
						state -> chain_color[3] &= 0xF7;		//	�E�C���X���ł̃��Z�b�g
						dm_score_make( state,1 );				//	���_�v�Z
					}
				}
				break;
			case	GSL_4PLAY:	//	4�o�̎�
			case	GSL_4DEMO:	//	4�o�f��
				if( state -> chain_line > 1 ){
					set_humming( player_no,state -> map_x + 20,206 ); // �n�~���O�J�n
				}

				if( state -> chain_color[3] & 0x08 ){
					state -> chain_color[3] &= 0xF7;		//	�E�C���X���܂ޏ��ł̃��Z�b�g
					dm_score_make( state,1 );				//	���_�v�Z
					dm_set_se( dm_se_virus_erace );			//	�E�C���X���܂ޏ��łr�d�Đ�
				}else{
					dm_set_se( dm_se_cap_erace );			//	�J�v�Z���݂̂̏��łr�d�Đ�
				}
			}
		}
		break;
	case	dm_mode_erase_anime:	//	���ŃA�j���[�V����
		dm_capsel_erase_anime( state,map );
		break;
	case	dm_mode_ball_down:
		go_down( state,map,dm_down_speed );	//	�J�v�Z��(��)����
		break;
	case	dm_mode_cap_set:	//	�V�J�v�Z���Z�b�g
		out = 1;
		dm_attack_se( state,player_no );	//	�U���r�d�Đ�
		dm_warning_h_line( state,map );	//	�ς݂�����`�F�b�N

		switch( evs_gamesel ){
		case	GSL_2DEMO:	//	2�o�f���̎�
		case	GSL_2PLAY:	//	2�o�̎�
		case	GSL_VSCPU:	//	VSCPU�̎�
			i =  dm_attack_set_2p( state ,player_no );	//	�U������
			if( i ){
				dm_anime_set_cnt( &state -> anime,ANIME_attack );	//	�U���A�j���[�V�����Z�b�g
			}
			set_bubble( state ,player_no,0 ,i,attack_col[0],attack_col[1],attack_col[2] );			//	���o�ݒ�
			if( dm_broken_set( state , map ) ){			//	���ꔻ��
				dm_anime_set( &state -> anime,ANIME_damage );		//	����A�j���[�V�����Z�b�g(����)
				dm_set_se( damage_se[player_no] );		//	�E�C���X���܂ޏ��łr�d�Đ�
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	������������
				out = 0;
				if ( state->ai.aiState & AIF_DAMAGE ) {
					state->ai.aiState |= AIF_DAMAGE2;
				} else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;
		case	GSL_4PLAY:	//	4�o���[�h�̎�
		case	GSL_4DEMO:	//	4�o�f���̎�
			dm_attack_set_4p( state ,player_no,&attack_col[0],&attack_col[1],&attack_col[2] );	//	�U������
			set_bubble( state ,player_no,1,0,attack_col[0],attack_col[1],attack_col[2]);		//	���o�ݒ�
			if( dm_broken_set( state , map )){		//	���ꔻ��
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	������������
				out = 0;
				if ( state->ai.aiState & AIF_DAMAGE ) {
					state->ai.aiState |= AIF_DAMAGE2;
				} else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	�����J�v�Z�����x�̌v�Z

			//	�ő���ŗ񐔂̕ۊ�
			if(	state -> max_chain_line < state -> chain_line ){
				state -> max_chain_line = state -> chain_line;
			}

			state -> chain_line = 0;			//	���Ń��C�����̃��Z�b�g
			state -> chain_count = 0;			//	�A�����̃��Z�b�g
			state -> erase_virus_count = 0;		//	���ŃE�C���X���̃��Z�b�g
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;	//	���ŐF�̃��Z�b�g
			}

			state -> game_mode[dm_mode_now] = dm_mode_down;	//	�J�v�Z������������
			//	�����O�v�l�ǉ��ʒu
			if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifMakeFlagSet( state );
			}
		}
		break;
	case	dm_mode_win_retry:
		i = win_main( center[flg][player_no] ,player_no );
		if( i > 0 ){
			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	�|�[�Y����
			}else	if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	�I��
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	���g���C���̒ǉ�
					}
					return	dm_ret_retry;		//	���g���C
				}
			}
		}
		break;
	case	dm_mode_win:	//	WIN���o
		i = win_main( center[flg][player_no],player_no );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_lose_retry:	//	LOSE & RERY
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0 ){
			if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	�I��
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	���g���C���̒ǉ�
					}
					return	dm_ret_retry;		//	���g���C
				}
			}
		}
		break;
	case	dm_mode_lose:	//	LOSE���o
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_draw:	//	DRAW���o
		i = draw_main( player_no,center[flg][player_no] );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_tr_chaeck:	//	���K�m�F
		if( key_data & DM_KEY_START ){	//	�U��������
			return	dm_ret_tr_a;
		}else	if( key_data & DM_KEY_Z ){	//	�U�������
			return	dm_ret_tr_b;
		}
		break;
	case	dm_mode_training:	//	�g���[�j���O������
		state -> retire_flg[dm_game_over_flg] = 1;				//	�Q�[���I�[�o�[�t���O�𗧂Ă�

		clear_map_all( map );		//	�}�b�v���폜
		state -> virus_number = 0;	//	�E�C���X���̃N���A
		state -> warning_flg = 0;	//	�x�����t���O�̃N���A

		//	�J�v�Z���������x�̐ݒ�
		state -> cap_speed = GameSpeed[state -> cap_def_speed];
		state -> cap_speed_max = 0;								//	�����ݒ肳���
		state -> cap_speed_vec = 1;								//	�����J�E���^�����l
		state -> cap_magazine_cnt = 1;							//	�}�K�W���c�Ƃ�1�ɂ���
		state -> cap_count = state ->cap_speed_count = 0;		//	�J�E���^�̏�����
		//	�J�v�Z�����̏����ݒ�
		dm_set_capsel( state );

		//	�E�C���X�A�j���[�V�����̐ݒ�
		state -> erase_anime = 0;								//	���ŃA�j���[�V�����R�}���̏�����
		state -> erase_anime_count = 0;							//	���ŃA�j���[�V�����J�E���^�̏�����
		state -> erase_virus_count = 0;							//	���ŃE�C���X�J�E���^�̏�����

		//	�A�����̏�����
		state -> chain_count = state -> chain_line =  0;

		//	�U���J�v�Z���f�[�^�̏�����
		for( i = 0;i < 4;i++ ){
			state -> chain_color[i] = 0;
		}
		//	�픚�J�v�Z���f�[�^�̏�����
#ifdef	DAMAGE_TYPE
		for( i = 0;i < DAMAGE_MAX;i++ ){
			state -> cap_attack_work[i][0] = 0;	//	�U������
			state -> cap_attack_work[i][1] = 0;	//	�U������
		}
#endif
#ifndef	DAMAGE_TYPE
		for( i = 0;i < DAMAGE_MAX;i++ ){
			for( j = 0;j < 5;j++ ){
				state -> cap_attack_work[i][j] = 0;
			}
		}
#endif
		stop_chain_main(player_no);	//	�A�����\���̋����I��
		state -> game_mode[dm_mode_now] = dm_mode_down;		//	�ʏ폈����
		state -> game_condition[dm_mode_now] = dm_cnd_wait;		//	�ʏ��Ԃ�
		break;
	case	dm_mode_tr_erase_chack:
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){
			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;

			dm_h_erase_chack_set( state,map );	//	�c��������
			dm_w_erase_chack_set( state,map );	//	����������
			dm_h_ball_chack( map );						//	�c��������
			dm_w_ball_chack( map );						//	����������

			state ->chain_count ++;
			if( state -> chain_color[3] & 0x08 ){
				state -> chain_color[3] &= 0xF7;		//	�E�C���X�t���O
			}else{
				dm_set_se( dm_se_cap_erace );			//	�J�v�Z���݂̂̏��łr�d�Đ�
			}
		}
		break;
	case	dm_mode_tr_cap_set:
		out = 1;
		dm_warning_h_line( state,map );				//	�ς݂�����`�F�b�N
		if( state -> retire_flg[dm_training_flg] ){	//	�U������
			dm_attack_se( state,player_no );			//	�U���r�d�Đ�
			dm_attack_set_4p( state ,player_no,&attack_col[0],&attack_col[1],&attack_col[2] );				//	�U������
			set_bubble( state ,player_no,1,0,attack_col[0],attack_col[1],attack_col[2] );					//	���o�ݒ�
			if( dm_broken_set( state , map )){					//	���ꔻ��
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	������������
				out = 0;
			}
		}else{

#ifdef	DAMAGE_TYPE
			for( j = 0;j < DAMAGE_MAX;j++ ){
				state -> cap_attack_work[j][0] = 0;	//	�U������
				state -> cap_attack_work[j][1] = 0;	//	�U������
			}
#endif

		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	�����J�v�Z�����x�̌v�Z
			state -> chain_line = 0;		//	���Ń��C�����̃��Z�b�g
			state -> chain_count = 0;		//	�A�����̃��Z�b�g
			state -> erase_virus_count = 0;	//	���ŃE�C���X���̃��Z�b�g
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;		//	���ŐF�̃��Z�b�g
			}
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	�J�v�Z������������
		}
		break;
	case	dm_mode_pause:	//	�|�[�Y����
		pause_main( player_no,center[flg][player_no] );
		if(  key_data & DM_KEY_START ){
			return	dm_ret_pause;		//	�|�[�Y����
		}
#ifdef	DM_DEBUG_FLG
		//	�f�o�b�N�p����
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	�f�o�b�N������
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
		}
#endif
		break;
	case	dm_mode_pause_retry:
		pause_main( player_no,center[flg][player_no] );

		if( dm_retry_flg ){

			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	�|�[�Y����
			}else	if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	�I��
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	���g���C���̒ǉ�
					}
					return	dm_ret_retry;		//	���g���C
				}
			}
		}else{
			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	�|�[�Y����
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	���S�I��
				}
			}

		}

#ifdef	DM_DEBUG_FLG
		//	�f�o�b�N�p����
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	�f�o�b�N������
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
			break;
		}
#endif

		break;
#ifdef	DM_DEBUG_FLG
	case	dm_mode_debug:
		//	�f�o�b�N�p����
		cnt_debug_main( player_no );
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_pause;					//	�f�o�b�N��Ԃ�
			state -> game_mode[dm_mode_now] = dm_mode_pause;						//	�f�o�b�N������
			evs_gamespeed = debug_gamespeed;
			if ( debug_p1cpu ) {
				game_state_data[0].player_state[0] = PUF_PlayerCPU;
				aiDebugP1 = debug_p1cpu - 1;
			} else {
				game_state_data[0].player_state[0] = PUF_PlayerMAN;
			}
		}
		break;
#endif
	case	dm_mode_no_action:	//	�������Ȃ�
		break;
	}
	return	0;
}
/*--------------------------------------
	�Q�[�������֐�(1P�p)
--------------------------------------*/
s8	dm_game_main_1p(void)
{
	s8	i,j,flg;
	game_state	*state	= &game_state_data[0];
	game_map	*map	= game_map_data[0];

	flg = dm_game_main_cnt_1P( state,map,0,0 );
	dm_anime_control( &state -> anime );	//	�A�j���[�V����
	for(i = 0;i < 3;i++){
		dm_anime_control( &big_virus_anime[i] );	//	����E�C���X�A�j���[�V����
	}

	//	�A�j���[�V�����̍ĉғ��̃J�E���g
	if( state -> game_condition[dm_static_cnd] != dm_cnd_wait && state -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		//	���s�����肵�Ă����ꍇ�J�E���g����
		dm_anime_restart_count ++;
		if( dm_anime_restart_count >= ANIME_RESTRT_TIME ){	//	5�b
			//	�ĉғ�
			dm_anime_restart_count = 0;
			dm_anime_restart( &state -> anime );
		}
	}

	if( flg == dm_ret_virus_wait ){		//	�E�C���X�z�u����
		if( dm_start_time > 0 ){
			dm_start_time --;
		}else{
			if( state -> game_mode[dm_mode_now] == dm_mode_wait ){
				story_time_flg = 1;									//	�^�C�}�[�쓮
				state -> game_mode[dm_mode_now] = dm_mode_throw;	//	�J�v�Z�������J�n
			}
		}
	}else	if( flg == dm_ret_pause ){	//	�|�[�Y�v�� or �|�[�Y�����v��
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	�ʏ��ԂȂ��
			auSeqpVolumeDown();						//	���ʌ���
			init_pause();							//	PAUSE ���o����������

			state -> game_condition[dm_static_cnd]	=	dm_cnd_pause;									//	��ΓI�R���f�B�V������ PASUE ��Ԃɂ���
			state -> game_condition[dm_mode_old]	=	game_state_data[0].game_condition[dm_mode_now];	//	���݂̏�Ԃ�ۑ�
			state -> game_mode[dm_mode_old]			=	game_state_data[0].game_mode[dm_mode_now];		//	���݂̏����̕ۑ�

			state -> game_condition[dm_mode_now]	=	dm_cnd_pause_re;		//	��Ԃ� PAUSE & RETRY �ɐݒ�
			state -> game_mode[dm_mode_now]			=	dm_mode_pause_retry;	//	������ PAUSE & RETRY �ɐݒ�
			dm_retry_flg 							=	0;	//	���炩���� RETRY �s�\�ɐݒ肵�Ă���
			dm_retry_position[0][0]					=	1;	//	�J�[�\���ʒu�� �I��� �ɐݒ�

			if( state -> virus_level < 22 || dm_retry_coin > 0 ){	//	LEVEL 21�ȉ� �� RETRY �p�R�C��������ꍇ
				dm_retry_flg 						=	1;	//	RETRY �\�ɐݒ�
				dm_retry_position[0][0]				=	0;	//	�J�[�\���ʒu�� RETRY �ɐݒ�
			}
		}else	if( state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
			auSeqpVolumeInit();						//	���ʂ�߂�
			state -> game_condition[dm_static_cnd]	=	dm_cnd_wait;							//	��ΓI�R���f�B�V������ʏ��Ԃɂ���
			state -> game_condition[dm_mode_now]	=	state -> game_condition[dm_mode_old];	//	PAUSE �O�̏�Ԃɖ߂�
			state -> game_mode[dm_mode_now]			=	state -> game_mode[dm_mode_old];		//	PAUSE �O�̏����ɖ߂�
		}
	}else	if( flg == dm_ret_clear ){	//	�N���A
		dm_play_bgm_set( SEQ_Win );					//	�N���A BGM �̐ݒ�
		story_time_flg 							=	0;							//	�^�C�}�[�J�E���g��~
		state -> game_condition[dm_static_cnd]	=	dm_cnd_win;					//	��ΓI�R���f�B�V������������Ԃɂ���
		state -> game_condition[dm_mode_now]	=	dm_cnd_stage_clear;			//	��Ԃ� STAGE CLEAR �ɐݒ�
		state -> game_mode[dm_mode_now]			=	dm_mode_stage_clear;		//	������ STAGE CLEAR �ɐݒ�

		//	�R�C���v�Z
		if( state -> virus_level >= 21 ){	//	LEVEL 21 �ȏ�̏ꍇ
			if( game_state_data[0].virus_level == 21 ){	//	LEVEL 21 �̎�
				dm_retry_coin ++;						//	�R�C���l��
				dm_set_se( dm_se_get_coin );			//	�R�C���l����
				state -> game_mode[dm_mode_now] 					= dm_mode_get_coin;		//	�������R�C���l�����o�ɐݒ�
				dm_retry_coin_pos[dm_retry_coin - 1].flg 			= 1;					//	�R�C���̉�]������@��ς���
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0]	= -1;					//	�R�C���̏����ړ�����(��)
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1]	= 0;					//	�R�C���̏����ړ�����(�ړ��J�E���^)
			}else{
				j = state -> virus_level - 21;			//	�R�C���l���̂��߂̍��������߂�
				if( j % 3 == 0 ){						//	�R�ʃN���A
					if( dm_retry_coin < 3 ){			//	���݂̃R�C�����Q���ȉ��̏ꍇ
						//	�R�C���l��
						dm_retry_coin ++;				//	�R�C���l��
						dm_set_se( dm_se_get_coin );	//	�R�C���l����
						state -> game_mode[dm_mode_now]			 			= dm_mode_get_coin;	//	�������R�C���l�����o�ɐݒ�
						dm_retry_coin_pos[dm_retry_coin - 1].flg 			= 1;	//	�R�C���̉�]������@��ς���
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] 	= -1;	//	�R�C���̏����ړ�����(��)
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] 	= 0;	//	�R�C���̏����ړ�����(�ړ��J�E���^)
					}
				}
			}
		}

		dm_anime_set( &state -> anime,ANIME_win );		//	�����A�j���[�V�����Z�b�g
	}else	if( flg == dm_ret_game_over ){	//	�Q�[���I�[�o�[
		dm_play_bgm_set( SEQ_Lose );		//	GAME OVER ���ݒ�
		dm_anime_set( &state -> anime,ANIME_draw );								//	���������A�j���[�V�����Z�b�g
		state -> virus_anime_spead 				=	v_anime_speed;				//	�A�j���[�V�������x�𑁂�����
		state -> game_condition[dm_static_cnd]	=	dm_cnd_lose;				//	��ΓI�R���f�B�V�����𕉂���Ԃɂ���
		state -> game_condition[dm_mode_now] 	=	dm_cnd_game_over_retry;		//	��Ԃ� GAME OVER & RETRY �ɐݒ�
		state -> game_mode[dm_mode_now] 		=	dm_mode_game_over_retry;	//	������ GAME OVER & RETRY �ɐݒ�
		dm_retry_position[0][0]					=	0;							//	�J�[�\���ʒu�� RETRY �ɐݒ�
		story_time_flg 							=	0;							//	�^�C�}�[�J�E���g��~

		if( state -> virus_level < 22 ){	//	LEVEL 21 �ȉ��̏ꍇ
			dm_retry_flg = 1;		//	RETRY �\�ɐݒ�
		}else	if( state -> virus_level >= 22 ){	//	LEVEL 22 �ȏ�̏ꍇ
			if( dm_retry_coin > 0 ){	//	RETRY �R�C���������Ă����ꍇ
				dm_retry_flg = 1;		//	RETRY �\�ɐݒ�
			}else{
				state -> game_condition[dm_mode_now]	=	dm_cnd_game_over;		//	��Ԃ� GAME OVER �ɐݒ肷��
				state -> game_mode[dm_mode_now]			=	dm_mode_game_over;		//	������ GAME OVER �ɐݒ肷��
				dm_retry_flg = 0;													//	RETRY �s�\�ɐݒ�
			}
		}
	}else	if( flg == dm_ret_next_stage || flg == dm_ret_retry ){	//	���� NEXT STAGE �� RETRY �̏ꍇ
		return	flg;
	}else	if( flg == dm_ret_end ){		//	�Q�[���I��
		return	dm_ret_game_over;
	}else	if( flg == dm_ret_game_end ){	//	���S�I��
		return	dm_ret_game_end;
	}

	disp_anime_data[0][wb_flag ^ 1] = stat -> anime;	//	�`��p�f�[�^�ɈڐA

	return	cap_flg_off;
}
/*--------------------------------------
	�Q�[�������֐�(2P�p)
--------------------------------------*/
s8	dm_game_main_2p(void)
{
	s8	i,j,l,sound;
	s8	flg[2],game_over_flg,black_up_flg,win_flg;
	s16	center[] = {68,251};
	game_state	*state[2];
	game_map	*map[2];

	game_over_flg = black_up_flg = win_flg = 0;

	//	���䏈��
	for( i = 0;i < 2;i++ ){
		state[i]	=	&game_state_data[i];
		map[i]		=	game_map_data[i];

		flg[i] = dm_game_main_cnt( state[i],map[i],i,1 );
		dm_anime_control( &state[i] -> anime );					//	�A�j���[�V����
	}
	//	�A�j���[�V�����̍ĉғ��̃J�E���g
	if( state[0] -> game_condition[dm_static_cnd] != dm_cnd_wait && state[0] -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		//	���s�����肵�Ă����ꍇ�J�E���g����
		dm_anime_restart_count ++;
		if( dm_anime_restart_count >= ANIME_RESTRT_TIME ){	//	10�b
			//	�ĉғ�
			dm_anime_restart_count = 0;
			for( i = 0;i < 2;i++ ){
				dm_anime_restart( &state[i] -> anime );
			}
		}
	}


	//	�Q�[�����ԃJ�E���g
	if( evs_gamesel == GSL_VSCPU ) {
		if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
			if( state[0] -> game_condition[dm_static_cnd] == dm_cnd_wait && story_time_flg ){
				if( evs_game_time < DM_MAX_TIME ){	//	99��59�b
					evs_game_time ++;
				}
			}
		}
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait ){
		//	�E�C���X�ݒ芮��
		if( dm_start_time > 0 ){	//	��莞�Ԃ̃E�F�C�g
			dm_start_time -- ;
		}else{
			j = 1;
			for( i = 0;i < 2;i ++ ){
				if( state[i] -> game_mode[dm_mode_now] != dm_mode_wait ){
					j = 0;	//	�܂��z�u���I����Ă��Ȃ�
					break;
				}
			}
			if( j ){	//	�z�u����
				story_time_flg = 1;			//	�^�C�}�[�J�E���g�J�n
				for( i = 0;i < 2;i ++ ){
					state[i] -> game_mode[dm_mode_now] = dm_mode_down;	//�J�v�Z�������J�n
					//	�����O�v�l�ǉ��ʒu
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){	//	CPU �̏ꍇ
						aifMakeFlagSet( state[i] );
					}
				}
			}
		}
	}else	if( flg[0] == dm_ret_game_over && flg[1] ==  dm_ret_game_over ){
		//	DRAW
		for( i = 0;i < 2;i ++ ){
			state[i] -> game_mode[dm_mode_now]		=	dm_mode_draw;			//	������ DRAW �ɐݒ�
			state[i] -> virus_anime_spead 			=	v_anime_speed;			//	�A�j���[�V�������x�𑁂�����
			state[i] -> work_flg 					=	16;						//	���オ��̐ݒ�
			state[i] -> retire_flg[dm_retire_flg]	=	1;						//	���^�C�A�t���O�𗧂Ă�
			if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN �̏ꍇ
				state[i] -> game_condition[dm_mode_now] = dm_cnd_draw_any_key;	//	DRAW & ANY KEY
			}else{																//	CPU �̏ꍇ
				state[i] -> game_condition[dm_mode_now] = dm_cnd_draw;			//	DRAW
			}
			dm_anime_set( &state[i] -> anime,ANIME_draw );					//	���������A�j���[�V�����Z�b�g
		}
	}else{
		for( i = 0;i < 2;i++ ){
			if( flg[i] == dm_ret_clear ){
				//	�N���A
				win_flg = 1;
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_win;		//	��ΓI�R���f�B�V������ WIN �ɐݒ�
			}else	if( flg[i] == dm_ret_game_over ){
				//	�Q�[���I�[�o�[���̐ݒ�
				game_over_flg = 1;
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;	//	��ΓI�R���f�B�V������ LOSE �ɐݒ�
				state[i] -> virus_anime_spead = v_anime_speed;					//	�A�j���[�V�������x�𑁂�����
				state[i] -> work_flg = 16;										//	���オ�菈���̃J�E���^�̐ݒ�
				state[i] -> retire_flg[dm_retire_flg] = 1;						//	���^�C�A�t���O�𗧂Ă�
				stop_chain_main(i);												//	�A�����\���̋����I��

			}else	if( flg[i] == dm_ret_pause ){	//	PAUSE �v�� or PAUSE �����v�� ����
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){	// �ʏ��Ԃ̏ꍇ PAUSE �v��
					auSeqpVolumeDown();				//	���ʌ���
					init_pause();					//	PAUSE ����������
					dm_retry_flg = 1;				//	�ʏ�̓��g���C�\
					for( j = 0;j < 2;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_pause;								//	��ΓI�R���f�B�V������ PAUSE �ɐݒ�
						state[j] -> game_condition[dm_mode_old]		=	state[j] -> game_condition[dm_mode_now];	//	���݂̏�Ԃ̕ۑ�
						state[j] -> game_mode[dm_mode_old]			=	state[j] -> game_mode[dm_mode_now];			//	���݂̏����̕ۑ�

						if( i == j ){
							//	�|�[�Y��v�������l�����g���C�E���f�I�����𓾂�
							state[j] -> game_mode[dm_mode_now]			=	dm_mode_pause_retry;	//	PAUSE & RETRY �����ɐݒ�
							state[j] -> game_condition[dm_mode_now]		=	dm_cnd_pause_re;		//	PAUSE & RETRY ��Ԃɐݒ�
							if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
								if( !evs_one_game_flg ){	//	�X�g�[���[�ʃZ���N�g�ȊO
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_pause_re_sc;		//	PAUSE & RETRY & SCORE ��Ԃɐݒ�

									if( dm_peach_stage_flg ){	//	�s�[�`�P��
										dm_retry_flg = 0;				//	���g���C �s�\�ɐݒ肷��
										dm_retry_position[i][0] = 1;	//	�J�[�\���ʒu�� �I���ɐݒ肷��
									}
								}
							}
						}else{
							state[j] -> game_condition[dm_mode_now]		= 	dm_cnd_pause;			//	PAUSE ��Ԃɐݒ�
							state[j] -> game_mode[dm_mode_now]			= 	dm_mode_pause;			//	PAUSE �����ɐݒ�
						}
					}
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_pause ){	//	PAUSE ��ԂȂ�� PAUSE �����v��

					auSeqpVolumeInit();	//	���ʉ�
					for( j = 0;j < 2;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;								//	��ΓI�R���f�B�V������ʏ��Ԃɐݒ�
						state[j] -> game_condition[dm_mode_now]		=	state[j] -> game_condition[dm_mode_old];	//	�O�̏�Ԃɖ߂�
						state[j] -> game_mode[dm_mode_now] 			=	state[j] -> game_mode[dm_mode_old];			//	�O�̏����ɖ߂�
					}
				}
				break;
			}else	if( flg[i] == dm_ret_retry ){
				return	dm_ret_retry;			//	RETRY
			}else	if( flg[i] == dm_ret_end ){	//	�I��
				return	-1;
			}else	if( flg[i] == dm_ret_game_end ){	//	���S�I��
				return	dm_ret_game_end;
			}
		}
		if( win_flg ){
			dm_retry_flg = 1;			//	�ʏ�̓��g���C�\
			for( i = j = 0;i < 2;i++ ){	//	�N���A�l���𒲂ׂ�
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){
					j ++;
				}
			}
			if( j == 1 ){	// 	�N���A�l���� �P �̏ꍇ
				for( i = j = 0;i < 2;i++ ){
					//	�T�E���h�̐ݒ�ƃQ�[���I������̑O�ݒ�
					if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){
						win_count[i]++;				//	�����|�C���g�̑���
						if( evs_gamesel == GSL_VSCPU ) {
							if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
								j = 1;				//	�I��
								if( i == 0 ){
									sound = SEQ_Win;	//	�v���C���[���������ꍇ
								}else{
									sound = SEQ_Lose;	//	�v���C���[���������ꍇ
								}
							}else{					//	VSCOM �̏ꍇ
								if( win_count[i] == WINMAX ){	//	3�{�����
									j = 1;
								}
								if( i == 0 ){	//	�v���C���[���������ꍇ
									if( j ){	//	�R�{�擾�������H
										sound = SEQ_VSEnd;	//	�R�{�擾�̏ꍇ
									}else{
										sound = SEQ_Win;	//	�P�{�擾�̏ꍇ
									}
								}else{
									sound = SEQ_Lose;		//	����
								}
							}
						}else{	//	VSMAN
							if( win_count[i] == WINMAX ){	//	3�{�����
 								j = 1;
 								sound = SEQ_VSEnd;	//	�R�{�擾�̏ꍇ
							}else{
								sound = SEQ_Win;	//	�P�{�擾�̏ꍇ
							}
						}
					}
				}
				for( i = 0;i < 2;i++ ){
					if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){	//	�����҂̏ꍇ
						win_main( center[i] ,i);									//	�������o�̏�����
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_win;	//	������ WIN �ɐݒ�
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win;		//	�\���� WIN �ɐݒ�
						dm_anime_set( &state[i] -> anime,ANIME_win );				//	�����A�j���[�V�����Z�b�g
						if( evs_gamesel == GSL_VSCPU ) {	//	VSCOM or STORY
							if( !evs_story_flg ){	//	�X�g�[���[���[�h�ȊO�̏ꍇ
								if( i == 0 ){
									if( j ){
										//	���s���t�����ꍇ���g���C��t�t���ɂ���
										state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ�
										state[i] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ�
									}else{
										state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;		//	�\���� WIN & ANY KEY �ɐݒ�
									}
								}
							}else{					//	�X�g�[���[���[�h�̏ꍇ
								if( i == 0 ){
									//	�v���C���[�������ꍇ
									if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
										state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ�
										state[i] -> game_mode[dm_mode_now]			=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ�
									}else{
										state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_any_key_sc;	//	�\���� WIN & ANY KEY & SCORE �ɐݒ�
										if( dm_peach_stage_flg ){	//	�s�[�`�P��{
											if( !evs_secret_flg ){
												dm_peach_get_flg[0] = 1;									//	�f�����o�J�n(�f�d�s �o�d�`�b�g)
											}
										}
									}
								}
							}
						}else{	//	VSMAN
							if( j ){
								//	���s���t�����ꍇ���g���C��t�t���ɂ���
								state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_win_retry;			//	�\���� WIN & RETRY �ɐݒ�
								state[i] -> game_mode[dm_mode_now] 			=	dm_mode_win_retry;			//	������ WIN & RETRY �ɐݒ�
							}else{
								state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_win_any_key;			//	�\���� WIN & ANY KEY �ɐݒ�
							}
						}
					}else{	//	�s�҂̏ꍇ

						lose_main( i,center[i] );								//	�������o�̏�����
						dm_anime_set( &state[i] -> anime,ANIME_lose );	//	�����A�j���[�V�����Z�b�g
						state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;	//	��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
						state[i] -> game_condition[dm_mode_now]		=	dm_cnd_lose;	//	�\���� LOSE �ɐݒ肷��
						state[i] -> game_mode[dm_mode_now] 			=	dm_mode_lose;	//	������ LOSE �ɐݒ肷��

						if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
							if( i == 0 ){		//	�v���C���[�̏ꍇ
								state[i] ->	game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
								if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
									state[i] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
								}else{
									state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_retry_sc;	//	�\���� LOSE & RETRY & SCORE �ɐݒ肷��
									if( dm_peach_stage_flg ){	//	�s�[�`�P��{
										dm_retry_flg = 0;		//	���g���C�s�\�ɐݒ肷��
										state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_sc;		//	�\���� LOSE & SCORE �ɐݒ肷��
										state[i] ->	game_mode[dm_mode_now]		=	dm_mode_lose;		//	������ LOSE �ɐݒ肷��
									}
								}
							}
						}else{
							if( evs_gamesel == GSL_VSCPU ) {	//	VSCOM �̏ꍇ
								if( i == 0 )
								{
									if( j ){	//	�R�{�Ƃ��Ă�����
										//	���s���t�����ꍇ���g���C��t�t���ɂ���
										state[i] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
										state[i] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
									}else{
										state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	�\���� LOSE & ANY KEY �ɂ���
									}
								}
							}else{	//	VSMAN �̏ꍇ
								if( j ){
									//	���s���t�����ꍇ���g���C��t�t���ɂ���
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
									state[i] -> game_mode[dm_mode_now] 		=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
								}else{
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	�\���� LOSE & ANY KEY �ɂ���
								}
							}
						}
					}
				}
			}else{	//	��������
				sound = SEQ_Lose;
				for( i = 0;i < 2;i++ ){
					draw_main( i,center[i] );												//	DRAW ���o�����̏�����
					state[i] -> game_mode[dm_mode_now]	=	dm_mode_draw;					//	������ DRAW �ɐݒ肷��
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_draw_any_key;	//	�\���� DRAW & ANY KEY �ɐݒ肷��
					}else{
						state[i] -> game_condition[dm_mode_now] =	dm_cnd_draw;			//	�\���� DRAW �ɐݒ肷��
					}
					dm_anime_set( &state[i] -> anime,ANIME_draw );	//	DRAW �A�j���[�V�����Z�b�g
				}
			}
			dm_play_bgm_set( sound );	//	�炷���y�̐ݒ�
		}
		if( game_over_flg ){
			for( i = j = 0;i < 2;i++ ){
				//	�T�E���h�̐ݒ�ƃQ�[���I������̑O�ݒ�
				if( state[i] -> game_condition[dm_static_cnd] != dm_cnd_lose ){
					win_count[i]++;
					if( evs_gamesel == GSL_VSCPU ) {
						if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
							j = 1;				//	�I��
							if( i == 0 ){
								sound = SEQ_Win;	//	�v���C���[���������ꍇ
							}else{
								sound = SEQ_Lose;	//	�v���C���[���������ꍇ
							}
						}else{
							if( win_count[i] == WINMAX ){	//	3�{�����
								j = 1;
							}
							if( i == 0 ){	//	�v���C���[���������ꍇ
								if( j ){	//	3�{�����
									sound = SEQ_VSEnd;
								}else{
									sound = SEQ_Win;
								}
							}else{			//	�v���C���[���������ꍇ
								sound = SEQ_Lose;
							}
						}
					}else{
						if( win_count[i] == WINMAX ){	//	3�{�����
 							j = 1;
 							sound = SEQ_VSEnd;
						}else{
							sound = SEQ_Win;
						}
					}
				}
			}
			for( i = 0;i < 2;i++ ){
				if( state[i] -> game_condition[dm_static_cnd] != dm_cnd_lose ){		//	�����҂̏ꍇ
					win_main( center[i],i );										//	�������o�̏�����
					state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_win;		//	��ΓI�R���f�B�V������ WIN �ɐݒ肷��
					state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win;		//	�\���� WIN �ɐݒ肷��
					state[i] -> game_mode[dm_mode_now]			=	dm_mode_win;	//	������ WIN �ɐݒ肷��

					dm_anime_set( &state[i] -> anime,ANIME_win );				//	�����A�j���[�V�����Z�b�g
					if( evs_gamesel == GSL_VSCPU ) {	// VSCPU & STORY
						if( !evs_story_flg ){	//	�X�g�[���[���[�h�ȊO�̏ꍇ
							if( i == 0 ){
								if( j ){
									//	���s���t�����ꍇ���g���C��t�t���ɂ���
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_retry;	//	�\���� WIN & RETRY �ɐݒ肷��
									state[i] -> game_mode[dm_mode_now] 		=	dm_mode_win_retry;	//	������ WIN & RETRY �ɐݒ肷��
								}else{
									state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key;	//	�\���� WIN & ANYKEY �ɐݒ肷��
								}
							}
						}else{		//	�X�g�[���[���[�h�̏ꍇ
							if( i == 0 ){
								//	�v���C���[�������ꍇ
								if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
									state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ�
									state[i] -> game_mode[dm_mode_now]			=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ�
								}else{
									state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_any_key_sc;	//	�\���� WIN & ANY KEY & SCORE �ɐݒ�
									if( dm_peach_stage_flg ){	//	�s�[�`�P��
										if( !evs_secret_flg ){
											dm_peach_get_flg[0] = 1;												//	�f�����o�J�n
										}
									}
								}
							}
						}
					}else{	//	VSMAN
						if( j ){
							//	���s���t�����ꍇ���g���C��t�t���ɂ���
							state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ肷��
							state[i] -> game_mode[dm_mode_now] 		=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ肷��
						}else{
							state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;		//	�\���� WIN & ANYKEY �ɐݒ肷��
						}
					}
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_lose ){	//	�s�҂̏ꍇ
					lose_main( i,center[i] );									//	�������o�����̏�����
					dm_anime_set( &state[i] -> anime,ANIME_lose );				//	�����A�j���[�V�����Z�b�g
					state[i] -> game_condition[dm_mode_now] = dm_cnd_lose;		//	�\���� LOSE �ɐݒ肷��
					state[i] -> game_mode[dm_mode_now]		= dm_mode_lose;		//	������ LOSE �ɐݒ肷��

					if( evs_gamesel == GSL_VSCPU ) {	//	VSCPU �̏ꍇ
						if( i == 0 )	//	�v���C���[�̏ꍇ
						{
							if( evs_story_flg ){	//	�X�g���[���[�h�̏ꍇ
								state[i] -> game_mode[dm_mode_now] 		=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
								if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;	//	�\���� LOSE & RETRY �ɐݒ肷��
								}else{
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry_sc;	//	�\���� LOSE & RETRY & SCORE �ɐݒ肷��
									if( dm_peach_stage_flg ){	//	�s�[�`�P��{
										dm_retry_flg = 0;		//	���g���C�s�\�ɐݒ肷��
										state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_sc;	//	�\���� LOSE & SCORE �ɐݒ肷��
									}
								}
							}else{	// VSCCOM
								if( j ){
									//	���s���t�����ꍇ���g���C��t�t���ɂ���
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;	//	�\���� LOSE & RETRY �ɐݒ肷��
									state[i] -> game_mode[dm_mode_now]		= dm_mode_lose_retry;	//	������ LOSE & RETRY �ɐݒ肷��
								}else{
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_any_key;	//	�\���� LOSE & ANY KEY �ɐݒ肷��
								}
							}
						}
					}else{
						if( j ){
							//	���s���t�����ꍇ���g���C��t�t���ɂ���
							state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
							state[i] -> game_mode[dm_mode_now] 		= dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
						}else{
							state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_any_key;	//	�\���� LOSE & ANY KEY �ɐݒ肷��
						}
					}
				}
			}
			dm_play_bgm_set( sound );	//	BGM �̐ݒ�
		}
	}
	for(i = 0;i < 2;i++){
		disp_anime_data[i][wb_flag ^ 1] = game_state_data[i].anime;	//	�`��p�f�[�^�ɈڐA
	}

	return	cap_flg_off;
}

/*--------------------------------------
	�Q�[�������֐�(4P�p)
--------------------------------------*/
s8	dm_game_main_4p(void)
{
	s8	i,j,k,l,sound,win_team;
	s8	flg[4],game_over_flg,black_up_flg,win_flg;
	u8	bit,end_flg;
	s16	center[] = {52,124,196,268};
	game_state *state[4];
	game_map *map[4];


	for( i = 0;i < 4;i++ ){
		state[i]	=	&game_state_data[i];
		map[i]		=	game_map_data[i];
		flg[i] 		=	dm_game_main_cnt( state[i],map[i],i,2 );	//	����
	}

	//	���Ԍv�Z
	if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
		for(i = j = 0;i < 4;i++ ){
			if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){
				j ++;
			}
		}
		if( j != 0 && story_time_flg ){
			if( evs_game_time < DM_MAX_TIME ){	//	99��59�b
				evs_game_time ++;
			}
		}
	}

	game_over_flg = black_up_flg = win_flg = 0;	//	�t���O�N���A

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait &&
		flg[2] == dm_ret_virus_wait && flg[3] == dm_ret_virus_wait  ){

		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			j = 1;
			for( i = 0;i < 4;i ++ ){
				if( state[i] -> game_mode[dm_mode_now] != dm_mode_wait ){
					j = 0;	//	�܂��z�u���I����Ă��Ȃ�
					break;
				}
			}
			if( j ){
				//	�E�C���X�ݒ芮��
				story_time_flg = 1;	//	�^�C�}�[�J�E���g�J�n
				for( i = 0;i < 4;i ++ ){
					state[i] -> game_mode[dm_mode_now] = dm_mode_down;	//�J�v�Z�������J�n
					//	�����O�v�l�ǉ��ʒu
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
						aifMakeFlagSet( state[i] );
					}
				}
			}
		}
	}else{
		for( i = 0;i < 4;i++ ){
			if( flg[i] == dm_ret_clear ){
				//	�N���A
				win_flg = 1;
				state[i] -> game_condition[dm_static_cnd] = dm_cnd_win;		//	��ΓI�R���f�B�V������ WIN �ɐݒ肷��
			}else	if( flg[i] == dm_ret_game_over ){
				//	�Q�[���I�[�o�[���̐ݒ�
				stop_chain_main(i);	//	�A�����\���̋����I��
				if( !state[i] -> retire_flg[dm_retire_flg] ){	//	���^�C�A��Ԃ���Ȃ��ꍇ
					state[i] -> virus_anime_spead 				=	v_anime_speed_4p;	//	�A�j���[�V�������x�𑁂�����
					state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_retire;		//	���^�C�A��Ԃɂ���
					state[i] -> game_condition[dm_training_flg] =	dm_cnd_retire;		//	���^�C�A��Ԃɂ���
					state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;		//	��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
					state[i] -> work_flg = 16;											//	���オ��p�J�E���^�̃Z�b�g
					state[i] -> retire_flg[dm_retire_flg] = 1;							//	���^�C�A�t���O�𗧂Ă�
					game_over_flg = 1;
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){			//	�b�o�t�Ȃ���K���Ȃ�
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_no_action;	//	������ NO ACTION (�������Ȃ�) �ɐݒ肷��
					}else{
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_tr_chack;	//	�\������K�m�F�ɐݒ肷��
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_tr_chaeck;	//	��������K�m�F�ɐݒ肷��
					}
				}else{	//	���K���̏ꍇ
					state[i] -> game_condition[dm_mode_now]		=	dm_cnd_tr_chack;	//	�\������K�m�F�ɐݒ肷��
					state[i] -> game_mode[dm_mode_now]			=	dm_mode_tr_chaeck;	//	��������K�m�F�ɐݒ肷��
				}
			}else	if( flg[i] == dm_ret_pause ){	//	PAUSE �v�� or PAUSE �����v��
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	�ʏ��ԂȂ��
					auSeqpVolumeDown();				//	�{�����[���_�E��
					init_pause();					//	�|�[�Y����������
					dm_retry_flg = 1;				//	���g���C�\�ɐݒ肷��
					for( j = 0;j < 4;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_pause;								//	��ΓI�R���f�B�V������ PAUSE �ɐݒ肷��
						state[j] -> game_condition[dm_mode_old]		=	state[j] -> game_condition[dm_mode_now];	//	���݂̏�Ԃ̕ۑ�
						state[j] -> game_mode[dm_mode_old] 			=	state[j] -> game_mode[dm_mode_now];			//	���݂̏����̕ۑ�
						if( i == j ){	//	�|�[�Y�v���������l
							state[j] -> game_mode[dm_mode_now]	=	dm_mode_pause_retry;					//	������ PAUSE & RETRY �ɐݒ肷��
							state[j] -> game_condition[dm_mode_now] = dm_cnd_pause_tar_re;					//	��Ԃ� PAUSE & TARGET & RETRY �ɐݒ肷��
							if( evs_story_flg ){
								//	�X�g^-���[���[�h�̏ꍇ
								if( !evs_one_game_flg ){	//	�ȊO�X�g�[���[EXTRA
									state[j] -> game_condition[dm_mode_now]	= dm_cnd_pause_tar_re_sc;		//	��Ԃ� PAUSE & TARGET & RETRY & SCORE �ɐݒ肷��
								}
							}
						}else{
							state[j] -> game_condition[dm_mode_now]		= dm_cnd_pause_tar;				//	��Ԃ� PAUSE & TARGET �ɐݒ肷��
							state[j] -> game_mode[dm_mode_now]			= dm_mode_pause;				//	������ PAUSE �� �ݒ肷��
						}
					}
					break;
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_pause ){
					auSeqpVolumeInit();	//	�{�����[����߂�
					for( j = 0;j < 4;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;								//	��ΓI�R���f�B�V������ʏ��Ԃɐݒ肷��
						state[j] -> game_condition[dm_mode_now]		=	state[j] -> game_condition[dm_mode_old];	//	�O�̏�Ԃɖ߂�
						state[j] -> game_mode[dm_mode_now]			=	state[j] -> game_mode[dm_mode_old];			//	�O�̏����ɖ߂�
					}
					break;
				}
			}else	if( flg[i] == dm_ret_retry ){	//	���g���C
				return	dm_ret_retry;
			}else	if( flg[i] == dm_ret_game_end ){
				return	dm_ret_game_end;
			}else	if( flg[i] == dm_ret_end ){		//	�I��
				return	-1;
			}else	if( flg[i] == dm_ret_tr_a ){	//	���K�U������
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;		//	��ΓI�R���f�B�V������ �ʏ� �ɐݒ肷��
				state[i] -> retire_flg[dm_training_flg] 	=	0;					//	�U���i��e�j�����ɐݒ肷��
				state[i] -> game_condition[dm_training_flg] =	dm_cnd_training;	//	��Ԃ���K�ɐݒ肷��
				state[i] -> game_mode[dm_mode_now]			=	dm_mode_training;	//	��������K�ɐݒ肷��
			}else	if( flg[i] == dm_ret_tr_b ){	//	���K�U������
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;		//	��ΓI�R���f�B�V������ �ʏ� �ɐݒ肷��
				state[i] -> retire_flg[dm_training_flg] 	=	1;					//	�U���i��e�j�L��ɐݒ肷��
				state[i] -> game_condition[dm_training_flg] =	dm_cnd_training;	//	��Ԃ���K�ɐݒ肷��
				state[i] -> game_mode[dm_mode_now]			=	dm_mode_training;	//	��������K�ɐݒ肷��
			}
		}
		if( win_flg ){
			//	�N�����N���A����
			for(j = k = l = 0;j < 4;j++ ){
				if( state[j] -> game_condition[dm_static_cnd] == dm_cnd_win ){
					//	�N���A�l���𒲂ׂ�
					win_team = state[j] -> player_state[PS_TEAM_FLG];	//	�����`�[���̐ݒ�
					k ++;
				}
			}

			if( k == 1 ){	//	�ЂƂ肾���N���A
				if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){
							//	�����҂Ɠ����`�[���̕�
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	��ΓI�R���f�B�V������ WIN �ɐݒ肷��
							state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;			//	������ WIN �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){		//	���҂Ƀv���C���[�������ꍇ
								sound = SEQ_VSEnd;													//	���s�m�艹�̐ݒ�
								if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;	//	�\���� WIN & ANY KEY & SCORE �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	������ WIN �ɐݒ肷��
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key_sc;	//	�\���� WIN & ANY KEY & SCORE �ɐݒ肷��
								}
							}else{
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;				//	�\����	WIN �ɐݒ肷��
							}
							win_main( center[j] ,j );												//	WIN ���o������
						}else{
							//	�����`�[��
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;			//	��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	�s�҂Ƀv���C���[�������ꍇ
								//	���g���C��t�t��
								sound = SEQ_Lose;													//	�s�m�艹�̐ݒ�
								if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;	//	�\���� LOSE & RETRY  �ɐݒ肷��
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry_sc;	//	�\���� LOSE & RETRY & SCORE �ɐݒ肷��
								}
								state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
							}else{
								state[j] -> game_condition[dm_mode_now] = dm_cnd_lose;				//	�\���� LOSE �ɐݒ肷��
								state[j] -> game_mode[dm_mode_now]		= dm_mode_lose;				//	������ LOSE �ɐݒ肷��
							}
							lose_main( j,center[j] );												//	LOSE ���o������
						}
					}
				}else{	//	����ȊO�̏ꍇ
					for(j = 0;j < 4;j++){
						if( state[j] -> game_condition[dm_static_cnd] == dm_cnd_win ){
							win_count[ state[j] -> player_state[PS_TEAM_FLG]] ++;				//	�����`�[���̃J�E���g��ǉ�����(�`�[���΍R��łȂ��ꍇ�̓`�[���ԍ����ʁX�Ȃ̂Ŗ��Ȃ�)
						}
					}
					for(j = end_flg = 0;j < 4;j++ )
					{
						if( win_count[j] == WINMAX ){	//	�������t����( �R�{��� )
							end_flg = 1;				//	�������t�����t���O�𗧂Ă�
							sound = SEQ_VSEnd;
						}else{
							sound = SEQ_Win;
						}
					}

					if( end_flg ){	//	���s�����܂��Ă����ꍇ(�N����3�{�����)
						for(j = 0;j < 4;j++ )
						{
							if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	�����������̂Ɠ����`�[���Ȃ�
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN �������ꍇ
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;	//	�\���� WIN & RETRY �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;	//	������ WIN & RETRY �ɐݒ肷��
								}else{																//	CPU �������ꍇ
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win;			//	�\���� WIN �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win;		//	������ WIN �ɐݒ肷��
								}
								win_main( center[j] ,j);											//	WIN ���o������
							}else{
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN �������ꍇ

									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;	//	�\���� LOSE & RETRY �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;	//	������ LOSE & RETRY �ɐݒ肷��
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;		//	�\���� LOSE �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose;		//	������ LOSE �ɐݒ肷��
								}
								lose_main( j,center[j] );											//	LOSE ���o������
							}
						}
					}else{		//	���s�����܂��Ă��Ȃ��ꍇ
						for(j = 0;j < 4;j++ )
						{
							if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	�����������̂Ɠ����`�[���Ȃ�
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN �̏ꍇ
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;	//	�\���� WIN & ANY KEY �ɐݒ肷��
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;			//	�\���� WIN �ɐݒ肷��
								}
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;		//	������ WIN �ɐݒ肷��
								win_main( center[j],j );											//	WIN ���o������
							}else{
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN �̏ꍇ
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;//	�\���� LOSE & ANY KEY �ɐݒ肷��
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;		//	�\���� LOSE �ɐݒ肷��
								}
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_lose;		//	������ LOSE �ɐݒ肷��
								lose_main( j,center[j] );											//	LOSE ���o������
							}
						}
					}
				}
			}else{	//	���l���̏ꍇ�́ADRAW
				for(j = 0;j < 4;j++ )
				{
					sound = SEQ_Lose;
					if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN �̏ꍇ
						state[j] -> game_condition[dm_mode_now]	=	dm_cnd_draw_any_key;	//	�\���� DRAW & ANY KEY �ɐݒ肷��
					}else{
						state[j] -> game_condition[dm_mode_now]	=	dm_cnd_draw;			//	�\���� DRAW �ɐݒ肷��
					}
					state[j] -> game_mode[dm_mode_now]			=	dm_mode_draw;			//	������ DRAW �ɐݒ肷��
					draw_main( j,center[j] );												//	DRAW ���o������
				}
			}
			dm_play_bgm_set( sound );	//	BGM �̐ݒ�
		}
		if( game_over_flg ){
			//	�Q�[���I�[�o�[�̒m�点����������
			for(j = k = l = bit = 0;j < 4;j++ ){
				if( state[j] -> retire_flg[dm_retire_flg] ){
					//	���^�C�A���Ă���
					bit |= 1 << j;	//	�r�b�g�𗧂Ă�
					k ++;
				}
			}
			if( k == 4 ){
				//	�S�����^�C�A(DRAW)
				dm_play_bgm_set( SEQ_Lose );
				for(j = 0;j < 4;j++ )
				{
					if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN �̏ꍇ
						state[j] -> game_condition[dm_mode_now] = dm_cnd_draw_any_key;	//	�\���� DRAW & ANY KEY �ɐݒ肷��
					}else{
						state[j] -> game_condition[dm_mode_now] = dm_cnd_draw;			//	�\���� DRAW �ɐݒ肷��
					}
					state[j] -> game_mode[dm_mode_now] = dm_mode_draw;					//	������ DRAW �ɐݒ肷��
					draw_main( j,center[j] );											//	DRAW ���o������
				}
				return	cap_flg_off;
			}else{
				end_flg = 0;			//	���������܂��Ă��Ȃ��ɐݒ肵�Ă���
				if( !dm_game_4p_team_flg ){	//	�o�g�����C����( 1P vs 2P vs 3P vs 4P )�̏ꍇ
					if( k == 3 ){	//	���^�C�A�� �R�l�̂Ƃ�
						for(j = 0;j < 4;j++){
							if( !state[j] -> retire_flg[dm_retire_flg] ){	//	���^�C�A���Ă��Ȃ�����
								l = j;
								win_count[ state[j] -> player_state[PS_TEAM_FLG] ] ++;	//	�����J�E���g�𑝂₷
								win_team = state[j] -> player_state[PS_TEAM_FLG];		//	�����`�[���ԍ��̐ݒ�
							}
						}
						end_flg = 2;			//	���������܂��Ă��Ȃ��ɐݒ肵�Ă���
						for(j = 0;j < 4;j++ )
						{
							if( win_count[j] == WINMAX ){	//	�N�����R�{�Ƃ��Ă����ꍇ
								end_flg = 1;	//	���������܂����ɐݒ肷��
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Win;
							}
						}
					}
				}else{	//	�`�[���΍R��	�̏ꍇ
					end_flg = 0;
					if( ( bit & dm_game_4p_team_bits[TEAM_MARIO] ) == dm_game_4p_team_bits[TEAM_MARIO] ){	//	�`�`�[���̑S�������^�C�A�����ꍇ
						win_count[TEAM_ENEMY] ++;						//	ENEMY �`�[���̎擾�|�C���g���Z
						win_team = TEAM_ENEMY;							//	�����`�[���̐ݒ������
						end_flg = 2;									//	�P�����I���̐ݒ������
					}else	if( ( bit & dm_game_4p_team_bits[TEAM_ENEMY] ) == dm_game_4p_team_bits[TEAM_ENEMY] ){	//	�a�`�[���̑S�������^�C�A�����ꍇ
						win_count[TEAM_MARIO] ++;						//	MARIO �`�[���̎擾�|�C���g���Z
						win_team = TEAM_MARIO;							//	�����`�[���̐ݒ������
						end_flg = 2;									//	�P�����I���̐ݒ������
					}

					if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
						if( end_flg ){	//	end_flg �� 0 �ȊO�̏ꍇ�������t���Ă��邱�ƂɂȂ�
							end_flg = 1;					//	�����I���̐ݒ������
							if( win_count[TEAM_MARIO] ){	//	�v���C���[�`�[�������������ꍇ
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Lose;
							}
						}
					}else{		//	����ȊO�̏ꍇ
						for(j = 0;j < 4;j++ )
						{
							if( win_count[j] == WINMAX ){	//	���s�����܂����ꍇ
								end_flg = 1;				//	�����I���̐ݒ������
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Win;
							}
						}
					}
				}
				if( end_flg == 1 ){	//	�����I��( 3�{��� )�̏ꍇ
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	���^�C�A���Ă��Ȃ����̂Ɠ����`�[���Ȃ�
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	��ΓI�R���f�B�V������ WIN �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN �̏ꍇ
								if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
									if( evs_one_game_flg ){	//	�X�g�[���[���[�h�̖ʃZ���N�g�̏ꍇ
										state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ肷��
										state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ肷��
									}else{
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key_sc;	//	�\���� WIN & ANY KEY & SCORE �ɐݒ肷��
										state[j] -> game_mode[dm_mode_now]		=	dm_mode_win;			//	������ WIN �ɐݒ肷��
									}
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	�\���� WIN & RETRY �ɐݒ肷��
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	������ WIN & RETRY �ɐݒ肷��
								}
							}else{														//	CPU �̏ꍇ
								state[j] -> game_condition[dm_mode_now]		=	dm_cnd_win;				//	�\���� WIN �ɐݒ肷��
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;			//	������ WIN �ɐݒ肷��
							}
							win_main( center[j],j );													//	WIN ���o������
						}else{
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;				//	��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN �̏ꍇ
								if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
									if( evs_one_game_flg ){	//	�X�g�[���[EXTRA
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
									}else{
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry_sc;	//	�\���� LOSE & RETRY & SCORE �ɐݒ肷��
									}
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	�\���� LOSE & RETRY �ɐݒ肷��
								}
								state[j] -> game_mode[dm_mode_now] 			=	dm_mode_lose_retry;		//	������ LOSE & RETRY �ɐݒ肷��
							}else{
								state[j] -> game_condition[dm_mode_now]		=	dm_cnd_lose;			//	�\���� LOSE �ɐݒ肷��
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_lose;			//	������ LOSE �ɐݒ肷��
							}
							lose_main( j,center[j] );													//	LOSE ���o������
						}
					}
				}else	if( end_flg == 2 ){	//	1�����I���̏ꍇ
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	���^�C�A���Ă��Ȃ����̂Ɠ����`�[���Ȃ�
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	��ΓI�R���f�B�V������ WIN �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){				//	MAN �̏ꍇ
								state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key;		//	�\���� WIN & ANY KEY �ɐݒ肷��
							}else{																	//	CPU�̏ꍇ
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;				//	�\���� WIN �ɐݒ肷��
							}
							state[j] -> game_mode[dm_mode_now] 			=	dm_mode_win;			//	������ WIN �ɐݒ肷��
							win_main( center[j],j );												//	WIN ���o������
						}else{
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;			//	��ΓI�R���f�B�V������ LOSE �ɐݒ肷��
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){				//	MAN�̏ꍇ
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	�\���� LOSE & ANY KEY �ɐݒ肷��
							}else{																	//	CPU�̏ꍇ
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;			//	�\���� LOSE �ɐݒ肷��
							}
							state[j] -> game_mode[dm_mode_now]			= dm_mode_lose;				//	������ LOSE �ɐݒ肷��
							lose_main( j,center[j] );
						}
					}
				}

				if( end_flg != 0 ){
					dm_play_bgm_set( sound );
				}
				return	cap_flg_off;
			}
		}
	}

	return	cap_flg_off;
}


/*--------------------------------------
	�Q�[���f�������֐�(1P�p)
--------------------------------------*/
s8	dm_game_demo_1p(void)
{
	s8	i,flg;

	flg = dm_game_main_cnt_1P( &game_state_data[0],game_map_data[0],0,0 );
	dm_anime_control( &game_state_data[0].anime );	//	�A�j���[�V����
	for(i = 0;i < 3;i++){
		dm_anime_control( &big_virus_anime[i] );	//	����E�C���X�A�j���[�V����
	}


	if( flg == dm_ret_virus_wait ){	//	�E�C���X�z�u����

		if( dm_start_time > 0 ){
			dm_start_time --;
		}else{
			dm_demo_time_flg = 1;
			game_state_data[0].game_mode[dm_mode_now] = dm_mode_throw;	//�J�v�Z�������J�n
			if( game_state_data[0].player_state[0] == PUF_PlayerCPU ){
				aifMakeFlagSet( &game_state_data[0] );
			}
		}
	}
	disp_anime_data[0][wb_flag ^ 1] = game_state_data[0].anime;	//	�`��p�f�[�^�ɈڐA

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	�P��
			return	cap_flg_on;		//	�f�����f
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	�f�����f
			}
		}
	}

	return	cap_flg_off;
}

/*--------------------------------------
	�Q�[���f�������֐�(2P�p)
--------------------------------------*/
s8	dm_game_demo_2p(void)
{
	s8	i,j,l;
	s8	flg[2];


	for( i = 0;i < 2;i++ ){
		flg[i] = dm_game_main_cnt( &game_state_data[i],game_map_data[i],i,1 );
		dm_anime_control( &game_state_data[i].anime );					//	�A�j���[�V����
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait ){

		//	�E�C���X�ݒ芮��
		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			dm_demo_time_flg = 1;
			for( i = 0;i < 2;i ++ ){
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;	//�J�v�Z�������J�n
				//	�����O�v�l�ǉ��ʒu
				if( game_state_data[i].player_state[0] == PUF_PlayerCPU ){
					aifMakeFlagSet( &game_state_data[i] );
				}
			}
		}
	}
	for(i = 0;i < 2;i++){
		disp_anime_data[i][wb_flag ^ 1] = game_state_data[i].anime;	//	�`��p�f�[�^�ɈڐA
	}

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	�P��
			return	cap_flg_on;		//	�f�����f
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	�f�����f
			}
		}
	}

	return	cap_flg_off;
}
/*--------------------------------------
	�Q�[���f�������֐�(4P�p)
--------------------------------------*/
s8	dm_game_demo_4p(void)
{
	u8	i;
	s8	flg[4];

	for( i = 0;i < 4;i++ ){
		flg[i] = dm_game_main_cnt( &game_state_data[i],game_map_data[i],i,2 );
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait &&
		flg[2] == dm_ret_virus_wait && flg[3] == dm_ret_virus_wait  ){

		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			//	�E�C���X�ݒ芮��
			dm_demo_time_flg = 1;
			for( i = 0;i < 4;i ++ ){
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;	//�J�v�Z�������J�n
				//	�����O�v�l�ǉ��ʒu
				if( game_state_data[i].player_state[0] == PUF_PlayerCPU ){
					aifMakeFlagSet( &game_state_data[i] );
				}
			}
		}
	}

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	�P��
			return	cap_flg_on;		//	�f�����f
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	�f�����f
			}
		}
	}

	return	cap_flg_off;

}
/*--------------------------------------
	�Q�[���`��֐��}�b�v�`��
--------------------------------------*/
void	dm_map_draw(game_map *map,u8 col_no,s16 x_p,s16 y_p,s8 size)
{
	u8	i,j;

	for( i = 0;i < 128;i++ ){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
			//	�E�C���X(�J�v�Z��)�̕\���t���O���n�m�̂Ƃ�
			if( (map + i) -> capsel_m_p == col_no ){
				//	�F�i�q�E�a�E�x�j���������Ƃ�
				gSPTextureRectangle(gp++,(map + i) -> pos_m_x * size + x_p << 2,(map + i) -> pos_m_y * size + y_p << 2,
												(map + i) -> pos_m_x * size + x_p + size << 2,(map + i) -> pos_m_y * size + y_p + size << 2,
														G_TX_RENDERTILE, 0, (map + i) -> capsel_m_g * size << 5,1 << 10,1 << 10);
			}
		}
	}
}
/*--------------------------------------
	(������)�`��֐�
--------------------------------------*/
void	dm_star_draw(s16 x_pos ,s16 y_pos, u8 win ,s8 flg)
{

	s8 i;

	if( !flg ){	//	���Ȃ��
		for( i = 0;i < win && i < 3;i++ ){
			draw_Tex( x_pos,y_pos,16,16,0,win_anime_count[0] << 4);
			x_pos += 22;
		}
	}else	if( flg ){	//	�c�Ȃ��
		for( i = 0;i < win && i < 3;i++ ){
			draw_Tex( x_pos,y_pos,16,16,0,win_anime_count[0] << 4);
			y_pos += 20;
		}
	}
}


/*--------------------------------------
	������ԃJ�v�Z������]�֐�
--------------------------------------*/
void	throw_rotate_capsel( game_cap *cap )
{
	s8	vec;
	u8	i,save;
	u8	rotate_table[] = {1,3,4,2};
	u8	rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};

	if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	�J�v�Z�����c�ɂȂ��Ă����ꍇ
		cap -> pos_x[1] ++;
		cap -> pos_y[1] ++;
		save = cap->capsel_p[0];
		cap->capsel_p[0] = cap->capsel_p[1];
		cap->capsel_p[1] = save;
		vec = 1;
	}else{	//	�J�v�Z�������ɂȂ��Ă����ꍇ
		//	���W�ύX
		cap -> pos_x[1] --;
		cap -> pos_y[1] --;
		vec = -1;
	}
	//	�O���t�B�b�N�̓���ւ�
	for(i = 0;i < 2;i++){
		save = rotate_table[cap -> capsel_g[i]];
		save += vec;
		cap -> capsel_g[i] = rotate_mtx[save];
	}
}

/*--------------------------------------
	�E�C���X���`��֐�
--------------------------------------*/
void	dm_disp_virus_number( game_state *state,s16 x_pos, s16 y_pos,s8 player_no )
{
	s8	i;
	u8	fspd[4] = { 2,8,6,4};
	float	fa;

	//	�����p���b�g�f�[�^�̓Ǎ���
	if( state -> virus_number < 4 ){
		load_TexPal( TLUT_virus_number_r );	//	�Ԃ��F
	}else{
		load_TexPal( TLUT_virus_number );	//	�ʏ�F
	}
	if( state -> virus_number < 10 ){	//	�E�C���X�̐����P��
		x_pos += 6;
		if( state -> virus_number > 3 ){	//	�S�ȏ�͒ʏ�\��
			virsu_num_size[player_no][0] = 12;
			virsu_num_size[player_no][1] = 16;
			virsu_num_size[player_no][2] = 1;
		}else{								//	�R�ȉ��͊g�k�\��
			virsu_num_size[player_no][2] += fspd[state -> virus_number];
			if( virsu_num_size[player_no][2] > 360 ){
				virsu_num_size[player_no][2] -= 360;
			}
			fa = sinf( DEGREE( virsu_num_size[player_no][2] ) );
			virsu_num_size[player_no][0] = (s16)(16 + (fa * 5 ));
			virsu_num_size[player_no][1] = (s16)(16 + (fa * 5 ));

			for( i = 0;i < 2;i++ ){
				if( virsu_num_size[player_no][i] % 2 ){
					virsu_num_size[player_no][i] ++;
				}
			}

			x_pos += 6;
			x_pos -= virsu_num_size[player_no][0] >> 1;
			y_pos += 8;
			y_pos -= virsu_num_size[player_no][1] >> 1;
		}
		draw_ScaleTex( x_pos,y_pos,12,16,virsu_num_size[player_no][0],virsu_num_size[player_no][1],0,state -> virus_number << 4 );
	}else{
		draw_Tex( x_pos,y_pos,12,16,0,(state -> virus_number / 10) << 4 );
		draw_Tex( x_pos + 12,y_pos,12,16,0,(state -> virus_number % 10) << 4 );
	}
}
/*--------------------------------------
	�����`��֐�
--------------------------------------*/
void	dm_disp_number( s16 num,s16 x_pos, s16 y_pos ,u16 *pal)
{
	s8	i;

	//	�����O���t�B�b�N�f�[�^�͓ǂݍ���ł�����̂Ƃ���

	//	�����p���b�g�f�[�^�̓Ǎ���
	load_TexPal( pal );

	if( num < 10 ){
		x_pos += 6;
		draw_Tex( x_pos,y_pos,12,16,0,num << 4 );
	}else{
		draw_Tex( x_pos,y_pos,12,16,0,(num / 10) << 4 );
		draw_Tex( x_pos + 12,y_pos,12,16,0,(num % 10) << 4 );
	}
}

/*--------------------------------------
	���_�`��֐�
--------------------------------------*/
void	dm_disp_score( u32	score,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[7];

	//	�����p���b�g�f�[�^�̓Ǎ���
	load_TexPal( lv_num_bm0_0tlut );
	//	�����O���t�B�b�N�f�[�^�̓Ǎ���
	load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );

	num[6] = score / 1000000;
	num[5] = (score % 1000000) / 100000;
	num[4] = (score % 100000) / 10000;
	num[3] = (score % 10000) / 1000;
	num[2] = (score % 1000) / 100;
	num[1] = (score % 100) / 10;
	num[0] = score % 10;

	for( i = 0;i < 7;i++ ){
		x_pos -= 8;
		draw_Tex( x_pos,y_pos,8,12,0,num[i] * 12 );
	}
}
/*--------------------------------------
	SCORE �����t�����_�`��֐�
--------------------------------------*/
void	dm_disp_score_plus( game_state *state )
{
	u8	x_p,y_p;
	u8	s_xp,s_yp;

	if( evs_gamesel == GSL_4PLAY ){
		x_p = 14;
		y_p = 10;
		s_xp = 60;
		s_yp = 21;
	}else{
		x_p = 21;
		y_p = 12;
		s_xp = 68;
		s_yp = 24;
	}


	//	SCORE
	load_TexPal( dm_4p_game_target_base_bm0_0tlut );
	load_TexTile_4b( dm_4p_game_score_bm0_0,36,10,0,0,35,9 );
	draw_Tex( state -> map_x + x_p,state -> map_y + y_p,36,10,0,0 );

	//	���_
	dm_disp_score( state -> game_score,state -> map_x + s_xp,state -> map_y + s_yp );
}
/*--------------------------------------
	�|�C���g�`��֐�
--------------------------------------*/
void	dm_disp_point( s16	point,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[3];

	if( point > 999 ){
		point = 999;
	}
	num[2] = point / 100;
	num[1] = (point % 100) / 10;
	num[0] = point % 10;

	for( i = 0;i < 3;i++ ){
		x_pos -= 8;
		draw_Tex( x_pos,y_pos,8,12,0,num[i] * 12 );
	}
}
/*--------------------------------------
	���Z���_�`��֐�
--------------------------------------*/
void	dm_disp_add_score( void )
{
	s8	i,j;


	gDPSetRenderMode( gp++, G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2);
	for( i = 0;i < 10;i++ ){
		if( lv_sc_pos[i].flg ){	//	�g�p���̏ꍇ
			for( j = 0;j < lv_sc_pos[i].num_max;j++ )
			{
				//	�����x�̐ݒ�
				if( lv_sc_pos[i].num_alpha[j] ){	//	0�ȊO�̂Ƃ�
					gDPSetPrimColor( gp++,0,0,255,255,255,lv_sc_pos[i].num_alpha[j] );
					draw_Tex( lv_sc_pos[i].score_pos[j][0],66 - lv_sc_pos[i].score_pos[j][1],8,12,0,lv_sc_pos[i].num_g[j] * 12 );
				}
			}
		}
	}
	gDPPipeSync(gp++);
	gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

}


/*--------------------------------------
	���ԕ`��֐�
--------------------------------------*/
void	dm_disp_time( u32 time,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[6];

	//	�����O���t�B�b�N�f�[�^�͓ǂݍ���ł�����̂Ƃ���

	//	�����p���b�g�f�[�^�̓Ǎ���
	load_TexPal( TLUT_virus_number );

	num[5] = time / 3600;
	num[4] = (time % 3600) / 60;

	num[3] = num[5] / 10;
	num[2] = num[5] % 10;
	num[1] = num[4] / 10;
	num[0] = num[4] % 10;

	x_pos -= 12;
	//	�b
	for( i = 0;i < 2;i++ ){
		draw_Tex( x_pos,y_pos,12,16,0,num[i] << 4 );
		x_pos -= 12;
	}
	x_pos -= 6;
	//	��
	for( i = 2;i < 4;i++ ){
		draw_Tex( x_pos,y_pos,12,16,0,num[i] << 4 );
		x_pos -= 12;
	}
}

/*--------------------------------------
	�X�g�[���[�S�o���p ���ԁE���x���E�X�e�[�W �`��֐�
--------------------------------------*/
void	dm_disp_time_4p( u32 time,u8 level )
{
	s8	i,j;
	s8 num[6];
	s16	x_pos;

	//	�����p���b�g�f�[�^�̓Ǎ���
	load_TexPal( dm_4p_game_level_bm0_0tlut );

	//	���x���`��
	load_TexTile_4b( &dm_4p_game_level_bm0_0[0],28,15,0,0,27,14 );
	draw_Tex( 194,30,28,5,0,level * 5);

	//	�����O���t�B�b�N�f�[�^�̓Ǎ���
	load_TexTile_4b( &dm_story_num_bm0_0[0],4,55,0,0,3,54 );

	//	�X�e�[�W�`��
	draw_Tex( 160,30,4,5,0,7 * 5 );

	//	���ԕ`��
	draw_Tex( 113,30,4,5,0,10 * 5 );	//	�R���}

	num[5] = time / 3600;
	num[4] = (time % 3600) / 60;

	num[3] = num[5] / 10;
	num[2] = num[5] % 10;
	num[1] = num[4] / 10;
	num[0] = num[4] % 10;

	//	�b
	x_pos = 122;
	for( i = 0;i < 2;i++ ){
		draw_Tex( x_pos,30,4,5,0,num[i] * 5 );
		x_pos -= 5;
	}
	x_pos -= 4;
	//	��
	for( i = 2;i < 4;i++ ){
		draw_Tex( x_pos,30,4,5,0,num[i] * 5 );
		x_pos -= 5;
	}
}


/*--------------------------------------
	4PLAY �p��`��֐�
--------------------------------------*/
void	dm_4p_game_face_draw( game_state *state,s16 x_pos,s16 y_pos )
{
	//	��p���b�g�f�[�^�̓Ǎ���
	load_TexPal( face_4p_game_pal_data[state -> anime.cnt_charcter_no] );

	//	��f�[�^�̓Ǎ���
	load_TexBlock_4b( face_4p_game_data[state -> anime.cnt_charcter_no],16,16 );

	draw_Tex( x_pos,y_pos,16,16,0,0 );
}

/*--------------------------------------
	1PLAY �p���x�`��֐�
--------------------------------------*/
void	dm_speed_draw( game_state *state,s16 x_pos,s16 y_pos )
{
	//	SPEED
	load_TexPal( dm_data_mode_game_spead_bm0_0tlut );
	load_TexTile_4b( dm_data_mode_game_spead_bm0_0, 24,48,0,0,23,47 );

	draw_Tex( x_pos,y_pos,24,16,0,(state -> cap_def_speed) << 4 );
}

/*--------------------------------------
	�Q�[���`��֐�
--------------------------------------*/
void	dm_game_graphic_p( game_state *state, game_map *map )
{
	s16	i,j,x_p,y_p;
	s8	size_flg = 0;
	u8	cap_size[] = {220,128};

	//	�|�[�Y���͕`�悵�Ȃ�
	if( state -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		if( state -> map_item_size == cap_size_8 ){
			size_flg = 1;
		}

		//	�E�C���X�O���t�B�b�N�f�[�^�̓Ǎ���
		load_TexTile_4b( cap_tex[size_flg],16,cap_size[size_flg],0,0,15,cap_size[size_flg] - 1 );


#ifndef	DM_S2D_MAIN

		//	�E�C���X�`��
		for( i = 0;i < 6;i++ ){
			//	�E�C���X�p���b�g�f�[�^�̓Ǎ���
			load_TexPal( cap_pal[size_flg][i] );
			dm_map_draw( map ,i,state -> map_x,state -> map_y,state -> map_item_size );
		}
#endif

		//	����J�v�Z���`��(�����ŕ`�����̂́A�Q�[���̌������t�����Ƃ��̂�)
		if( state -> now_cap.capsel_flg[cap_disp_flg] ){	//	�\���t���O�������Ă��邩�H
			//	�����Ă����ꍇ
			if( state -> now_cap.pos_y[0] < 1 ){
				for( i = 0;i < 2;i++ ){
					//	�p���b�g���[�h
					load_TexPal( cap_pal[size_flg][state -> now_cap.capsel_p[i]] );
					//	�J�v�Z���`��
					draw_Tex( (state -> now_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
							(state -> now_cap.pos_y[i] * state -> map_item_size) + state -> map_y - 10,
								state -> map_item_size,state -> map_item_size,0,state -> now_cap.capsel_g[i] * state -> map_item_size );
				}
			}else	if( state -> game_condition[dm_mode_now] != dm_cnd_pause && state -> game_condition[dm_mode_now] != dm_cnd_wait ){
				for( i = 0;i < 2;i++ ){
					//	�p���b�g���[�h
					load_TexPal( cap_pal[size_flg][state -> now_cap.capsel_p[i]] );
					//	�J�v�Z���`��
					draw_Tex( (state -> now_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
							(state -> now_cap.pos_y[i] * state -> map_item_size) + state -> map_y + 1,
								state -> map_item_size,state -> map_item_size,0,state -> now_cap.capsel_g[i] * state -> map_item_size );
				}
			}
		}

		//	NEXT�J�v�Z���`��
		if( state -> next_cap.capsel_flg[cap_disp_flg] ){	//	�\���t���O�������Ă��邩�H
			//	�����Ă����ꍇ
			if( state -> now_cap.pos_y[0] > 0 ){	//	����J�v�Z����NEXT�̍����ɂ��邩�H
				//	�������Ă���
				for( i = 0;i < 2;i++ ){
					//	�p���b�g���[�h
					load_TexPal( cap_pal[size_flg][state -> next_cap.capsel_p[i]] );
					//	�J�v�Z���`��
					draw_Tex( (state -> next_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
								(state -> next_cap.pos_y[i] * state -> map_item_size) + state -> map_y - 10,
									state -> map_item_size,state -> map_item_size,0,state -> next_cap.capsel_g[i] * state -> map_item_size );
				}
			}
		}
	}
}

/*--------------------------------------
	�Q�[���`��֐�(1P�p)
--------------------------------------*/
void	dm_game_graphic_1p( game_state *state, game_map *map )
{
	s16	i,j,x_p,y_p;


	//	�|�[�Y���͕`�悵�Ȃ�
	if( state -> game_condition[dm_static_cnd] != dm_cnd_pause ){

		//	�E�C���X�O���t�B�b�N�f�[�^�̓Ǎ���
		load_TexTile_4b( &TEX_mitem10r_0_0[0],16,220,0,0,15,219 );
#ifndef	DM_S2D_MAIN
		//	�E�C���X�`��
		for( i = 0;i < 3;i++ ){
			//	�E�C���X�p���b�g�f�[�^�̓Ǎ���
			load_TexPal(capsel_pal[i]);
			dm_map_draw( map ,i,state -> map_x,state -> map_y,state -> map_item_size );
		}

#endif
		//	����J�v�Z���`��
		if( state -> now_cap.capsel_flg[cap_disp_flg] ){	//	�\���t���O�������Ă��邩�H
			//	��P���̂Ƃ�����
			if( state -> game_mode[dm_mode_now] == dm_mode_wait ){
				for( i = 0;i < 2;i++ ){
					load_TexPal( capsel_pal[state -> now_cap.capsel_p[i]] );	//	�p���b�g���[�h
					//	�J�v�Z���`��

					draw_Tex( i * cap_size_10 + 222,77,
								cap_size_10,cap_size_10,0,state -> now_cap.capsel_g[i] * cap_size_10);
				}
			}
		}

		//	NEXT�J�v�Z���`��
		if( state -> next_cap.capsel_flg[cap_disp_flg] ){	//	�\���t���O�������Ă��邩�H
			//	�����Ă����ꍇ
			if( state -> now_cap.pos_y[0] > 0 ){	//	����J�v�Z����NEXT�̍����ɂ��邩�H
				//	�������Ă���
				for( i = 0;i < 2;i++ ){
					load_TexPal( capsel_pal[state -> next_cap.capsel_p[i]] );	//	�p���b�g���[�h
					//	�J�v�Z���`��

					draw_Tex( i * cap_size_10 + 222,77,
								cap_size_10,cap_size_10,0,state -> next_cap.capsel_g[i] * cap_size_10);
				}
			}
		}

	}
}
/*--------------------------------------
	���g���C�`��֐�
--------------------------------------*/
void	retry_draw( game_state *state ,s8 palyer_no )
{
	u8	allow_pos[] = {4,18};
	u8	x_p,y_p;

	if( evs_gamesel == GSL_4PLAY ){
		x_p = 2;
		y_p = 153;
	}else{
		x_p = 10;
		y_p = 180;
	}

	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	//	���g���C�v���[�g�̕`��
	if( dm_retry_flg ){
		//	���g���C�\
		load_TexPal( dm_retry_bm0_0tlut );
	}else{
		//	���g���C�s�\
		load_TexPal( dm_no_retry_bm0_0tlut );
	}
	load_TexTile_4b( dm_retry_bm0_0,60,34,0,0,59,33 );
	draw_Tex( state -> map_x + x_p,y_p,60,34,0,0 );

//	dm_retry_position

	//	���̕`��
	gDPSetPrimColor( gp++,0,0,dm_retry_alpha[0],dm_retry_alpha[0],dm_retry_alpha[0],255);
	load_TexPal( arrow_r_bm0_0tlut );
	load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );
	draw_Tex( state -> map_x + x_p + 2 + dm_retry_pos[0],y_p + allow_pos[dm_retry_position[palyer_no][0]],8,11,0,0 );
}
/*--------------------------------------
	�S�o���̊珈���֐�
--------------------------------------*/
void	demo_story_4p_main(void)
{
	s8	i,j;

	switch( story_4p_demo_flg[1] )
	{
	case	0:	//	�g��
		story_4p_demo_w[0] += 10;
		if( story_4p_demo_w[0] >= 192 ){
			story_4p_demo_w[0] = 192;
			story_4p_demo_flg[1] = 1;
		}
		S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,story_4p_demo_w[0],58,159 - (story_4p_demo_w[0] / 2 ),102,0,0 );
		break;
	case	1:	//	�L�[�҂�
		for(i = 0;i < 4;i++){
			if( joyupd[i] & DM_ANY_KEY ){
				story_4p_demo_h[0] = 58;
				story_4p_demo_flg[1] = 2;
			}
		}
		break;
	case	2:	//	�k��
		story_4p_demo_h[0] -= 8;
		if( story_4p_demo_h[0] <= 2 ){
			story_4p_demo_h[0] = 2;
			story_4p_demo_flg[1] = 0;	//	�����I��
			story_4p_demo_flg[0] = 0;	//	�����I��
		}
		S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,192,story_4p_demo_h[0],63,131 - (story_4p_demo_h[0] / 2),0,0 );
		break;
	}
}

/*--------------------------------------
	�S�o���̊�`��֐�
--------------------------------------*/
void	demo_story_4p_draw(void)
{
	s8	i,j;
	s16	w_size;

	//	���
	load_TexPal( story_4p_demo_bg_tlut );
 	gSPBgRect1Cyc( gp++, &s2d_demo_4p );

	//	PUSH ANY KEY
	if( story_4p_demo_flg[1] == 1 ){
		push_any_key_draw( 128 ,165 );

		load_TexPal( dm_4p_story_gage_bm0_0tlut );	//	������ �p���b�g���[�h
		load_TexTile_4b( &dm_4p_story_gage_bm0_0[0],154,24,0,0,153,23 );	//	������ �O���t�B�b�N���[�h

		//	����`��
		for(i = 0;i < 4;i++ ){
			draw_Tex( story_4p_demo_face_pos[i],124,24,24,130,0);
		}

		//	��`��
		for(i = j = 0;i < 4;i++ ){
			if( !game_state_data[i].player_state[PS_TEAM_FLG] ){
				//	��p���b�g�f�[�^�̓Ǎ���
				load_TexPal( face_4p_game_pal_data[game_state_data[i].anime.cnt_charcter_no] );
				//	��f�[�^�̓Ǎ���
				load_TexBlock_4b( face_4p_game_data[game_state_data[i].anime.cnt_charcter_no],16,16 );
				draw_TexFlip( story_4p_demo_face_pos[j] + 5,127,16,16,0,0,flip_on,flip_off);
				j++;
			}
		}
		//	��`��
		for(i = 0;i < 4;i++ ){
			if( game_state_data[i].player_state[PS_TEAM_FLG] ){
				//	��p���b�g�f�[�^�̓Ǎ���
				load_TexPal( face_4p_game_pal_data[game_state_data[i].anime.cnt_charcter_no] );
				//	��f�[�^�̓Ǎ���
				load_TexBlock_4b( face_4p_game_data[game_state_data[i].anime.cnt_charcter_no],16,16 );
				draw_Tex( story_4p_demo_face_pos[j] + 5,127,16,16,0,0);
				j++;
			}
		}

		//	VS
		if( evs_story_flg ){
			load_TexPal( dm_4p_game_face_back_vs_b_bm0_0tlut );
		}else{
			load_TexPal( dm_4p_team_face_back_vs_bm0_0tlut );
		}
		load_TexBlock_4b( &dm_4p_game_face_back_vs_bm0_0[0],16,10);
		draw_Tex( story_4p_demo_face_pos[4],130,16,10,0,0);


	}
}
/*--------------------------------------
	�U������`��֐�
--------------------------------------*/
void	draw_target( game_state *state,u8 player_no )
{
	s8	i;
	u8	target[4][3] = {
		{1,2,3},
		{2,3,0},
		{3,0,1},
		{0,1,2},
	};

	//	����`��
	load_TexPal( dm_4p_game_target_base_bm0_0tlut );
	load_TexTile_4b( dm_4p_game_target_base_bm0_0,60,36,0,0,59,35 );
	draw_Tex( state -> map_x + 2,115,60,36,0,0);

	//	�U������
	load_TexTile_4b( dm_4p_game_target_p_bm0_0 ,56,10,0,0,55,9 );
	for(i = 0;i < 3;i++){
		draw_Tex( state -> map_x + 6 + i * 19,136,14,10,target[player_no][i] * 14,0);
	}

	//	�U������J�v�Z��
	load_TexTile_4b( cap_tex[1],16,128,0,0,15,128 - 1 );
	for(i = 2;i >= 0;i--){
		load_TexPal( capsel_8_pal[i] );
		draw_Tex( state -> map_x + 43 - (i * 19),120,8,8,0,capsel_l * 8);
		draw_Tex( state -> map_x + 51 - (i * 19),120,8,8,0,capsel_r * 8);
	}

	//	���̕`��
	gDPSetPrimColor( gp++,0,0,dm_retry_alpha[0],dm_retry_alpha[0],dm_retry_alpha[0],255);
	load_TexPal( arrow_d_bm0_0tlut );
	load_TexTile_4b( arrow_d_bm0_0,16,7,0,0,15,6 );
	for(i = 0;i < 3;i++){
		if( dm_retry_pos[3] == i ){
			draw_Tex( state -> map_x + 7 + i * 19,129,16,7,0,0 );
		}else{
			draw_Tex( state -> map_x + 7 + i * 19,128,16,7,0,0 );
		}
	}

}
/*--------------------------------------
	�Q�[���`��֐�(���o�֌W�p)
--------------------------------------*/
void	dm_game_graphic_effect( game_state *state,u8 player_no,u8 type )
{
	s16	i,j;

	switch( state -> game_condition[dm_training_flg] )
	{
	case	dm_cnd_retire:			//	���^�C�A
		load_TexPal( TLUT_retire );
		load_TexBlock_8b( &TEX_retire_0_0[0],56,29 );
		draw_Tex( state -> map_x + 4,201,56,29,0,0 );
		break;
	case	dm_cnd_training:		//	���K��
		load_TexPal( TLUT_training );
		load_TexBlock_8b( &TEX_training_0_0[0],56,29 );
		draw_Tex( state -> map_x + 4,201,56,29,0,0 );
		break;
	}

	switch( state -> game_condition[dm_mode_now] )
	{
	case	dm_cnd_stage_clear:		//	TRY NEXT STAGE
		try_next_stage_draw( 2 );
		push_any_key_draw( 128,200 );
		break;
	case	dm_cnd_game_over:		//	GAME OVER
		game_over_draw( 160 );
		push_any_key_draw( 128,200 );
		break;
	case	dm_cnd_game_over_retry:	//	GAME OVER & RETRY
		game_over_draw( 160 );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_win_retry:		//	WIN & RETRY
		retry_draw( state, player_no );
		win_draw( player_no );
		break;
	case	dm_cnd_win_any_key:		//	WIN & PUSH ANY KEY
		win_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_win_any_key_sc:	//	WIN & PUSH ANY KEY & SCORE
		dm_disp_score_plus( state );
		win_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_win:				//	WIN
		win_draw( player_no );
		break;
	case	dm_cnd_lose_retry:		//	LOSE & RETRY
		retry_draw( state, player_no );
		lose_draw( player_no );
		break;
	case	dm_cnd_lose_any_key:	//	LOSE & PUSH ANY KEY
		lose_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_lose_retry_sc:	//	LOSE & RETRY & SCORE
		dm_disp_score_plus( state );
		retry_draw( state, player_no );
		lose_draw( player_no );
		break;
	case	dm_cnd_lose_sc:			//	LOSE & PUSH ANY KEY & SCORE
		dm_disp_score_plus( state );
		lose_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_lose:			//	LOSE
		lose_draw( player_no );
		break;
	case	dm_cnd_draw_any_key:	//	DRAW & PUSH ANY KEY
		draw_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_draw:			//	DRAW
		draw_draw( player_no );
		break;
	case	dm_cnd_tr_chack:		//	���K�m�F(��)
		load_TexPal( TLUT_mes06 );
		load_TexBlock_4b( &TEX_mes06_0_0[0],64,64 );
		draw_Tex( state -> map_x,100,64,64,0,0 );
		break;
	case	dm_cnd_pause:			//	PAUSE
		pause_draw( player_no );
		break;
	case	dm_cnd_pause_tar:	//	PAUSE & TARGET
		pause_draw( player_no );
		draw_target( state,player_no );
		break;
	case	dm_cnd_pause_re:		//	PAUSE & RETRRY
		pause_draw( player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_re_sc:		//	PAUSE & RETRRY & SCORE
		dm_disp_score_plus( state );
		pause_draw( player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_tar_re:		//	PAUSE & RETRRY & TARGET
		pause_draw( player_no );
		draw_target( state,player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_tar_re_sc:	//	PAUSE & RETRRY & TARGET & SCORE
		dm_disp_score_plus( state );
		pause_draw( player_no );
		draw_target( state,player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_wait:			//	�ʏ���
	case	dm_cnd_manual:			//	�������
		bubble_draw( state ,player_no );		//	�U�����o
//		draw_chaine( player_no,type );	//	�A���\��
		shock_draw( player_no );		//	�_���[�W
		humming_draw( player_no );		//	�n�~���O
	}

	//	�`��͈͂�߂�
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	�������ݒ�
	gSPDisplayList(gp++, Texture_TE_dl );

}

/*--------------------------------------
	����J�v�Z���`��֐�
--------------------------------------*/
void	dm_draw_capsel( game_state *state ,u8 player_no )
{
	game_cap *n_cap;
	int		z;
	int		gg, hh=0;
	int		a,b,c;
	u32		xx[2],yy[2],zz[2],i,j,k,m,n,p,q;
	float	aa,bb,cc,dd,ee,ff,ii;
	u8		curve = 36,disp_flg = 0;
	u8*		tex;
	u16*	pal;


	if( state -> game_mode[dm_mode_now] != dm_mode_pause ){	//	�|�[�Y���͕`�悵�Ȃ�
		n_cap = &state -> now_cap;
		if( n_cap -> capsel_flg[cap_disp_flg] ){	//	�\���t���O�������Ă���
			if( state -> game_mode[dm_mode_now] == dm_mode_throw   ){
				//	������
				if( state -> cap_def_speed == SPEED_LOW ) {
					gg = ( state -> cap_speed_count >> 2 );
				} else {
					gg = ( state -> cap_speed_count >> 1 );
				}
				gg = gg % 4;
				gg = ONES_ooAB + gg;
				hh = 0;
				if( ONES_8_AB == gg || ONES_8_BA == gg ){
					hh = 5;
				}
				//	��]
				if( state -> now_cap.pos_x[0] == state -> now_cap.pos_x[1] ){
					//	�����J�v�Z�����c�������ꍇ
					if( gg == ONES_ooAB || gg == ONES_ooBA ){
						throw_rotate_capsel( &state -> now_cap );
					}
				}else{
					//	�����J�v�Z�������������ꍇ
					if( gg == ONES_8_AB || gg == ONES_8_BA ){
						throw_rotate_capsel( &state -> now_cap );
					}
				}
				//	�ʂ�`��
				if( state -> cap_speed_count < ( FlyingCnt[state -> cap_def_speed] / 3 ) ){
					ii = curve / (FlyingCnt[state -> cap_def_speed] / 3) * state -> cap_speed_count; // 1/3 �o����
				}else	if( state -> cap_speed_count < ( FlyingCnt[state -> cap_def_speed] / 3 << 1) ){	// 2/3 ���Ԉʒu
					ii = curve;
				}else{
					ii = (FlyingCnt[state -> cap_def_speed] << 1) / 3;
					ii = state -> cap_speed_count - ii;
					ii = curve - (ii * 2);
				}
				aa = ( state -> map_x + ( state -> map_item_size * n_cap -> pos_x[0] ));		// ���e�n�_ x,y
				bb = ( state -> map_y + ( state -> map_item_size * ( n_cap -> pos_y[0] - 1 )));
				cc = state -> map_x + 101;														// �J�n�n�_ x,y
				dd = state -> map_y + 16;
				ee = aa+(((cc-aa)/(FlyingCnt[state -> cap_def_speed]-1))*(FlyingCnt[state -> cap_def_speed] - state -> cap_speed_count ));	// ����ʒu x,y
				ff = bb+(((dd-bb)/(FlyingCnt[state -> cap_def_speed]-1))*(FlyingCnt[state -> cap_def_speed] - state -> cap_speed_count ));

				for( i = 0;i < 2;i++ ){
					xx[i] = (u32)ee + hh + (state -> map_item_size * (n_cap -> pos_x[i] - 3));
					yy[i] = ((u32)ff + hh - (u16)ii) + 1 + (state -> map_item_size * (n_cap -> pos_y[i] + 1));
					zz[i] = (n_cap -> capsel_g[i] << 3) *  state -> map_item_size ;
				}
				disp_flg = 1;
			}else{
				//	������
				if( n_cap -> pos_y[0] > 0 ){
					//	�J�[�\���ʒu�̊���o��
					for( i = 0;i < 2;i ++ ){
						xx[i] = state -> map_x + (state -> map_item_size * n_cap -> pos_x[i]);
						yy[i] = state -> map_y + (state -> map_item_size * n_cap -> pos_y[i]) + 1;
						zz[i] = (n_cap -> capsel_g[i] << 3) *  state -> map_item_size ;
					}
					disp_flg = 1;
				}
			}
			if( disp_flg ){
				// �t���[���o�b�t�@�ւ̓_�`�揈��
				if( state -> map_item_size == cap_size_10 ){	//	�傫���ꍇ
					q = 0;
				}else{
					q = 1;
				}

				a = (int)&( cap_tex[q][0] );						//	�O���t�B�b�N�A�h���X
				c = (int)&( gfx_freebuf[GFX_SPRITE_PAGE] );			//	���A�h���X
				tex = (u8 *)(( a & 0xfffff )+ c );					//	�e�N�X�`���A�h���X�擾

				for( i = 0;i < 2;i++ ){
					b = (int)&( cap_pal[q][n_cap -> capsel_p[i]][0] );	//	�p���b�g�A�h���b�X
					pal = (u16*)(( b & 0xfffff )+ c );		// �p���b�g�A�h���X�擾

					for( j = 0;j < state -> map_item_size;j ++ ){		//	�c���[�v
						for( k = 0;k < state -> map_item_size;k++ ){	//	�����[�v
							m = (u8)tex[ zz[i] + (j << 3) + (k >> 1) ];	//	�e�N�X�`���f�[�^���P�o�C�g�擾
							if( !( k & 1) ){							//	�����h�b�g�Ȃ���S�r�b�g���L��
								m = m >> 4;
							}
							m &= 0x0f;									//	m = �p���b�g�ԍ�
							if( m ){
								n = (u16)pal[m];						//	�t���[���o�b�t�@�ɑł�����
								fbuffer[wb_flag^1][(yy[i] + j ) * 320 + xx[i] + k] = n;
							}
						}
					}
				}
			}
		}
	}

}
/*--------------------------------------
	�Q�[���p�L�[���쐬�����ݒ�֐�
--------------------------------------*/
void	key_cntrol_init( void )
{
	u8	i;

	for( i = 0;i < 4;i ++ ){
		joyflg[i] =( 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT | DM_KEY_A | DM_KEY_B | DM_KEY_CU | DM_KEY_CD | DM_KEY_CL | DM_KEY_CR );
		keepjoy[i] = STARTJ;
		counjoy[i] = idexjoy[i] = 0;
		joygmf[i] = TRUE;
		joygam[i] = 0x0000;
	}
	joycur1 = evs_keyrept[0];
	joycur2 = evs_keyrept[1];
	joystp = FALSE;
}

/*--------------------------------------
	�Q�[���p�L�[���쐬�֐�
--------------------------------------*/
void	dm_make_key(void)
{
	u8	i;

	// joygam[x] �� �ްїp�� ���쐬
	switch( evs_gamesel ){
	case	GSL_1PLAY:
	case	GSL_VSCPU:
		if( joygmf[main_joy[0]] ){
			joygam[0] = ( joyupd[main_joy[0]] &( DM_KEY_A|DM_KEY_B ))|( joycur[main_joy[0]] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
			if( !( joynew[main_joy[0]] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
				joygam[0] = joygam[0] | ( joynew[main_joy[0]] & DM_KEY_DOWN );
			}
		}
		break;
	case	GSL_2PLAY:
		for(i = 0;i < 2;i++) {
			if( joygmf[main_joy[i]] ) {
				joygam[i] = ( joyupd[main_joy[i]] &( DM_KEY_A|DM_KEY_B ))|( joycur[main_joy[i]] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
				if( !( joynew[main_joy[i]] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
					joygam[i] = joygam[i]|( joynew[main_joy[i]] & DM_KEY_DOWN );
				}
			}
		}
		break;
	default:	//	4PLAY & DEMO
		for(i=0;i < 4;i++) {
			if( !game_state_data[i].player_state[PS_PLAYER] ){	//	�l�Ԃ̂Ƃ�
				if( joygmf[i] ) {
					joygam[i] = ( joyupd[i] &( DM_KEY_A|DM_KEY_B ))|( joycur[i] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
					if( !( joynew[i] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
						joygam[i] = joygam[i]|( joynew[i] & DM_KEY_DOWN );
					}
				}
			}
		}
		break;
	}
}
/*--------------------------------------
	�J�v�Z������֐�
--------------------------------------*/
void	key_control_main( game_state *state,game_map *map, u8 player_no ,u8 joy_no )
{
	u8	i;
	game_cap *cap;

	if( state -> game_mode[dm_mode_now] == dm_mode_throw  ){	//	����
		dm_draw_capsel(  state,player_no );
		// ���۰ׂ������Ȃ����̏��� :����߰Ă𒼑O���ް��ɂ���
		if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
			if( joynew[joy_no] & DM_KEY_RIGHT ) {
				joyCursorFastSet( DM_KEY_RIGHT,joy_no );
			}
			if( joynew[joy_no] & DM_KEY_LEFT ) {
				joyCursorFastSet( DM_KEY_LEFT, joy_no );
			}
		}
	}else	if( state -> game_mode[dm_mode_now] == dm_mode_down ){	//	����J�v�Z������
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	�ʏ���


			//	�������v�l�ǉ��ʒu
			if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifKeyOut( state );
			}


			cap = &state -> now_cap;
			if( joygam[player_no] & DM_ROTATION_L ){	//	����]
				rotate_capsel( map,cap,cap_turn_l );
			}else	if( joygam[player_no] & DM_ROTATION_R ){	//	�E��]
				rotate_capsel( map,cap,cap_turn_r );
			}
			if( joygam[player_no] & DM_KEY_LEFT ){	//	���ړ�
				translate_capsel( map,state,cap_turn_l,joy_no );
			}else	if( joygam[player_no] & DM_KEY_RIGHT ){	//	�E�ړ�
				translate_capsel( map,state,cap_turn_r,joy_no );
			}

			if( cap -> pos_y[0] > 0 ){
				if( joygam[player_no] & DM_KEY_DOWN ){	//	��������
					i = FallSpeed[state -> cap_speed] >> 1;
					if( FallSpeed[state -> cap_speed] % 2 ){
						i ++;
					}
					state -> cap_speed_vec = i;
				}else{
					state -> cap_speed_vec = 1;
				}
			}else{
				state -> cap_speed_vec = 1;
			}
		}else{
			// ���۰ׂ������Ȃ����̏��� :����߰Ă𒼑O���ް��ɂ���
			if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
				if( joynew[joy_no] & DM_KEY_RIGHT ) {
					joyCursorFastSet( DM_KEY_RIGHT,joy_no );
				}
				if( joynew[joy_no] & DM_KEY_LEFT	) {
					joyCursorFastSet( DM_KEY_LEFT, joy_no );
				}
			}
		}
		dm_draw_capsel(  state,player_no );
		dm_capsel_down( state,map );
	}else{
		if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
			joyCursorFastSet( DM_KEY_RIGHT,joy_no );
			joyCursorFastSet( DM_KEY_LEFT, joy_no );
		}
	}
}
/*--------------------------------------
	�v�l����֐�
--------------------------------------*/
void	make_ai_main(void)
{
	s8	i;

	if( dm_think_flg ){
		switch( evs_gamesel )
		{
		case	GSL_VSCPU:
		case	GSL_2DEMO:
			for( i = 0;i < 2;i++ ){
				if( game_state_data[i].player_state[PS_PLAYER] == PUF_PlayerCPU ){
					if( game_state_data[i].game_condition[dm_static_cnd] == dm_cnd_wait ){
						aifMake( &game_state_data[i] );
					}
				}
			}
			break;
		case	GSL_4PLAY:
		case	GSL_4DEMO:
			for( i = 0;i < 4;i++ ){
				if( game_state_data[i].player_state[PS_PLAYER] == PUF_PlayerCPU ){
					if( game_state_data[i].game_condition[dm_static_cnd] == dm_cnd_wait ){
						aifMake( &game_state_data[i] );
					}
				}
			}
			break;
		case	GSL_1DEMO:
			if( game_state_data[0].player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifMake( &game_state_data[0] );
			}
			break;
		}
	}
}
/*--------------------------------------
	���ʉ��o�v�Z�֐�
--------------------------------------*/
void	dm_effect_make(void)
{
	//	push_any_key
	push_any_key_main();

	//	���g���C�̓_�Ōv�Z
	dm_retry_alpha[0] =(s16)(sinf( DEGREE( dm_retry_alpha[1] ) ) * 64 ) + 191;
	dm_retry_alpha[1] += 10;
	if( dm_retry_alpha[1] >= 360 ){
		dm_retry_alpha[1] -= 360;
	}

	//	���g���C�̖�󍶉E�ړ��̌v�Z
	dm_retry_pos[1] ++;
	if( dm_retry_pos[1] >= 15 ){
		dm_retry_pos[1] = 0;
		dm_retry_pos[0] ^= 1;
	}
	//	���g���C�̍U������\���̖��㉺�̌v�Z
	dm_retry_pos[2] ++;
	if( dm_retry_pos[2] > 30 )
	{
		dm_retry_pos[2] = 0;
		dm_retry_pos[3]++;
		if( dm_retry_pos[3] > 2 )
		{
			dm_retry_pos[3] = 0;
		}
	}

	//	�s�[�`�P�l���f���v�Z
	if( dm_peach_get_flg[0] ){
		dm_peach_get_flg[1] += 4;
		if( dm_peach_get_flg[1] >= 152 ){
			dm_peach_get_flg[1] = 152;
		}
	}

	//	�������̌v�Z
	win_anime_count[1] ++;
	if( win_anime_count[1] > 4 ){
		win_anime_count[1] = 0;
		win_anime_count[0] ++;
		if( win_anime_count[0] > 14 ){
			win_anime_count[0] = 0;
		}
	}

}
/*--------------------------------------
	�Q�[���p EEP �������݊֐�
--------------------------------------*/
void	dm_game_eep_write(u32 err_add)
{
	EepRomErr err = EepRomErr_NoError;
	s8	i;

	for(i = 0;i < 2;i++){
		if( eep_rom_flg[i] ){
			if(err == EepRomErr_NoError) {
				err = EepRom_WritePlayer(evs_select_name_no[i]);
			}
		}
	}
	*(EepRomErr *)err_add = err;
}
/*--------------------------------------
	�Q�[���������֐�
--------------------------------------*/
void	dm_game_init(void)
{
	s16	i,j,k;
	s16	map_x_table[][4] = {
		{dm_wold_x,dm_wold_x,dm_wold_x,dm_wold_x},							// 1P
		{dm_wold_x_vs_1p,dm_wold_x_vs_2p,dm_wold_x_vs_1p,dm_wold_x_vs_2p},	// 2P
		{dm_wold_x_4p_1p,dm_wold_x_4p_2p,dm_wold_x_4p_3p,dm_wold_x_4p_4p}	// 4P
	};
	u8	map_y_table[] = {dm_wold_y,dm_wold_y_4p};
	u8	size_table[] = {cap_size_10,cap_size_8};


	auSeqPlayerStop(0);		//	�a�f�l���čĐ����邽��
	init_effect_all();		//	�G�t�F�N�g������
	dm_make_magazine();		//	�J�v�Z���쐬
	auSeqpVolumeInit();		//	�{�����[����߂�

	evs_mainx10 = FALSE;			// �`�����ݸޒ���
	evs_graph10 = FALSE;			//		..

	dm_game_init_flg		= 0;						//	1��� �w�i���������`�悵�Ȃ�
	evs_seqnumb 			= seq_save;					//	BGM�̐ݒ�
	dm_start_time 			= dm_mode_wait_count;		//	��P���܂ł̃E�F�C�g
	evs_seSetPtr 			= evs_seGetPtr = 0;			//	SE������
	bgm_bpm_flg[0] 			= bgm_bpm_flg[1] = 0;		//	BGM�̑������P��ς��邽�߂̃t���O
	dm_demo_time_flg 		= dm_demo_time = 0;			//	�f���̂Ƃ��̃^�C�}�[�ƃt���O�̃N���A
	last_3_se_flg 			= 0;						//	���[�`����炷�t���O�̃N���A
	story_time_flg 			= 0;						//	�^�C�����Z���J�n����t���O
	story_4p_demo_flg[0] 	= story_4p_demo_flg[1] = 0;	//	4P���̃`�[���\���̃t���O�̃N���A
	dm_anime_restart_count	= 0;						//	���s�����̃A�j���[�V�����ĉғ��t���O�̃N���A

	dm_retry_flg = 1;								//	�ʏ�̓��g���C�\

	for( i = 0;i < 2;i++ ){
		dm_retry_alpha[i] =  0;		//	���g���C�J�[�\���̓_�ŗp�ϐ��̏�����

	}

	//	1P�p�̏�����
	for(i = j = 0;i < 3;i++){

		//	�R�C���A�j���[�V�����p�ݒ菉����
		for( k = 0;k < 3; k++ ){
			dm_retry_coin_pos[k].flg = 0;
			dm_retry_coin_pos[k].pos = 0;
			dm_retry_coin_pos[k].move_vec[0] = 0;
			dm_retry_coin_pos[k].move_vec[1] = 0;
			dm_retry_coin_pos[k].anime[0] = 0;
			dm_retry_coin_pos[k].anime[1] = 0;
		}

		big_virus_flg[i][0] = 0;	//	���ŃA�j���[�V�����t���O
		big_virus_flg[i][1] = 0;	//	����SE�t���O

		//	����E�C���X�̍��W���v�Z����
		big_virus_position[i][2] = j;	//	sin,cos�p���l
		big_virus_position[i][0] = (( 10 * sinf( DEGREE( big_virus_position[i][2] ) ) ) * -2 ) + 45;
		big_virus_position[i][1] = (( 10 * cosf( DEGREE( big_virus_position[i][2] ) ) ) * 2  ) + 155;
		j += 120;
	}
	//	���_���o�p�ϐ��̏�����
	for(i = 0;i < 10;i++){
		lv_sc_pos[i].score = 0;					//	��������ϐ��̃N���A
		lv_sc_pos[i].num_max = 0;				//	��������ϐ��̃N���A
		lv_sc_pos[i].flg = 0;					//	��������ϐ��̃N���A
		lv_sc_pos[i].mode = 0;					//	��������ϐ��̃N���A
		for( j = 0;j < 5;j++ ){
			lv_sc_pos[i].num_alpha[j] = 0;		//	�����x�̃N���A
			lv_sc_pos[i].num_alpha_flg[j] = 0;	//	���������I������ϐ��̃N���A
			lv_sc_pos[i].num_g[j] = 0;			//	�����O���t�B�b�N���w���ϐ��N���A
			lv_sc_pos[i].score_pos[j][0] = lv_sc_pos[i].score_pos[j][1] = 0;	//	���W�N���A
		}
	}

	//	�E�C���X���\���T�C�Y�̏�����
	for( i = 0;i < 4;i++ ){

		for(j = 0;j < 4;j++){
			story_4p_stock_cap[i][j] = -1;	//	4P�`�[���΍R��̍U���J�v�Z���X�g�b�N�̃N���A
		}

		for( j = 0;j < 3;j++ ){
			dm_retry_position[i][j] = 0;	//	���g���C�J�[�\���Ɋւ���ϐ��̏�����
		}
		dm_retry_pos[i] =  0;		//	���g���C�J�[�\���̍��W�p�ϐ��̏�����

		big_virus_count[i] = 0;	//	�e�F�̃E�C���X��
		virsu_num_size[i][0] = 12;
		virsu_num_size[i][1] = 16;
		virsu_num_size[i][2] = 0;
	}

	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
		k = 0;	//	�A�C�e���̑傫��
		j = 0;	//	��w���W
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		k = 0;	//	�A�C�e���̑傫��
		j = 1;	//	��w���W
		break;
	case	GSL_4PLAY:
	case 	GSL_4DEMO:
		k = 1;	//	�A�C�e���̑傫��
		j = 2;	//	��w���W
		break;
	}

	for( i = 0;i < 4;i ++ ){
		//	���W�n�̐ݒ�
		game_state_data[i].map_x = map_x_table[j][i];		//	��w���W
		game_state_data[i].map_y = map_y_table[k];			//	��x���W
		game_state_data[i].map_item_size = size_table[k];	//	��A�C�e���T�C�Y
	}

	for(i = 0;i < 4;i++)
	{
		//	��������ϐ��̐ݒ�
		game_state_data[i].player_state[PS_PLAYER_NO]		= i;			//	�v���C���[�ԍ�
		game_state_data[i].game_mode[dm_mode_now] 			= dm_mode_init;	//	������������������[�h�ɐݒ�
		game_state_data[i].game_condition[dm_mode_now] 		= dm_cnd_init;	//	��Ԃ���������Ԃɐݒ�
		game_state_data[i].game_condition[dm_training_flg]	= dm_cnd_wait;	//	�g���[�j���O��Ԃ�ʏ���(�g���[�j���O����)�ɐݒ�
		game_state_data[i].game_condition[dm_static_cnd]	= dm_cnd_wait;	//	��ΓI��Ԃ�ʏ��Ԃɐݒ�
		game_state_data[i].virus_number						= 0;			//	�E�C���X���̃N���A
		game_state_data[i].warning_flg 						= 0;			//	�x�����t���O�̃N���A

		for( j = 0;j < 3;j++ ){
			game_state_data[i].retire_flg[j] = 0;				//	���^�C�A�t���O�̃N���A
		}

		//	�J�v�Z���������x�̐ݒ�
		game_state_data[i].cap_speed 						= GameSpeed[game_state_data[i].cap_def_speed];	//	���������J�E���g�̐ݒ�
		game_state_data[i].cap_speed_max 					= 0;											//	�����ݒ肳���
		game_state_data[i].cap_speed_vec 					= 1;											//	�����J�E���^�����l
		game_state_data[i].cap_magazine_cnt 				= 1;											//	�}�K�W���c�Ƃ�1�ɂ���
		game_state_data[i].cap_count = game_state_data[i].cap_speed_count = 0;								//	�J�E���^�̏�����
		game_state_data[i].cap_move_se_flg 					= 0;											//	�ړ����̂r�d�Đ��p�t���O�̃N���A

		//	�J�v�Z�����̏����ݒ�
		dm_set_capsel( &game_state_data[i] );

		//	�E�C���X�A�j���[�V�����̐ݒ�
		game_state_data[i].virus_anime = game_state_data[i].virus_anime_count = 0;	//	�J�E���^�ƃA�j���V�����ԍ��̏�����
		game_state_data[i].virus_anime_vec = 1;										//	�A�j���[�V�����i�s�����̎w��
		game_state_data[i].erase_anime = 0;											//	���ŃA�j���[�V�����R�}���̏�����
		game_state_data[i].erase_anime_count = 0;									//	���ŃA�j���[�V�����J�E���^�̏�����
		game_state_data[i].erase_virus_count = 0;									//	���ŃE�C���X�J�E���^�̏�����

		//	�A�����̏�����
		game_state_data[i].chain_count = game_state_data[i].chain_line =  0;

		switch(evs_gamesel)
		{
		case	GSL_4PLAY:
		case	GSL_4DEMO:
			//	�S�o�̏ꍇ
			game_state_data[i].virus_anime_spead = v_anime_def_speed_4p;			//	�A�j���[�V�����Ԋu�̐ݒ�
			game_state_data[i].virus_anime_max = 0;									//	�E�C���X�A�j���[�V�����ő�R�}���̐ݒ�
			break;
		default:
			//	����ȊO�̏ꍇ
			game_state_data[i].virus_anime_spead = v_anime_def_speed;				//	�A�j���[�V�����Ԋu�̐ݒ�
			game_state_data[i].virus_anime_max = 2;									//	�E�C���X�A�j���[�V�����ő�R�}���̐ݒ�
			break;
		}

		//	�U���J�v�Z���f�[�^�̏�����
		for( j = 0;j < 4;j++ ){
			game_state_data[i].chain_color[j] = 0;
		}

		//	�픚�J�v�Z���f�[�^�̏�����
#ifdef	DAMAGE_TYPE
		for( j = 0;j < DAMAGE_MAX;j++ ){
			game_state_data[i].cap_attack_work[j][0] = 0;	//	�U������
			game_state_data[i].cap_attack_work[j][1] = 0;	//	�U������
		}
#endif
#ifndef	DAMAGE_TYPE
		for( j = 0;j < DAMAGE_MAX;j++ ){
			for( k = 0;k < 5;k++ ){
				game_state_data[i].cap_attack_work[j][k] = 0;
			}
		}
#endif
		//	�}�b�v���i�r�̒��j�̏�����
		for(j = 0;j < 128;j ++){
			game_map_data[i][j].capsel_m_g = game_map_data[i][j].capsel_m_p = 0;
			for(k = 0;k < 6;k++){
				game_map_data[i][j].capsel_m_flg[k] = 0;
			}
		}
		for(j = 0;j < 16;j++){
			for(k = 0;k < 8;k++){
				game_map_data[i][(j << 3) + k].pos_m_x = k;		//	�w���\
				game_map_data[i][(j << 3) + k].pos_m_y = j + 1;	//	�x���W
			}
		}

	}
	switch(evs_gamesel)
	{
	case	GSL_4PLAY:
	case	GSL_4DEMO:
		for( i = 0;i < 4;i++ ){
			//	�E�C���X�f�[�^�̍쐬
			dm_virus_init(&game_state_data[i],virus_map_data[i],virus_map_disp_order[i]);
		}
		for( i = 0;i < 3;i++ )
		{
			for( j = i + 1;j < 4;j++ ){
				if( game_state_data[i].virus_level == game_state_data[j].virus_level ){
					//	���x���������������ꍇ�E�C���X�����R�s�[����
					dm_virus_map_copy(virus_map_data[i],virus_map_data[j],virus_map_disp_order[i],virus_map_disp_order[j] );
					break;
				}
			}
		}
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		for( i = 0;i < 2;i++ ){
			//	�E�C���X�f�[�^�̍쐬
			dm_virus_init(&game_state_data[i],virus_map_data[i],virus_map_disp_order[i]);
		}
		if( game_state_data[0].virus_level == game_state_data[1].virus_level ){
			//	���x���������������ꍇ�E�C���X�����R�s�[����
			dm_virus_map_copy(virus_map_data[0],virus_map_data[1],virus_map_disp_order[0],virus_map_disp_order[1] );
		}
		break;
	case	GSL_1PLAY:
	case	GSL_1DEMO:
		//	�E�C���X�f�[�^�̍쐬
		dm_virus_init(&game_state_data[0],virus_map_data[0],virus_map_disp_order[0]);
		break;
	}


	aifGameInit();
}

/*--------------------------------------
	�Q�[���������C���֐�
--------------------------------------*/
int		dm_game_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	EepRomErr	eep_err;
	int	ret;
	u32	address;
	u32	score_save;
	s16	*msgtype;
	u8	i,j;
	s8	loop_flg = 0;
	s8	gs;
	u16	joybak;


	//	���b�Z�[�W�L���[�̍쐬
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	���b�Z�[�W�p�m���蓖��
	//	�N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);

	//	������
	seq_save = evs_seqnumb;	//	BGM�̕ۊ�

	dm_game_init();									//	�Q�[���̏�����
	key_cntrol_init();
	win_anime_count[0]  = win_anime_count[1] = 0;
	dm_retry_coin = 0;
	dm_peach_stage_flg = 0;
	dm_TouchDownWait = 2;
	dm_think_flg = 1;

	//	�ϐ��̏�����
	for(i = 0;i < 2;i++){
		dm_peach_get_flg[i] = 0;
		story_4p_name_flg[i] = i;
		vs_win_count[i] = 0;
		vs_lose_count[i] = 0;
	}

	for( i = 0;i < 4;i++ ){
		win_count[i] = 0;	//	�������J�E���g�̏�����
	}


	//	�O���t�B�b�N�f�[�^�̓Ǎ���(�w�i��)
	switch( evs_gamesel )
	{
	case	GSL_1DEMO:
		score_save = evs_high_score;	//	�n�C�X�R�A��ۑ����ăN���A����
		evs_high_score = 0;
	case	GSL_1PLAY:
		dm_TouchDownWait = 1;
		auRomDataRead( (u32)dm_anime_rom_address[0][0],gfx_freebuf[GFX_ANIME_PAGE_1],(u32)dm_anime_rom_address[0][1] - (u32)dm_anime_rom_address[0][0] );		//	�}���I
		auRomDataRead( (u32)dm_anime_rom_address[12][0],gfx_freebuf[GFX_ANIME_PAGE_2],(u32)dm_anime_rom_address[12][1] - (u32)dm_anime_rom_address[12][0] );	//	�E�C���X

		game_state_data[0].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_1];
		dm_anime_char_set(&game_state_data[0].anime,game_anime_data_mario );
		dm_anime_set( &game_state_data[0].anime,ANIME_opening );
		for( j = 0;j < 2;j++ ){
			disp_anime_data[0][j] = game_state_data[0].anime;
		}
		//	�E�C���X�A�j���[�V�����ݒ�
		for( i = 0;i < 3;i++ ){
			big_virus_anime[i].cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];
			dm_anime_char_set(&big_virus_anime[i],game_anime_table[i + 12] );
			dm_anime_set( &big_virus_anime[i],ANIME_nomal );
		}

		//	�w�i�Ǎ���
		auRomDataRead((u32)_dm_bg_level_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_level_modeSegmentRomEnd - (u32)_dm_bg_level_modeSegmentRomStart);
		S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		//	�A�j���[�V�����f�[�^�Ǎ���
		auRomDataRead( (u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][0],gfx_freebuf[GFX_ANIME_PAGE_1],
										(u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][1] - (u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][0] );
		auRomDataRead( (u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][0],gfx_freebuf[GFX_ANIME_PAGE_2],
										(u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][1] - (u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][0] );

		//	�A�h���X�ݒ�
		game_state_data[0].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_1];
		game_state_data[1].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];

		//	�����A�j���[�V�����f�[�^�̐ݒ�
		dm_anime_char_set(&game_state_data[0].anime,game_anime_table[game_state_data[0].anime.cnt_charcter_no] );
		dm_anime_char_set(&game_state_data[1].anime,game_anime_table[game_state_data[1].anime.cnt_charcter_no] );

		//	�����A�j���[�V�����̐ݒ�
		for( i = 0;i < 2; i++ ){
			dm_anime_set( &game_state_data[i].anime,ANIME_opening );
			for( j = 0;j < 2;j++ ){
				disp_anime_data[i][j] = game_state_data[i].anime;
			}
		}
		//	�w�i�Ǎ���
		if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
			if( evs_story_no == 11 ){
				dm_peach_stage_flg = 1;	//	�G�N�X�g���X�e�[�W
			}else{
				dm_game_bg_no = evs_story_no - 1;
			}
		}else{
			//	�|�C���g�\���̐��l�����炩���߃Z�b�g���Ă���
			for(i = 0;i < 2;i++){
				dm_vsmode_win_point[i] = 0;
			}
			switch( evs_gamesel ){
			case	GSL_2PLAY:
				for(i = 0;i < 2;i++){
					if( evs_select_name_no[i] != 8 ){	//	�������Q�X�g�ł͂Ȃ�
						if( evs_select_name_no[i ^ 1] != 8 ){	//	������Q�X�g�ł͂Ȃ�
							dm_vsmode_win_point[i] = evs_mem_data[evs_select_name_no[i]].vs_data[evs_select_name_no[i ^ 1]][0];
						}
					}
				}
				break;
			case	GSL_VSCPU:
				if( evs_select_name_no[0] != 8 ){	//	�������Q�X�g�ł͂Ȃ�
					j = game_state_data[1].anime.cnt_charcter_no;	//	����̃L�����N�^�[�ԍ�
					dm_vsmode_win_point[0] = evs_mem_data[evs_select_name_no[0]].vscom_data[j][0];
					dm_vsmode_win_point[1] = evs_mem_data[evs_select_name_no[0]].vscom_data[j][1];
				}
				break;
			}

			if( evs_stage_no > 0 ){	//	�s�[�`�X�e�[�W�łȂ�������
				dm_game_bg_no = evs_stage_no - 1;//	��
			}else{
				dm_peach_stage_flg = 1;
			}
		}
		if( dm_peach_stage_flg ){
			//	�s�[�`�X�e�[�W
			auRomDataRead((u32) _bg_dat_exSegmentRomStart, BGBuffer, (u32)_bg_dat_exSegmentRomEnd - (u32)_bg_dat_exSegmentRomStart);
			S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		}else{
			//	�ʏ�X�e�[�W
			auRomDataRead((u32)dm_bg_rom_address[dm_game_bg_no], BGBuffer,51720);
			S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,160,320,160,0,40,0,0 );
		}
		break;
	case	GSL_4PLAY:
	case	GSL_4DEMO:

		//	�w�i�Ǎ���
		auRomDataRead((u32)_dm_bg_vs4p_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_vs4p_modeSegmentRomEnd - (u32)_dm_bg_vs4p_modeSegmentRomStart);
		S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		for( i = 0;i < 4;i++ ){
			story_4p_name_num[i] = 0;
		}
		//	�`�[���̃����o�[���v�Z
		for( i = 0;i < 4;i++ ){
			story_4p_name_num[ game_state_data[i].player_state[PS_TEAM_FLG] ] ++;
		}

		if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
			//	�`�[���΍R��
			dm_game_4p_team_flg = 1;
			//	�f���������s��
			story_4p_demo_flg[0] = 1;
			story_4p_demo_w[0] = 2;
			//	�f���Ɏg��������̐ݒ�
			S2d_ObjBg_InitTile( &s2d_demo_4p,dm_4p_game_face_back_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,2,58,62,102,96,0 );

			//	�w�i(���)�̑I��
			story_4p_demo_bg = dm_4p_game_face_back_bm0_0;
			story_4p_demo_bg_tlut = dm_4p_game_face_back_bm0_0tlut;

			story_4p_demo_face_pos[0] = 91;		//	1p
			story_4p_demo_face_pos[3] = 205;	//	4p

			switch( evs_story_level )
			{
			case	0:	//	3 VS 1	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_3_VS_1;
				dm_game_4p_team_bits[0] = 0x07;
				dm_game_4p_team_bits[1] = 0x08;
				story_4p_demo_face_pos[1] = 119;	//	2p
				story_4p_demo_face_pos[2] = 147;	//	3p
				story_4p_demo_face_pos[4] = 180;	//	vs
				break;
			case	1:	//	2 VS 2	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_2_VS_2;
				dm_game_4p_team_bits[0] = 0x03;
				dm_game_4p_team_bits[1] = 0x0c;
				story_4p_demo_face_pos[1] = 119;	//	2p
				story_4p_demo_face_pos[2] = 177;	//	3p
				story_4p_demo_face_pos[4] = 153;	//	vs
				break;
			case	2:	//	1 VS 3	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_1_VS_3;
				dm_game_4p_team_bits[0] = 0x01;
				dm_game_4p_team_bits[1] = 0x0e;
				story_4p_demo_face_pos[1] = 147;	//	2p
				story_4p_demo_face_pos[2] = 177;	//	3p
				story_4p_demo_face_pos[4] = 124;	//	vs
				break;
			}
		}else{
			if( story_4p_name_num[0] == 1 && story_4p_name_num[1] == 1
					&& story_4p_name_num[2] == 1 && story_4p_name_num[3] == 1 ){

				//	�o�g�����C����
				dm_game_4p_team_flg = 0;
				dm_game_4p_team_bit = 0;
			}else{
				//	�`�[���΍R��
				dm_game_4p_team_flg = 1;

				//	�f���������s��
				story_4p_demo_flg[0] = 1;
				story_4p_demo_w[0] = 2;

				//	����p�r�b�g�̍쐬
				dm_game_4p_team_bits[0] = dm_game_4p_team_bits[1] = 0;
				for(i = 0;i < 4;i++){
					if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
						dm_game_4p_team_bits[TEAM_MARIO] |= 1 << i;
					}
				}
				for(i = 0;i < 4;i++){
					if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_ENEMY ){
						dm_game_4p_team_bits[TEAM_ENEMY] |= 1 << i;
					}
				}

				story_4p_demo_face_pos[0] = 91;		//	1p
				story_4p_demo_face_pos[3] = 205;	//	4p

				switch( story_4p_name_num[0] )
				{
				case	1:	//	1 VS 3	( A : B )
					dm_game_4p_team_bit = dm_1_VS_3;
					story_4p_demo_face_pos[1] = 146;	//	2p
					story_4p_demo_face_pos[2] = 177;	//	3p
					story_4p_demo_face_pos[4] = 124;	//	vs
					break;
				case	2:	//	2 VS 2	( A : B )
					dm_game_4p_team_bit = dm_2_VS_2;
					story_4p_demo_face_pos[1] = 119;	//	2p
					story_4p_demo_face_pos[2] = 177;	//	3p
					story_4p_demo_face_pos[4] = 153;	//	vs
					break;
				case	3:	//	3 VS 1	( A : B )
					dm_game_4p_team_bit = dm_3_VS_1;
					story_4p_demo_face_pos[1] = 119;	//	2p
					story_4p_demo_face_pos[2] = 147;	//	3p
					story_4p_demo_face_pos[4] = 180;	//	vs
					break;
				}

				story_4p_demo_bg = dm_4p_team_face_back_bm0_0;
				story_4p_demo_bg_tlut = dm_4p_team_face_back_bm0_0tlut;
				S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,2,58,62,102,96,0 );
			}
		}

		break;
	}
	//	�O���t�B�b�N�f�[�^�̓Ǎ���(�J�v�Z����)
	auRomDataRead((u32)_spriteSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)_spriteSegmentRomEnd-(u32)_spriteSegmentRomStart);

	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	graphic_no = GFX_GAME;

//	auSeqPlayerPlay(0,evs_seqnumb);	//	���y�Đ�
	while( loop_flg  == 0 )
	{
		//	���g���[�X���b�Z�[�W
		(void) osRecvMesg(&msgQ,(OSMesg*)&msgtype, OS_MESG_BLOCK);
		// �b�o�t��������
		if (!MQ_IS_EMPTY(&msgQ)) {
			nnp_over_time = 1;
		}


		if( *msgtype == NN_SC_RETRACE_MSG ) {
#ifdef	DM_DEBUG_FLG
			if( joyupd[main_joy[0]] & DM_KEY_L ){	//	�ʕύX
				if( evs_story_flg ){	//	�X�g�[���[���[�̏ꍇ
					//	�e�X�g�p�����I��
					game_state_data[0].game_condition[dm_static_cnd] = dm_cnd_win;
//					game_state_data[main_joy[0]].game_condition[dm_static_cnd] = dm_cnd_win;
					break;
				}else{
					if( evs_gamesel != GSL_1PLAY ){
						break;
					}
				}
			}

			if( evs_gamesel == GSL_1PLAY ){
				if( joyupd[main_joy[0]] & DM_KEY_Z ){	//	�R�C���ǉ�
					if( dm_retry_coin < 3){
						dm_retry_coin ++;
					}
				}
			}
#endif

			#ifdef NN_SC_PERF
				if( joyupd[main_joy[0]] & Z_TRIG ){	//	�p�t�H�[�}���X���[�^�[�\������
					nnp_over_time = 0;
					disp_meter_flg ^= 1;
				}
			#endif

			evs_mainx10 = TRUE;
			#ifdef NN_PERF_Main_Graph
				#ifdef NN_SC_PERF
					nnScPushPerfMeter(NN_SC_PERF_MAIN);
				#endif
			#endif

#ifdef	DM_DEBUG_FLG

			joybak = joyupd[0];
			for(gs = 0;gs < evs_gamespeed;gs++) {
				if ( gs ) joyupd[0] = 0;
#endif
				if( bgm_bpm_flg[1] ){
					auSeqpSetTempo(0.75);				//	�Ȃ̂a�o�l��ς���
					bgm_bpm_flg[1] = 0;
				}


				dm_effect_make();	//	���ʌv�Z

				switch( evs_gamesel )
				{
				case	GSL_1PLAY:
					loop_flg = dm_game_main_1p();	//	1P PLAY
					if( loop_flg > 0 ){	//	�X�e�[�W�N���A
						if( loop_flg == dm_ret_next_stage ){
							if( game_state_data[0].virus_level < 99 ){
								game_state_data[0].virus_level ++;
							}
						}
						loop_flg = 0;
						dm_anime_set( &game_state_data[0].anime,ANIME_opening );
						for( i = 0;i < 3;i++ ){
							dm_anime_set( &big_virus_anime[i],ANIME_nomal );
						}

						dm_game_init();
					}else{
						//	���ʉ��Đ�
						dm_play_se();
					}
					break;
				case	GSL_2PLAY:
				case	GSL_VSCPU:
					//	�������̃A�j���[�V����

					loop_flg = dm_game_main_2p();
					if( loop_flg == dm_ret_game_end )
					{
						for( i = 0;i < 2;i++ ){
							if( win_count[i] == WINMAX ){	//	�N�����R�����Ă���
								vs_win_count[i] ++;
								vs_lose_count[i ^ 1] ++;
								break;
							}
						}
					}else	if( loop_flg == -1 ){
						if( !evs_story_flg ){	//	�X�g�[���[���[�h�łȂ��ꍇ
							loop_flg = 0;
							for( i = 0;i < 2;i++ ){
								if( win_count[i] == WINMAX ){	//	�N�����R�����Ă���
									vs_win_count[i] ++;
									vs_lose_count[i ^ 1] ++;
									loop_flg = -1;	//	�I��
									break;
								}
							}
							if( loop_flg == 0 ){
								dm_game_init();	//	�Ď���
								for( i = 0;i < 2; i++ ){
									dm_anime_set( &game_state_data[i].anime,ANIME_opening );
								}
							}
						}
					}else	if( loop_flg == dm_ret_retry ){	//	���g���C�̏ꍇ
						loop_flg = 0;
						dm_game_init();	//	�Ď���

						//	���s����
						if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
							for( i = 0;i < 2; i++ ){
								win_count[i] = 0;
							}
						}else{
							for( i = 0;i < 2;i++ ){
								if( win_count[i] == WINMAX ){	//	�N�����R�����Ă���
									vs_win_count[i] ++;
									vs_lose_count[i ^ 1] ++;
									for( j = 0;j < 2;j++ ){
										win_count[j] = 0;
									}
									break;
								}
							}
							if( evs_gamesel == GSL_VSCPU ){
								for( i = 0;i < 2; i++ ){
									win_count[i] = 0;
								}
							}
						}
						for( i = 0;i < 2; i++ ){
							dm_anime_set( &game_state_data[i].anime,ANIME_opening );
						}
					}else{
						//	���ʉ��Đ�
						dm_play_se();
					}
					break;
				case	GSL_4PLAY:
					if( story_4p_demo_flg[0] ){
						demo_story_4p_main();
					}else{
						loop_flg = dm_game_main_4p();
					}
					if( loop_flg == -1 ){
						loop_flg = 0;
						if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
							loop_flg = -1;	//	�Ƃ肠�����P�Q�[���ŏI��
						}else{
							for( i = 0;i < 4;i++ ){
								if( win_count[i] == WINMAX ){	//	�N�����R�����Ă���
									loop_flg = -1;	//	�I��
									break;
								}
							}
							if( loop_flg == 0 ){
								dm_game_init();	//	�Ď���
							}
						}
					}else	if( loop_flg == dm_ret_retry ){
						for( i = 0;i < 4;i++ ){
							win_count[i] = 0;
						}
						loop_flg = 0;
						dm_game_init();	//	�Ď���
					}else{
						if( loop_flg != dm_ret_game_end ){
							dm_play_se();
						}
					}
					break;
				case	GSL_1DEMO:
					loop_flg = dm_game_demo_1p();
					break;
				case	GSL_2DEMO:
					loop_flg = dm_game_demo_2p();
					break;
				case	GSL_4DEMO:
					loop_flg = dm_game_demo_4p();
					break;
				}
#ifdef	DM_DEBUG_FLG

				if( loop_flg != 0 )
					break;
			}
			joyupd[0] = joybak;
#endif
			evs_mainx10 = FALSE;
			#ifdef NN_PERF_Main_Graph
				#ifdef NN_SC_PERF
					nnScPopPerfMeter();	//	NN_SC_PERF_MAIN
				#endif
			#endif
		}
	}

	dm_think_flg = 0;	//	�v�l��~

	//	���y��~
	auSeqPlayerStop(0);

	//	�\����~
	graphic_no = GFX_NULL;

	//	�O���t�B�b�N�E�I�[�f�B�I�^�X�N�I���҂�
	while((auSeqPlayerState(0) != AL_STOPPED) || (pendingGFX != 0)) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	//	�N���C�A���g�̖���
	nnScRemoveClient(sched, &client);


//	evs_seqnumb = seq_save;	//	BGM�����ɖ߂�

	//	EEPROM �̏������ݏ����t���O
	eep_rom_flg[0] = eep_rom_flg[1] = 0;

	//	���݂̃��C���ԍ��̕ۑ�
	main_old = MAIN_GAME;

	//	���ɂǂ̏����ɔ�Ԃ��ݒ肷��
	switch( evs_gamesel )
	{
	case	GSL_4PLAY:
		if( evs_story_flg ){	//	�X�g�[���[���[�h
			evs_gamesel = GSL_VSCPU;
			if( evs_one_game_flg ){
				ret	=	MAIN_60;
			}else{
				if( game_state_data[0].game_condition[dm_static_cnd] ==  dm_cnd_win ){
					//	�Q�[���X�R�A�ƃ��^�C�A�񐔂��R�s�[����
					if( !evs_clear_stage_flg[evs_story_no] ){
						evs_clear_stage_flg[evs_story_no] = 1;
					}
					ret	=	MAIN_50;
				}else{	//	�Q�[���I�[�o�[
					//	�Z�[�u�f�[�^�Z�b�g
					if( evs_select_name_no[0] != DM_MEM_GUEST ){
						eep_rom_flg[0] = 1;
						dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,evs_story_no  - 1 );
					}

					ret	=	MAIN_60;
				}
			}
		}else{
			ret	=	MAIN_60;
		}
		break;
	case	GSL_2PLAY:
		for(i = 0;i < 2;i++){
			//	�L�^�̕ۑ�
			if( evs_select_name_no[i] != DM_MEM_GUEST ){
				eep_rom_flg[i] = 1;
				dm_vsman_set(&game_state_data[i],vs_win_count[i],vs_lose_count[i],evs_select_name_no[i],evs_select_name_no[i ^ 1]);
			}
		}

		ret	=	MAIN_60;
		break;
	case	GSL_VSCPU:
		if( evs_story_flg ){	//	�X�g�[���[���[�h
			if( evs_one_game_flg ){
				ret	=	MAIN_60;
			}else{
				if( game_state_data[0].game_condition[dm_static_cnd] ==  dm_cnd_win ){

					if( evs_story_no == 11 ){	//	EX�X�e�[�W
						if( !evs_secret_flg ){
							evs_secret_flg = 1;	//	�s�[�`�P�g�p�\
						}
						//	�Z�[�u�f�[�^�Z�b�g
						if( evs_select_name_no[0] != DM_MEM_GUEST ){
							eep_rom_flg[0] = 1;
							dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,0xff);
						}
						evs_story_no = 0;
						ret	=	MAIN_STAFF;
					}else{
						if( !evs_clear_stage_flg[evs_story_no] ){
							evs_clear_stage_flg[evs_story_no] = 1;
						}
						ret	=	MAIN_50;
					}
				}else{
					//	�Z�[�u�f�[�^�Z�b�g
					if( evs_select_name_no[0] != DM_MEM_GUEST ){
						dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,evs_story_no - 1 );
						eep_rom_flg[0] = 1;
					}
					if( evs_story_no == 11 ){	//	EX�X�e�[�W
						ret	=	MAIN_STAFF;
					}else{
						ret	=	MAIN_60;
					}
				}
			}
		}else{
			//	�L�^�̕ۑ�
			if( evs_select_name_no[0] != DM_MEM_GUEST ){
				eep_rom_flg[0] = 1;
				dm_vscom_set(&game_state_data[0],vs_win_count[0],vs_lose_count[0],evs_select_name_no[0],game_state_data[1].anime.cnt_charcter_no);
			}

			ret	=	MAIN_60;
		}
		break;
	case	GSL_1PLAY:
		if( evs_high_score < game_state_data[0].game_score ){
			evs_high_score = game_state_data[0].game_score;	//	�n�C�X�R�A�̍X�V
		}
		//	�Z�[�u�f�[�^�Z�b�g
		if( evs_select_name_no[0] != DM_MEM_GUEST ){
			eep_rom_flg[0] = 1;
			dm_level_sort_set( &game_state_data[0],evs_select_name_no[0] );
		}

		evs_mem_data[evs_select_name_no[0]].state_old.p_1p_lv = game_state_data[0].virus_level;

		ret	=	MAIN_60;
		break;
	case	GSL_1DEMO:
		 evs_high_score = score_save;	//	�n�C�X�R�A�����ɖ߂�
	case	GSL_2DEMO:
	case	GSL_4DEMO:
		ret	=	MAIN_TITLE;
		break;
	}

#ifdef	_DM_EEP_ROM_USE_
		//	EEP �������݃X�^�[�g
		if( eep_rom_flg[0] || eep_rom_flg[1] ){
			EepRom_CreateThread();
			EepRom_SendTask(dm_game_eep_write,(u32)&eep_err);

			//	�������ݏI���҂�
			while(1){
				if( !EepRom_GetTaskCount() ){
					EepRom_DestroyThread();
				}
			}

		}
#endif	//	_DM_EEP_ROM_USE_



	return	ret;
}

/*--------------------------------------
	�Q�[���`�惁�C���֐�
--------------------------------------*/
void	dm_game_graphic(void)
{
	NNScTask*	gt;
	s16	i,j,k;
	s16	n_pos[2] = {46,236};
	s16	s_pos[2] = {49,239};
	s16	s_pos_4p[2] = {109,179};

	// �ި���ڲؽ��ޯ̧�����ޯ̧�̎w��
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	// RSP �����ݒ�

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_SPRITE_PAGE]));	//	�A�C�e���i�J�v�Z�����j�̃Z�O�����g�ݒ�
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_1]));	//	1P�L�����N�^�̃Z�O�����g�ݒ�
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_2]));	//	2P�L�����N�^�̃Z�O�����g�ݒ�

	// �t���[���o�b�t�@�̃N���A
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);

	gSPDisplayList(gp++, S2Spriteinit_dl);

//	gSPDisplayList(gp++, ClearCfb );

	//	�`��͈͂̎w��
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	�������ݒ�
	gSPDisplayList(gp++, Texture_TE_dl );

	//	�w�i�`��

#ifdef	DM_DEBUG_FLG
	if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
	case	GSL_4PLAY:
	case	GSL_4DEMO:
		//	�w�i�`��

		load_TexPal( (u16 * )&BGBuffer[1] );
 		gSPBgRect1Cyc( gp++, &s2d_dm_bg );

/*		for(i = 0;i < 40;i++)
		{
			load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
			draw_Tex(0,(i * 6),320,6,0,0);
		}
*/		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		//	�w�i�`��
		load_TexPal( (u16 * )&BGBuffer[1] );
 		gSPBgRect1Cyc( gp++, &s2d_dm_bg );

/*
		for(i = 0;i < 26;i++)
		{
			load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
			draw_Tex(0,(i * 6) + 40,320,6,0,0);
		}
		load_TexTile_8b((u8 *)&BGBuffer[(240 * 26) + 66],320,4,0,0,319,3);
		draw_Tex(0,(26 * 6) + 40,320,4,0,0);
*/
		//	�s�[�`�X�e�[�W�ȊO�̏ꍇ
		if( !dm_peach_stage_flg ){

			//	�w�i�̏㉺�^�C���`��
			load_TexPal( dm_bg_tile_pal_data[dm_game_bg_no] );
			load_TexTile_4b( dm_bg_tile_data[dm_game_bg_no],80,40,0,0,79,39 );
			for( i = 0;i < 4;i++ ){
				draw_Tex(80 * i,0,80,40,0,0 );
				draw_Tex(80 * i,200,80,40,0,0 );
			}

			gDPSetRenderMode( gp++,G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2);
			gDPSetPrimColor( gp++,0,0,255,255,255,128);
			//	�r�̉e�`��

			load_TexPal( dm_bin_shadow_bm0_0tlut );
			for( i = 0;i < 4;i++ ){
				load_TexBlock_4b(&dm_bin_shadow_bm0_0[48 * 42 * i],96,42 );
				for( j = 0;j < 2;j++ ){
					draw_Tex( j * 184 + 20,i * 42 + 16,96,42,0,0);
				}
			}
			//	���܂�
			load_TexBlock_4b(&dm_bin_shadow_bm0_0[48 * 42 * 4],96,40 );
			for( j = 0;j < 2;j++ ){
				draw_Tex( j * 184 + 20,4 * 42 + 16,96,40,0,0);
			}

			gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
			gDPSetPrimColor(gp++,0,0,255,255,255,255);
			//	�r�̕`��
			load_TexPal( dm_bin_bm0_0tlut );
			for( i = 0;i < 4;i++ ){
				load_TexBlock_4b(&dm_bin_bm0_0[48 * 42 * i],96,42 );
				for( j = 0;j < 2;j++ ){
					draw_Tex( j * 184 + 20,i * 42 + 16,96,42,0,0);
				}
			}
			//	���܂�
			load_TexBlock_4b(&dm_bin_bm0_0[48 * 42 * 4],96,40 );
			for( j = 0;j < 2;j++ ){
				draw_Tex( j * 184 + 20,4 * 42 + 16,96,40,0,0);
			}
		}
		//	��ʒ����̏���
		if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
			load_TexPal( dm_parts_story_bm0_0tlut );
			for( i = 0;i < 6;i++ ){
				load_TexTile_8b(dm_parts_story_bm0_0 + (86 * 20 * i),86,20,0,0,85,19 );
				draw_Tex( 116,20 * i + 95,86,20,0,0);
			}
			load_TexTile_8b(dm_parts_story_bm0_0 + (86 * 20 * 6),86,6,0,0,85,5 );
			draw_Tex( 116,20 * 6 + 95,86,6,0,0);
		}else{
			load_TexPal( dm_parts_vscom_bm0_0tlut );
			for( i = 0;i < 5;i++ ){
				load_TexTile_8b(&dm_parts_vscom_bm0_0[74 * 24 * i],74,24,0,0,73,23 );
				draw_Tex( 122,24 * i + 93,74,24,0,0);
			}
			load_TexTile_8b(&dm_parts_vscom_bm0_0[74 * 24 * 5],74,13,0,0,73,12 );
			draw_Tex( 122,24 * 5 + 93,74,13,0,0);
		}
		break;
	}
#ifdef	DM_DEBUG_FLG
	}
#endif




	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			dm_virus_anime( &game_state_data[0],game_map_data[0] );		//	�E�C���X�A�j���[�V����

			dm_disp_score( game_state_data[0].game_score,94,66 );		//	���ݓ��_
			dm_disp_score( evs_high_score,94,40 );						//	�ō����_
			dm_disp_add_score();										//	���Z���_
			load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );		//	�����f�[�^���[�h
			dm_disp_number( game_state_data[0].virus_level,239,132,TLUT_virus_number );	//	�E�C���X���x��
			dm_disp_virus_number( &game_state_data[0],239,208,0 );		//	�c��E�C���X��
			dm_disp_time(evs_game_time,279,182);						//	���ԕ`��
			dm_speed_draw( &game_state_data[0],239,157 );				//	���x
			dm_anime_draw( &disp_anime_data[0][wb_flag],220,24,0 );		//	�}���I�`��

			//	�R�C��
			load_TexPal( dm_game_coin_bm0_0tlut );
			load_TexBlock_4b( &dm_game_coin_bm0_0[0],16,64 );
			for( i = 0;i < dm_retry_coin;i++ ){
				draw_Tex( (i * 16) + 40,92 + dm_retry_coin_pos[i].pos,16,16,0,dm_retry_coin_pos[i].anime[1] * 16 );
			}
			if( game_state_data[0].game_condition[dm_static_cnd] != dm_cnd_pause ){	//	�ʏ��Ԃ�����
				dm_anime_draw( &big_virus_anime[2],big_virus_position[2][0],big_virus_position[2][1],0 );				//	����E�C���X�`��
				dm_anime_draw( &big_virus_anime[0],big_virus_position[0][0],big_virus_position[0][1],0 );				//	����ԃE�C���X�`��
				dm_anime_draw( &big_virus_anime[1],big_virus_position[1][0],big_virus_position[1][1],0 );				//	���剩�E�C���X�`��
			}
			dm_game_graphic_1p( &game_state_data[0],game_map_data[0] );	//	�r�̒��g�`��
			dm_game_graphic_effect( &game_state_data[0],0,0 );			//	���o�`��

#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		j = 92;
		if( !evs_story_flg ){	//	�X�g�[���[���[�h�łȂ��ꍇ
			k = 129;
		}else{
			k = 121;
		}
#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			//	�����f�[�^�Ǎ���
			load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );

			if( evs_story_flg ){	//	�X�g�[���[���[�h�̏ꍇ
				//	�X�e�[�W�ԍ�
				dm_disp_number( evs_story_no,174,99,dm_stage_num_b_bm0_0tlut);
				//	���ԕ`��
				dm_disp_time(evs_game_time,187,162);
				//	�c��E�C���X
				for(i = 0;i < 2;i++){
					dm_disp_virus_number( &game_state_data[i],130 + (i * 36),198,i );	//	�c��E�C���X��
				}

				//	���x���`��
				load_TexPal( dm_4p_game_level_bm0_0tlut );
				load_TexTile_4b( &dm_4p_game_level_bm0_0[0],28,15,0,0,27,14 );
				draw_Tex( 146,136,28,5,0,evs_story_level * 5);
			}else{
				for(i = 0;i < 2;i++){
					dm_disp_virus_number( &game_state_data[i],130 + (i * 36),203,i );	//	�c��E�C���X��
				}
				load_TexPal( lv_num_bm0_0tlut );
				load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );
				for(i = 0;i < 2;i++){
					dm_disp_point( dm_vsmode_win_point[i] + vs_win_count[i],153 + i * 39,170 );	//	�|�C���g
				}
			}

			load_TexPal( TLUT_star );						//	���e�N�X�`�����[�h
			load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	���e�N�X�`�����[�h
			if( !evs_story_flg ){	//	�X�g�[���[���[�h�łȂ��ꍇ
				for(i = 0;i < 2;i++){
					dm_star_draw( k,96,win_count[i],1);
					k += 46;
				}
			}else{					//	�X�g�[���[���[�h�̏ꍇ
				for(i = 0;i < 2;i++){
					dm_star_draw(k,127,win_count[i],0);
					k += 62;
				}
			}

			for( i = 0;i < 2;i++ ){
				dm_virus_anime( &game_state_data[i],game_map_data[i] );		//	�E�C���X�A�j���[�V����
				dm_game_graphic_p( &game_state_data[i],game_map_data[i] );	//	�r�̒��g�`��
				dm_game_graphic_effect( &game_state_data[i],i,0 );			//	���o�`��
				dm_anime_draw( &game_state_data[i].anime,j,10,i );			//	�A�j���[�V�����`��
				j += 136;
			}

			if( dm_peach_get_flg[0] ){
				//	PEACH GET �`��
				load_TexPal( dm_get_peach_bm0_0tlut );
				load_TexTile_4b( &dm_get_peach_bm0_0[0],152,16,0,0,151,15 );
				draw_Tex( 84,120,dm_peach_get_flg[1],16,0,0 );
			}
#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif

		break;
	case	GSL_4PLAY:
	case	GSL_4DEMO:

#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			if( evs_story_flg ){	//	�X�g�[���[���[�̏ꍇ

					load_TexPal( dm_4p_game_virus_s_bm0_0tlut );	//	Virus �p���b�g���[�h
					load_TexBlock_4b( dm_4p_game_virus_s_bm0_0,48,10 );	//	Virus �O���t�B�b�N���[�h
					for( i = 0,j = 46;i < 4;i ++ ){
						draw_Tex( j,221,34,10,0,0);
						j += 72;
					}

					//	�`�[���`��
					for(i = 0,j = 24;i < 4;i++){
						if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
							load_TexPal( dm_4p_game_team_aa_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_aa_bm0_0,64,13 );
							draw_Tex( j,191,55,13,0,0);
						}else{
							load_TexPal( dm_4p_game_team_bb_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_bb_bm0_0,64,13 );
							draw_Tex( j,191,52,13,0,0);
						}
						j += 72;
					}

				load_TexPal( dm_4p_story_name_bm0_0tlut );						//	MARIO & ENEMY �p���b�g���[�h
				load_TexTile_4b( &dm_4p_story_name_bm0_0[0],38,20,0,0,37,19 );	//	MARIO & ENEMY �O���t�B�b�N���[�h
				draw_Tex( n_pos[story_4p_name_flg[1]],16,38,10,0,0);	//	ENEMY
				draw_Tex( n_pos[story_4p_name_flg[0]],16,34,10,0,10);	//	MARIO

				load_TexPal( dm_4p_cap_stock_bm0_0tlut );	//	�J�v�Z���X�g�b�N �p���b�g���[�h
				load_TexTile_4b( &dm_4p_cap_stock_bm0_0[0],40,16,0,0,39,15 );	//	�J�v�Z���X�g�b�N �O���t�B�b�N���[�h
				draw_Tex( 44,26,40,16,0,0);			//	MARIO��
				draw_Tex( 234,26,40,16,0,0);		//	ENEMY��

				load_TexPal( dm_4p_story_gage_bm0_0tlut );	//	������ �p���b�g���[�h
				load_TexTile_4b( &dm_4p_story_gage_bm0_0[0],154,24,0,0,153,23 );	//	������ �O���t�B�b�N���[�h
				draw_Tex( 96,18,130,24,0,0);		//	������
				draw_Tex( 17,18,24,24,130,0);	//	�������P�[�W
				draw_Tex( 277,18,24,24,130,0);	//	�������P�[�W


				//	���ԕ`��
				dm_disp_time_4p( evs_game_time,evs_story_level );

				load_TexPal( TLUT_star );						//	���e�N�X�`�����[�h
				load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	���e�N�X�`�����[�h
				for(i = 0,j = 22;i < 2;i++ ){
					dm_star_draw( j,21,win_count[i],0 );
					j += 260;
				}

				//	�E�C���X�O���t�B�b�N�f�[�^�̓Ǎ���
				load_TexTile_4b( cap_tex[1],16,128,0,0,15,127 );
				for( i = 0;i < 2;i++ ){
					for(j = k = 0;j < 4;j++){
						if( story_4p_stock_cap[story_4p_name_flg[i]][j] >= 0 ){
							//	�X�g�b�N������ꍇ
							load_TexPal( capsel_8_pal[story_4p_stock_cap[story_4p_name_flg[i]][j]] );	//	�p���b�g���[�h
							//	�J�v�Z���`��
							draw_Tex( s_pos[story_4p_name_flg[i]] + (k << 3),29,cap_size_8,cap_size_8,0,capsel_b << 3);
							k++;
						}
					}
				}
			}else{
				if( dm_game_4p_team_flg ){
					//	�`�[���΍R��
					load_TexPal( dm_4p_game_virus_s_bm0_0tlut );	//	Virus �p���b�g���[�h
					load_TexBlock_4b( dm_4p_game_virus_s_bm0_0,48,10 );	//	Virus �O���t�B�b�N���[�h
					for( i = 0,j = 46;i < 4;i ++ ){
						draw_Tex( j,221,34,10,0,0);
						j += 72;
					}

					//	�`�[���`��
					for(i = 0,j = 24;i < 4;i++){
						if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
							load_TexPal( dm_4p_game_team_aa_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_aa_bm0_0,64,13 );
							draw_Tex( j,191,55,13,0,0);
						}else{
							load_TexPal( dm_4p_game_team_bb_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_bb_bm0_0,64,13 );
							draw_Tex( j,191,52,13,0,0);
						}
						j += 72;
					}

					//	TEAM A �`��
					load_TexBlock_4b( &dm_4p_game_team_bm0_0[0],128,32 );
					load_TexPal( dm_4p_game_team_a_bm0_0tlut );
					draw_Tex( 24,9,128,32,0,0);
					draw_TexFlip( 24,41,128,2,0,0,flip_off,flip_on);
					//	TEAM B �`��
					load_TexPal( dm_4p_game_team_b_bm0_0tlut );
					draw_Tex( 168,9,128,32,0,0);
					draw_TexFlip( 168,41,128,2,0,0,flip_off,flip_on);
					//	�����Q�[�W
					load_TexPal( dm_4p_win_gage_bm0_0tlut );		//	�����Q�[�W�p���b�g���[�h
					load_TexTile_4b( &dm_4p_win_gage_bm0_0[0],68,24,0,0,67,23 );	//	�����Q�[�O���t�B�b�N���[�h
					draw_Tex( 32,16,66,22,2,0);
					draw_Tex(222,16,66,22,2,0);
					//	�J�v�Z���X�g�b�N
					load_TexPal( dm_4p_cap_stock_bm0_0tlut );	//	�J�v�Z���X�g�b�N �p���b�g���[�h
					load_TexTile_4b( &dm_4p_cap_stock_bm0_0[0],40,16,0,0,39,15 );	//	�J�v�Z���X�g�b�N �O���t�B�b�N���[�h
					draw_Tex( 106,24,38,14,2,0);
					draw_Tex( 176,24,38,14,2,0);


					//	�E�C���X�O���t�B�b�N�f�[�^�̓Ǎ���
					load_TexTile_4b( cap_tex[1],16,128,0,0,15,127 );
					for( i = 0;i < 2;i++ ){
						for(j = k = 0;j < 4;j++){
							if( story_4p_stock_cap[story_4p_name_flg[i]][j] >= 0 ){
								//	�X�g�b�N������ꍇ
								load_TexPal( capsel_8_pal[story_4p_stock_cap[story_4p_name_flg[i]][j]] );	//	�p���b�g���[�h
								//	�J�v�Z���`��
								draw_Tex( s_pos_4p[story_4p_name_flg[i]] + (k << 3),27,cap_size_8,cap_size_8,0,capsel_b << 3);
								k++;
							}
						}
					}

					j = 35;
					load_TexPal( TLUT_star );						//	���e�N�X�`�����[�h
					load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	���e�N�X�`�����[�h
					for(i = 0;i < 2;i++){
						dm_star_draw( j,19,win_count[i],0 );
						j += 190;
					}
				}else{
					//	�o�g�����C����
					load_TexPal( dm_4p_game_virus_bm0_0tlut );	//	Virus �p���b�g���[�h
					load_TexBlock_4b( dm_4p_game_virus_bm0_0,48,12 );	//	Virus �O���t�B�b�N���[�h
					for( i = 0,j = 31;i < 4;i ++ ){
						draw_Tex( j,192,40,12,0,0);
						j += 72;
					}

					j = 17;
					load_TexPal( dm_4p_win_gage_bm0_0tlut );		//	�����Q�[�W�p���b�g���[�h
					load_TexTile_4b( &dm_4p_win_gage_bm0_0[0],68,24,0,0,67,23 );	//	�����Q�[�O���t�B�b�N���[�h
					for(i = 0;i < 4;i++){
						draw_Tex( j,13,68,24,0,0);
						j += 72;
					}
					j = 22;
					load_TexPal( TLUT_star );						//	���e�N�X�`�����[�h
					load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	���e�N�X�`�����[�h
					for(i = 0;i < 4;i++){
						dm_star_draw( j,16,win_count[i],0 );
						j += 72;
					}
				}
			}

			if( story_4p_demo_flg[0] ){
				demo_story_4p_draw();
			}else{
				for( i = 0;i < 4;i++ ){
					dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	�E�C���X�A�j���[�V����

					dm_game_graphic_p( &game_state_data[i],game_map_data[i] );

					load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );
					dm_disp_virus_number( &game_state_data[i],game_state_data[i].map_x + 30,207,i);	//	�c��E�C���X��
					dm_4p_game_face_draw( &game_state_data[i],game_state_data[i].map_x + 7,206);	//	��
				}
				for(i = 0;i < 4;i++){
					dm_game_graphic_effect( &game_state_data[i],i,1 );	//	WIN LOSE DRAW �A�������̕`��
				}
			}
#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif
		break;
	}
	//	�f���\��
	switch( evs_gamesel )
	{
	case	GSL_1DEMO:
	case	GSL_4DEMO:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY
		break;
	case	GSL_2DEMO:
		push_any_key_draw( 128,210 );	// PUSH ANY KEY
		break;
	}

	/*------------------*/
	// �ި���ڲؽč쐬�I��, ���̨������̊J�n
	gDPFullSync(gp++);			// �ި���ڲؽčŏI����
	gSPEndDisplayList(gp++);	// �ި���ڲؽďI�[
	osWritebackDCacheAll();		// dinamic�����Ă��ׯ��

	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);


}
