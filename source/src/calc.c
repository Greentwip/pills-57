#include	<ultra64.h>

#include	"def.h"
#include	"calc.h"

#define	RMAX 65536;
static	int	randomindex;
static	int	randomtable[55];

static	int	start_tbl[] = {
	0, 1, 2, 3, 4, 5, 6, 8,
	9, 10, 11, 12, 13, 14, 15, 17,
	18, 19, 20, 21, 22, 23, 24, 25,
	26, 28, 29, 30, 31, 32, 33, 34,
	35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 54, 55, 56, 57,
	58, 59, 60, 61, 61, 62, 63, 64,
	65, 66, 66, 67, 68, 69, 69, 70,
	71, 72, 73, 73, 74, 75, 75, 76,
	77, 78, 78, 79, 80, 80, 81, 82,
	82, 83, 83, 84, 85, 85, 86, 87,
	87, 88, 88, 89, 90
};
static	float	tan_table[] = {
	0.0000000000, 0.0087268678, 0.0174550656, 0.0261859223, 0.0349207707, 0.0436609425, 0.0524077788, 0.0611626208,
	0.0699268132, 0.0787017047, 0.0874886662, 0.0962890461, 0.1051042378, 0.1139356047, 0.1227845624, 0.1316525042,
	0.1405408382, 0.1494510025, 0.1583844423, 0.1673426032, 0.1763269752, 0.1853390485, 0.1943803132, 0.2034523040,
	0.2125565559, 0.2216946632, 0.2308681905, 0.2400787622, 0.2493280023, 0.2586175799, 0.2679491937, 0.2773245573,
	0.2867453992, 0.2962135077, 0.3057306707, 0.3152987957, 0.3249197006, 0.3345953226, 0.3443275988, 0.3541185856,
	0.3639702201, 0.3738846779, 0.3838640451, 0.3939104676, 0.4040262401, 0.4142135680, 0.4244748056, 0.4348123670,
	0.4452286959, 0.4557262659, 0.4663076699, 0.4769755304, 0.4877325892, 0.4985816181, 0.5095254779, 0.5205670595,
	0.5317094326, 0.5429556966, 0.5543090701, 0.5657727718, 0.5773502588, 0.5890449882, 0.6008605957, 0.6128007770,
	0.6248693466, 0.6370702386, 0.6494075656, 0.6618855596, 0.6745085120, 0.6872809529, 0.7002075315, 0.7132930756,
	0.7265425324, 0.7399610877, 0.7535540462, 0.7673270106, 0.7812856436, 0.7954359055, 0.8097840548, 0.8243364096,
	0.8390996456, 0.8540806770, 0.8692867160, 0.8847252727, 0.9004040360, 0.9163311720, 0.9325150847, 0.9489645958,
	0.9656887650, 0.9826972485, 1.0000000000, 999.00000000
};


/*
 *----------------------------------------
 *   tan
 *		func:	tan(a)
 *----------------------------------------
 */
float	tanf(float a)
{
	return( sinf(a) / cosf(a) );
}



/*
 *----------------------------------------
 *   matrixMulF
 *		func:	s * d = r
 *----------------------------------------
 */
void	matrixMulF(FMatrix s, FMatrix d, FMatrix r)
{
	int		i, j, k;
	FMatrix	res;

	for ( i = 0; i < 4; i++ ) {
		for ( j = 0; j < 4; j++ ) {
			res[i][j] = 0;
			for ( k = 0; k < 4; k++ ) {
				res[i][j] += s[i][k] * d[k][j];
			}
		}
	}

	for ( i = 0; i < 4; i++ ) {
		for ( j = 0; j < 4; j++ ) {
			r[i][j] = res[i][j];
		}
	}
}



/*
 *----------------------------------------
 *   makeMatrixF
 *----------------------------------------
 */
