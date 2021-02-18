/*--------------------------------------
	filename	:	dm_logo_action_main.c

	create		:	1999/07/05
	modify		:	1999/12/13

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define F2DEX_GBI
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

#include	"dm_nab_include.h"

/*--------------------------------------
	�ϐ��f�[�^
--------------------------------------*/

//	2D�`��ݒ�DL
static	Gfx	init_2d_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPSetColorDither( G_CD_BAYER ),
	gsDPSetCombineKey( G_CK_NONE ),
	gsDPSetAlphaDither( G_AD_NOTPATTERN ),
	gsDPSetAlphaCompare( G_AC_NONE ),
	gsDPSetTextureLUT(G_TT_RGBA16),
	gsDPSetTextureLOD( G_TL_TILE ),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert( G_TC_FILTCONV ),
	gsDPSetRenderMode(  G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
	gsDPSetCombineMode(G_CC_MODULATEIA_PRIM,G_CC_MODULATEIA_PRIM),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetEnvColor( 255,255,255,255 ),
	gsSPObjRenderMode( 0x08 ),
	gsSPEndDisplayList(),
};

//	2D�`��ݒ�DL(�C���e���V�e�B)
static	Gfx	init_ia_2d_dl[] = {
	gsDPPipeSync(),
	gsDPSetTextureLUT ( G_TT_NONE ),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetRenderMode(G_RM_XLU_SPRITE, G_RM_XLU_SPRITE2),
	gsDPSetCombineLERP(  PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0,0,ENVIRONMENT,0
						,PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0,0,ENVIRONMENT,0),
	gsDPSetPrimColor(0,0,255,255,255,255),
	gsDPSetEnvColor(255,255,255,255),
	gsSPEndDisplayList(),

};


/*---------- GAME OVER ----------*/
#include	"game_over_action.dat"									//	GAME OVER �̏����f�[�^�z��
static	s8	game_over_action_count_flg;								//	GAME OVER �̓�������ϐ�
static	s8	game_over_action_count[2];								//	GAME OVER �̓�������ϐ�
static	s8	game_over_action_flg;									//	GAME OVER �̓�������ϐ�
static	s8	game_over_size[2][2];									//	GAME OVER �̏c���̃T�C�Y
static	s8	game_over_mul[2];										//	GAME OVER �̊g�k�̑傫���p�ϐ�
static	s16	game_over_position[2];									//	GAME OVER �̍��W
static	s16	game_over_sin;											//	GAME OVER �̊g�k�v�Z�p�ϐ�
/*---------- GAME OVER ----------*/

/*---------- TRY NEXT STAGE ----------*/
static	s8	try_out_count;											//	TRY NEXT STAGE �̓�������p�ϐ�
static	s8	try_action_flg;											//	TRY NEXT STAGE �̓�������p�ϐ�
static	u8	try_no[] = {7,8,0,3,2,1,4,2,0,6,8,6,10,5,2,9,8 };		//	TRY NEXT STAGE �̕`�悷��O���t�B�b�N�̔ԍ�
static	s16	try_position[17][4];									//	TRY NEXT STAGE �̕`�悷��O���t�B�b�N�̍��W
static	s16	init_try_position[17][3] = {							//	TRY NEXT STAGE �̕`�悷��O���t�B�b�N�̏��������W
	{36,-30,86},
	{51,-40,88},
	{60,-26,88},
	{70,-35,87},
	{83,-21,87},
	{35,-30,108},
	{49,-40,109},
	{59,-26,108},
	{71,-35,109},
	{83,-21,108},
	{47,-30,148},
	{60,-40,147},
	{73,-26,147},
	{39,-35,169},
	{54,-21,168},
	{67,-30,168},
	{83,-40,169}
};

static	u8	*try_bm[] = {											//	TRY NEXT STAGE �̕`�悷��O���t�B�b�N�f�[�^
	test_a_bm0_0,
	test_c_bm0_0,
	test_e_bm0_0,
	test_g_bm0_0,
	test_l_bm0_0,
	test_n_bm0_0,
	test_r_bm0_0,
	test_s_bm0_0,
	test_t_bm0_0,
	test_x_bm0_0,
	test_y_bm0_0
};

static	u16	*try_bm_tlut[] = {										//	TRY NEXT STAGE �̕`�悷��p���b�g�f�[�^
	test_a_bm0_0tlut,
	test_c_bm0_0tlut,
	test_e_bm0_0tlut,
	test_g_bm0_0tlut,
	test_l_bm0_0tlut,
	test_n_bm0_0tlut,
	test_r_bm0_0tlut,
	test_s_bm0_0tlut,
	test_t_bm0_0tlut,
	test_x_bm0_0tlut,
	test_y_bm0_0tlut
};
/*---------- TRY NEXT STAGE ----------*/

/*---------- WIN ----------*/
#include	"win2_action.dat"										//	WIN �̏����f�[�^�z��
static	uObjMtx			s2d_win_mtx[4];								//	WIN	�p�}�g���N�X�ϐ�
static	uObjSprite		s2d_win_sp;									//	WIN �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_win_tex;								//	WIN	�p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_win_pal;								//	WIN	�p�p���b�g���[�h�ϐ�
static	uObjSprite		s2d_win_ia_sp;								//	WIN	�p�X�v���C�g�ݒ�ϐ�(�C���e���V�e�B)
static	uObjTxtr		s2d_win_ia_tex;								//	WIN	�p�e�N�X�`�����[�h�ϐ�(�C���e���V�e�B)
static	u8				win_mode[4];									//	WIN �p����ϐ�
static	u8				win_count[4];								//	WIN �p����ϐ�
static	s8				win_flg[4];									//	WIN �p����ϐ�
/*---------- WIN ----------*/

/*---------- LOSE ----------*/
#include	"lose2_action.dat"										//	LOSE �̏����f�[�^�z��
static	uObjMtx			s2d_lose_mtx[4];							//	LOSE �p�}�g���N�X�ϐ�
static	uObjSprite		s2d_lose_sp;								//	LOSE �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_lose_tex;								//	LOSE �p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_lose_pal;								//	LOSE �p�p���b�g���[�h�ϐ�
static	uObjSprite		s2d_lose_ia_sp;								//	LOSE �p�X�v���C�g�ݒ�ϐ�(�C���e���V�e�B)
static	uObjTxtr		s2d_lose_ia_tex;							//	LOSE �p�e�N�X�`�����[�h�ϐ�(�C���e���V�e�B)
static	u8				lose_count[4];								//	LOSE �p����ϐ�
static	s8				lose_flg[4];								//	LOSE �p����ϐ�
/*---------- LOSE ----------*/

/*---------- DRAW ----------*/
#include	"draw3_action.dat"										//	DRAW �̏����f�[�^�z��
static	uObjMtx			s2d_draw_mtx[4];							//	DRAW �p�}�g���N�X�ϐ�
static	uObjSprite		s2d_draw_sp;								//	DRAW �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_draw_tex;								//	DRAW �p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_draw_pal;								//	DRAW �p�p���b�g���[�h�ϐ�
static	uObjSprite		s2d_draw_ia_sp;								//	DRAW �p�X�v���C�g�ݒ�ϐ�(�C���e���V�e�B)
static	uObjTxtr		s2d_draw_ia_tex;							//	DRAW �p�e�N�X�`�����[�h�ϐ�(�C���e���V�e�B)
static	u8				draw_count[4];								//	DRAW �p����ϐ�
static	s8				draw_flg[4];								//	DRAW �p����ϐ�
/*---------- DRAW ----------*/

/*---------- �A�� ----------*/
static	uObjSprite		s2d_chaine_sp[4];							//	CHAINED �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_chaine_tex;								//	CHAINED �p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_chaine_pal;								//	CHAINED �p�p���b�g���[�h�ϐ�

static	uObjSprite		s2d_chaine_num_sp;							//	�A���� �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_chaine_num_pal;							//	�A���� �p�p���b�g���[�h�ϐ�
static	uObjTxtr		s2d_chaine_num_tex[4][4];					//	�A���� �p�e�N�X�`�����[�h�ϐ�
static	uObjSubMtx		s2d_chaine_num_mtx[4][4];					//	�A���� �p�}�g���N�X�ϐ�

static	uObjSprite		s2d_chaine_mes_sp[4];						//	�A�����b�Z�[�W	�p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_chaine_mes_tex[4];						//	�A�����b�Z�[�W	�p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_chaine_mes_pal[4];						//	�A�����b�Z�[�W	�p�p���b�g���[�h�ϐ�

static	uObjSprite		s2d_chaine_cap_sp;							//	�A���J�v�Z��	�p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_chaine_cap_tex[3];						//	�A���J�v�Z��	�p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_chaine_cap_pal[3];						//	�A���J�v�Z��	�p�p���b�g���[�h�ϐ�
static	uObjMtx			s2d_chaine_cap_mtx[4][3];					//	�A���J�v�Z��	�p�}�g���N�X�ϐ�

static	uObjSprite		s2d_chaine_player_sp;						//	�A���v���C���[(1P.2P.3P,4P)	�p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_chaine_player_tex[4][3];				//	�A���v���C���[(1P.2P.3P,4P)	�p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_chaine_player_pal[4][3];				//	�A���v���C���[(1P.2P.3P,4P)	�p�p���b�g���[�h�ϐ�
static	uObjSubMtx		s2d_chaine_player_mtx[4][3];				//	�A���v���C���[(1P.2P.3P,4P)	�p�}�g���N�X�ϐ�

