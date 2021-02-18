/*--------------------------------------
	filename	:	dm_manual_main.c
	create		:	1999/09/04
	modify		:	1999/12/14

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
#include "audio.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"

#include	"dm_nab_include.h"
#include	"o_segment.h"

static	s8	dm_manual_debug_bg_no;
static	s16	dm_manual_debug_timer;

static	game_a_cap	dm_manual_at_cap[4][4];			//	��������p�U���J�v�Z��

static	game_anime	dm_manual_big_virus_anime[3];	//	����E�C���X�A�j������p�ϐ�
static	s16	dm_manual_big_virus_pos[3][3];		//	����E�C���X�̍��W
static	s8	dm_manual_big_virus_count[4];		//	����E�C���X�̍��W
static	s8	dm_manual_big_virus_flg[3][2];		//	0:���ŃA�j���[�V��������񂾂��ݒ肷��t���O 1:���ŉ�����񂾂��ݒ肷��t���O

static	s16	dm_manual_timer;				//	��������p�^�C�}�[
static	s16	dm_manual_timer_count;			//	��������p�^�C�}�[�̃J�E���^�[
static	s16	dm_manual_mode[3];				//	��������ϐ�
static	s16	dm_manual_waku_alpha[4];		//	��������p�g�_�ŗp�����x
static	s8	dm_manual_mode_stop_flg;		//	���������i�s�𐧌䂷��t���O
static	s8	dm_manual_move_vec;				//	���������p ��]�E�ړ������p�t���O
static	s8	dm_manual_move_count;			//	���������p ��]�E�ړ� �񐔗p�t���O
static	s8	dm_manual_last3_flg;			//	�E�C���X���c��R�̂Ƃ��ɖ炷���߂̃t���O
static	s8	dm_manual_at_cap_count[4];		//	��������p�U���J�v�Z�������J�E���^
static	s8	dm_manual_flash_count;			//	��������p�_�ŗp�J�E���^
static	s8	dm_manual_flash_color;			//	��������p�_�ŗp�J�E���^
static	s8	dm_manual_waku_flash_count[4];	//	��������p�_�ŗp�J�E���^
static	s8	dm_manual_waku_flash_flg[4];	//	��������p�_�ŗp�t���O

static	s8	dm_manual_1_ef_flg;				//	��������P�p�ėp���t���O
static	s8	dm_manual_3_ef_flg;				//	��������R�p�ėp���t���O
static	s8	dm_manual_6_ef_flg;				//	��������U�p�ėp���t���O

static	u8	manual_1_CapsMagazine_a[] = {	//	�����P�X�e�[�W�N���A�܂ł̃}�K�W��
	dm_manual_yy,
	dm_manual_ry,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_rr,

};

static	u8	manual_1_CapsMagazine_b[] = {	//	�����P�Q�[���I�[�o�[�܂ł̃}�K�W��
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_by,
	dm_manual_rb,
	dm_manual_ry
};

static	u8	manual_2_CapsMagazine[] = {	//	�����Q�̃}�K�W��
	dm_manual_br,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_yr,
	dm_manual_yy,
	dm_manual_br
};

static	u8	manual_3_CapsMagazine[] = {	//	�����R�̃}�K�W��
	dm_manual_ry,
	dm_manual_by,
	dm_manual_yr,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_rb,
	dm_manual_rb,
	dm_manual_rr,
	dm_manual_ry,
	dm_manual_yr,
	dm_manual_rr,
	dm_manual_by,

};


static	u8	manual_4_CapsMagazine[] = {	//	�����S�̃}�K�W��
	dm_manual_yy,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_yr,
	dm_manual_ry,
	dm_manual_br,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_br,
	dm_manual_yb,
	dm_manual_bb,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_yy
};

static	u8	manual_5_CapsMagazine[] = {	//	�����T�̃}�K�W��
	dm_manual_rr,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_rb,
	dm_manual_yr,
	dm_manual_yr,
	dm_manual_rb,
	dm_manual_rb,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_rb,
	dm_manual_yb,
	dm_manual_rr,
	dm_manual_rr,
	dm_manual_by,
	dm_manual_by,
	dm_manual_yy,
	dm_manual_yy
};

static	u8	manual_6_CapsMagazine[] = {	//	�����U�̃}�K�W��
	dm_manual_yb,
	dm_manual_by,
	dm_manual_yr,
	dm_manual_yy,
	dm_manual_rr,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_yb,
	dm_manual_yy,
	dm_manual_by,
	dm_manual_yb,
	dm_manual_br,


	//	2P�}�K�W��
	dm_manual_yy,
	dm_manual_yy,
	dm_manual_br,

};


/*--------------------------------------
	��������������֐�
--------------------------------------*/
void	dm_manual_all_init( void )
{
	s16	i,j,k;
	s16	map_x_table[][4] = {
		{dm_wold_x,dm_wold_x,dm_wold_x,dm_wold_x},							// 1P
		{dm_wold_x_vs_1p,dm_wold_x_vs_2p,dm_wold_x_vs_1p,dm_wold_x_vs_2p},	// 2P
		{dm_wold_x_4p_1p,dm_wold_x_4p_2p,dm_wold_x_4p_3p,dm_wold_x_4p_4p}	// 4P
	};
	u8	map_y_table[] = {dm_wold_y,dm_wold_y_4p};
	u8	size_table[] = {cap_size_10,cap_size_8};

	init_effect_all();		//	�G�t�F�N�g������
	dm_game_init();			//	�Q�[���p�ϐ�������

	dm_manual_last3_flg = cap_flg_off;			//	���[�`SE�t���O���n�e�e�ɂ���
	manual_flash_alpha = cap_flg_off;			//	�_�ł̃t���O�n�e�e�ɂ���
	manual_key_flash_count = cap_flg_off;		//	�L�[�_�Ŏ��Ԃn�e�e�ɂ���

	for(i = j = 0;i < 3;i++ ){
		dm_manual_big_virus_flg[i][0] = 0;
		dm_manual_big_virus_flg[i][1] = 0;
		//	����E�C���X�̍��W���v�Z����
		dm_manual_big_virus_pos[i][2] = j;	//	sin,cos�p���l
		dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
		dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
		j += 120;
	}

	switch( evs_manual_no )
	{
	case	0:	//	���̂P(�P�l�p)
	case	3:	//	���̂R(�P�l�p)
		k = 0;	//	�A�C�e���̑傫��
		j = 0;	//	��w���W
		break;
	case	1:	//	���̂Q(�Q�l�p)
	case	4:	//	���̂S(�Q�l�p)
		k = 0;	//	�A�C�e���̑傫��
		j = 1;	//	��w���W
		break;
	case	2:	//	���̂R(�S�l�p)
	case	5:	//	���̂R(�S�l�p)
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
		dm_manual_waku_flash_flg[i] = cap_flg_off;

		//	�J�v�Z���������x�̐ݒ�

		game_state_data[i].cap_def_speed = SPEED_MID;								//	�}�j���A���� SPEED_MID �ɌŒ�
		game_state_data[i].cap_speed = GameSpeed[game_state_data[i].cap_def_speed];	//	�������x�̐ݒ�

		switch(evs_manual_no)
		{
		case	2:
		case	5:
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

		//	�U���J�v�Z���̐ݒ�
		dm_manual_at_cap_count[i] = 0;
		for( j = 0;j < 4;j++ ){
			dm_manual_at_cap[i][j].pos_a_x = dm_manual_at_cap[i][j].pos_a_y = 0;
			dm_manual_at_cap[i][j].capsel_a_p = 0;
			for( k = 0;k < 3;k++ ){
				dm_manual_at_cap[i][j].capsel_a_flg[k] = 0;
			}
		}
	}

	for( i = 0;i < 3;i++ )
	{
		manual_bubble_col_flg[i] = 0;
		dm_manual_mode[i] = 0;
	}

	dm_manual_timer = dm_manual_mode_stop_flg = 0;

	evs_seqence = 2;	//	BGM�͖���
}
/*--------------------------------------
	��������p�����񑀍�֐�(�P�s�̒����擾)
--------------------------------------*/
s8		dm_get_manual_str_len(u16 *str)
{
	s8	len = 0;

	while(1){
		len++;
		if(*str == 0xffff || *str == 0xfffd){
			return	len;
		}
		str++;
	}
}

/*--------------------------------------
	��������p�����񑀍�֐�(������f�[�^��\���p�o�b�t�@�փR�s�[)
--------------------------------------*/
void	dm_set_manual_str(u16 *str)
{
	u8	i = 0;

	manual_str_timer = manual_str_count = manual_str_len = 0;		//	���b�Z�[�W�\�������̏�����

	while(1){
		manual_str_buffer[i] = *str;
		if(*str == 0xffff ){
			break;
		}
		str++;
		i++;
	}
}


/*--------------------------------------
	��������p�����񑀍�֐�(������f�[�^���o�b�t�@�ւ̍Ō���ɃR�s�[)
--------------------------------------*/
void	dm_set_manual_str_link(u16 *str)
{
	u8	i = 0;

	manual_str_timer = 0;

	while(1){
		if( manual_str_buffer[i] == 0xffff){
			break;
		}
		i++;
	}

	manual_str_buffer[i] = 0xfffd;	//	���s�R�[�h���
	i ++;

	while(1){
		manual_str_buffer[i] = *str;
		if(*str == 0xffff ){
			break;
		}
		str++;
		i++;
	}
}

/*--------------------------------------
	��������p�����񑀍�֐�(�T�s�ȏ�̂Ƃ��̂P�s���s)
--------------------------------------*/
void	dm_manual_str_next_line(void)
{
	s8	i,j,len;
	u16	*str_start,*str_end;
	u16	save_buffer[100];

	for(i = 0;i < 100;i++){
		save_buffer[i] = manual_str_buffer[i];	//	������f�[�^�̃Z�[�u
		manual_str_buffer[i] = 0xffff;			//	�I�[������f�[�^�̃Z�b�g
	}

	len = dm_get_manual_str_len(save_buffer);	//	1�s�ڂ̒��������߂�

	str_start = manual_str_buffer;
	str_end = save_buffer;

	str_end += len;	//	�P�s���i�܂���

	for(i = 0;i < (100 - len);i++){
		*str_start = *str_end;
		str_start++;
		str_end++;
	}

	//	�\���������̒���
	str_start = manual_str_buffer;
	for(i = manual_str_len = 0;i < 5;i++)
	{
		len = dm_get_manual_str_len(str_start);
		str_start += len;
		manual_str_len += len;
	}
}
/*--------------------------------------
	��������p�����񑀍�֐�(������`��)
--------------------------------------*/
s8	dm_draw_manual_str(s16 x_pos,s16 y_pos,u16 *str,s8 max)
{
	s16	i,j;
	s16	x_p;
	s8	str_max;

	i = str_max = 0;
	x_p = x_pos;
	for(;;)
	{
		//	������I��
		if(*str == 0xffff)
			return	-1;

		if(i >= max)
			return	1;

		//	�U�s�ȏ�͕`�悵�Ȃ�
		if( str_max > 5)
			break;
		switch(*str)
		{
		case	0xfff0:
		case	0xfff1:
		case	0xfff2:
		case	0xfff3:
		case	0xfff4:
			//	�p���b�g�ύX
			load_TexPal( dm_manual_font_pal[*str - 0xfff0] );
			break;
		case	0xfffd:
			//	���s
			str_max++;
			x_pos = x_p;
			y_pos += 12;
			break;
		case	0xfffe:
			//	�X�y�[�X
			x_pos += 12;
			break;
		default:
			//	�����`��
			gDPLoadTextureTile_4b(gp++,dm_manual_font[*str], G_IM_FMT_I,12,12,0,0,11,11,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
			gSPScisTextureRectangle(gp++,x_pos << 2,y_pos << 2,x_pos + 12 << 2,y_pos + 12 << 2,G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
			x_pos += 12;
			break;
		}
		i++;
		str++;
	}

	if( str_max > 4){
		//	���s���
		dm_manual_str_next_line();
		return	1;
	}
}
/*--------------------------------------
	��������p�����񑀍�֐�(������i�s)
--------------------------------------*/
s8	dm_manual_str_main(void)
{
	//	������i�s����
	if( manual_str_stop_flg ){
		//	���͕\�����̏ꍇ
		if(manual_str_end_flg > 0){
			//	���͂��I���ɒB���Ă��Ȃ�
			manual_str_count ++;

			//	�L�m�s�I�̃N�`�p�N
			if((manual_str_count % 4) == 0){
				manual_pino_anime_flg ^= 1;
			}
			if( manual_str_count > dm_manual_str_wait){
				manual_str_count = 0;
				manual_str_len ++;
			}
			return	1;
		}else{
			//	�L�m�s�I�ɃN�`�p�N�����Ȃ�
			manual_pino_anime_flg = 0;

			manual_str_timer ++;
			if(manual_str_timer >= dm_manual_str_erace_wait)
			{
				manual_str_timer = dm_manual_str_erace_wait;
				return	-1;
			}else{
				return	1;
			}
		}
	}else{
		//	�L�m�s�I�ɃN�`�p�N�����Ȃ�
		manual_pino_anime_flg = 0;
		return	-1;
	}
}
/*--------------------------------------
	��������U���J�v�Z���ݒ�֐�
--------------------------------------*/
void	dm_manual_set_attack_capsel( game_a_cap *cap,s8 pos_x,s8 color )
{
	cap -> pos_a_x = pos_x;					//	�w���W�̐ݒ�
	cap -> pos_a_y = 1;						//	�x���W�̐ݒ�
	cap -> capsel_a_p = color;				//	�F�̐ݒ�
	cap -> capsel_a_flg[cap_disp_flg] = 1;	//	�\���t���O�𗧂Ă�
}
/*--------------------------------------
	��������U���J�v�Z�����������֐�
--------------------------------------*/
void	dm_manual_attack_capsel_down( void )
{
	s8	i,j,flg;

	//	�U���J�v�Z����������
	flg = 0;
	for( i = 0;i < 4;i++ ){
		dm_manual_at_cap_count[i] ++;
		if( dm_manual_at_cap_count[i] >= dm_down_speed ){
			dm_manual_at_cap_count[i] = 0;

			for(j = 0;j < 4;j++){
				if( dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] ){
					//	�\���t���O�������Ă����ꍇ

					//	�����J�v�Z���ƃ}�b�v���̔���
					if( get_map_info( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,dm_manual_at_cap[i][j].pos_a_y + 1) ){
						//	�����J�v�Z���̉��ɃA�C�e�����������ꍇ

						//	�}�b�v���֕ύX����
						set_map( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,
												dm_manual_at_cap[i][j].pos_a_y,capsel_b,dm_manual_at_cap[i][j].capsel_a_p);
						dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;	//	�\���t���O�̏���
					}else{
						flg = 1;
						if(dm_manual_at_cap[i][j].pos_a_y < 16){
							dm_manual_at_cap[i][j].pos_a_y ++;
						}

						if(dm_manual_at_cap[i][j].pos_a_y == 16){
						//	�����ŉ��i�������ꍇ

							//	�}�b�v���֕ύX����
							set_map( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,
											dm_manual_at_cap[i][j].pos_a_y,capsel_b,dm_manual_at_cap[i][j].capsel_a_p);
							dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;	//	�\���t���O�̏���
						}
					}
				}
			}
		}
	}
	if( flg ){
		dm_set_se(dm_se_cap_a_down);
	}
}
/*--------------------------------------
	�ړ��E��]�ݒ�֐�
--------------------------------------*/
void	dm_manual_set_move( s16 now,s16 next,s8 count,s8 wait,s8 vec )
{
	dm_manual_timer = 0;					//	�����^�C�}�[�N���A
	dm_manual_mode[dm_manual_now]	= now;	//	�����ԍ��̐ݒ�( �ړ��E��]�E���������̂ǂꂩ )
	dm_manual_mode[dm_manual_next]	= next;	//	���̏����ԍ��̐ݒ�
	dm_manual_move_count = count;			//	��](�ړ�)�̉񐔂̐ݒ�
	dm_manual_move_vec = vec;				//	��](�ړ�)�����̐ݒ�
	dm_manual_timer_count = wait;			//	��](�ړ�)�̊Ԋu(����)�̐ݒ�
}
/*--------------------------------------
	��������p���o�����֐�
--------------------------------------*/
void	dm_manual_effect_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};

	if( state -> game_condition[dm_mode_now] != dm_cnd_wait && state -> game_condition[dm_mode_now] != dm_cnd_pause ){
		dm_black_up( state,map );		//	���オ�菈��
	}

	if( state -> game_condition[dm_mode_now] == dm_cnd_manual ){	//	�ʏ��Ԃ�����
		bubble_main( player_no );	//	�U�����o����
		shock_main( player_no, state -> map_x + 10,207);		//	���炢���o�ݒ�
		humming_main( player_no, state -> map_x + 20 );			//	�n�~���O���o�ݒ�
	}
}
/*--------------------------------------
	�Q�[�������֐�
--------------------------------------*/
s8	dm_manual_main_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j,out;
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};

	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:	//	�E�C���X�z�u
		return	dm_ret_virus_wait;
	case	dm_mode_wait:
		return	dm_ret_virus_wait;
	case	dm_mode_down:	//	�J�v�Z������
		dm_capsel_down( state,map );
		break;
	case	dm_mode_down_wait:	//	���n�E�F�C�g
		if( dm_check_game_over( state,map ) ){
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
			dm_h_ball_chack( map );						//	�c��������
			dm_w_ball_chack( map );						//	����������

			state -> virus_number = get_virus_color_count( map,&dm_manual_big_virus_count[0], &dm_manual_big_virus_count[1] ,&dm_manual_big_virus_count[2] );
			switch(dm_manual_debug_bg_no)
			{
			case	0:
			case	3:
				for( i = 0;i < 3;i++ )
				{
					if( dm_manual_big_virus_count[i] == 0 )
					{
						if( !dm_manual_big_virus_flg[i][0] ){
							dm_manual_big_virus_flg[i][0] = 1;
							dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_lose );	//	����E�C���X���ŃA�j���[�V�����Z�b�g
							if( state -> virus_number != 0 ){
								dm_set_se( dm_se_big_virus_damage );			//	����E�C���X�_���[�W
							}
						}
					}else{
						if( state -> chain_color[3] & (0x10 << i) ){
							dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_damage );	//	����E�C���X����A�j���[�V�����Z�b�g
							dm_set_se( dm_se_big_virus_damage );						//	����E�C���X�_���[�W

						}
					}
				}
			}
			state -> chain_color[3] &= 0x0f;	//	����E�C���X�A�j���[�V�������Z�b�g


			if( state -> virus_number == 0 ){
				//	�E�C���X����
				dm_score_make( state,1 );					//	���_�v�Z
				state -> game_condition[dm_mode_now] = dm_cnd_stage_clear;	//	�X�e�[�W�N���A
				state -> game_mode[dm_mode_now] = dm_mode_stage_clear;
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	�E�C���X���c��R�ȉ��̂Ƃ��x������炷
				if( !dm_manual_last3_flg ){
					//	�P��Ȃ炵����Ȍ�炳�Ȃ�
					dm_manual_last3_flg = 1;
					dm_set_se( dm_se_last_3 );			//	�E�C���X�c��R�r�d�Đ�
				}
			}

			state ->chain_count ++;
			if( state -> chain_color[3] & 0x08 ){
				state -> chain_color[3] &= 0xF7;		//	�E�C���X���܂ޏ��ł̃��Z�b�g
				dm_set_se( dm_se_virus_erace );			//	�E�C���X���܂ޏ��łr�d�Đ�
			}else{
				dm_set_se( dm_se_cap_erace );			//	�J�v�Z���݂̂̏��łr�d�Đ�
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
		dm_warning_h_line( state,map );		//	�ς݂�����`�F�b�N

		switch( evs_gamesel ){
		case	GSL_2PLAY:	//	2�o�̎�
		case	GSL_VSCPU:	//	VSCPU�̂Ƃ�
			set_bubble( state ,player_no,0,1,0,0,0 );			//	���o�ݒ�
			break;
		case	GSL_4PLAY:
			set_bubble( state ,player_no,1,0,manual_bubble_col_flg[0],manual_bubble_col_flg[1],manual_bubble_col_flg[2] );	//	���o�ݒ�
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	�����J�v�Z�����x�̌v�Z
			state -> chain_line = 0;			//	���Ń��C�����̃��Z�b�g
			state -> chain_count = 0;			//	�A�����̃��Z�b�g
			state -> erase_virus_count = 0;		//	���ŃE�C���X���̃��Z�b�g
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;	//	���ŐF�̃��Z�b�g
			}

			state -> game_mode[dm_mode_now] = dm_mode_down;	//	�J�v�Z������������
			//	�����O�v�l�ǉ��ʒu
			for(i = 0;i < 3;i++){
				manual_bubble_col_flg[i] = 0;
			}
		}
		break;
	case	dm_mode_stage_clear:	//	�X�e�[�W�N���A���o
		i = try_next_stage_main();
		if( i > 0){
			return	dm_ret_next_stage;
		}
		break;
	case	dm_mode_game_over:		//	�Q�[���I�[�o�[���o
		i = game_over_main();
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_win:	//	WIN���o
		i = win_main( center[flg][player_no] , player_no );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_lose:	//	LOSE���o
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_draw:	//	DRAW���o
		i = draw_main( player_no,center[flg][player_no] );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_tr_chaeck:	//	���K�m�F
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
		dm_attack_se( state,player_no );			//	�U���r�d�Đ�
		dm_warning_h_line( state,map );				//	�ς݂�����`�F�b�N
		if( game_state_data[i].retire_flg[dm_training_flg] ){	//	�U������
			if( dm_broken_set( state , map )){		//	���ꔻ��
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	������������
				out = 0;
			}
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );					//	�����J�v�Z�����x�̌v�Z
			state -> chain_line = 0;						//	���Ń��C�����̃��Z�b�g
			state -> chain_count = 0;						//	�A�����̃��Z�b�g
			state -> erase_virus_count = 0;					//	���ŃE�C���X���̃��Z�b�g
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;				//	���ŐF�̃��Z�b�g
			}
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	�J�v�Z������������
		}
		break;
	}
	return	0;
}

