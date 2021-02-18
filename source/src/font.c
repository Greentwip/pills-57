#include <ultra64.h>
#include "tex_func.h"
#include "font.h"
#include "local.h"

#define NON_PALETTE
#include "texture/font/font_a.txt"
#include "texture/font/font_2.txt"
#include "texture/font/font_e.txt"
#include "texture/font/font_e2.txt"
#undef NON_PALETTE

//////////////////////////////////////
// グラデーションのデータ（12*12）
static	u16	font_grade_tex[] = {
	0xffff,0xffff,0xffff,
	0xeeee,0xeeee,0xeeee,
	0xdddd,0xdddd,0xdddd,
	0xcccc,0xcccc,0xcccc,
	0xbbbb,0xbbbb,0xbbbb,
	0xaaaa,0xaaaa,0xaaaa,
	0x9999,0x9999,0x9999,
	0x8888,0x8888,0x8888,
	0x7777,0x7777,0x7777,
	0x6666,0x6666,0x6666,
	0x5555,0x5555,0x5555,
	0x4444,0x4444,0x4444,
};

#include "texture/font/ascii_code.tbl"
#include "texture/font/ascii_code2.tbl"
#include "texture/font/char_code.tbl"
#include "texture/story_bmp/Fonts/kanji_font.DMP"

#if (LOCAL == CHINA)
    #ifdef BETTER_FONT
         #include "i10n/assets/zh/font/gbfont.txt"
         #include "i10n/assets/zh/font/gbfont.h"
    #else
         #include "i10n/assets/zh/font/gb_mes_font.all"
    #endif
    #define  GB_INDEX_START   1000
#endif

//////////////////////////////////////////////////////////////////////////////

int fontStr_nextChar(const unsigned char *str)
{
	int	n;

	if ( str[0] <= 0x7f ) {
		// 1Byte文字
		n = 1;
		if ( str[0] == FONT_CTRL ) {
			// 制御コード
			if ( str[1] == FONT_CTRL_EOS ) {
				n = 0;
			} else {
				if ( str[1] == FONT_CTRL_TIMER ) n = 3; else n = 2;
			}
		}
	} else {
#if (LOCAL == CHINA) 
                n = (( str[0] >= 0xa1 && str[0] <= 0xfe ) &&
                     (str[1] >= 0xa1 && str[1] <= 0xfe )) ? 2 : 1;
#else   
		if ( str[0] >= 0xa1 && str[0] <= 0xdf ) {
			// 半角カナ
			n = 1;
		} else {
			// 2Byte文字
			n = 2;
		}
#endif
	}

	return n;
}

int fontStr_length(const unsigned char *str)
{
	int	i = 0;
	int	n = 0;

	do {
		n = fontStr_nextChar(&str[i]);
		i += n;
	} while ( n != 0 );

	return i;
}

int fontStr_charSize(const unsigned char *str, int type)
{
	int i, w, c = str[0];

	if(c < 0x80) {
		ascii2index(c, type, &i, &w);
	}
	else {
		w = FONT_WORD_W;
	}

	return w;
}

//////////////////////////////////////////////////////////////////////////////
// フォント番号から漢字コードへ変換
int	index2font(u8 code)
{
	int	n;

	n = (int)code2kanji_tbl[code];

	return( n & 0x0000ffff );
}


// 漢字コードからフォント番号へ変換
int	font2index(u8* charcode)
{
	int	n;

	n = (int)charcode[0];

#if (LOCAL == CHINA) 
           /* Handle GB code here */
        if  ( (n >= 0xa1 && n <= 0xfe) && 
              (charcode[1] >= 0xA1 && charcode[1] <= 0xfe)) {

              n = (n << 8) | charcode[1];
              /* Binary search to find the index */
              {
                  int st, end, cur;
                  st = 0;
#ifdef BETTER_FONT
                  end = gbfont_num - 1;
#else
                  end = gb_mes_code_num - 1;
#endif
 
                  while (st <= end)  {
                      cur = (st + end) >> 1;
#ifdef BETTER_FONT
                      if ( n == gbfont_code[cur])  
#else
                      if ( n == gb_mes_code_code[cur]) 
#endif
                           return cur + GB_INDEX_START + 1;

#ifdef BETTER_FONT
                      if ( n > gbfont_code[cur])
#else
                      if ( n > gb_mes_code_code[cur])
#endif
                           st = cur + 1;
                      else
                           end = cur - 1;
                  }
                  return GB_INDEX_START + 1;   /* Not found */
              }
	}
#endif

	// S-JISの1Byte目
	if ( n >= 0x81 && n <= 0x9f ) {
		n = ( n - 0x0080 ) << 8;
		n += (int)charcode[1];
	} else if ( n >= 0xe0 && n <= 0xef ) {
		n = ( n - 0x00c0 ) << 8;
		n += (int)charcode[1];
	}

	n = (int)char_code_tbl[n];

	return(n);
}