static	u8	test_chaine_mode[4];									//	�A�����\����������ϐ�
static	u8	test_chaine_num[4];										//	���݂̘A�����ێ��ϐ�
static	s8	test_chaine_max[4][4];									//	�U�����萔�Ƒ���ԍ��ϐ�
static	u8	test_chaine_save[4];									//	�O�̘A�����ێ��ϐ�
static	u8	test_chaine_scis_w[4];									//	�`��͈�(����)�ݒ�p�ϐ�
static	s8	test_chaine_num_w_size[4];								//	�A�����p	����
static	s8	test_chaine_num_h_size[4];								//	�A�����p	�c��
static	s16	test_chaine_alpha[4];									//	�A�����p	�����x
static	s16	test_chaine_center[4];									//	�`�撆�S���W
static	s16	test_chaine_scis_x[4];									//	�`��͈͎w��̊J�n���W�ݒ�p�ϐ�
static	s16	test_chaine_cap_r[4][3];								//	�J�v�Z����]�p�p�x�ێ��ϐ�

/*---------- �A�� ----------*/

/*---------- PAUSE ----------*/

#include	"pause_action.dat"										//	PAUSE �̏����f�[�^�z��
static	uObjSprite		s2d_pause_sp;								//	PAUSE �p�X�v���C�g�ݒ�ϐ�
static	uObjTxtr		s2d_pause_tex;								//	PAUSE �p�e�N�X�`�����[�h�ϐ�
static	uObjTxtr		s2d_pause_pal;								//	PAUSE �p�p���b�g���[�h�ϐ�
static	uObjSubMtx		s2d_pause_mtx[4];							//	PAUSE �p�}�g���N�X�ϐ�
static	u8				pause_count[4];								//	PAUSE �p����ϐ�
static	u8				pause_anime_count[4];						//	PAUSE �p����ϐ�
static	s8				pause_mode[4];								//	PAUSE �p����ϐ�

/*---------- PAUSE ----------*/

/*---------- 4PLAY SHOCK ----------*/
static	s8	shock_mode[4];											//	4PLAY �V���b�N�p��������ϐ�
static	s8	shock_size[4][2];										//	4PLAY �V���b�N�p�\������傫��
static	s16	shock_position[4][2];									//	4PLAY �V���b�N�p���W
static	s16	shock_alpha[4];											//	4PLAY �V���b�N�p�����x
/*---------- 4PLAY SHOCK ----------*/

/*---------- 4PLAY HUMMING ----------*/
static	s8	humming_mode[4];										//	4PLAY �n�~���O�p��������ϐ�
static	s8	humming_size[4][2];										//	4PLAY �n�~���O�p�\������傫��
static	s16	humming_sin[4];											//	4PLAY �n�~���O�p�p�x�ێ��ϐ�(�h���\�����邽��)
static	s16	humming_position[4][3];									//	4PLAY �n�~���O�p���W
static	s16	humming_alpha[4];										//	4PLAY �n�~���O�p�����x
/*---------- 4PLAY HUMMING ----------*/


/*---------- �A�����̂Q ----------*/
	effect_bubble	dm_bubble[4];									//	�A����̏���

static	u16	*bubble_pal[] = {										//	�o�u���p�p���b�g�f�[�^
	dm_bubble_red_bm0_0tlut,										//	�o�u����
	dm_bubble_blue_bm0_0tlut,										//	�o�u����
	dm_bubble_yellow_bm0_0tlut,										//	�o�u����
	dm_bubble_green_bm0_0tlut										//	�o�u����
};

static	u8	*bubble_graphic[] = {									//	�o�u���p�O���t�B�b�N�f�[�^
	dm_bubble_b_bm0_0,												//	�o�u����
	dm_bubble_m_bm0_0,												//	�o�u����
	dm_bubble_s_bm0_0,												//	�o�u����
	dm_bubble_ss_bm0_0												//	�o�u����
};

/*---------- �A�����̂Q ----------*/

/*---------- PUSH ANY KEY ----------*/
static	s16	push_sin;												//	�㉺�̗h������邽�߂̕ϐ�
static	s8	push_pos_y;												//	�x���W
/*---------- PUSH ANY KEY ----------*/

/*--------------------------------------
	S2DEX �p�X�v���C�g�ݒ�֐�
--------------------------------------*/
void	set_sprite(uObjSprite *sp,s16 x_p,s16 y_p,s16 size_w,s16 size_h,u8 im_fmt,u8 im_siz,s16 wide )
{
	sp->s.objX = x_p << 2;
	sp->s.objY = y_p << 2;
	sp->s.imageW = size_w << 5;
	sp->s.imageH = size_h << 5;
	sp->s.scaleW = sp->s.scaleH = 1<<10;
	sp->s.paddingX = sp->s.paddingY = sp->s.imageFlags = 0;
	sp->s.imageStride = GS_PIX2TMEM(wide,im_siz);
	sp->s.imageAdrs = GS_PIX2TMEM(0,im_siz);
	sp->s.imageFmt = im_fmt;
	sp->s.imageSiz = im_siz;
	sp->s.imagePal = 0;
}

/*--------------------------------------
	S2DEX �p�e�N�X�`�����[�h�ݒ�֐�
--------------------------------------*/
void	load_set_sprite(uObjTxtr *tex,u8 type,u8 *image,u8 im_fmt,u8 im_siz,s16	image_w,s16 image_h)
{
	tex->block.image	=	(u64 *)image;
	tex->block.tmem		=	GS_PIX2TMEM(0,im_siz);
	tex->block.sid		=	0;
	tex->block.mask		=	-1;
   	tex->block.flag		=	(u32)image;

	switch(type)
	{
	case	0:	//	block
		tex->block.type		=	G_OBJLT_TXTRBLOCK;
		tex->block.tsize	=	GS_TB_TSIZE(image_w * image_h,im_siz);
    	tex->block.tline	=	GS_TB_TLINE(image_w,im_siz);
		break;
	case	1:	//	tile
		tex->tile.type		=	G_OBJLT_TXTRTILE;
		tex->tile.twidth	=	GS_TT_TWIDTH(image_w,im_siz);
    	tex->tile.theight	=	GS_TT_THEIGHT(image_h,im_siz);
		break;
	}
}

/*--------------------------------------
	S2DEX �p�p���b�g���[�h�ݒ�֐�
--------------------------------------*/
void	load_set_pal(uObjTxtr *tex,u16 *image,s16 col)
{
	tex->tlut.type		=	G_OBJLT_TLUT;
	tex->tlut.image		=	(u64 *)image;
	tex->tlut.phead		=	GS_PAL_HEAD(0);
	tex->tlut.pnum		=	GS_PAL_NUM(col);
	tex->tlut.zero		=	tex->tlut.sid	=	0;
	tex->tlut.mask		=	-1;
	tex->tlut.flag		=	(u32)image;
}

/*--------------------------------------
	S2DEX �p��]�\�}�g���N�X�ݒ�֐�
--------------------------------------*/
void	set_sp_mtx(uObjMtx *mtx,s32 a,s32 b,s32 c,s32 d,s16 x,s16 y,s16 sc_w,s16 sc_h,s16 size_w,s16 size_h)
{

	mtx->m.A = ((a << 1) * sc_w ) / size_w;
	mtx->m.B = ((b << 1) * sc_h ) / size_h;
	mtx->m.C = ((c << 1) * sc_w ) / size_w;
	mtx->m.D = ((d << 1) * sc_h ) / size_h;

	mtx->m.X = x << 2;
	mtx->m.Y = y << 2;
	mtx->m.BaseScaleX = 1 << 10;
	mtx->m.BaseScaleY = 1 << 10;
}

/*--------------------------------------
	S2DEX �p��]�s�\�}�g���N�X�ݒ�֐�
--------------------------------------*/
void	set_sp_sub_mtx(uObjSubMtx *mtx,s16 x,s16 y,s16 sc_w,s16 sc_h,s16 size_w,s16 size_h)
{
	mtx->m.X = x << 2;
	mtx->m.Y = y << 2;
	mtx->m.BaseScaleX = (size_w << 10) / sc_w;
	mtx->m.BaseScaleY = (size_h << 10) / sc_h;
}



