/*-----------------------------------------------------------------------------
	virus initial routine

			make	m.urushibata
			date	1997/12/10
			ver.	1.0
-----------------------------------------------------------------------------*/
#include <ultra64.h>
#include "vr_init.h"

//================= mt random =======================================

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df	  /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/******************************************************************************
*****　ｍｔランダム ***********************************************************
******************************************************************************/
/*-----[ mt random initialize ]-----*/
void sgenrand(u32 seed)
{
	mt[0]= seed & 0xffffffff;
	for (mti=1; mti<N; mti++)
		mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

/*-----[ mt random ]-----*/
u16 genrand(u16 in_limit)
{
	u32	limit;
	u32	idx;
	u32	mask_work;
	u32	mask_cnt;
	u32 y;
	static u32 mag01[2]={0x0, MATRIX_A};
	/* mag01[x] = x * MATRIX_A	for x=0,1 */

	limit = (u32)in_limit;

	do {
		if (mti >= N) { /* generate N words at one time */
			int kk;

			if (mti == N+1)	  /* if sgenrand() has not been called, */
				sgenrand(4357); /* a default initial seed is used	*/

			for (kk=0;kk<N-M;kk++) {
				y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
				mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			for (;kk<N-1;kk++) {
				y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
				mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
			}
			y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
			mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

			mti = 0;
		}

		y = mt[mti++];
		y ^= TEMPERING_SHIFT_U(y);
		y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
		y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
		y ^= TEMPERING_SHIFT_L(y);

		// マスク
		mask_work = 0xffffffff & limit;
		mask_cnt = 0;
		do {
			mask_work = mask_work >> 1;
			mask_cnt++;
		} while ( mask_work != 0 );
		for ( ; mask_cnt != 0 ; mask_cnt-- ) {
			mask_work = mask_work << 1;
			mask_work++;
		}
		y = y & mask_work;

	} while ( y >= limit );

	return (u16)y;
}

