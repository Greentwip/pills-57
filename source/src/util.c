
#include <ultra64.h>
#include <math.h>

#include "def.h"
#include "nnsched.h"
#include "graphic.h"
#include "audio.h"
#include "util.h"
#include "debug.h"

//////////////////////////////////////////////////////////////////////////////
// 圧縮データを展開

#if 0 // 削除
/*
#define EXPAND_WORK_END fbuffer

int ExpandRomData(void *src, void *dest, u32 size)
{
	u32 work = ((u32)EXPAND_WORK_END - size) & ~0x0000000f;

	auRomDataRead((u32)src, (void *)work, size);

	return ExpandData((u8 *)work, (u8 *)dest);
}
*/
#endif

u32 ExpandGZip(void *src, void *dest, u32 size)
{
	u32 len;

	len = expand_gzip((char *)src, (char *)dest, size);

	return ((u32)dest + len + 7) & ~7;
}

//////////////////////////////////////////////////////////////////////////////
//{### ラップ

int WrapI(int low, int hi, int val)
{
	int len, ans;

	len = hi - low;
	ans = (val - low) % len;

	if(ans < 0)
		ans += len;

	return ans + low;
}

float WrapF(float low, float hi, float val)
{
	float len, ans;

	if(val >= low && val < hi)
		return val;

	len = hi - low;
	ans = fmod(val - low, len);

	if(ans < 0.f)
		ans += len;

	return ans + low;
}

//////////////////////////////////////////////////////////////////////////////
//{### EOF