/*--------------------------------------
	TRY NEXT STAGE �������֐�
--------------------------------------*/
void	init_try_next_stage(void)
{
	s8	i,j;

	try_out_count = 0;
	for( i = 0;i < 17;i++ ){
		try_position[i][3] = 0;
		for( j = 0;j < 3;j++ ){
			try_position[i][j] = init_try_position[i][j];	//	TRY NEXT STAGE �̍��W�̏�����
		}
	}
}
/*--------------------------------------
	TRY NEXT STAGE �������C���֐�
--------------------------------------*/
s8	try_next_stage_main(void)
{
	s8	i,j;

	try_out_count ++;
	for( i = j = 0;i < 17;i++ ){
		if( try_position[i][3] ){
			//	�㉺�ɂ��鏈��
			j++;
			try_position[i][1] = try_position[i][2] + (5 * sinf(DEGREE(try_position[i][3])));
			try_position[i][3] += 6;
			if( try_position[i][3] >= 360 )
				try_position[i][3] = 1;
		}else{
			//	�w����W�܂ňړ�
			try_position[i][1] += 4;
			if( try_position[i][1] >= try_position[i][2] ){
				try_position[i][1] = try_position[i][2];
				try_position[i][3] = 1;
			}
		}
	}
	if( try_out_count >= dm_demo_count ){
		try_out_count = dm_demo_count;
		return	1;	//	�L�[��t�J�n
	}else	{
		return	-1;	//	�܂�������
	}
}
/*--------------------------------------
	TRY NEXT STAGE �`�惁�C���֐�
--------------------------------------*/
void	try_next_stage_draw(s8	flg)
{
	s16	i;
	s16	puls;
	s16	x_pos;

	if( flg == 0 ){	//	1P�ʒu(���ݖ��g�p)
		puls = 0;
	}else	if( flg == 1 ){	//	2P�ʒu(���ݖ��g�p)
		puls = 184;
	}else	if( flg == 2 ){	//	���S�ʒu
		puls = 90;
	}

	//	�`��( �O���t�B�b�N�̃T�C�Y�͋��� )
	for( i = 16;i >= 0;i-- ){
		gDPLoadTLUT_pal256(gp++,try_bm_tlut[try_no[i]]);
		gDPLoadTextureTile_4b(gp++,try_bm[try_no[i]], G_IM_FMT_CI,24,22,0,0,23,21,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
		x_pos = try_position[i][0] + puls;
		gSPScisTextureRectangle(gp++,x_pos << 2,try_position[i][1] << 2,x_pos + 24 << 2,
										try_position[i][1] + 22 << 2,G_TX_RENDERTILE, 0, 0,1<<10,1 << 10);

	}

}

/*--------------------------------------
	GAME OVER �������֐�
--------------------------------------*/
void	init_game_over(void)
{
	game_over_action_count[0] = game_over_action_count[1] = 0;				//	��������t���O�̏�����
	game_over_action_flg = 0;												//	��������t���O�̏�����

	game_over_size[0][0] = game_w_scale_table[0];							//	game�̏�������
	game_over_size[0][1] = game_h_scale_table[0];							//	game�̏����c��
	game_over_size[1][0] = over_w_scale_table[0];							//	over�̏�������
	game_over_size[1][1] = over_h_scale_table[0];							//	over�̏�������
	game_over_position[0] = game_position_table[0];							//	game�̏����x���W
	game_over_position[1] = over_position_table[0];							//	over�̏����x���W

}

/*--------------------------------------
	GAME OVER �������C���֐�
--------------------------------------*/
s8	game_over_main(void)
{
	switch(game_over_action_flg)
	{
	case	0:	//	�ړ�

		game_over_position[0] = game_position_table[game_over_action_count[0]];	//	GAME �̂x���W
		game_over_position[1] = over_position_table[game_over_action_count[1]];	//	OVER �̂x���W

		game_over_size[0][0] = game_w_scale_table[game_over_action_count[0]];	//	GAME �̉���
		game_over_size[0][1] = game_h_scale_table[game_over_action_count[0]];	//	GAME �̏c��
		game_over_size[1][0] = over_w_scale_table[game_over_action_count[1]];	//	OVER �̉���
		game_over_size[1][1] = over_h_scale_table[game_over_action_count[1]];	//	OVER �̏c��
		if( game_over_action_count_flg >= 1 ){
			game_over_action_count_flg = 0;
			game_over_action_count[0] ++;
			game_over_action_count[1] ++;
		}else{
			game_over_action_count_flg ++;
		}

		if( game_over_action_count[0] >= 40 ){
			game_over_action_flg = 1;
		}
		if( game_over_action_count[0] >= dm_demo_count ){
			return	1;	//	�L�[��t�J�n
		}else	{
			return	-1;	//	�܂�������
		}
	case	1:	//	�g��
		game_over_position[0] = game_position_table[game_over_action_count[0]];	//	GAME �̂x���W
		game_over_position[1] = over_position_table[game_over_action_count[1]];	//	OVER �̂x���W

		game_over_size[0][0] = game_w_scale_table[game_over_action_count[0]];	//	GAME �̉���
		game_over_size[0][1] = game_h_scale_table[game_over_action_count[0]];	//	GAME �̏c��
		game_over_size[1][0] = over_w_scale_table[game_over_action_count[1]];	//	OVER �̉���
		game_over_size[1][1] = over_h_scale_table[game_over_action_count[1]];	//	OVER �̏c��

		if( game_over_action_count_flg >= 1 ){
			game_over_action_count_flg = 0;
			game_over_action_count[0] ++;
			game_over_action_count[1] ++;

			if( game_over_action_count[0] > 52 ){
				game_over_action_count[0] = 40;
			}

			if( game_over_action_count[1] > 52 ){
				game_over_action_count[1] = 40;
			}
		}else{
			game_over_action_count_flg ++;
		}

		return	1;
	}
}
/*--------------------------------------
	GAME OVER �`�惁�C���֐�
--------------------------------------*/
void	game_over_draw(s16 x_pos)
{
	s8	i;

	//	game�`��
	gDPLoadTLUT_pal256(gp++,test_game_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,&test_game_bm0_0[0], G_IM_FMT_CI,72,23,0,0,71,22,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle(gp++,x_pos - (game_over_size[0][0] >> 1)<< 2,game_over_position[0] << 2,x_pos + (game_over_size[0][0] >> 1) << 2,
									game_over_position[0] + game_over_size[0][1] << 2,G_TX_RENDERTILE, 0, 0,  (72 << 10) / game_over_size[0][0], (23 << 10) / game_over_size[0][1]);

	//	over�`��
	gDPLoadTLUT_pal256(gp++,test_over_bm0_0tlut);
	gDPLoadTextureTile_4b(gp++,&test_over_bm0_0[0], G_IM_FMT_CI,72,23,0,0,71,22,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	gSPScisTextureRectangle(gp++,x_pos - (game_over_size[1][0] >> 1)<< 2,game_over_position[1] << 2,x_pos + (game_over_size[1][0] >> 1) << 2,
									game_over_position[1] + game_over_size[1][1] << 2,G_TX_RENDERTILE, 0, 0,  (72 << 10) / game_over_size[1][0], (23 << 10) / game_over_size[1][1]);

}
/*--------------------------------------
	WIN �������֐�
--------------------------------------*/
void	init_win(void)
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		win_mode[i] = 0;
		win_count[i] = 0;
		win_flg[i] = -1;
		set_sp_mtx(&s2d_win_mtx[i],1,0,0,1,win2_position_x_table[win_count[i]],win2_position_y_table[win_count[i]] + 110,48,32,48,32);	//	�����}�g���N�X�̐ݒ�
	}
	set_sprite(&s2d_win_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);						//	WIN �p�X�v���C�g�̐ݒ�
	load_set_sprite(&s2d_win_tex,0,test_win_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);			//	WIN	�p�e�N�X�`�����[�h�̐ݒ�
	load_set_pal(&s2d_win_pal,test_win_bm0_0tlut,16);										//	WIN	�p�p���b�g���[�h�̐ݒ�
	set_sprite(&s2d_win_ia_sp,-24,-16,48,32,G_IM_FMT_I,G_IM_SIZ_4b,48);						//	WIN �p�X�v���C�g�̐ݒ�(�C���e���V�e�B)
	load_set_sprite(&s2d_win_ia_tex,0,test_win_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,32);		//	WIN	�p�e�N�X�`�����[�h�̐ݒ�(�C���e���V�e�B)
}

/*--------------------------------------
	WIN �������C���֐�
--------------------------------------*/
s8	win_main(s16 x_pos,s8 mtx_no)
{
	s32	Cos,Sin;

	//	�}�g���N�X�v�Z
	Cos = (s32)coss( (win2_rotate_z_table[win_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (win2_rotate_z_table[win_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_win_mtx[mtx_no],Cos,Sin,-Sin,Cos,win2_position_x_table[win_count[mtx_no]] + x_pos,win2_position_y_table[win_count[mtx_no]] + 110,
					win2_w_scale_table[win_count[mtx_no]] * 48,win2_h_scale_table[win_count[mtx_no]] * 32,48,32);

	win_count[mtx_no] ++;

	if( win_count[mtx_no] >= dm_demo_count ){
		win_flg[mtx_no] = 1;	//	�L�[��t�J�n
	}

	if(win_count[mtx_no] > 74)
	{
		win_count[mtx_no] = 26;
	}
	return	win_flg[mtx_no];
}

/*--------------------------------------
	WIN �`�惁�C���֐�
--------------------------------------*/
void	win_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );			//	2D�`��ݒ�
	gSPObjMatrix( gp++, &s2d_win_mtx[mtx_no] );	//	�}�g���N�X���[�h

	//	�e�N�X�`���`��
	gSPObjLoadTxtr( gp++,&s2d_win_pal );	//	�p���b�g���[�h
	gSPObjLoadTxtr( gp++,&s2d_win_tex );	//	�e�N�X�`�����[�h
	gSPObjSprite( gp++,&s2d_win_sp );		//	�`��
}

/*--------------------------------------
	LOSE �������֐�
--------------------------------------*/
void	init_lose(void)
{
	s8	i;

	set_sprite(&s2d_lose_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	LOSE �p�X�v���C�g�̐ݒ�
	load_set_sprite(&s2d_lose_tex,0,test_lose_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);		//	LSOE �p�e�N�X�`�����[�h�̐ݒ�
	load_set_pal(&s2d_lose_pal,test_lose_bm0_0tlut,16);									//	LSOE �p�p���b�g���[�h�̐ݒ�

	set_sprite(&s2d_lose_ia_sp,-24,-15,48,30,G_IM_FMT_I,G_IM_SIZ_4b,48);				//	LOSE �p�X�v���C�g�̐ݒ�(�C���e���V�e�B)
	load_set_sprite(&s2d_lose_ia_tex,0,test_lose_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,30);	//	LSOE �p�e�N�X�`�����[�h�̐ݒ�(�C���e���V�e�B)


	for(i = 0;i < 4;i++){
		lose_count[i] = 0;
		lose_flg[i] = -1;
		set_sp_mtx(&s2d_lose_mtx[i],1,0,0,1,lose2_x_position_table[lose_count[i]],lose2_y_position_table[lose_count[i]] + 110,48,30,48,30);	// �����}�g���N�X�̍쐬
	}
}

/*--------------------------------------
	LOSE �������C���֐�
--------------------------------------*/
s8	lose_main(s8 mtx_no,s16 x_pos)
{
	s32	Cos,Sin;

	Cos = (s32)coss( (lose2_rotate_z_table[lose_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (lose2_rotate_z_table[lose_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_lose_mtx[mtx_no],Cos,Sin,-Sin,Cos,lose2_x_position_table[lose_count[mtx_no]] + x_pos,(-1 * lose2_y_position_table[lose_count[mtx_no]]) + 110,
					lose2_w_scale_table[lose_count[mtx_no]] * 48,lose2_h_scale_table[lose_count[mtx_no]] * 30,48,30);

	lose_count[mtx_no] ++;
	if(lose_count[mtx_no] >= dm_demo_count){
		lose_flg[mtx_no] = 1;
	}

	if(lose_count[mtx_no] > 169)
	{
		lose_count[mtx_no] = 26;
	}

	return	lose_flg[mtx_no];
}

/*--------------------------------------
	LOSE �`�惁�C���֐�
--------------------------------------*/
void	lose_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );				//	2D�`��ݒ�
	gSPObjMatrix( gp++,	&s2d_lose_mtx[mtx_no] );	//	�}�g���N�X���[�h

	//	�e�N�X�`���`��
	gSPObjLoadTxtr( gp++,&s2d_lose_pal );			//	�p���b�g���[�h
	gSPObjLoadTxtr( gp++,&s2d_lose_tex );			//	�e�N�X�`�����[�h
	gSPObjSprite( gp++,&s2d_lose_sp );				//	�`��
}

/*--------------------------------------
	DRAW �������֐�
--------------------------------------*/
void	init_draw(void)
{
	s8	i;

	set_sprite(&s2d_draw_sp,-32,-16,64,32,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	DRAW �p�X�v���C�g�̐ݒ�
	load_set_sprite(&s2d_draw_tex,0,test_draw_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,32);		//	DRAW �p�e�N�X�`�����[�h�̐ݒ�

	load_set_pal(&s2d_draw_pal,test_draw_bm0_0tlut,16);									//	DRAW �p�p���b�g���[�h�̐ݒ�

	set_sprite(&s2d_draw_ia_sp,-24,-14,48,28,G_IM_FMT_I,G_IM_SIZ_4b,48);				//	DRAW �p�X�v���C�g�̐ݒ�(�C���e���V�e�B)
	load_set_sprite(&s2d_draw_ia_tex,0,test_draw_i_bm0_0,G_IM_FMT_I,G_IM_SIZ_4b,48,30);	//	DRAW �p�e�N�X�`�����[�h�̐ݒ�(�C���e���V�e�B)


	for(i = 0;i < 4;i++){
		draw_count[i] = 0;
		draw_flg[i] = -1;
		set_sp_mtx(&s2d_draw_mtx[i],1,0,0,1,draw3_x_position_table[draw_count[i]],0,48,28,48,28);	// �����}�g���N�X�̍쐬
	}
}
/*--------------------------------------
	DRAW �������C���֐�
--------------------------------------*/
s8	draw_main(s8 mtx_no,s16 x_pos)
{
	s32	Cos,Sin;

	Cos = (s32)coss( (draw3_rotate_z_table[draw_count[mtx_no]] * 0xffff / 360) );
	Sin = (s32)sins( (draw3_rotate_z_table[draw_count[mtx_no]] * 0xffff / 360) );
	set_sp_mtx(&s2d_draw_mtx[mtx_no],Cos,Sin,-Sin,Cos,draw3_x_position_table[draw_count[mtx_no]] + x_pos,110,
					draw3_w_scale_table[draw_count[mtx_no]] * 48,draw3_h_scale_table[draw_count[mtx_no]] * 28,48,28);

	draw_count[mtx_no] ++;
	if(draw_count[mtx_no] >= dm_demo_count){
		draw_flg[mtx_no] = 1;
	}
	if(draw_count[mtx_no] > 68)
	{
		draw_count[mtx_no] = 28;
	}

	return	draw_flg[mtx_no];
}

/*--------------------------------------
	DRAW �`�惁�C���֐�
--------------------------------------*/
void	draw_draw(s8 mtx_no)
{
	gSPDisplayList( gp++, init_2d_dl );				//	2D�`��ݒ�
	gSPObjMatrix( gp++,	&s2d_draw_mtx[mtx_no] );	//	�}�g���N�X���[�h


	//	�e�N�X�`���`��
	gSPObjLoadTxtr( gp++,&s2d_draw_pal );			//	�p���b�g���[�h
	gSPObjLoadTxtr( gp++,&s2d_draw_tex );			//	�e�N�X�`�����[�h
	gSPObjSprite( gp++,&s2d_draw_sp );				//	�`��
}


/*--------------------------------------
	�A�� �������֐�
--------------------------------------*/
void	init_chaine(void)
{
	s8	i,j;

	u16	*cap_pal[] = {
		test_capsel_blue_bm0_0tlut,
		test_capsel_yellow_bm0_0tlut,
		test_capsel_red_bm0_0tlut,
	};

	u16	*p_icon_pal[] = {
		dm_4p_game_target_1p_bm0_0tlut,
		dm_4p_game_target_2p_bm0_0tlut,
		dm_4p_game_target_3p_bm0_0tlut,
		dm_4p_game_target_4p_bm0_0tlut
	};

	u8	*p_icon_bm[] = {
		dm_4p_game_target_1p_bm0_0,
		dm_4p_game_target_2p_bm0_0,
		dm_4p_game_target_3p_bm0_0,
		dm_4p_game_target_4p_bm0_0
	};

	u8	*cap_bm[] = {
		test_capsel_blue_bm0_0,
		test_capsel_yellow_bm0_0,
		test_capsel_red_bm0_0,
	};

	u8	set_table[4][3] = {
		{1,2,3},{2,3,0},{3,0,1},{0,1,2}
	};

	//	�A���\���֌W�̏�����
	load_set_sprite(&s2d_chaine_tex,0,test_chained_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,160,20);
	load_set_pal(&s2d_chaine_pal,test_chained_bm0_0tlut,16);

	//	����
	set_sprite(&s2d_chaine_num_sp,-16,-16,32,32,G_IM_FMT_CI,G_IM_SIZ_4b,32);														//	�X�v���C�g�ݒ�
	load_set_pal(&s2d_chaine_num_pal,test_chained_num_bm0_0tlut,16);																//	�Ǎ��݃p���b�g�ݒ�

	//	�J�v�Z��
	set_sprite(&s2d_chaine_cap_sp,-8,-8,16,16,G_IM_FMT_CI,G_IM_SIZ_4b,16);															//	�X�v���C�g�ݒ�
	for(i = 0;i < 3;i++){
		load_set_sprite(&s2d_chaine_cap_tex[i],0,cap_bm[i],G_IM_FMT_CI,G_IM_SIZ_4b,16,16);													//	�Ǎ��݃e�N�X�`���ݒ�
		load_set_pal(&s2d_chaine_cap_pal[i],cap_pal[i],16);																			//	�Ǎ��݃p���b�g�ݒ�
	}

	//	�v���C���[�}�[�N
	set_sprite(&s2d_chaine_player_sp,0,0,32,16,G_IM_FMT_CI,G_IM_SIZ_4b,32);															//	�X�v���C�g�ݒ�
	for(i = 0;i < 4;i++){
		test_chaine_mode[i] = 0;
		for(j = 0;j < 3;j++){
			load_set_sprite(&s2d_chaine_player_tex[i][j],0,p_icon_bm[set_table[i][j]],G_IM_FMT_CI,G_IM_SIZ_4b,32,16);				//	�Ǎ��݃e�N�X�`���ݒ�
			load_set_pal(&s2d_chaine_player_pal[i][j],p_icon_pal[set_table[i][j]],16);												//	�Ǎ��݃p���b�g�ݒ�
		}
	}
}
/*--------------------------------------
	�A�� �����\����~�֐�
--------------------------------------*/
void	stop_chain_main(u8 no){
	test_chaine_mode[no] = 0;
}

/*--------------------------------------
	�A�� �������C���֐�
--------------------------------------*/
void	chain_main(game_state *state,u8 player_no,u8 chaine,u8 mode,s16 center)
{
	s8	size_w[] = {80,64};							//	�`��͈͂̉���
	s8	chaine_w[] = {70,60};						//	�k�����i�X�P�[���l�j
	s8	num_max_w[][2] = {{48,36},{42,36}};			//	�����g�厞�̍ő�̑傫��
	s8	num_center[][3] = {{24,18,30},{20,16,24}};	//	�����̊g�咆�S���W�����߂鐔�l
	s16	i,j,x,x2,y;
	s32	Cos,Sin;

	switch( test_chaine_mode[player_no] )
	{
	case	0:	//	�����ݒ�
		if( chaine > 1 ){


			test_chaine_mode[player_no] = 1;								//	���̏�����
			test_chaine_num[player_no]	= chaine;							//	�A�����̃Z�b�g
			test_chaine_alpha[player_no] = 0xff;							//	�����x
			test_chaine_center[player_no] = center;						//	�r�̒��S�ʒu�̕ۑ�
			test_chaine_scis_x[player_no] = center - (size_w[mode]	>> 1);	//	�`��͈͊J�n�w���W�̐ݒ�
			test_chaine_scis_w[player_no] = size_w[mode];					//	�`��͈͉����̐ݒ�

			//	CHAINED
			set_sprite(&s2d_chaine_sp[player_no],test_chaine_center[player_no] - 10,104,160,20,G_IM_FMT_CI,G_IM_SIZ_4b,160);	//	�X�v���C�g�ݒ�
			s2d_chaine_sp[player_no].s.scaleW = (160 << 10) / chaine_w[mode];
			s2d_chaine_sp[player_no].s.scaleH = 2048;

			//	�A����
			load_set_sprite(&s2d_chaine_num_tex[player_no][0],0,&test_chained_num_bm0_0[512 * test_chaine_num[player_no]],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	�Ǎ��݃e�N�X�`���ݒ�
			set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,14,10,32,32);		//	�}�g���N�X�ݒ�

			for( i = 0;i < 4;i++ ){
				test_chaine_max[player_no][i] = 0;
			}

			for( i = 2,x = j = 0 ;i >= 0;i-- ){
				if( state -> chain_color[3] & ( 1 << i ) ){
					test_chaine_max[player_no][x] = 1;	//	�\������t���O
					j ++;
				}
				x++;
			}
			test_chaine_max[player_no][3] = j; //	�U������̐�
		}
		break;
	case	1:	//	�A������r
		if( chaine == 0 ){
			//	�A���I��
			test_chaine_mode[player_no] = 99;	//	�m�菈��(NICE.FINE.BEOUTIFUL�\��)��

			//	�X�v���C�g�̐ݒ�
			set_sprite(&s2d_chaine_mes_sp[player_no],test_chaine_center[player_no] + (size_w[mode]	>> 1),120,160,20,G_IM_FMT_CI,G_IM_SIZ_4b,160);				//	�X�v���C�g�ݒ�
			s2d_chaine_mes_sp[player_no].s.scaleW = (160 << 10) / chaine_w[mode];
			s2d_chaine_mes_sp[player_no].s.scaleH = 2048;

			//	�A���������ɕ\�����镶����I��
			switch( test_chaine_num[player_no] )
			{
			case	2:	//	Fine
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0,&test_fine_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);						//	�Ǎ��݃e�N�X�`���ݒ�
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_fine_bm0_0tlut,16);														//	�Ǎ��݃p���b�g�ݒ�
				break;
			case	3:	//	Nice
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0 ,&test_nice_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);						//	�Ǎ��݃e�N�X�`���ݒ�
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_nice_bm0_0tlut,16);														//	�Ǎ��݃p���b�g�ݒ�
				break;
			default:	//	Beoutiful
				load_set_sprite(&s2d_chaine_mes_tex[player_no],0,&test_beoutiful_bm0_0[0],G_IM_FMT_CI,G_IM_SIZ_4b,160,20);					//	�Ǎ��݃e�N�X�`���ݒ�
				load_set_pal(&s2d_chaine_mes_pal[player_no],test_beoutiful_bm0_0tlut,16);													//	�Ǎ��݃p���b�g�ݒ�
				break;
			}
			if( mode ){
				//	4P���[�h�̏ꍇ
				Cos = (s32)coss( (180 * 0xffff / 360)  );
				Sin = (s32)sins( (180 * 0xffff / 360)  );
				x = test_chaine_center[player_no] + 40;
				y = 140;
				//	���ł��� 1P.2P.3P.4P�ƃJ�v�Z���̐ݒ�
				for( i = j = 0;i < 3;i++ ){
					test_chaine_cap_r[player_no][i] = 180;
					set_sp_sub_mtx(&s2d_chaine_player_mtx[player_no][i],x + 10,y - 8,20,12,32,16);		//	�ړ������̃}�g���N�X�ݒ�
					set_sp_mtx(&s2d_chaine_cap_mtx[player_no][i],Cos,Sin,-Sin,Cos,x,y,12,14,16,16);		//	��]�̃}�g���N�X�ݒ�
					y += 14;
					x += 10;
				}
			}
		}else{
			if( test_chaine_num[player_no] != chaine ){	//	�A���������Ă���
				//	�A����(�������g��p)

				test_chaine_save[player_no] = chaine;	//	�g��O�̘A�����ۑ�
				test_chaine_mode[player_no] = 10;		//	�g�又����

				test_chaine_num_w_size[player_no] = 14;	//	�����A���������̐ݒ�
				test_chaine_num_h_size[player_no] = 10;	//	�����A�����c���̐ݒ�

				load_set_sprite(&s2d_chaine_num_tex[player_no][3],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] % 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	�Ǎ��݃e�N�X�`���ݒ�

				if(test_chaine_num[player_no] < 10){
					//	�A�������P��
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - 24,108,14,10,32,32);				//	�}�g���N�X�ݒ�

				}else	if(test_chaine_num[player_no] > 9){
					//	�A�������Q��
					load_set_sprite(&s2d_chaine_num_tex[player_no][2],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] / 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	�Ǎ��݃e�N�X�`���ݒ�
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - 18,108,14,10,32,32);				//	�}�g���N�X�ݒ�
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][2],test_chaine_center[player_no] - 30,108,14,10,32,32);				//	�}�g���N�X�ݒ�

				}
			}
		}
		break;
	case	10:	//	�����g�又��

		i = 2;	//	�g�呬�x
		if( test_chaine_save[player_no] > 4 ){
			j = num_max_w[mode][0];	//	�g��̍ő�̑傫��
		}else{
			j = num_max_w[mode][1];	//	�g��̍ő�̑傫��
		}

		test_chaine_num_w_size[player_no] += i;	//	��������
		test_chaine_num_h_size[player_no] += i;	//	�c������

		//	�c�����ő�l�ɒB�����ꍇ
		if( test_chaine_num_h_size[player_no] >= j )
			test_chaine_num_h_size[player_no] = j;	//	�c�����ő�l�ɂ���

		//	�������ő�l�ɒB�����ꍇ
		if(test_chaine_num_w_size[player_no] >= j)
		{
			test_chaine_mode[player_no] = 1;		//	�A���҂���
			test_chaine_num[player_no] = test_chaine_save[player_no];	//	�A�����̑���
			load_set_sprite(&s2d_chaine_num_tex[player_no][0],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] % 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	�Ǎ��݃e�N�X�`���ݒ�
			if( test_chaine_num[player_no] < 4){
				//	�A�������P���łR�ȉ�
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,14,10,32,32);					//	�}�g���N�X�ݒ�
			}else{
				if( test_chaine_num[player_no] < 10){
					//	�A�������P���łS�ȏ�
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][0],108,16,16,32,32);				//	�}�g���N�X�ݒ�
				}else	if( test_chaine_num[player_no] > 9){
					//	�A�������Q��
					load_set_sprite(&s2d_chaine_num_tex[player_no][1],0,&test_chained_num_bm0_0[512 * (test_chaine_num[player_no] / 10)],G_IM_FMT_CI,G_IM_SIZ_4b,32,32);	//	�Ǎ��݃e�N�X�`���ݒ�
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][0],test_chaine_center[player_no] - num_center[mode][1],108,16,16,32,32);				//	�}�g���N�X�ݒ�
					set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][1],test_chaine_center[player_no] - num_center[mode][2],108,16,16,32,32);				//	�}�g���N�X�ݒ�
				}
			}
		}else{
			if( test_chaine_num[player_no] < 10){
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - num_center[mode][0],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	�}�g���N�X�ݒ�
			}else	if( test_chaine_num[player_no] > 9){
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][3],test_chaine_center[player_no] - num_center[mode][1],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	�}�g���N�X�ݒ�
				set_sp_sub_mtx(&s2d_chaine_num_mtx[player_no][2],test_chaine_center[player_no] - num_center[mode][2],108,test_chaine_num_w_size[player_no],test_chaine_num_h_size[player_no],32,32);				//	�}�g���N�X�ݒ�
			}
		}
		break;
	case	99:	//	FINE NICE BEOUTIFUL �̉��ړ�
		x = s2d_chaine_mes_sp[player_no].s.objX >> 2;
		x -= 4;
		//	�����ɂ���čŏI�ʒu���Ⴄ
		if( test_chaine_num[player_no] < 4 ){
			j = test_chaine_center[player_no] - 8;		//	FINE
		}else{
			j = test_chaine_center[player_no] - 32 + (mode << 2);	//	BEOUTIFUL
		}

		//	�ŏI�ʒu�ɒB�����ꍇ
		if( x <= j ){
			x = j ;
			if(mode){
				test_chaine_mode[player_no] = 100;		//	�J�v�Z���X�N���[���C��������
			}else{
				test_chaine_mode[player_no] = 101;		//	�t�F�[�h�A�E�g������
			}
		}
		s2d_chaine_mes_sp[player_no].s.objX = x << 2;
		break;
	case	100:	//	�J�v�Z���X�N���[���C��
		j = test_chaine_center[player_no] - 4;	//	�J�v�Z���̍ŏI�ړ��n�_
		for(i = 0;i < test_chaine_max[player_no][3];i++){
			test_chaine_cap_r[player_no][i] += 30;				//	��]�p�x��30�x����
			x = s2d_chaine_cap_mtx[player_no][i].m.X >> 2;		//	���݂̂w���W
			y = s2d_chaine_cap_mtx[player_no][i].m.Y >> 2;		//	���݂̂x���W

			x -= 2;	//	�w���W�̈ړ�
			if( x <= j ){
				x = j;	//	�ŏI�ړ��n�_�ɓ��B
				test_chaine_cap_r[player_no][i] = 0;				//	��]�p�x���O�ɖ߂�

				if( i == test_chaine_max[player_no][3] - 1){
					//	�J�v�Z�����ŏI�ړ��n�_�ɓ��B�����ꍇ
					test_chaine_mode[player_no] = 101;		//	�t�F�[�h�A�E�g������
				}
			}
			Cos = (s32)coss( (test_chaine_cap_r[player_no][i] * 0xffff / 360)  );
			Sin = (s32)sins( (test_chaine_cap_r[player_no][i] * 0xffff / 360)  );
			set_sp_mtx(&s2d_chaine_cap_mtx[player_no][i],Cos,Sin,-Sin,Cos,x,y,12,14,16,16);	//	�J�v�Z���}�g���N�X�ݒ�

			y = s2d_chaine_player_mtx[player_no][i].m.Y >> 2;
			set_sp_sub_mtx(&s2d_chaine_player_mtx[player_no][i],x + 10,y,20,12,32,16);			//	1P.2P.3P.4P�̃}�g���N�X�ݒ�
		}
		break;
	case	101:	//	�t�F�[�h�A�E�g����
		test_chaine_alpha[player_no] -= 8;
		if( test_chaine_alpha[player_no] <= 0x00 )
		{
			test_chaine_alpha[player_no] = 0;
			test_chaine_mode[player_no] = 0;		//	�A���҂�
		}
		break;
	}
}

