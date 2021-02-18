/*--------------------------------------
	filename	:	dm_game_anime_main.c

	create		:	1999/08/11
	modify		:	1999/12/02

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

/*--------------------------------------
	�A�j���[�V�����p�A�h���X�ݒ�֐�
--------------------------------------*/
u32	dm_anime_address_set( u32 address_bss,u32 address )
{
	return	(u32)(address_bss + address - (u32)GS1_SEGMENT * 0x01000000);
}

/*--------------------------------------
	�A�j���[�V�����L�����N�^�[�ݒ�֐�
--------------------------------------*/
void	dm_anime_char_set( game_anime *ani ,game_anime_data **ani_dat)
{
	//	�A�j���f�[�^�̎��A�h���X�ݒ�
	ani -> anime_charcter = (game_anime_data **)dm_anime_address_set( ani -> cnt_anime_address,(u32)ani_dat );
}

/*--------------------------------------
	�A�j���[�V���������ݒ�֐�
--------------------------------------*/
void	dm_anime_set( game_anime *ani ,u8 ani_type)
{
	ani -> cnt_anime_flg = cap_flg_off;	//	�A�j���[�V���������݋֎~
	ani	-> cnt_now_anime = ani_type;	//	�A�j���[�V�����ԍ�
	ani -> cnt_now_frame = 0;			//	�A�j���[�V�����t���[�����ŏ���
	ani -> cnt_anime_count = 0;			//	�A�j���[�V�����J�E���g�N���A
	ani -> cnt_anime_loop_count = 0;	//	�A�j���[�V�������[�v�J�E���g�̃N���A
	ani -> cnt_now_type = (game_anime_data *)dm_anime_address_set( ani -> cnt_anime_address,(u32)ani -> anime_charcter[ani_type] );	//	�A�j���[�V�����̃Z�b�g
}

/*--------------------------------------
	�A�j���[�V�����ݒ�֐�
--------------------------------------*/
void	dm_anime_set_cnt( game_anime *ani ,u8 ani_type)
{
	if( ani -> cnt_anime_flg ){	//	�A�j���[�V���������݉\
		dm_anime_set( ani,ani_type );	//	�A�j���[�V�����ݒ�
	}

}
/*--------------------------------------
	�A�j���[�V�����ĉғ��֐�(�ŏ�����A�j���[�V����������)
--------------------------------------*/
void	dm_anime_restart( game_anime *ani )
{
	ani -> cnt_now_frame = 0;			//	�A�j���[�V�����t���[�����ŏ���
	ani -> cnt_anime_count = 0;			//	�A�j���[�V�����J�E���g�N���A
	ani -> cnt_anime_loop_count = 0;	//	�A�j���[�V�������[�v�J�E���g�̃N���A
}

/*--------------------------------------
	�A�j���[�V��������֐�
--------------------------------------*/
void	dm_anime_control( game_anime *ani )
{

	ani -> cnt_anime_count ++;
	if( ani -> cnt_anime_count > ( (ani -> cnt_now_type + ani -> cnt_now_frame) -> aniem_wait << 1) ){	//	�A�j���[�V�����Ԋu�������
		ani -> cnt_anime_count = 0;	//	�J�E���g������
		switch(  (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg )
		{
		case	anime_next:	//	�P�R�}�i�߂�
			ani -> cnt_now_frame ++;
			break;
		case	anime_loop:	//	���[�v(�L��)
			ani -> cnt_anime_loop_count++;
			ani -> cnt_now_frame ++;
			if( ani -> cnt_anime_loop_count >= (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_wait){
				//	���[�v�I��
				dm_anime_set( ani,ANIME_nomal );		//	�ʏ�A�j���[�V�����̐ݒ�
				ani -> cnt_anime_flg = cap_flg_on;	//	�A�j���[�V���������݋���
			}else{
				ani -> cnt_now_frame = (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg;	//	���[�v����R�}�̃Z�b�g
			}
			break;
		case	anime_loop_infinity:	//	���[�v(����)
			ani -> cnt_now_frame ++;
			ani -> cnt_now_frame = (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg;	//	���[�v����R�}�̃Z�b�g
			ani -> cnt_anime_flg = cap_flg_on;	//	�A�j���[�V���������݋���
			break;
		case	anime_stop:	//	�������Ȃ�
			ani -> cnt_anime_flg = cap_flg_on;	//	�A�j���[�V���������݋���
			break;
		case	anime_no_write://	�������Ȃ�
			break;
		case	anime_end:	//	�ʏ��Ԃɖ߂�
			dm_anime_set( ani,ANIME_nomal );		//	�ʏ�A�j���[�V�����̐ݒ�
			ani -> cnt_anime_flg = cap_flg_on;	//	�A�j���[�V���������݋���
			break;
		}
	}
}

/*--------------------------------------
	�A�j���[�V�����`��֐�
--------------------------------------*/
void	dm_anime_draw( game_anime *ani,s16 x_pos,s16 y_pos,s8 flip_flg )
{
	s16	i,j,pos;
	game_anime_data *dat;
	u8	*address;
	u8	h_size[3];
	s8	vec = 1;

	dat = ani -> cnt_now_type + ani -> cnt_now_frame;

	if( (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg != anime_no_write){	//	�`��֎~��ԂłȂ������ꍇ
		//	2P��
		if( flip_flg )
			vec = -1;

		//	�T�C�Y�̈ڍs
		h_size[0] = 0;
		h_size[1] = dat -> h_size[0];
		h_size[2] = dat -> h_size[0] + dat -> h_size[1];

		load_TexPal( (u16 *)dm_anime_address_set( ani -> cnt_anime_address,(u32)dat -> anime_pal ) );	//	�p���b�g���[�h

		pos = x_pos - (dat -> def_w_size * flip_flg );
		pos += (dat -> pos_an_x * vec);
		address = (u8 *)dm_anime_address_set( ani -> cnt_anime_address,(u32)dat -> anime_dat );	//	�O���t�B�b�N�f�[�^�̃A�h���X
		for( i = j = 0;i < 3;i++ ){	//	�L�����N�^�[�`��
			//	�e�N�X�`�����[�h
			load_TexTile_8b( address + dat -> def_w_size * h_size[i],dat -> def_w_size,dat -> h_size[i],0,0,dat -> def_w_size - 1,dat -> h_size[i] - 1 );
			//	�`��
			draw_TexFlip(pos,y_pos + dat -> pos_an_y + j,dat -> def_w_size,dat -> h_size[i],0,0,flip_flg,flip_off );
			j += dat -> h_size[i];
		}
	}
}

