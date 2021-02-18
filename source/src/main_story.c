#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"
#include "util.h"

//#include "dm_code_data.h"
//#include "o_segment.h"
//#include "o_static.h"

#include "sound.h"
#include "font.h"
#include "msgwnd.h"

#include "comp.h"
#include "util.h"
#include "main_story.h"
#include "tex_func.h"
#include "game_etc.h"

#include "calc.h"
#include "lws.h"
#include "local.h"

/*------------------------------------------------
// main_story.c�Ŏg�p����q�n�l�A�h���X
extern	u8	_coffee01SegmentRomStart[], _coffee01SegmentRomEnd[];
extern	u8	_menu_bgSegmentRomStart[], _menu_bgSegmentRomEnd[];
extern	u8	_menu_bg2SegmentRomStart[], _menu_bg2SegmentRomEnd[];
extern	u8	_wakuSegmentRomStart[], _wakuSegmentRomEnd[];
extern	u8	_waku2SegmentRomStart[], _waku2SegmentRomEnd[];
extern	u8	_title_allSegmentRomStart[], _title_allSegmentRomEnd[];
extern	u8	_title_bmpSegmentRomStart[], _title_bmpSegmentRomEnd[];
u8	*storyRomData[][2] = {
	{ _coffee01SegmentRomStart, _coffee01SegmentRomEnd },
	{ _menu_bgSegmentRomStart, _menu_bgSegmentRomEnd },
	{ _menu_bg2SegmentRomStart, _menu_bg2SegmentRomEnd },
	{ _wakuSegmentRomStart, _wakuSegmentRomEnd },
	{ _waku2SegmentRomStart, _waku2SegmentRomEnd },
	{ _title_allSegmentRomStart, _title_allSegmentRomEnd },
	{ _title_bmpSegmentRomStart, _title_bmpSegmentRomEnd },
};
extern	u8	_story_bg01SegmentRomStart[], _story_bg01SegmentRomEnd[];	// �����ꓹ
extern	u8	_story_bg02SegmentRomStart[], _story_bg02SegmentRomEnd[];	// ���ނ�
extern	u8	_story_bg03SegmentRomStart[], _story_bg03SegmentRomEnd[];	// ��
extern	u8	_story_bg04SegmentRomStart[], _story_bg04SegmentRomEnd[];	// ��R
extern	u8	_story_bg05SegmentRomStart[], _story_bg05SegmentRomEnd[];	// �_
extern	u8	_story_bg07SegmentRomStart[], _story_bg07SegmentRomEnd[];	// �C��
extern	u8	_story_bg08SegmentRomStart[], _story_bg08SegmentRomEnd[];	// �C��
extern	u8	_story_bg09SegmentRomStart[], _story_bg09SegmentRomEnd[];	// �X
extern	u8	_story_bg10SegmentRomStart[], _story_bg10SegmentRomEnd[];	// �����
extern	u8	_story_bg11SegmentRomStart[], _story_bg11SegmentRomEnd[];	// �_�a
u8	*bgRomData[][2] = {
	{ NULL, NULL },
	{ _story_bg03SegmentRomStart, _story_bg03SegmentRomEnd },
	{ _story_bg01SegmentRomStart, _story_bg01SegmentRomEnd },
	{ _story_bg09SegmentRomStart, _story_bg09SegmentRomEnd },
	{ _story_bg02SegmentRomStart, _story_bg02SegmentRomEnd },
	{ _story_bg07SegmentRomStart, _story_bg07SegmentRomEnd },
	{ _story_bg05SegmentRomStart, _story_bg05SegmentRomEnd },
	{ _story_bg10SegmentRomStart, _story_bg10SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ NULL, NULL },
	{ NULL, NULL },
	{ _story_bg03SegmentRomStart, _story_bg03SegmentRomEnd },
	{ _story_bg01SegmentRomStart, _story_bg01SegmentRomEnd },
	{ _story_bg09SegmentRomStart, _story_bg09SegmentRomEnd },
	{ _story_bg02SegmentRomStart, _story_bg02SegmentRomEnd },
	{ _story_bg08SegmentRomStart, _story_bg08SegmentRomEnd },
	{ _story_bg04SegmentRomStart, _story_bg04SegmentRomEnd },
	{ _story_bg10SegmentRomStart, _story_bg10SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ NULL, NULL },
};
------------------------------------------------*/



#define	STORY_FF_SPEED	3		// �X�g�[���[�̑�����{��

// �Đ�����X�g�[���[�̔ԍ�
#define	LWS_TITLE		0		// �^�C�g��
#define	LWS_STORY_M01a	1		// �}���I&�����I 1
#define	LWS_STORY_M01b	2		// �}���I 2
#define	LWS_STORY_M02	3		// �}���I 2
#define	LWS_STORY_W02	4		// �����I 2
#define	LWS_STORY_M03	5		// �}���I 3
#define	LWS_STORY_W03	6		// �����I 3
#define	LWS_STORY_M04	7		// �}���I 4
#define	LWS_STORY_W04	8		// �����I 4
#define	LWS_STORY_M05	9		// �}���I 5
#define	LWS_STORY_W05	10		// �����I 5
#define	LWS_STORY_M06	12		// �}���I 6
#define	LWS_STORY_W06	11		// �����I 6
#define	LWS_STORY_M07	13		// �}���I 7
#define	LWS_STORY_W07	13		// �����I 7
#define	LWS_STORY_M08a	14		// �}���I 8a
#define	LWS_STORY_W08a	15		// �����I 8a
#define	LWS_STORY_M08b	16		// �}���I 8b
#define	LWS_STORY_W08b	17		// �����I 8b
#define	LWS_STORY_M09	18		// �}���I 9
#define	LWS_STORY_W09	19		// �����I 9
#define	LWS_STORY_ED_Ma	20		// �G���f�B���O �}���I
#define	LWS_STORY_ED_Mb	21		// �G���f�B���O �}���I
#define	LWS_STORY_ED_Mc	22		// �G���f�B���O �}���I
#define	LWS_STORY_ED_Wa	23		// �G���f�B���O �����I
#define	LWS_STORY_ED_Wb	24		// �G���f�B���O �����I
#define	LWS_STORY_ED_Wc	25		// �G���f�B���O �����I
#define	LWS_STORY_PA1	26		// �I�[�v�j���O
#define	LWS_STORY_PA2	27		// �I�[�v�j���O
#define	LWS_STORY_PB	28		// �I�[�v�j���O
#define	LWS_STORY_PC	29		// �I�[�v�j���O
#define	LWS_STORY_M07E	30		// �}���I 7 easy
#define	LWS_STORY_W07E	31		// �����I 7 easy
#define	LWS_STORY_M08E	32		// �}���I 8 easy
#define	LWS_STORY_W08E	32		// �����I 8 easy
#define	LWS_TITLE_2		33		// �^�C�g�� �[��
#define	LWS_EPISODE		34		// 
#define	LWS_BG_1		35		// �����ꓹ
#define	LWS_BG_2		36		// ���ނ�
#define	LWS_BG_3		37		// ��
#define	LWS_BG_4		38		// ��R
#define	LWS_BG_5		39		// �_
#define	LWS_BG_6		40		// �C��
#define	LWS_BG_7		41		// �C��
#define	LWS_BG_8		42		// �X
#define	LWS_BG_9		43		// �����
#define	LWS_BG_10		44		// �_�a