// アスキーコードからフォント番号へ変換
void ascii2index(int code, int type, int *index, int *width)
{
	static const u8 *_tbl[] = { font_e_tbl, font_e2_tbl };
	const u8 *tbl = _tbl[type & 1];
	code = (code & 0x7f) << 1;
	*index = tbl[code + 0];
	*width = tbl[code + 1];
}

void font16_initDL(Gfx **gpp)
{
	static const Gfx init_dl[] = {
		gsSPDisplayList(normal_texture_init_dl),
		gsDPSetRenderMode(G_RM_XLU_SURF, G_RM_XLU_SURF2),
		gsDPSetTextureLUT(G_TT_NONE),
		gsDPSetCombineLERP(
			0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0,
			0,0,0,PRIMITIVE, TEXEL0,0,PRIMITIVE,0),
		gsSPEndDisplayList(),
	};
	gSPDisplayList((*gpp)++, init_dl);
}

void font16_initDL2(Gfx **gpp)
{
	Gfx *gp = *gpp;

	font16_initDL(&gp);

	// グラデーションの下側の色
	gDPSetEnvColor(gp++, 250, 250, 250, 255);

	// グラデーションのロード
	gDPLoadMultiTile_4b(gp++,
		(u8 *)font_grade_tex, 410, G_TX_RENDERTILE + 1, G_IM_FMT_I,
		12, 12, 0, 0, 11, 11, 0,
		G_TX_CLAMP, G_TX_CLAMP,
		G_TX_NOMASK, G_TX_NOMASK,
		G_TX_NOLOD, G_TX_NOLOD);

	*gpp = gp;
}

void fontXX_draw(Gfx **gpp, float x, float y, float w, float h, u8 *charcode)
{
	fontXX_drawID(gpp, x, y, w, h, font2index(charcode));
}

int fontXX_drawID(Gfx **gpp, float x, float y, float w, float h, int index)
{
	u8 *tex;
	int texW, texH, t[8];

	if(w <= 0 || h <= 0 || index == 0) return 0;

#if (LOCAL == CHINA) 
        if (index > GB_INDEX_START) {
#ifdef BETTER_FONT
            tex = gbfont_tex;
#else
            tex = gb_mes_code_bitmap;
#endif
            index -= GB_INDEX_START; 

        } else tex = font_a_tex;
#else
	tex = font_a_tex;
#endif

	texW = 12;
	texH = 12;

	if(index > 0) {
		gDPLoadTextureTile_4b((*gpp)++,
			(u8 *)tex + (index - 1) * texW * texH / 2, G_IM_FMT_I,
			texW, texH, 0, 0, texW-1, texH-1, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD);
	}

	t[0] = x * 4;
	t[1] = y * 4;
	t[2] = (x + w) * 4;
	t[3] = (y + h) * 4;
	t[4] = 0 << 5;
	t[5] = 0 << 5;
	t[6] = 1024 * texW / w;
	t[7] = 1024 * texH / h;

	gSPScisTextureRectangle((*gpp)++,
		t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
		t[4], t[5], t[6], t[7]);

	return 1;
}

void fontXX_draw2(Gfx **gpp, float x, float y, float w, float h, u8 *charcode)
{
	fontXX_drawID2(gpp, x, y, w, h, font2index(charcode));
}

int fontXX_drawID2(Gfx **gpp, float x, float y, float w, float h, int index)
{
	u8 *tex;
	int texW, texH, t[8], c, i;

	if(w <= 0 || h <= 0 || index == 0) return 0;

#if (LOCAL == CHINA) /* XXX make another bitmap ? */
        if (index > GB_INDEX_START) {
            /* Use same texture rendering until we have font type 2 */
            //tex = gbfont_tex; 
            //index -= GB_INDEX_START;  
            return fontXX_drawID(gpp, x, y, w, h, index);
        } else tex = font_2_tex;
#else
	tex = font_2_tex;
#endif
	texW = 12;
	texH = 12;

	t[0] = (int)( x * 4.0 + 0.5 );
	t[1] = (int)( y * 4.0 + 0.5 );
	t[2] = (int)( (x + w) * 4.0 + 0.5 );
	t[3] = (int)( (y + h) * 4.0 + 0.5 );
	t[4] = 0 << 5;
	t[5] = 0 << 5;
	t[6] = (int)( ( 1024.0 * texW ) / w + 0.5 );
	t[7] = (int)( ( 1024.0 * texH ) / h + 0.5 );

	if(index > 0) {
		gDPLoadTextureTile_4b((*gpp)++,
			(u8 *)tex + (index - 1) * texW * texH, G_IM_FMT_I,
			texW+texW, texH, 0, 0, texW+texW-1, texH-1, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD);
	}

	for(i = 0; i < 2; i++) {
		if ( i == 0 ) {
			// 中
			gDPSetCycleType((*gpp)++, G_CYC_2CYCLE);
			gDPSetCombineLERP((*gpp)++, 0,0,0,1, 0,0,0,TEXEL0, PRIMITIVE,ENVIRONMENT,TEXEL0,ENVIRONMENT, 0,0,0,COMBINED);

			gSPScisTextureRectangle((*gpp)++,
				t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
				t[4], t[5], t[6], t[7]);
		} else {
			// 縁
			gDPSetCycleType((*gpp)++, G_CYC_1CYCLE);
			gDPSetCombineLERP((*gpp)++, 0, 0, 0, 0, 0, 0, 0, TEXEL0, 0, 0, 0, 0, 0, 0, 0, TEXEL0);

			gSPScisTextureRectangle((*gpp)++,
				t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
				t[4] + FONT_WORD_W<<5, t[5], t[6], t[7]);
		}
	}

	return 1;
}

