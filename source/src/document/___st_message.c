// ///////////////////////////////////////////////////////////////////////////
//
//	Story-mode Message Program File
//
//  Author		: Katsuyuki Ohkura
//
//	Last Update	: 1999/12/13
//
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "story_data.h"

#include "dm_code_data.h"
#include "dm_font_main.h"


#define MES_BUF			1500			// �ő僁�b�Z�[�W��
#define LINE_BUF		70				// ���b�Z�[�W���C���o�b�t�@
#define KEY_WAIT_NORMAL	7				// �L�[�E�F�C�g�i�ʏ�j
#define KEY_WAIT_PUSH	0				// �L�[�E�F�C�g�i������j

//	NAB ADD //////////////////////
typedef	struct{
	u8	str_pos[16];					//	�w���W
	u8	*str_grp[16];					//	�����O���t�B�b�N�ւ̃|�C���^
}str_line;

static	str_line	line_mes_tex[LINE_BUF];	//	70��
static	s8	name_line_flg;					//	���O�\���t���O
static	s8	line_mes_max;					//	1�񂠂���̍ő啶�������i�[����ϐ�
s8	disp_face_flg;							//	��O���t�B�b�N�\���p�t���O
//	NAB ADD //////////////////////


u8 face_number = 9;
u8 st_flag = 0;							// �X�g�[���[���[�h�p�t���O
u8 st_page = 0;							// �X�g�[���[�y�[�W
u8 st_scr_stop = 0;						// ������i�s�̃t���O
s16 comple_font_y = 0;					// �t�H���g�x���W�␳�p



static s8 mes_line = -1;

static u16 now_moji_num = 0;					// ���ݕ\�����Ă��镶���ʒu
u8 moji_work[MES_BUF];							// ������ۑ����[�N

static u8  now_line_moji_num = 0;				// ���ݏ������Ă��郉�C���̌��ݕ\�����Ă��镶���ԍ�
static u8  now_line = 0;						// ���ݏ������Ă��郉�C���ԍ�
static u8  disp_mes_line_num = 1;				// �\�����C����
static s16 disp_mes_line[4]  = {-1,0,-1,-1};	// �\�����C���ԍ�

static s8 mes_scr_adj_y = 0;					// �X�N���[�����x���W�����p
static u8 key_wait_mode = 0;					// �L�[���͑҂�
static u16 mes_wait = 0;						// ���b�Z�[�W�\���҂�

static u8 pr = 0,pg = 0,pb = 0,er = 255,eg = 255,eb = 255;	//	�C���e���V�e�B�p�ϐ�

// �e�[�u���ϊ�
u8 change_font( u16 font_num )
{
	u8 num;

	// �����R�[�h���e�[�u���ԍ��ϊ�
	if( font_num == 0x8141 ){				// �A
		num = 0;
	} else if( font_num == 0x8142 ){		// �B
		num = 1;
	} else if( font_num == 0x8148 ){		// �H
		num = 2;
	} else if( font_num == 0x8149 ){		// �I
		num = 3;
	} else if( font_num == 0x8151 ){		// �Q
		num = 4;
	} else if( font_num == 0x815b ){		// �[
		num = 5;
	} else if( font_num == 0x8195 ){		// ���i���g�p�j
		num = 6;
	} else if( font_num == 0x8199 ){		// ���i���g�p�j
		num = 7;
	} else if( font_num == 0x81a0 ){		// ���i���g�p�j
		num = 8;
	} else if( font_num == 0x81a2 ){		// ���i���g�p�j
		num = 9;
	} else if( font_num == 0x8140 ){		// �@(�X�y�[�X)
		num = 10;
	} else if( font_num == 0x8145 ){		// �E
		num = 0xdb;
	} else if( font_num >= 0x824f && font_num <= 0x8258 ){		// �O�`�X
		num = font_num - 0x824f + 11;
	} else if( font_num >= 0x8260 && font_num <= 0x8279 ){		// �`�`�y
		num = font_num - 0x8260 + 21;
	} else if( font_num >= 0x8281 && font_num <= 0x829a ){		// ���`��
		num = font_num - 0x8281 + 47;
	} else if( font_num >= 0x829f && font_num <= 0x82f2 ){		// ���`��
		num = font_num - 0x829f + 73;
	} else if( font_num >= 0x8340 && font_num <= 0x8394 ){		// �A�`��
		num = font_num - 0x8340 + 156;
	} else {
		num = 0xff;
	}
	return num;
}


