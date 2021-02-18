
#include <ultra64.h>
#include "def.h"

#include "nnsched.h"
#include "graphic.h"

#include "hardcopy.h"
#include "tex_func.h"
#include "util.h"

#include "debug.h"

//////////////////////////////////////////////////////////////////////////////
//{### 構造体

typedef struct HCCurtain HCCurtain;
typedef struct HCSphere HCSphere;

static union {
	HCCurtain *curtain;
	HCSphere *sphere;
} hc;

//////////////////////////////////////////////////////////////////////////////
//{### 共通ルーチン

//## カメラを初期化
static
void HCInitCamera(Mtx *projectMtx, u16 *perspNorm)
{
	float ml[4][4], mr[4][4], vf[4];
	int i;

	guTranslateF(ml, 0.f, 0.f, -512.f);
	guPerspectiveF(mr, perspNorm,
		30.f, 320.f/240.f, 512.f-320.f, 512.f+320.f, 1.f);

	guMtxCatF(ml, mr, mr);
	for(i = 0; i < 4; i++) {
		vf[i] = mr[0][i]*160.f + mr[1][i]*120.f + mr[3][i];
	}
	guScaleF(ml, vf[3]/vf[0], vf[3]/vf[1], 1.f);
	guMtxCatF(ml, mr, ml);
	guMtxF2L(ml, projectMtx);
}

//## コピーレクタングル16
static
void HCCopyRect16(Gfx **gpp, void *tex, int x, int y, int w, int h)
{
	static Gfx init_dl[] = {
		gsDPPipeSync(),
		gsSPSetGeometryMode(G_TEXTURE_ENABLE),
		gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
		gsDPSetCycleType(G_CYC_COPY),
		gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP),
		gsDPSetCombineLERP(0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0),
//		gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
		gsDPSetTextureLUT(G_TT_NONE),
		gsSPEndDisplayList(),
	};

	Gfx *gp = *gpp;
	int step = 2048 / w;
	int i;

	gSPDisplayList(gp++, init_dl);

	for(i = 0; i < h; i += step) {
		gDPLoadTextureTile(gp++,
			&((u16 *)tex)[w*i],
			G_IM_FMT_RGBA, G_IM_SIZ_16b,
			w, step, 0, 0, w-1, MIN(step,h-i)-1, 0,
			G_TX_CLAMP, G_TX_CLAMP,
			G_TX_NOMASK, G_TX_NOMASK,
			G_TX_NOLOD, G_TX_NOLOD);

		gSPScisTextureRectangle(gp++,
			x<<(x<0 ? 0 : 2), (y+i)<<2,
			(x+w-1)<<2, (MIN(y+i+step,y+h)-1)<<2,
			G_TX_RENDERTILE, 0, 0, 1<<12, 1<<10);
	}

	*gpp = gp;
}

//## 平面の頂点を作成
static
void HCMakePlanarVtx(Vtx **vtxPP, void **freePP)
{
	Vtx_tn *vn;
	int x, y;

	*vtxPP = (Vtx *)ALIGN_16(*freePP);
	*freePP = (Vtx *)*vtxPP + 16 * 21;

	for(x = 0; x < 21; x++) {
		for(y = 0; y < 16; y++) {
			vn = &(*vtxPP)[16 * x + y].n;

			vn->ob[0] = x * 16 - 160;
			vn->ob[1] = 120 - y * 16;
			vn->ob[2] = 0;
			vn->flag = 0;
			vn->tc[0] = (x * 16) << 6;
			vn->tc[1] = (y * 16) << 6;
			vn->n[0] = 0;
			vn->n[1] = 0;
			vn->n[2] = 127;
			vn->a = 255;

			if(x == 20) {
				vn->tc[0] -= (2 << 6);
			}
		}

		vn->tc[1] -= (2 << 6);
	}
}

