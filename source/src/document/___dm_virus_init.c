/*--------------------------------------
	filename	:	dm_virus_init.c

	create		:	1999/08/03
	modify		:	1999/12/09

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


u8	limit_table[] =		// ���قɂ��virus�ςݍ��ݏ��ð���, max = LEVLIMIT
	{ 6,6,6,6,6, 6,6,6,6,6, 6,6,6,6,6, 5,5,4,4,3, 3,3,3,3,3 };


/*--------------------------------------
	�}�b�v���N���A�֐�
--------------------------------------*/
void	clear_map_all( game_map *map )
{
	u8	i,j;

	for(i = 0;i < 128;i ++){
		//	�e�t���O�̏���
		(map + i) -> capsel_m_g = (map + i) -> capsel_m_p = 0;
		for(j = 0;j < 6;j++)
			(map + i) -> capsel_m_flg[j] = 0;
	}
}
/*--------------------------------------
	�}�b�v���ꕔ�N���A�֐�
--------------------------------------*/
void	clear_map( game_map *map,s8 x_pos,s8 y_pos )
{
	u8	i,j;

	i = ((y_pos - 1) << 3) + x_pos;	//	��������ꏊ�̌v�Z
	(map + i) -> capsel_m_g = (map + i) -> capsel_m_p = 0;
	//	�e�t���O�̏���
	for(j = 0;j < 6;j++){
		(map + i) -> capsel_m_flg[j] = 0;
	}
}

/*--------------------------------------
	�}�b�v���擾�֐�
--------------------------------------*/
s8	get_map_info( game_map *map,s8 x_pos,s8 y_pos )
{
	u8	i;

	i = ((y_pos - 1) << 3) + x_pos;	//	��������ꏊ�̌v�Z
	if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
		//	�\���t���O�������Ă���
		return	cap_flg_on;
	}else{
		//	�\���t���O�������Ă��Ȃ�
		return	cap_flg_off;
	}
}

/*--------------------------------------
	�E�C���X���擾�֐�
--------------------------------------*/
u8	get_virus_count( game_map *map )
{
	u8	i,ret;

	for(i = ret = 0;i < 128;i++)
	{
		if( (map + i) -> capsel_m_flg[cap_disp_flg] )
		{
			//	�\���t���O�������Ă���
			if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){
				//	���ŏ�Ԃł͂Ȃ�����
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 )
				{
					//	�E�C���X�̏ꍇ
					ret ++;
				}
			}
		}
	}
	return	ret;
}

/*--------------------------------------
	�E�C���X���擾�֐�
--------------------------------------*/
u8	get_virus_color_count( game_map *map ,u8 *red,u8 *yellow,u8 *blue )
{
	u8	i,ret;
	u8	color[3];

	for( i = 0;i < 3;i++ ){
		color[i] = 0;
	}

	for(i = ret = 0;i < 128;i++)
	{
		if( (map + i) -> capsel_m_flg[cap_disp_flg] )
		{
			//	�\���t���O�������Ă���
			if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){
				//	���ŏ�Ԃł͂Ȃ�����
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 )
				{
					//	�E�C���X�̏ꍇ
					color[(map + i) -> capsel_m_flg[cap_col_flg]]++;
					ret ++;
				}
			}
		}
	}
	//	���ꂼ��̐F�̃E�C���X���̐ݒ�
	*red	= color[0];	//	��
	*yellow = color[1];	//	��
	*blue 	= color[2];	//	��

	return	ret;
}

/*--------------------------------------
	�}�b�v���ݒ�֐�
--------------------------------------*/
void	set_map( game_map *map,s8 x_pos,s8 y_pos,s8 item,s8 pal )
{
	u8	i,pal_save;

	pal_save = pal;
	if( pal_save > 2 ){				//	�Â��F�������ꍇ
		pal_save -= 3;
	}

	i = ((y_pos - 1) << 3) + x_pos;	//	�ݒ肷��ꏊ�̌v�Z

	(map + i) -> capsel_m_g = item;							//	�ݒ蕨
	(map + i) -> capsel_m_p = pal;							//	�\���F
	(map + i) -> capsel_m_flg[cap_disp_flg] = 1;			//	�\���t���O�𗧂Ă�
	(map + i) -> capsel_m_flg[cap_down_flg] = 0;			//	�����t���O������
	(map + i) -> capsel_m_flg[cap_condition_flg] = 0;		//	��Ԃ�ʏ��
	(map + i) -> capsel_m_flg[cap_virus_flg] = -1;			//	�E�C���X�ȊO
	(map + i) -> capsel_m_flg[cap_col_flg] = pal_save;		//	�����v�Z�F
}