/*--------------------------------------
	����������̂P�������C���֐�
--------------------------------------*/
s8	dm_manual_1_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;

	state = &game_state_data[0];
	map = game_map_data[0];

	//	�L�[�_�ŃJ�E���^�[
	if(manual_key_flash_count > 0)
		manual_key_flash_count--;

	//	�����⒅�n���蓙���s���Ă���
	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,0 );
	}


	//	�^�C�}�[����
	dm_manual_timer ++;

	dm_virus_anime( state,map );	//	�E�C���X�A�j���[�V����
	for(i = 0;i < 3;i++){
		dm_anime_control( &dm_manual_big_virus_anime[i] );	//	����E�C���X�A�j���[�V����
	}
	//	����E�C���X�A�j���[�V����
	for(i = j = 0;i < 3;i++){
		if( dm_manual_big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	�����A�j���[�V�����̏ꍇ
			if( dm_manual_big_virus_anime[i].cnt_now_type[dm_manual_big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
				j++;
				if( dm_manual_big_virus_anime[i].cnt_now_frame == 20 ){
					if( !dm_manual_big_virus_flg[i][1] ){			//	����E�C���X���� SE ���Đ����Ă��Ȃ�
						dm_manual_big_virus_flg[i][1] = 1;			//	�Q��炳�Ȃ����߂̃t���O�Z�b�g
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_erace );	//	����E�C���X���� SE �Z�b�g
						}
					}
				}
			}
		}else	if( dm_manual_big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
			j ++;
		}
	}
	//	��]�i�~��`���j����
	if( j == 0 ){
		dm_manual_big_virus_count[3] ++;
		if( dm_manual_big_virus_count[3] >= 10 ){
			dm_manual_big_virus_count[3] = 0;

			for(i = 0;i < 3;i++){
				dm_manual_big_virus_pos[i][2] ++;
				if( dm_manual_big_virus_pos[i][2] >= 360 ){
					dm_manual_big_virus_pos[i][2] = 0;
				}
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
			}
		}
	}



	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 190;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 30;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 210;							//	�L�m�s�I�̍��W

		manual_str_win_flg = cap_flg_off;			//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;			//	������i�s�t���O��OFF�ɂ���

		dm_manual_1_ef_flg = 0;							//	�ėp���t���O�̏�����

		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 6;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_1_CapsMagazine_a[i];
		}
		state -> cap_magazine_cnt = 1;					//	�}�K�W���c�Ƃ�1�ɂ���
		dm_set_capsel( state );							//	�J�v�Z���ݒ�

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u
		set_virus( map,manual_main_1_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
						manual_main_1_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
							manual_main_1_virus_table_a[state -> virus_number][0],	//	�E�C���X�̐F
								virus_anime_table[manual_main_1_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�

		state -> virus_number ++;
		if( state -> virus_number > 3 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_1[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_1[1] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;				//	��������i�s�J�n
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
			state -> game_mode[dm_mode_now] = dm_mode_down;		//	�J�v�Z�������J�n
			dm_manual_mode[dm_manual_now] = 11;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	11:	//	�����Q�r���܂ŕ\��
		if( manual_str_len == 9 ){
			//	"�܂��̓J�v�Z���B"�܂ŕ`�悵����J�v�Z���������~
			dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
			dm_manual_timer = 0;							//	���������^�C�}�[�̃N���A
			dm_manual_mode[dm_manual_now] = 12;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	12:	//	�J�v�Z���_�ŏ���
		if( dm_manual_timer > 10 ){
			dm_manual_timer = 0;							//	���������^�C�}�[�̃N���A
			for( i = 0;i < 2;i++ ){
				if( state -> now_cap.capsel_p[i] < 3 ){	//	���邩������Â�����
					state -> now_cap.capsel_p[i] += 3;
				}else{									//	�Â������疾�邭����
					state -> now_cap.capsel_p[i] -= 3;
				}
			}
		}
		if( manual_str_flg < 0 ){
			//	���͕\���I��
			for( i = 0;i < 2;i++ ){
				state -> now_cap.capsel_p[i] = capsel_yellow;	//	�J�v�Z���_�Œ��~
			}
			dm_set_manual_str(manual_txt_1[2]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 20;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	20:	//	�E�֐���
		if( manual_str_len == 4 ){
			//	"�E�ցA"�܂ŕ\�������當�͐i�s��~
			manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O���n�e�e�ɂ���
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n

			dm_manual_1_ef_flg = 1;							//	���`��J�n
			dm_manual_set_move( dm_manual_translate,21,3,manual_main_time_table_1[0],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
		}
		return	dm_manual_work_on;
	case	21:	//	���֐���
		dm_manual_1_ef_flg = 0;
		dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
		manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
		if( manual_str_len == 10 ){
			//	"���ցA"�܂ŕ\�������當�͐i�s��~
			manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O���n�e�e�ɂ���
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_1_ef_flg = 2;							//	���`��J�n
			dm_manual_set_move( dm_manual_translate,22,6,manual_main_time_table_1[1],cap_turn_l);	//	�ړ������̐ݒ�(���U��)
		}
		return	dm_manual_work_on;
	case	22:	//	���֐���
		dm_manual_1_ef_flg = 0;							//	���`��J�n
		dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
		manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
		if( manual_str_len == 13 ){
			//	"���ցA"�܂ŕ\�������獂�������J�n
			dm_manual_timer = 0;							//	���������^�C�}�[�̃N���A
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
			dm_manual_1_ef_flg = 3;							//	���`��J�n
			manual_key_flash_pos = 1;						//	�_�ł�����L�[�̐ݒ�i���j
			dm_manual_mode[dm_manual_now] = 23;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	23:
		//	�����҂�
		manual_key_flash_count = 4;						//	�L�[�_�Ŏ��Ԃ̐ݒ�
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	�L�[�_�Ŏ��Ԃ̐ݒ�
			dm_manual_1_ef_flg = 0;							//	���`��I��
			state -> cap_speed_vec = 1;						//	�J�v�Z���������x��߂�
			dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
			dm_manual_mode[dm_manual_now] = 24;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	24:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_set_manual_str(manual_txt_1[3]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_set_move( dm_manual_wait,30,0,30,cap_turn_r);	//	�����܂ł̑҂�����
		}
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,3,manual_main_time_table_1[2],cap_turn_r);	//	��]�����̐ݒ�(�E�R��)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_rotate,32,3,manual_main_time_table_1[2],cap_turn_l);	//	��]�����̐ݒ�(���R��)
		return	dm_manual_work_on;
	case	32:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){			//	�J�v�Z��������������
			dm_set_manual_str(manual_txt_1[4]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_1_ef_flg = 4;									//	�_�Řg( �J�v�Z���̎��� )�̕\���J�n
			dm_manual_set_move( dm_manual_wait,35,0,20,cap_turn_r);	//	20��Ă̑҂�����
		}
		return	dm_manual_work_on;
	case	35:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_1[3],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 36;		//	����������̏����ݒ�
		return	dm_manual_work_on;
	case	36:
		if(game_map_data[0][(13 << 3) + 3].capsel_m_flg[cap_condition_flg]){	//	���ŏ�ԑ҂�
			manual_str_stop_flg = cap_flg_on;									//	������i�s�J�n
			dm_manual_mode_stop_flg = cap_flg_off;								//	��������i�s��~
			dm_manual_mode[dm_manual_now] = 37;									//	���̏�����
		}
		return	dm_manual_work_on;
	case	37:
		if( manual_str_flg < 0 ){
			dm_manual_1_ef_flg = 0;							//	�_�Řg( �J�v�Z���̎��� )�̕\���I��
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_mode[dm_manual_now] = 40;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	38:
		if( !game_map_data[0][(12 << 3) + 4].capsel_m_flg[cap_disp_flg] ){
			dm_manual_mode[dm_manual_now] = 40;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	40:
		//	���ő҂�
		if( game_map_data[0][(15 << 3) + 4].capsel_m_g == capsel_b ){
			dm_set_manual_str(manual_txt_1[5]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			dm_manual_mode[dm_manual_now] = 41;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	41:
		//	"���́A"�܂ŕ\�������獂�������J�n & ���͐i�s��~
		if( manual_str_len == 4 ){
			dm_manual_set_move( dm_manual_wait,42,0,20,cap_turn_r);	//	20��Ă̑҂�����
			manual_str_stop_flg = cap_flg_off;						//	������i�s��~
			dm_manual_1_ef_flg = 5;									//	�_�Řg( �J�v�Z���̎��� )�̕\���J�n
		}
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move(dm_manual_translate,dm_manual_down,1,manual_main_time_table_1[4],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 43;		//	����������̏����ݒ�
		return	dm_manual_work_on;
	case	43:
		//	���ő҂�(�J�v�Z����Ԃ�����ŏ�ԂɂȂ�܂ő҂�)
		if( game_map_data[0][(15 << 3) + 5].capsel_m_flg[cap_condition_flg] ){
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
			dm_manual_mode[dm_manual_now] = 44;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	44:
		if( manual_str_flg < 0 ){
			manual_str_stop_flg = cap_flg_off;				//	������i�s��~
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_mode[dm_manual_now] = 50;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	50:
		if( !game_map_data[0][(15 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			dm_manual_1_ef_flg = 0;							//	�_�Řg( �J�v�Z���̎��� )�̕\���I��
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
			dm_set_manual_str(manual_txt_1[6]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 51;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	51:
		if( manual_str_len == 20 ){
			//	"�r�̒��̃E�C���X��S�ď�������"�܂ŕ\��������
			manual_str_stop_flg = cap_flg_off;				//	������i�s��~
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_1[5],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,60,2,manual_main_time_table_1[6],cap_turn_l);	//	�ړ������̐ݒ�(���Q��)
		return	dm_manual_work_on;
	case	60:
		if( ret == dm_ret_clear ){
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 61;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	61:
		if( ret == dm_ret_next_stage ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 62;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	62:
		if(dm_manual_timer > manual_main_time_table_1[7] && manual_str_flg < 0 ){
			//	�Q�[���I�[�o�[�p������
			for( i = 0;i < 9;i ++ ){						//	�����J�v�Z���쐬
				CapsMagazine[i + 1] = manual_1_CapsMagazine_b[i];
			}
			state -> game_mode[dm_mode_now] = dm_mode_init;		//	������������������[�h�ɐݒ�
			state -> game_condition[dm_mode_now] = dm_cnd_init;	//	��Ԃ���������Ԃɐݒ�

			state -> virus_number = 0;	//	�E�C���X���̃N���A
			state -> warning_flg = 0;	//	�x�����t���O�̃N���A

			//	�J�v�Z���������x�̐ݒ�
			state -> cap_speed = GameSpeed[state -> cap_def_speed];
			state -> cap_speed_vec = 1;								//	�����J�E���^�����l
			state -> cap_magazine_cnt = 1;							//	�}�K�W���c�Ƃ�1�ɂ���
			state -> cap_count = state ->cap_speed_count = 0;		//	�J�E���^�̏�����
			//	�J�v�Z�����̏����ݒ�
			dm_set_capsel( state );

			//	�E�C���X�A�j���[�V�����̐ݒ�
			state -> erase_anime = 0;								//	���ŃA�j���[�V�����R�}���̏�����
			state -> erase_anime_count = 0;							//	���ŃA�j���[�V�����J�E���^�̏�����
			state -> erase_virus_count = 0;							//	���ŃE�C���X�J�E���^�̏�����

			for(i = j = 0;i < 3;i++ ){
				dm_manual_big_virus_flg[i][0] = 0;
				dm_manual_big_virus_flg[i][1] = 0;
				dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_nomal );
				//	����E�C���X�̍��W���v�Z����
				dm_manual_big_virus_pos[i][2] = j;	//	sin,cos�p���l
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
				j += 120;
			}


			//	�A�����̏�����
			state -> chain_count = state -> chain_line =  0;

			clear_map_all( map );									//	�}�b�v���̃N���A

			manual_str_win_flg = cap_flg_off;						//	�����\�����Ȃ��悤�ɂ���
			manual_str_stop_flg = cap_flg_off;						//	������i�s��~
			dm_manual_mode_stop_flg = cap_flg_off;					//	��������i�s�J�n

			dm_manual_mode[dm_manual_now] = 70;						//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	70:	//	�Q�[���I�[�o�[�����p�E�C���X�z�u
		set_virus( map,manual_main_1_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
			manual_main_1_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
				manual_main_1_virus_table_b[state -> virus_number][0],	//	�E�C���X�̐F
					virus_anime_table[manual_main_1_virus_table_b[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�

		state -> virus_number ++;
		if( state -> virus_number > 43 ){
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			manual_str_win_flg = cap_flg_on;						//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;						//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str(manual_txt_1[7]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������i�s�J�n
			dm_manual_mode[dm_manual_now] = 71;						//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	71:
		if( ret == dm_ret_game_over ){
			state -> game_condition[dm_mode_now] = dm_cnd_game_over;	//	�Q�[���I�[�o�[
			state -> game_mode[dm_mode_now] = dm_mode_game_over;
			state -> virus_anime_spead 		= v_anime_speed;			//	�A�j���[�V�������x�𑁂�����

			auSeqPlayerPlay(0,SEQ_Lose);						//	���y�Đ�
			manual_bgm_flg = 1;									//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;								//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_a;			//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 72;					//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	72:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 73;
		}
		return	dm_manual_work_on;
	case	73:
		if(dm_manual_timer > manual_main_time_table_1[8] && manual_str_flg < 0 ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_set_manual_str(manual_txt_1[8]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 80;
		}
		return	dm_manual_work_on;
	case	80:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	�L�[�_�Ŏ��Ԃ̐ݒ�
			//	�������Ȃ��Ȃ���
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
			manual_key_flash_count = 4;						//	�L�[�_�Ŏ��Ԃ̐ݒ�
			manual_key_flash_pos = 1;						//	�_�ł�����L�[�̐ݒ�i���j
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 2;								//	�_�ł�����L�[�̐ݒ�i�E�j
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 0;								//	�_�ł�����L�[�̐ݒ�i���j
			}

			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 4;								//	�_�ł�����L�[�̐ݒ�i�`�j
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 3;								//	�_�ł�����L�[�̐ݒ�i�a�j
			}

			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}

}

/*--------------------------------------
	����������̂Q�������C���֐�
--------------------------------------*/
s8	dm_manual_2_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,1 );
		dm_manual_main_cnt( &game_state_data[1],game_map_data[1],1,1 );
	}
	dm_manual_effect_cnt( state,map,0,1 );	//	�A�����̓���\���̏���
	dm_manual_attack_capsel_down();			//	�U���J�v�Z���̗�������

	//	�^�C�}�[����
	dm_manual_timer ++;

	for(i = 0;i < 2;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	�E�C���X�A�j���[�V����
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 102;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 30;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 128;							//	�L�m�s�I�̍��W


		manual_str_win_flg = cap_flg_off;			//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;			//	������i�s�t���O��OFF�ɂ���

		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 12;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_2_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	�}�K�W���c�Ƃ�1�ɂ���
		dm_set_capsel( state );							//	�J�v�Z���ݒ�

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u

		set_virus( map,manual_main_2_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
						manual_main_2_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
							manual_main_2_virus_table_a[state -> virus_number][0],	//	�E�C���X�̐F
								virus_anime_table[manual_main_2_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�

		if( game_state_data[1].virus_number < 4 ){
			set_virus( game_map_data[1],manual_main_2_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_2_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_2_virus_table_b[state -> virus_number][0],	//	�E�C���X�̐F
									virus_anime_table[manual_main_2_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	�A�j���ԍ�
			game_state_data[1].virus_number ++;
		}

		state -> virus_number ++;

		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_2[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_2[1] );			//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 11;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	11:	//	�����Q�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_2[2] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;				//	��������i�s�J�n
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
			state -> game_mode[dm_mode_now] = dm_mode_down;		//	�J�v�Z�������J�n
			dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		}
		return	dm_manual_work_on;
	case	20:	//	�A���Ƃ�
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_2[0],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 21;			//	���̏�����
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_wait,22,1,20,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;
	case	22:
		dm_manual_set_move( dm_manual_rotate,23,2,manual_main_time_table_2[1],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_2[1],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	24:
		//	���ő҂�
		if( !game_map_data[0][(8 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 1;										//	�L�m�s�I�̌����ύX�i�E�����j

			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],2,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_red );
			dm_manual_at_cap_count[1] = 0;

			dm_manual_set_move( dm_manual_rotate,25,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	25:
		dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_set_manual_str(manual_txt_2[3]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����ύX�i�������j
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_set_move( dm_manual_rotate,26,1,manual_main_time_table_2[2],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;dm_manual_wait;
	case	26:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_2[3],cap_turn_r);	//	�ړ������̐ݒ�(�E�S��)
		dm_manual_mode[dm_manual_next_next] = 27;			//	���̏�����
		return	dm_manual_work_on;
	case	27:
		dm_manual_set_move( dm_manual_rotate,30,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,2,manual_main_time_table_2[4],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_2[5],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
		dm_manual_mode[dm_manual_next_next] = 32;			//	���̏�����
		return	dm_manual_work_on;
	case	32:
		dm_set_manual_str_link(manual_txt_2[4]);		//	�������̃f�[�^���o�b�t�@�ɂ�������
		dm_manual_mode[dm_manual_now] = 33;				//	���̏�����
		return	dm_manual_work_on;
	case	33:
		if( state -> now_cap.pos_y[0] == 13){
			dm_manual_timer = 0;												//	�^�C�}�[�N���A
			dm_manual_set_move( dm_manual_translate,34,2,manual_main_time_table_2[6],cap_turn_r);	//	�ړ������̐ݒ�(�E�Q��)
		}
		return	dm_manual_work_on;
	case	34:
		//	���ő҂�
		if( !game_map_data[0][(14 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 1;										//	�L�m�s�I�̌����ύX�i�E�����j

			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],4,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;
			dm_manual_timer = 0;										//	�^�C�}�[�N���A
			dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		// 30���
		}
		return	dm_manual_work_on;
	case	35:
		dm_manual_mode_stop_flg = cap_flg_off;			//	��������i�s��~
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_set_manual_str_link(manual_txt_2[5]);		//	�������̃f�[�^���o�b�t�@�ɂ�������

			manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����ύX�i�������j
			dm_manual_mode_stop_flg = cap_flg_on;			//	��������i�s�J�n
			dm_manual_set_move( dm_manual_rotate,36,1,manual_main_time_table_2[7],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_2[8],cap_turn_l);	//	�ړ������̐ݒ�(���R��)
		dm_manual_mode[dm_manual_next_next] = 37;			//	���̏�����
		return	dm_manual_work_on;
	case	37:
		dm_manual_set_move( dm_manual_wait,38,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	38:
		if( state -> now_cap.pos_y[0] == 6){
			dm_manual_set_move( dm_manual_translate,40,3,manual_main_time_table_2[9],cap_turn_l);	//	�ړ������̐ݒ�(���R��)
		}
		return	dm_manual_work_on;
	case	40:
		//	����J�v�Z�������҂�
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_2[10],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 42;			//	���̏�����
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_wait,43,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move(dm_manual_rotate,44,2,manual_main_time_table_2[11],cap_turn_r);					//	��]�����̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_2[12],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 45;			//	���̏�����
		return	dm_manual_work_on;
	case	45:
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	46:
		if( state -> now_cap.pos_y[0] == 8){
			dm_manual_set_move( dm_manual_translate,50,2,manual_main_time_table_2[13],cap_turn_l);			//	�ړ������̐ݒ�(���Q��)
		}
		return	dm_manual_work_on;
	case	50:
		//	���ő҂�
		if( !game_map_data[0][(13 << 3)].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;								//	�����\���t���O���n�e�e�ɂ���
			manual_pino_flip_flg = 1;										//	�L�m�s�I�̌����ύX�i�E�����j

			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],0,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],2,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],5,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][3],7,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			dm_manual_timer = 0;										//	�^�C�}�[�N���A

			dm_manual_mode[dm_manual_now] = 51;				//	���̏�����

		}
		return	dm_manual_work_on;
	case	51:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 0;										//	�L�m�s�I�̌����ύX�i�������j
		}
		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 60;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	60:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 61;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	61:
		if( dm_manual_timer > manual_main_time_table_2[11]){
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			manual_str_win_flg = cap_flg_on;				//	���b�Z�[�W�E�C���h�E�̕`��n�m
			dm_set_manual_str(manual_txt_2[6]);	//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 70;	//	���̏�����
		}
		return	dm_manual_work_on;
	case	70:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	�������Ȃ��Ȃ���
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	����������̂R�������C���֐�
--------------------------------------*/
s8	dm_manual_3_main(void)
{
	s16	i,j;
	s8	ret;
	s8	map_table[] = {0,2,3};
	game_state	*state;
	game_map	*map;
	s8	flash_cap_pos_table_blue[] = {59,67,83,91};	//	�_�ł�����J�v�Z��(��)�̈ʒu���(�z��ԍ�)
	s8	flash_cap_pos_table_red[] = {62,86,94,102};	//	�_�ł�����J�v�Z��(��)�̈ʒu���(�z��ԍ�)
	s8	flash_cap_pos_table_yellow[] = {58,82,90};	//	�_�ł�����J�v�Z��(��)�̈ʒu���(�z��ԍ�)


	state = &game_state_data[1];
	map = game_map_data[1];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,1,2 );
		for( i = 0;i < 3;i++ ){
			dm_manual_main_cnt( &game_state_data[map_table[i]],game_map_data[map_table[i]],map_table[i],2 );
		}
	}
	dm_manual_effect_cnt( state,map,1,2 );	//	�A�����̓���\���̏���
	dm_manual_attack_capsel_down();

	//	�^�C�}�[����
	dm_manual_timer ++;

	for(i = 0;i < 4;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	�E�C���X�A�j���[�V����
	}

	//	�_�ŏ���
	switch( dm_manual_3_ef_flg )
	{
	case	1:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_blue ){
				dm_manual_flash_color = capsel_b_blue;	//	�Â��F
			}else{
				dm_manual_flash_color = capsel_blue;
			}
			for( i = 0;i < 4;i++ ){
				( map + flash_cap_pos_table_blue[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	case	2:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_yellow ){
				dm_manual_flash_color = capsel_b_yellow;	//	�Â��F
			}else{
				dm_manual_flash_color = capsel_yellow;
			}
			for( i = 0;i < 3;i++ ){
				( map + flash_cap_pos_table_yellow[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	case	3:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_red ){
				dm_manual_flash_color = capsel_b_red;	//	�Â��F
			}else{
				dm_manual_flash_color = capsel_red;
			}
			for( i = 0;i < 4;i++ ){
				( map + flash_cap_pos_table_red[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	default:
		dm_manual_flash_count = 0;
		break;
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 175;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 15;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 240;							//	�L�m�s�I�̍��W


		manual_str_win_flg = cap_flg_off;				//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O��OFF�ɂ���

		dm_manual_3_ef_flg = 0;							//	�_�ŏ�������
		dm_manual_flash_count = 0;						//	�_�ŗp�J�E���^������
		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 12;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_3_CapsMagazine[i];
		}
		for( i = 0;i < 4;i++ ){
			game_state_data[i].cap_magazine_cnt = 1;	//	�}�K�W���c�Ƃ�1�ɂ���
			dm_set_capsel( &game_state_data[i] );		//	�J�v�Z���ݒ�
		}

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u

		set_virus( map,manual_main_3_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
						manual_main_3_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
							manual_main_3_virus_table_a[state -> virus_number][0],	//	�E�C���X�̐F
								virus_anime_table[manual_main_3_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�

		for( i = 0;i < 3;i++ ){
			if( state -> virus_number < 4 ){
				set_virus( game_map_data[map_table[i]],
							manual_main_3_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
								manual_main_3_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
									manual_main_3_virus_table_b[state -> virus_number][0],		//	�E�C���X�̐F
										virus_anime_table[manual_main_3_virus_table_b[state -> virus_number][0]][game_state_data[map_table[i]].virus_anime] );	//	�A�j���ԍ�
			}
		}

		state -> virus_number ++;

		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_3[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_3[1] );			//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 11;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	11:	//	�����Q�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_3[2] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 12;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	12:	//	�����R�\��
		if( manual_str_flg < 0 ){
			dm_manual_3_ef_flg = 1;									//	�_�ŊJ�n
			dm_set_manual_str( manual_txt_3[3] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������i�s�J�n
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_translate,21,1,manual_main_time_table_3[0],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_3[1],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 22;					//	���̏�����
		return	dm_manual_work_on;
	case	22:
		dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_3[2],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	24:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_translate,25,1,manual_main_time_table_3[3],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	25:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;												//	���ł̂��ߓ_�Œ��~
			for(i = 0;i < 4;i++){
				( map + flash_cap_pos_table_blue[i] ) -> capsel_m_p = capsel_blue;	//	���̐F�ɖ߂�
			}
			dm_manual_mode[dm_manual_now] = 26;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	26:
		//	���Ŕ���
		if( game_map_data[1][(14 << 3) + 5].capsel_m_g == capsel_b ){
			manual_bubble_col_flg[2] = 1;	//	�F�o�u���ł��グ
			dm_manual_set_attack_capsel( &dm_manual_at_cap[2][0],0,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[2][1],7,capsel_yellow );
			dm_manual_at_cap_count[2] = 0;

			dm_manual_3_ef_flg = 2;										//	�_�ŐF�ύX
			manual_str_win_flg = cap_flg_off;							//	�����\���t���O���n�e�e�ɂ���
			dm_set_manual_str(manual_txt_3[4]);							//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_set_move( dm_manual_wait,27,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	27:
		dm_manual_set_move( dm_manual_rotate,28,2,manual_main_time_table_3[4],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	28:
		if( dm_manual_at_cap[2][0].pos_a_y >= 8){
			manual_str_win_flg = cap_flg_on;														//	�����\���t���O���n�m�ɂ���
		}
		if( state -> now_cap.pos_y[0] == 9){
			dm_manual_set_move( dm_manual_translate,30,2,manual_main_time_table_3[5],cap_turn_l);	//	�ړ������̐ݒ�(���Q��)
		}
		return	dm_manual_work_on;
	case	30:
		if( manual_str_len == 33){
			manual_str_stop_flg = cap_flg_off;																//	������i�s��~
			dm_manual_set_move( dm_manual_rotate,31,1,manual_main_time_table_3[6],cap_turn_r);				//	��]�����̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_3[7],cap_turn_l);	//	�ړ������̐ݒ�(���Q��)
		dm_manual_mode[dm_manual_next_next] = 32;			//	���̏�����
		return	dm_manual_work_on;
	case	32:
		dm_manual_set_move( dm_manual_wait,33,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_rotate,34,1,manual_main_time_table_3[8],cap_turn_r);					//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	34:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			manual_str_stop_flg = cap_flg_on;					//	������i�s�ĊJ
			dm_manual_set_move( dm_manual_translate,35,1,manual_main_time_table_3[9],cap_turn_l);		//	�ړ������̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	35:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;														//	���ł̂��ߓ_�Œ��~
			for(i = 0;i < 3;i++){
				( map + flash_cap_pos_table_yellow[i] ) -> capsel_m_p = capsel_yellow;	//	���̐F�ɖ߂�
			}
			dm_manual_mode[dm_manual_now] = 36;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	36:
		//	���ő҂�
		if( !game_map_data[1][(12 << 3) + 1].capsel_m_flg[cap_disp_flg] ){

			manual_str_win_flg = cap_flg_off;					//	���b�Z�[�W�E�C���h�E�̕`��n�e�e

			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_blue );
			dm_manual_at_cap_count[3] = 0;
			manual_bubble_col_flg[1] = 1;	//	���F�o�u���ł��グ

			dm_manual_mode[dm_manual_now] = 37;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	37:
		//	�L�m�s�I����
		manual_pino_pos -= 3;
		if( manual_pino_pos <= 160)
		{
			dm_manual_3_ef_flg = 3;				//	���ŊJ�n��
			manual_pino_flip_flg = 1;			//	�L�m�s�I�̌����i�E�����j
			dm_set_manual_str(manual_txt_3[5]);	//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_pos = 160;
			dm_manual_mode[dm_manual_now] = 40;	//	���̏�����
		}
		return	dm_manual_work_on;
	case	40:
		if( dm_manual_at_cap[3][0].pos_a_y >= 8){							//	�U���J�v�Z���̂x���W���W�ȏ�ɂȂ�����
			manual_str_win_flg = cap_flg_on;								//	�����\���t���O���n�m�ɂ���
			manual_pino_flip_flg = 0;										//	�L�m�s�I�̌����i�������j
		}
		if( state -> now_cap.pos_y[0] == 9){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_3[10],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
			dm_manual_mode[dm_manual_next_next] = 41;
		}
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_rotate,42,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	42:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_3[11],cap_turn_r);		//	�ړ������̐ݒ�(�E�R��)
			dm_manual_mode[dm_manual_next_next] = 43;
		}
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move( dm_manual_wait,44,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_rotate,45,1,manual_main_time_table_3[12],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	45:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_translate,46,3,manual_main_time_table_3[13],cap_turn_r);		//	�ړ������̐ݒ�(�E�R��)
		}
		return	dm_manual_work_on;
	case	46:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;													//	���ł̂��ߓ_�Œ��~
			for(i = 0;i < 4;i++){
				( map + flash_cap_pos_table_red[i] ) -> capsel_m_p = capsel_red;	//	���̐F�ɖ߂�
			}
			dm_manual_mode[dm_manual_now] = 50;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	50:
		//	���ő҂�
		if( !game_map_data[1][(14 << 3) + 7].capsel_m_flg[cap_disp_flg] ){

			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][0],2,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][1],4,capsel_yellow );
			dm_manual_at_cap_count[0] = 0;
			manual_bubble_col_flg[0] = 1;	//	�ԐF�o�u���ł��グ

			dm_set_manual_str(manual_txt_3[6]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g

			dm_manual_set_move( dm_manual_wait,51,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	51:
		dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_3[14],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,53,1,manual_main_time_table_3[15],cap_turn_l);		//	��]�����̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	53:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_set_manual_str(manual_txt_3[7]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_set_move( dm_manual_wait,54,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	54:
		dm_manual_set_move( dm_manual_rotate,55,1,manual_main_time_table_3[16],cap_turn_l);		//	��]�����̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	55:
		//	���ő҂�
		if( !game_map_data[1][(15 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;							//	�����\���t���O���n�e�e�ɂ���

			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][1],6,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_yellow );
			dm_manual_at_cap_count[0] = 0;
			dm_manual_at_cap_count[3] = 0;
			manual_bubble_col_flg[0] = 1;	//	�ԐF�o�u���ł��グ
			manual_bubble_col_flg[1] = 1;	//	���F�o�u���ł��グ

			dm_manual_set_move( dm_manual_wait,60,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	60:
		dm_manual_set_move( dm_manual_translate,61,3,manual_main_time_table_3[17],cap_turn_l);		//	�ړ������̐ݒ�(���R��)
		return	dm_manual_work_on;
	case	61:
		dm_manual_set_move( dm_manual_rotate,62,1,manual_main_time_table_3[18],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	62:
		//	���ő҂�
		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			for( i = 0;i < 3;i++ ){
				game_state_data[map_table[i]].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
				game_state_data[map_table[i]].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE
			}

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 63;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	63:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 64;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	64:
		if( dm_manual_timer > manual_main_time_table_3[19]){
			manual_str_win_flg = cap_flg_on;				//	���b�Z�[�W�E�C���h�E�̕`��n�m
			dm_set_manual_str(manual_txt_3[8]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 70;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	70:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	�������Ȃ��Ȃ���
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;


			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}

		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;


			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	����������̂S�������C���֐�
--------------------------------------*/
s8	dm_manual_4_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;

	state = &game_state_data[0];
	map = game_map_data[0];

	//	�L�[�_�ŃJ�E���^�[
	if(manual_key_flash_count > 0)
		manual_key_flash_count--;

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,0 );
	}
	dm_manual_effect_cnt( state,map,0,0 );	//	�A�����̓���\���̏���

	//	�^�C�}�[����
	dm_manual_timer ++;

	dm_virus_anime( state,map );	//	�E�C���X�A�j���[�V����
	for(i = 0;i < 3;i++){
		dm_anime_control( &dm_manual_big_virus_anime[i] );	//	����E�C���X�A�j���[�V����
	}
	//	����E�C���X�A�j���[�V����
	for(i = j = 0;i < 3;i++){
		if( dm_manual_big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	�����A�j���[�V�����̏ꍇ
			if( dm_manual_big_virus_anime[i].cnt_now_type[dm_manual_big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
				j++;
				if( dm_manual_big_virus_anime[i].cnt_now_frame == 20 ){
					if( !dm_manual_big_virus_flg[i][1] ){			//	����E�C���X���� SE ���Đ����Ă��Ȃ�
						dm_manual_big_virus_flg[i][1] = 1;			//	�Q��炳�Ȃ����߂̃t���O�Z�b�g
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_erace );	//	����E�C���X���� SE �Z�b�g
						}
					}
				}
			}
		}else	if( dm_manual_big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
			j ++;
		}
	}
	//	��]�i�~��`���j����
	if( j == 0 ){
		dm_manual_big_virus_count[3] ++;
		if( dm_manual_big_virus_count[3] >= 10 ){
			dm_manual_big_virus_count[3] = 0;

			for(i = 0;i < 3;i++){
				dm_manual_big_virus_pos[i][2] ++;
				if( dm_manual_big_virus_pos[i][2] >= 360 ){
					dm_manual_big_virus_pos[i][2] = 0;
				}
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
			}
		}
	}


	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 190;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 30;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 210;							//	�L�m�s�I�̍��W

		manual_str_win_flg = cap_flg_off;				//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O��OFF�ɂ���

		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 27;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_4_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	�}�K�W���c�Ƃ�1�ɂ���
		dm_set_capsel( state );							//	�J�v�Z���ݒ�

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u
		set_virus( map,manual_main_4_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
						manual_main_4_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
							manual_main_4_virus_table_a[state -> virus_number][0],	//	�E�C���X�̐F
								virus_anime_table[manual_main_4_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�

		state -> virus_number ++;
		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_4[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_4[1] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 12;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	11:	//	�����Q�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_4[2] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_timer = 0;								//	�^�C�}�[�̃N���A
			dm_manual_mode[dm_manual_now] = 12;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	12:	//	�J�v�Z���_�ŏ���
		if( dm_manual_timer > 10 ){
			dm_manual_timer = 0;							//	���������^�C�}�[�̃N���A
			for( i = 0;i < 2;i++ ){
				if( state -> now_cap.capsel_p[i] < 3 ){	//	���邩������Â�����
					state -> now_cap.capsel_p[i] += 3;
				}else{									//	�Â������疾�邭����
					state -> now_cap.capsel_p[i] -= 3;
				}
			}
		}
		if( manual_str_flg < 0 ){
			//	���͕\���I��
			for( i = 0;i < 2;i++ ){
				state -> now_cap.capsel_p[i] = capsel_yellow;	//	�J�v�Z���_�Œ��~
			}
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������i�s�J�n
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			dm_set_manual_str( manual_txt_4[2] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_set_move( dm_manual_wait,13,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	13:
		dm_manual_set_move( dm_manual_translate,14,2,manual_main_time_table_4[0],cap_turn_l);	//	�ړ������w��(���Q��)
		return	dm_manual_work_on;
	case	14:
		dm_manual_set_move( dm_manual_rotate,15,1,manual_main_time_table_4[1],cap_turn_r);	//	��]�����w��(�E�P��)
		return	dm_manual_work_on;
	case	15:
		manual_key_flash_count = 4;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
		manual_key_flash_pos = 1;										//	�_�ł�����L�[�̐ݒ�i���j
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 12 ){
			manual_key_flash_count = 4;										//	�L�[�_�Œ�~
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,16,2,manual_main_time_table_4[2],cap_turn_r);	//	�ړ������w��(�E�Q��)
		}
		return	dm_manual_work_on;
	case	16:
		if( state -> now_cap.pos_y[0] == 16 ){
			dm_manual_set_move( dm_manual_rotate,17,1,manual_main_time_table_4[3],cap_turn_l);	//	��]�����w��(���P��)
		}
		return	dm_manual_work_on;
	case	17:
		//	�P�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_translate,21,3,manual_main_time_table_4[4],cap_turn_r);	//	�ړ������w��(�E�R��)
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[5],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 22;					//	���̏�����
		return	dm_manual_work_on;
	case	22:
		//	�Q�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,2,manual_main_time_table_4[6],cap_turn_l);	//	�ړ������w��(���Q��)
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,25,1,manual_main_time_table_4[7],cap_turn_l);	//	��]�����w��(���P��)
		return	dm_manual_work_on;
	case	25:
		manual_key_flash_count = 4;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
		manual_key_flash_pos = 1;										//	�_�ł�����L�[�̐ݒ�i���j
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 12 ){
			manual_key_flash_count = 0;									//	�L�[�_�Œ�~
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[8],cap_turn_l);	//	�ړ������w��(���P��)
			dm_manual_mode[dm_manual_next_next] = 30;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	30:
		//	�R�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,31,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_4[9],cap_turn_r);	//	�ړ������w��(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 32;					//	���̏�����
		return	dm_manual_work_on;
	case	32:
		//	�S�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,33,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[10],cap_turn_r);	//	��]�����w��(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 34;					//	���̏�����
		return	dm_manual_work_on;
	case	34:
		//	�T�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	35:
		manual_str_win_flg = cap_flg_off;				//	�����\���t���O���n�e�e�ɂ���
		dm_manual_set_move( dm_manual_translate,36,2,manual_main_time_table_4[10],cap_turn_l);	//	�ړ������w��(���Q��)
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[11],cap_turn_r);	//	��]�����w��(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 40;					//	���̏�����
		return	dm_manual_work_on;
	case	40:
		//	�U�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,42,1,manual_main_time_table_4[12],cap_turn_r);	//	�ړ������w��(�E�P��)
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[13],cap_turn_r);	//	��]�����w��(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 43;					//	���̏�����
		return	dm_manual_work_on;
	case	43:
		//	�V�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,44,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[14],cap_turn_r);	//	��]�����w��(�E�R��)
		dm_manual_mode[dm_manual_next_next] = 45;					//	���̏�����
		return	dm_manual_work_on;
	case	45:
		//	�W�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	46:
		dm_manual_set_move( dm_manual_translate,47,2,manual_main_time_table_4[15],cap_turn_l);	//	�ړ������w��(���Q��)
		return	dm_manual_work_on;
	case	47:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[16],cap_turn_r);	//	��]�����w��(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 50;					//	���̏�����
		return	dm_manual_work_on;
	case	50:
		//	�X�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,51,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	51:
		dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_4[17],cap_turn_l);	//	��]�����w��(���P��)
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_4[18],cap_turn_r);	//	�ړ������w��(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 53;					//	���̏�����
		return	dm_manual_work_on;
	case	53:
		//	10�ڂ̃J�v�Z���I��
		dm_set_manual_str(manual_txt_4[3]);							//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
		dm_manual_set_move( dm_manual_wait,54,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	54:
		dm_manual_mode_stop_flg = cap_flg_off;						//	���������~
		manual_str_win_flg = cap_flg_on;							//	�����\���t���O���n�m�ɂ���
		dm_manual_mode[dm_manual_now] = 55;							//	���̏�����
		return	dm_manual_work_on;
	case	55:
		//	�����I��
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;						//	��������J�n
			manual_str_win_flg = cap_flg_off;							//	�����\���t���O��off�ɂ���
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[19],cap_turn_l);	//	�ړ������w��(���R��)
			dm_manual_mode[dm_manual_next_next] = 56;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	56:
		//	11�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,57,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	57:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[20],cap_turn_r);	//	�ړ������w��(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 60;					//	���̏�����
		return	dm_manual_work_on;
	case	60:
		//	12�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,61,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	61:
		dm_manual_set_move( dm_manual_rotate,62,1,manual_main_time_table_4[21],cap_turn_r);	//	��]�����w��(�E�P��)
		return	dm_manual_work_on;
	case	62:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[22],cap_turn_l);	//	�ړ������w��(���R��)
		dm_manual_mode[dm_manual_next_next] = 63;					//	���̏�����
		return	dm_manual_work_on;
	case	63:
		//	13�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[23],cap_turn_r);	//	��]�����w��(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 65;					//	���̏�����
		return	dm_manual_work_on;
	case	65:
		//	14�ڂ̃J�v�Z���I��
		dm_manual_set_move( dm_manual_wait,66,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	66:
		//	15�ڂ̃J�v�Z���I��
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,67,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	67:
		dm_manual_mode_stop_flg = cap_flg_off;					//	���������~
		manual_str_win_flg = cap_flg_on;						//	�����\���t���O���n�m�ɂ���
		dm_set_manual_str(manual_txt_4[4]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
		dm_manual_timer = 0;									//	�^�C�}�[�̃N���A
		manual_flash_pos[0] = dm_wold_x;						//	�_�ŃG���A�J�n�w���W
		manual_flash_pos[1] = dm_wold_y + 10;					//	�_�ŃG���A�J�n�x���W
		manual_flash_alpha = 1;									//	�_�ł̃t���O
		dm_manual_mode[dm_manual_now] = 68;						//	���̏�����
		return	dm_manual_work_on;
	case	68:
		if( manual_str_flg < 0 ){
			manual_flash_alpha = 0;
			dm_manual_mode_stop_flg = cap_flg_on;						//	��������J�n
			dm_set_manual_str(manual_txt_4[5]);							//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 69;							//	���̏�����
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;
				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	69:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[24],cap_turn_r);	//	��]�����w��(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 70;					//	���̏�����
		return	dm_manual_work_on;
	case	70:
		//	16�ڂ̃J�v�Z���I��
		if( game_map_data[0][(15 << 3) + 7].capsel_m_g == capsel_b ){
					//	15�ڂ̃J�v�Z���I��
			dm_manual_set_move( dm_manual_wait,71,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	71:
		dm_manual_set_move( dm_manual_rotate,72,1,manual_main_time_table_4[25],cap_turn_l);	//	��]�����w��(���P��)
		return	dm_manual_work_on;
	case	72:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[26],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 73;					//	���̏�����
		return	dm_manual_work_on;
	case	73:
		if( !game_map_data[0][(15 << 3) + 4].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;							//	�����\���t���O���n�e�e�ɂ���
			dm_manual_set_move( dm_manual_wait,74,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	74:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[27],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 75;					//	���̏�����
		return	dm_manual_work_on;
	case	75:
		if( !game_map_data[0][(11 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,76,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	76:
		dm_manual_set_move( dm_manual_rotate,77,1,manual_main_time_table_4[28],cap_turn_l);	//	��]�����w��(���P��)
		return	dm_manual_work_on;
	case	77:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[29],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 80;					//	���̏�����
		return	dm_manual_work_on;
	case	80:
		if( !game_map_data[0][(13 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			dm_set_manual_str(manual_txt_4[6]);							//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_str_win_flg = cap_flg_on;							//	�����\���t���O���n�m�ɂ���
			dm_manual_set_move( dm_manual_wait,81,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	81:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[30],cap_turn_r);	//	��]�����w��(�E�R��)
		dm_manual_mode[dm_manual_next_next] = 82;					//	���̏�����
		return	dm_manual_work_on;
	case	82:
		dm_manual_set_move( dm_manual_wait,83,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	83:
		dm_manual_set_move( dm_manual_rotate,84,1,manual_main_time_table_4[31],cap_turn_l);	//	��]�����w��(���P��)
		return	dm_manual_work_on;
	case	84:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_4[32],cap_turn_r);	//	��]�����w��(�E�S��)
		dm_manual_mode[dm_manual_next_next] = 85;					//	���̏�����
		return	dm_manual_work_on;
	case	85:
		dm_manual_set_move( dm_manual_wait,90,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	90:
		dm_manual_set_move( dm_manual_translate,91,1,manual_main_time_table_4[33],cap_turn_r);	//	�ړ������w��(�E�P��)
		return	dm_manual_work_on;
	case	91:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[34],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 92;					//	���̏�����
		return	dm_manual_work_on;
	case	92:
		dm_manual_set_move( dm_manual_wait,93,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	93:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[35],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 94;					//	���̏�����
		return	dm_manual_work_on;
	case	94:
		//	���ő҂�
		if( game_map_data[0][(15 << 3) + 3].capsel_m_g == capsel_b ){
			manual_str_stop_flg = cap_flg_off;								//	������i�s�̒�~
			manual_str_win_flg = cap_flg_off;								//	�����\���t���O���n�e�e�ɂ���
			dm_manual_set_move( dm_manual_wait,95,1,30,cap_turn_r);			//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	95:
		dm_manual_set_move( dm_manual_translate,96,3,manual_main_time_table_4[36],cap_turn_r);	//	�ړ������w��(�E�R��)
		return	dm_manual_work_on;
	case	96:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[37],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 100;					//	���̏�����
		return	dm_manual_work_on;
	case	100:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,101,1,30,cap_turn_r);			//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	101:
		dm_manual_set_move( dm_manual_translate,102,1,manual_main_time_table_4[38],cap_turn_l);	//	�ړ������w��(���P��)
		return	dm_manual_work_on;
	case	102:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[39],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 103;					//	���̏�����
		return	dm_manual_work_on;
	case	103:
		dm_manual_set_move( dm_manual_wait,104,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	104:
		dm_manual_set_move( dm_manual_translate,105,1,manual_main_time_table_4[40],cap_turn_l);	//	�ړ������w��(���P��)
		return	dm_manual_work_on;
	case	105:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[41],cap_turn_l);	//	��]�����w��(���P��)
		dm_manual_mode[dm_manual_next_next] = 110;					//	���̏�����
		return	dm_manual_work_on;
	case	110:
		if( ret == dm_ret_clear ){
			dm_set_manual_str(manual_txt_4[7]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_str_stop_flg = cap_flg_on;				//	������i�s�̊J�n
			manual_str_win_flg = cap_flg_on;				//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;				//	������i�s�J�n
			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 111;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	111:
		if( ret == dm_ret_next_stage ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 112;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	112:
		if(dm_manual_timer > manual_main_time_table_4[42] && manual_str_flg < 0 ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_set_manual_str(manual_txt_4[8]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 120;
		}
		return	dm_manual_work_on;
	case	120:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	�L�[�_�Œ�~
			//	�������Ȃ��Ȃ���
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
			manual_key_flash_count = 4;						//	�L�[�_�Ŏ��Ԃ̐ݒ�
			manual_key_flash_pos = 1;						//	�_�ł�����L�[�̐ݒ�i���j
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 2;								//	�_�ł�����L�[�̐ݒ�i�E�j
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 0;								//	�_�ł�����L�[�̐ݒ�i���j
			}

			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	�L�[�_�Ŏ��Ԃ̐ݒ�
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 4;								//	�_�ł�����L�[�̐ݒ�i�`�j
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 3;								//	�_�ł�����L�[�̐ݒ�i�a�j
			}

			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}

}

/*--------------------------------------
	����������̂T�������C���֐�
--------------------------------------*/
s8	dm_manual_5_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,1 );
		dm_manual_main_cnt( &game_state_data[1],game_map_data[1],1,1 );
	}
	dm_manual_effect_cnt( state,map,0,1 );	//	�A�����̓���\���̏���
	dm_manual_attack_capsel_down();

	//	�^�C�}�[����
	dm_manual_timer ++;

	for(i = 0;i < 2;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	�E�C���X�A�j���[�V����
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 102;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 30;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 128;							//	�L�m�s�I�̍��W


		manual_str_win_flg = cap_flg_off;				//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O��OFF�ɂ���

		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 20;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_5_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	�}�K�W���c�Ƃ�1�ɂ���
		dm_set_capsel( state );							//	�J�v�Z���ݒ�

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u

		//	�PP��
		if( state -> virus_number < 27 ){
			set_virus( map,manual_main_5_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_5_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_5_virus_table_a[state -> virus_number][0],	//	�E�C���X�̐F
									virus_anime_table[manual_main_5_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	�A�j���ԍ�
			state -> virus_number ++;
		}

		//	2P��
		set_virus( game_map_data[1],manual_main_5_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
						manual_main_5_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
							manual_main_5_virus_table_b[state -> virus_number][0],	//	�E�C���X�̐F
								virus_anime_table[manual_main_5_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	�A�j���ԍ�

		game_state_data[1].virus_number ++;

		if( game_state_data[1].virus_number > 27 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_5[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������J�n
			dm_set_manual_str( manual_txt_5[1] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_set_move( dm_manual_wait,11,1,30,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		}
		return	dm_manual_work_on;
	case	11:
		dm_manual_set_move( dm_manual_translate,12,1,manual_main_time_table_5[0],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	12:
		dm_manual_set_move( dm_manual_rotate,13,1,manual_main_time_table_5[1],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	13:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��( �ꎞ�I )
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,14,1,manual_main_time_table_5[2],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	14:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,15,1,30,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		}
		return	dm_manual_work_on;
	case	15:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[3],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 20;			//	���̏�����
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_wait,21,1,30,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[4],cap_turn_l);	//��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 22;			//	���̏�����
		return	dm_manual_work_on;
	case	22:
		//	���ő҂�
		if( !game_map_data[0][(8 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],4,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			dm_set_manual_str( manual_txt_5[2] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_flip_flg = 1;								//	�L�m�s�I�̌����i�E�����j
			dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);	//	��i�ڗ����܂ł̃E�F�C�g
		}
		return	dm_manual_work_on;
	case	23:
		dm_manual_mode_stop_flg = cap_flg_off;						//	��������J�n
		if( manual_str_flg < 0 ){
			manual_pino_flip_flg = 0;								//	�L�m�s�I�̌����i�������j
			dm_set_manual_str( manual_txt_5[3] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������J�n
			dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_5[5],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[6],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 25;					//	���̏�����
		return	dm_manual_work_on;
	case	25:
		dm_manual_set_move( dm_manual_wait,30,1,30,cap_turn_r);		//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;dm_manual_wait;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,1,manual_main_time_table_5[7],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,32,4,manual_main_time_table_5[8],cap_turn_r);	//	�ړ������̐ݒ�(�E�S��)
		return	dm_manual_work_on;
	case	32:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��( �ꎞ�I )
		if( state -> now_cap.pos_y[0] == 11 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_rotate,33,1,manual_main_time_table_5[9],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_5[10],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 34;					//	���̏�����
		return	dm_manual_work_on;
	case	34:
		dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;
	case	35:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[11],cap_turn_l);	//	�ړ������̐ݒ�(���R��)
		dm_manual_mode[dm_manual_next_next] = 36;					//	���̏�����
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_wait,37,1,30,cap_turn_r);		//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;
	case	37:
		dm_manual_set_move( dm_manual_translate,38,1,manual_main_time_table_5[12],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	38:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[13],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 40;					//	���̏�����
		return	dm_manual_work_on;
	case	40:
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);		//	��i�ڗ����܂ł̃E�F�C�g
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_5[14],cap_turn_l);	//	�ړ������̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 42;			//	���̏�����
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_wait,43,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move( dm_manual_translate,44,3,manual_main_time_table_5[15],cap_turn_l);	//	�ړ������̐ݒ�(���R��)
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[16],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 45;			//	���̏�����
		return	dm_manual_work_on;
	case	45:
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	46:
		dm_manual_set_move( dm_manual_translate,50,1,manual_main_time_table_5[21],cap_turn_l);			//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	50:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[22],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 51;			//	���̏�����
		return	dm_manual_work_on;
	case	51:
		if( !game_map_data[0][(12 << 3)].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],3,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;

			dm_set_manual_str_link(manual_txt_5[4]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_flip_flg = 1;									//	�L�m�s�I�̌����i�������j
			dm_manual_set_move( dm_manual_wait,52,1,30,cap_turn_r);												//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_rotate,53,1,manual_main_time_table_5[17],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	53:
		dm_manual_set_move( dm_manual_translate,54,4,manual_main_time_table_5[18],cap_turn_r);	//	�ړ������̐ݒ�(�E�S��)
		return	dm_manual_work_on;
	case	54:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��( �ꎞ�I )
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_rotate,55,1,manual_main_time_table_5[19],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	55:
		dm_manual_set_move( dm_manual_translate,56,1,manual_main_time_table_5[20],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	56:
		//	���ő҂�
		if( game_map_data[0][(9 << 3) + 4].capsel_m_g == capsel_b ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],3,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;									//	�L�m�s�I�̌����i�������j
			dm_manual_set_move( dm_manual_wait,57,1,30,cap_turn_r);		//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	57:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_5[21],cap_turn_r);	//	�ړ������̐ݒ�(�E�Q��)
		dm_manual_mode[dm_manual_next_next] = 60;			//	���̏�����
		return	dm_manual_work_on;
	case	60:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 0;										//	�L�m�s�I�̌����ύX�i�������j
			manual_str_win_flg = cap_flg_off;								//	�����\���t���O���n�e�e�ɂ���
		}
		//	���ő҂�
		if( game_map_data[0][(13 << 3) + 3].capsel_m_g == capsel_b ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],0,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;

			manual_pino_flip_flg = 1;									//	�L�m�s�I�̌����i�������j
			dm_manual_mode[dm_manual_now] = 61;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	61:
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	���������~
			manual_str_win_flg = cap_flg_on;						//	�����\���t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_5[5] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 62;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	62:
		//	�J�v�Z���_�ŏ���
		if( manual_str_flg < 0 ){
			dm_set_manual_str(manual_txt_5[6]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������J�n
			manual_pino_flip_flg = 0;								//	�L�m�s�I�̌����i�������j
			dm_manual_mode[dm_manual_now]		= dm_manual_down;	//	��������
			dm_manual_mode[dm_manual_next_next] = 63;				//	���̏�����
			for(i = 0;i < 12;i++){
				j = ((manual_main_5_virus_table_c[i][2] - 1) << 3) + manual_main_5_virus_table_c[i][1];
				game_map_data[1][j].capsel_m_p = manual_main_5_virus_table_c[i][0];	//	���̐F�ɖ߂�
			}
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				for( i = 0;i < 12;i++ ){
					j = ((manual_main_5_virus_table_c[i][2] - 1) << 3) + manual_main_5_virus_table_c[i][1];
					if( game_map_data[1][j].capsel_m_p < 3 ){	//	���邩������Â�����
						game_map_data[1][j].capsel_m_p += 3;
					}else{									//	�Â������疾�邭����
						game_map_data[1][j].capsel_m_p -= 3;
					}
				}
			}
		}
		return	dm_manual_work_on;
	case	63:
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);		//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_translate,65,1,manual_main_time_table_5[25],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	65:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[26],cap_turn_l);		//	��]�����̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 66;			//	���̏�����
		return	dm_manual_work_on;
	case	66:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3)+3].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],4,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;										//	�L�m�s�I�̌����i�������j
			dm_manual_mode[dm_manual_now]		= 67;	//	���̏�����
		}
		return	dm_manual_work_on;
	case	67:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	���������~
			manual_str_win_flg = cap_flg_off;						//	�����\���t���O���n�e�e�ɂ���
			game_state_data[1].virus_number = 0;					//	�E�C���X�����O�ɂ���
			clear_map_all( game_map_data[1] );						//	2P�̕r�̒����N���A
			dm_manual_mode[dm_manual_now] = 70;						//	���̏�����
		}
		return	dm_manual_work_on;
	case	70:
		//	�E�C���X�Ĕz�u
		set_virus( game_map_data[1],manual_main_5_virus_table_d[ game_state_data[1].virus_number ][1],		//	�E�C���X�w���W
					manual_main_5_virus_table_d[ game_state_data[1].virus_number ][2],		//	�E�C���X�x���W
						manual_main_5_virus_table_d[ game_state_data[1].virus_number ][0],	//	�E�C���X�̐F
							virus_anime_table[manual_main_5_virus_table_d[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	�A�j���ԍ�

		game_state_data[1].virus_number ++;
		if( game_state_data[1].virus_number > 80 ){
			manual_flash_pos[0] = dm_wold_x_vs_1p + 184;			//	�_�ŃG���A�J�n�w���W
			manual_flash_pos[1] = dm_wold_y + 40;					//	�_�ŃG���A�J�n�x���W
			manual_flash_alpha = cap_flg_on;						//	�_�ł̃t���O

			manual_str_win_flg = cap_flg_on;						//	�����\���t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_5[7] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 71;						//	���̏�����
			dm_manual_timer = 0;									//	�^�C�}�[�N���A
		}
		return	dm_manual_work_on;
	case	71:
		//	�_�ŏ���
		if( manual_str_flg < 0 ){
			manual_flash_alpha = cap_flg_off;						//	�_�ŏI��
			dm_set_manual_str(manual_txt_5[8]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������J�n
			dm_manual_mode[dm_manual_next_next] = 72;				//	���̏�����
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[27],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	72:
		dm_manual_set_move( dm_manual_wait,73,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	73:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[28],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
		dm_manual_mode[dm_manual_next_next] = 74;				//	���̏�����
		return	dm_manual_work_on;
	case	74:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;								//	�L�m�s�I�̌����i�������j
			dm_manual_mode[dm_manual_now] = 75;						//	���̏�����
		}
		return	dm_manual_work_on;
	case	75:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	���������~
			game_state_data[1].virus_number = 0;					//	�E�C���X�����O�ɂ���
			clear_map_all( game_map_data[1] );						//	�}�b�v��񏉊���
			dm_manual_mode[dm_manual_now] = 80;						//	���̏�����
		}
		return	dm_manual_work_on;
	case	80:
		set_virus( game_map_data[1],manual_main_3_virus_table_b[ game_state_data[1].virus_number ][1],		//	�E�C���X�w���W
					manual_main_3_virus_table_b[ game_state_data[1].virus_number ][2],		//	�E�C���X�x���W
						manual_main_3_virus_table_b[ game_state_data[1].virus_number ][0],	//	�E�C���X�̐F
							virus_anime_table[manual_main_3_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	�A�j���ԍ�

		game_state_data[1].virus_number ++;
		if( game_state_data[1].virus_number > 3 ){
			manual_flash_pos[0] = dm_wold_x_vs_1p + 184;			//	�_�ŃG���A�J�n�w���W
			manual_flash_pos[1] = dm_wold_y + 120;					//	�_�ŃG���A�J�n�x���W
			manual_flash_alpha = 1;									//	�_�ł̃t���O
			dm_manual_timer = 0;									//	�^�C�}�[�N���A
			dm_set_manual_str( manual_txt_5[9] );					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 81;						//	���̏�����
		}
		return	dm_manual_work_on;
	case	81:
		//	�_�ŏ���
		if( manual_str_flg < 0 ){
			manual_flash_alpha = cap_flg_off;						//	�_�ŏI��
			dm_set_manual_str(manual_txt_5[10]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;					//	��������J�n
			dm_manual_mode[dm_manual_next_next] = 82;				//	���̏�����
			dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[29],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	82:
		dm_manual_set_move( dm_manual_wait,83,1,30,cap_turn_r);												//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	83:
		dm_manual_set_move( dm_manual_rotate,84,1,manual_main_time_table_5[30],cap_turn_r);		//	��]�����̐ݒ�(�E�P��)
		return	dm_manual_work_on;
	case	84:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_5[31],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 85;			//	���̏�����
		return	dm_manual_work_on;
	case	85:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3)].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;					//	�L�m�s�I�̌����i�E�����j
			dm_manual_mode[dm_manual_now]		= 86;	//	���̏�����
		}
		return	dm_manual_work_on;
	case	86:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg =	0;										//	�L�m�s�I�̌����i�������j
			manual_str_win_flg = cap_flg_off;								//	�����\���t���O���n�e�e�ɂ���
		}

		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 90;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	90:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	�^�C�}�[�N���A
			dm_manual_mode[dm_manual_now] = 91;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	91:
		if( dm_manual_timer > manual_main_time_table_5[32]){
			manual_str_win_flg = cap_flg_on;			//	���b�Z�[�W�E�C���h�E�̕`��n�m
			dm_set_manual_str(manual_txt_5[11]);			//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 92;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	92:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	�������Ȃ��Ȃ���
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}

}
/*--------------------------------------
	����������̂U�������C���֐�
--------------------------------------*/
s8	dm_manual_6_main(void)
{
	s16	i,j;
	s8	ret[4];
	s8	map_table[] = {0,2,3};
	game_state	*state;
	game_map	*map;
	s8	flash_cap_pos_table_blue[] = {59,67,83,91};
	s8	flash_cap_pos_table_red[] = {62,86,94,102};
	s8	flash_cap_pos_table_yellow[] = {58,82,90};


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		for( i = 0;i < 4;i++ ){
			ret[i] =  dm_manual_main_cnt( &game_state_data[i],game_map_data[i],i,2 );
		}
	}
	dm_manual_effect_cnt( state,map,0,2 );	//	�A�����̓���\���̏���
	dm_manual_effect_cnt( &game_state_data[1],game_map_data[1],1,2 );	//	�A�����̓���\���̏���
	dm_manual_attack_capsel_down();


	//	�^�C�}�[����
	dm_manual_timer ++;

	for(i = 0;i < 4;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	�E�C���X�A�j���[�V����
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	������
		manual_str_win_pos[0] = 175;					//	���b�Z�[�W�E�C���h�E�̏����w���W
		manual_str_win_pos[1] = 15;						//	���b�Z�[�W�E�C���h�E�̏����x���W
		manual_pino_flip_flg = 0;						//	�L�m�s�I�̌����i�������j
		manual_pino_pos	= 160;							//	�L�m�s�I�̍��W


		manual_str_win_flg = cap_flg_off;				//	�����\���t���O��OFF�ɂ���
		manual_str_stop_flg = cap_flg_off;				//	������i�s�t���O��OFF�ɂ���

		dm_manual_6_ef_flg = 0;							//	�_�ŏ�������
		dm_manual_flash_count = 0;						//	���삷��r�̔ԍ��̐ݒ�
		dm_manual_mode[dm_manual_now] = 1;				//	���̏�����

		for( i = 0;i < 15;i ++ ){						//	�����J�v�Z���쐬
			CapsMagazine[i + 1] = manual_6_CapsMagazine[i];
		}
		for( i = 0;i < 4;i++ ){
			game_state_data[i].cap_magazine_cnt = 1;	//	�}�K�W���c�Ƃ�1�ɂ���
			dm_set_capsel( &game_state_data[i] );		//	�J�v�Z���ݒ�
		}

		//	�O���t�B�b�N�̐ݒ�
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	�E�C���X�z�u
		//	�PP�E�C���X
		if( state -> virus_number < 20 ){
			set_virus( game_map_data[0],
						manual_main_6_virus_table_a[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_6_virus_table_a[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_6_virus_table_a[state -> virus_number][0],		//	�E�C���X�̐F
									virus_anime_table[manual_main_6_virus_table_a[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	�A�j���ԍ�
		}

		//	2P�E�C���X
		if( state -> virus_number < 83 ){
			set_virus( game_map_data[1],
						manual_main_6_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_6_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_6_virus_table_b[state -> virus_number][0],		//	�E�C���X�̐F
									virus_anime_table[manual_main_6_virus_table_b[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	�A�j���ԍ�
		}

		//	3P�E�C���X
		if( state -> virus_number < 44 ){
			set_virus( game_map_data[2],
						manual_main_6_virus_table_c[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_6_virus_table_c[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_6_virus_table_c[state -> virus_number][0],		//	�E�C���X�̐F
									virus_anime_table[manual_main_6_virus_table_c[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	�A�j���ԍ�
		}

		//	4P�E�C���X
		if( state -> virus_number < 4 ){
			set_virus( game_map_data[3],
						manual_main_3_virus_table_b[state -> virus_number][1],		//	�E�C���X�w���W
							manual_main_3_virus_table_b[state -> virus_number][2],		//	�E�C���X�x���W
								manual_main_3_virus_table_b[state -> virus_number][0],		//	�E�C���X�̐F
									virus_anime_table[manual_main_3_virus_table_b[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	�A�j���ԍ�
		}


		state -> virus_number ++;

		if( state -> virus_number > 82 ){
			manual_str_win_flg = cap_flg_on;			//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;			//	������i�s�t���O���n�m�ɂ���
			dm_set_manual_str( manual_txt_6[0] );		//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 10;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	10:	//	�����P�\��
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_6[1] );			//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode[dm_manual_now] = 11;				//	���̏�����
		}
		return	dm_manual_work_on;
	case	11:	//	�����Q�\��
		if( manual_str_flg < 0 ){
			dm_manual_mode[dm_manual_now] = 12;					//	���̏�����
			dm_manual_mode_stop_flg = cap_flg_on;				//	��������i�s�J�n
			for( i = 0;i < 2;i++ ){
				game_state_data[i].game_condition[dm_mode_now] = dm_cnd_manual;	//	�J�v�Z�������J�n
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			}
		}
		return	dm_manual_work_on;
	case	12:
		//	�Q�o�̃J�v�Z��������������
		if( !game_state_data[1].now_cap.capsel_flg[cap_down_flg] ){
			dm_set_manual_str( manual_txt_6[2] );				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_off;				//	��������i�s�J�n
			dm_manual_waku_flash_flg[1] = cap_flg_on;			//	�_�Ńt���O�𗧂Ă�
			dm_manual_mode[dm_manual_now] = 13;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	13:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;				//	�A�j���[�V�����i�s�ĊJ
			dm_set_manual_str(manual_txt_6[3]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_waku_flash_flg[1] = cap_flg_off;			//	�_�Ńt���O������
			dm_manual_mode[dm_manual_now] = 14;					//	���̏�����
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_init;	//	2P�̃J�v�Z��������~
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_init;		//	2P�̃J�v�Z��������~
		}
		return	dm_manual_work_on;
	case	14:
		if( state -> now_cap.pos_y[0] == 8 ){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_6[0],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
			dm_manual_mode[dm_manual_next_next] = 15;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	15:
		dm_manual_set_move( dm_manual_wait,16,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	16:
		dm_manual_set_move( dm_manual_rotate,17,2,manual_main_time_table_6[1],cap_turn_r);	//	��]�����̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	17:
		if( state -> now_cap.pos_y[0] == 8 ){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_6[2],cap_turn_r);	//	�ړ������̐ݒ�(�E�R��)
			dm_manual_mode[dm_manual_next_next] = 20;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	20:
		//	���ő҂�
		if( !game_map_data[0][(12 << 3) + 6].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],3,capsel_yellow );	//	������Q�Ԗڂ̕r�ɍU���J�v�Z���ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );	//	������Q�Ԗڂ̕r�ɍU���J�v�Z���ݒ�

			manual_bubble_col_flg[2] = 1;	//	�F�o�u���ł��グ

			dm_manual_mode[dm_manual_now] = 21;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	21:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] )
		{
			dm_manual_mode_stop_flg = cap_flg_off;				//	�A�j���[�V�����i�s��~
			dm_set_manual_str(manual_txt_6[4]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_waku_flash_flg[3] = cap_flg_on;			//	�_�Ńt���O�𗧂Ă�
			manual_pino_flip_flg = 1;							//	�L�m�s�I�̌����i�E�����j
			dm_manual_mode[dm_manual_now] = 22;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	22:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;				//	�A�j���[�V�����i�s�J�n
			dm_set_manual_str(manual_txt_6[5]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_flip_flg = 0;							//	�L�m�s�I�̌����i�������j
			dm_manual_set_move( dm_manual_rotate,23,1,manual_main_time_table_6[3],cap_turn_r);	//	��]�����̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	23:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 8 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_6[4],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,25,1,manual_main_time_table_6[5],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	25:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_6[6],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 26;			//	���̏�����
		return	dm_manual_work_on;
	case	26:
		dm_manual_set_move( dm_manual_wait,27,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;dm_manual_wait;
	case	27:
		dm_manual_set_move( dm_manual_translate,28,3,manual_main_time_table_6[7],cap_turn_l);	//	�ړ������̐ݒ�(���R��)
		return	dm_manual_work_on;
	case	28:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[8],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 30;			//	���̏�����
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_wait,31,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_rotate,32,1,manual_main_time_table_6[9],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	32:
		dm_manual_set_move( dm_manual_translate,33,1,manual_main_time_table_6[10],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	33:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 11 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_translate,34,1,manual_main_time_table_6[11],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	34:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3)].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_red );	//	������S�Ԗڂ̕r�ɍU���J�v�Z���ݒ�
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_yellow );	//	������S�Ԗڂ̕r�ɍU���J�v�Z���ݒ�
			dm_manual_at_cap_count[3] = 0;	//	��ԉE�U���p�U���J�v�Z���J�E���^�[�̏�����
			manual_bubble_col_flg[0] = 1;	//	�ԐF�o�u���ł��グ

			manual_str_win_flg = cap_flg_off;							//	�����\���t���O���n�e�e�ɂ���
			manual_pino_flip_flg = 1;									//	�L�m�s�I�̌����i�E�����j
			dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	35:
		//	�U���J�v�Z�������҂�
		dm_manual_mode_stop_flg = cap_flg_off;					//	�A�j���[�V�����i�s��~
		if( !dm_manual_at_cap[3][1].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_on;					//	�A�j���[�V�����i�s�ĊJ
			dm_set_manual_str(manual_txt_6[6]);						//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_pino_flip_flg = 0;								//	�L�m�s�I�̌����i�������j
			manual_str_win_flg = cap_flg_on;						//	�����\���t���O���n�m�ɂ���
			dm_manual_waku_flash_flg[3] = cap_flg_off;				//	�_�Ńt���O������
			dm_manual_set_move( dm_manual_rotate,36,1,manual_main_time_table_6[12],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_translate,37,4,manual_main_time_table_6[13],cap_turn_r);	//	�ړ������̐ݒ�(�E�S��)
		return	dm_manual_work_on;
	case	37:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��(�ꎞ�I)
		if( state -> now_cap.pos_y[0] == 16 ){
			state -> cap_speed_vec = 1;							//	�������x��߂�
			dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[14],cap_turn_l);	//	��]�����̐ݒ�(���P��)
			dm_manual_mode[dm_manual_next_next] = 40;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	40:
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,42,2,manual_main_time_table_6[15],cap_turn_r);	//	�ړ������̐ݒ�(�E�Q��)
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[16],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 43;					//	���̏�����
		return	dm_manual_work_on;
	case	43:
		//	���ő҂�
		if( !game_map_data[0][(15 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			//	�U���J�v�Z���̐ݒ�
			for( i = 1;i < 4;i += 2 ){
				dm_manual_set_attack_capsel( &dm_manual_at_cap[i][0],3,capsel_blue );	//	������Q�E�S�Ԗڂ̕r�ɍU���J�v�Z���ݒ�
				dm_manual_set_attack_capsel( &dm_manual_at_cap[i][1],7,capsel_yellow );	//	������Q�E�S�Ԗڂ̕r�ɍU���J�v�Z���ݒ�
				dm_manual_at_cap_count[i] = 0;
			}
			manual_bubble_col_flg[2] = 1;	//	�F�o�u���ł��グ
			manual_bubble_col_flg[0] = 1;	//	�ԐF�o�u���ł��グ

			dm_manual_mode[dm_manual_now] = 44;			//	���̏�����
		}
		return	dm_manual_work_on;
	case	44:
		//	�U���J�v�Z�������҂�
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){				//	2P�ɃJ�v�Z���𗎉������Q�[���I�[�o�[�ɂ���
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_manual;		//	�J�v�Z�������J�n
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_down;			//	�J�v�Z�������J�n
			dm_set_capsel( &game_state_data[1] );								//	�J�v�Z���ݒ�
			dm_manual_mode[dm_manual_now] = 45;									//	���̏�����
		}
		return	dm_manual_work_on;
	case	45:
		if( ret[1] == dm_ret_game_over ){
			// 2P �Q�[���I�[�o�[
			game_state_data[1].virus_anime_spead = v_anime_speed_4p;		//	�A�j���[�V�������x�𑁂�����
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_tr_chaeck;	//	���^�C�A��Ԃɂ���
			game_state_data[1].game_condition[2] = dm_cnd_retire;			//	���^�C�A��Ԃɂ���
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_tr_chack;	//	���^�C�A��Ԃɂ���
			game_state_data[1].work_flg = 16;								//	���オ��p�J�E���^�̃Z�b�g
			game_state_data[1].retire_flg[dm_retire_flg] = 1;				//	���^�C�A�t���O�𗧂Ă�
			game_state_data[1].now_cap.capsel_flg[cap_disp_flg] = 0;		//	�l�N�X�g��\�����Ȃ��悤�ɂ���
			game_state_data[1].next_cap.capsel_flg[cap_disp_flg] = 0;		//	�l�N�X�g��\�����Ȃ��悤�ɂ���

			dm_manual_mode[dm_manual_now] = 46;			//	���̏�����

		}
		return	dm_manual_work_on;
	case	46:
		if( dm_manual_at_cap[3][0].pos_a_y >= 8 ){
			dm_set_manual_str(manual_txt_6[7]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_str_win_flg = cap_flg_on;					//	�����\���t���O���n�m�ɂ���
			dm_manual_mode_stop_flg = cap_flg_off;				//	�A�j���[�V�����i�s��~
			dm_manual_mode[dm_manual_now] = 50;					//	���̏�����
		}
		return	dm_manual_work_on;
	case	50:
		if( manual_str_flg < 0 ){
			dm_set_manual_str(manual_txt_6[8]);					//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			dm_manual_mode_stop_flg = cap_flg_on;				//	�A�j���[�V�����i�s�ĊJ
			dm_manual_set_move( dm_manual_translate,51,2,manual_main_time_table_6[17],cap_turn_l);	//	�ړ������̐ݒ�(���Q��)
		}
		return	dm_manual_work_on;
	case	51:
		if( state -> now_cap.pos_y[0] == 11 ){
			dm_manual_set_move( dm_manual_translate,52,1,manual_main_time_table_6[18],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		}
		return	dm_manual_work_on;
	case	52:
		//	���n�҂�
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,53,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	53:
		dm_manual_set_move( dm_manual_translate,54,1,manual_main_time_table_6[19],cap_turn_l);	//	�ړ������̐ݒ�(���P��)
		return	dm_manual_work_on;
	case	54:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[20],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 55;			//	���̏�����
		return	dm_manual_work_on;
	case	55:
		dm_manual_set_move( dm_manual_down,56,1,30,cap_turn_r);
		dm_manual_mode[dm_manual_next_next] = 56;			//	���̏�����
		return	dm_manual_work_on;
	case	56:
		if( !game_map_data[0][(13 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,60,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	60:
		state -> game_mode[dm_mode_now] = dm_mode_init;	//	�ꎞ�I�ɓ����Ȃ�����
		dm_set_manual_str(manual_txt_6[9]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
		dm_manual_mode[dm_manual_now] = 61;				//	����������̏����ݒ�
		return	dm_manual_work_on;
	case	61:
		if( manual_str_flg < 0 ){
			dm_manual_6_ef_flg  = 1;							//	���삷��J�v�Z���̐؂�ւ�
			dm_set_manual_str(manual_txt_6[10]);				//	�������̃f�[�^���o�b�t�@�ɃZ�b�g

			//	���K�p�ݒ�
			clear_map_all( game_map_data[1] );									//	2P�̃}�b�v���̃N���A
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_down;			//	���^�C�A��Ԃɂ���
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_manual;		//	���^�C�A��Ԃɂ���
			game_state_data[1].game_condition[dm_training_flg] = dm_cnd_training;		//	���K�ɐ؂肩����
			game_state_data[1].game_condition[dm_static_cnd] = dm_cnd_lose;				//	���K�ɐ؂肩����
			game_state_data[1].retire_flg[dm_game_over_flg] = 1;	//	�Q�[���I�[�o�[�t���O�𗧂Ă�( �O�̂��� )
			game_state_data[1].warning_flg = 0;						//	�x�����t���O�̃N���A
			game_state_data[1].cap_magazine_cnt = 13;				//	�}�K�W���c�Ƃ̐ݒ�(2P�ɗ����Ă�����̂ɐݒ�)
			dm_manual_mode[dm_manual_now] = 62;						//	���̏�����
			dm_set_capsel( &game_state_data[1] );					//	�J�v�Z���ݒ�

			dm_manual_set_move( dm_manual_wait,62,1,30,cap_turn_r);	//	30��Ă̑҂�
		}
		return	dm_manual_work_on;
	case	62:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[21],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 63;			//	���̏�����
		return	dm_manual_work_on;
	case	63:
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);	//	30��Ă̑҂�
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[22],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		dm_manual_mode[dm_manual_next_next] = 65;			//	���̏�����
		return	dm_manual_work_on;
	case	65:
		//	���ő҂�
		if( !game_map_data[1][(15 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			dm_manual_6_ef_flg  = 0;										//	���삷��J�v�Z���̐؂�ւ�
			state -> game_mode[dm_mode_now] = dm_mode_down;					//	1P:�����ĊJ
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_init;		//	2P:�����Ȃ�����
			manual_str_win_flg = cap_flg_off;								//	�����\���t���O���n�e�e�ɂ���
			manual_str_stop_flg = cap_flg_off;								//	������i�s�̒�~
			dm_manual_set_move( dm_manual_rotate,70,1,manual_main_time_table_6[23],cap_turn_l);	//	��]�����̐ݒ�(���P��)
		}
		return	dm_manual_work_on;
	case	70:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_6[24],cap_turn_r);	//	�ړ������̐ݒ�(�E�P��)
		dm_manual_mode[dm_manual_next_next] = 71;			//	���̏�����
		return	dm_manual_work_on;
	case	71:
		//	���ő҂�
		if( ret[0] == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			for( i = 1;i < 4;i++ ){
				game_state_data[i].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE
			}

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	���y�Đ�
			manual_bgm_flg = 1;								//	�Đ��t���O���n�m�ɂ���
			manual_bgm_timer = 0;							//	�Đ����ԃJ�E���^���N���A
			manual_bgm_timer_max = manual_bgm_time_b;		//	�Đ����Ԃ̐ݒ�
			dm_manual_mode[dm_manual_now] = 72;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	72:
		if( ret[0] == dm_ret_end ){
			dm_set_manual_str(manual_txt_6[11]);			//	�������̃f�[�^���o�b�t�@�ɃZ�b�g
			manual_str_win_flg = cap_flg_on;				//	�����\���t���O���n�m�ɂ���
			manual_str_stop_flg = cap_flg_on;				//	������i�s�̊J�n
			dm_manual_mode[dm_manual_now] = 73;				//	����������̏����ݒ�
		}
		return	dm_manual_work_on;
	case	73:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	�E�F�C�g����
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	��������
		if( !game_state_data[dm_manual_6_ef_flg].now_cap.capsel_flg[cap_down_flg] ){
			//	�������Ȃ��Ȃ���
			game_state_data[dm_manual_6_ef_flg].cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				game_state_data[dm_manual_6_ef_flg].cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	���������̂��߃J�v�Z���������x���グ��
				if( game_state_data[dm_manual_6_ef_flg].cap_speed_vec % 2 )
					game_state_data[dm_manual_6_ef_flg].cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	���E�ړ�
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,&game_state_data[dm_manual_6_ef_flg],dm_manual_move_vec,0);	//	�J�v�Z�����E�ړ�
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	��]
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&game_state_data[dm_manual_6_ef_flg].now_cap,dm_manual_move_vec);	//	�J�v�Z����]
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	�w��񐔏I��
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	���̏���
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	����������̂P�ǉ��`��֐�
--------------------------------------*/
void	dm_manual_1_graphic(void)
{
	switch( dm_manual_1_ef_flg )
	{
	case	1:	//	�E�������
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	�p���b�g���[�h
		load_TexTile_4b( dm_manual_arrow_side_bm0_0,12,16,0,0,11,15 );	//	�O���t�B�b�N�f�[�^���[�h
		draw_Tex( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 + 24,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 - 2,
						12,16,0,0 );

		break;
	case	2:	//	���������
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	�p���b�g���[�h
		load_TexTile_4b( dm_manual_arrow_side_bm0_0,12,16,0,0,11,15 );	//	�O���t�B�b�N�f�[�^���[�h
		draw_TexFlip( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 - 16,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 - 2,
						12,16,0,0,flip_on,flip_off );
		break;
	case	3:	//	���������
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	�p���b�g���[�h
		load_TexTile_4b( dm_manual_arrow_bm0_0,16,12,0,0,15,11 );	//	�O���t�B�b�N�f�[�^���[�h
		draw_Tex( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 + 2,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 + 14,
						16,12,0,0 );
		break;
	case	4:	//	���g
		load_TexPal( manual_cap_waku_a_bm0_0tlut );					//	�p���b�g���[�h
		load_TexTile_4b( manual_cap_waku_a_bm0_0,10,44,0,0,9,43 );	//	�O���t�B�b�N�f�[�^���[�h
		draw_Tex( game_state_data[0].map_x + cap_size_10 * 3,
					game_state_data[0].map_y + cap_size_10 * 11 - 2,
						10,44,0,0 );
		break;
	case	5:	//	�c�g
		load_TexPal( manual_cap_waku_a_bm0_0tlut );					//	�p���b�g���[�h
		load_TexBlock_4b( manual_cap_waku_b_bm0_0,64,10);	//	�O���t�B�b�N�f�[�^���[�h
		draw_Tex( game_state_data[0].map_x - 1,
					game_state_data[0].map_y + cap_size_10 * 16,
						64,10,0,0 );

	}
}

/*--------------------------------------
	����������̂R�ǉ��`��֐�
--------------------------------------*/
void	dm_manual_3_graphic( void )
{
	s8	i,p_no,flg;
	game_state *enemy_state;

	switch( dm_manual_3_ef_flg )	//	�g��`�悷��r�ԍ��̐ݒ�
	{
	case	1:		//	������R�Ԗ�
		p_no = 2;
		break;
	case	2:		//	������S�Ԗ�
		p_no = 3;
		break;
	case	3:		//	���[
		p_no = 0;
		break;
	default:		//	���̑��̏ꍇ�́A�ϐ��̏��������s��
		for( i = 0;i < 4;i ++ ){
			dm_manual_waku_flash_count[i] = 0;
			dm_manual_waku_alpha[i] = 0xff;
		}
	}

	if( dm_manual_3_ef_flg ){	//	0�ȊO�̏ꍇ

		if( dm_manual_waku_flash_count[p_no] > 10  ){	//	10�C���g�o�߂������H
			dm_manual_waku_flash_count[p_no] = 0;		//	�J�E���g�̃N���A
			if( dm_manual_waku_alpha[p_no] == 0xff ){	//	���邢�ꍇ
				dm_manual_waku_alpha[p_no] = 128;		//	�Â�����
			}else{										//	�Â��ꍇ
				dm_manual_waku_alpha[p_no] = 0xff;		//	���邭����
			}
		}

		enemy_state = &game_state_data[p_no];			//	�g�\�����s���r�̏��̎擾
		//	�����x�̐ݒ�
		gDPSetPrimColor(gp++,0,0,dm_manual_waku_alpha[p_no],dm_manual_waku_alpha[p_no],dm_manual_waku_alpha[p_no],255);

		load_TexPal( manual_bin_waku_a_bm0_0tlut );	//	�p���b�g���[�h
		//	�f�[�^�̓Ǎ���
		load_TexTile_4b( manual_bin_waku_a_bm0_0,70,6,0,0,69,5 );

		draw_Tex( enemy_state -> map_x - 3,57 ,70,3,0,0);	//	��
		draw_Tex( enemy_state -> map_x - 3,188,70,3,0,3);	//	��

		//	�f�[�^�̓Ǎ���
		load_TexTile_4b( manual_bin_waku_b_bm0_0,6,128,0,0,5,127 );

		draw_Tex( enemy_state -> map_x - 3,60 ,3,128,0,0);	//	��
		draw_Tex( enemy_state -> map_x + 64,60,3,128,3,0);	//	��

		dm_manual_waku_flash_count[p_no]  ++;				//	�_�ŃJ�E���^�̉��Z
		gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);		//	�����x(���邳)�����ɖ߂�
	}
}
/*--------------------------------------
	����������̂U�ǉ��`��֐�
--------------------------------------*/
void	dm_manual_6_graphic( game_state *state,game_a_cap *cap,s8 player_no )
{
	s8	i,flg;

	if( dm_manual_waku_flash_flg[ player_no ] ){			//	�`��t���O�������Ă����ꍇ

		if( dm_manual_waku_flash_count[player_no] > 10  ){	//	10�C���g�o�߂������H
				dm_manual_waku_flash_count[player_no] = 0;	//	�J�E���g�̃N���A
			if( dm_manual_waku_alpha[player_no] == 0xff ){	//	���邢�ꍇ
				dm_manual_waku_alpha[player_no] = 128;		//	�Â�����
			}else{											//	�Â��ꍇ
				dm_manual_waku_alpha[player_no] = 0xff;		//	���邭����
			}
		}

		//	�����x�̐ݒ�
		gDPSetPrimColor(gp++,0,0,dm_manual_waku_alpha[player_no],dm_manual_waku_alpha[player_no],dm_manual_waku_alpha[player_no],255);

		load_TexPal( manual_bin_waku_a_bm0_0tlut );	//	�p���b�g���[�h
		//	�f�[�^�̓Ǎ���
		load_TexTile_4b( manual_bin_waku_a_bm0_0,70,6,0,0,69,5 );

		draw_Tex( state -> map_x - 3,57 ,70,3,0,0);	//	��
		draw_Tex( state -> map_x - 3,188,70,3,0,3);	//	��

		//	�f�[�^�̓Ǎ���
		load_TexTile_4b( manual_bin_waku_b_bm0_0,6,128,0,0,5,127 );

		draw_Tex( state -> map_x - 3,60 ,3,128,0,0);	//	��
		draw_Tex( state -> map_x + 64,60,3,128,3,0);	//	��

		flg = 1;
	}
	if( flg ){	//	�g�\�������Ă���ꍇ
		dm_manual_waku_flash_count[player_no]  ++;		//	�_�ŃJ�E���^�̑���
		gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);	//	�����x(���邳)�����ɖ߂�
	}else{		//	�g�\�������Ă��Ȃ��ꍇ
		dm_manual_waku_flash_count[player_no] = 0;		//	�_�ŃJ�E���^���O�ɂ���B
		dm_manual_waku_alpha[player_no] = 0xff;			//	�����x��0xff(�s���� = ���邢)�ɐݒ�
	}
}


/*--------------------------------------
	����������̂S�ǉ��`��֐�
--------------------------------------*/
void	dm_manual_4_graphic( void )
{
	//	�_�ŕ`��
	if( manual_flash_alpha ){
		gDPSetRenderMode(  gp++,G_RM_XLU_SURF, G_RM_XLU_SURF2);
		disp_rect(255,0,0,128,manual_flash_pos[0],manual_flash_pos[1],80,30);
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );
	}

	gDPSetPrimColor(gp++,0,0,255,255,255,255);
}

/*--------------------------------------
	��������p�U���J�v�Z���`��֐�
--------------------------------------*/
void	dm_manual_at_cap_draw( game_state *state,game_a_cap *cap,s8 size_flg )
{
	s8	i,j;
	u8	cap_size[] = {220,128};

	for( i = 0;i < 3;i++ ){	//	�F�̕��������[�v���܂킷
		load_TexPal( cap_pal[size_flg][i] );	//	�p���b�g���[�h
		for( j = 0;j < 4;j++ ){
			if( (cap + j) -> capsel_a_flg[cap_disp_flg] ){	//	�\���t���O�������Ă���
				if( (cap + j) -> capsel_a_p == i ){				//	�����F�̏ꍇ
						draw_Tex( state -> map_x + ( cap + j ) -> pos_a_x *  state -> map_item_size,
									state -> map_y + ( cap + j ) -> pos_a_y * state -> map_item_size,
										state -> map_item_size,state -> map_item_size,0,capsel_b * state -> map_item_size );	//	�{�[���`��
				}
			}
		}
	}
}

/*--------------------------------------
	��������U���J�v�Z�����`��֐�
--------------------------------------*/
void	dm_manual_atarrow_draw( game_state *state,game_a_cap *cap,s8 flg )
{
	s8	j;
	s8	sub[] = {3,4};

	//	���p���b�g���[�h
	load_TexPal(dm_manual_arrow_bm0_0tlut);

	//	���O���t�B�b�N�f�[�^�̓Ǎ���
	load_TexBlock_4b(&dm_manual_arrow_bm0_0[0],16,12);

	for( j = 0;j < 4;j++ ){
		if( (cap + j) -> capsel_a_flg[cap_disp_flg] ){	//	�\���t���O�������Ă����ꍇ�B
			draw_Tex( state -> map_x + ( cap + j ) -> pos_a_x *  state -> map_item_size - sub[flg],
						state -> map_y + ( cap + j ) -> pos_a_y * state -> map_item_size - 13,
								16,12,0,0 );
		}
	}
}
/*--------------------------------------
	�L�m�s�I�`��֐�
--------------------------------------*/
void	disp_kinopio(s16 x_pos,s16 y_pos,s8	flip)
{
	//	�L�m�s�I�`��
	s8	i;

	gDPLoadTLUT_pal256(gp++,dm_mode_select_pino_bm0_0tlut);
	if( flip ){	//	���]�t���O�������Ă����ꍇ
		for(i = 0;i < 2;i++){
			load_TexBlock_4b(&dm_mode_select_pino_bm0_0[i * 1344],64,42);
			gSPScisTextureRectangle(gp++,x_pos << 2,y_pos + i * 42 << 2,x_pos + 64 << 2,y_pos + 42 + i * 42 << 2,G_TX_RENDERTILE,63 << 5,0,((-1)<<10)&0xffff, 1 << 10);
		}
	}else{		//	���]�t���O�������Ă��Ȃ��ꍇ
		for(i = 0;i < 2;i++){
			load_TexBlock_4b(&dm_mode_select_pino_bm0_0[i * 1344],64,42);
			draw_Tex(x_pos,y_pos + i * 42,64,42,0,0);
		}
	}

	//	�N�`�p�N
	if(manual_pino_anime_flg){
		load_TexTile_4b(dm_mode_select_pino_mouth_bm0_0,8,5,0,0,7,4);
		if( flip ){	//	���]�t���O�������Ă����ꍇ
			gSPScisTextureRectangle(gp++,x_pos + 32 << 2,y_pos + 34 << 2,x_pos + 40 << 2,y_pos + 39 << 2,G_TX_RENDERTILE,7 << 5, 0,((-1)<<10)&0xffff, 1 << 10);
		}else{		//	���]�t���O�������Ă��Ȃ��ꍇ
			draw_Tex(x_pos + 24,y_pos + 34,8,5,0,0);
		}
	}

}
/*--------------------------------------
	�R���g���[���`��֐�
--------------------------------------*/
void	disp_cont(void)
{
	load_TexPal(dm_manual_b_flash_bm0_0tlut);
	load_TexTile_4b(&dm_manual_b_flash_bm0_0[0],14,14,0,0,13,13);

	//	�L�[�_��
	if(manual_key_flash_count > 0)
	{
		switch(manual_key_flash_pos)
		{
			//	�L�[
			case	0:	//	��
				draw_Tex(32,81,14,14,0,0);
				break;
			case	1:	//	��
				draw_Tex(40,88,14,14,0,0);
				break;
			case	2:	//	�E
				draw_Tex(48,81,14,14,0,0);
				break;
			//	�{�^���̏ꍇ
			case	3:	//	�a�{�^��	�����v���
				draw_Tex(69,75,14,14,0,0);
				break;
			case	4:	//	�`�{�^��	���v���
				draw_Tex(80,87,14,14,0,0);
				break;
		}
	}
}

/*--------------------------------------
	��������������C���֐�
--------------------------------------*/
int		dm_manual_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	u32	bg_no = 0;
	s16	i,j,k;
	s16	sec_count = 0;	//	�f���p�J�E���^�[
	s8	sec_30_flg = 0;	//	�����R�O�t���[������p
	s8	out_flg = 1;	//	���[�v�E�o�p�t���O
	s8	flg_f = 0;


	//	���b�Z�[�W�L���[�̍쐬
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	���b�Z�[�W�p�m���蓖��
	//	�N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);

	//	�O���t�B�b�N�f�[�^�̓Ǎ���(�J�v�Z����)
	auRomDataRead((u32)_spriteSegmentRomStart, gfx_freebuf[GFX_ANIME_PAGE_1], (u32)_spriteSegmentRomEnd-(u32)_spriteSegmentRomStart);

	//	�O���t�B�b�N�f�[�^�̓Ǎ���(�w�i��)
	auRomDataRead((u32)_dm_manual_dataSegmentRomStart,(void *)_dm_manual_dataSegmentStart, (u32)_dm_manual_dataSegmentRomEnd-(u32)_dm_manual_dataSegmentRomStart);

	//	�O���t�B�b�N�f�[�^�̓Ǎ���(�L�m�s�I)
	auRomDataRead((u32)_dm_mode_select_dataSegmentRomStart,(void *)_dm_mode_select_dataSegmentStart, (u32)(_dm_mode_select_dataSegmentRomEnd - _dm_mode_select_dataSegmentRomStart));

	dm_manual_debug_bg_no = evs_manual_no;
	switch(dm_manual_debug_bg_no)
	{
	case	0:
	case	3:
		evs_gamesel = GSL_1PLAY;	//	SE �̐ݒ�̂���
		bg_no = 0;
		auRomDataRead((u32)_dm_bg_manual_aSegmentRomStart, BGBuffer, (u32)_dm_bg_manual_aSegmentRomEnd - (u32)_dm_bg_manual_aSegmentRomStart);
		//	�E�C���X�A�j���f�[�^�Ǎ���
		auRomDataRead( (u32)dm_anime_rom_address[12][0],gfx_freebuf[GFX_ANIME_PAGE_2],(u32)dm_anime_rom_address[12][1] - (u32)dm_anime_rom_address[12][0] );	//	�E�C���X
		//	�E�C���X�A�j���[�V�����ݒ�
		for( i = 0;i < 3;i++ ){
			dm_manual_big_virus_anime[i].cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];
			dm_anime_char_set(&dm_manual_big_virus_anime[i],game_anime_table[i + 12] );
			dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_nomal );
		}
		break;
	case	1:
	case	4:
		evs_gamesel = GSL_2PLAY;	//	SE �̐ݒ�̂���
		bg_no = 1;
		auRomDataRead((u32) _bg_dat_exSegmentRomStart, BGBuffer, (u32)_bg_dat_exSegmentRomEnd - (u32)_bg_dat_exSegmentRomStart);

		break;
	case	2:
	case	5:
		evs_gamesel = GSL_4PLAY;	//	SE �̐ݒ�̂���
		bg_no = 2;
		auRomDataRead((u32)_dm_bg_vs4p_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_vs4p_modeSegmentRomEnd - (u32)_dm_bg_vs4p_modeSegmentRomStart);
		break;
	}

	dm_manual_all_init();	//	������


	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);


	//	���C�����[�v
	while(out_flg > 0)
	{
		#ifndef THREAD_JOY
		joyProcCore();
		#endif

		//	���g���[�X���b�Z�[�W
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		//	������i�s����
		manual_str_flg = dm_manual_str_main();
		//	PUSH ANY KEY �̗h�ꏈ��
		push_any_key_main();
		switch(dm_manual_debug_bg_no)
		{
		case	0:
			out_flg = dm_manual_1_main();
			break;
		case	1:
			out_flg = dm_manual_2_main();
			break;
		case	2:
			out_flg = dm_manual_3_main();
			break;
		case	3:
			out_flg = dm_manual_4_main();
			break;
		case	4:
			out_flg = dm_manual_5_main();
			break;
		case	5:
			out_flg = dm_manual_6_main();
			break;
		}

		dm_play_se();//	�r�d�Đ�

		//	���y�Đ���
		if( manual_bgm_flg ){
			manual_bgm_timer++;
			if( manual_bgm_timer >= manual_bgm_timer_max){
				//	�Đ����ԏI��
				manual_bgm_timer = 0;
				manual_bgm_flg = 0;
				auSeqPlayerStop(0);	//	�a�f�l��~
			}
		}

		dm_manual_debug_timer ++;
		if( dm_manual_debug_timer > 20000)
			dm_manual_debug_timer = 0;

		//	�����R�O�t���[������p
		sec_30_flg ^= 1;

#ifdef	M_DEBUG

		if( joyupd[0] & A_BUTTON ){
			flg_f = 1;
			break;
		}else	if( joyupd[0] & START_BUTTON ){
			dm_manual_all_init();
		}else	if( joyupd[0] & R_CBUTTONS ){
			evs_manual_no++;
			if(	evs_manual_no > 5){
				evs_manual_no = 0;
			}
			flg_f = 0;
			break;
		}else	if( joyupd[0] & L_CBUTTONS ){
			evs_manual_no--;
			if(	evs_manual_no < 0){
				evs_manual_no = 5;
			}
			flg_f = 0;
			break;
		}
#else
		for(i = 0;i < 4;i++){
			if( joyupd[i] & DM_ANY_KEY ){
				out_flg = 0;
				flg_f = 1;
				break;
			}
		}
		if( out_flg == 0 )
			break;
#endif


	}

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

#ifdef	M_DEBUG

	if(flg_f){
		if(main_old == MAIN_TITLE);
			return	MAIN_TITLE;	//	�^�C�g��
	}else{
		return	MAIN_MANUAL;	//	���[�h�Z���N�g��
	}
#else
	if(main_old == MAIN_TITLE){
		return	MAIN_TITLE;			//	�^�C�g��
	}else	if( main_old == MAIN_MODE_SELECT ){
		main_old = MAIN_MANUAL;
		return	MAIN_MODE_SELECT;	//	���[�h�Z���N�g
	}
#endif

}

/*--------------------------------------
	��������`�惁�C���֐�
--------------------------------------*/
void	dm_manual_graphic(void)
{
	NNScTask*	gt;
	int			i,j,k,p;

	// �ި���ڲؽ��ޯ̧�����ޯ̧�̎w��
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	// RSP �����ݒ�


	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_1]));	//	�A�C�e���i�J�v�Z�����j�̃Z�O�����g�ݒ�
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_2]));	//	2P�L�����N�^�̃Z�O�����g�ݒ�
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	// �t���[���o�b�t�@�̃N���A
	gSPDisplayList(gp++, ClearCfb );

	//	�`��͈͂̎w��
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	�������ݒ�
	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	//	�����x�̐ݒ�
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

	//	�f�o�b�N�p�^�C�}�[�\��

	//	�w�i�`��
	gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	load_TexPal( (u16 * )&BGBuffer[1] );
	for(i = 0;i < 40;i++)
	{
		load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
		draw_Tex(0,(i * 6),320,6,0,0);
	}


	//	����w�i�`��
	switch( dm_manual_debug_bg_no )
	{
	case	0:
	case	3:
		//	����E�C���X
		dm_anime_draw( &dm_manual_big_virus_anime[2],dm_manual_big_virus_pos[2][0],dm_manual_big_virus_pos[2][1],0 );				//	����E�C���X�`��
		dm_anime_draw( &dm_manual_big_virus_anime[0],dm_manual_big_virus_pos[0][0],dm_manual_big_virus_pos[0][1],0 );				//	����ԃE�C���X�`��
		dm_anime_draw( &dm_manual_big_virus_anime[1],dm_manual_big_virus_pos[1][0],dm_manual_big_virus_pos[1][1],0 );				//	���剩�E�C���X�`��
		break;
	case	2:
	case	5:
		//	�}�X�N
		for(i = 0;i < 6;i++)
		{
			load_TexTile_8b(&dm_bg_manual_4p_bm0_0[282 * 6 * i],282,6,0,0,281,5);
			draw_Tex(18,192 + i * 6,282,6,0,0);
		}
		load_TexTile_8b(&dm_bg_manual_4p_bm0_0[282 * 6 * 6],282,3,0,0,281,2);
		draw_Tex(18,228,282,3,0,0);
		break;
	}


	//	�r�̒��̏��`��
	switch( dm_manual_debug_bg_no )
	{
	case	0:	//	����������̂P
		dm_game_graphic_p( &game_state_data[0],game_map_data[0] );	//	�J�v�Z���`��
		dm_game_graphic_effect( &game_state_data[0],0,0 );			//	���o�`��
		dm_manual_1_graphic();										//	����������̂P�ǉ��`��
		disp_cont();												//	���`��
		break;
	case	3:	//	����������̂S
		dm_game_graphic_p( &game_state_data[0],game_map_data[0] );	//	�J�v�Z���`��
		dm_manual_4_graphic( );										//	�ԓ_��
		dm_game_graphic_effect( &game_state_data[0],0,0 );			//	���o�`��
		disp_cont();												//	���`��
		break;
	case	1:	//	����������̂Q
	case	4:	//	����������̂T
		for( i = 0;i < 2;i++ ){
			dm_game_graphic_p( &game_state_data[i],game_map_data[i] );				//	�J�v�Z���`��
			dm_manual_at_cap_draw( &game_state_data[i],dm_manual_at_cap[i],0);		//	�U���J�v�Z���`��
			dm_manual_atarrow_draw( &game_state_data[i],dm_manual_at_cap[i],0 );	//	�U���J�v�Z���p���`��
			dm_game_graphic_effect( &game_state_data[i],i,0);						//	���o�`��
		}
		dm_manual_4_graphic( );	//	�ԓ_��
		break;
	case	2:	//	����������̂R
	case	5:	//	����������̂U
		for( i = 0;i < 4;i++ ){
			dm_game_graphic_p( &game_state_data[i],game_map_data[i] );				//	�J�v�Z���`��
			dm_manual_at_cap_draw( &game_state_data[i],dm_manual_at_cap[i],1);		//	�U���J�v�Z���`��

			switch( dm_manual_debug_bg_no ){
			case	2:	//	����������̂R
				if( i == 0 ){
					dm_manual_3_graphic( );											//	����������̂R�ǉ��`��
				}
				break;
			case	5:	//	����������̂U
				dm_manual_6_graphic( &game_state_data[i],dm_manual_at_cap[i],i );	//	����������̂U�ǉ��`��
				break;
			}
			dm_manual_atarrow_draw( &game_state_data[i],dm_manual_at_cap[i],1 );	//	�U���J�v�Z���p���`��
		}
		for( i = 0;i < 4;i++ ){
			dm_game_graphic_effect( &game_state_data[i],i,1 );						//	���o�`��
		}


		break;
	}

	//	�L�m�s�I�`��
	disp_kinopio(manual_pino_pos,137,manual_pino_flip_flg);

	//	PUSH ANY KEY�`��
	switch( dm_manual_debug_bg_no )
	{
	case	0:
	case	3:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY
		break;
	case	1:
	case	4:
		push_any_key_draw( 128,210 );	// PUSH ANY KEY
		break;
	case	2:
	case	5:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY

		break;
	}


	if( manual_str_win_flg ){
		//	���b�Z�[�W�E�C���h�w�i�̕`��
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );

		//	�t�H���g�p���b�g�f�[�^�̓Ǎ���
		load_TexPal(dm_manual_mes_win_bm0_0tlut);

		//	���b�Z�[�W�w�i�̓Ǎ���
		for(i = 0;i < 4;i++){
			load_TexTile_4b(&dm_manual_mes_win_bm0_0[i * 1160],116,20,0,0,115,19);
			draw_Tex(manual_str_win_pos[0],manual_str_win_pos[1] + i * 20,116,20,0,0);
		}

		//	���̓f�[�^�̕`��
		load_TexPal( dm_manual_font_pal[0] );//�f�t�H���g�ŌĂ�ł���
		manual_str_end_flg = dm_draw_manual_str(manual_str_win_pos[0] + 4,manual_str_win_pos[1] + 4,manual_str_buffer,manual_str_len);
	}

	/*------------------*/
	// �ި���ڲؽč쐬�I��, ���̨������̊J�n
	gDPFullSync(gp++);			// �ި���ڲؽčŏI����
	gSPEndDisplayList(gp++);	// �ި���ڲؽďI�[
	osWritebackDCacheAll();		// dinamic�����Ă��ׯ��

	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);

}
