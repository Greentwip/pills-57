
#if !defined(_TEX_FUNC_)
#define _TEX_FUNC_

//////////////////////////////////////////////////////////////////////////////

extern void gfxSetScissor(Gfx **gpp, int flags, int x, int y, int w, int h);

extern void FillRectRGB(Gfx **gpp, int x, int y, int w, int h, int r, int g, int b);
extern void FillRectRGBA(Gfx **gpp, int x, int y, int w, int h, int r, int g, int b, int a);

//////////////////////////////////////////////////////////////////////////////

#define G_CC_ALPHATEXTURE_PRIM TEXEL0,0,PRIMITIVE,0, TEXEL1,0,PRIMITIVE,0

extern Gfx copy_texture_init_dl[];
extern Gfx alpha_texture_init_dl[];
extern Gfx normal_texture_init_dl[];

//////////////////////////////////////////////////////////////////////////////

extern void CopyTexBlock4(Gfx **gpp, void *lut, void *tex, int x, int y, int w, int h);
extern void CopyTexBlock8(Gfx **gpp, void *lut, void *tex, int x, int y, int w, int h);
extern void CopyTexBlock16(Gfx **gpp, void *tex, int x, int y, int w, int h);

//////////////////////////////////////////////////////////////////////////////

extern void StretchAlphaTexBlock(Gfx **gpp,
	int texW, int texH,
	void *colorTex, int colorTexW,
	void *alphaTex, int alphaTexW,
	float x, float y, float w, float h);

extern void StretchTexBlock4(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	float x, float y, float w, float h);

extern void StretchTexBlock8(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	float x, float y, float w, float h);

extern void StretchTexBlock16(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h);

extern void StretchTexBlock4i(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h);

extern void StretchTexBlock8i(Gfx **gpp,
	int texW, int texH, void *tex,
	float x, float y, float w, float h);

extern void StretchAlphaTexTile(Gfx **gpp,
	int texW, int texH,
	void *colorTex, int colorTexW,
	void *alphaTex, int alphaTexW,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);
extern void RectAlphaTexTile(Gfx **gpp, Vtx **vpp,
	int texW, int texH,
	void *colorTex, int colorTexW,
	void *alphaTex, int alphaTexW,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);

extern void StretchTexTile4(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);

extern void StretchTexTile8(Gfx **gpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);
extern void RectTexTile8(Gfx **gpp, Vtx **vpp,
	int texW, int texH, void *lut, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);

extern void StretchTexTile16(Gfx **gpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);

extern void StretchTexTile4i(Gfx **gpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);
extern void RectTexTile4i(Gfx **gpp, Vtx **vpp,
	int texW, int texH, void *tex,
	int lx, int ly, int lw, int lh,
	float x, float y, float w, float h);

//////////////////////////////////////////////////////////////////////////////

extern Gfx **loadGfxData(void **hpp, void **segAddr, void *baseAddr, void *romStart, void *romEnd);

//////////////////////////////////////////////////////////////////////////////

typedef struct {
	void **addr;
	unsigned short *size;
} STexInfo;

extern void tiMappingAddr(STexInfo *tiArray, int count, u32 segGap);

extern STexInfo *tiLoadTexData(void **hpp, void *romStart, void *romEnd);

extern void tiCopyTexBlock(Gfx **gpp, STexInfo *ti, int cached, int x, int y);

extern void tiStretchTexBlock(Gfx **gpp, STexInfo *ti, int cached,
	float x, float y, float w, float h);

extern void tiStretchTexTile(Gfx **gpp, STexInfo *ti, int cached,
	int lx, int ly, int lw, int lh, float x, float y, float w, float h);

extern void tiStretchTexItem(Gfx **gpp, STexInfo *ti, int cached,
	int count, int index, float x, float y, float w, float h);

extern void tiStretchAlphaTexItem(Gfx **gpp, STexInfo *texC, STexInfo *texA,
	int cached, int count, int index, float x, float y, float w, float h);

extern void tiStretchAlphaTexItem2(Gfx **gpp, STexInfo *texC, STexInfo *texA,
	int cached, int count, int index, float x, float y, float w, float h);

//////////////////////////////////////////////////////////////////////////////

extern void drawCursorPattern(Gfx **gpp,
	int texW, int texH, int frmW, int frmH,
	int posX, int posY, int posW, int posH);

//////////////////////////////////////////////////////////////////////////////

#endif // _TEX_FUNC_