static	u8	bgLwsTbl[] = {
	3,
	LWS_BG_3,		// ��
	LWS_BG_1,		// �����ꓹ
	LWS_BG_8,		// �X
	LWS_BG_2,		// ���ނ�
	LWS_BG_6,		// �C��
	LWS_BG_5,		// �_
	LWS_BG_9,		// �����
	LWS_BG_10,		// �_�a
	LWS_BG_10,		// �_�a
	LWS_BG_10,		// �_�a
	0,
	0,
	LWS_BG_3,		// ��
	LWS_BG_1,		// �����ꓹ
	LWS_BG_8,		// �X
	LWS_BG_2,		// ���ނ�
	LWS_BG_7,		// �C��
	LWS_BG_4,		// ��R
	LWS_BG_9,		// �����
	LWS_BG_10,		// �_�a
	LWS_BG_10,		// �_�a
	LWS_BG_10,		// �_�a
	0,
};

void	init_objMtx(void);

extern	void	HardCopy(u16 *buff, u16 *vram);
extern	u32		framecont;

#undef	_TEST_TEST_

//-------------------------------------------------------------------------
// �J�[�e���̃e�N�X�`���[
static	Gfx aaaaaa_dl[] = {
	gsSPEndDisplayList(),
};
#include	"texture/story_bmp/curtain/curtain_alpha_00.txt"
#include	"texture/story_bmp/curtain/curtain_00.txt"
#include	"texture/story_bmp/title_bmp/changestar.txt"

//-------------------------------------------------------------------------
// �^�C�g��
static	int		title_time;
static	int		title_wait;
static	u32		title_data;
static	u32		title_bmp_data;
#if LOCAL==AMERICA
static	u32		title_bmp_tbl[] = {
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\ninlogo.gbi
0x000011d0,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_col.gbi
0x000025e0,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_i.gbi
};
#elif LOCAL==CHINA
static	u32		title_bmp_tbl[] = {
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\ninlogo.gbi
0x000011d0,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_col.gbi
0x000025e0,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_i.gbi
};
#elif LOCAL==JAPAN
static	u32		title_bmp_tbl[] = {
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\ninlogo.gbi
0x000011d0,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_col.gbi
0x00002820,  //C:\Ultra\kaz\dm64\texture\story_bmp\title_bmp\press_i.gbi
};
#endif


static	SCENE_DATA **lws_data;
static	SCENE_DATA *lws_scene;


//-------------------------------------------------------------------------
// �q�n�l�A�h���X
extern	u8	*storyRomData[][2];
extern	u8	*bgRomData[][2];
#define		ROM_ORG_END		0
#define		ROM_MENU_BG1	1
#define		ROM_MENU_BG2	2
#define		ROM_WAKU1		3
#define		ROM_WAKU2		4
#define		ROM_TITLE_ALL	5
#define		ROM_TITLE_BMP	6


//-------------------------------------------------------------------------
// �䎌���X�g
static		SMsgWnd	mess_st;
static		SMsgWnd	mess_roll_st;
static		u8		mess_heap_area[1024];
static		void	*mess_heap = (void *)mess_heap_area;
static		void	*mess_roll_heap;
#define		MESS_AREA_X		62
#define		MESS_AREA_Y		177
//
typedef struct {
	int		frame;
	char	*mes;
} ST_MESSAGE;
static		int		st_message_count;
#if LOCAL==JAPAN
#include	"main_story_j.mes"
#elif LOCAL==AMERICA
#include	"main_story.mes"
#elif LOCAL==CHINA
#include	"main_story_zh.mes"
#endif

static	ST_MESSAGE *st_mes_ptr;



//-------------------------------------------------------------------------
int		story_proc_no = STORY_M_OPEN;		// �X�g�[���[�̏����i�X�e�[�W�j
char	end_dumm[] = "~0�s�d�r�s�@~w2~z";
char	*EndingLastMessage = end_dumm;		// �G���f�B���O�̍Ō�ɕ\�����郁�b�Z�[�W

//-------------------------------------------------------------------------
#define		MESWIN_OFF	0
#define		MESWIN_ON	16
static	int		loop_flg;
static	int		story_time_cnt = 0;
static	int		story_seq_step = 0;
static	int		story_zoom = 0;
static	int		story_curtain = CURTAIN_UP;
static	int		story_spot_cnt = 0;
static	int		story_kay_wait = FALSE;				// ���b�Z�[�W�����҂��t���O
static	int		story_message_on = FALSE;			// ���b�Z���E�C���h�E�\���t���O
static	int		story_message_start = FALSE;		// ���b�Z�[�W�\���J�n�t���O
static	int		story_doing = TRUE;
static	int		story_staff_roll;
static	int		bgtime = 0;
static	int		story_bg_no = 0;
static	int		mes_time = 0;

#define	SROLL_BUFF_SIZE	0x00010000
static	u32		story_read_buf = (u32)&gfx_freebuf[0][0];
static	u32		story_buffer = (u32)&gfx_freebuf[0][0] + SROLL_BUFF_SIZE;
static	u32		story_z_buffer = (u32)&gfx_freebuf[0][0];

#define	ST_MAX_OBJ	80
static	u16		story_norm;
static	Mtx		story_viewMtx;
static	Mtx		story_objectMtx[2][ST_MAX_OBJ];
static	int		objMtx_FF = 0;
Mtx		*pObjectMtx;

static	u32		wakuGraphic;
static	u32		wakuGraphic_ofs[] = {
0x00000000,  //C:\Ultra\kaz\dm64\texture\story_bmp\waku\main_board.gbi
0x00003410,  //C:\Ultra\kaz\dm64\texture\story_bmp\waku\main_board_alpha.gbi
0x000042a0,  //C:\Ultra\kaz\dm64\texture\story_bmp\waku\main_board_b.gbi
};
//
static	u32		charaGraphic[20];
static	u32		bgGraphic;
static	u32		storyGraphic;
static	u32		messageData;

static	Vp	vp = {
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,
	SCREEN_WD*2, SCREEN_HT*2, G_MAXZ/2, 0,
};
static	Gfx story_setup[] = {
	gsDPPipeSync(),

	gsSPViewport(&vp),

	gsDPSetBlendColor(0, 0, 0, 4),

//	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1),
	gsDPSetScissor(G_SC_NON_INTERLACE, 60, 37, 60+200, 37+100),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsDPPipelineMode(G_PM_1PRIMITIVE),
//	gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
	gsDPSetRenderMode(G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2),
	gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

	gsSPClearGeometryMode(G_SHADING_SMOOTH | G_CULL_BOTH | G_ZBUFFER |
							G_FOG | G_LIGHTING | G_TEXTURE_GEN |
							G_TEXTURE_GEN_LINEAR | G_LOD ),
	gsSPSetGeometryMode( G_SHADE | G_CULL_BACK ),
	gsSPTexture(0, 0, 0, 0, G_OFF),

	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTexturePersp(G_TP_NONE),
//	gsDPSetTexturePersp(G_TP_PERSP),

	gsDPPipeSync(),
	gsSPEndDisplayList(),
};
//static	u16	story_hcopy[128*96];
static	int	first_copy;