/*--------------------------------------
	�A�� �`�惁�C���֐�
--------------------------------------*/
void	draw_chaine(s8 player_no,s8 mode)
{
	s8	i,j;

	gSPDisplayList( gp++, init_2d_dl );							//	2D�`��ݒ�
	gDPSetRenderMode( gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2 );	//	�������w��

	if( test_chaine_mode[player_no] > 0){
		gDPSetScissor( gp++, G_SC_NON_INTERLACE,test_chaine_scis_x[player_no],0, test_chaine_scis_x[player_no] + test_chaine_scis_w[player_no], SCREEN_HT );	//	�`��͈͎w��
		gDPSetPrimColor(gp++,0,0,255,255,255,test_chaine_alpha[player_no]);	//	�����x�ݒ�
		gSPObjLoadTxtr( gp++,&s2d_chaine_tex );							//	CHAINE	�e�N�X�`�����[�h
		gSPObjLoadTxtr( gp++,&s2d_chaine_pal );							//	CHAINE	�p���b�g���[�h
		gSPObjRectangle( gp++,&s2d_chaine_sp[player_no]);						//	CHAINE	�`��

		//	�`�揇�̌v�Z
		if( test_chaine_num[player_no] > 9){
			j = 1;	//	2���̏ꍇ�A�\�̈ʂ���`�悷��
		}else{
			j = 0;	//	1���̏ꍇ�͂��̂܂�
		}

		//	���݂̘A����
		gSPObjLoadTxtr( gp++,&s2d_chaine_num_pal );				//	�A����	�p���b�g�̃��[�h
		for( i = j;i >= 0;i-- ){
			gSPObjLoadTxtr( gp++,&s2d_chaine_num_tex[player_no][i] );	//	�A����	�e�N�X�`���̃��[�h

			gSPObjSubMatrix(gp++, &s2d_chaine_num_mtx[player_no][i] );	//	�A����	�}�g���N�X�̃��[�h(�ړ����l)
			gSPObjRectangleR( gp++,&s2d_chaine_num_sp );			//	�A����	�`��
		}


		//	�ߋ��̊g��A����
		if( test_chaine_mode[player_no] > 1 && test_chaine_mode[player_no] < 99 ){
			gDPSetPrimColor(gp++,0,0,255,255,255,128);
			if( test_chaine_num[player_no] > 9)
				j = 2;
			else
				j = 3;

			for(i = 3;i >= j;i--){
				gSPObjLoadTxtr( gp++,&s2d_chaine_num_tex[player_no][i] );
				gSPObjSubMatrix(gp++, &s2d_chaine_num_mtx[player_no][i] );
				gSPObjRectangleR( gp++,&s2d_chaine_num_sp );
			}
			gDPSetPrimColor(gp++,0,0,255,255,255,test_chaine_alpha[player_no]);	//	�A���t�@��߂�
		}

		if( test_chaine_mode[player_no] > 98 ){
			//	���b�Z�[�W�̕`��
			gSPObjLoadTxtr( gp++,&s2d_chaine_mes_tex[player_no] );
			gSPObjLoadTxtr( gp++,&s2d_chaine_mes_pal[player_no] );
			gSPObjRectangle( gp++,&s2d_chaine_mes_sp[player_no] );

			if( mode ){
				//	4P���[�h�̏ꍇ
				//	�J�v�Z���̕`��
				for(i = j = 0;i < 3;i++){
					if( test_chaine_max[player_no][i] ){
						gSPObjLoadTxtr( gp++,&s2d_chaine_cap_tex[i] );					//	�e�N�X�`�����[�h
						gSPObjLoadTxtr( gp++,&s2d_chaine_cap_pal[i] );					//	�p���b�g���[�h
						gSPObjMatrix(gp++, &s2d_chaine_cap_mtx[player_no][j] );			//	�}�g���N�X���[�h
						gSPObjSprite( gp++,&s2d_chaine_cap_sp );						//	�`��J�n

						gSPObjLoadTxtr( gp++,&s2d_chaine_player_tex[player_no][i] );	//	�e�N�X�`�����[�h
						gSPObjLoadTxtr( gp++,&s2d_chaine_player_pal[player_no][i] );	//	�p���b�g���[�h
						gSPObjSubMatrix(gp++, &s2d_chaine_player_mtx[player_no][j] );	//	�}�g���N�X���[�h
						gSPObjRectangleR( gp++,&s2d_chaine_player_sp );					//	�`��J�n
						j ++;
					}
				}
			}
		}
	}
}