void	makeMatrixF(FMatrix m, float rx, float ry, float rz, float lx, float ly, float lz)
{
	float	sx, sy, sz, cx, cy, cz;

	rx = DEG2RAD(rx);
	ry = DEG2RAD(ry);
	rz = DEG2RAD(rz);

	sx = sinf(rx);
	sy = sinf(ry);
	sz = sinf(rz);
	cx = cosf(rx);
	cy = cosf(ry);
	cz = cosf(rz);

	m[0][0] = sz*sx*sy + cz*cy;
	m[0][1] = sz*cx;
	m[0][2] = sz*sx*cy - cz*sy;
	m[1][0] = cz*sx*sy - sz*cy;
	m[1][1] = cz*cx;
	m[1][2] = cz*sx*cy + sz*sy;
	m[2][0] = cx*sy;
	m[2][1] = -sx;
	m[2][2] = cx*cy;
	m[3][0] = lx;
	m[3][1] = ly;
	m[3][2] = lz;
	m[0][3] = m[1][3] = m[2][3] = 0; m[3][3] = 1;
}



/*
 *----------------------------------------
 *   angle2vct
 *		func:		
 *		input:	short	pitch
 *				short	yaw
 *		output:	float x
 *				float y
 *				float z
 *----------------------------------------
 */
void	angle2vct(s16 pit, s16 yaw, float *x, float *y, float *z)
{
	float	sx, sy, cx, cy, rx, ry;

	rx = angleS2F(pit);
	ry = angleS2F(yaw);
	rx = DEG2RAD(rx);
	ry = DEG2RAD(ry);
	sx = sinf(rx);
	sy = sinf(ry);
	cx = cosf(rx);
	cy = cosf(ry);

	*x = cx*sy;
	*y = -sx;
	*z = cx*cy;
}


/*
 *----------------------------------------
 *   vct2angle
 *		func:		
 *		input:	float x
 *				float y
 *				float z
 *		output:	short	pitch
 *				short	yaw
 *----------------------------------------
 */
void	vct2angle(float x, float y, float z, s16 *pit, s16 *yaw)
{
	s16		xr, yr;

	yr = angleF2S( get_angleF(z, x) );
	z = sqrtf( x*x + z*z );
	if ( yr & 0x8000 ) {
		if ( x >= 0 ) z = -z;
	}
	xr = angleF2S( get_angleF(z, -y) );

	*pit = xr;
	*yaw = yr;
}


/*
 *----------------------------------------
 *   get_angleF
 *		func:		
 *		input:	float t
 *		retuen:	float angle  ( -90 ～ +90 )
 *----------------------------------------
 */
float	atanF(float t)
{
	float	wk, ang;
	unsigned	char	flag;
	int		i;

	flag = 0;
	if ( t == 0 ) return(0);
	if ( t < 0 ) {
		t = -t;
		flag |= 0x10;
	}
	if ( t == 1 ) {
		if ( flag == 0 ) {
			return(45);
		} else {
			return(-45);
		}
	}
	if ( t > 1 ) {
		t = 1 / t;
		flag |= 0x01;
	}

	wk = t;

	i = (int)( wk * 100 );
	i = start_tbl[i];
	while ( tan_table[i] <= wk ) i++;
	i--;
	ang = (float)i;
	if ( wk != tan_table[i] ) {
		wk -= tan_table[i];
		wk = wk / ( tan_table[i+1] - tan_table[i] );
		ang += wk;
	};
	ang /= 2;

	if ( flag & 0x01 ) ang = 90 - ang;
	if ( flag & 0x10 ) ang = -ang;

	return( ang );
}


/*
 *----------------------------------------
 *   get_angleF
 *		func:		
 *		input:	float x
 *				float y
 *		retuen:	float angle
 *----------------------------------------
 */
