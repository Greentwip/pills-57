//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_manual_data.c
//	create		:	1999/06/28
//	modify		:	1999/12/13
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//{### 基本ルール

//## ステージクリアまでのウィルステーブル
static const u8 virus_1_1[][3] = {
	{ capsel_yellow, 1, 16 },
	{ capsel_blue,   2, 13 },
	{ capsel_blue,   2, 14 },
	{ capsel_red,    5, 15 },
};

//## ゲームオーバーまでのウィルステーブル
static const u8 virus_1_2[][3] = {
	{ capsel_red,    1,  6 },
	{ capsel_red,    5,  6 },

	{ capsel_blue,   0,  6 },
	{ capsel_blue,   4,  6 },
	{ capsel_yellow, 2,  6 },

	{ capsel_yellow, 3,  6 },
	{ capsel_yellow, 6,  6 },
	{ capsel_yellow, 7,  6 },
};

//## ステージクリアまでのマガジン
static const u8 capsel_1_1[] = {
	dm_manual_yr,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_br,
};

//## ゲームオーバーまでのマガジン
static const u8 capsel_1_2[] = {
	dm_manual_br,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_br,
};

//## ゲームオーバーまでのカプセル落下位置
static const u8 position_1_1[][4] = {
	{ 3, 5, 1, 0 },
	{ 3, 3, 0, 0 },
	{ 3, 2, 0, 0 },
	{ 3, 1, 0, 0 },
};

//////////////////////////////////////////////////////////////////////////////
//{### 2P対戦のルール

static const u8 virus_2_1[][3] = {
	{ capsel_red,    0, 14 },
	{ capsel_red,    2, 10 },
	{ capsel_red,    2, 11 },
	{ capsel_red,    5,  8 },
	{ capsel_red,    5,  9 },
	{ capsel_red,    6, 12 },
	{ capsel_red,    6, 14 },
	{ capsel_blue,   3, 14 },
	{ capsel_blue,   4,  7 },
	{ capsel_blue,   4,  8 },
	{ capsel_blue,   5, 11 },
	{ capsel_blue,   5, 12 },
	{ capsel_blue,   5, 14 },
	{ capsel_blue,   5, 15 },
	{ capsel_yellow, 1,  7 },
	{ capsel_yellow, 1, 10 },
	{ capsel_yellow, 1, 11 },
	{ capsel_yellow, 1, 16 },
	{ capsel_yellow, 2, 16 },
	{ capsel_yellow, 7, 15 },
};

static const u8 capsel_2_1[] = {
	dm_manual_br,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_yr,
	dm_manual_yy,
	dm_manual_yy,
};

static const u8 position_2_1[][4] = {
	{ 4,  6, 0, 0 },
	{ 4,  5, 0, 1 },
	{ 7, 14, 1, 0 },
	{ 6, 11, 0, 1 },
	{ 5, 13, 0, 0 },
	{ 0, 13, 1, 0 },
	{ 0,  9, 0, 0 },
	{ 3, 13, 1, 0 },
	{ 2,  9, 0, 1 },
	{ 1,  8, 0, 0 },
};

//////////////////////////////////////////////////////////////////////////////
//{### 4P対戦のルール

static const u8 virus_3_1[][3] = {
	{ capsel_red,     0, 12 },
	{ capsel_red,     1, 13 },
	{ capsel_red,     1, 15 },
	{ capsel_red,     4, 15 },
	{ capsel_red,     6, 13 },
	{ capsel_red,     6,  8 },
	{ capsel_red,     7, 16 },
	{ capsel_blue,    3,  8 },
	{ capsel_blue,    3,  9 },
	{ capsel_blue,    3, 11 },
	{ capsel_blue,    3, 12 },
	{ capsel_blue,    7, 14 },
	{ capsel_blue,    7, 15 },
	{ capsel_yellow,  1, 16 },
	{ capsel_yellow,  2,  8 },
	{ capsel_yellow,  2, 12 },
	{ capsel_yellow,  4, 13 },
	{ capsel_yellow,  4, 14 },
	{ capsel_yellow,  4, 16 },
	{ capsel_yellow,  5, 16 },
};

static const u8 capsel_3_1[] = {
	dm_manual_yr,
	dm_manual_by,
	dm_manual_yr,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_rb,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_yr,
	dm_manual_ry,
	dm_manual_rr,
};

static const u8 position_3_1[][4] = {
	{ 4, 12, 0, 0 },
	{ 3, 10, 0, 0 },
	{ 1, 11, 0, 1 },
	{ 1, 10, 1, 0 },
	{ 2, 10, 1, 0 },
	{ 6, 12, 0, 0 },
	{ 6, 11, 0, 1 },
	{ 6, 10, 1, 0 },
	{ 2, 16, 1, 1 },
	{ 3, 16, 1, 0 },
};

//////////////////////////////////////////////////////////////////////////////
//{### 上達への道

static const u8 virus_4_1[][3] = {
	{ capsel_red,     0, 13 },
	{ capsel_red,     2,  7 },
	{ capsel_red,     5, 10 },
	{ capsel_red,     5, 11 },
	{ capsel_red,     7, 16 },
	{ capsel_blue,    2, 14 },
	{ capsel_blue,    2, 15 },
	{ capsel_blue,    3, 10 },
	{ capsel_blue,    3, 13 },
	{ capsel_blue,    5,  8 },
	{ capsel_yellow,  1, 13 },
	{ capsel_yellow,  4,  8 },
	{ capsel_yellow,  4,  9 },
	{ capsel_yellow,  5, 14 },
	{ capsel_yellow,  5, 15 },
	{ capsel_yellow,  6, 10 },
};

static const u8 capsel_4_1[] = {
	dm_manual_yy,
	dm_manual_yb,
	dm_manual_bb,
	dm_manual_ry,
	dm_manual_yr,
	dm_manual_bb,
	dm_manual_ry,
	dm_manual_yy,
	dm_manual_rr,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_br,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_by,
	dm_manual_rr,
	dm_manual_yy,
};

static const u8 position_4_1[][4] = {
	{ 4,  7, 1, 0 },
	{ 5,  7, 0, 1 },
	{ 5,  6, 1, 0 },
	{ 5,  8, 0, 0 },
	{ 5,  7, 0, 1 },
	{ 3, 12, 1, 0 },
	{ 0, 12, 0, 0 },
	{ 1, 11, 1, 0 },
	{ 0, 11, 1, 0 },
	{ 2, 13, 1, 0 },
	{ 7, 15, 1, 0 },
	{ 6, 13, 0, 0 },
	{ 2,  6, 0, 0 },
	{ 3,  5, 1, 0 },
	{ 3,  3, 0, 0 },
	{ 2,  5, 1, 0 },
};

//////////////////////////////////////////////////////////////////////////////
