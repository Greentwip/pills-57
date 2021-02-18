
#include <ultra64.h>
#ifndef LINUX_BUILD
#include <memory.h>
#endif
#include "def.h"
#include "nnsched.h"
#include "util.h"
#include "joy.h"
#include "font.h"
#include "msgwnd.h"
#include "debug.h"

#define MSG_BUF_SIZE 0x0400
#define WORD_MARGIN  2

#define _getKeyTrg(playerNo) ((int)joyupd[main_joy[(playerNo)]])
#define _getKeyRep(playerNo) ((int)joycur[main_joy[(playerNo)]])

#define CONT_OK     (CONT_A | CONT_START)
#define CONT_CANCEL (CONT_B)

//////////////////////////////////////
// 文字の色
const unsigned char _msgColorTbl[][3] = {
	{   0,   0,   0 },
	{   0,   0, 255 },
	{   0, 200,   0 },
	{   0, 200, 200 },
	{ 220,  50,   0 },
	{ 210,   0, 210 },
	{ 220, 140,   0 },
	{ 255, 255, 255 },
};

//////////////////////////////////////
// 何かキーが押されているかチェック
static int _checkKeyTrg(int contFlags, int keyFlags)
{
	int i;

	for(i = 0; i < MAXCONTROLLERS; i++) {
		if((contFlags & (1 << i)) && (_getKeyTrg(i) & keyFlags)) {
			break;
		}
	}

	return i < MAXCONTROLLERS;
}

//////////////////////////////////////
// 初期化
void msgWnd_init(SMsgWnd *st, void **heap, int cols, int rows, int x, int y)
{
	int size = MSG_BUF_SIZE;
	size -= (u8 *)ALIGN_16(*heap) - (u8 *)(*heap);
	size -= sizeof(SMsgLayout) * (rows + 1);
	msgWnd_init2(st, heap, size, cols, rows, x, y);
}
void msgWnd_init2(SMsgWnd *st, void **heap, int bufSize, int cols, int rows, int x, int y)
{
	void *hp = st->heap = *heap;

	st->layout = (SMsgLayout *)ALIGN_16(hp);
	st->layoutSize = rows + 1;
	hp = st->layout + st->layoutSize;

	st->msgBuf = hp;
	st->msgBufSize = bufSize;
	hp = st->msgBuf + bufSize;

	st->contFlags = 1;
	st->fontType = 0;
	st->centering = 0;

	st->posX = x;
	st->posY = y;

	st->fntW = FONT_WORD_W;
	st->fntH = FONT_WORD_H;

	st->colSize = cols * 2;
	st->colStep = FONT_WORD_W / 2;

	st->rowSize = rows;

	st->rowStep = FONT_WORD_H + 2;

	st->msgSpeed = 0.25;
	st->scrSpeed = 1.0 / 8.0;

	st->alpha = 255;
	st->scisFlag = 1;

	*heap = hp;

	msgWnd_clear(st);
}

//////////////////////////////////////
// クリア
void msgWnd_clear(SMsgWnd *st)
{
	st->msgBuf[0] = FONT_CTRL;
	st->msgBuf[1] = FONT_CTRL_EOS;

	st->msgBufTop = 0;
	st->msgBufNow = 0;

	st->colNow = 0;
	st->rowNow = 0;

	st->msgCount = 0.0;
	st->scrCount = 0.0;

	st->msgIsEnd = 0;
	st->isSpeak = 0;
	st->topColor = 7;
	st->keyWait = 0;
	st->countWait = 0;
	st->grapCount = 0;
}

//////////////////////////////////////
// レイアウト
static void msgWnd_layout(SMsgWnd *st)
{
	int loopFlag = 1;
	int top = st->msgBufTop;
	int col = 0;
	int row = 0;
	int space = st->colStep * 2 - st->fntW;
	int width = msgWnd_getWidth(st);
	int color = st->topColor;
	float fnt2scr = (float)st->fntW / (float)FONT_WORD_W;

	st->layout[row].top = top;
	st->layout[row].color = color;

	while(loopFlag) {
		int layoutFlag = 0;

		if(st->msgBuf[top] == FONT_CTRL) {
			switch(st->msgBuf[top + 1]) {
			case FONT_CTRL_RETURN:
				layoutFlag = 1;
				break;
			case FONT_CTRL_INKEY:
				layoutFlag = (col + st->colStep > width);
				break;
			case FONT_CTRL_EOS:
				layoutFlag = 1;
				loopFlag = 0;
				break;
			case FONT_CTRL_TIMER:
				break;
			case FONT_CTRL_CLR:
				layoutFlag = 1;
				break;
			default:
				color = st->msgBuf[top + 1] - '0';
				break;
			}
			top += fontStr_nextChar(st->msgBuf + top);
		}
		else {
			int step = fontStr_charSize(st->msgBuf + top, st->fontType) * fnt2scr + space;

			if(col + step <= width) {
				top += fontStr_nextChar(st->msgBuf + top);
			}
			if(col + step >= width) {
				layoutFlag = 1;
			}
			if(col + step <= width) {
				col += step;
			}
		}

		if(layoutFlag) {
			st->layout[row].end = top;
			st->layout[row].width = col;

			col = 0;
			row++;

			if(row < st->layoutSize) {
				st->layout[row].top = top;
				st->layout[row].color = color;
			}
			else {
				loopFlag = 0;
			}
		}
	}
}

//////////////////////////////////////
// 文字列を追加
void msgWnd_addStr(SMsgWnd *st, const unsigned char *str)
{
	int strLen, bufLen;

	bufLen = fontStr_length(st->msgBuf + st->msgBufTop);
	strLen = fontStr_length(str);

	if(bufLen + strLen + 2 > st->msgBufSize) {
		PRTFL();
		PRT0("バッファがあふれた\n");
		strLen = st->msgBufSize - bufLen - 2;
	}

	memmove(st->msgBuf, st->msgBuf + st->msgBufTop, bufLen);
	st->msgBufNow -= st->msgBufTop;
	st->msgBufTop = 0;

	memmove(st->msgBuf + bufLen, (void *)str, strLen);
	st->msgBuf[bufLen + strLen + 0] = FONT_CTRL;
	st->msgBuf[bufLen + strLen + 1] = FONT_CTRL_EOS;

	st->msgIsEnd = 0;

	msgWnd_layout(st);
}

//////////////////////////////////////
// 文字列を一行進める
static void msgWnd_shiftUp(SMsgWnd *st)
{
#if 1
	st->msgBufTop = st->layout[1].top;
	st->topColor = st->layout[1].color;
	msgWnd_layout(st);
#else
/*
	int loop = 1;
	int top = st->msgBufTop;
	int col = 0;
	int space = st->colStep * 2 - st->fntW;
	int width = msgWnd_getWidth(st);
	int color = st->topColor;
	float fnt2scr = (float)st->fntW / (float)FONT_WORD_W;

	while(loop) {
		if(st->msgBuf[top] == FONT_CTRL) {
			switch(st->msgBuf[top + 1]) {
			case FONT_CTRL_RETURN:
				loop = 0;
				break;
			case FONT_CTRL_INKEY:
				loop = (col + st->colStep <= width);
				break;
			case FONT_CTRL_EOS:
				loop = 0;
				break;
			case FONT_CTRL_TIMER:
				break;
			case FONT_CTRL_CLR:
				loop = 0;
				break;
			default:
				color = st->msgBuf[top + 1] - '0';
				break;
			}
			top += fontStr_nextChar(st->msgBuf + top);
		}
		else {
			col += fontStr_charSize(st->msgBuf + top, st->fontType) * fnt2scr + space;
			if(col <= width) {
				top += fontStr_nextChar(st->msgBuf + top);
			}
			if(col >= width) {
				loop = 0;
			}
		}
	}

	st->msgBufTop = top;
	st->topColor = color;
*/
#endif
}

//////////////////////////////////////
// 更新
void msgWnd_update(SMsgWnd *st)
{
	int space = st->colStep * 2 - st->fntW;
	int width = msgWnd_getWidth(st);
	float fnt2scr = (float)st->fntW / (float)FONT_WORD_W;

	// 早送り
	if(!st->keyWait) {
		if(_checkKeyTrg(st->contFlags, CONT_OK | CONT_CANCEL)) {
			msgWnd_skip(st);
		}
	}

	// スクロールを更新
	if(st->scrCount > 0.0) {
		st->scrCount -= st->scrSpeed;
		if(st->scrCount > 0.0) {
			return;
		}
		st->rowNow--;
		msgWnd_shiftUp(st);
	}
	if(st->rowNow >= st->rowSize) {
		st->scrCount += 1.0;
		return;
	}
	st->scrCount = 0.0;

	// キー入力待ち
	if(st->keyWait) {
		st->countWait = 0;
		if(_checkKeyTrg(st->contFlags, CONT_OK | CONT_CANCEL)) {
			st->keyWait = 0;
		}
		return;
	}
	// タイマー
	if ( st->countWait > 0 ) {
		st->countWait--;
		return;
	}

	// メッセージを更新
	st->msgCount += st->msgSpeed;
	while(st->msgCount >= 1.0) {
		if(st->msgBuf[st->msgBufNow] == FONT_CTRL) {
			switch(st->msgBuf[st->msgBufNow + 1]) {
			case FONT_CTRL_RETURN:
				st->colNow = 0;
				st->rowNow++;
				break;
			case FONT_CTRL_TIMER:
				st->countWait = (int)( st->msgBuf[st->msgBufNow + 2] - '0' ) * 60;
				st->msgCount = 0.0;
				break;
			case FONT_CTRL_CLR:
				st->msgCount = 0.0;
				st->colNow = 0;
				st->rowNow = 0;
				st->msgBufTop = st->msgBufNow + fontStr_nextChar(st->msgBuf + st->msgBufNow);
				msgWnd_layout(st);
				break;
			case FONT_CTRL_INKEY:
				st->keyWait = 1;
				st->msgCount = 0.0;
				if(st->colNow + st->colStep > width) {
					st->colNow = 0;
					st->rowNow++;
				}
				break;
			case FONT_CTRL_EOS:
				st->msgIsEnd = 1;
				st->msgCount = 0.0;
				break;
			}
			st->msgBufNow += fontStr_nextChar(st->msgBuf + st->msgBufNow);
			st->isSpeak = 0;
		}
		else {
			st->colNow += fontStr_charSize(st->msgBuf + st->msgBufNow, st->fontType) * fnt2scr + space;
			if(st->colNow <= width) {
				st->msgBufNow += fontStr_nextChar(st->msgBuf + st->msgBufNow);
				st->msgCount -= 1.0;
			}
			if(st->colNow >= width) {
				st->colNow = 0;
				st->rowNow++;
			}
			st->isSpeak = 1;
		}
	}
}