float	get_angleF(float x, float y)
{
	float	wk, ang;
	unsigned	long		flag;
	int		i;

	flag = 0;
	if ( x == 0 ) {
		if ( y == 0 ) {
			return(0);
		} else {
			if ( y < 0 )
				return(-90);
			else
				return(90);
		}
	}
	if ( x < 0 ) {
		x = -x;
		flag ^= 0x0110;
	}
	if ( y == 0 ) {
		if ( flag == 0 )
			return(0);
		else
			return(180);
	}
	if ( y < 0 ) {
		y = -y;
		flag ^= 0x0010;
	}
	if ( x < y ) {
		wk = x;
		x = y;
		y = wk;
		flag ^= 0x0001;
	}

	wk = y / x;

	i = (int)( wk * 100 );
	i = start_tbl[i];
	while ( tan_table[i] <= wk ) i++;
	i--;

	ang = (float)i;
	if ( wk != tan_table[i] ) {
		wk -= tan_table[i];
		wk = wk / ( tan_table[i+1] - tan_table[i] );
		ang += wk;
	};
	ang /= 2;

	if ( flag & 0x0001 ) ang = 90 - ang;
	if ( flag & 0x0010 ) ang = -ang;
	if ( flag & 0x0100 ) ang += 180;

	if ( ang > 180 ) ang -= 360;
	if ( ang < -180 ) ang += 360;

	return( ang );
}


/*
 *----------------------------------------
 * Short Angle to Float Angle
 *----------------------------------------
 */
float	angleS2F(short a)
{
	float   ang;

	ang = (float)a;
	ang *= 90.0 / 16384.0;
	return(ang);
}


/*
 *----------------------------------------
 * Float Angle to Short Angle
 *----------------------------------------
 */
short	angleF2S(float a)
{
	a *= 16384.0 / 90.0;
	return( (short)a );
}


/*
 *----------------------------------------
 * Long Matrix to Float Matrix
 *----------------------------------------
 */
void	matrixL2F(FMatrix fm, LMatrix lm)
{
	int		i;

	for ( i = 0; i < 4; i++ ) {
		fm[i][0] = ( (float)(lm[i][0]) ) / FIXPOINT;
		fm[i][1] = ( (float)(lm[i][1]) ) / FIXPOINT;
		fm[i][2] = ( (float)(lm[i][2]) ) / FIXPOINT;
		fm[i][3] = ( (float)(lm[i][3]) ) / FIXPOINT;
	}
}


/*
 *----------------------------------------
 * Float Matrix to Long Matrix
 *----------------------------------------
 */
void	matrixF2L(LMatrix lm, FMatrix fm)
{
	int		i;

	for ( i = 0; i < 4; i++ ) {
		lm[i][0] = (s32)( (fm[i][0]) * FIXPOINT );
		lm[i][1] = (s32)( (fm[i][1]) * FIXPOINT );
		lm[i][2] = (s32)( (fm[i][2]) * FIXPOINT );
		lm[i][3] = (s32)( (fm[i][3]) * FIXPOINT );
	}
}


/*
 *----------------------------------------
 * Float Matrix to Long Matrix
 *----------------------------------------
 */
void	matrixF2Lloc0(LMatrix lm, FMatrix fm)
{
	int		i;

	for ( i = 0; i < 3; i++ ) {
		lm[i][0] = (s32)( (fm[i][0]) * FIXPOINT );
		lm[i][1] = (s32)( (fm[i][1]) * FIXPOINT );
		lm[i][2] = (s32)( (fm[i][2]) * FIXPOINT );
		lm[i][3] = (s32)( (fm[i][3]) * FIXPOINT );
	}
	lm[3][0] = 0;
	lm[3][1] = 0;
	lm[3][2] = 0;
	lm[3][3] = FIXPOINT;
}


/*
 *----------------------------------------
 *   rotpoint
 *		func:	Rotate Point  Center O(x0,y0)  Point P(x,y) 
 *		input:	float a		Rot Angle
 *				float x0	Rot OrgX
 *				float y0	Rot OrgY
 *				float *x	Point X
 *				float *y	Point Y
 *		output:	float *x	Point X'
 *				float *y	Point Y'
 *----------------------------------------
 */
void	rotpoint(float a, float x0, float y0, float *x, float *y)
{
	float   sa, ca, dx, dy;

	sa = sinf(a*M_DTOR);
	ca = cosf(a*M_DTOR);
	dx = *x - x0;
	dy = *y - y0;
	*x = ( dx * ca - dy * sa ) + x0;
	*y = ( dy * ca + dx * sa ) + y0;
}