// �t�H���g������
void init_font( void )
{
	mes_line = -1;
	now_moji_num = 0;

	now_line_moji_num = 0;
	now_line = 0;
	disp_mes_line_num = 1;
	disp_mes_line[0] = -1;	//	��\���̐ݒ�
	disp_mes_line[1] =  0;	//	�\�����̐ݒ�
	disp_mes_line[2] = -1;	//	��\���̐ݒ�
	disp_mes_line[3] = -1;	//	��\���̐ݒ�

	comple_font_y = 0;		//	�x���W�␳�l�̃N���A
	mes_scr_adj_y = 0;		//	�X�N���[�����̂x���W�␳�l�̃N���A
	key_wait_mode = 0;		//	�L�[�����ʏ폈���ɐݒ�
	mes_wait = 0;			//	���b�Z�[�W�i�s�J�E���^�̃N���A

	st_page = 0;			//	�\���w�i�ԍ��̃N���A
	st_flag = 0;			//	�^�C�������ԍ��̃N���A

//	NAB ADD //////////////////////
	line_mes_max  = 16;		//	�P�s�̕\��������
	disp_face_flg = 0;		//	��\���t���O�̃N���A
	name_line_flg = 0;		//	���O�\���t���O�̃N���A
	bzero(&line_mes_tex[0],sizeof(str_line) * LINE_BUF);	//	�z��̈�̂O�N���A
//	NAB ADD //////////////////////

}

//	�C���e���V�e�B�Ŏg���ϐ��̐ݒ�
void set_intencity_rgb( u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2 )
{
	pr = r1;	//	�v���~�e�B�u�J���[( �� )
	pg = g1;	//	�v���~�e�B�u�J���[( �� )
	pb = b1;	//	�v���~�e�B�u�J���[( �� )
	er = r2;	//	���J���[( �� )
	eg = g2;	//	���J���[( �� )
	eb = b2;	//	���J���[( �� )
}

//	�C���e���V�e�B�g�p�̐ݒ�
void set_intencity_mode( void )
{
	gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);	//	�C���e���V�e�B�g�p�̂��߂̐ݒ�
	gDPSetPrimColor( gp++, 0,0, pr*(100-evs_fadecol)/100,pg*(100-evs_fadecol)/100,pb*(100-evs_fadecol)/100,255 );	//	�v���~�e�B�u�J���[�̐ݒ�
	gDPSetEnvColor( gp++, er*(100-evs_fadecol)/100,eg*(100-evs_fadecol)/100,eb*(100-evs_fadecol)/100,255 );			//	���J���[�̐ݒ�
}