/*--------------------------------------
	PAUSE �������֐�
--------------------------------------*/
void	init_pause(void)
{
	s8	i;

	set_sprite(&s2d_pause_sp,-32,-12,64,20,G_IM_FMT_CI,G_IM_SIZ_4b,64);					//	PAUSE �p�X�v���C�g�̐ݒ�
	load_set_sprite(&s2d_pause_tex,0,test_pause_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,64,20);	//	PAUSE �p�e�N�X�`�����[�h�̐ݒ�
	load_set_pal(&s2d_pause_pal,test_pause_bm0_0tlut,16);								//	PAUSE �p�p���b�g���[�h�̐ݒ�

	for( i = 0;i < 4;i++ ){
		pause_count[i] = pause_anime_count[i] = pause_mode[i] = 0;								//	PAUSE �p����ϐ�
	}
}

/*--------------------------------------
	PAUSE �������C���֐�
--------------------------------------*/
s8	pause_main( u8 player_no,s16 center )
{
	switch( pause_mode[player_no] )
	{
	case	0:	//	������
		pause_count[player_no] = 1;
		pause_anime_count[player_no] = 0;
		set_sp_sub_mtx(&s2d_pause_mtx[player_no],center,pause_position_table[pause_anime_count[player_no]] + 20,
				pause_w_scale_table[pause_anime_count[player_no]],pause_h_scale_table[pause_anime_count[player_no]],64,20);		//	�}�g���N�X�ݒ�

		pause_mode[player_no] = 1;
		return	-1;
	case	1:	//	�A�j���[�V����

		pause_count[player_no] ++;
		if( pause_anime_count[player_no] < 18 ){
			if( pause_count[player_no] > 1 ){
				pause_count[player_no] = 0;
				pause_anime_count[player_no] ++;	//	�R�}��i�߂�
			}
		}else{
			if( pause_count[player_no] > 6 ){
				pause_count[player_no] = 0;
				pause_anime_count[player_no] ++;	//	�R�}��i�߂�
			}
		}

		if( pause_anime_count[player_no] > 22 ){
			pause_anime_count[player_no] = 18;
		}

		set_sp_sub_mtx(&s2d_pause_mtx[player_no],center,pause_position_table[pause_anime_count[player_no]] + 20,
				pause_w_scale_table[pause_anime_count[player_no]],pause_h_scale_table[pause_anime_count[player_no]],64,20);		//	�}�g���N�X�ݒ�

		if( pause_anime_count[player_no] >= 5 ){
			return	1;
		}else{
			return	-1;
		}
	}
}
/*--------------------------------------
	PAUSE �`�惁�C���֐�
--------------------------------------*/
void	pause_draw( u8 player_no )
{
	if( pause_mode[player_no] > 0 ){
		gSPObjLoadTxtr( gp++,&s2d_pause_pal );				//	PAUSE �p���b�g���[�h
		gSPObjLoadTxtr( gp++,&s2d_pause_tex );				//	PAUSE �e�N�X�`�����[�h
		gSPObjSubMatrix(gp++, &s2d_pause_mtx[player_no] );	//	PAUSE �}�g���N�X���[�h
		gSPObjRectangleR( gp++,&s2d_pause_sp );				//	PAUSE �`��
	}
}