/*
 *----------------------------------------
 *   defangle
 *		func:	deff angle
 *		input:	float a1	Angle1
 *				float a2	Angle2
 *		output:	none
 *		return:	float		Diff Angle
 *----------------------------------------
 */
float	defangle(float a1, float a2)
{
	long	angle1, angle2, angle01, angle02, d;

	angle1 = (long)angleF2S(a1) & 0x0000ffff;
	angle2 = (long)angleF2S(a2) & 0x0000ffff;

	if ( angle1 < angle2 ) {
		angle01 = angle1 + 0x10000;
		angle02 = angle2;
	} else {
		angle01 = angle1;
		angle02 = angle2 + 0x10000;
	}
	angle2 -= angle1;
	angle02 -= angle01;
	if ( ABS(angle2) > ABS(angle02) ) d = angle02; else d = angle2;

	return( angleS2F((s16)d) );
}


/*
 *----------------------------------------
 *   distance
 *		func:	Distance P1 to P2
 *		input:	float x1, y1, z1
 *				float x2, y2, z2
 *		output:	none
 *		return:	float		Distance
 *----------------------------------------
 */
float	distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	float   x, y, z;

	x = x2 - x1;
	y = y2 - y1;
	z = z2 - z1;
	return( sqrtf(x*x + y*y + z*z) );
}


/*
 *----------------------------------------
 *   calcTableF
 *		func:	
 *		input:	float	x
 *				float	*table
 *		output:	none
 *		return:	float	data
 *----------------------------------------
 */
float	calcTableF(float x, float *table)
{
	float	res;
	float	min, max;
	s32		n, h, l;

	min = table[0];
	max = table[1];

	if ( x < min ) x = min;
	if ( x > max ) x = max;

	n = (s32)( 256 * 256 * ( x - min ) / ( max - min ) );

	h = n >> 8;
	l = n & 0x00ff;

	res = table[h+2];
	if ( l != 0 ) {
		res = ( (float)l * ( table[h+3] - res ) / 256.0 ) + res;
	}

	return(res);
}


float	calcTableF_Rev(float x, float *table, float s, float e)
{
	float	res, old;
	float	min, max;
	s32		n, h, l, n1, n2;

	min = table[0];
	max = table[1];

	if ( x < min ) x = min;
	if ( x > max ) x = max;


	n1 = (s32)( 256 * 256 * ( s - min ) / ( max - min ) );
	n2 = (s32)( 256 * 256 * ( e - min ) / ( max - min ) );
	h = n1 >> 8;
	l = n2 >> 8;
	old = 9999999;
	while ( h < l ) {
		res = ABS( x - table[h+2+1] );
		if ( res < old ) {
			old = res;
			h++;
		} else {
			break;
		}
	}

	res = table[h+2];
	old = table[h+2-1];

	res = table[h+2+1] - table[h+2-1];
	old = x - table[h+2-1];
	res = old * 2 / res;

	res = 2 * ( x - table[h+2-1] ) / ( table[h+2+1] - table[h+2-1] ) + (float)(h-1);
	res = min + ( res * ( max - min ) / 256 );

	return(res);
}


/*
 *----------------------------------------
 *   mtx2angleF
 *		func:	行列から角度を求める
 *		input:	LMatrix	Matrix
 *		output:	float	rx
 *				float	ry
 *				float	rz
 *		return:	none
 *----------------------------------------
 */
void	mtx2angleF(FMatrix m, float *rx, float *ry, float *rz)
{
	float	xr, yr, zr;
	float	wk;

	if ( ( m[2][0] == 0 ) && ( m[2][2] == 0 ) ) {
		if ( m[2][1] > 0 ) xr = -0x4000; else xr = 0x4000;
		yr = get_angleF(m[0][0], -m[0][2]);
		zr = 0;
/*
		zr = get_angleF(m[0][0], -m[1][0]);
		yr = 0;
*/
	} else {
		wk = sqrtf( m[2][0]*m[2][0] + m[2][2]*m[2][2] );
		zr = get_angleF(m[1][1], m[0][1]);
		yr = get_angleF(m[2][2], m[2][0]);
/*
		if ( yr & 0x8000 ) {
			if ( m[2][0] >= 0 ) wk = -wk;
		}
*/
		xr = get_angleF(wk, -m[2][1]);

	}
	*rx = xr;
	*ry = yr;
	*rz = zr;
}