int fontAsc_draw(Gfx **gpp, float x, float y, float w, float h, u8 *charcode)
{
	int index, width;
	ascii2index(*charcode, 0, &index, &width);
	fontAsc_drawID(gpp, x, y, w, h, index);
}
int fontAsc_drawID(Gfx **gpp, float x, float y, float w, float h, int index)
{
	u8 *tex;
	int texW, texH, blk, ofs, t[8];

	if(w <= 0 || h <= 0 || index == 0) return 0;

	tex = font_e_tex;
	texW = FONT_BYTE_W;
	texH = FONT_BYTE_H;
	blk = (index - 1) & ~1;
	ofs = ((index - 1) & 1) ? texH : 0;

	if(index > 0) {
		gDPLoadTextureTile_4b((*gpp)++,
			(u8 *)tex + blk * texW * texH / 2, G_IM_FMT_I,
			texW, texH, 0, ofs, texW-1, ofs+texH-1, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD);
	}

	t[0] = x * 4;
	t[1] = y * 4;
	t[2] = (x + w) * 4;
	t[3] = (y + h) * 4;
	t[4] = 0 << 5;
	t[5] = ofs << 5;
	t[6] = 1024 * texW / w;
	t[7] = 1024 * texH / h;

	gSPScisTextureRectangle((*gpp)++,
		t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
		t[4], t[5], t[6], t[7]);

	return 1;
}

int fontAsc_draw2(Gfx **gpp, float x, float y, float w, float h, u8 *charcode)
{
	int index, width;
	ascii2index(*charcode, 1, &index, &width);
	fontAsc_drawID2(gpp, x, y, w, h, index);
}
int fontAsc_drawID2(Gfx **gpp, float x, float y, float w, float h, int index)
{
	u8 *tex;
	int texW, texH, t[8], c, i;

	if(w <= 0 || h <= 0 || index == 0) return 0;
#if defined(DEBUG)
{static n;if(!n){osSyncPrintf("%f %f\n",w,h);n++;}}
#endif

	tex = font_e2_tex;
	texW = FONT_BYTE_W;
	texH = FONT_BYTE_H;

	t[0] = x * 4;
	t[1] = y * 4;
	t[2] = (x + w) * 4;
	t[3] = (y + h) * 4;
	t[4] = 0 << 5;
	t[5] = 0 << 5;
	t[6] = (1024 * texW) / w;
	t[7] = (1024 * texH) / h;

	if(index > 0) {
		gDPLoadTextureTile_4b((*gpp)++,
			(u8 *)tex + (index - 1) * texW * texH, G_IM_FMT_I,
			texW+texW, texH, 0, 0, texW+texW-1, texH-1, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD);
	}

	for(i = 0; i < 2; i++) {
		if ( i == 0 ) {
			// 中
			gDPSetCycleType((*gpp)++, G_CYC_2CYCLE);
			gDPSetCombineLERP((*gpp)++, 0,0,0,1, 0,0,0,TEXEL0, PRIMITIVE,ENVIRONMENT,TEXEL0,ENVIRONMENT, 0,0,0,COMBINED);

			gSPScisTextureRectangle((*gpp)++,
				t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
				t[4], t[5], t[6], t[7]);
		} else {
			// 縁
			gDPSetCycleType((*gpp)++, G_CYC_1CYCLE);
			gDPSetCombineLERP((*gpp)++, 0, 0, 0, 0, 0, 0, 0, TEXEL0, 0, 0, 0, 0, 0, 0, 0, TEXEL0);

			gSPScisTextureRectangle((*gpp)++,
				t[0], t[1], t[2], t[3], G_TX_RENDERTILE,
				t[4] + FONT_BYTE_W<<5, t[5], t[6], t[7]);
		}
	}

	return 1;
}

