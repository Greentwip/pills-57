
#if !defined(_FONT_H_)

// �S�p�t�H���g�e�N�X�`���̃T�C�Y
#define FONT_WORD_W 12
#define FONT_WORD_H 12

// ���p�t�H���g�e�N�X�`���̃T�C�Y
#define FONT_BYTE_W 10
#define FONT_BYTE_H 12

#define FONT_CTRL '~' // ���䕶���J�n�R�[�h
	#define FONT_CTRL_RETURN	'n'	// ���s
	#define FONT_CTRL_INKEY		'm'	// �L�[����
	#define FONT_CTRL_TIMER		'w'	// �^�C�}�[
	#define FONT_CTRL_CLR		'h'	// �N���A
	#define FONT_CTRL_EOS		'z'	// ������̏I�[

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