/*--------------------------------------
	�E�C���X�}�b�v���ݒ�֐�
--------------------------------------*/
void	set_virus( game_map *map,s8 x_pos,s8 y_pos,u8 virus,u8 virus_anime_no )
{
	u8	i,pal_save;

	pal_save = virus;
	if( pal_save > 2 ){				//	�Â��F�������ꍇ
		pal_save -= 3;
	}

	i = ((y_pos - 1)<< 3) + x_pos;

	(map + i) -> capsel_m_g = virus_anime_no;				//	�E�C���X�A�j���ԍ�
	(map + i) -> capsel_m_p = virus;						//	�F
	(map + i) -> capsel_m_flg[cap_disp_flg] = 1;			//	�\���t���O�𗧂Ă�
	(map + i) -> capsel_m_flg[cap_down_flg] = 0;			//	�����t���O������
	(map + i) -> capsel_m_flg[cap_condition_flg] = 0;		//	��Ԃ�ʏ��
	(map + i) -> capsel_m_flg[cap_virus_flg] = virus;		//	�E�C���X�ԍ����Z�b�g����
	(map + i) -> capsel_m_flg[cap_col_flg] = pal_save;		//	�����v�Z�F
}

/*--------------------------------------
	�E�C���X�}�b�v���쐬�p�⏕�֐�
--------------------------------------*/
u16	y_point_set(void)
{
	u16		rnd_weight;
	u16		rnd_cnt;
	u16		idx;

	idx = dm_up_weight + dm_middle_weight + dm_low_weight;
	rnd_weight = genrand( idx );
	if ( rnd_weight < dm_up_weight ) {
			rnd_cnt = genrand(STD_MFieldY / 3);
	} else {
		if ( rnd_weight < ( dm_up_weight + dm_middle_weight )) {
			rnd_cnt = ( genrand(STD_MFieldY/3)+ STD_MFieldY/3 );
		} else {
			rnd_cnt = ( genrand((STD_MFieldY/3+STD_MFieldY%3))+STD_MFieldY/3*2 );
		}
	}

	return ( rnd_cnt );
}
/*--------------------------------------
	�E�C���X�}�b�v���쐬�p�⏕�֐�(�E�C���X���݊m�F)
--------------------------------------*/
u8	dm_virus_check( virus_map *v_map,u16 x_pos,u16 y_pos )
{
	u8	i;

	//	���擾��̌v�Z( X,Y ���W����z��ԍ�������o�� )
	i = ((y_pos - 1) << 3) + x_pos;

	if( (v_map + i) -> virus_type < 0 ){
		return	FALSE;	//	����
	}
	return	TRUE;	// �L��
}