/*
 *----------------------------------------
 *   mtx2angleL
 *		func:	行列から角度を求める
 *		input:	LMatrix	Matrix
 *		output:	s16		rx
 *				s16		ry
 *				s16		rz
 *		return:	none
 *----------------------------------------
 */
void	mtx2angleL(LMatrix m, s16 *rx, s16 *ry, s16 *rz)
{
	s32	xr, yr, zr;
	s32	wk;

	if ( ( m[2][0] == 0 ) && ( m[2][2] == 0 ) ) {
		if ( m[2][1] > 0 ) xr = -0x4000; else xr = 0x4000;
		yr = get_angleL(m[0][0], -m[0][2]);
		zr = 0;
/*
		zr = get_angleL(m[0][0], -m[1][0]);
		yr = 0;
*/
	} else {
		wk = sqrt_a2b2(m[2][0], m[2][2]);
		zr = get_angleL(m[1][1], m[0][1]);
		yr = get_angleL(m[2][2], m[2][0]);
/*
		if ( yr & 0x8000 ) {
			if ( m[2][0] >= 0 ) wk = -wk;
		}
*/
		xr = get_angleL(wk, -m[2][1]);

	}
	*rx = (s16)xr;
	*ry = (s16)yr;
	*rz = (s16)zr;
}



/*
 *----------------------------------------
 * lc2wcF
 *		func:	座標位置をマトリクスで変換
 *		input:	FMatrix	m	変換行列（この行列でアフィン変換）
 *				float	x	X値
 *				float	y	Y値
 *				float	z	Z値
 *		output:	float	wx	変換後のX値
 *				float	wy	変換後のX値
 *				float	wz	変換後のX値
 *----------------------------------------
 */
void	lc2wcF(FMatrix m, float lx, float ly, float lz, float *wx, float *wy, float *wz)
{
	int	i;
	float	r[3];

	for ( i = 0; i < 3; i++ ) {
		r[i] = m[3][i] + ( m[0][i]*lx + m[1][i]*ly + m[2][i]*lz );
	}
	*wx = r[0];
	*wy = r[1];
	*wz = r[2];
}



/*
 *----------------------------------------
 * wc2lcF
 *		func:	マトリクスから見た座標位置へ変換
 *		input:	FMatrix	m	変換行列（この行列を原点軸とした座標系へ変換）
 *				float	x	X値
 *				float	y	Y値
 *				float	z	Z値
 *		output:	float	wx	変換後のX値
 *				float	wy	変換後のX値
 *				float	wz	変換後のX値
 *----------------------------------------
 */
void	wc2lcF(FMatrix m, float wx, float wy, float wz, float *lx, float *ly, float *lz)
{
	int	i;
	float	r[3];

	wx -= m[3][0];
	wy -= m[3][1];
	wz -= m[3][2];
	for ( i = 0; i < 3; i++ ) {
		r[i] = m[i][0]*wx + m[i][1]*wy + m[i][2]*wz;
	}
	*lx = r[0];
	*ly = r[1];
	*lz = r[2];
}
// 位置を先に処理してあるバージョン
void	wc2lcF0(FMatrix m, float wx, float wy, float wz, float *lx, float *ly, float *lz)
{
	int	i;
	float	r[3];

	for ( i = 0; i < 3; i++ ) {
		r[i] = m[i][0]*wx + m[i][1]*wy + m[i][2]*wz;
	}
	*lx = r[0];
	*ly = r[1];
	*lz = r[2];
}



/*
 *----------------------------------------
 * makeMatrixVctZ
 *		func:	マトリクスから見た座標位置へ変換
 * S = sin (a)
 * C = cos (a)
 * H = sqrt (X*X + Z*Z)
 *
 * [   v   ]  [  C  S        ]  [  1           ]  [ -Z/H  X/H    ]
 *            [ -S  C        ]  [     H  Y     ]  [     1        ]
 *            [        1     ]  [    -Y  H     ]  [ -X/H -Z/H    ]
 *            [           1  ]  [           1  ]  [           1  ]
 *
 * if ( ABS(pitch) > 90 ) rz += 180.0;
 *		or
 * -90 <= pitch <= 90
 *
 *----------------------------------------
 */
