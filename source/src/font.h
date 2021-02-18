
#if !defined(_FONT_H_)

// 全角フォントテクスチャのサイズ
#define FONT_WORD_W 12
#define FONT_WORD_H 12

// 半角フォントテクスチャのサイズ
#define FONT_BYTE_W 10
#define FONT_BYTE_H 12

#define FONT_CTRL '~' // 制御文字開始コード
	#define FONT_CTRL_RETURN	'n'	// 改行
	#define FONT_CTRL_INKEY		'm'	// キー入力
	#define FONT_CTRL_TIMER		'w'	// タイマー
	#define FONT_CTRL_CLR		'h'	// クリア
	#define FONT_CTRL_EOS		'z'	// 文字列の終端

extern	int	index2font(u8 code);
extern	int	font2index(u8* charcode);
extern void ascii2index(int charcode, int type, int *index, int *width);

extern int fontStr_nextChar(const unsigned char *str);
extern int fontStr_length(const unsigned char *str);
extern int fontStr_charSize(const unsigned char *str, int type);

extern void font16_initDL(Gfx **gpp);
extern void font16_initDL2(Gfx **gpp);

extern void fontXX_draw(Gfx **gpp, float x, float y, float w, float h, u8 *charcode);
extern int  fontXX_drawID(Gfx **gpp, float x, float y, float w, float h, int index);

extern void fontXX_draw2(Gfx **gpp, float x, float y, float w, float h, u8 *charcode);
extern int  fontXX_drawID2(Gfx **gpp, float x, float y, float w, float h, int index);

extern int fontAsc_draw(Gfx **gpp, float x, float y, float w, float h, u8 *charcode);
extern int fontAsc_drawID(Gfx **gpp, float x, float y, float w, float h, int index);

extern int fontAsc_draw2(Gfx **gpp, float x, float y, float w, float h, u8 *charcode);
extern int fontAsc_drawID2(Gfx **gpp, float x, float y, float w, float h, int index);

#endif // _FONT_H_
