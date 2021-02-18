#ifndef _STATIC_H_
#define _STATIC_H_

extern Gfx S2RDPinit_dl[];		// �e�평���� DL
extern Gfx S2ClearCFB_dl[];
extern Gfx S2Spriteinit_dl[];
extern Gfx F3SetupRSP_dl[];
extern Gfx F3RDPinit_dl[];
extern Gfx F3SetupRDP_dl[];

#if 0 // �폜
/*
extern Gfx prev_normal_ab_dl[];
extern Gfx prev_normal_aB_dl[];
extern Gfx prev_normal_Ab_dl[];
extern Gfx prev_normal_AB_dl[];
extern Gfx prev_halftrans_ab_dl[];
extern Gfx prev_halftrans_aB_dl[];
extern Gfx prev_halftrans_Ab_dl[];
extern Gfx prev_halftrans_AB_dl[];
extern Gfx prev_halftrans_AB_no_prim_dl[];
extern Gfx prev_mario_dl[];
*/
#endif

/*--------------------------------------
	create		:	1999/03/15
	modify		:	1999/10/29

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define PAI         3.141592654
#define DEGREE(a)   ((a) * PAI / 180)
#define flip_on     1
#define flip_off    0

extern Gfx ClearCfb[];	//	��ʂ̃N���A
extern Gfx ClearZbuff[];
extern Gfx Texture_NZ_dl[];
extern Gfx Texture_TE_dl[];	//	�s����
extern Gfx Intensity_XNZ_Texture_dl[];	//	�C���e���V�e�B�g�p�̑O����

extern void disp_rect(u8,u8,u8,u8,int,int,int,int);	//	�l�p�`��
extern void gfxRCPInit(void);
extern void gfxClearCfb(void);

extern void load_TexPal(void *);													//	�p���b�g�Ǎ���
extern void load_TexPal_16(void *,int);											//	�p���b�g�Ǎ���(16�F16�G���g��)
extern void load_TexBlock_4b(void *,int,int);										//	�e�N�X�`���Ǎ���16�F
extern void load_TexTile_4b(void *,int,int,int,int,int,int);						//	�e�N�X�`���Ǎ���16�F
extern void load_TexBlock_i(void *,int,int);										//	�e�N�X�`���Ǎ��݃C���e���V�e�B
extern void load_TexBlock_8b(void *,int,int);										//	�e�N�X�`���Ǎ���256�F
extern void load_TexTile_8b(void *,int,int,int,int,int,int);						//	�e�N�X�`���Ǎ���256�F
extern void draw_Tex(int,int,int,int,int,int);									//	�e�N�X�`���`��
extern void draw_Tex_Copy(int,int,int,int,int,int);								//	�e�N�X�`���`��(G_CYC_COPY�p)
extern void draw_ScaleTex(int,int,int,int,int,int,int,int);						//	�e�N�X�`���`��(�g��E�k������)
extern void draw_TexFlip(int,int,int,int,int,int,int,int);						//	�e�N�X�`���`��(���]����)
extern void draw_TexScaleFlip(int,int,int,int,int,int,int,int,int,int,int,int);	//	�e�N�X�`���`��(�g��E�k���E���]����)

#if 0 // �폜
/*
extern void S2d_ObjBg_InitTile(uObjBg *, void *,int,int,int,int,int,int,int,int,int,int);			//	ObjBg�^��ݒ肷��
*/
#endif

#endif
