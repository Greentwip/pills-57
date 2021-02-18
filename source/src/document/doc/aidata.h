/***********************************************************
	ai data header file		Tadashi Ogura
***********************************************************/

enum {
	AIDT_DEAD,			// 死んだ後練習モード用
	AIDT_NARROW,		// 移動可能範囲が狭くなってきた時
	AIDT_LASTVS,		// 終盤でｳｨﾙｽがある時
	AIDT_LASTVSNH,		// 終盤でｳｲﾙｽがない時で高く積んだｶﾌﾟｾﾙを消す時
	AIDT_LASTVSN,		// 終盤でｳｲﾙｽがない時
	AIDT_BADLINE2,		// 消せる高積みﾗｲﾝが中央２ﾗｲﾝにある時
	AIDT_BADLINE1,		// 消せない高積みﾗｲﾝが中央６ﾗｲﾝにある時
	AIDT_NORMAL,		//
};

typedef struct {
	s16		dt_LnOnLinVrs;
	s16		dt_LnEraseVrs;
	s16		dt_EraseLinP1;
	s16		dt_HeiEraseLinRate;
	s16		dt_WidEraseLinRate;
	s16		dt_P_ALN_FallCap;
	s16		dt_P_ALN_FallVrs;
	s16		dt_P_ALN_Capsule;
	s16		dt_P_ALN_Virus;
	s16		dt_WP_ALN_Bottom;
	s16		dt_WP_ALN_FallCap;
	s16		dt_WP_ALN_FallVrs;
	s16		dt_WP_ALN_Capsule;
	s16		dt_WP_ALN_Virus;
	s16		dt_P_ALN_HeightP;
	s16		dt_OnVirusP;
	s16		dt_RensaP;
	s16		dt_RensaMP;
	s16		dt_HeiLinesAllp2;
	s16		dt_HeiLinesAllp3;
	s16		dt_HeiLinesAllp4;
	s16		dt_HeiLinesAllp5;
	s16		dt_HeiLinesAllp6;
	s16		dt_HeiLinesAllp7;
	s16		dt_HeiLinesAllp8;
	s16		dt_WidLinesAllp2;
	s16		dt_WidLinesAllp3;
	s16		dt_WidLinesAllp4;
	s16		dt_WidLinesAllp5;
	s16		dt_WidLinesAllp6;
	s16		dt_WidLinesAllp7;
	s16		dt_WidLinesAllp8;

	s16		dt_LnHighCaps;
	s16		dt_LnHighVrs;

	s16		dt_aiWall_F;
} AI_DATA;
