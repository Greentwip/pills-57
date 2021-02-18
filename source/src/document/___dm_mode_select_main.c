/*--------------------------------------
	filename	:	dm_mode_select_main.c

	create		:	1999/10/28
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
#include "record.h"

#include	"hardcopy.h"

#include	"dm_nab_include.h"

//#define	_IA_TEST_

static	mode_select_control		dm_ms_control;
static	mode_select_control		*ms_p	=	&dm_ms_control;
static	ms_title_pos			*ms_tp	=	&dm_ms_control.title_pos;
static	ms_mes_win_pos			*ms_mp	=	&dm_ms_control.ms_mes_win;
static	ms_cap_pos				*ms_cp	=	&dm_ms_control.cap_pos;

/*--------------------------------------
	���[�h�Z���N�g�p EEP �������݊֐�
--------------------------------------*/
void	dm_ms_eep_write(u32 err_add)
{
	EepRomErr err = EepRomErr_NoError;

	err = EepRom_WriteSystem();

	*(EepRomErr *)err_add = err;
}

/*--------------------------------------
	���[�h�Z���N�g�������֐�
--------------------------------------*/
void	init_dm_ms(void)
{
	s8	i;


	for( i = 0;i < 4;i++ ){
		joyflg[i] = 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT ;
	}

	ms_tp -> t_pos[MS_X_P]			=	32;		//	�^�C�g�� �̏����w���W�ݒ�
	ms_tp -> t_pos[MS_Y_P]			=	-40;	//	�^�C�g�� �̏����x���W�ݒ�
	ms_tp -> k_pos[MS_X_P]			=	328;	//	�L�[ 	 �̏����w���W�ݒ�
	ms_tp -> k_pos[MS_Y_P]			=	12;		//	�L�[ 	 �̏����x���W�ݒ�
	ms_tp -> p_pos[MS_X_P]			=	350;	//	�L�m�s�I �̏����w���W�ݒ�
	ms_tp -> p_pos[MS_Y_P]			=	140;	//	�L�m�s�I �̏����x���W�ݒ�
	ms_tp -> p_mouth_flg			=	0;		//	�L�m�s�I�N�`�p�N�t���O 		�̐ݒ�
	ms_tp -> p_mouth_count			=	1;		//	�L�m�s�I�N�`�p�N�J�E���^	�̐ݒ�
	ms_tp -> p_eye_flg				=	0;		//	�L�m�s�I�ڃp�`�t���O 		�̐ݒ�
	ms_tp -> p_eye_count			=	0;		//	�L�m�s�I�ڃp�`�J�E���^		�̐ݒ�

	ms_mp -> mes_win_pos[MS_X_P]	=	256;	//	Message Window �̏����w���W�ݒ�
	ms_mp -> mes_win_pos[MS_Y_P]	=	172;	//	Message Window �̏����x���W�ݒ�
	ms_mp -> mes_win_siz[MS_X_SIZ]	=	0;		//	Message Window �̏��������ݒ�
	ms_mp -> mes_win_siz[MS_Y_SIZ]	=	2;		//	Message Window �̏����c���ݒ�
	ms_mp -> mes_win_flg			=	0;		//	Message Window �̕\�������ɐݒ�
	ms_mp -> mes_mes_flg			=	1;		//	Message �\�������ɐݒ�
	ms_mp -> mes_next_flg			=	0;		//	Message ���s�\�������ݒ�
	ms_mp -> mes_mes_len			=	0;		//	Message �������O�ɐݒ�
	ms_mp -> mes_advance_count		=	0;		//	Message �i�s�J�E���^�̐ݒ�


	ms_p -> scrool_x				= 	MS_SCROOL_SPEED;	//	�ړ����x
	ms_p -> black_alpha				= 	0xff;				//	����
	ms_cp -> c_count				=	0;					//	�L�[���͑҂��J�E���g�̏�����
	ms_cp -> c_pos_size				=	0;					//	��������I�����̃J�[�\���̃T�C�Y�̑����l���O�ɂ���
	ms_cp -> c_pos					=	0;					//	�J�v�Z���̗h��̑傫��
	ms_cp -> main_c_pos				=	0;					//	���C���̃J�[�\���ʒu�ݒ�
	ms_cp -> sub_c_pos				=	0;					//	�ߋ��̃J�[�\���ʒu�ۑ�
	for( i = 0;i < 2;i++ ){
		ms_cp -> c_alpha[i]			=	0xff;				//	�J�[�\���̓_��
		ms_cp -> name_c_pos[i]		=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
		ms_cp -> name_c_flg[i]		=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
	}

	switch(main_old)
	{
	case	MAIN_TITLE:			//	�^�C�g�����炫��
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN ����ݒ�
		ms_p -> main_type_next	= 	MS_M_MAIN;			//	���� MAIN ����ݒ�
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL ���ł̍ŏ��̏����ݒ�
		ms_p -> main_grp_type	=	MS_M_MAIN;			//	MAIN �`��ݒ�
		ms_p ->	main_x			=	-274;				//	����W�ݒ�
		ms_p ->	stop_x			=	23;					//	�ړ��ڕW���W�ݒ�
		break;
	case	MAIN_60:			//	�Q�[���ݒ肩�痈��
	case	MAIN_NAME_ENTRY:	//	���O���͂��炫���ꍇ
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN ����ݒ�
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL ���ł̍ŏ��̏����ݒ�
		switch( evs_gamesel ){
		case	GSL_4PLAY:		//	4PLAY
			ms_p -> main_type_next	= 	MS_M_MAIN_C;				//	���� MAIN ����ݒ�
			ms_p -> main_grp_type	=	MS_M_MAIN_C;				//	MAIN �`��ݒ�
			ms_p ->	main_x			=	-274;						//	����W�ݒ�
			ms_p ->	stop_x			=	23;							//	�ړ��ڕW���W�ݒ�
			ms_cp -> main_c_pos		=	evs_4p_state_old.p_4p_p_m;	//	���C���̃J�[�\���ʒu�ݒ�
			break;
		case	GSL_VSCPU:		//	VSCOM
			if( evs_story_flg ){
				ms_p -> main_type_old	=	MS_M_MAIN_A;		//	�ȑO�� MAIN ����ݒ�
				ms_cp -> sub_c_pos		=	0;					//	���C���̃J�[�\���ʒu�ݒ�
			}else{
				ms_p -> main_type_old	=	MS_M_MAIN_B;		//	�ȑO�� MAIN ����ݒ�
				ms_cp -> sub_c_pos		=	1;					//	���C���̃J�[�\���ʒu�ݒ�
			}
			ms_p -> main_type_next		= 	MS_M_NAME_A;		//	���� MAIN ����ݒ�
			ms_p -> main_grp_type		=	MS_M_NAME_A;		//	MAIN �`��ݒ�
			ms_p ->	main_x				=	-300;				//	����W�ݒ�
			ms_p ->	stop_x				=	0;					//	�ړ��ڕW���W�ݒ�

			break;
		case	GSL_2PLAY:
			ms_cp -> sub_c_pos			=	0;					//	���C���̃J�[�\���ʒu�ݒ�
			ms_p -> main_type_old		=	MS_M_MAIN_B;		//	�ȑO�� MAIN ����ݒ�
			ms_p -> main_type_next		= 	MS_M_NAME_B;		//	���� MAIN ����ݒ�
			ms_p -> main_grp_type		=	MS_M_NAME_B;		//	MAIN �`��ݒ�
			ms_p ->	main_x				=	-300;				//	����W�ݒ�
			ms_p ->	stop_x				=	0;					//	�ړ��ڕW���W�ݒ�
			break;
		case	GSL_1PLAY:
			ms_p -> main_type_old		=	MS_M_MAIN_A;		//	�ȑO�� MAIN ����ݒ�
			ms_p -> main_type_next		= 	MS_M_NAME_A;		//	���� MAIN ����ݒ�
			ms_p -> main_grp_type		=	MS_M_NAME_A;		//	MAIN �`��ݒ�
			ms_p ->	main_x				=	-300;				//	����W�ݒ�
			ms_p ->	stop_x				=	0;					//	�ړ��ڕW���W�ݒ�
			ms_cp -> sub_c_pos			=	1;					//	���C���̃J�[�\���ʒu�ݒ�
			break;
		case	GSL_OPTION:
			ms_p -> main_type_old		=	MS_M_MAIN_D;		//	�ȑO�� MAIN ����ݒ�
			ms_p -> main_type_next		= 	MS_M_NAME_C;		//	���� MAIN ����ݒ�
			ms_p -> main_grp_type		=	MS_M_NAME_C;		//	MAIN �`��ݒ�
			ms_p ->	main_x				=	-300;				//	����W�ݒ�
			ms_p ->	stop_x				=	0;					//	�ړ��ڕW���W�ݒ�
			ms_cp -> sub_c_pos			=	2;					//	���C���̃J�[�\���ʒu�ݒ�
			break;
		}
		break;
	case	MAIN_RANK:			//	�L�^���痈��
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN ����ݒ�
		ms_p -> main_type_next	= 	MS_M_MAIN_D;		//	���� MAIN ����ݒ�
		ms_p -> sub_type		= 	MS_S_FADE_IN_A;		//	MS_M_SCROOL ���ł̍ŏ��̏����ݒ�
		ms_p -> main_grp_type	=	MS_M_MAIN_D;		//	MAIN �`��ݒ�
		ms_p ->	main_x			=	-320;				//	����W�ݒ�
		ms_p ->	stop_x			=	20;					//	�ړ��ڕW���W�ݒ�
		break;
	case	MAIN_MANUAL:		//	����������痈��
		ms_p -> main_type		= 	MS_M_SCROOL;		//	MAIN ����ݒ�
		ms_p -> main_type_next	= 	MS_M_MAIN_D_A;		//	���� MAIN ����ݒ�
		ms_p -> main_grp_type	=	MS_M_MAIN_D_A;		//	MAIN �`��ݒ�
		ms_p -> sub_type		= 	MS_S_FADE_IN_B;		//	MS_M_SCROOL ���ł̍ŏ��̏����ݒ�
		ms_p ->	main_x			=	-246;				//	����W�ݒ�
		ms_p ->	stop_x			=	37;					//	�ړ��ڕW���W�ݒ�
		ms_cp -> main_c_pos		=	evs_manual_no;		//	�J�[�\���ʒu��I��������������ɂ���
		break;
	}
}



