#if !defined(_UTIL_H_)
#define _UTIL_H_

//#include <ultra64.h>
#include <stdint.h>


//////////////////////////////////////////////////////////////////////////////
//{### �}�N��

//## �Z�O�����g�̐錾
#define MSegDecl(seg) \
	extern char _##seg##SegmentStart[], _##seg##SegmentEnd[]; \
	extern char _##seg##SegmentRomStart[], _##seg##SegmentRomEnd[]; \
	extern char _##seg##SegmentTextStart[], _##seg##SegmentTextEnd[]; \
	extern char _##seg##SegmentDataStart[], _##seg##SegmentDataEnd[]; \
	extern char _##seg##SegmentBssStart[], _##seg##SegmentBssEnd[];

//## �Z�O�����g�A�h���X����q�n�l�A�h���X�ɕϊ�
#define MSeg2Rom(seg, off) \
	((u32)(off) - (u32)_##seg##SegmentStart + (u32)_##seg##SegmentRomStart)

// �Z�O�����g�T�C�Y���擾
#define MSegSize(seg) ((u32)_##seg##SegmentEnd - (u32)_##seg##SegmentStart)
#define MSegRomSize(seg) ((u32)_##seg##SegmentRomEnd - (u32)_##seg##SegmentRomStart)
#define MSegTextSize(seg) ((u32)_##seg##SegmentTextEnd - (u32)_##seg##SegmentTextStart)
#define MSegDataSize(seg) ((u32)_##seg##SegmentDataEnd - (u32)_##seg##SegmentDataStart)
#define MSegBssSize(seg) ((u32)_##seg##SegmentBssEnd - (u32)_##seg##SegmentBssStart)

//## �P�U�o�C�g�A���C��
#define ALIGN_16(p) (((u32)(p) + 15) & ~15)

//## �U�S�o�C�g�A���C��
#define ALIGN_64(p) (((u32)(p) + 63) & ~63)

//## �z��̗v�f�����擾
#define ARRAY_SIZE(ary) (sizeof((ary)) / sizeof(*(ary)))

//## �ϐ��̒l���X���b�v
#define SWAP(l, r, t) ((void)((t)=(l),(l)=(r),(r)=(t)))

//## �N�����v
#define CLAMP(l, h, v) ((v) < (l) ? (l) : (v) > (h) ? (h) : (v))

//## �u�[��
typedef enum { false, true } bool;

//////////////////////////////////////////////////////////////////////////////

extern uint32_t ExpandGZip(void *src, void *dest, uint32_t size);

//////////////////////////////////////////////////////////////////////////////
//{### ���b�v

extern int WrapI(int low, int hi, int val);
extern float WrapF(float low, float hi, float val);

//////////////////////////////////////////////////////////////////////////////

#endif // _UTIL_H_

