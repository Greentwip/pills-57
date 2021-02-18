#include <ultra64.h>
#include "replay.h"

#define	REC_LEFT	0x01
#define	REC_RIGHT	0x02
#define	REC_DOWN	0x04
#define	REC_L_BTN	0x08
#define	REC_A_BTN	0x10
#define	REC_B_BTN	0x20
#define	REC_NEXT	0x40
#define	REC_END		0x80

#define	RECBUFF_SIZE	65536
static	u8	*rec_buff;
static	u8	*pRecBuff[4];
static	u16	oldCont[4];
static	int	RecPos[4];
static	int	PlayPos[4];
static	int	WaitTime[4];
static	int	max_rec;
static	int	replay_player;


//----------------------------------------------------------------------------
// リプレイ記録開始時のバッファーを初期化
//		heap	ヒープ領域を示す変数へのポインタ
//----------------------------------------------------------------------------
void	replay_record_init_buffer(void **heap)
{
	rec_buff = *heap;
	*heap = rec_buff + RECBUFF_SIZE;
}

//----------------------------------------------------------------------------
// リプレイ記録開始時の初期化
//		player	参加人数
//----------------------------------------------------------------------------
void	replay_record_init(int player)
{
	int i;

	replay_player = player;

	for ( i = 0; i < 4; i++ ) {
		RecPos[i] = 0;
		WaitTime[i] = 0;
		PlayPos[i] = 0;
		oldCont[i] = 0;
	}

	switch ( player ) {
		case 1:
			max_rec = RECBUFF_SIZE;
			pRecBuff[0] = &rec_buff[0];
			break;
		case 2:
			max_rec = RECBUFF_SIZE / 2;
			pRecBuff[0] = &rec_buff[0 * max_rec];
			pRecBuff[1] = &rec_buff[1 * max_rec];
			break;
		case 4:
			max_rec = RECBUFF_SIZE / 4;
			pRecBuff[0] = &rec_buff[0 * max_rec];
			pRecBuff[1] = &rec_buff[1 * max_rec];
			pRecBuff[2] = &rec_buff[2 * max_rec];
			pRecBuff[3] = &rec_buff[3 * max_rec];
			break;
		default:
			max_rec = RECBUFF_SIZE / 4;
			pRecBuff[0] = &rec_buff[0 * max_rec];
			pRecBuff[1] = &rec_buff[1 * max_rec];
			pRecBuff[2] = &rec_buff[2 * max_rec];
			pRecBuff[3] = &rec_buff[3 * max_rec];
			break;
	}
	max_rec -= 2;
}



//----------------------------------------------------------------------------
// リプレイデータの記録
//		player	プレイヤー番号
//		pad		パッドの状態
//----------------------------------------------------------------------------
int	replay_record(int player, u16 pad)
{
	u8	data, *pRec, flg;

	pRec = pRecBuff[player];

/*
	pRec[RecPos[player]] = (u8)( ( pad >> 8 ) & 0xff );
	RecPos[player]++;
	pRec[RecPos[player]] = (u8)( pad & 0xff );
	RecPos[player]++;
*/

	// 余分なデータを排除
	pad &= ( CONT_LEFT | CONT_RIGHT | CONT_DOWN | CONT_A | CONT_B | CONT_L | CONT_R );

	// パッドデータ解析
	flg = FALSE;
	if ( RecPos[player] == 0 ) oldCont[player] = pad ^ 0xffff;
	if ( oldCont[player] != pad ) {
		data = 0;
		if ( pad & CONT_LEFT )  data |= REC_LEFT;
		if ( pad & CONT_RIGHT ) data |= REC_RIGHT;
		if ( pad & CONT_DOWN )  data |= REC_DOWN;
		if ( pad & CONT_A )     data |= REC_A_BTN;
		if ( pad & CONT_B )     data |= REC_B_BTN;
		if ( pad & (CONT_L|CONT_R) ) data |= REC_L_BTN;
		oldCont[player] = pad;
		flg = TRUE;
	}

	// 記録
	if ( RecPos[player] < max_rec ) {
		if ( flg ) {
			// 押したか離した時
			pRec[RecPos[player]] = WaitTime[player];
			RecPos[player]++;
			pRec[RecPos[player]] = data;
			RecPos[player]++;
			WaitTime[player] = 0;
		} else {
			// ウエイトをカウント
			if ( WaitTime[player] == 0xff ) {
				// カウンターが一杯になった
				pRec[RecPos[player]] = WaitTime[player];
				RecPos[player]++;
				pRec[RecPos[player]] = REC_NEXT;
				RecPos[player]++;
				WaitTime[player] = 0;
			} else {
				WaitTime[player]++;
			}
		}
	} else {
		// バッファオーバー
		return(FALSE);
	}

	pRec[RecPos[player]+0] = WaitTime[player];
	pRec[RecPos[player]+1] = REC_END;

	return(TRUE);
}



//----------------------------------------------------------------------------
// リプレイ再生開始時の初期化
//----------------------------------------------------------------------------
void	replay_play_init(void)
{
	int	i;
	u8	*pRec;
	for ( i = 0; i < replay_player; i++ ) {
		pRec = pRecBuff[i];
		WaitTime[i] = pRec[0];
		PlayPos[i] = 1;
		oldCont[i] = 0;
	}
}



//----------------------------------------------------------------------------
// リプレイデータの取り出し
//		player	プレイヤー番号
// データが終わった場合 CONT_START を返す
//----------------------------------------------------------------------------
u16	replay_play(int player)
{
	u8	data, *pRec, flg;
	u16	cont;

	pRec = pRecBuff[player];

/*
	cont = (u16)pRec[PlayPos[player]];
	PlayPos[player]++;
	cont <<= 8;
	cont |= (u16)pRec[PlayPos[player]];
	PlayPos[player]++;
	oldCont[player] = cont;
*/

	// 終わり
	if ( WaitTime[player] == 0 ) {
		if ( pRec[PlayPos[player]] == REC_END ) return(CONT_START);
		//if ( pRec[PlayPos[player]] == REC_END ) return(oldCont[player]);
		//if ( PlayPos[player] >= max_rec ) return( CONT_START );
		//if ( PlayPos[player] >= RecPos[player] ) return( CONT_START );
	}

	// データ取得
	flg = FALSE;
	if ( WaitTime[player] == 0 ) {
		data = pRec[PlayPos[player]];
		PlayPos[player]++;
		WaitTime[player] = pRec[PlayPos[player]];
		PlayPos[player]++;
		if ( data != REC_NEXT ) flg = TRUE;
	} else {
		WaitTime[player]--;
	}

	// パッドデータ作成
	cont = 0;
	if ( flg ) {
		if ( data & REC_LEFT )  cont |= CONT_LEFT;
		if ( data & REC_RIGHT ) cont |= CONT_RIGHT;
		if ( data & REC_DOWN )  cont |= CONT_DOWN;
		if ( data & REC_L_BTN ) cont |= ( CONT_L | CONT_R );
		if ( data & REC_A_BTN ) cont |= CONT_A;
		if ( data & REC_B_BTN ) cont |= CONT_B;
		oldCont[player] = cont;
	}

	return( oldCont[player] );
}
