
#if !defined(_DBG_FONT_H_)
#define _DBG_FONT_H_

#define DBG_FONT_W 6
#define DBG_FONT_H 8

#define DBG_FONT_CHAR_CHECK_0 0x80
#define DBG_FONT_CHAR_CHECK_1 0x81
#define DBG_FONT_CHAR_STAR_0 0x82
#define DBG_FONT_CHAR_STAR_1 0x83

extern void DbgFont_Init(Gfx **gpp);
extern void DbgFont_SetColor(Gfx **gpp, int color);
extern void DbgFont_Draw(Gfx **gpp, int x, int y, int index);
extern void DbgFont_DrawAscii(Gfx **gpp, int x, int y, int ascii);

typedef void (EnumMarkerCallback)(Gfx **gpp, int x, int y, int marker);

extern void DbgCon_Open(Gfx **gpp, EnumMarkerCallback *func, int x, int y);
extern void DbgCon_Close();
extern void DbgCon_Format(const char *fmt, ...);

#endif // _DBG_FONT_H_