/*--------------------------------------
	���[�h�Z���N�g�������C���֐�
--------------------------------------*/
int		dm_mode_select_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	EepRomErr	eep_err;
	int	ret;
	s16	i,j,max,omake_time = 0;
	s16	sin_f = 180,sin_f_c = 180;
	s8	sec_30_flg = 0;
	s8	sec_str_flg = 0;
	s8	out_flg = 1;	//	���[�v�E�o�p�t���O
	u8	main_cont[2];
	s8	k[2];

	//	���b�Z�[�W�L���[�̍쐬
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	���b�Z�[�W�p�̊��蓖��

	//	�N���C�A���g�ɓo�^
	nnScAddClient(sched, &client, &msgQ);


	//	�O���t�B�b�N�f�[�^�̓Ǎ���
	auRomDataRead((u32)_dm_mode_select_dataSegmentRomStart,(void *)_dm_mode_select_dataSegmentStart	, (u32)(_dm_mode_select_dataSegmentRomEnd - _dm_mode_select_dataSegmentRomStart));

	//	�ϐ�������
	init_dm_ms();

	//	��ʐ؂�ւ�����������
//	init_change_scene();

	//	��ʐ؂�ւ�������
	if( main_old != MAIN_TITLE ){
	    HCEffect_Init(gfx_freebuf[0], HC_SPHERE);
	}


	//	�R���g���[���ڑ��󋵊m�F
	evs_playmax = joyResponseCheck();
	for(i = 0;i < 2;i++){
		main_cont[i] = main_joy[i];
	}

	//	���g���[�X���b�Z�[�W�i�ی��j
	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);


	dm_init_se();
	evs_seqence = TRUE;	//	���y�t���O�𗧂Ă�
	evs_seqnumb = SEQ_Menu;	//	�Đ��Ȃ̃Z�b�g


	//	�O���t�B�b�N�̐ݒ�
	graphic_no = GFX_MODE_SELECT;


	//	���C�����[�v

	while( out_flg )
	{
		#ifndef THREAD_JOY
		joyProcCore();
		#endif

		//	���g���[�X���b�Z�[�W
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
//		calc_change_scene();	//	��ʐ؂�ւ�����

		sec_30_flg ^= 1;

		//	�ڃp�`
		if( ms_tp -> p_eye_flg ){
			ms_tp -> p_eye_count ++;
			if( ms_tp -> p_eye_count >= 3 ){	//	�ڃp�`���~
				ms_tp -> p_eye_flg		=	0;
				ms_tp -> p_eye_count	=	0;
			}
		}

		//	�e�A�j���[�V��������
		if( ms_p -> sub_type == MS_S_DEFAULT ){
			//	���͂������߂�t���O
			ms_mp -> mes_advance_count ++;
			if( ms_mp -> mes_advance_count >= 2 ){
				ms_mp -> mes_advance_count = 0;
				//	������i�s�̐ݒ�
				ms_mp -> mes_mes_len ++;
				if( ms_mp -> mes_mes_len > MS_STR_LEN_MAX ){
					ms_mp -> mes_mes_len = MS_STR_LEN_MAX;
				}
				//	���p�N�A�j���[�V�����̐ݒ�
				if( ms_tp -> p_mouth_count == 1 ){
					if( ms_mp -> mes_mes_len % 2 ){
						ms_tp -> p_mouth_flg ^= 1;
					}
				}else{
					ms_tp -> p_mouth_flg = 0;
				}
				//	�ڃp�`�A�j���[�V�����̐ݒ�
				if( !ms_tp -> p_eye_flg ){
					if( (RAND(100) % 100) == 99){
						ms_tp -> p_eye_flg = 1;
					}
				}
			}

			//	�J�v�Z���̏㉺�̗h��
			sin_f -= 4;
			if( sin_f <= 0 ){
				sin_f += 360;
			}
			ms_cp -> c_pos = sinf(DEGREE(sin_f)) * 10;

			//	�J�[�\���̓_��
			sin_f_c -= 10;
			if(sin_f_c <= 0){
				sin_f_c += 360;
			}
			ms_cp -> c_alpha[0] = ms_cp -> c_alpha[1] = 200 + (sinf(DEGREE(sin_f_c)) * 55);

			//	�J�[�\���̊g��k��
			ms_cp -> c_pos_size = (s8)(sinf( DEGREE( ms_cp -> c_pos_size_count ) ) * 8 );
			ms_cp -> c_pos_size_count += 4;
			if( ms_cp -> c_pos_size_count >= 360 ){
				ms_cp -> c_pos_size_count -= 360;
			}

		}else{
			//	�A�j���[�V�������Ȃ�
			ms_tp -> p_mouth_flg	= 0;
			ms_mp -> mes_mes_len	= 0;
			ms_cp -> c_alpha[0]		= 0xff;
			ms_cp -> c_alpha[1]		= 0xff;
		}

		switch( ms_p -> main_type )
		{
		case	MS_M_SCROOL:
			switch( ms_p -> sub_type )
			{
			case	MS_S_FADE_IN_A:		//	FADE in	A
				if( !HCEffect_Main() ){
					ms_p -> black_alpha = 0;
					ms_p -> sub_type = MS_S_TITLE_IN_A;//	�������^�C�g���ƃL�[�ƃL�m�s�I�̈ړ���
				}
				break;
			case	MS_S_FADE_IN_B:		//	FADE in	A
				if( !HCEffect_Main() ){
					ms_p -> black_alpha = 0;
					ms_p -> sub_type = MS_S_TITLE_IN_B;	//	�������^�C�g���ƃL�[�̈ړ���
				}
				break;
			case	MS_S_TITLE_IN_A:	//	TITLE type  A
				ms_tp -> p_pos[MS_X_P] -= 8;		//	�L�m�s�I�̈ړ�
				ms_tp -> k_pos[MS_X_P] -= 8;		//	�L�[�̈ړ�
				ms_tp -> t_pos[MS_Y_P] += 4;		//	�^�C�g���̈ړ�

				if( ms_tp -> t_pos[MS_Y_P] >= 12 ){		//	�^�C�g���̈ړ��C��
					ms_tp -> t_pos[MS_Y_P] = 12;
				}
				if( ms_tp -> k_pos[MS_X_P] <= 208 ){	//	�L�[�̈ړ��C��
					ms_tp -> k_pos[MS_X_P] = 208;
				}
				if( ms_tp -> p_pos[MS_X_P] <= 232 ){	//	�L�m�s�I�̈ړ��C��
					ms_tp -> p_pos[MS_X_P] 			=	232;
					ms_mp -> mes_win_siz[MS_SIZ_P]	=	0;
					ms_mp -> mes_win_flg			=	1;					//	Message Window �̕\������ɐݒ�
					ms_mp -> mes_mes_flg			=	1;					//	Message �̕\������ɐݒ�
					ms_p -> sub_type 				=	MS_S_MES_WIN_IN_A;	//	Message Window �̈ړ��E�g�又����
				}
				break;
			case	MS_S_TITLE_IN_B:	//	TITLE type  B
				ms_tp -> k_pos[MS_X_P] -= 8;			//	�L�[�̈ړ�
				ms_tp -> t_pos[MS_Y_P] += 4;			//	�^�C�g���̈ړ�

				if( ms_tp -> t_pos[MS_Y_P] >= 12 ){		//	�^�C�g���̈ړ��C��
					ms_tp -> t_pos[MS_Y_P] = 12;
				}
				if( ms_tp -> k_pos[MS_X_P] <= 208 ){		//	�L�[�̈ړ��C��
					ms_tp -> k_pos[MS_X_P] = 208;
					ms_p -> sub_type = MS_S_SCROOL_IN_A;	//	�A�C�e��(�J�v�Z���⍕��)����ʓ��Ɉړ����鏈����
				}
				break;
			case	MS_S_MES_WIN_IN_A:	//	MESSAGE WIN type A
				ms_mp -> mes_win_pos[MS_X_P]	= ms_win_pos_x[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�w���W�̐ݒ�
				ms_mp -> mes_win_pos[MS_Y_P]	= ms_win_pos_y[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�x���W�̐ݒ�
				ms_mp -> mes_win_siz[MS_X_SIZ]	= ms_win_siz_w[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�����̐ݒ�
				ms_mp -> mes_win_siz[MS_Y_SIZ]	= ms_win_siz_h[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�c���̐ݒ�
				if(	sec_30_flg  ){
					ms_mp -> mes_win_siz[MS_SIZ_P] ++;
				}
				if( ms_mp -> mes_win_siz[MS_SIZ_P] > 7 ){	//	�ړ��E�g��I��
					ms_mp -> mes_win_siz[MS_SIZ_P] = 7;
					ms_p -> sub_type = MS_S_SCROOL_IN_A;	//	�A�C�e��(�J�v�Z���⍕��)����ʓ��Ɉړ����鏈����
				}
				break;
			case	MS_S_SCROOL_IN_A:	//	ITEM in
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_type_next;	//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_MES_WIN_OUT_A:
				ms_mp -> mes_win_pos[MS_X_P]	= ms_win_pos_x[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�w���W�̐ݒ�
				ms_mp -> mes_win_pos[MS_Y_P]	= ms_win_pos_y[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�x���W�̐ݒ�
				ms_mp -> mes_win_siz[MS_X_SIZ]	= ms_win_siz_w[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�����̐ݒ�
				ms_mp -> mes_win_siz[MS_Y_SIZ]	= ms_win_siz_h[ms_mp -> mes_win_siz[MS_SIZ_P]];	//	�c���̐ݒ�
				if(	sec_30_flg  ){
					ms_mp -> mes_win_siz[MS_SIZ_P] --;
				}
				if( ms_mp -> mes_win_siz[MS_SIZ_P] <= 0 ){	//	Message Window �̈ړ��E�k�������I��
					ms_mp -> mes_win_siz[MS_SIZ_P] = 0;
					ms_mp -> mes_win_flg = 0;
					ms_mp -> mes_mes_flg = 0;
					ms_p -> sub_type = MS_S_SCROOL_OUT_A;	//	�L�m�s�I�̉�ʊO�ړ�
				}
				break;
			case	MS_S_SCROOL_OUT_A:
				ms_tp -> p_pos[MS_X_P] += 8;			//	�L�m�s�I�̈ړ�
				if( ms_tp -> p_pos[MS_X_P] >= 320 ){	//	�L�m�s�I�̈ړ��C��
					ms_tp -> p_pos[MS_X_P] 			=	320;
					ms_p -> main_grp_type			=	MS_M_MAIN_D_A;			//	�`�揈���𑀍�����I��
					ms_p -> main_type_next			=	MS_M_MAIN_D_A;			//	���C�������𑀍�����I��
					ms_p -> sub_type 				=	MS_S_SCROOL_IN_A;		//	�A�C�e��(�J�v�Z���⍕��)����ʓ��Ɉړ����鏈����
					ms_p -> main_x					=	-246;					//	����W�ݒ�
					ms_p -> stop_x					=	37;						//	��~���W�ݒ�
				}
				break;
			case	MS_S_MENU_OUT_START:	//	���[�h�Z���N�g�I�������J�n
				out_flg = 0;
				ms_p -> sub_type = MS_S_MENU_OUT_END;
				break;
			case	MS_S_MENU_OUT_END:
				break;
			}
			break;
		case	MS_M_MAIN:	//	1P/2P/4P/���̑�
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 3 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> main_type	= MS_M_SCROOL;
					ms_p -> sub_type	= MS_S_MENU_OUT_START;
					ret = MAIN_TITLE;	//	�^�C�g���ɖ߂�
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );					//	SE �ݒ�
					ms_p -> sub_type	=	 MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:	//	�J�v�Z���X�N���[���A�E�g
				evs_story_flg = 0;
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_cp -> sub_c_pos 			=	ms_cp -> main_c_pos;	//	�J�[�\���ʒu�ۑ�
					ms_cp -> main_c_pos			=	0;						//	�J�[�\���ʒu�ݒ�
					switch( ms_cp -> sub_c_pos ){
					case	0:	//	1PLAY
						ms_p -> main_x			=	-224;			//	����W�ݒ�
						ms_p -> stop_x			=	48;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_A;	//	�`��̐ݒ�
						break;
					case	1:	//	2PLAY
						ms_p -> main_x			=	-224;			//	����W�ݒ�
						ms_p -> stop_x			=	48;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_B;	//	�`��̐ݒ�
						if( evs_playmax < 2 ){
							ms_cp -> main_c_pos	=	1;				//	�J�[�\���ʒu�ݒ�
						}
						break;
					case	2:	//	4PLAY
						ms_p -> main_x			=	-274;			//	����W�ݒ�
						ms_p -> stop_x			=	23;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_C;	//	�`��̐ݒ�
						ms_cp -> main_c_pos		=	4 - evs_playmax;//	�J�[�\���ʒu�ݒ�
						break;
					case	3:	//	OTHER
						ms_p -> main_x			=	-320;			//	����W�ݒ�
						ms_p -> stop_x			=	20;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	�`��̐ݒ�
						break;
					}
					ms_p -> sub_type		 	=	MS_S_SCROOL_IN_NEXT;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_A:	//	1PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�

				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	����W�ݒ�
					ms_p -> stop_x			=	23;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	0;						//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	���̃T�u�����̐ݒ�
					ms_p -> main_type_old	=	ms_p -> main_type;		//	���݂̏����̕ۑ�
					ms_p -> main_grp_type	=	MS_M_NAME_A;			//	�`��̐ݒ�
					ms_p -> main_x			=	-300;					//	����W�ݒ�
					ms_p -> stop_x			=	0;						//	��~���W�ݒ�
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	���݂̃J�[�\���ʒu�̕ۑ�
					ms_cp -> name_c_pos[0]	=	4;						//	�J�[�\���ʒu���Q�X�g�ɐݒ�
					ms_cp -> name_c_flg[0]	=	0;						//	�J�[�\������t���O�𖢌���ɐݒ�
					evs_game_time			=	0;						//	�v���C���Ԃ̏�����
					switch( ms_cp -> sub_c_pos){
					case	0:	//	STORY
						evs_story_flg	=	1;
						evs_gamesel 	=	GSL_VSCPU;				//	�Q�[�����[�h��VSCPU�֕ύX
						break;
					case	1:	//	LEVEL
						evs_gamesel 	=	GSL_1PLAY;				//	�Q�[�����[�h��1PLAY�֕ύX
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_A;		//	���̃��C�������̐ݒ�( ���O�I�� )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_B:	//	2PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					i = 1;
					if( ms_cp -> main_c_pos == 1 ){	//	�J�[�\���ʒu�� MAN vs COM
						if( evs_playmax < 2 ){		//	�R���g���[���[���P�݂̂̏ꍇ
							i = 0;
						}
					}
					if( i ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_cp -> main_c_pos --;
						ms_mp -> mes_mes_len = 0;
						if( ms_cp -> main_c_pos < 0 ){
							ms_cp -> main_c_pos = 1;
						}
					}

				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					i = 1;
					if( ms_cp -> main_c_pos == 1 ){	//	�J�[�\���ʒu�� MAN vs COM
						if( evs_playmax < 2 ){		//	�R���g���[���[���P�݂̂̏ꍇ
							i = 0;
						}
					}
					if( i ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_cp -> main_c_pos ++;
						ms_mp -> mes_mes_len = 0;
						if( ms_cp -> main_c_pos > 1 ){
							ms_cp -> main_c_pos = 0;
						}
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					if( ms_cp -> main_c_pos == 0 ){		//	VSMAN �̏ꍇ
						if( evs_playmax >= 2 ){			//	2CON �ȏ�łȂ��Ǝ��ɐi�߂Ȃ�
							ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�
						}
					}else{
						ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	����W�ݒ�
					ms_p -> stop_x			=	23;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	1;						//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	���̃T�u�����̐ݒ�
					ms_p -> main_type_old	=	ms_p -> main_type;		//	���݂̏����̕ۑ�
					ms_p -> main_x			=	-300;					//	����W�ݒ�
					ms_p -> stop_x			=	0;						//	��~���W�ݒ�
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	���݂̃J�[�\���ʒu�̕ۑ�
					evs_game_time			=	0;						//	�v���C���Ԃ̏�����
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
						ms_cp -> name_c_flg[i]	=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
					}

					switch( ms_cp -> sub_c_pos){
					case	0:	//	VSMAN
						evs_gamesel 	=	GSL_2PLAY;				//	�Q�[�����[�h��2PLAY�֕ύX
						ms_p -> main_grp_type	=	MS_M_NAME_B;	//	�`��̐ݒ�
						break;
					case	1:	//	VSCPU
						evs_gamesel 	=	GSL_VSCPU;				//	�Q�[�����[�h��VSCPU�֕ύX
						ms_p -> main_grp_type	=	MS_M_NAME_A;	//	�`��̐ݒ�
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_C:	//	4PLAY
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					j = ms_cp -> main_c_pos;	//	�ړ����������ׂ邽�߃J�[�\���ʒu�̕ۑ�
					if( evs_playmax < (( 4 - ms_cp -> main_c_pos ) + 1 ) ){
						ms_cp -> main_c_pos = 3;
					}else{
						ms_cp -> main_c_pos --;
					}
					if( j != ms_cp -> main_c_pos ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_mp -> mes_mes_len = 0;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					j = ms_cp -> main_c_pos;	//	�ړ����������ׂ邽�߃J�[�\���ʒu�̕ۑ�
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 3 ){
						ms_cp -> main_c_pos = 4 - evs_playmax;
					}
					if( j != ms_cp -> main_c_pos ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_mp -> mes_mes_len = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					evs_gamesel	=	GSL_4PLAY;	//	�Q�[�����[�h���S�l�p�֕ύX
					ret			=	MAIN_60;	//	�Q�[���ݒ��
					evs_4p_state_old.p_4p_p_m = ms_cp -> main_c_pos;	//	�I�������X�^�C���̕ۑ�
					switch( ms_cp -> main_c_pos ){
					case	0:	//	4MAN
						if( evs_playmax == 4){
							dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for( i = 0;i < 4;i++ ){
								game_state_data[i].player_state[PS_PLAYER]				= 	PUF_PlayerMAN;	//	�S���l
							}
						}
						break;
					case	1:	//	3MAN & 1COM
						if( evs_playmax >= 3){
							dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for(i = j = 0;i < 4;i++ ){
								if( link_joy[i] ){
									if( j < 3 ){
										game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerMAN;	//	�l
										j++;
									}else{
										game_state_data[i].player_state[PS_PLAYER] 				=	PUF_PlayerCPU;	//	CPU
									}
								}else{
									game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
								}
							}
						}
						break;
					case	2:	//	2MAN & 2COM
						if( evs_playmax >= 2){
							dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
							ms_p -> main_type	=	MS_M_SCROOL;
							ms_p -> sub_type	=	MS_S_MENU_OUT_START;
							for(i = j = 0;i < 4;i++ ){
								if( link_joy[i] ){
									if( j < 2 ){
										game_state_data[i].player_state[PS_PLAYER] 				=	PUF_PlayerMAN;	//	�l
										j++;
									}else{
										game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerCPU;	//	CPU
									}
								}else{
									game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
								}
							}
						}
						break;
					case	3:	//	1MAN & 3COM
						dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
						ms_p -> main_type	=	MS_M_SCROOL;
						ms_p -> sub_type	=	MS_S_MENU_OUT_START;
						for(i = j = 0;i < 4;i++ ){
							if( link_joy[i] ){
								if( j < 1 ){
									game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerMAN;	//	�l
									j++;
								}else{
									game_state_data[i].player_state[PS_PLAYER]				=	PUF_PlayerCPU;	//	CPU
								}
							}else{
								game_state_data[i].player_state[PS_PLAYER]					=	PUF_PlayerCPU;	//	CPU
							}
						}
						break;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	����W�ݒ�
					ms_p -> stop_x			=	23;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	2;						//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D:	//	OTHER
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 4;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					ms_mp -> mes_mes_len = 0;
					if( ms_cp -> main_c_pos > 4 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-274;					//	����W�ݒ�
					ms_p -> stop_x			=	23;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN;				//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	3;						//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN;				//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> sub_type		=	MS_S_SCROOL_IN_NEXT;	//	���̃T�u�����̐ݒ�
					ms_p -> main_type_old	=	ms_p -> main_type;		//	���݂̏����̕ۑ�
					ms_cp -> sub_c_pos		=	ms_cp -> main_c_pos;	//	���݂̃J�[�\���ʒu�̕ۑ�
					ms_cp -> main_c_pos		=	0;

					switch( ms_cp -> sub_c_pos )
					{
					case	0:	//	�L�^������
						ms_p -> main_type = MS_M_SCROOL;
						ms_p -> sub_type = MS_S_MENU_OUT_START;
						ret = MAIN_RANK;	//	�L�^���[�h�ɍs��
						break;
					case	1:	//	�V�ѕ�
						ms_p -> main_type 		= 	MS_M_SCROOL;
						ms_p -> sub_type 		= 	MS_S_MES_WIN_OUT_A;
						break;
					case	2:	//	���O
						ms_p -> main_x			=	-300;					//	����W�ݒ�
						ms_p -> stop_x			=	0;						//	��~���W�ݒ�
						for( i = 0;i < 2;i++ ){
							ms_cp -> name_c_pos[i]	=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
							ms_cp -> name_c_flg[i]	=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
						}
						ms_p -> main_grp_type	=	MS_M_NAME_C;	//	�`��̐ݒ�
						break;
					case	3:	//	�T�E���h
						ms_p -> main_x			=	-224;			//	����W�ݒ�
						ms_p -> stop_x			=	48;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_D_C;	//	�`��̐ݒ�
						break;
					case	4:	//	�o�b�N�A�b�v
						ms_p -> main_x			=	-224;			//	����W�ݒ�
						ms_p -> stop_x			=	48;				//	��~���W�ݒ�
						ms_p -> main_grp_type	=	MS_M_MAIN_D_D;	//	�`��̐ݒ�
						break;
					}
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_grp_type;	//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_A:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> main_c_pos > 0 ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_cp -> main_c_pos --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> main_c_pos < 5 ){
						dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
						ms_cp -> main_c_pos ++;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					evs_manual_no 		= ms_cp -> main_c_pos;
					ms_p -> main_type	= MS_M_SCROOL;
					ms_p -> sub_type 	= MS_S_MENU_OUT_START;
					ret = MAIN_MANUAL;	//	��������ɍs��
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;				//	����W�ݒ�
					ms_p -> stop_x			=	20;					//	��~���W�ݒ�
					ms_p -> main_type_next	=	MS_M_MAIN_D;		//	���̃��C�������̐ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_D;		//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	1;					//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_TITLE_IN_A;	//	�X�N���[���C�������ݒ�
					ms_p ->	main_type		=	MS_M_SCROOL;		//	�X�N���[���C�������ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_C:	//	�T�E���h
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	�X�N���[���A�E�g�����ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	����W�ݒ�
					ms_p -> stop_x			=	20;				//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	3;				//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_D:	//	�o�b�N�A�b�v�����m�F
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					switch( ms_cp -> main_c_pos ){
					case	0:	//	�L�����Z��
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	�X�N���[���A�E�g�����ݒ�
						break;
					case	1:
						//	������

						for(i = 0;i < 9;i++)
						{
							dm_init_save_mem( &evs_mem_data[i], i );
						}
						dm_init_config_4p_save();
						ms_mp -> mes_mes_len 	= 	0;
#ifdef	_DM_EEP_ROM_USE_
						EepRom_CreateThread();
						EepRom_SendTask(dm_ms_eep_write, (u32)&eep_err);
						ms_p -> sub_type		=	MS_S_EEP_WAIT;		//	���̏���
						break;

#else
						ms_p -> main_type 		=	MS_M_MAIN_D_D_A;	//	����
						ms_p -> main_grp_type	=	MS_M_MAIN_D_D_A;	//	�`��̐ݒ�
						break;
#endif	//	_DM_EEP_ROM_USE_

					}
				}
				break;
			case	MS_S_EEP_WAIT:
#ifdef	_DM_EEP_ROM_USE_
				if( EepRom_GetTaskCount() == 0 ){	//	EEP �������݊���
					EepRom_DestroyThread();
					ms_p -> main_type		=	MS_M_MAIN_D_D_A;	//	���̏���
					ms_p -> main_grp_type	=	MS_M_MAIN_D_D_A;	//	���̏���
				}
#endif	//	_DM_EEP_ROM_USE_
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	����W�ݒ�
					ms_p -> stop_x			=	20;				//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	4;				//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_D_A:	//	BACKUP ERACE
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( ms_tp -> p_mouth_count == -1 ){	//	������I��
					ms_cp -> c_count = (ms_cp -> c_count + 1) % 24;
					if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	���j���[�I������(�^�C�g���ɖ߂�)
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;			//	����W�ݒ�
					ms_p -> stop_x			=	20;				//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_D;	//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	4;				//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_NAME_A:	//	NAME SELECT 1P
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] ++;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> name_c_pos[0] > 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] -= 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> name_c_pos[0] < 6 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] += 3;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					if( ms_cp -> name_c_pos[0] == 4 ){	//	�Q�X�g
						j = -1;
						evs_select_name_no[0] = DM_MEM_GUEST;
					}else	if( ms_cp -> name_c_pos[0] < 4 ){
						evs_select_name_no[0] = j = ms_cp -> name_c_pos[0];
					}else	if( ms_cp -> name_c_pos[0] > 4 ){
						j = ms_cp -> name_c_pos[0] - 1;
						evs_select_name_no[0] = j;
					}
					if( j == -1 ){
						ret = MAIN_60;	//	�Q�[���ݒ��
					}else{
						if(	evs_mem_data[j].mem_use_flg & DM_MEM_USE ){	//	���O�L��
							ret = MAIN_60;	//	�Q�[���ݒ��
						}else	if( ( evs_mem_data[j].mem_use_flg & DM_MEM_USE ) == 0 ){	//	���O����
							evs_neme_entry_flg[0] = 1;
							ret = MAIN_NAME_ENTRY;
						}
					}
					ms_p -> sub_type	= MS_S_MENU_OUT_START;	//	�I��
					ms_p -> main_type	= MS_M_SCROOL;			//	�I��
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-224;					//	����W�ݒ�
					ms_p -> stop_x			=	48;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	ms_p -> main_type_old;	//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	ms_cp -> sub_c_pos;		//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	ms_p -> main_type_old;	//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_NAME_B:	//	NAME SELECT	2P
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				for(i = 0;i < 2;i++ ){
					if( !ms_cp -> name_c_flg[i] ){	//	������
						if( joycur[main_cont[i]] & DM_KEY_LEFT) {
							if( ( ms_cp -> name_c_pos[i] % 3 ) == 2 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
								if( ms_cp -> name_c_pos[i] - 1 == ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪�����ꍇ
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	���肪�^�񒆂������ꍇ
										ms_cp -> name_c_pos[i] --;
									}else{
										ms_cp -> name_c_pos[i] -= 2;
									}
								}else{
									//	���Ȃ��ꍇ
									ms_cp -> name_c_pos[i] --;
								}
							}else	if( ( ms_cp -> name_c_pos[i] % 3) == 1 ){
								if( ms_cp -> name_c_pos[i] - 1 != ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪���Ȃ��ꍇ
									dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
									ms_cp -> name_c_pos[i] --;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_RIGHT ) {
							if( ( ms_cp -> name_c_pos[i] % 3) == 0 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
								if( ms_cp -> name_c_pos[i] + 1 == ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪�����ꍇ
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	���肪�^�񒆂������ꍇ
										ms_cp -> name_c_pos[i] ++;
									}else{
										ms_cp -> name_c_pos[i] += 2;
									}
								}else{
									//	���Ȃ��ꍇ
									ms_cp -> name_c_pos[i] ++;
								}
							}else	if( ( ms_cp -> name_c_pos[i] % 3) == 1 ){
								if( ms_cp -> name_c_pos[i] + 1 != ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪���Ȃ��ꍇ
									dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
									ms_cp -> name_c_pos[i] ++;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_UP) {
							if( ms_cp -> name_c_pos[i] > 5 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
								if( ms_cp -> name_c_pos[i] - 3 == ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪�����ꍇ
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	���肪�^�񒆂������ꍇ
										ms_cp -> name_c_pos[i] -= 3;
									}else{
										ms_cp -> name_c_pos[i] -= 6;
									}
								}else{
									ms_cp -> name_c_pos[i] -= 3;
								}
							}else	if( ms_cp -> name_c_pos[i] > 2 ){
								if( ms_cp -> name_c_pos[i] - 3 != ms_cp -> name_c_pos[i ^ 1] ){
									dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
									ms_cp -> name_c_pos[i] -= 3;
								}
							}
						}else	if( joycur[main_cont[i]] & DM_KEY_DOWN) {
							if( ms_cp -> name_c_pos[i] < 3 ){
								dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
								if( ms_cp -> name_c_pos[i] + 3 == ms_cp -> name_c_pos[i ^ 1] ){
									//	�ړ���ɑ��肪�����ꍇ
									if( ms_cp -> name_c_pos[i ^ 1] == 4 ){
										//	���肪�^�񒆂������ꍇ
										ms_cp -> name_c_pos[i] += 3;
									}else{
										ms_cp -> name_c_pos[i] += 6;
									}
								}else{
									ms_cp -> name_c_pos[i] += 3;
								}
							}else	if( ms_cp -> name_c_pos[i] < 6 ){
								if( ms_cp -> name_c_pos[i] + 3 != ms_cp -> name_c_pos[i ^ 1] ){
									dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
									ms_cp -> name_c_pos[i] += 3;
								}
							}
						}else	if( joyupd[main_cont[i]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
							ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
						}else	if( joyupd[main_cont[i]] & DM_KEY_OK ){	//	����
							dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
							ms_cp -> name_c_flg[i] = 1;
						}
					}else{
						if( joyupd[main_cont[i]] & DM_KEY_B ){	//	����L�����Z��
							ms_cp -> name_c_flg[i] = 0;
						}
					}
				}
				if( ms_cp -> name_c_flg[0] == 1 && ms_cp -> name_c_flg[1] == 1){
					//	�������肵���ꍇ
					for( i = 0;i < 2;i++ ){
						evs_neme_entry_flg[i] = 0;
						if( ms_cp -> name_c_pos[i] == 4 ){
							k[i] = -1;
							evs_select_name_no[i] = DM_MEM_GUEST;	//	�Q�X�g�͂W��
						}else{
							if( ms_cp -> name_c_pos[i] < 4 ){
								k[i] = evs_select_name_no[i] = ms_cp -> name_c_pos[i];
							}else	if( ms_cp -> name_c_pos[i] > 4 ){
								k[i] = evs_select_name_no[i] = ms_cp -> name_c_pos[i] - 1;
							}
						}
					}
					for( i = 0;i < 2;i++ ){
						if( k[i] >= 0 ){
							if( (evs_mem_data[evs_select_name_no[i]].mem_use_flg & DM_MEM_USE) == 0 ){
								evs_neme_entry_flg[i] = 1;
							}
						}
					}
					if( evs_neme_entry_flg[0] == 0 && evs_neme_entry_flg[1] == 0 ){
						ret = MAIN_60;
					}else{
						ret = MAIN_NAME_ENTRY;
					}
					ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	�I��
					ms_p -> main_type		=	MS_M_SCROOL;			//	�I��
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-224;					//	����W�ݒ�
					ms_p -> stop_x			=	48;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_B;			//	�`��̐ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
					ms_cp -> main_c_pos		=	0;						//	�J�[�\���ʒu�ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_B;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;			break;
		case	MS_M_NAME_C:	//	OTHER
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 0 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] --;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					if( ( ms_cp -> name_c_pos[0] % 3) != 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] ++;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_UP) {
					if( ms_cp -> name_c_pos[0] > 2 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] -= 3;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_DOWN) {
					if( ms_cp -> name_c_pos[0] < 6 ){
						dm_set_menu_se( SE_mLeftRight );	//	SE �ݒ�
						ms_cp -> name_c_pos[0] += 3;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					if( ms_cp -> name_c_pos[0] == 4 ){
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;	//	�X�N���[���A�E�g�����ݒ�
					}else{
						if( ms_cp -> name_c_pos[0] < 4 ){
							ms_cp -> neme_d_pos = ms_cp -> name_c_pos[0];
						}else	if( ms_cp -> name_c_pos[0] > 4 ){
							ms_cp -> neme_d_pos = ms_cp -> name_c_pos[0] - 1;
						}
						if( ( evs_mem_data[ms_cp -> neme_d_pos].mem_use_flg & DM_MEM_USE ) == 0 ){	//	���O����
							evs_gamesel				=	GSL_OPTION;				//	�I�v�V�������[�h
							evs_neme_entry_flg[0]	=	1;
							evs_select_name_no[0]	=	ms_cp -> neme_d_pos;
							ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	�I��
							ms_p -> main_type		=	MS_M_SCROOL;			//	�I��
							ret = MAIN_NAME_ENTRY;	//	���O����
						}else{
							ms_p -> sub_type = MS_S_SCROOL_OUT_NEXT;	//	�X�N���[���A�E�g�����ݒ�
						}
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-320;					//	����W�ݒ�
					ms_p -> stop_x			=	20;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_MAIN_D;			//	�`��̐ݒ�
					ms_cp -> main_c_pos		=	2;						//	�J�[�\���ʒu�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B;			//	�`��̐ݒ�
					ms_p -> main_x			=	-224;					//	����W�ݒ�
					ms_p -> stop_x			=	48;						//	��~���W�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B;		//	���̃��C�������̐ݒ�( ���O���ǂ����邩 )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					switch( ms_cp -> main_c_pos ){
					case	0:	//	�ύX
						evs_gamesel				=	GSL_OPTION;				//	�I�v�V�������[�h
						evs_neme_entry_flg[0]	=	1;
						evs_select_name_no[0]	=	ms_cp -> neme_d_pos;
						ms_p -> sub_type		=	MS_S_MENU_OUT_START;	//	�I��
						ms_p -> main_type		=	MS_M_SCROOL;			//	�I��
						ret = MAIN_NAME_ENTRY;	//	���O����
						break;
					case	1:	//	�폜
						ms_p -> sub_type		=	MS_S_SCROOL_OUT_NEXT;	//	���̏���
						ms_cp -> main_c_pos 	=	0;
						break;
					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	����W�ݒ�
					ms_p -> stop_x			=	0;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	�`��̐ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
						ms_cp -> name_c_flg[i]	=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_A;		//	�`��̐ݒ�
					ms_p -> main_x			=	-224;					//	����W�ݒ�
					ms_p -> stop_x			=	48;						//	��~���W�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B_A;	//	���̃��C�������̐ݒ�( ���O���ǂ����邩 )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B_A:
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:		//	KEY ����
				if( joycur[main_cont[0]] & DM_KEY_LEFT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos --;
					if( ms_cp -> main_c_pos < 0 ){
						ms_cp -> main_c_pos = 1;
					}
				}else	if( joycur[main_cont[0]] & DM_KEY_RIGHT) {
					dm_set_menu_se( SE_mUpDown );	//	SE �ݒ�
					ms_cp -> main_c_pos ++;
					if( ms_cp -> main_c_pos > 1 ){
						ms_cp -> main_c_pos = 0;
					}
				}else	if( joyupd[main_cont[0]] & DM_KEY_B ){	//	���j���[�I������(�^�C�g���ɖ߂�)
					ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;
				}else	if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	����
					dm_set_menu_se( SE_mDecide );		//	SE �ݒ�
					switch( ms_cp -> main_c_pos ){
					case	0:	//	�L�����Z��
						ms_p -> sub_type		=	MS_S_SCROOL_OUT_BACK;
						break;
					case	1:	//	�폜
						ms_mp -> mes_mes_len	=	0;
						for( i = 0;i < 8;i++ ){
							evs_mem_data[i].vs_data[ms_cp -> neme_d_pos][0] = 0;
							evs_mem_data[i].vs_data[ms_cp -> neme_d_pos][1] = 0;
						}
						dm_init_save_mem( &evs_mem_data[ms_cp -> neme_d_pos],ms_cp -> neme_d_pos  );
#ifdef	_DM_EEP_ROM_USE_
						EepRom_CreateThread();
						EepRom_SendTask(dm_ms_eep_write, (u32)&eep_err);
						ms_p -> sub_type		=	MS_S_EEP_WAIT;		//	���̏���
						break;

#else
						ms_p -> main_type		=	MS_M_MAIN_D_B_B;	//	���̏���
						ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;	//	���̏���
						break;
#endif	//	_DM_EEP_ROM_USE_
					}
				}
				break;
			case	MS_S_EEP_WAIT:
#ifdef	_DM_EEP_ROM_USE_
				if( EepRom_GetTaskCount() == 0 ){	//	EEP �������݊���
					EepRom_DestroyThread();
					ms_p -> main_type		=	MS_M_MAIN_D_B_B;	//	���̏���
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;	//	���̏���
				}
#endif	//	_DM_EEP_ROM_USE_
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	����W�ݒ�
					ms_p -> stop_x			=	0;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	�`��̐ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
						ms_cp -> name_c_flg[i]	=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			case	MS_S_SCROOL_OUT_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_grp_type	=	MS_M_MAIN_D_B_B;		//	�`��̐ݒ�
					ms_p -> main_x			=	-224;					//	����W�ݒ�
					ms_p -> stop_x			=	48;						//	��~���W�ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_NEXT;	//	�X�N���[���C�������ݒ�
				}
				break;
			case	MS_S_SCROOL_IN_NEXT:
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_MAIN_D_B_B;	//	���̃��C�������̐ݒ�( ���O���ǂ����邩 )
					ms_p -> sub_type	=	MS_S_DEFAULT;		//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		case	MS_M_MAIN_D_B_B:	//	���O�폜���܂���
			switch( ms_p -> sub_type )
			{
			case	MS_S_DEFAULT:
				if( ms_tp -> p_mouth_count == -1 ){	//	������I��
					ms_cp -> c_count = (ms_cp -> c_count + 1) % 24;
					if( joyupd[main_cont[0]] & DM_KEY_OK ){	//	���j���[�I������(�^�C�g���ɖ߂�)
						ms_p -> sub_type = MS_S_SCROOL_OUT_BACK;

					}
				}
				break;
			case	MS_S_SCROOL_OUT_BACK:		//	�J�v�Z���X�N���[���A�E�g
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= 320 ){
					ms_p -> main_x			=	-300;					//	����W�ݒ�
					ms_p -> stop_x			=	0;						//	��~���W�ݒ�
					ms_p -> main_grp_type	=	MS_M_NAME_C;			//	�`��̐ݒ�
					ms_p ->	sub_type		=	MS_S_SCROOL_IN_BACK;	//	�X�N���[���C�������ݒ�
					for( i = 0;i < 2;i++ ){
						ms_cp -> name_c_pos[i]	=	4;					//	�J�[�\���ʒu���Q�X�g�ɐݒ�
						ms_cp -> name_c_flg[i]	=	0;					//	�J�[�\������t���O�𖢌���ɐݒ�
					}
				}
				break;
			case	MS_S_SCROOL_IN_BACK:		//	�J�v�Z���X�N���[���C��
				ms_p -> main_x += ms_p -> scrool_x;
				if( ms_p -> main_x >= ms_p -> stop_x ){	//	ITEM �ړ��C��
					ms_p -> main_x		=	ms_p -> stop_x;
					ms_p -> main_type	=	MS_M_NAME_C;			//	���̃��C�������̐ݒ�
					ms_p -> sub_type	=	MS_S_DEFAULT;			//	���̃T�u�����̐ݒ�
				}
				break;
			}
			break;
		}

		//	���O�I���Ō��肵�Ă���ꍇ
		for( i = 0;i < 2;i++ ){
			if( ms_cp -> name_c_flg[i] ){
				ms_cp -> c_alpha[i] = 0xff;
			}
		}

		dm_menu_se_play();	//	SE�Đ�
	}

	//	�\����~
	graphic_no = GFX_NULL;

	//	�O���t�B�b�N�I���҂�
	while( pendingGFX != 0 ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	//	�N���C�A���g�̖���
	nnScRemoveClient(sched, &client);

	main_old = MAIN_MODE_SELECT;

	return	ret;
}

/*--------------------------------------
	���[�h�Z���N�g�`�惁�C���֐�
--------------------------------------*/
void	dm_mode_select_graphic(void)
{
	NNScTask	*gt;
	s16			i,j,k,p;
	s16			x_pos,y_pos;
	s16			skip,max;
	s16			n_x_pos[] = {36,128,220,36,128,220,36,128,220};
	s16			n_y_pos[] = {52,52,52,84,84,84,116,116,116};
	s8			cursor_x[2],cursor_h[2],cursor_g[2][2];
	u8			f_alpha[] = {128,255};
	u8			**menu_p;
	u16			*str_p;


	// �ި���ڲؽ��ޯ̧�����ޯ̧�̎w��
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[0]));	//	�A�C�e���i�J�v�Z�����j�̃Z�O�����g�ݒ�

	F3RCPinitRtn();
	F3ClearFZRtn(0);

	gSPDisplayList(gp++, S2Spriteinit_dl);

	//	�`��͈͂̎w��
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	�`��ݒ�
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

	//	�L�m�s�I�`��
	load_TexPal( dm_mode_select_pino_bm0_0tlut );
	load_TexBlock_4b( &dm_mode_select_pino_bm0_0[0],64,42 );
	draw_Tex( ms_tp -> p_pos[MS_X_P],ms_tp -> p_pos[MS_Y_P],64,42,0,0 );
	load_TexBlock_4b( &dm_mode_select_pino_bm0_0[1344],64,42 );
	draw_Tex( ms_tp -> p_pos[MS_X_P],ms_tp -> p_pos[MS_Y_P] + 42,64,42,0,0 );
	if( ms_tp -> p_mouth_flg ){	//	��
		load_TexTile_4b( &dm_mode_select_pino_mouth_bm0_0[0],8,5,0,0,7,4 );
		draw_Tex( ms_tp -> p_pos[MS_X_P] + 24,ms_tp -> p_pos[MS_Y_P] + 34,8,5,0,0 );
	}
	if( ms_tp -> p_eye_flg ){	//	��
		load_TexTile_4b( &dm_mode_select_pino_eye_bm0_0[0],12,7,0,0,11,6 );
		draw_Tex( ms_tp -> p_pos[MS_X_P] + 24,ms_tp -> p_pos[MS_Y_P] + 28,12,7,0,0 );
	}

	//	�{�^�������̕`��
	load_TexPal(dm_mode_select_bt_ab_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_bt_ab_bm0_0,80,32);
	draw_Tex( ms_tp -> k_pos[MS_X_P],ms_tp -> k_pos[MS_Y_P],80,32,0,0);

	//	���j���[���̕`��

	load_TexPal(dm_mode_select_menu_bm0_0tlut);
	load_TexBlock_4b(dm_mode_select_menu_bm0_0,160,25);
	draw_Tex( ms_tp -> t_pos[MS_X_P],ms_tp -> t_pos[MS_Y_P],160,25,0,0);
	load_TexBlock_4b(&dm_mode_select_menu_bm0_0[2000],160,6);
	draw_Tex( ms_tp -> t_pos[MS_X_P],ms_tp -> t_pos[MS_Y_P] + 25,160,6,0,0);




	if( ms_mp -> mes_win_flg ){
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );

		//	���b�Z�[�W�E�C���h�E�̕`��
		disp_rect( 64,64,64,176,ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P],
									ms_mp -> mes_win_siz[MS_X_SIZ],ms_mp -> mes_win_siz[MS_Y_SIZ] );
	}

	//	���O�p���n
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A://	���O�I��	�P�l�p
	case	MS_M_NAME_B://	���O�I��	�Q�l�p
	case	MS_M_NAME_C://	���O�I��	���̑��p
		for(i = 0;i < 4;i++){
			disp_rect( 64,64,64,176,ms_p -> main_x + n_x_pos[i],n_y_pos[i],64,16 );
			disp_rect( 64,64,64,176,ms_p -> main_x + n_x_pos[i + 5],n_y_pos[i + 5],64,16 );
		}
		break;
	case	MS_M_MAIN_D_B:		//	���O�I��	���̑��p
	case	MS_M_MAIN_D_B_A:	//	���̑��őI���������O
	case	MS_M_MAIN_D_B_B:	//	���̑��őI�������폜
		disp_rect(64,64,64,176,ms_p -> main_x + n_x_pos[1] - 48,n_y_pos[1],64,16);
		break;
	}


	//	�`��ݒ�
	gSPDisplayList(gp++, Texture_TE_dl );

	max = skip = 0;
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_MAIN:
		//	���C�����j���[
		max 	= 4;									//	�J�v�Z���\����
		skip 	= 70;									//	�`��Ԋu
		menu_p 	= ms_cap_256_a;							//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= ms_str_a[ms_cp -> main_c_pos];			//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_A:	//	1PALY
		max 	= 2;									//	�J�v�Z���\����
		skip	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_aa;						//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= ms_str_b[ms_cp -> main_c_pos];		//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_B:	//	2PLAY
		max 	= 2;									//	�J�v�Z���\����
		skip	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_b;							//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= ms_str_c[ms_cp -> main_c_pos];		//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_C:	//	4PLAY
		max 	= 4;									//	�J�v�Z���\����
		skip	= 70;									//	�`��Ԋu
		menu_p 	= ms_cap_256_c;							//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= ms_str_d[ms_cp -> main_c_pos];		//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D:	//	OTHER
		max 	= 5;									//	�J�v�Z���\����
		skip	= 54;									//	�`��Ԋu
		menu_p 	= ms_cap_256_d;							//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= ms_str_e[ms_cp -> main_c_pos];		//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D_C:	//	SOUND
		max 	= 2;									//	�J�v�Z���\����
		skip	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_sound;							//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_sound;				//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D_D:	//	BACKUP
		max 	= 2;									//	�J�v�Z���\����
		skip	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_backup;					//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_check;				//	���̓f�[�^�̃Z�b�g
		break;
	case	MS_M_NAME_A:
	case	MS_M_NAME_B:
		str_p	= mode_select_str_name;					//	���O�I��p���͂̃Z�b�g
		break;
	case	MS_M_NAME_C:
		str_p	= mode_select_ext_str_name;				//	���̑��p���O�I��p���͂̃Z�b�g
		break;
	case	MS_M_MAIN_D_B:
		max 	= 2;									//	�J�v�Z���\����
		skip 	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_name;						//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_do_name;				//	���O���ǂ����邩�̕��̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D_B_A:	//	���̑��őI���������O
		max 	= 2;									//	�J�v�Z���\����
		skip 	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_backup;					//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_check;				//	���O���ǂ����邩�̕��̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D_D_A:	//	�o�b�N�A�b�v�폜
		max 	= 2;									//	�J�v�Z���\����
		skip 	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_backup;					//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_erace_backup;			//	���O���ǂ����邩�̕��̓f�[�^�̃Z�b�g
		break;
	case	MS_M_MAIN_D_B_B:	//	���̑��őI���������O
		max 	= 2;									//	�J�v�Z���\����
		skip 	= 160;									//	�`��Ԋu
		menu_p 	= ms_cap_256_backup;					//	�O���t�B�b�N�f�[�^�̃Z�b�g
		str_p	= mode_select_str_erace_name;			//	���O���ǂ����邩�̕��̓f�[�^�̃Z�b�g
		break;
	}

	//	�J�v�Z���`��
	if( max ){
		load_TexPal( dm_ms_capsel_pal_bm0_0tlut );
		x_pos = ms_p -> main_x;	//	����W�̐ݒ�
		for( i = 0;i < max;i++ ){
			switch( ms_p -> main_grp_type )
			{
			case	MS_M_MAIN_B:	//	2PLAY
				if( i == 0 ){
					if( evs_playmax < 2 ){
						gDPSetPrimColor(gp++,0,0,128,128,128,255);
					}else{
						gDPSetPrimColor(gp++,0,0,255,255,255,255);
					}
				}else{
					gDPSetPrimColor(gp++,0,0,255,255,255,255);
				}
				break;
			case	MS_M_MAIN_C:	//	4PLAY
				if( i < (4 - evs_playmax ) ){
					gDPSetPrimColor(gp++,0,0,128,128,128,255);
				}else{
					gDPSetPrimColor(gp++,0,0,255,255,255,255);
				}

				break;
			}

			if( ms_cp -> main_c_pos != i ){
				//	�k���`��
				y_pos = 83;
				for( j = 0;j < 2;j++ ){
					load_TexBlock_8b( menu_p[i] + 2048 * j,64,32);
					draw_ScaleTex(x_pos + 16,y_pos,64,32,32,16,0,0);
					y_pos += 16;
				}
			}else	if( ms_cp -> main_c_pos == i ){
				//	�ʏ�`��
				y_pos = 67 + ms_cp -> c_pos;
				for( j = 0;j < 2;j++ ){
					load_TexBlock_8b( menu_p[i] + 2048 * j,64,32);
					draw_Tex(x_pos,y_pos,64,32,0,0);
					y_pos += 32;
				}
			}
			x_pos += skip;
		}
	}

	if( ms_mp -> mes_win_flg ){
		load_TexPal( dm_ms_mes_win_side_a_bm0_0tlut);
		load_TexTile_4b(dm_ms_mes_win_side_b_bm0_0,4,3,0,0,3,2);	//	�㉺�g
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P] - 3,ms_mp -> mes_win_siz[MS_X_SIZ],3,0,0);
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P],ms_mp -> mes_win_pos[MS_Y_P] + ms_mp -> mes_win_siz[MS_Y_SIZ],ms_mp -> mes_win_siz[MS_X_SIZ],3,0,0);

		load_TexTile_4b(dm_ms_mes_win_side_a_bm0_0,4,80,0,0,3,79);	//	���E�g
		draw_Tex( ms_mp -> mes_win_pos[MS_X_P] - 3,ms_mp -> mes_win_pos[MS_Y_P] - 3,3,ms_mp -> mes_win_siz[MS_Y_SIZ] + 6,0,0);
		draw_TexFlip( ms_mp -> mes_win_pos[MS_X_P] + ms_mp -> mes_win_siz[MS_X_SIZ] -1,ms_mp -> mes_win_pos[MS_Y_P] - 3,3,ms_mp -> mes_win_siz[MS_Y_SIZ] + 6,0,0,flip_on,flip_off);
	}

	//	���O�p�v���[�g
	load_TexPal(dm_mode_select_name_plate_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,dm_mode_select_name_gest[2], G_IM_FMT_CI,72,24,0,0,71,23,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A://	���O�I��	�P�l�p
	case	MS_M_NAME_B://	���O�I��	�Q�l�p
	case	MS_M_NAME_C://	���O�I��	���̑��p
		for(i = 0;i < 4;i++){
			draw_Tex( ms_p -> main_x + n_x_pos[i] - 4,n_y_pos[i] - 4,72,24,0,0 );
			draw_Tex( ms_p -> main_x + n_x_pos[i + 5] - 4,n_y_pos[i + 5] - 4,72,24,0,0 );
		}
		break;
	case	MS_M_MAIN_D_A:
		load_TexPal( dm_mode_select_black_board_bm0_0tlut );	//	�p���b�g���[�h
		load_TexTile_4b( &dm_mode_select_black_board_bm0_0[0],246,16,0,0,245,15 );
		draw_Tex( ms_p -> main_x,51,246,16,0,0 );	//	�㕔
		draw_ScaleTex( ms_p -> main_x,67,246,16,246,103,0,10 );	//	����
		draw_TexFlip( ms_p -> main_x,170,246,16,0,0,flip_off,flip_on );	//	����
		break;
	case	MS_M_MAIN_D_B:		//	���̑��őI���������O
	case	MS_M_MAIN_D_B_A:	//	���̑��őI���������O
	case	MS_M_MAIN_D_B_B:	//	���̑��őI���������O
		draw_Tex( ms_p -> main_x + n_x_pos[1] - 52,n_y_pos[i] - 4,72,24,0,0 );
		break;
	}

	switch( ms_p -> main_grp_type ){
	case	MS_M_MAIN_D_D_A:
	case	MS_M_MAIN_D_B_B:	//	���̑��őI���������O
		if( ms_tp -> p_mouth_count == -1 ){	//	������I��
			if( ms_p -> sub_type == MS_S_DEFAULT ){
				load_TexPal( w_button_bm0_0tlut );	//	�p���b�g���[�h
				load_TexTile_4b( &w_button_bm0_0[32*(ms_cp -> c_count / 8)],8,8,0,0,7,7 );
				draw_Tex( ms_p -> main_x + 150,210,8,8,0,0 );	//	�㕔
			}
		}
		break;
	}

	//	�C���e���V�e�B���g������
	gSPDisplayList(gp++, Intensity_XNZ_Texture_dl);
	gDPSetEnvColor(gp++,64,64,64,255);
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

	//	���O�`��
	switch( ms_p -> main_grp_type )
	{
	case	MS_M_NAME_A:	//	���O�I��	�P�l�p
	case	MS_M_NAME_B:	//	���O�I��	�Q�l�p
	case	MS_M_NAME_C:	//	���O�I��	���̑��p
		for(i = 0;i < 4;i++){
			draw_font_name( ms_p -> main_x + n_x_pos[i] ,n_y_pos[i],evs_mem_data[i].mem_name);
			draw_font_name( ms_p -> main_x + n_x_pos[i + 5] ,n_y_pos[i + 5],evs_mem_data[i + 4].mem_name);
		}
		break;
	case	MS_M_MAIN_D_B:		//	���̑��őI���������O
	case	MS_M_MAIN_D_B_A:	//	���̑��őI���������O
	case	MS_M_MAIN_D_B_B:	//	���̑��őI���������O
		draw_font_name( ms_p -> main_x + n_x_pos[1] - 48,n_y_pos[0],evs_mem_data[ms_cp -> neme_d_pos].mem_name);
		break;
	}

	//	������`��
	if( ms_mp -> mes_win_flg ){
		if( ms_mp -> mes_mes_flg ){
			ms_tp -> p_mouth_count = draw_font_speed_str(30,150,str_p,ms_mp -> mes_mes_len);
		}
	}

	//	�������ݒ�
	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	switch( ms_p -> main_grp_type ){
	case	MS_M_NAME_A:	//	���O�I��	�P�l�p
	case	MS_M_NAME_B:	//	���O�I��	�Q�l�p
	case	MS_M_NAME_C:	//	���O�I��	���̑��p
		for(i = 0;i < 2;i++){
			cursor_g[i][0] 	= 0;							//	���ʂ̃J�[�\��
			cursor_g[i][1] 	= i;							//	�P�oor�Q�o
			cursor_h[i] 	= 24;							//	�J�[�\���̍���
		}
		j 	=	0;	//	�Q�X�g
		cursor_x[0]			= -12;						//	�J�[�\���̂���
		switch( ms_p -> main_grp_type ){
		case	MS_M_NAME_C:	//	���O�I��	���̑��p
			j 	=	1;							//	�I���
		case	MS_M_NAME_A:	//	���O�I��	�P�l�p
			max 				= 0;						//	�ő�\�����̐ݒ�
			if( ms_cp -> name_c_pos[0] == 4){
				//	�J�[�\���ʒu���Q�X�g���͏I���̏ꍇ
				cursor_g[0][0] 	= 1 + j;					//	�Q�X�g�J�[�\�����I���J�[�\��
				cursor_g[0][1] 	= 0;						//	�P�o
			}
			break;
		case	MS_M_NAME_B:	//	���O�I��	�Q�l�p
			max					= 1;						//	�ő�\�����̐ݒ�
			cursor_x[1]			= -4;						//	�J�[�\���̂���i�Q�o�j
			if( ms_cp -> name_c_pos[0] == ms_cp -> name_c_pos[1] ){
				//	�����J�[�\�����d�Ȃ��Ă����ꍇ�i�Q�X�g�̏ꍇ�̂݁j
				cursor_g[0][0] 	= 1;						//	�Q�X�g�J�[�\��
				cursor_g[0][1] 	= 2;						//	�R�o�i�d�Ȃ�p�J�[�\���j
				cursor_g[1][0] 	= 1;						//	�Q�X�g�J�[�\��
				cursor_h[0]		= 16;						//	�J�[�\���̍����i�P�o�j
			}else{
				for(i = 0;i < 2;i++){
					if( ms_cp -> name_c_pos[i] == 4){
						cursor_g[i][0] 	= 1;				//	�Q�X�g�J�[�\��
					}
				}
			}
			break;
		}
		//	�Q�X�g����
		load_TexPal( dm_mode_select_name_gest_bm0_0tlut );
		load_TexTile_4b( dm_ms_gest[j],72,24,0,0,71,23 );
		draw_Tex( ms_p -> main_x + n_x_pos[4] - 4,n_y_pos[4] - 4,72,24,0,0 );

		//	�J�[�\���`��
		for(i = max;i >= 0;i--)
		{
			gDPSetPrimColor(gp++,0,0,ms_cp -> c_alpha[i],ms_cp -> c_alpha[i],ms_cp -> c_alpha[i],255);
			load_TexPal( dm_ms_cursor_pal_all[cursor_g[i][0]][cursor_g[i][1]]);
			load_TexBlock_4b( dm_ms_cousor_all[cursor_g[i][0]][cursor_g[i][1]],80,cursor_h[i] );
			draw_Tex( ms_p -> main_x + n_x_pos[ms_cp -> name_c_pos[i]] + cursor_x[i],
							n_y_pos[ms_cp -> name_c_pos[i]] - 4,80,cursor_h[i],0,0 );
		}

		break;
	case	MS_M_MAIN_D_A:	//	��������I��
		load_TexPal( dm_mode_select_rule_a_bm0_0tlut );	//	�p���b�g���[�h
		gDPSetPrimColor(gp++,0,0,128,128,128,255);
		for( i = 0;i < 6;i++ ){
			//	�I�����Ă��Ȃ����j���[�̕`��
			if( ms_cp -> main_c_pos != i ){
				load_TexTile_4b( dm_ms_manual_menu[i],dm_ms_manual_menu_size[i],16,0,0,dm_ms_manual_menu_size[i] - 1,15 );
				draw_Tex( ms_p -> main_x + dm_ms_manual_menu_pos[i][0],51 + dm_ms_manual_menu_pos[i][1],dm_ms_manual_menu_size[i],16,0,0 );
			}
		}
		gDPSetPrimColor(gp++,0,0,255,255,255,255);
		load_TexTile_4b( dm_ms_manual_menu[ms_cp -> main_c_pos],dm_ms_manual_menu_size[ms_cp -> main_c_pos],16,0,0,dm_ms_manual_menu_size[ms_cp -> main_c_pos] - 1,15 );
		k = dm_ms_manual_menu_size[ms_cp -> main_c_pos] >> 1;
		j = ms_cp -> c_pos_size	>> 1;	//	�����l / 2
		draw_ScaleTex( ms_p -> main_x + dm_ms_manual_menu_pos[ms_cp -> main_c_pos][0] + k - (j + k),
						51 + dm_ms_manual_menu_pos[ms_cp -> main_c_pos][1] + 8 - (8 + j),dm_ms_manual_menu_size[ms_cp -> main_c_pos],16,
							dm_ms_manual_menu_size[ms_cp -> main_c_pos] + ms_cp -> c_pos_size,16 + ms_cp -> c_pos_size,0,0 );

		break;

	}

	//	��ʐ؂�ւ������֐�
	//disp_change_scene();
	HCEffect_Grap(&gp);


	// �ި���ڲؽč쐬�I��, ���̨������̊J�n
	gDPFullSync(gp++);			// �ި���ڲؽčŏI����
	gSPEndDisplayList(gp++);	// �ި���ڲؽďI�[
	osWritebackDCacheAll();		// dinamic�����Ă��ׯ��
	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}