/*--------------------------------------
	�E�C���X�}�b�v���쐬�p�⏕�֐�(�㉺���E�A�����F�R�ȏ���є���)
--------------------------------------*/
u8	dm_check_color( virus_map *v_map,u16 x_pos,u16 y_pos,u8 check_color )
{
	u8	p,p_y[2];

	p = ((y_pos - 1) << 3) + x_pos;

	//	�����`�F�b�N
	if( x_pos > 1 ){
		if( (v_map + (p - 1)) -> virus_type == check_color ){		//	�P���ׂ������F������
			if( (v_map + (p - 2)) -> virus_type == check_color ){	//	�Q���ׂ������F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	//	���E�`�F�b�N
	if( x_pos != 0 && x_pos != 7){
		if( (v_map + (p - 1)) -> virus_type == check_color ){		//	�P���ׂ������F������
			if( (v_map + (p + 1)) -> virus_type == check_color ){	//	�P�E�ׂ������F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	//	���E�`�F�b�N
	if( x_pos < 6 ){
		if( (v_map + (p + 1)) -> virus_type == check_color ){		//	�P�E�ׂ������F������
			if( (v_map + (p + 2)) -> virus_type == check_color ){	//	�Q�E�ׂ������F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	p_y[0] = (y_pos  << 3) + x_pos;
	p_y[1] = ((y_pos + 1) << 3) + x_pos;

	if( y_pos < 15 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	�P���������F������
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	�Q���������F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	p_y[1] = ((y_pos - 2) << 3) + x_pos;

	if( y_pos > 3 && y_pos < 16 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	�P���������F������
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	�P�オ�����F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	p_y[0] = ((y_pos - 2) << 3) + x_pos;
	p_y[1] = ((y_pos - 3) << 3) + x_pos;

	if( y_pos > 5 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	�P�オ�����F������
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	�Q�オ�����F������
				return	FALSE;	//	�u���܂���
			}
		}
	}

	return	TRUE;
}

/*--------------------------------------
	�E�C���X�}�b�v���쐬�p�⏕�֐�(�㉺���E�A�����F�R�Ȕ�ѐF����)
--------------------------------------*/
u8	dm_check_color_2( virus_map *v_map,u16 x_pos,u16 y_pos,u8 check_color )
{
	u8	p;

	p = ((y_pos - 1) << 3) + x_pos;

	//	���`�F�b�N
	if( x_pos > 1 ){
		if( (v_map + (p - 2)) -> virus_type == check_color ){	//	�Q�ׂ������F������
			return	FALSE;	//	�u���܂���
		}
	}

	//	�E�`�F�b�N
	if( x_pos < 6 ){
		if( (v_map + (p + 2)) -> virus_type == check_color ){	//	�Q�ׂ������F������
			return	FALSE;	//	�u���܂���
		}
	}

	p = ((y_pos - 3) << 3) + x_pos;

	if( y_pos > 5 ){
		if( (v_map + p) -> virus_type == check_color ){	//	�Q�オ�����F������
			return	FALSE;	//	�u���܂���
		}
	}

	p = ((y_pos + 1) << 3) + x_pos;

	if( y_pos < 15 ){
		if( (v_map + p) -> virus_type == check_color ){	//	�Q���������F������
			return	FALSE;	//	�u���܂���
		}
	}

	return	TRUE;
}
/*--------------------------------------
	�E�C���X�}�b�v��񕡐�(�R�s�[)�֐�
--------------------------------------*/
void	dm_virus_map_copy( virus_map *v_map_a,virus_map *v_map_b,u8 *order_a,u8*order_b )
{
	s16	i;

	for(i = 0;i < 128;i++)
	{
		//	�R�s�[��f�[�^�̏�����
		(v_map_b + i) -> virus_type = -1;
		(v_map_b + i) -> x_pos = (v_map_b + i) -> y_pos = 0;

		//	�R�s�[���
		(v_map_b + i) -> virus_type = (v_map_a + i) -> virus_type;
		(v_map_b + i) -> x_pos = (v_map_a + i) -> x_pos;
		(v_map_b + i) -> y_pos = (v_map_a + i) -> y_pos;
	}

	//	�`�揇�̃R�s�[
	for(i = 0;i < 96;i++){
		*(order_b + i) = *(order_a + i);
	}
}

/*--------------------------------------
	�E�C���X�}�b�v���쐬�֐�
--------------------------------------*/
void	dm_virus_init(game_state *state,virus_map *v_map,u8 *order)
{
	s16	i,j;
	u16	virus_color[4];
	u16	cont_count;
	u16	dead_count;
	u16	x_set,y_set;
	s8	col_p;
	u8	color_set;
	u8	virus_work,virus_max,virus_count;
	u8	limit_line;
	u8	set_flg;

RESTART_POS:

	//	�E�C���X�}�b�v���̏�����
	for(i = 0;i < 128;i++)
	{
		(v_map + i) -> virus_type = -1;
		(v_map + i) -> x_pos = (v_map + i) -> y_pos = 0;
	}
	//	�\�������̃N���A
	for(i = 0;i < 96;i++){
		*(order + i) = 0xff;
	}

	virus_work = state -> virus_level;	//	�E�C���X���x���̃Z�b�g
	if( virus_work > LEVLIMIT ){	//	����l�̔���
		virus_work = LEVLIMIT;
	}


	//	�����E�C���X��
	virus_max = ( virus_work + 1 ) << 2;


	// ���قɂ��virus�s�ݍs���Z�o
	limit_line = limit_table[virus_work];

	//	�F�ʃE�C���X���̌v�Z
	virus_work = virus_max / 3;
	for(i = 0;i < 4;i++){
		virus_color[i] = virus_work;
	}
	for(i = 0;i < (virus_max % 3);i++){
		virus_color[i] ++;
	}


	for( virus_count = dead_count = col_p = 0;virus_count < virus_max;virus_count++ ){

		cont_count = dead_count = 0;
		do{
			//	�F�w��
			do{
				j = genrand(3);
			}while( virus_color[j] == 0 );
			color_set = j;

			if( cont_count++ == 2){
				dead_count ++;
				if( dead_count > 2 ){
					goto RESTART_POS;	//	�������[�v���
				}
				//	���W�w��( ��������� )
				cont_count = 0;
				for( x_set = 0;x_set < 8;x_set ++ ){
					for(y_set = 16;y_set > limit_line;y_set --){
						set_flg = FALSE;
						if( !dm_virus_check(v_map,x_set,y_set) ){
							set_flg = dm_check_color(v_map,x_set,y_set,color_set);
							if( set_flg == TRUE){
								set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
								if( set_flg ){
									goto OUT_POS;
								}
							}
						}
					}
				}
OUT_POS:
			}else{
				//	���W�w��
				do{
					x_set = genrand(8);
					do{
						y_set = genrand(17);
//						y_set = y_point_set();
					}while(y_set < limit_line + 1);
				}while(dm_virus_check(v_map,x_set,y_set));
				//	���̐F���u���邩
				set_flg = dm_check_color(v_map,x_set,y_set,color_set);
				if( set_flg == TRUE){
					set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
				}
			}
		}while(!set_flg);
		//	�E�C���X�ݒ�

SET_POS:

		virus_color[j]--;
		j = ((y_set - 1)<< 3) + x_set;
		(v_map + j) -> virus_type = color_set;
		(v_map + j) -> x_pos = x_set;
		(v_map + j) -> y_pos = y_set;
		*(order + virus_count) = j;
	}

}
