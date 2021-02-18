#include <ultra64.h>
#include "def.h"
#include "calc.h"
#include "lws.h"

//-------------------------------------------------------------------
extern	Mtx	*pObjectMtx;

//-------------------------------------------------------------------
#define	MAX_PARTS		64		// 最大パーツ数
typedef struct {
	LMatrix 	m;
} ANIM_PROC;
static	ANIM_PROC	AnimProc[MAX_PARTS];


//-------------------------------------------------------------------
// ＬＷＳアニメーション表示
//-------------------------------------------------------------------
int	lws_anim(Gfx **glp, LMatrix pm, SCENE_DATA *pSceneData, int frame, u32 baseAddr)
{
	Gfx			*pgfx;
	SCENE_OBJ	*pObj;
	SCENE_KEY	*pKey, *pKey1, *pKey2;
	int			i, key, ret;
	float		lx, ly, lz;
	float		rx, ry, rz;
	float		sx, sy, sz;
	float		step;
	s32			ilx, ily, ilz;
	s16			irx, iry, irz;
	s32			n, tmp;
	LMatrix 	m, ms, mt;

	pgfx = *glp;

	if ( frame >= pSceneData->last_frame ) {
		ret = TRUE;
		frame = pSceneData->last_frame - 1;
	} else {
		ret = FALSE;
	}

	for ( i = 0; i < pSceneData->obj_count; i++ ) {
//		pObj = &pSceneData->obj[i];
//		pKey = &pSceneData->key[pObj->key_index];
		pObj = (SCENE_OBJ *)( ((u32)pSceneData->obj & 0x00ffffff ) + baseAddr );
		pObj = &pObj[i];
		pKey = (SCENE_KEY *)( ((u32)pSceneData->key & 0x00ffffff ) + baseAddr );
		pKey = &pKey[pObj->key_index];
		// キーを見つける
		pKey1 = pKey2 = pKey;
		for ( key = 1; key < pObj->key_count; key++ ) {
			pKey1 = &pKey[key-1];
			pKey2 = &pKey[key  ];
			if ( pKey2->key_no > frame ) break;
		}
		if ( frame >= pKey2->key_no ) {
			pKey1 = pKey2;
			step = 0;
		} else {
			if ( (u32)pKey1 == (u32)pKey2 ) {
				step = 0;
			} else {
				step = (float)( frame - pKey1->key_no );
			}
		}
		// 位置と回転
		lx = (float)pKey1->lx;
		ly = (float)pKey1->ly;
		lz = (float)pKey1->lz;
		rx = (float)pKey1->rx;
		ry = (float)pKey1->ry;
		rz = (float)pKey1->rz;
		sx = (float)pKey1->sx;
		sy = (float)pKey1->sy;
		sz = (float)pKey1->sz;
		if ( step > 0 ) {
			step /= (float)( pKey2->key_no - pKey1->key_no );
			lx += ( (float)pKey2->lx - lx ) * step;
			ly += ( (float)pKey2->ly - ly ) * step;
			lz += ( (float)pKey2->lz - lz ) * step;
			rx += ( (float)pKey2->rx - rx ) * step;
			ry += ( (float)pKey2->ry - ry ) * step;
			rz += ( (float)pKey2->rz - rz ) * step;
			sx += ( (float)pKey2->sx - sx ) * step;
			sy += ( (float)pKey2->sy - sy ) * step;
			sz += ( (float)pKey2->sz - sz ) * step;
		}
		// 行列作成
		makeScaleMatrix(ms, FIXPOINT);
		ms[0][0] = (s32)( sx * 8.0 + 0.5 );
		ms[1][1] = (s32)( sy * 8.0 + 0.5 );
		ms[2][2] = (s32)( sz * 8.0 + 0.5 );
		ilx = (s32)( lx * 4096.0 );
		ily = (s32)( ly * 4096.0 );
		ilz = (s32)( lz * 4096.0 );
		irx = angleF2S( rx / 16.0 );
		iry = angleF2S( ry / 16.0 );
		irz = angleF2S( rz / 16.0 );
		makeMatrix(mt, irx, iry, irz, ilx, ily, ilz);
//		matrixMulL(mt, ms, m);
		matrixMulL(ms, mt, m);
		if ( pObj->parent == -1 ) {
			matrixMulL(m, pm, AnimProc[i].m);
		} else {
			matrixMulL(m, AnimProc[pObj->parent].m, AnimProc[i].m);
		}

		// Ｚ値を強制
		//AnimProc[i].m[3][2] = -128 * FIXPOINT;

		// ＤＬ定義
		if ( pObj->dl != NULL ) {
			matrixConv(AnimProc[i].m, pObjectMtx, 0);
			gSPMatrix(pgfx++, pObjectMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
			pObjectMtx++;
			gSPDisplayList(pgfx++, pObj->dl);
		}
	}

	*glp = pgfx;
	return(ret);
}