//## 平面のディスプレイリストを作成
static
void HCMakePlanarGfx(Gfx **gfxPP, Vtx *vtx, void *img, void **freePP)
{
	int x, y, flag = 0;
	Gfx *gfx = *gfxPP = (Gfx *)ALIGN_16(*freePP);

	gSPVertex(gfx++, &vtx[0], 16, 0);

	for(x = 0; x < 20; x++) {
		gSPVertex(gfx++, &vtx[(x + 1) * 16], 16, (x&1)?0:16);

		for(y = 0; y < 15; y++) {
			if(y % 4 == 0) {
				gDPLoadTextureTile(gfx++,
					img, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 240,
					x*16, y*16, x*16+15, MIN(y*16+63, 239), 0,
					G_TX_CLAMP, G_TX_CLAMP,
					G_TX_NOMASK, G_TX_NOMASK,
					G_TX_NOLOD, G_TX_NOLOD);
			}

			if(x & 1) {
				gSP2Triangles(gfx++, y+16, y+17, y+0, 0, y+17, y+1, y+0, 0);
			}
			else {
				gSP2Triangles(gfx++, y+0, y+1, y+16, 0, y+1, y+17, y+16, 0);
			}
		}
	}

	gSPEndDisplayList(gfx++);

	*freePP = gfx;
}

//## 平面の法線を求める
static
void HCCalcPlanarNormal(Vtx *vtx)
{
	// ※スタック使いすぎ
	float norm[20][15][3];
	float len, v0[3], v1[3];
	Vtx_tn *vn0, *vn1, *vn2;
	int i, x, y;

	// 各面の法線を求める
	for(x = 0; x < 20; x++) {
		for(y = 0; y < 15; y++) {
			float *np;

			vn0 = &vtx[(x + 1) * 16 + y].n;
			vn1 = &vtx[x * 16 + y].n;
			vn2 = &vtx[x * 16 + y + 1].n;

			v0[0] = (float)(vn1->ob[0] - vn0->ob[0]);
			v0[1] = (float)(vn1->ob[1] - vn0->ob[1]);
			v0[2] = (float)(vn1->ob[2] - vn0->ob[2]);

			v1[0] = (float)(vn2->ob[0] - vn1->ob[0]);
			v1[1] = (float)(vn2->ob[1] - vn1->ob[1]);
			v1[2] = (float)(vn2->ob[2] - vn1->ob[2]);

			np = &norm[x][y][0];

			np[0] = v0[1] * v1[2] - v0[2] * v1[1];
			np[1] = v0[2] * v1[0] - v0[0] * v1[2];
			np[2] = v0[0] * v1[1] - v0[1] * v1[0];
		}
	}

	// グローシェーディング用の法線を求める
	for(x = 1; x < 20; x++) {
		for(y = 1; y < 15; y++) {
			v0[0] = norm[x][y][0] + norm[x][y-1][0] + norm[x-1][y][0] + norm[x-1][y-1][0];
			v0[1] = norm[x][y][1] + norm[x][y-1][1] + norm[x-1][y][1] + norm[x-1][y-1][1];
			v0[2] = norm[x][y][2] + norm[x][y-1][2] + norm[x-1][y][2] + norm[x-1][y-1][2];

			len = sqrtf(v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2]);
			if(len != 0.f) {
				len = 127.f / len;
			}

			vn0 = &vtx[x * 16 + y].n;

			vn0->n[0] = (short)(v0[0] * len);
			vn0->n[1] = (short)(v0[1] * len);
			vn0->n[2] = (short)(v0[2] * len);
		}
	}

	// 角
	for(x = 0; x < 21; x += 20) {
		for(y = 0; y < 16; y += 15) {
			vn0 = &vtx[x * 16 + y].n;
			vn1 = &vtx[(x+(x?-1:1)) * 16 + (y+(y?-1:1))].n;

			vn0->n[0] = vn1->n[0];
			vn0->n[1] = vn1->n[1];
			vn0->n[2] = vn1->n[2];
		}
	}

	// 上下
	for(x = 1; x < 20; x++) {
		vn0 = &vtx[x * 16].n;
		vn1 = &vtx[x * 16 + 1].n;

		vn0->n[0] = vn1->n[0];
		vn0->n[1] = vn1->n[1];
		vn0->n[2] = vn1->n[2];

		vn0 = &vtx[x * 16 + 15].n;
		vn1 = &vtx[x * 16 + 14].n;

		vn0->n[0] = vn1->n[0];
		vn0->n[1] = vn1->n[1];
		vn0->n[2] = vn1->n[2];
	}

	// 左右
	for(y = 1; y < 15; y++) {
		vn0 = &vtx[0 * 16 + y].n;
		vn1 = &vtx[1 * 16 + y].n;

		vn0->n[0] = vn1->n[0];
		vn0->n[1] = vn1->n[1];
		vn0->n[2] = vn1->n[2];

		vn0 = &vtx[20 * 16 + y].n;
		vn1 = &vtx[19 * 16 + y].n;

		vn0->n[0] = vn1->n[0];
		vn0->n[1] = vn1->n[1];
		vn0->n[2] = vn1->n[2];
	}
}