/*--------------------------------------
	SHOCK �������֐�
--------------------------------------*/
void	init_shock( void )
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		shock_mode[i] = 0;
		shock_size[i][0] = shock_size[i][1] = 1;
		shock_position[i][0] = shock_position[i][1] = 1;
		shock_alpha[i] = 0xff;
	}
}
/*--------------------------------------
	SHOCK �����ݒ�֐�
--------------------------------------*/
void	set_shock( u8 player_no, s16 x_pos,s16 y_pos )
{
	shock_mode[player_no] = 1;												//	�����J�n
	shock_alpha[player_no] = 0xff;											//	�����x(�s����)
	shock_size[player_no][0] = shock_size[player_no][1] = 12;				//	�����\���T�C�Y
	shock_position[player_no][0] = x_pos - (shock_size[player_no][0] >> 1);	//	�����w���W
	shock_position[player_no][1] = y_pos - (shock_size[player_no][1] >> 1);	//	�����x���W
}
/*--------------------------------------
	SHOCK �������C���֐�
--------------------------------------*/
void	shock_main( u8 player_no, s16 x_pos,s16 y_pos )
{
	s8	i;

	switch( shock_mode[player_no] )
	{
	case	1:	//	�g�又��
		shock_size[player_no][0] += 1;
		shock_size[player_no][1] += 2;
		shock_position[player_no][0] = x_pos - (shock_size[player_no][0] >> 1);	//	���W�ݒ�
		shock_position[player_no][1] = y_pos - (shock_size[player_no][1] >> 1);	//	���W�ݒ�

		if( shock_size[player_no][0] >= 30 ){
			shock_mode[player_no] = 2;
		}
		break;
	case	2:	//	�t�F�[�h�A�E�g
		shock_alpha[player_no] -= 0x08;
		if( shock_alpha[player_no] <= 0 ){
			shock_mode[player_no] = 0;
		}
		break;
	}
}
/*--------------------------------------
	SHOCK �`�惁�C���֐�
--------------------------------------*/
void	shock_draw( u8 player_no )
{
	if( shock_mode[player_no] ){
		gDPSetPrimColor(gp++,0,0,255,255,255,shock_alpha[player_no]);		//	�����x�ݒ�
		load_TexPal( face_4p_game_shock_bm0_0tlut );						//	�p���b�g���[�h
		load_TexTile_4b( face_4p_game_shock_bm0_0,24,24,0,0,23,23 );		//	�e�N�X�`�����[�h
		draw_ScaleTex( shock_position[player_no][0],shock_position[player_no][1],
						24,24,shock_size[player_no][0],shock_size[player_no][1],0,0 );	//	�`��
	}
}