// �t�H���g�f�[�^�쐬(�����񐧌�R�[�h�𔲂��āA������̃O���t�B�b�N������ݒ肷��)
void font_make( u8 *moji )
{
	int n = 0,i,j;
	u16 num,font_num,font_next;
	u8 cnt = 0;
	u8 err_work[3];

	if( mes_line == -1 ){
		strcpy( moji_work,moji );				// �ŏ�
		mes_line ++;
	} else {
		strcat( moji_work,moji );				// �ǉ�
	}

	while( moji[n] != '\0' ){
		font_num = (moji[n] << 8) | moji[n+1];
		font_next = (moji[n+2] << 8) | moji[n+3];

		// �����R�[�h���e�[�u���ԍ��ϊ�
		num = change_font( font_num );
		switch( font_num ){
		case	0x89fc:			// ��
		case	0x8fc1:			// ��
			cnt = 0;
			mes_line ++;
			break;
		case	0x8ae7:			//	��
			line_mes_max = 12;
		case	0x8af8:			//	��
		case	0x96bc:			//	��
			n += 2;
			break;
		case	0x91bd:			//	��
			line_mes_max = 16;
			break;
		case	0x8e6e:			//	�n
		case	0x8e7e:			//	�~
		case	0x8f49:			//	�I
		case	0x95c5:			//	��
			break;
		case	0x91d2:			//	��
			n += 4;
			break;
		default:				//	���̑�
			if( num == 255 ){	//	��`����Ă��Ȃ������̏ꍇ
				err_work[0] = moji[n];
				err_work[1] = moji[n+1];
				err_work[2] = '\0';
#ifdef	DM_DEBUG_FLG
				osSyncPrintf("[%s]�͒�`����Ă��܂���I\n",err_work);
#endif
			}
			if( cnt == 0 ){	//	���s����A��̍ŏ��������ꍇ
				for(i = 0;i < 16;i++){
					line_mes_tex[mes_line].str_pos[cnt] = cnt << 4;				//	X���W( 16�h�b�g�������
					line_mes_tex[mes_line].str_grp[i] = &dm_font_16[0x0a][0];	//	�����O���t�B�b�N
				}
			}
			line_mes_tex[mes_line].str_pos[cnt] = cnt << 4;						//	X���W( 16�h�b�g�������
			line_mes_tex[mes_line].str_grp[cnt] = &dm_font_16[num][0];			//	�����O���t�B�b�N�̐ݒ�
			// �������s����
			if( cnt < line_mes_max ){		//	�ő�\�����ȉ��̏ꍇ
				cnt ++;						//	�P�����i�߂�
			} else {						//	�ő�\�����ȏ�̏ꍇ
				if( font_next != 0x89fc ){	//	���̕����R�[�h�����s�łȂ��ꍇ
					cnt = 0;				//	�������ŏ��ɖ߂�( �P�s�̐擪 )
					mes_line ++;			//	���s
				}
			}
			break;
		}
		n += 2;	//	�|�C���^�̉��Z( n���̂̓|�C���^�ł͂Ȃ� )
	}

}


// ���b�Z�[�W�L�[���͑҂��̕`��
void wait_button( void )
{
	static u8 cnt = 0;

	if( key_wait_mode ){	//	�L�[���͑҂��ɂȂ��Ă����ꍇ
		disp_tex_4bt( &w_button_bm0_0[32*(cnt/8)],w_button_bm0_0tlut,8,8,288,216,SET_PTD );	//�{�^�����͂�`�悷��
		cnt = ( cnt + 1 ) % 24;	//	�A�j���[�V�����������鏈��
	}
}


// ���b�Z�[�W����
s8 calc_message( void )
{
	int i;
	u16 font_num,font_next;

	if( st_scr_stop ){	//	������i�s�t���O�� �P(��~)�������� -1��Ԃ�(�������s��Ȃ�)
		return -1;
	}

	// ������I���`�F�b�N
	if( moji_work[now_moji_num] != '\0' ){
		font_num  = (moji_work[now_moji_num  ] << 8) | moji_work[now_moji_num+1];	//	���݂̕����R�[�h(�t�H���g�ԍ�)�̎擾
		font_next = (moji_work[now_moji_num+2] << 8) | moji_work[now_moji_num+3];	//	���̕����R�[�h(�t�H���g�ԍ�)�̎擾
		switch( font_num ){
		case	0x8ae7:			//	��
			face_number = (moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0');			//	�\����ԍ��̎擾
			disp_face_flg = 1;													//	��\���t���O�𗧂Ă�
			line_mes_max = 12;													//	�P�s�̍ő�\�����������P�Q�ɐݒ�
			now_moji_num += 4;													//	������|�C���^���S�o�C�g�i�߂�( now_moji_num���̂̓|�C���^�ł͂Ȃ� )
			break;
		case	0x8af8:			//	��											//	�^�C������t���O�̐ݒ�( �^�C������Ɉړ�������ݒ�����Ă��� )
			st_flag |=	(moji_work[now_moji_num+2]-'0') * 10 +
						(moji_work[now_moji_num+3]-'0');
			now_moji_num += 4;													//	������|�C���^���S�o�C�g�i�߂�( now_moji_num���̂̓|�C���^�ł͂Ȃ� )
			return -1;															//	���̏����̎��͕������i�܂��Ȃ����߂ɂ����ŏ����𒆎~����B
		case	0x96bc:			//	��
			now_moji_num += ((moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0')) * 2;	//	���O�̕\���������������������i�߂�
			now_moji_num += 4;													//	������|�C���^���S�o�C�g�i�߂�
			name_line_flg = 1;													//	���O�\���t���O�𗧂Ă�
			break;
		case	0x91bd:			//	��
			disp_face_flg = 0;													//	��\���t���O���N���A
			line_mes_max = 16;													//	�P�s�̍ő�\�����������P�U�ɐݒ�
			now_moji_num += 2;													//	������|�C���^���Q�o�C�g�i�߂�
			break;
		case	0x8e6e:			//	�n
			st_page = 255;														//	�^�C������t���O�̐ݒ�( �^�C������Ԃł���������A���C�����[�v�𔲂���悤�ɂȂ� )
			now_moji_num += 2;													//	������|�C���^���Q�o�C�g�i�߂�
			break;
		case	0x8e7e:			//	�~
			key_wait_mode = 1;													//	�L�[���͑҂��t���O�𗧂Ă�
			if( joyupd[main_joy[0]] & CONT_A ){									//	�����A�`�{�^����������Ă����ꍇ
				key_wait_mode = 0;												//	�L�[���͑҂��t���O���N���A
				now_moji_num += 2;												//	������|�C���^���Q�o�C�g�i�߂�
			}
			return -1;
		case	0x8fc1:			//	��
			now_line_moji_num = 0;												//	������|�C���^���O�ɐݒ肷��( �P�s�̐擪�ɂ��� )
			now_line ++;														//	���ݕ\�����̃��C���ԍ���i�߂�

			disp_mes_line[0] = -1;												//	��\���̐ݒ�
			disp_mes_line[1] =  now_line;										//	�\�����̐ݒ�
			disp_mes_line[2] = -1;												//	��\���̐ݒ�
			disp_mes_line[3] = -1;												//	��\���̐ݒ�
			disp_mes_line_num = 1;												//	�\���s�����P�ɐݒ�
			name_line_flg = 0;													//	���O�\���t���O�̃N���A

			now_moji_num += 2;													//	������|�C���^���Q�o�C�g�i�߂�
			break;
		case	0x91d2:			// ��(���ݖ��g�p)
			mes_wait =	(moji_work[now_moji_num+2]-'0') * 1000 +
						(moji_work[now_moji_num+3]-'0') * 100 +
						(moji_work[now_moji_num+4]-'0') * 10 +
						(moji_work[now_moji_num+5]-'0');
			now_moji_num += 6;
			break;
		case	0x95c5:			//	�v
			st_page ++;															//	���̔w�i�ԍ��̐ݒ�
			now_moji_num += 2;													//	������|�C���^���Q�o�C�g�i�߂�
			break;
		default:	//	���̑�
			mes_scr_adj_y = ( mes_scr_adj_y - 4 >= 0 )?mes_scr_adj_y - 4:0;
			if( !mes_wait ){
				// �������s �� �I�� �� �������s
				if( font_num == 0x89fc || (font_num == 0x8f49 && moji_work[now_moji_num+2] != '\0') || now_line_moji_num >= line_mes_max ){
					// �P�R�����ڂ̌�ɉ��s���������疳������ׂɂQbit��ɑ���
					if( now_line_moji_num >= line_mes_max  && font_next == 0x89fc ){
						now_moji_num += 2;
					}
					// �P�R�����ڂ̌�Ɏ~�߂����������疳������ׂɂQbit��ɑ���
					if( now_line_moji_num >= line_mes_max  && font_next == 0x8e7e ){
						now_line_moji_num ++;
					}else{
						now_line_moji_num = 0;	//	������|�C���^���O�ɐݒ肷��( �P�s�̐擪�ɂ��� )
						now_line ++;			//	���ݏ������Ă��郉�C���ԍ��̕ύX
						// �\�����C���X�V
						if( disp_mes_line_num >= 3 ){							//	3�s�ȏ�\������Ă����ꍇ
							if( !name_line_flg ){								//	���O�\���t���O�������Ă��Ȃ��ꍇ
								for( i = 0; i < 3; i ++ ){						//	�\���s���P�s�Â��炷
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}else{												//	���O�\���t���O�������Ă����ꍇ
								for( i = 2; i < 3; i ++ ){						//	����s�����\���s�����炷
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}
							disp_mes_line[3] = now_line;						//	�\���s�ɕ\������s�ԍ��̐ݒ�
							mes_scr_adj_y = 16;									//	��������X�N���[��������J�E���^�̐ݒ�
						} else {
							disp_mes_line_num ++;								//	���̍s��\������悤�ɐݒ�
							disp_mes_line[disp_mes_line_num] = now_line;		//	�\���s�ɕ\������s�ԍ��̐ݒ�
						}
					}
				} else {						// ��������
					now_line_moji_num ++;
				}
				now_moji_num += 2;

				// ������
				if( joynew[main_joy[0]] & CONT_A ){	//	�`�{�^����������Ă����ꍇ
					mes_wait = KEY_WAIT_PUSH;		//	�E�F�C�g���Ԃ̐ݒ�KEY_WAIT_PUSH( 0 )
				} else {
					mes_wait = KEY_WAIT_NORMAL;		//	�E�F�C�g���Ԃ̐ݒ�KEY_WAIT_NORMAL( 7 )
				}

			} else {
				mes_wait --;
			}
			break;
		}
		return	1;	//	������i�s��
	}else{
		return	-1;	//	�������~��
	}
}

#ifdef	DM_DEBUG_FLG
// ���b�Z�[�W����
s8 calc_skip_message( void )
{
	int i;
	u16 font_num,font_next;

	while(1){
	// ������I���`�F�b�N
	if( moji_work[now_moji_num] != '\0' ){
		font_num  = (moji_work[now_moji_num  ] << 8) | moji_work[now_moji_num+1];
		font_next = (moji_work[now_moji_num+2] << 8) | moji_work[now_moji_num+3];
		switch( font_num ){
		case	0x8ae7:			//	��
			line_mes_max = 12;
			now_moji_num += 4;
			break;
		case	0x8af8:			//	��
			now_moji_num += 4;
			return -1;
		case	0x96bc:			//	��
			now_moji_num += ((moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0')) * 2;
			now_moji_num += 4;
			break;
		case	0x91bd:			//	��
			disp_face_flg = 0;
			line_mes_max = 16;
			now_moji_num += 2;
			break;
		case	0x8e6e:			//	�n
			st_page = 255;
			now_moji_num += 2;
			return	-1;
		case	0x8e7e:			//	�~
			now_moji_num += 2;
			break;
		case	0x8fc1:			//	��
			now_line_moji_num = 0;
			now_line ++;

			disp_mes_line[0] = -1;
			disp_mes_line[1] =  now_line;
			disp_mes_line[2] = -1;
			disp_mes_line[3] = -1;
			disp_mes_line_num = 1;
			name_line_flg = 0;

			now_moji_num += 2;
			break;
		case	0x91d2:			// ��
			now_moji_num += 6;
			break;
		case	0x95c5:			//	�v
			st_page ++;
			now_moji_num += 2;
			return	-1;
		default:	//	���̑�
			mes_scr_adj_y = ( mes_scr_adj_y - 4 >= 0 )?mes_scr_adj_y - 4:0;
				// �������s �� �I�� �� �������s
				if( font_num == 0x89fc || (font_num == 0x8f49 && moji_work[now_moji_num+2] != '\0') || now_line_moji_num >= line_mes_max ){
					// �P�R�����ڂ̌�ɉ��s���������疳������ׂɂQbit��ɑ���
					if( now_line_moji_num >= line_mes_max  && font_next == 0x89fc ){
						now_moji_num += 2;
					}
					// �P�R�����ڂ̌�Ɏ~�߂����������疳������ׂɂQbit��ɑ���
					if( now_line_moji_num >= line_mes_max  && font_next == 0x8e7e ){
						now_line_moji_num ++;
					}else{
						now_line_moji_num = 0;
						now_line ++;
						// �\�����C���X�V
						if( disp_mes_line_num >= 3 ){
							if( !name_line_flg ){
								for( i = 0; i < 3; i ++ ){
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}else{
								for( i = 2; i < 3; i ++ ){
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}
							disp_mes_line[3] = now_line;
							mes_scr_adj_y = 16;
						} else {
							disp_mes_line_num ++;
							disp_mes_line[disp_mes_line_num] = now_line;
						}
					}
				} else {						// ��������
					now_line_moji_num ++;
				}
				now_moji_num += 2;
			break;
		}
	}else{
		return	-1;
	}
	}
}
#endif


// ���b�Z�[�W�\��
void disp_message( void )
{
	int i,j,st;
	s8	x_p;
	s8	adj_y[4] = {0,0,0,0};

	wait_button();							//	�L�[�҂���Ԃ̎��̃{�^���`��

	set_intencity_mode();					//	�C���e���V�e�B�g�p�̂��߂̐ݒ�

	if( mes_scr_adj_y <= 0 ){				//	�����񂪃X�N���[�����łȂ��ꍇ
		st = 1;								//	�\���J�n�s���P�ɂ���
	} else {								//	�����񂪃X�N���[�����̏ꍇ
		if( name_line_flg ){				//	���O�\���t���O�������Ă����ꍇ
			st = 1;							//	�\���J�n�s���P�ɂ���
			for(i = 2;i < 3;i++){
				adj_y[i] = mes_scr_adj_y;	//	�x���W�ړ��l�̐ݒ�
			}
		}else{
			st = 0;							//	�\���J�n�s���O�ɂ���
			for(i = 0;i < 3;i++){
				adj_y[i] = mes_scr_adj_y;	//	�x���W�ړ��l�̐ݒ�
			}
		}
	}

	//	������`��J�n�w���W
	if( disp_face_flg ){					//	��\���t���O�������Ă����ꍇ
		x_p = 80;
	}else{									//	��\���t���O�������Ă��Ȃ��ꍇ
		x_p = 32;
	}



	// �����ς݃��C��( �P�s�S���\������Ă��� )
	for( i = st; i < disp_mes_line_num; i ++ ){
		if( disp_mes_line[i] != -1 ){		//	��\����ԂłȂ��ꍇ
			for(j = 0;j < 16;j++){
				load_TexBlock_i( line_mes_tex[disp_mes_line[i]].str_grp[j],16,16 );											//	�t�H���g�Ǎ���
				draw_Tex(x_p + line_mes_tex[disp_mes_line[i]].str_pos[j],176+i*16-16+adj_y[i]+comple_font_y,16,16,0,0 );	//	�t�H���g�`��
			}
		}
	}
	// ���������C��
	for(j = 0;j < now_line_moji_num;j++){
		load_TexBlock_i( line_mes_tex[disp_mes_line[disp_mes_line_num]].str_grp[j],16,16 );											//	�t�H���g�Ǎ���
		draw_Tex(x_p + line_mes_tex[disp_mes_line[disp_mes_line_num]].str_pos[j],176+i*16-16+adj_y[2]+comple_font_y,16,16,0,0 );	//	�t�H���g�`��
	}
}