//## カーテン
static
void HCPlanarCurtain(Vtx *vtx, float angle, float cycle, float amplitude)
{
	float amp[16];
	int x, y;

	cycle = 3.141592f / cycle;

	for(y = 0; y < 16; y++) {
		amp[y] = (float)(y + 1) * amplitude;
	}

	for(x = 0; x < 21; x++) {
		Vtx_tn *vn = &vtx[x * 16].n;
		float rad = (angle + (float)x * 16.f) * cycle;
		float sx = sinf(rad);

		for(y = 0; y < 16; y++) {
			vn[y].ob[2] = (signed short)(sx * amp[y]);
		}
	}

	HCCalcPlanarNormal(vtx);
}

//## モーフ
static
void HCMorphVtx(
	Vtx *vtx,
	int count,
	const float (*from)[6],
	const float (*to)[6],
	float ratio)
{
	float *f0, *f1;
	float vx, vy, vz, nx, ny, nz;
	int i;

	for(i = 0; i < count; i++) {
		f0 = (float *)&from[i][0];
		f1 = (float *)&to[i][0];

		vx = f0[0] + (f1[0] - f0[0]) * ratio;
		vy = f0[1] + (f1[1] - f0[1]) * ratio;
		vz = f0[2] + (f1[2] - f0[2]) * ratio;

		nx = f0[3] + (f1[3] - f0[3]) * ratio;
		ny = f0[4] + (f1[4] - f0[4]) * ratio;
		nz = f0[5] + (f1[5] - f0[5]) * ratio;

		vtx[i].n.ob[0] = (short)vx;
		vtx[i].n.ob[1] = (short)vy;
		vtx[i].n.ob[2] = (short)vz;

		vtx[i].n.n[0] = (signed char)nx;
		vtx[i].n.n[1] = (signed char)ny;
		vtx[i].n.n[2] = (signed char)nz;
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### カーテン

//## バッファ数
#define HC_CURTAIN_BUFFER_DEPTH 3

//## 構造体
struct HCCurtain {
	Mtx		projectMtx;
	u16		perspNorm;
	u16		*frameBuf;

	Vtx		*vtx[HC_CURTAIN_BUFFER_DEPTH];
	Gfx		*gfx[HC_CURTAIN_BUFFER_DEPTH];
	Mtx		*mtx[HC_CURTAIN_BUFFER_DEPTH];

	int		current;
	int		exitFlag;
	u32		mainCount;
	u32		grapCount;
};

//## 初期化DL
static const Lights1 curtain_light = gdSPDefLights1(
	 159, 159, 159,
	 255, 255, 255,  73,  73,  73 );

static const Gfx curtain_init_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsSPClearGeometryMode(0
		|G_CULL_BACK
		|G_CULL_FRONT
		|G_FOG
		|G_TEXTURE_GEN
		|G_TEXTURE_GEN_LINEAR
		|G_CLIPPING
		|G_LOD
		|G_ZBUFFER
		),
	gsSPSetGeometryMode(0
		|G_TEXTURE_ENABLE
		|G_LIGHTING
		|G_SHADE
		|G_SHADING_SMOOTH
		),
	gsSPSetLights1(curtain_light),
	gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
	gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
	gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
	gsDPSetCombineKey(G_CK_NONE),
//	gsDPSetPrimColor(255,255,200,200,200,255),
//	gsDPSetEnvColor(200, 200, 200, 255),
//	gsDPSetBlendColor(200, 200, 200, 128),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetConvert(175, -43, -89, 222, 114, 42),
	gsSPEndDisplayList(),
};

//## 初期化
void *HCCurtain_Init(void *ptr)
{
	HCCurtain *st;
	int i, j;

	// 
	st = hc.curtain = (HCCurtain *)ALIGN_16(ptr);
	ptr = hc.curtain + 1;

	// カメラの初期化
	HCInitCamera(&st->projectMtx, &st->perspNorm);

	// フレームバッファ
	st->frameBuf = (u16 *)ALIGN_16(ptr);
	ptr = st->frameBuf + 320*240;
	bcopy(fbuffer[wb_flag^1], st->frameBuf, 320*240*sizeof(u16));

	for(i = 0; i < HC_CURTAIN_BUFFER_DEPTH; i++) {
		// 頂点を作成
		HCMakePlanarVtx(&st->vtx[i], &ptr);
		HCCalcPlanarNormal(st->vtx[i]);

		// ディスプレイリストを作成
		HCMakePlanarGfx(&st->gfx[i], st->vtx[i], st->frameBuf, &ptr);

		// マトリックスを作成
		st->mtx[i] = (Mtx *)ALIGN_16(ptr);
		ptr = st->mtx[i] + 1;
		guMtxIdent(st->mtx[i]);
	}

	st->current = 0;
	st->exitFlag = 0;
	st->mainCount = 0;
	st->grapCount = 0;

	return ptr;
}

//## メイン
int HCCurtain_Main()
{
	HCCurtain *st = hc.curtain;
	int current, i, j;
	float ratio, rotate, angle, cycle, amplitude;
	float ml[4][4], mr[4][4];

	if(st->exitFlag) {
		return 0;
	}

	ratio = MIN(st->mainCount * (1.f/40.f), 1.f);
	st->exitFlag = (ratio == 1.f);

	current = (st->current + 1) % HC_CURTAIN_BUFFER_DEPTH;
	rotate = ratio * 60.f;

	angle = (float)st->mainCount;
	amplitude = rotate * (1.f / 2.f);
	cycle = 120.f + rotate * 4.f;

	HCPlanarCurtain(st->vtx[current], angle, cycle, amplitude);

	guTranslateF(ml, 0.f, -160.f, 0.f);
	guRotateRPYF(mr, -rotate * 0.5f, 0.f, 0.f);
	guMtxCatF(ml, mr, ml);
	guTranslateF(mr, 0.f, 160.f, 0.f);
	guMtxCatF(ml, mr, ml);
	guMtxF2L(ml, st->mtx[current]);

	st->current = current;
	st->mainCount++;

	return 1;
}

//## グラフィック
void HCCurtain_Grap(Gfx **gpp)
{
	HCCurtain *st = hc.curtain;
	Gfx *gfx;
	int current;

	if(st->exitFlag) {
		return;
	}

	gfx = *gpp;
	current = st->current;

	gSPDisplayList(gfx++, curtain_init_dl);

	gSPPerspNormalize(gfx++, st->perspNorm);
	gSPMatrix(gfx++, &st->projectMtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
	gSPMatrix(gfx++, st->mtx[current], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
	gSPDisplayList(gfx++, st->gfx[current]);

	*gpp = gfx;
	st->grapCount++;
}

//////////////////////////////////////////////////////////////////////////////
//{### 球

//## バッファ数
#define HC_SPHERE_BUFFER_DEPTH 3

//## 構造体
struct HCSphere {
	Mtx		projectMtx;
	u16		perspNorm;
	u16		*frameBuf;

	Vtx		*vtx[HC_SPHERE_BUFFER_DEPTH];
	Gfx		*gfx[HC_SPHERE_BUFFER_DEPTH];
	Mtx		*mtx[HC_SPHERE_BUFFER_DEPTH];

	int		current;
	int		exitFlag;
	u32		mainCount;
	u32		grapCount;

	float	(*planar)[6];
	float	(*sphere)[6];
};

//## 初期化DL
static const Lights1 sphere_light = gdSPDefLights1(
	 159, 159, 159,
	 255, 255, 255,  73,  73,  73 );

static const Gfx sphere_init_dl[] = {
	gsDPPipeSync(),
	gsDPSetCycleType(G_CYC_1CYCLE),
	gsSPClearGeometryMode(0
		|G_CULL_FRONT
		|G_FOG
		|G_TEXTURE_GEN
		|G_TEXTURE_GEN_LINEAR
		|G_CLIPPING
		|G_LOD
		|G_ZBUFFER
		),
	gsSPSetGeometryMode(0
		|G_CULL_BACK
		|G_LIGHTING
		|G_SHADE
		|G_SHADING_SMOOTH
		|G_TEXTURE_ENABLE
		),
	gsSPSetLights1(sphere_light),
	gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
	gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
	gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
	gsDPSetCombineKey(G_CK_NONE),
//	gsDPSetPrimColor(255,255,200,200,200,255),
//	gsDPSetEnvColor(200, 200, 200, 255),
//	gsDPSetBlendColor(200, 200, 200, 128),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTexturePersp(G_TP_PERSP),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetConvert(175, -43, -89, 222, 114, 42),
	gsSPEndDisplayList(),
};

//## 初期化
void *HCSphere_Init(void *ptr)
{
	HCSphere *st;
	int i, j, x, y;

	st = hc.sphere = (HCSphere *)ALIGN_16(ptr);
	ptr = hc.sphere + 1;

	HCInitCamera(&st->projectMtx, &st->perspNorm);

	st->frameBuf = (u16 *)ALIGN_16(ptr);
	ptr = st->frameBuf + 320*240;
	bcopy(fbuffer[wb_flag^1], st->frameBuf, 320*240*sizeof(u16));

	// 平面の頂点パターンを作成
	st->planar = (float (*)[6])ALIGN_16(ptr);
	ptr = st->planar + 16 * 21;

	for(x = 0; x < 21; x++) {
		for(y = 0; y < 16; y++) {
			float *f = (float *)&st->planar[x * 16 + y][0];

			f[0] = (float)((x * 16) - 160);
			f[1] = (float)(120 - (y * 16));
			f[2] = 0.f;

			f[3] = 0.f;
			f[4] = 0.f;
			f[5] = 127.f;
		}
	}

	// 球の頂点パターンを作成
	st->sphere = (float (*)[6])ALIGN_16(ptr);
	ptr = st->sphere + 16 * 21;

	for(x = 0; x < 21; x++) {
		float rx, sx, cx;

		rx = (float)x * (3.141592f / 20.f * 2.f);
		sx = sinf(rx);
		cx = cosf(rx);

		for(y = 0; y < 16; y++) {
			float *f = (float *)&st->sphere[x * 16 + y][0];
			float ry, sy, cy;
			float len;

			ry = (float)y * (3.141592f / 15.f);
			sy = sinf(ry);
			cy = cosf(ry);

			f[0] = f[3] = -sx * sy;
			f[1] = f[4] = cy;
			f[2] = f[5] = -cx * sy;

			f[0] *= 80.f;
			f[1] *= 80.f;
			f[2] *= 80.f;

			f[3] *= 127.f;
			f[4] *= 127.f;
			f[5] *= 127.f;
		}
	}

	for(i = 0; i < HC_SPHERE_BUFFER_DEPTH; i++) {
		// 頂点を作成
		HCMakePlanarVtx(&st->vtx[i], &ptr);
		HCMorphVtx(st->vtx[i], 16*21, st->planar, st->sphere, 0.f);

		// ディスプレイリストを作成
		HCMakePlanarGfx(&st->gfx[i], st->vtx[i], st->frameBuf, &ptr);

		// マトリックスを作成
		st->mtx[i] = (Mtx *)ALIGN_16(ptr);
		ptr = st->mtx[i] + 1;
		guRotateRPY(st->mtx[i], 0.f, j*180.f, 0.f);
	}

	st->current = 0;
	st->exitFlag = 0;
	st->mainCount = 0;
	st->grapCount = 0;

	return ptr;
}

//## メイン
int HCSphere_Main()
{
	HCSphere *st = hc.sphere;
	int current;
	float ratio, scale;

	if(st->exitFlag) {
		return 0;
	}

	ratio = MIN(st->mainCount * (1.f/30.f), 1.f);
	st->exitFlag = (ratio == 1.f);

	current = (st->current + 1) % HC_SPHERE_BUFFER_DEPTH;

	// 平面 -> 球
	if(ratio < 1.f/2.f) {
		ratio *= 2.f;
		ratio = 1.f - ratio;
		ratio = ratio * ratio;
		ratio = 1.f - ratio;
		HCMorphVtx(st->vtx[current], 16*21, st->planar, st->sphere, ratio);
	}
	// 縮小
	else if(ratio <= 1.f) {
		ratio = (ratio - 1.f/2.f) * 2.f;
		ratio = 1.f - ratio * ratio;
		HCMorphVtx(st->vtx[current], 16*21, st->planar, st->sphere, 1.f);
		guScale(st->mtx[current], ratio, ratio, ratio);
	}

	st->current = current;
	st->mainCount++;

	return 1;
}

//## グラフィック
void HCSphere_Grap(Gfx **gpp)
{
	HCSphere *st = hc.sphere;
	Gfx *gfx;
	int current;

	if(st->exitFlag) {
		return;
	}

	gfx = *gpp;
	current = st->current;

	gSPPerspNormalize(gfx++, st->perspNorm);
	gSPMatrix(gfx++, &st->projectMtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

	gSPDisplayList(gfx++, sphere_init_dl);

	gSPMatrix(gfx++, st->mtx[current], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
	gSPDisplayList(gfx++, st->gfx[current]);

	*gpp = gfx;
	st->grapCount++;
}

//////////////////////////////////////////////////////////////////////////////
//{### 

//## 使用中のエフェクト番号
static HC_EFFECT hc_no = HC_NULL;

//## 初期化
void *HCEffect_Init(void *ptr, HC_EFFECT effect)
{
	void *top = ptr;
	hc_no = effect;

	switch(effect)
	{
	case HC_CURTAIN:
		ptr = HCCurtain_Init(top);
		break;

	case HC_SPHERE:
		ptr = HCSphere_Init(top);
		break;
	}

	PRT3("start:%08x end:%08x size:%08x\n", top, ptr, (u32)ptr-(u32)top);

	return ptr;
}

//## メイン
int HCEffect_Main()
{
	int result = 0;

	switch(hc_no)
	{
	case HC_CURTAIN:
		result = HCCurtain_Main();
		break;

	case HC_SPHERE:
		result = HCSphere_Main();
		break;
	}

	return result;
}

//## グラフィック
void HCEffect_Grap(Gfx **gpp)
{
	switch(hc_no)
	{
	case HC_CURTAIN:
		HCCurtain_Grap(gpp);
		break;

	case HC_SPHERE:
		HCSphere_Grap(gpp);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF

