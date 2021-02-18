
#if !defined(_MSGWIN_H_)
#define _MSGWIN_H_

typedef struct {
	int top, end;
	int width;
	int color;
} SMsgLayout;

typedef struct {
	void *heap;

	SMsgLayout *layout;
	int layoutSize;

	unsigned char *msgBuf;
	int msgBufSize;
	int msgBufTop;
	int msgBufNow;

	int contFlags;
	int fontType;  // フォントの種類 0:普通, 1:縁あり
	int centering; // センタリングフラグ

	int posX;
	int posY;

	int fntW;
	int fntH;

	int colSize;
	int colStep;
	int colNow;

	int rowSize;
	int rowStep;
	int rowNow;

	float msgCount;
	float msgSpeed;

	float scrCount;
	float scrSpeed;

	int msgIsEnd;
	int isSpeak;
	int topColor;
	int keyWait;
	int countWait;

	int alpha;
	int scisFlag;

	unsigned int grapCount;
} SMsgWnd;

// 文字の色テーブル
extern const unsigned char _msgColorTbl[][3];

extern void msgWnd_init(SMsgWnd *st, void **heap, int cols, int rows, int x, int y);
extern void msgWnd_init2(SMsgWnd *st, void **heap, int bufSize, int cols, int rows, int x, int y);
extern void msgWnd_addStr(SMsgWnd *st, const unsigned char *str);
extern void msgWnd_update(SMsgWnd *st);
extern void msgWnd_draw(SMsgWnd *st, Gfx **gpp);
extern void msgWnd_clear(SMsgWnd *st);
extern int  msgWnd_isEnd(SMsgWnd *st);
extern void msgWnd_skip(SMsgWnd *st);
extern int  msgWnd_isSpeaking(SMsgWnd *st);
extern int  msgWnd_isScroll(SMsgWnd *st);
extern int msgWnd_getWidth(SMsgWnd *st);
extern int msgWnd_getHeight(SMsgWnd *st);

#endif // _MSGWIN_H_