//////////////////////////////////////
// 描画
void msgWnd_draw(SMsgWnd *st, Gfx **gpp)
{
	Gfx *gp = *gpp;
	int now, col, row, rowSize;
	int space = st->colStep * 2 - st->fntW;
	int width = msgWnd_getWidth(st);
	int color = st->topColor;
	int ascFntW = FONT_BYTE_W * st->fntW / FONT_WORD_W;
	float fnt2scr = (float)st->fntW / (float)FONT_WORD_W;
	float scrY;

#if 0 && defined(DEBUG)
	FillRectRGBA(&gp, st->posX, st->posY,
		st->colSize * st->colStep, st->rowSize * st->rowStep,
		0,0,0,64);
#endif

	// 縁ありフォント
	if ( st->fontType ) {
		font16_initDL2(&gp);
	}
	else {
		font16_initDL(&gp);
	}

	// シザー
	if(st->scisFlag) {
		gfxSetScissor(&gp, 2, st->posX, st->posY,
			msgWnd_getWidth(st), msgWnd_getHeight(st));
	}

	now = st->msgBufTop;
	col = 0;
	row = 0;
	rowSize = st->rowSize;
	scrY = 0;

	if(st->scrCount > 0.0) {
		scrY = (1.0 - st->scrCount) * st->rowStep;
		rowSize++;
	}

	gDPSetPrimColor(gp++, 0,0,
		_msgColorTbl[color][0], _msgColorTbl[color][1],
		_msgColorTbl[color][2], st->alpha);

	if(st->fntW != FONT_WORD_W || st->fntH != FONT_WORD_H || scrY != 0) {
		gDPSetTextureFilter(gp++, G_TF_BILERP);
	}

	while(now < st->msgBufNow) {
		if(st->msgBuf[now] == FONT_CTRL) {
			switch(st->msgBuf[now + 1]) {
			case FONT_CTRL_RETURN:
				col = 0;
				row++;
				break;
			case FONT_CTRL_INKEY:
				if(col + st->colStep > width) {
					col = 0;
					row++;
				}
				break;
			case FONT_CTRL_TIMER:
				break;
			case FONT_CTRL_CLR:
				col = 0;
				row = 0;
				break;
			default:
				color = st->msgBuf[now + 1] - '0';
				gDPSetPrimColor(gp++, 0,0,
					_msgColorTbl[color][0], _msgColorTbl[color][1],
					_msgColorTbl[color][2], st->alpha);
				break;
			}
			now += fontStr_nextChar(st->msgBuf + now);
		} else {
			int nextChar = fontStr_nextChar(st->msgBuf + now);
			int charSize = fontStr_charSize(st->msgBuf + now, st->fontType) * fnt2scr + space;
			int orgX = st->centering ? (width - st->layout[row].width) >> 1 : 0;

			switch ( nextChar ) {
			case 1:
				// 半角
				if(col + charSize <= width) {
					switch(st->fontType) {
					case 0:
						fontAsc_draw(&gp,
							(int)(st->posX + col + orgX),
							(int)(st->posY + row * st->rowStep - scrY),
							ascFntW, st->fntH, &st->msgBuf[now]);
						break;
					case 1:
						fontAsc_draw2(&gp,
							(int)(st->posX + col + orgX),
							(int)(st->posY + row * st->rowStep - scrY),
							ascFntW, st->fntH, &st->msgBuf[now]);
						break;
					}
					now += nextChar;
				}
				col += charSize;
				break;
			case 2:
				// 全角
				if(col + charSize <= width) {
					switch(st->fontType) {
					case 0:
						fontXX_draw(&gp,
							(int)(st->posX + col + orgX),
							(int)(st->posY + row * st->rowStep - scrY),
							st->fntW, st->fntH, &st->msgBuf[now]);
						break;
					case 1:
						fontXX_draw2(&gp,
							(int)(st->posX + col + orgX),
							(int)(st->posY + row * st->rowStep - scrY),
							st->fntW, st->fntH, &st->msgBuf[now]);
						break;
					}
					now += nextChar;
				}
				col += charSize + WORD_MARGIN;
				break;
			}
			if(col >= width) {
				col = 0;
				row++;
			}
		}

		if(row >= rowSize) {
			break;
		}
	}

	// キー入力待ちカーソル
	if(st->keyWait && st->scrCount == 0.0) {
		int blink = sins(st->grapCount<<10)*(64.f/32768.f)+191.f;

		blink = (blink * st->alpha) >> 8;
		gDPSetPrimColor(gp++, 0,0,
			_msgColorTbl[color][0], _msgColorTbl[color][1],
			_msgColorTbl[color][2], blink);

		switch(st->fontType) {
		case 0:
			fontXX_draw(&gp,
				(int)(st->posX + col * st->colStep),
				(int)(st->posY + row * st->rowStep - scrY),
				st->fntW, st->fntH, "▼");
			break;
		case 1:
			fontXX_draw2(&gp,
				(int)(st->posX + col * st->colStep),
				(int)(st->posY + row * st->rowStep - scrY),
				st->fntW, st->fntH, "▼");
			break;
		}
	}

	gDPSetScissor(gp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);

	st->grapCount++;

	*gpp = gp;
}

//////////////////////////////////////
// メッセージが終了しているか確認
int msgWnd_isEnd(SMsgWnd *st)
{
	return st->msgIsEnd;
}

//////////////////////////////////////
// メッセージをスキップ
void msgWnd_skip(SMsgWnd *st)
{
	st->msgCount = st->msgBufSize;
	st->countWait = 0;
}

//////////////////////////////////////
// メッセージ進行中?
int msgWnd_isSpeaking(SMsgWnd *st)
{
//	return !st->msgIsEnd && !st->keyWait && st->countWait == 0 && st->scrCount == 0.0;
	return st->isSpeak;
}

//////////////////////////////////////
// スクロール中か確認
int msgWnd_isScroll(SMsgWnd *st)
{
	return st->rowNow >= st->rowSize;
}

//////////////////////////////////////
// 幅を取得
int msgWnd_getWidth(SMsgWnd *st)
{
	return st->colSize * st->colStep;
}

//////////////////////////////////////
// 高さを取得
int msgWnd_getHeight(SMsgWnd *st)
{
	return st->rowSize * st->rowStep;
}
