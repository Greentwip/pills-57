
#if !defined(_CHAR_ANIME_H_)
#define _CHAR_ANIME_H_

#include "tex_func.h"

//////////////////////////////////////////////////////////////////////////////

#define _ASEQ_SET_REPEAT 0xf0
#define _ASEQ_EXE_REPEAT 0xf1
#define _ASEQ_GOTO       0xf2
#define _ASEQ_END        0xff

enum {
	ASEQ_NORMAL, // �ʏ�
	ASEQ_ATTACK, // �U��
	ASEQ_DAMAGE, // ��e
	ASEQ_WIN,    // ����
	ASEQ_LOSE,   // �s�k
	_ASEQ_SUM,
};

#define ASEQ_SET_REPEAT(n) _ASEQ_SET_REPEAT, (n)
#define ASEQ_EXE_REPEAT()  _ASEQ_EXE_REPEAT, 100
#define ASEQ_SET_LOOP()    _ASEQ_SET_REPEAT, 0xff
#define ASEQ_EXE_LOOP(n)   _ASEQ_EXE_REPEAT, (n)
#define ASEQ_GOTO(n)       _ASEQ_GOTO, (n)
#define ASEQ_END()         _ASEQ_END

typedef struct SAnimeSeq SAnimeSeq;

struct SAnimeSeq {
	// �J��Ԃ������p�̃X�^�b�N
	#define ANIME_STACK_DEPTH 4
	unsigned char labelStack[ANIME_STACK_DEPTH];
	unsigned char countStack[ANIME_STACK_DEPTH];
	int stackDepth;

	// �A�j���[�V�����V�[�P���X
	unsigned char **seqArray;

	// �A�j���[�V�����ԍ�
	int animeNo;

	// �V�[�P���X�i�s�J�E���^
	int seqCount;

	// �\�����̃e�N�X�`���ԍ�
	int textureNo;
};

extern void animeSeq_init(SAnimeSeq *st, unsigned char **seqArray, int animeNo);
extern void animeSeq_set(SAnimeSeq *st, int animeNo);
extern void animeSeq_update(SAnimeSeq *st, int step);
extern int  animeSeq_isEnd(SAnimeSeq *st);

//////////////////////////////////////////////////////////////////////////////

typedef struct SAnimeState SAnimeState;

struct SAnimeState {
	SAnimeSeq animeSeq[1];
	STexInfo *texArray;
	u32 frameCount;
	int center[2];
	int charNo;
	int color[4];
};

extern int  animeState_getDataSize(int charNo);
extern void animeState_load(SAnimeState *st, void **hpp, int charNo);
extern void animeState_init(SAnimeState *st, unsigned char **seqArray, STexInfo *texArray, int cx, int cy, int charNo);

extern void animeState_set(SAnimeState *st, int animeNo);
extern void animeState_update(SAnimeState *st);
extern int  animeState_isEnd(SAnimeState *st);

extern void animeState_initDL(SAnimeState *st, Gfx **gpp);
extern void animeState_initIntensityDL(SAnimeState *st, Gfx **gpp);

extern void animeState_draw(SAnimeState *st, Gfx **gpp, float x, float y, float sx, float sy);
extern void animeState_drawI(SAnimeState *st, Gfx **gpp, float x, float y, float sx, float sy);

//////////////////////////////////////////////////////////////////////////////

typedef struct SAnimeSmog SAnimeSmog;

struct SAnimeSmog {
	SAnimeState animeState[4];
	int pos[4][2];
	int frameCount;
};

extern void animeSmog_init(SAnimeSmog *st, SAnimeState *state);
extern void animeSmog_load(SAnimeSmog *st, void **hpp);
extern void animeSmog_start(SAnimeSmog *st);
extern void animeSmog_stop(SAnimeSmog *st);
extern void animeSmog_update(SAnimeSmog *st);
extern void animeSmog_draw(SAnimeSmog *st, Gfx **gpp, float x, float y, float sx, float sy);

//////////////////////////////////////////////////////////////////////////////

extern void mappingAnimeSeq(unsigned char **seqArray, int count, u32 segGap);

extern void loadAnimeSeq(void **hpp,
	STexInfo **texArray, unsigned char ***seqArray,
	void *romStart, void *romEnd);

//////////////////////////////////////////////////////////////////////////////

#endif // _CHAR_ANIME_H_
