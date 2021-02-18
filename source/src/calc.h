#ifndef	__calc__
#define	__calc__

#include "vector.h"
#include "matrix.h"

#define	GRAVITY			9.80665		// 重力加速度
#define	AIR_DENSITY		0.12492		// 空気密度

#define	SFT_FIXPOINT	15
#define	FIXPOINT		32768

#define	SEC				FRAME_RATE

#ifndef ABS
#define ABS(N)		( ( (N) < 0 ) ? -(N) : (N) )
#endif

#ifndef SGN
#define SGN(N)		( ( (N) < 0 ) ? -1 : 1 )
#endif

#ifndef MAX
#define MAX(X, Y)	( ( (X) > (Y) ) ? (X) : (Y) )
#endif

#ifndef MIN
#define MIN(X, Y)	( ( (X) < (Y) ) ? (X) : (Y) )
#endif


#define	SetFlag(n,f)	( n |= (f) )
#define	ClearFlag(n,f)	( n &= ~(f) )
#define	FlipFlag(n,f)	( n ^= (f) )


typedef	s32		LMatrix[4][4];
typedef	s32		LVector[3];
typedef	float	FVector[3];

#define PI			3.14159265358979323846
#define DEG2RAD(a)	( (a) * PI / 180.0 )
#define RAD2DEG(a)	( 180.0 * (a) / PI )

#define	KNOT	0.51444
#define	LB		0.45359
#define	FEET	0.3048
#define	NM		1851.98

#define	ANGLE4(r)	( ( ( (r) + 0x2000 ) >> 14 ) & 0x03 )
#define	ANGLE8(r)	( ( ( (r) + 0x1000 ) >> 13 ) & 0x07 )
#define	ANGLE16(r)	( ( ( (r) + 0x0800 ) >> 12 ) & 0x0f )

#define	time2BCDmin(N)	( time2BCD((N), 1) )
#define	time2BCDsec(N)	( time2BCD((N), 0) )

extern	float	tanf(float a);
extern	void	matrixMulF(FMatrix s, FMatrix d, FMatrix r);
extern	void	makeMatrixF(FMatrix m, float rx, float ry, float rz, float lx, float ly, float lz);
extern	void	makeMatrixVctZ(FMatrix mf, float a, float x, float y, float z, float lx, float ly, float lz);
extern	float	normalVct(float *vx, float *vy, float *vz);
extern	void	vct2angle(float x, float y, float z, s16 *pit, s16 *yaw);
extern	void	angle2vct(s16 pit, s16 yaw, float *x, float *y, float *z);
extern	float	atanF(float t);
extern	float	get_angleF(float x, float y);
extern	float	angleS2F(short a);
extern	short	angleF2S(float a);
extern	void	matrixL2F(FMatrix fm, LMatrix lm);
extern	void	matrixF2L(LMatrix lm, FMatrix fm);
extern	void	matrixF2Lloc0(LMatrix lm, FMatrix fm);
extern	void	lc2wcF(FMatrix m, float lx, float ly, float lz, float *wx, float *wy, float *wz);
extern	void	wc2lcF(FMatrix m, float wx, float wy, float wz, float *lx, float *ly, float *lz);
extern	void	wc2lcF0(FMatrix m, float wx, float wy, float wz, float *lx, float *ly, float *lz);
extern	void	rotpoint(float a, float x0, float y0, float *x, float *y);
extern	float	defangle(float a1, float a2);
extern	float	distance(float x1, float y1, float z1, float x2, float y2, float z2);
extern	float	calcTableF(float x, float *table);
extern	float	calcTableF_Rev(float x, float *table, float s, float e);
extern	u32		time2BCD(u32 time, int flag);

extern	void	mtx2angleF(FMatrix m, float *rx, float *ry, float *rz);
extern	void	mtx2angleL(LMatrix m, s16 *rx, s16 *ry, s16 *rz);

extern	void	randomseed(int s);
extern	int		irandom(void);
extern	int		random(int n);

/*
 * ASM routine
 */
extern	void	makeXrotMatrix(LMatrix im, s16 xrot, s32 xofs, s32 yofs, s32 zofs);
extern	void	makeYrotMatrix(LMatrix im, s16 yrot, s32 xofs, s32 yofs, s32 zofs);
extern	void	makeZrotMatrix(LMatrix im, s16 zrot, s32 xofs, s32 yofs, s32 zofs);
extern	void	makeXZMatrix(LMatrix m, s16 xrot, s16 zrot);
extern	void	makeMatrix(LMatrix im, s16 xrot, s16 yrot, s16 zrot, s32 xofs, s32 yofs, s32 zofs);
extern	void	makeTransrateMatrix(LMatrix m, s32 xofs, s32 yofs, s32 zofs);
extern	void	makeScaleMatrix(LMatrix m, s32 scale);
extern	void	makeVect(s16 xrot, s16 yrot, s32 *vx, s32 *vy, s32 *vz);
extern	void	lc2wc(LMatrix m, s32 x, s32 y, s32 z, s32 *px, s32 *py, s32 *pz);
extern	void	wc2lc(LMatrix m, s32 x, s32 y, s32 z, s32 *px, s32 *py, s32 *pz);

extern	void	matrixMulL(LMatrix m, LMatrix n, LMatrix r);
extern	void	matrixCopyL(LMatrix dst, LMatrix src);
extern	void	rotpointL(s16 a, s32 ox, s32 oy, s32 *x, s32 *y);

extern	void	matrixConv(LMatrix lmat, Mtx *mtx, s32 shift);

extern	s32		get_angleL(s32 x, s32 y);
extern	s32		sinL(s16 a);
extern	s32		cosL(s16 a);
extern	s32		sqrtS(u32 v);

extern	s32		defangleL(s16 a1, s16 a2);
extern	s32		distanceS(s32 x1, s32 y1, s32 z1, s32 x2, s32 y2, s32 z2);
extern	s32		distanceL(s32 x1, s32 y1, s32 z1, s32 x2, s32 y2, s32 z2);
extern	s32		sqrt_a2b2(s32 a, s32 b);
extern	s32		sqrt_abc(s32 a, s32 b, s32 c);
extern	s32		muldiv(s32 a, s32 b, s32 c);

extern	int		waitEQ4(int *var, int data);

#endif
