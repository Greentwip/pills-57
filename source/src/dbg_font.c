
#include <ultra64.h>
#include <stdarg.h>
#include <ctype.h>
#include "io_file.h"
#include "dbg_font.h"

//////////////////////////////////////////////////////////////////////////////
// デバッグ用フォント

#define DBG_FONT_COL 26
#define DBG_FONT_ROW 6
#define DBG_FONT_TEX_W 160
#define DBG_FONT_TEX_H 48

#include "texture/debug/dbg_font.bft"

static char asc2dbg_tbl[] = {
	// 0x20 ~ 0x7f
	-1,-1,-1,-1,-1,66,63,76,-1,-1,-1,-1,125,62,126,75,
	52,53,54,55,56,57,58,59,60,61,65,-1,73,-1,70,74,
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
	15,16,17,18,19,20,21,22,23,24,25,68,-1,69,64,-1,
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,67,-1,

	// チェックボックス、星
	139,140,142,141,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	 -1, -1, -1, -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

	// 0xa0 ~ 0xdf
	 -1,126, -1, -1,125, 72,123,130,131,132,133,134,136,137,138,135,
	 71, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
	 93, 94, 95, 96, 97, 98, 99,100,101,102,104,105,106,107,108,109,
	110,111,112,113,114,115,116,117,118,119,120,121,122,124,127,128,
};

void DbgFont_Init(Gfx **gpp)
{
	static Gfx init_dl[] = {
		gsDPPipeSync(),
		gsDPSetCycleType( G_CYC_1CYCLE ),
		gsDPSetScissor( G_SC_NON_INTERLACE,0,0,319,239 ),
		gsSPTexture( 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON ),
		gsDPSetRenderMode( G_RM_TEX_EDGE, G_RM_TEX_EDGE2 ),
		gsDPSetCombineMode( G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM ),
		gsDPSetTexturePersp( G_TP_NONE ),
		gsDPSetTextureFilter ( G_TF_POINT ),
		gsDPSetTextureDetail( G_TD_CLAMP ),
		gsDPSetTextureLOD ( G_TL_TILE ),
		gsDPSetTextureLUT ( G_TT_NONE ),
		gsDPSetTextureConvert( G_TC_FILT ),
		gsDPSetColorDither( G_CD_BAYER ),
		gsDPLoadTextureBlock_4b(
			dbg_font_bm0_0, G_IM_FMT_I,
			DBG_FONT_TEX_W, DBG_FONT_TEX_H, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD),
		gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
		gsSPEndDisplayList(),
	};
	Gfx *gp = *gpp;

	gSPDisplayList(gp++, init_dl);

	*gpp = gp;
}

void DbgFont_SetColor(Gfx **gpp, int color)
{
	static u8 color_tbl[][3] = {
		{ 0x00, 0x00, 0x00, },
		{ 0x00, 0x00, 0xff, },
		{ 0x00, 0xff, 0x00, },
		{ 0x00, 0xff, 0xff, },
		{ 0xff, 0x00, 0x00, },
		{ 0xff, 0x00, 0xff, },
		{ 0xff, 0xff, 0x00, },
		{ 0xff, 0xff, 0xff, },
	};
	Gfx *gp = *gpp;
	u8 *rgb = color_tbl[color & 7];

	gDPPipeSync(gp++);
	gDPSetPrimColor(gp++, 0, 0, rgb[0], rgb[1], rgb[2], 255);

	*gpp = gp;
}

void DbgFont_Draw(Gfx **gpp, int x, int y, int index)
{
	Gfx *gp = *gpp;
	int s = ((index % DBG_FONT_COL) * DBG_FONT_W) << 5;
	int t = ((index / DBG_FONT_COL) * DBG_FONT_H) << 5;

	gSPScisTextureRectangle(gp++,
		x<<2, y<<2, (x+DBG_FONT_W)<<2, (y+DBG_FONT_H)<<2,
		G_TX_RENDERTILE, s, t, 1<<10, 1<<10);

	*gpp = gp;
}

void DbgFont_DrawAscii(Gfx **gpp, int x, int y, int ascii)
{
	int font;

	if(ascii > 0x20 && ascii < 0xe0) {
		if((font = asc2dbg_tbl[ascii - 0x20]) != -1) {
			DbgFont_Draw(gpp, x, y, font);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// デバッグ用IO

typedef struct {
	Gfx **gpp;
	EnumMarkerCallback *func;
	short x, y;
	short col, row;
	short mode;
} IoDbgState;

static IoDbgState io_dbg_state;

static int io_dbg_open(IO_FILE *fp, va_list va);
static int io_dbg_close(IO_FILE *fp);
static int io_dbg_write(IO_FILE *fp, void *buf, int size);

static IO_FUNC_TBL io_ftbl_dbg = {
	io_dbg_open, io_dbg_close, io_dbg_write,
};

static IO_FILE io_dbg_con = {
	&io_ftbl_dbg, &io_dbg_state,
};

static int io_dbg_open(IO_FILE *fp, va_list va)
{
	IoDbgState *state = (IoDbgState *)fp->farg;

	state->gpp = va_arg(va, Gfx **);
	state->func = va_arg(va, EnumMarkerCallback *);
	state->x = va_arg(va, int);
	state->y = va_arg(va, int);
	state->col = 0;
	state->row = 0;
	state->mode = 0;

	if(state->gpp) {
		DbgFont_Init(state->gpp);
	}

	return 0;
}

static int io_dbg_close(IO_FILE *fp)
{
	return 0;
}

static int io_dbg_write(IO_FILE *fp, void *buf, int size)
{
	IoDbgState *state;
	int code, font, remain;

	state = (IoDbgState *)fp->farg;
	remain = size;

	while(remain > 0) {
		remain--;
		code = *((unsigned char *)buf)++;

		switch(state->mode) {
		case '@':
			switch(code) {
			case 'c': case 'm':
				state->mode = code;
				break;
			default:
				state->mode = 0;
				break;
			}
			break;
		case 'c':
			if(state->gpp) {
				DbgFont_SetColor(state->gpp, code - '0');
			}
			state->mode = 0xff;
			break;
		case 'm':
			if(state->func) {
				state->func(state->gpp,
					state->x + state->col * DBG_FONT_W,
					state->y + state->row * DBG_FONT_H, code);
			}
			state->mode = 0xff;
			break;
		default:
			state->mode = 0;
			break;
		}

		if(state->mode) {
			continue;
		}

		if(code == '@') {
			state->mode = code;
		}
		else if(code < 0x20) {
			switch(code) {
			case '\t':
				state->col = (state->col + 4) & ~3;
				break;
			case '\n':
				state->col = 0;
				state->row++;
				break;
			}
		}
		else {
			if(state->gpp) {
				DbgFont_DrawAscii(state->gpp,
					state->x + state->col * DBG_FONT_W,
					state->y + state->row * DBG_FONT_H, code);
			}
#if defined(DEBUG)
			else if(!state->func) {
				osSyncPrintf("%c", code);
			}
#endif
			state->col++;
		}
	}

	return size;
}

//////////////////////////////////////////////////////////////////////////////
// デバッグコンソール

void DbgCon_Open(Gfx **gpp, EnumMarkerCallback *func, int x, int y)
{
	io_open(&io_dbg_con, gpp, func, x, y);
}

void DbgCon_Close()
{
	io_close(&io_dbg_con);
}

void DbgCon_Format(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
#ifndef LINUX_BUILD
	io_vprintf(&io_dbg_con, fmt, va);
#endif
	va_end(va);
}