void	makeMatrixVctZ(FMatrix mf, float a, float x, float y, float z, float lx, float ly, float lz)
{
	float	s, c, h, hinv;

	normalVct(&x, &y, &z);

	a = DEG2RAD(a);
	s = sinf(a);
	c = cosf(a);
	h = sqrtf(x*x + z*z);

	if ( h != 0 ) {
		hinv = 1 / h;

		mf[0][0] = (z*c - s*y*x) * hinv;
		mf[0][1] = s*h;
		mf[0][2] = -(x*c + s*y*z) * hinv;
		mf[0][3] = 0;

		mf[1][0] = -(z*s + c*y*x) * hinv;
		mf[1][1] = c*h;
		mf[1][2] = (x*s - c*y*z) * hinv;
		mf[1][3] = 0;

		mf[2][0] = x;
		mf[2][1] = y;
		mf[2][2] = z;
		mf[2][3] = 0;

		mf[3][0] = lx;
		mf[3][1] = ly;
		mf[3][2] = lz;
		mf[3][3] = 1;
	} else {
		mf[0][0] = c;
		mf[0][1] = 0;
		mf[0][2] = -y*s;
		mf[0][3] = 0;

		mf[1][0] = -s;
		mf[1][1] = 0;
		mf[1][2] = -y*c;
		mf[1][3] = 0;

		mf[2][0] = 0;
		mf[2][1] = y;
		mf[2][2] = 0;
		mf[2][3] = 0;

		mf[3][0] = lx;
		mf[3][1] = ly;
		mf[3][2] = lz;
		mf[3][3] = 1;
	}
}



/*
 *----------------------------------------
 *   normalVct
 *		func:	ベクトルの正規化
 *		input:	float	vx	元のベクトル
 *				float	vy
 *				float	vz
 *		output:	float	vx	正規化後のベクトル
 *				float	vy
 *				float	vz
 *----------------------------------------
 */
float	normalVct(float *vx, float *vy, float *vz)
{
	float	a, x, y, z;

	x = *vx;
	y = *vy;
	z = *vz;
	a = sqrtf( x*x + y*y + z*z );
	if ( a != 0 ) {
		x /= a;
		y /= a;
		z /= a;
	}
	*vx = x;
	*vy = y;
	*vz = z;

	return(a);
}



//---------------------------------------------------------------------------
static	void	randomize00(void)
{
	int	i,j;
	for ( i = 0; i < 24; i++ ) {
		j = randomtable[i] - randomtable[i+31];
		if ( j >= 0 ) randomtable[i] = j; else randomtable[i] = j + RMAX;
	}
	for ( i = 24; i < 55; i++ ) {
		j = randomtable[i] - randomtable[i-24];
		if ( j >= 0 ) randomtable[i] = j; else randomtable[i] = j + RMAX;
	}
}



//---------------------------------------------------------------------------
void	randomseed(int s)
{
	int	i,j,k;

	randomtable[54] = s;
	k = 1;
	for ( i = 1; i <= 54; i++ ) {
		j = ( 21 * i % 55 ) - 1;
		randomtable[j] = k;
		k = s - k;
		if ( k < 0 ) k += RMAX;
		s = randomtable[j];
    }
	randomize00(); randomize00(); randomize00(); randomize00();
	randomindex = 0;
}



//---------------------------------------------------------------------------
// ０～６５５３５までの間の乱数を返す
int	irandom(void)
{
	randomindex++;
	if ( randomindex >= 55 ) {
		randomize00();
		randomindex = 0;
	}
	return( randomtable[randomindex] & 0xffff );
}



//---------------------------------------------------------------------------
// ０～ｎ－１までの間の乱数を返す
int	random(int n)
{
	return( ( n * irandom() ) >> 16 );
}