/*--------------------------------------
	HUMMING �������֐�
--------------------------------------*/
void	init_humming( void )
{
	s8	i;

	for( i = 0;i < 4;i++ ){
		humming_mode[i] = 0;
		humming_sin[i] = 0;
		humming_size[i][0] = humming_size[i][1] = 1;
		humming_position[i][0] = humming_position[i][1] = humming_position[i][1] = 1;
		humming_alpha[i] = 0xff;
	}
}
/*--------------------------------------
	HUMMING �����ݒ�֐�
--------------------------------------*/
void	set_humming( u8 player_no, s16 x_pos,s16 y_pos )
{
	humming_mode[player_no] = 1;												//	�����J�n
	humming_alpha[player_no] = 0xff;											//	�����x(�s����)
	humming_sin[player_no] = 0x00;												//	�h��̎w��
	humming_size[player_no][0] = humming_size[player_no][1] = 12;				//	�����\���T�C�Y
	humming_position[player_no][0] = x_pos - (humming_size[player_no][0] >> 1);	//	�����w���W
	humming_position[player_no][1] = y_pos - (humming_size[player_no][1] >> 1);	//	�����x���W
	humming_position[player_no][2] = y_pos;
}
/*--------------------------------------
	HUMMING �������C���֐�
--------------------------------------*/
void	humming_main( u8 player_no, s16 x_pos )
{
	s8	i;

	switch( humming_mode[player_no] )
	{
	case	1:	//	�g�又��
		for( i = 0;i < 2;i++ ){
			humming_size[player_no][i] += 1;
		}
		humming_sin[player_no] += 30;
		if( humming_sin[player_no] >= 360 ){
			humming_sin[player_no] = 0;
		}
		x_pos = x_pos + (s16)(sinf( DEGREE (humming_sin[player_no])) * 10);
		humming_position[player_no][2] --;
		humming_position[player_no][0] = x_pos - (humming_size[player_no][0] >> 1);							//	���W�ݒ�
		humming_position[player_no][1] = humming_position[player_no][2] - (humming_size[player_no][1] >> 1);	//	���W�ݒ�

		if( humming_position[player_no][2] < 190 ){
			humming_mode[player_no] = 2;
		}
		break;
	case	2:	//	�t�F�[�h�A�E�g
		humming_alpha[player_no] -= 0x08;
		if( humming_alpha[player_no] <= 0 ){
			humming_mode[player_no] = 0;
		}
		break;
	}
}
/*--------------------------------------
	HUMMING �`�惁�C���֐�
--------------------------------------*/
void	humming_draw( u8 player_no )
{
	if( humming_mode[player_no] ){
		gDPSetPrimColor(gp++,0,0,255,255,255,humming_alpha[player_no]);		//	�����x�ݒ�
		load_TexPal( face_4p_game_onpu_bm0_0tlut );						//	�p���b�g���[�h
		load_TexTile_4b( face_4p_game_onpu_bm0_0,24,24,0,0,23,23 );		//	�e�N�X�`�����[�h
		draw_ScaleTex( humming_position[player_no][0],humming_position[player_no][1],
						24,24,humming_size[player_no][0],humming_size[player_no][1],0,0 );	//	�`��
	}
}

/*--------------------------------------
	�o�u�� �������֐�
--------------------------------------*/
void	init_bubble( void )
{
	s8	i,j,k;

	//	�e�t���O�̏�����
	for( i = 0;i < 4;i++ ){
		dm_bubble[i].bubble_disp_flg = 0;
		for( j = 0;j < 3;j ++ ){
			for( k = 0;k < 2;k ++ ){
				dm_bubble[i].bubble[j].bubble_flg[k] = 0;
				dm_bubble[i].bubble[j].bubble_pos[k] = 0;
			}
			for( k = 0;k < 4;k ++ ){
				dm_bubble[i].bubble[j].bubble_size[k] = 0;
			}
		}
	}
}
/*--------------------------------------
	�o�u�� �ݒ�֐�
--------------------------------------*/
void	set_bubble( game_state *state,s8 player_no ,s8 flg ,s8	set_flg,s8 col_r,s8 col_y,s8 col_b )
{
	s8	i,j,k,x,size,g_no;
	s8	attack_pos_2p[] = {12,40,68};
	s8	attack_col_table_2p[2] = {1,0};
	s8	attack_col[3];
	s8	attack_sp[2][2] = {
		{13,26},{12,24}
	};
	s8	attack_col_table_4p[4][3] = {
		{3,2,1},{0,3,2},{1,0,3},{2,1,0}
	};


	if( !dm_bubble[player_no].bubble_disp_flg ){
		//	�\�����łȂ�
		if( state -> chain_line > 1 ){
			dm_bubble[player_no].bubble_disp_flg = 1;
			if( state -> chain_line == 2 ){
				size = 12;
				g_no = bubble_small;
			}else	if( state -> chain_line == 3 ){
				size = 16;
				g_no = bubble_mid;
			}else	{
				size = 20;
				g_no = bubble_big;
			}
			if( flg == 0 ){	//	2PLAY
				dm_bubble[player_no].scis_y = 215;																//	�V�U�����O�͈͂̂x���\�ݒ�
				for( i = 0;i < 3;i ++ ){
					dm_bubble[player_no].bubble[i].bubble_size[bubble_w] = size;								//	�����ݒ�
					dm_bubble[player_no].bubble[i].bubble_size[bubble_h] = size;								//	�c���ݒ�
					dm_bubble[player_no].bubble[i].bubble_size[bubble_g] = g_no;								//	�O���t�B�b�N�ԍ��ݒ�
					dm_bubble[player_no].bubble[i].bubble_size[bubble_p] = attack_col_table_2p[flg];			//	�F�̐ݒ�( 2PLAY �Ȃ̂� ���� )
					dm_bubble[player_no].bubble[i].bubble_size[bubble_s] = attack_sp[0][set_flg] + 1;			//	�ړ��l( set_flg �� �O�̏ꍇ�A�r���ŗ������Ă���悤�ɂȂ� )

					dm_bubble[player_no].bubble[i].bubble_pos[0] = state -> map_x + attack_pos_2p[i];			//	�w���W�̐ݒ�
					dm_bubble[player_no].bubble[i].bubble_pos[1] = state -> map_y + 160 - size + i * size;		//	�x���W�̐ݒ�
					dm_bubble[player_no].bubble[i].bubble_pos[2] = dm_bubble[player_no].bubble[i].bubble_pos[1] + size;	//	���������ꍇ�̔�����łx���W
					dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 1;									//	�\���t���O�𗧂Ă�
					dm_bubble[player_no].bubble[i].bubble_flg[bubble_attack] = attack_col_table_2p[flg];		//	�U���ڕW�̐ݒ�
				}
				//	�c��ΐݒ�
				if( genrand( 6 ) == 1 ){

					i = genrand( 3 );

					dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] = dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] = 8;	//	�c���E�����ݒ�
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] = 3;									//	�O���t�B�b�N�ԍ��ݒ�
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] = attack_col_table_2p[flg];			//	�F�̐ݒ�( 2PLAY �Ȃ̂� ���� )
					dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] = 11;								//	�ړ��l( �r���ŗ������Ă���悤�ɂȂ� )

					dm_bubble[player_no].bubble_mini.bubble_pos[0] = state -> map_x + attack_pos_2p[i];			//	�w���W�̐ݒ�
					dm_bubble[player_no].bubble_mini.bubble_pos[1] = state -> map_y + 190 - size + i * 20;		//	�x���W�̐ݒ�
					dm_bubble[player_no].bubble_mini.bubble_pos[2] = dm_bubble[player_no].bubble_mini.bubble_pos[1];	//	���������ꍇ�̔�����łx���W
					dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 1;								//	�\���t���O�𗧂Ă�
				}
			}else{	//	4PLAY
				dm_bubble[player_no].scis_y = 188;																//	�V�U�����O�͈͂̂x���\�ݒ�
				attack_col[0] = col_r;																			//	�ԐF�ōU���������ǂ����̐ݒ�
				attack_col[1] = col_y;																			//	���F�ōU���������ǂ����̐ݒ�
				attack_col[2] = col_b;																			//	�F�ōU���������ǂ����̐ݒ�

				//	�U�����萔����A�w���W�̐��l������o��
				for( i = j = 0;i < 3;i++ ){
					if( state -> chain_color[3] & ( 1 << i ) ){
						j ++;
					}
				}
				if( j == 3 ){			//	�R�l�U��
					x = 8;
				}else	if( j == 2 ){	//	�Q�l�U��
					x = 16;
				}else{					//	�P�l�U��
					x = 32;
				}

				for( i = j = k = 0;i < 3;i++ ){
					if( state -> chain_color[3] & ( 1 << i ) ){																	//	�ԁE���E�̏��ōU�����������ׂ�
						dm_bubble[player_no].bubble[j].bubble_size[bubble_w] = size;											//	�����ݒ�
						dm_bubble[player_no].bubble[j].bubble_size[bubble_h] = size;											//	�c���ݒ�
						dm_bubble[player_no].bubble[j].bubble_size[bubble_g] = g_no;											//	�O���t�B�b�N�ԍ��ݒ�
						dm_bubble[player_no].bubble[j].bubble_size[bubble_p] = attack_col_table_4p[player_no][i];				//	�F�̐ݒ�( �U�����鑊��̐F )
						dm_bubble[player_no].bubble[j].bubble_size[bubble_s] = attack_sp[1][attack_col[i]] + 1;					//	�ړ��l( attack_col[i] �� �O�̏ꍇ�A�r���ŗ������Ă���悤�ɂȂ� )

						dm_bubble[player_no].bubble[j].bubble_pos[0] = state -> map_x + x;										//	�w���W�̐ݒ�
						dm_bubble[player_no].bubble[j].bubble_pos[1] = state -> map_y + 128 - size + j * size;					//	�x���W�̐ݒ�
						dm_bubble[player_no].bubble[j].bubble_pos[2] = dm_bubble[player_no].bubble[j].bubble_pos[1] + size;		//	���������ꍇ�̔�����łx���W
						dm_bubble[player_no].bubble[j].bubble_flg[bubble_disp] = 1;												//	�\���t���O�𗧂Ă�
						dm_bubble[player_no].bubble[i].bubble_flg[bubble_attack] = attack_col_table_4p[player_no][i];			//	�U���ڕW�̐ݒ�

						//	�c��ΐݒ�
						if( k == 0 ){
							if( genrand( 6 ) == 1 ){
								k = 1;
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] = dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] = 8;	//	�c���E�����ݒ�
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] = 3;											//	�O���t�B�b�N�ԍ��ݒ�
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] = attack_col_table_4p[player_no][i];			//	�F�̐ݒ�( �U�����鑊��̐F )
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] = 12;										//	�ړ��l( �r���ŗ������Ă���悤�ɂȂ� )

								dm_bubble[player_no].bubble_mini.bubble_pos[0] = state -> map_x + x;								//	�w���W�̐ݒ�
								dm_bubble[player_no].bubble_mini.bubble_pos[1] = state -> map_y + 148 - size + j * size;			//	�x���W�̐ݒ�
								dm_bubble[player_no].bubble_mini.bubble_pos[2] = dm_bubble[player_no].bubble_mini.bubble_pos[1];	//	���������ꍇ�̔�����łx���W
								dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 1;										//	�\���t���O�𗧂Ă�
							}
						}
						j++;
						x += 32;
					}
				}
			}
		}
	}
}