//-------------------------------------------------------------------------
// �m�����e������
// input:
//	int	count		ZOOM_EFF_0 --> ZOOM_EFF_MAX
void	story_zoomfade(Gfx **glp, int count)
{
	Gfx		*pgfx;
	int		i, j;
	u16		*vram_addr;
	float	xp, yp, xs, ys, w;

	pgfx = *glp;

//	vram_addr = (u16 *)osViGetCurrentFramebuffer();
	vram_addr = &fbuffer[wb_flag^1][0];

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gDPSetTextureLUT(pgfx++, G_TT_NONE);
	gDPSetTextureFilter(pgfx++, G_TF_BILERP);
	gDPSetEnvColor(pgfx++, 255, 255, 255, 255);

	gDPSetRenderMode(pgfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
	gDPSetCombineLERP(pgfx++, TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, 0,0,0,1, TEXEL0,ENVIRONMENT,PRIMITIVE,ENVIRONMENT, 0,0,0,1);

	// ���~�b�^�|
	if ( count < ZOOM_EFF_0 ) count = ZOOM_EFF_0;
	if ( count > ZOOM_EFF_MAX ) count = ZOOM_EFF_MAX;

	w = (float)count;
	w /= ZOOM_EFF_MAX;		// �J�E���^�̐��K��

	if ( w > 0.75 ) {
		xp = 255 * 0.75 / 6.0;
		yp = ( w - 0.75 ) / ( 1.0 - 0.75 );
		i = (int)( 255 - ( ( 255 - xp ) * yp ) - xp );
		if ( i < 0 ) i = 0;
	} else {
		i = 255 - (int)( 255 * w / 6.0 );
	}
	gDPSetPrimColor(pgfx++, 0, 0, i, i, i, 255);

	xp = 1.5 * sinf(w*6);
	yp = 1.5 * cosf(w*6);
	w = 1.0 + w * 2.0;		// �{��
	xp *= w; yp *= w;

	w = 1.0 + 1.0 / 16.0;
	xs = 320 * w;
	ys = 240 * w;
	xp += ( 320.0 - xs ) / 2.0;
	yp += ( 240.0 - ys ) / 2.0;
	StretchTexTile16(&pgfx,
		320, 240, (void *)vram_addr,
		0, 0, 320, 240,
		xp, yp, xs, ys);

	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineLERP(pgfx++, 0,0,0,TEXEL0, 0,0,0,PRIMITIVE, 0,0,0,TEXEL0, 0,0,0,PRIMITIVE);
	gDPSetPrimColor(pgfx++, 0, 0, 128, 128, 128, 50);

	StretchTexTile16(&pgfx,
		320, 240, (void *)vram_addr,
		0, 0, 320, 240,
		0, 0, 320, 240);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
void	get_gbi_stat(GBI_DATA *gbi, u32 gbi_addr)
{
	gbi->x = *( (u16 *)( gbi_addr + 8 ) );
	gbi->y = *( (u16 *)( gbi_addr + 10 ) );
	gbi->type = *( (u8 *)( gbi_addr + 14 ) );
	gbi->lut_addr = (u8 *)( gbi_addr + 16 );
	gbi->tex_addr = (u8 *)( gbi_addr + 16 + 512 );
}



//-------------------------------------------------------------------------
// �J�[�e���\������
//-------------------------------------------------------------------------
void curtain_proc(Gfx **glp, int count)
{
#define		CutrainOL		8
	Gfx		*pgfx;
	int		i, iy1, iy2;
	float	x, y, w, lim;

	pgfx = *glp;

	if ( count <= CURTAIN_UP ) return;
	if ( count > CURTAIN_DOWN ) count = CURTAIN_DOWN;

	w = (float)( count + 1 );
	w /= CURTAIN_DOWN;
	i = (int)( 255.0 * w + 0.5);
	if ( i > 255 ) i = 255;
	gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE );
	gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetPrimColor(pgfx++, 0, 0, 255, 255, 255, i);
	gDPFillRectangle(pgfx++, 0, 0, 320, 240);

	*glp = pgfx;
}
void curtain_proc_org(Gfx **glp, int count)
{
#define		CutrainOL		8
	Gfx		*pgfx;
	int		i, iy1, iy2;
	float	x, y, w, lim;

	pgfx = *glp;

	if ( count <= CURTAIN_UP ) return;
	if ( count > CURTAIN_DOWN ) count = CURTAIN_DOWN;

	gSPDisplayList(pgfx++, alpha_texture_init_dl);

	gDPLoadMultiBlock(pgfx++, curtain_00_tex, 0, G_TX_RENDERTILE+0, G_IM_FMT_RGBA, G_IM_SIZ_16b,
		32, 48, 0, G_TX_MIRROR, G_TX_CLAMP, 5, 0, 0, 0);
	gDPLoadMultiBlock_4b(pgfx++, curtain_alpha_00_tex, 384, G_TX_RENDERTILE+1, G_IM_FMT_I,
		32, 48, 0, G_TX_MIRROR, G_TX_CLAMP, 5, 0, 0, 0);

	w = (float)count;
	w /= CURTAIN_DOWN;
	x = 0;
	y = 0;
	for ( i = 0; i < 7; i++ ) {
		// �X���C�h��
		//y = (float)( count * (7-i) );
		y = w * (float)( (48-CutrainOL) * 7 ) - 48.0 - (float)( i * CutrainOL );
		lim = (float)( (6-i) * (48-CutrainOL) );
		if ( y > lim ) y = lim;
		// �`��
		iy1 = (int)( y * 4.0 + 0.5 ) ;
		iy2 = iy1 + ( 48 << 2 );			// (int)( ( y + 48 ) * 4.0 + 0.5 );
		gSPTextureRectangle(pgfx++,
			0 , iy1, 320 << 2, iy2,
			G_TX_RENDERTILE,
			(int)(0*32) & 0xffff, (int)(0*32) & 0xffff,
			1 << 10, 1 << 10);
	}

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// �X�g�[���[��ʕ\���̘g
//-------------------------------------------------------------------------
void waku_disp(Gfx **glp, u32 dataAddr)
{
	Gfx		*pgfx;
	int		i;
	float	x, y, w;
	u32		addr;
	u8		*texAddr, *alpAddr, *lutAddr;

	pgfx = *glp;

	gSPDisplayList(pgfx++, normal_texture_init_dl);

	addr = dataAddr;
	lutAddr = (u8*)( addr + 16 );
	texAddr = (u8*)( addr + 512 + 16 );
	StretchTexBlock8(&pgfx,
		328, 240, lutAddr, texAddr,
		0, 0, 328, 240);

/*
	// ��
	addr = dataAddr + waku_ofs_tbl[1];
	lutAddr = (u8*)( addr + 16 );
	texAddr = (u8*)( addr + 512 + 16 );
	for ( i = 0; i < 8; i++ ) {
		y = (float)( i * 25 );
		StretchTexBlock4(&pgfx,
			16, 25, lutAddr, texAddr,
			0, y, 16, 25);
		StretchTexBlock4(&pgfx,
			16, 25, lutAddr, texAddr,
			320-16, y, 16, 25);
		lutAddr = NULL;
		texAddr = NULL;
	}

	// ��
	addr = dataAddr + waku_ofs_tbl[0];
	lutAddr = (u8*)( addr + 16 );
	texAddr = (u8*)( addr + 512 + 16 );
	StretchTexBlock4(&pgfx,
		400, 40, lutAddr, texAddr,
		0, 200, 400, 40);

	// ��
	addr = dataAddr + waku_ofs_tbl[2];
	texAddr = (u8*)( addr + 512 + 16 );
	addr = dataAddr + waku_ofs_tbl[3];
	alpAddr = (u8*)( addr + 512 + 16 );
	gSPDisplayList(pgfx++, alpha_texture_init_dl);
	StretchAlphaTexBlock(&pgfx,			// �o�͐�Gfx�o�b�t�@�ϐ��̃A�h���X
			320, 46,					// �A���C���������������e�N�X�`���̃T�C�Y
			texAddr, 432,				// �J���[�e�N�X�`���̃A�h���X�ƕ�
			alpAddr, 400,				// �A���t�@�e�N�X�`���̃A�h���X�ƕ�
			0, 0,						// �`����W
			320, 46);					// �`��͈�, ���̒l���w�肷��Ɣ��]�`��
*/
	*glp = pgfx;
}



//-------------------------------------------------------------------------
// �w�i�\������
//-------------------------------------------------------------------------
void *story_bg_init(int stage, u32 buffAddr)
{
	u32	nextAddr;

	// �w�i�̓ǂݍ���
	bgGraphic = (u32)buffAddr;
	nextAddr = ExpandGZip((void *)bgRomData[stage][0] ,(void*)bgGraphic, ( (u32)bgRomData[stage][1] - (u32)bgRomData[stage][0] ) );
//	nextAddr = ExpandGZip((void *)storyRomData[ROM_TITLE_ALL][0], (void*)bgGraphic, ( (u32)storyRomData[ROM_TITLE_ALL][1] - (u32)storyRomData[ROM_TITLE_ALL][0] ));
	nextAddr = ( nextAddr + 15 ) & 0xfffffff0;

	// �g�̓ǂݍ���
	wakuGraphic = nextAddr;
	nextAddr = ExpandGZip((void *)storyRomData[ROM_WAKU2][0], (void*)wakuGraphic, ( (u32)storyRomData[ROM_WAKU2][1] - (u32)storyRomData[ROM_WAKU2][0] ));
	nextAddr = ( nextAddr + 15 ) & 0xfffffff0;

//	bgtime = 0;
//	story_bg_no = stage;
//	lws_data = (SCENE_DATA **)bgGraphic;
//	lws_scene = (SCENE_DATA *)( ( (u32)lws_data[ (int)bgLwsTbl[story_bg_no] ] & 0x00ffffff ) + bgGraphic );

	return((void *)nextAddr);
}



//-------------------------------------------------------------------------
// �w�i�\������
//-------------------------------------------------------------------------
void story_bg_proc(Gfx **glp)
{
	Gfx			*pgfx;
	GBI_DATA	gbi, gbia;
	u8			*texAddr, *lutAddr;
	u16			xs, ys;
	LMatrix 	pm;

	pgfx = *glp;

	// �w�i�̕`��
	// BG
	get_gbi_stat(&gbi, bgGraphic);
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	StretchTexBlock8(&pgfx,
		gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
		0, 0, (float)gbi.x, (float)gbi.y);
/*
	gSPSegment(pgfx++, ST_SEGMENT, osVirtualToPhysical((void *)bgGraphic));
	gSPMatrix(pgfx++, OS_K0_TO_PHYSICAL(&story_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);
	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);
	if ( story_bg_no == STORY_M_ST3 || story_bg_no == STORY_W_ST3 ) {
		gDPSetEnvColor(gp++, 170, 150, 185, 255);
	} else {
		gDPSetEnvColor(pgfx++, 255, 255, 255, 255);
	}
	makeTransrateMatrix(pm, 0 * FIXPOINT, -25 * FIXPOINT, -1900 * FIXPOINT);
	if ( lws_anim(&pgfx, pm, lws_scene, bgtime, bgGraphic) == TRUE ) {
		bgtime = 0;
	} else {
		bgtime++;
	}
*/

	// �g�P
	get_gbi_stat(&gbi, wakuGraphic + wakuGraphic_ofs[0]);
	get_gbi_stat(&gbia, wakuGraphic + wakuGraphic_ofs[1]);
	gSPDisplayList(pgfx++, alpha_texture_init_dl);
	StretchAlphaTexTile(&pgfx,		// �o�͐�Gfx�o�b�t�@�ϐ��̃A�h���X
		gbi.x, gbi.y,				// �A���C���������������e�N�X�`���̃T�C�Y
		gbi.tex_addr, gbi.x,		// �J���[�e�N�X�`���̃A�h���X�ƕ�
		gbia.tex_addr, gbia.x,		// �A���t�@�e�N�X�`���̃A�h���X�ƕ�
		0, 0, gbi.x, gbi.y,
		0, 140, (float)gbi.x, (float)gbi.y);				// �`��͈�, ���̒l���w�肷��Ɣ��]�`��
	// �g�Q
	get_gbi_stat(&gbi, wakuGraphic + wakuGraphic_ofs[2]);
	gSPDisplayList(pgfx++, normal_texture_init_dl);
	StretchTexTile8(&pgfx,
		gbi.x, gbi.y, gbi.lut_addr, gbi.tex_addr,
		0, 0, gbi.x, gbi.y,
		0, 160, (float)gbi.x, (float)gbi.y);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// �X�e�[�W�̎n�܂�
//-------------------------------------------------------------------------
static	void	story_st_start(void)
{
	// �J�[�e�����グ��
	if ( story_curtain == CURTAIN_UP ) {
			story_seq_step++;
			story_time_cnt = 0;
			story_message_on = TRUE;
			story_message_start = FALSE;
	} else {
		// �J�[�e���t�o
		story_message_on = FALSE;
		story_curtain--;
	}
}



//-------------------------------------------------------------------------
// �X�e�[�W�̏I���
//-------------------------------------------------------------------------
static	void	story_st_end(void)
{
	story_zoom++;
	story_doing = FALSE;
}



//-------------------------------------------------------------------------
// ���b�Z�[�W�����҂�
//-------------------------------------------------------------------------
static	void	story_st_clear(void)
{
	int	i;

	msgWnd_clear(&mess_st);
	story_kay_wait = FALSE;
	story_message_start = FALSE;
	story_seq_step++;
	story_time_cnt = 0;
	story_message_start = FALSE;
}


static	int	story_st_meswait(void)
{
	int	r;

	r = msgWnd_isEnd(&mess_st);
	if ( r != 0 ) {
		// ���b�Z�[�W�����҂�
		story_st_clear();
	}

	return(r);
}



//-------------------------------------------------------------------------
// �X�|�b�g���C�g����
//		texAddr �� 64 * 64 I4
//		count : 0 �^���� �` 255
//-------------------------------------------------------------------------
static	void	story_spot(Gfx **glp, int x, int y, int count, u8 *texAddr)
{
	GBI_DATA	gbi;
	Gfx			*pgfx;
	float	cx, cy;
	float	fx, fy;
	float	sx, sy, s;
	int		x1, x2, y1, y2;

	pgfx = *glp;

	if ( count >= 255 ) return;
	if ( count < 0 ) count = 0;

	s = (float)count / 18.0;
	cx = (float)x;
	cy = (float)y;

	sx = 64.0 * s;
	sy = 64.0 * s;
	fx = cx - sx / 2.0;
	fy = cy - sy / 2.0;

	x1 = (int)( fx + 1 );
	x2 = (int)( fx + sx - 1 );
	y1 = (int)( fy + 1 );
	y2 = (int)( fy + sy - 1 );

	gDPSetTextureLUT(pgfx++, G_TT_NONE);
	gDPSetPrimColor(pgfx++, 0, 0, 0, 0, 0, 255);
	gDPSetRenderMode(pgfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
	gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE );

	if ( sx < 1 || sy < 1 ) count = 0;
	if ( count == 0 ) {
		gDPFillRectangle(pgfx++, 0, 0, 320, 240);
	} else {
		if ( x1 > 0   ) gDPFillRectangle(pgfx++, 0, 0, x1, 240);
		if ( x2 < 320 ) gDPFillRectangle(pgfx++, x2, 0, 320, 240);
		if ( y1 > 0   ) gDPFillRectangle(pgfx++, 0, 0, 320, y1);
		if ( y2 < 240 ) gDPFillRectangle(pgfx++, 0, y2, 320, 240);
		gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE,   0,0,0,TEXEL0,   0,0,0,PRIMITIVE,   0,0,0,TEXEL0);
		StretchTexBlock4i(&pgfx,
			64, 64, texAddr,
			fx, fy, sx, sy);
	}
	gSPDisplayList(pgfx++, normal_texture_init_dl);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// ���^�X�|�b�g���C�g����
//		count : 0 �^���� �` 255
//-------------------------------------------------------------------------
void	star_spot(Gfx **glp, int x, int y, int count)
{
	story_spot(glp, x, y, count, changestar_tex);
}



//-------------------------------------------------------------------------
// �R�[�q�[�u���C�N�̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int type		�f���̎��	0:EASY 1:MED 2:HARD
//-------------------------------------------------------------------------
u32	init_coffee_break(u32 buffer, int type)
{
	u32	next;

	bgGraphic = ( buffer + 15 ) & 0xfffffff0;
	next = ExpandGZip((void *)storyRomData[0][0], (void*)bgGraphic, ( (u32)storyRomData[0][1] - (u32)storyRomData[0][0] ));
	next = ( next + 15 ) & 0xfffffff0;

	init_coffee_break_cnt();

	return(next);
}



//-------------------------------------------------------------------------
// �R�[�q�[�u���C�N�̓����J�E���^�[������
//-------------------------------------------------------------------------
void	init_coffee_break_cnt(void)
{
	guOrtho(&story_viewMtx,
		-(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
		-(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
		1.0, 2000.0, 1.0);

	bgtime = 0;
	mes_time = 350;
	story_time_cnt = -89;
	story_seq_step = 0;
}


//-------------------------------------------------------------------------
// �R�[�q�[�u���C�N�̕\��
// type      0:Low, 1:Med, 2:Hi
// mode      FALSE=Lv21  TRUE=Lv24
// disp_flg  FALSE=���b�Z�[�W�n����  TRUE=���b�Z�[�W�n��
//-------------------------------------------------------------------------
void draw_coffee_break(Gfx **glp, int type, int mode, int disp_flg)
{
	Gfx			*pgfx;
	LMatrix 	pm;
	SCENE_DATA	*scn_dat, *scn_dat2;
	int			flg;

	init_objMtx();

	pgfx = *glp;

	gSPSegment(pgfx++, 0, 0x0);
	gSPSegment(pgfx++, ST_SEGMENT, osVirtualToPhysical((void *)bgGraphic));

	gSPMatrix(pgfx++, OS_K0_TO_PHYSICAL(&story_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);
	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	lws_data = (SCENE_DATA **)bgGraphic;
	if ( mode ) {
		// �[��
		gDPSetEnvColor(pgfx++, 183, 127,  95, 255);
		scn_dat = (SCENE_DATA *)( ( (u32)lws_data[1] & 0x00ffffff ) + bgGraphic );
		scn_dat2 = (SCENE_DATA *)( ( (u32)lws_data[6] & 0x00ffffff ) + bgGraphic );
	} else {
		// ����
		gDPSetEnvColor(pgfx++, 255, 255, 255, 255);
		scn_dat = (SCENE_DATA *)( ( (u32)lws_data[0] & 0x00ffffff ) + bgGraphic );
		scn_dat2 = (SCENE_DATA *)( ( (u32)lws_data[5] & 0x00ffffff ) + bgGraphic );
	}

	makeTransrateMatrix(pm, 0 * FIXPOINT, -120 * FIXPOINT, -1900 * FIXPOINT);

	// �w�i
	if ( lws_anim(&pgfx, pm, scn_dat, bgtime, bgGraphic) == TRUE ) {
		bgtime = 50 - 1;
	}
	bgtime++;

	// �}���I�f��
	switch ( story_seq_step ) {
		case 0:
			scn_dat = (SCENE_DATA *)( ( (u32)lws_data[2] & 0x00ffffff ) + bgGraphic );
			break;
		case 1:
			scn_dat = (SCENE_DATA *)( ( (u32)lws_data[3] & 0x00ffffff ) + bgGraphic );
			break;
		case 2:
			scn_dat = (SCENE_DATA *)( ( (u32)lws_data[4] & 0x00ffffff ) + bgGraphic );
			break;
		default:
			scn_dat = NULL;
			story_time_cnt = -1;
			break;
	}
	flg = 0;
	if ( story_time_cnt >= 0 && scn_dat != NULL ) {
		if ( lws_anim(&pgfx, pm, scn_dat, story_time_cnt, bgGraphic) == TRUE ) {
			story_time_cnt = -1;
			story_seq_step++;
			flg = 1;
		}
	}
	story_time_cnt++;
	if ( flg ) {
		switch ( type ) {
			case 0:
				story_seq_step = 3;
				break;
			case 1:
				if ( story_seq_step == 1 ) {
					story_time_cnt = -60;
				} else {
					story_seq_step = 3;
				}
				break;
			default:
				if ( story_seq_step == 1 ) {
					story_time_cnt = -60;
				} else if ( story_seq_step == 2 ) {
					story_time_cnt = -40;
				} else {
					story_seq_step = 3;
				}
				break;
		}
	}

	// ���߂łƂ��p�l��
	if ( disp_flg ) {
		if ( lws_anim(&pgfx, pm, scn_dat2, mes_time, bgGraphic) == TRUE ) {
			mes_time = 650;
		} else {
			mes_time++;
		}
	}

	// �㉺�̃g���~���O
	gDPSetCycleType(pgfx++,  G_CYC_1CYCLE);
	gDPSetRenderMode(pgfx++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
	gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE, 0,0,0,PRIMITIVE );
	gDPSetPrimColor(pgfx++, 0, 0, 0, 0, 0, 255);
	gDPFillRectangle(pgfx++, 0, 0, 320, 32);
	gDPFillRectangle(pgfx++, 0, 240-32, 320, 240);

	gSPDisplayList(pgfx++, normal_texture_init_dl);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// ���j���[�w�i�̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int flg			�w�i�̒��A�[�t���O  ��:0�A�[:1
//-------------------------------------------------------------------------
u32	init_menu_bg(u32 buffer, int flg)
{
	u32	next;

	bgtime = 0;

	guOrtho(&story_viewMtx,
		-(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
		-(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
		1.0, 2000.0, 1.0);

	bgGraphic = ( buffer + 15 ) & 0xfffffff0;
	if ( flg ) {
		next = ExpandGZip((void *)storyRomData[2][0], (void*)bgGraphic, ( (u32)storyRomData[2][1] - (u32)storyRomData[2][0] ));
	} else {
		next = ExpandGZip((void *)storyRomData[1][0], (void*)bgGraphic, ( (u32)storyRomData[1][1] - (u32)storyRomData[1][0] ));
	}
	next = ( next + 15 ) & 0xfffffff0;

	return(next);
}



//-------------------------------------------------------------------------
// ���j���[�w�i�̕\��
//-------------------------------------------------------------------------
void draw_menu_bg(Gfx **glp, int ofsx, int ofsy)
{
	Gfx			*pgfx;
	LMatrix 	pm;

	init_objMtx();

	pgfx = *glp;

	gSPSegment(pgfx++, 0, 0x0);
	gSPSegment(pgfx++, ST_SEGMENT, osVirtualToPhysical((void *)bgGraphic));

	gSPMatrix(pgfx++, OS_K0_TO_PHYSICAL(&story_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);
	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	makeTransrateMatrix(pm, ofsx * FIXPOINT, ofsy * FIXPOINT, -1900 * FIXPOINT);

	if ( lws_anim(&pgfx, pm, (SCENE_DATA *)bgGraphic, bgtime, bgGraphic) == TRUE ) {
		bgtime = 0;
	} else {
		bgtime++;
	}

	gSPDisplayList(pgfx++, normal_texture_init_dl);

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// �^�C�g���̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int flg			TRUE=���߂���\��	 FALSE=�����Ȃ�\���i���C�����j���[����߂����ꍇ�j
//-------------------------------------------------------------------------
u32	init_title(u32 buffer, int flg)
{
	u32	next;

	if ( flg ) {
		title_time = 0;
		story_spot_cnt = -100;
	} else {
		title_time = 720;
		story_spot_cnt = 256;
	}
	title_wait = 0;

	guOrtho(&story_viewMtx,
		-(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
		-(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
		1.0, 2000.0, 1.0);

	title_data = ( buffer + 15 ) & 0xfffffff0;
	next = ExpandGZip((void *)storyRomData[ROM_TITLE_ALL][0], (void*)title_data, ( (u32)storyRomData[ROM_TITLE_ALL][1] - (u32)storyRomData[ROM_TITLE_ALL][0] ));
	next = ( next + 15 ) & 0xfffffff0;

	title_bmp_data = next;
	next = ExpandGZip((void *)storyRomData[ROM_TITLE_BMP][0], (void*)title_bmp_data, ( (u32)storyRomData[ROM_TITLE_BMP][1] - (u32)storyRomData[ROM_TITLE_BMP][0] ));
	next = ( next + 15 ) & 0xfffffff0;

	return(next);
}



//-------------------------------------------------------------------------
// �^�C�g���̕\��
//		int flg			���A�[�t���O  ��:0�A�[:1
//-------------------------------------------------------------------------
int demo_title(Gfx **glp, int flg21)
{
	Gfx			*pgfx;
	GBI_DATA	gbi, gbia;
	LMatrix 	pm;
	int			i, flg, ret, mask;

	init_objMtx();

	ret = 0;
	if ( title_time >= 720 ) flg = TRUE; else flg = FALSE;

	pgfx = *glp;

	gSPSegment(pgfx++, 0, 0x0);
	gSPSegment(pgfx++, ST_SEGMENT, osVirtualToPhysical((void *)title_data));

	gSPMatrix(pgfx++, OS_K0_TO_PHYSICAL(&story_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

	gSPDisplayList(pgfx++, normal_texture_init_dl);
	gSPDisplayList(pgfx++, story_setup);
	gDPSetScissor(pgfx++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);

	lws_data = (SCENE_DATA **)title_data;
	if ( flg21 ) {
		gDPSetEnvColor(pgfx++, 183, 127, 95, 255);
		lws_scene = (SCENE_DATA *)( ( (u32)lws_data[LWS_TITLE_2] & 0x00ffffff ) + title_data );
	} else {
		gDPSetEnvColor(pgfx++, 255, 255, 255, 255);
		lws_scene = (SCENE_DATA *)( ( (u32)lws_data[LWS_TITLE] & 0x00ffffff ) + title_data );
	}

	makeTransrateMatrix(pm, 0 * FIXPOINT, -120 * FIXPOINT, -1900 * FIXPOINT);

	// �����������甲����
	if ( story_spot_cnt > 0 ) {
		//for ( i = 0; i < 4; i++ ) {
			if ( joyupd[main_joy[0]] & ( DM_ANY_KEY ) ) {
				if ( flg ) {
					if ( title_wait == 0 ) {
						dm_snd_play(SE_mGameStart);
						title_wait = 1;
					}
				} else {
					title_time = 720;
				}
			}
		//}
	}
	// ������܂łɂP�b�҂�
	if ( title_wait > 0 ) {
		title_wait++;
		if ( title_wait >= 60 ) ret = 1;
	}

	if ( lws_anim(&pgfx, pm, lws_scene, title_time, title_data) == TRUE ) {
		ret = -1;
		//title_time = 720;
	}
	gSPDisplayList(pgfx++, normal_texture_init_dl);

	if ( story_spot_cnt > 0 ) title_time++;

	// ���^�X�|�b�g
	if ( story_spot_cnt < 256 ) {
		gSPDisplayList(pgfx++, normal_texture_init_dl);
		story_spot(&pgfx, 160, 120, story_spot_cnt, changestar_tex);
		if ( story_spot_cnt < 0 ) {
			// NINTENDO Logo
			i = 255;
			if ( story_spot_cnt > -24 ) {
				i = ( -255 * story_spot_cnt ) / 24;
				if ( i > 255 ) i = 255;
				if ( i < 0 ) i = 0;
			}
			if ( story_spot_cnt < -76 ) {
				i = 255 - ( -255 * (story_spot_cnt+76) ) / 24;
				if ( i > 255 ) i = 255;
				if ( i < 0 ) i = 0;
			}
			get_gbi_stat(&gbi, title_bmp_data + title_bmp_tbl[0]);
			gDPSetTextureLUT(pgfx++, G_TT_NONE);
			gDPSetPrimColor(pgfx++, 0, 0, i, i, i, 255);
			gDPSetRenderMode(pgfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
			gDPSetCombineLERP(pgfx++, 0,0,0,PRIMITIVE,   0,0,0,TEXEL0,   0,0,0,PRIMITIVE,   0,0,0,TEXEL0);
			StretchTexBlock4i(&pgfx,
				gbi.x, gbi.y, gbi.tex_addr,
				( 160 - gbi.x/2 ), ( 120 - gbi.y/2 ), gbi.x, gbi.y);
			story_spot_cnt += 1;
		} else {
			story_spot_cnt += 5;
		}
	}

	// �o�������r��������
	if ( title_wait == 0 ) mask = 0x0010; else mask = 0x01;
	if ( ( title_time & mask ) && ( title_time > 720 ) && ( title_wait == 0 ) ) {
//	if ( ( title_time & mask ) && ( title_time > 720 ) ) {
		get_gbi_stat(&gbi, title_bmp_data + title_bmp_tbl[1]);
		get_gbi_stat(&gbia, title_bmp_data + title_bmp_tbl[2]);
		gSPDisplayList(pgfx++, alpha_texture_init_dl);
		StretchAlphaTexBlock(&pgfx,			// �o�͐�Gfx�o�b�t�@�ϐ��̃A�h���X
				gbi.x, gbi.y,				// �A���C���������������e�N�X�`���̃T�C�Y
				gbi.tex_addr, gbi.x,		// �J���[�e�N�X�`���̃A�h���X�ƕ�
				gbia.tex_addr, gbia.x,		// �A���t�@�e�N�X�`���̃A�h���X�ƕ�
				88, 165,					// �`����W
				gbi.x, gbi.y);				// �`��͈�, ���̒l���w�肷��Ɣ��]�`��
	}


/* debug */
//	if ( joyupd[main_joy[0]] & CONT_C )
//		add_attack_effect(0 , 40, 180, 100, 150);
//	if ( joyupd[main_joy[0]] & CONT_D )
//		add_attack_effect(1 , 250, 180, 100, 150);
//	if ( joyupd[main_joy[0]] & CONT_E )
//		add_attack_effect(2 , 140, 150, 200, 180);
//	if ( joyupd[main_joy[0]] & CONT_F )
//		add_attack_effect(3 , 50, 180, 250, 50);
//	disp_attack_effect(&pgfx);


	*glp = pgfx;

	return(ret);
}



//-------------------------------------------------------------------------
// �n�o
//-------------------------------------------------------------------------
void	main_open(Gfx **glp, int next)
{
	Gfx			*pgfx;
	GBI_DATA	gbi, gbia;
	LMatrix 	pm;
	int			i, flg, ret;

	pgfx = *glp;

	*glp = pgfx;
}



//-------------------------------------------------------------------------
// �}���I�̃X�g�[���[
//-------------------------------------------------------------------------
#include	"story_mario.h"



//-------------------------------------------------------------------------
// �����I�̃X�g�[���[
//-------------------------------------------------------------------------
#include	"story_wario.h"



//-------------------------------------------------------------------------
// �f�[�^�̓ǂݍ���
//-------------------------------------------------------------------------
static	void	read_graphic_data()
{
	int		n, i;
	u32		addr;

	addr = story_buffer;

	// �w�i�X�g�[���[
	storyGraphic = addr;
	addr = story_buffer;
	addr = ExpandGZip((void *)storyRomData[ROM_TITLE_ALL][0], (void*)addr, ( (u32)storyRomData[ROM_TITLE_ALL][1] - (u32)storyRomData[ROM_TITLE_ALL][0] ));
	addr = ( addr + 15 ) & 0xfffffff0;

	// �g
	wakuGraphic = addr;
	addr = ExpandGZip((void *)storyRomData[ROM_WAKU1][0], (void*)wakuGraphic, ( (u32)storyRomData[ROM_WAKU1][1] - (u32)storyRomData[ROM_WAKU1][0] ));
	addr = ( addr + 15 ) & 0xfffffff0;

	// �Ō�̂a�f
	bgGraphic = addr;
	addr = ExpandGZip((void *)storyRomData[1][0], (void*)bgGraphic, ( (u32)storyRomData[1][1] - (u32)storyRomData[1][0] ));
	addr = ( addr + 15 ) & 0xfffffff0;

	// �䎌
	messageData = addr;
}



//-------------------------------------------------------------------------
// ���C��
//-------------------------------------------------------------------------
void main_story(NNSched* sched)
{
	OSMesgQueue msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;

	int			i;

	static int snd_tbl[] = {
		SEQ_Opening,
		SEQ_Opening,	//	SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_C,
		SEQ_Story_B,
		SEQ_Ending,
		SEQ_Ending,

		SEQ_Opening,
		SEQ_Opening,	//	SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_B,
		SEQ_Story_A,
		SEQ_Story_C,
		SEQ_Story_B,
		SEQ_Ending,
		SEQ_Ending,
	};

	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	// create message queue for VI retrace
	nnScAddClient(sched, &client, &msgQ);			// add client to shceduler
	graphic_no = GFX_NULL;

	// ���������낢��
	story_z_buffer = (u32)&gfx_freebuf[0][0];
	guPerspective(&story_viewMtx, &story_norm,
		45.0, 4.0/3.0, 1.0, 1000.0, 1.0);
	guOrtho(&story_viewMtx,
		-(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
		-(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
		1.0, 2000.0, 1.0);
	first_copy = TRUE;

	for( i = 0; i < MAXCONTROLLERS; i++ ) joyflg[i] = ( DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT );
	story_read_buf = (u32)&gfx_freebuf[0][0];
	story_buffer = story_read_buf + SROLL_BUFF_SIZE;
	framecont = 0;
	story_time_cnt = 0;
	story_seq_step = 0;
	story_kay_wait = FALSE;
	story_curtain = CURTAIN_DOWN;
	story_zoom = ZOOM_EFF_0;
	story_message_on = FALSE;
	story_message_start = FALSE;
	story_doing = TRUE;
	story_staff_roll = 0;
	st_message_count = 0;
	loop_flg = TRUE;

	// ���b�Z�[�W�\���̏�����
	mess_heap = (void *)mess_heap_area;
#if LOCAL==JAPAN
	msgWnd_init(&mess_st, &mess_heap, 16, 3, MESS_AREA_X, MESS_AREA_Y);
#elif LOCAL==AMERICA
	msgWnd_init(&mess_st, &mess_heap, 16, 3, MESS_AREA_X + 4, MESS_AREA_Y);
#elif LOCAL==CHINA
	msgWnd_init(&mess_st, &mess_heap, 16, 3, MESS_AREA_X + 4, MESS_AREA_Y);
#endif
	mess_st.fntW = 12;
	mess_st.fntH = 12;
	mess_st.colStep = 6;
	mess_st.rowStep = 14;
	mess_st.msgSpeed = 1.0 / 2.5; //1.0 / 5.0;
//	mess_roll_heap = (void *)mess_heap_area2;
//	msgWnd_init(&mess_roll_st, &mess_roll_heap, 17, 7, 60+4+6, 37+3);
//	msgWnd_init(&mess_roll_st, &mess_roll_heap, 17, 11, 60+4+6, 36-14);
	mess_roll_heap = (void *)(&gfx_freebuf[0][0]); //story_read_buf
#if LOCAL==JAPAN
	msgWnd_init2(&mess_roll_st, &mess_roll_heap, sizeof(st_staffroll_txt) + 16, 20, 11, 60, 36-14);
#elif LOCAL==AMERICA
	msgWnd_init2(&mess_roll_st, &mess_roll_heap, sizeof(st_staffroll_txt) + 16, 20, 11, 40, 36-14);
	mess_roll_st.centering = 1;
#elif LOCAL==CHINA
	msgWnd_init2(&mess_roll_st, &mess_roll_heap, sizeof(st_staffroll_txt) + 16, 20, 11, 40, 36-14);
	mess_roll_st.centering = 1;
#endif
	mess_roll_st.fntW = 12;
	mess_roll_st.fntH = 12;
	mess_roll_st.colStep = 6;
	mess_roll_st.rowStep = 14;
	mess_roll_st.msgSpeed = 1.0 / 4.0;
	mess_roll_st.fontType = 1;

	// �q�n�l�ǂݍ���
	read_graphic_data();

	// ��
//	auSeqPlayerStop(0);
//	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.

	//	�O�̂���
	joyProcCore();

	// �V�[�P���X���Đ�
	dm_seq_play(snd_tbl[story_proc_no]);

	// ���C�����[�v
	while( loop_flg ) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.

		if ( GameHalt ) {
			joyProcCore();
			graphic_no = GFX_NULL;
			dm_audio_update();
			continue;
		}
		graphic_no = GFX_STORY;

		// �Ƃ肠���������������甲����
//		if ( joyupd[main_joy[0]] != 0 ) loop_flg = FALSE;

		// ���b�Z�[�W�̏���
		msgWnd_update(&mess_st);
		msgWnd_update(&mess_roll_st);

		if ( ( story_doing == FALSE ) && ( story_zoom >= ZOOM_EFF_MAX ) ) {
			// ���[�v�I��
			loop_flg = FALSE;
		}

		joyProcCore();

		// �I�[�f�B�I�R�}���h����
		dm_audio_update();
	}


	// �S�T�E���h���~
	dm_audio_stop();
	// graphics finish.
	graphic_no = GFX_NULL;
	// �O���t�B�b�N�ƃT�E���h�̏I���҂�
	while ( pendingGFX || !dm_audio_is_stopped() ) {
		osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
		// �I�[�f�B�I�R�}���h����
		dm_audio_update();
	}

	for(i = 0;i < 3;i++ ){
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	nnScRemoveClient(sched, &client);							// remove client to shceduler

#ifdef _TEST_TEST_
//	story_proc_no++;													//	�X�g�[���[�ԍ���i�߂鏈��
#endif
	evs_story_no = story_proc_no;										//	�X�g�[���[�ԍ���i�߂鏈��
	if ( evs_story_no >= STORY_W_OPEN ) evs_story_no -= STORY_W_OPEN;	//	�X�g�[���[�ԍ���i�߂鏈��

	//evs_story_no = (evs_story_no+1)%9;							

	evs_seqnumb = evs_story_no % 3;								//	�Q�[���̋Ȃ�ݒ肷��

	return;
}



//-------------------------------------------------------------------------
// �s��̏�����
//-------------------------------------------------------------------------
void	init_objMtx(void)
{
	pObjectMtx = &story_objectMtx[objMtx_FF][0];
	objMtx_FF ^= 1;
}


//-------------------------------------------------------------------------
// �O���t�B�b�N
//-------------------------------------------------------------------------
void graphic_story(void)
{
	NNScTask *t;
	int i;
	GBI_DATA 	gbi;

	// graphic display list buffer. task buffer.
	gp = &gfx_glist[gfx_gtask_no][0];
	t  = &gfx_task[gfx_gtask_no];

	init_objMtx();

	// RSP initialize.
	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, ST_SEGMENT, osVirtualToPhysical((void *)story_buffer));

	lws_data = (SCENE_DATA **)story_buffer;

	S2RDPinitRtn(TRUE);

	gDPPipeSync(gp++);
	gDPSetCycleType(gp++, G_CYC_FILL);

//	gDPSetColorImage(gp++, G_IM_FMT_RGBA, G_IM_SIZ_16b,SCREEN_WD,OS_K0_TO_PHYSICAL(story_z_buffer));
//	gDPSetFillColor(gp++, GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0));
//	gDPFillRectangle(gp++,  0, 0, SCREEN_WD-1, SCREEN_HT-1);
//	gDPSetDepthImage(gp++, OS_K0_TO_PHYSICAL(story_z_buffer));

	gDPSetCycleType(gp++,  G_CYC_1CYCLE);
	gDPSetEnvColor(gp++, 255, 255, 255, 255);

	S2ClearCFBRtn(TRUE);

//	gSPDisplayList(gp++, S2Spriteinit_dl);
//	gSPPerspNormalize(gp++, story_norm);
	gSPMatrix(gp++, OS_K0_TO_PHYSICAL(&story_viewMtx),
			G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

//	demo_title(&gp);

	if ( story_zoom > 0 ) {
		story_zoomfade(&gp, story_zoom);
		story_zoom++;
	} else {

	switch( story_proc_no ) {
		case STORY_M_OPEN:
			story_st_new(&gp, LWS_STORY_M01a, 0);
			break;
		case STORY_W_OPEN:
			story_st_new(&gp, LWS_STORY_M01a, 0);
			break;
		case STORY_M_END:
			story_m_end(&gp, LWS_STORY_ED_Ma, 27);
			break;
		case STORY_M_END2:
			story_m_end(&gp, LWS_STORY_ED_Mc, 28);
			break;
		case STORY_W_END:
			story_w_end(&gp, LWS_STORY_ED_Wa);
			break;
		case STORY_W_END2:
			story_w_end(&gp, LWS_STORY_ED_Wc);
			break;

		case STORY_M_ST1:
//			story_m_end(&gp, LWS_STORY_ED_M);
//			story_st_new(&gp, LWS_STORY_M01a, 0);
			story_st_new_op(&gp, 1);
			break;
		case STORY_M_ST2:
			story_st_new2(&gp, LWS_STORY_M01b, 5, LWS_STORY_M02, 7);
			break;
		case STORY_M_ST3:
			gDPSetEnvColor(gp++, 170, 150, 185, 255);
			story_st_new(&gp, LWS_STORY_M03, 9);
			break;
		case STORY_M_ST4:
			story_st_new2_f(&gp, LWS_EPISODE, 29, LWS_STORY_M04, 11);
//			story_st_new(&gp, LWS_STORY_M04, 11);
			break;
		case STORY_M_ST5:
			story_st_new(&gp, LWS_STORY_M05, 13);
			break;
		case STORY_M_ST6:
			story_st_new(&gp, LWS_STORY_M06, 15);
			break;
		case STORY_M_ST7:
			if ( evs_story_level == 0 ) {
				story_st_new(&gp, LWS_STORY_M07E, 25);
			} else {
				story_st_new(&gp, LWS_STORY_M07, 17);
			}
			break;
		case STORY_M_ST8:
			if ( evs_story_level == 0 ) {
				story_st_new2_f(&gp, LWS_STORY_M08E, 26, LWS_STORY_M08b, 19);
			} else {
				story_st_new2_f(&gp, LWS_STORY_M08a, 18, LWS_STORY_M08b, 19);
			}
			break;
		case STORY_M_ST9:
			story_st_new(&gp, LWS_STORY_M09, 20);
			break;

		case STORY_W_ST1:
//			story_w_end(&gp, LWS_STORY_ED_Wa);
//			story_w_end(&gp, LWS_STORY_ED_Wc);
//			story_st_new(&gp, LWS_STORY_M01a, 0);
			story_st_new_op(&gp, 0);
			break;
		case STORY_W_ST2:
			story_st_new2(&gp, LWS_STORY_M01b, 6, LWS_STORY_W02, 8);
			break;
		case STORY_W_ST3:
			gDPSetEnvColor(gp++, 170, 150, 185, 255);
			story_st_new(&gp, LWS_STORY_W03, 10);
			break;
		case STORY_W_ST4:
			story_st_new2_f(&gp, LWS_EPISODE, 29, LWS_STORY_W04, 12);
//			story_st_new(&gp, LWS_STORY_W04, 12);
			break;
		case STORY_W_ST5:
			story_st_new(&gp, LWS_STORY_W05, 14);
			break;
		case STORY_W_ST6:
			story_st_new(&gp, LWS_STORY_W06, 16);
			break;
		case STORY_W_ST7:
			if ( evs_story_level == 0 ) {
				story_st_new(&gp, LWS_STORY_W07E, 25);
			} else {
				story_st_new(&gp, LWS_STORY_W07, 17);
			}
			break;
		case STORY_W_ST8:
			if ( evs_story_level == 0 ) {
				story_st_new2_f(&gp, LWS_STORY_W08E, 26, LWS_STORY_W08b, 19);
			} else {
				story_st_new2_f(&gp, LWS_STORY_W08a, 18, LWS_STORY_W08b, 19);
			}
			break;
		case STORY_W_ST9:
			story_st_new_w9(&gp, LWS_STORY_W09, 21);
			break;
	}

	// ��
	curtain_proc_org(&gp, story_curtain);

	// �X�^�b�t���[��
	if ( story_staff_roll == 2 ) {
		msgWnd_draw(&mess_roll_st, &gp);
		if ( joyupd[main_joy[0]] & ( CONT_START ) ) story_staff_roll = -1;
		if ( joynew[main_joy[0]] & ( CONT_A | CONT_B ) ) {
			mess_roll_st.scrSpeed = 1.0 / 6.0;
		} else {
			mess_roll_st.scrSpeed = 1.0 / 60.0;
		}
	}

	// �g
	waku_disp(&gp, wakuGraphic);

	// ���b�Z�[�W�̕`��
	if ( story_message_on ) {
		msgWnd_draw(&mess_st, &gp);
		gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0, 0, 320-1, 240-1);
	}

	}

	// �X�^�b�t���[���I���
	if ( story_staff_roll < 0 ) {
		gSPDisplayList(gp++, normal_texture_init_dl);
		story_spot(&gp, 160, 120, story_spot_cnt, changestar_tex);
		story_spot_cnt -= 4;
		if ( story_spot_cnt < -20 ) {
			loop_flg = FALSE;
		}
	}

	// end of display list & go the graphic task.
	story_time_cnt++;

	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();
	gfxTaskStart(t, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_F3DEX, NN_SC_SWAPBUFFER);
}



//-------------------------------------------------------------------------
// �X�^�b�t���[����̃��b�Z�[�W���̔w�i
//-------------------------------------------------------------------------
void	draw_ending_mess_bg(Gfx **glp)
{
	draw_menu_bg(glp, 160, 120);
}