/*--------------------------------------
	�o�u�� �������C���֐�
--------------------------------------*/
void	bubble_main( s8 player_no )
{
	s8	i,j;

	if( dm_bubble[player_no].bubble_disp_flg){
		//	�\����
		for( i = 0;i < 3;i++ ){
			dm_bubble[player_no].bubble[i].bubble_size[bubble_s] --;			//	�ړ��l�̌���

			dm_bubble[player_no].bubble[i].bubble_pos[1] -= dm_bubble[player_no].bubble[i].bubble_size[bubble_s];			//	�x���W����ړ��l������
			if( dm_bubble[player_no].bubble[i].bubble_pos[1] <= -dm_bubble[player_no].bubble[i].bubble_size[bubble_h] ){	//	�o�u������ʊO�ɏ������ꍇ( �㏸ )
				//	�\���t���O������
				dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 0;
			}else	if( dm_bubble[player_no].bubble[i].bubble_pos[1] >= dm_bubble[player_no].bubble[i].bubble_pos[2] ){		//	���������ꍇ
				//	�\���t���O������
				dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] = 0;
			}
		}
		//	���̈ړ�
		dm_bubble[player_no].bubble_mini.bubble_size[bubble_s] --;
		dm_bubble[player_no].bubble_mini.bubble_pos[1] -= dm_bubble[player_no].bubble_mini.bubble_size[bubble_s];
		if( dm_bubble[player_no].bubble_mini.bubble_pos[1] > dm_bubble[player_no].bubble_mini.bubble_pos[2] ){		//	���������ꍇ
			dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] = 0;											//	�\���t���O������
		}

		//	�\���t���O�������Ă��Ȃ������ׂ�
		for( i = j = 0;i < 3;i++ ){
			if( dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] ){
				j ++;
			}
		}
		if( j == 0 ){	//	�R�Ƃ��\���t���O�������Ă��Ȃ�������
			//	�\���I��
			dm_bubble[player_no].bubble_disp_flg = 0;

		}
	}
}

/*--------------------------------------
	�o�u�� �`�惁�C���֐�
--------------------------------------*/
void	bubble_draw( game_state *state , s8 player_no )
{
	s8	i,j,k;
	s8	size,g_no,flip;

	gDPSetScissor( gp++, G_SC_NON_INTERLACE,0,0, 319, dm_bubble[player_no].scis_y - 1 );	//	�`��͈͎w��
	gDPSetRenderMode( gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2 );	//	�������w��
	gDPSetPrimColor(gp++,0,0,255,255,255,200);					//	�����x�ݒ�

	if( dm_bubble[player_no].bubble_disp_flg ){
		//	�\����

		for( i = 0;i < 3;i++ ){
			if( dm_bubble[player_no].bubble[i].bubble_flg[bubble_disp] ){
				flip = 0;
				if( dm_bubble[player_no].bubble[i].bubble_size[bubble_s] < 0 ){
					flip = flip_on;	//	�����̏ꍇ
				}
				load_TexPal( bubble_pal[ dm_bubble[player_no].bubble[i].bubble_size[bubble_p] ] );		//	�p���b�g���[�h
				load_TexTile_4b( bubble_graphic[ dm_bubble[player_no].bubble[i].bubble_size[bubble_g] ],
									dm_bubble[player_no].bubble[i].bubble_size[bubble_w],dm_bubble[player_no].bubble[i].bubble_size[bubble_h],
									0,0,dm_bubble[player_no].bubble[i].bubble_size[bubble_w] - 1,dm_bubble[player_no].bubble[i].bubble_size[bubble_h] - 1 );		//	�e�N�X�`�����[�h
				draw_TexFlip( dm_bubble[player_no].bubble[i].bubble_pos[0] - ( dm_bubble[player_no].bubble[i].bubble_size[bubble_w] >> 1 ),dm_bubble[player_no].bubble[i].bubble_pos[1],
								dm_bubble[player_no].bubble[i].bubble_size[bubble_w],dm_bubble[player_no].bubble[i].bubble_size[bubble_h],0,0,flip_off,flip );
			}
		}
		if( dm_bubble[player_no].bubble_mini.bubble_flg[bubble_disp] ){
			load_TexPal( bubble_pal[ dm_bubble[player_no].bubble_mini.bubble_size[bubble_p] ] );		//	�p���b�g���[�h
			load_TexTile_4b( bubble_graphic[ dm_bubble[player_no].bubble_mini.bubble_size[bubble_g] ],
								dm_bubble[player_no].bubble_mini.bubble_size[bubble_w],dm_bubble[player_no].bubble_mini.bubble_size[bubble_h],
								0,0,dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] - 1,dm_bubble[player_no].bubble_mini.bubble_size[bubble_h] - 1 );		//	�e�N�X�`�����[�h
				draw_Tex( dm_bubble[player_no].bubble_mini.bubble_pos[0] - ( dm_bubble[player_no].bubble_mini.bubble_size[bubble_w] >> 1 ),dm_bubble[player_no].bubble_mini.bubble_pos[1],
						dm_bubble[player_no].bubble_mini.bubble_size[bubble_w],dm_bubble[player_no].bubble_mini.bubble_size[bubble_h],0,0 );
		}
	}

	//	�픚��ԕ`��
	if(  state -> game_condition[dm_mode_now] == dm_cnd_wait ){
		for( i = 4;i >= 0;i-- ){	//	�Ƃ肠�����S�܂�
#ifdef	DAMAGE_TYPE
			if( state -> cap_attack_work[i][0] != 0x0000 ){	//	�U�����󂯂Ă���
				for( j = k = 0;j < 16;j += 2 ){
					if( state -> cap_attack_work[i][0] & ( 0x03 << j ) ){
						k++;
					}
				}
				if( k == 2 ){
					size = 12;
					g_no = bubble_small;
				}else	if( k == 3 ){
					size = 16;
					g_no = bubble_mid;
				}else	{
					size = 20;
					g_no = bubble_big;
				}

				load_TexPal( bubble_pal[ state -> cap_attack_work[i][1] ] );		//	�p���b�g���[�h	�U���ݒ�҂̐F
				load_TexTile_4b( bubble_graphic[ g_no ],
									size,size,0,0,size - 1,size - 1);
				draw_TexFlip( state -> map_x + i * 20,30 - ( size >> 1 ),size,size,0,0, flip_off,flip_on);
			}
		}
#endif

	}
	//	���ɖ߂�
	gDPSetScissor( gp++, G_SC_NON_INTERLACE,0,0, 319, 239 );	//	�`��͈͎w��
	gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);				//	�����x�ݒ�
}
/*--------------------------------------
	push_any_key �������֐�
--------------------------------------*/
void	init_push_any_key(void)
{
	push_sin = 0;
	push_pos_y = 0;
}

/*--------------------------------------
	push_any_key �������C���֐�
--------------------------------------*/
void	push_any_key_main(void)
{
	push_pos_y =(s8)(sinf( DEGREE( push_sin ) ) * 4 );
	push_sin += 10;
	if( push_sin >= 360 ){
		push_sin -= 360;
	}
}
/*--------------------------------------
	push_any_key �`�惁�C���֐�
--------------------------------------*/
void	push_any_key_draw(s16 x_pos,s16 y_pos )
{
	load_TexPal( dm_push_any_key_bm0_0tlut );		//	�p���b�g���[�h
	load_TexBlock_4b( dm_push_any_key_bm0_0,64,7 );
	draw_Tex( x_pos,y_pos + push_pos_y,64,7,0,0 );
}
/*--------------------------------------
	�������֐�
--------------------------------------*/
void	init_effect_all(void)
{
	init_chaine();			//	�A������������
	init_lose();			//	LOSE ������
	init_win();				//	WIN ������
	init_draw();			//	DRAW ������
	init_game_over();		//	GAME OVER ������
	init_try_next_stage();	//	TRY NEXT STAGE ������
	init_shock();			//	SHOCK ������
	init_humming();			//	HUMMING	������
	init_bubble();			//	�A���Q����������
	init_push_any_key();	//	PUSH ANY KEY ������
}


