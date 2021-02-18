//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_virus_init.c
//	create		:	1999/08/03
//	modify		:	1999/12/09
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#include <ultra64.h>
#include "nnsched.h"

#include "debug.h"
#include "util.h"
#include "calc.h"
#include "evsworks.h"
#include "dm_game_def.h"
#include "dm_virus_init.h"

// ﾚﾍﾞﾙによるvirus積み込み上限ﾃｰﾌﾞﾙ, max = LEVLIMIT
static const u8 limit_table[] = {
	6,6,6,6,6, 6,6,6,6,6, 6,6,6,6,6, 5,5,4,4,3, 3,3,3,3,3
};

//////////////////////////////////////////////////////////////////////////////
//## マップ情報を初期化
void init_map_all(game_map *map)
{
	int i, j, p;

	bzero(map, sizeof(game_map) * GAME_MAP_W * (GAME_MAP_H + 1));

	for(i = 0; i < GAME_MAP_H + 1; i++) {
		for(j = 0; j < GAME_MAP_W; j++) {
			p = i * GAME_MAP_W + j;
			map[p].pos_m_x = j;     // Ｘ座表
			map[p].pos_m_y = i + 1; // Ｙ座標
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//## マップ情報一部クリア関数
void clear_map(game_map *map, int x_pos, int y_pos)
{
	int i, j;

	// 消去する場所の計算
	i = ((y_pos - 1) * GAME_MAP_W) + x_pos;

	// 各フラグの消去
	map[i].capsel_m_g = map[i].capsel_m_p = 0;

	for(j = 0; j < ARRAY_SIZE(map[i].capsel_m_flg); j++) {
		map[i].capsel_m_flg[j] = 0;
	}
}

//////////////////////////////////////////////////////////////////////////////
//## マップ情報クリア関数
void clear_map_all( game_map *map )
{
	int i, j;

	for(i = 0; i < GAME_MAP_H + 1; i++) {
		for(j = 0; j < GAME_MAP_W; j++) {
			clear_map(map, j, i);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//## マップ情報取得関数
int get_map_info(game_map *map, int x_pos, int y_pos)
{
	int i;

	// 調査する場所の計算
	i = ((y_pos - 1) * GAME_MAP_W) + x_pos;

	if(map[i].capsel_m_flg[cap_disp_flg]) {
		// 表示フラグが立っていた
		return cap_flg_on;
	}
	else {
		// 表示フラグが立っていない
		return cap_flg_off;
	}
}

//////////////////////////////////////////////////////////////////////////////
//## 指定したＸ座標軸上で、一番上にある ウィルス or カプセルの色を取得
int get_map_top_color(game_map *map, int x)
{
	int y;

	map += x;

	for(y = 0; y < GAME_MAP_H; y++) {
		if(map->capsel_m_flg[cap_disp_flg]) {
			break;
		}
		map += GAME_MAP_W;
	}

	return (y < GAME_MAP_H) ? map->capsel_m_flg[cap_col_flg] : -1;
}
void get_map_top_colors(game_map *map, int colors[GAME_MAP_W])
{
	int x;

	for(x = 0; x < GAME_MAP_W; x++) {
		colors[x] = get_map_top_color(map, x);
	}
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルス情報取得関数
int get_virus_color_count(game_map *map, u8 *red, u8 *yellow, u8 *blue)
{
	int i, ret;
	int color[3] = { 0, 0, 0 };

	for(i = ret = 0; i < GAME_MAP_W * GAME_MAP_H; i++)
	{
		if( !map[i].capsel_m_flg[cap_disp_flg] ) {
			// 非表示
			continue;
		}

		if( map[i].capsel_m_flg[cap_condition_flg] ){
			// 消滅状態
			continue;
		}

		if(map[i].capsel_m_flg[cap_virus_flg] >= 0) {
			// ウイルスの場合
			color[map[i].capsel_m_flg[cap_col_flg]]++;
			ret++;
		}
	}

	// それぞれの色のウイルス数の設定
	*red    = color[0]; // 赤
	*yellow = color[1]; // 黄
	*blue   = color[2]; // 青

	return ret;
}

int get_virus_count(game_map *map)
{
	u8 ryb[3];

	return get_virus_color_count(map, &ryb[0], &ryb[1], &ryb[2]);
}

//////////////////////////////////////////////////////////////////////////////
//## マップ情報設定関数
void set_map(game_map *map, int x_pos, int y_pos, int item, int pal)
{
	int i, pal_save;

	pal_save = pal;
	if( pal_save > 2 ){
		// 暗い色だった場合
		pal_save -= 3;
	}

	// 設定する場所の計算
	i = ((y_pos - 1) * GAME_MAP_W) + x_pos;

	map[i].capsel_m_g = item;                    // 設定物
	map[i].capsel_m_p = pal;                     // 表示色
	map[i].capsel_m_flg[cap_disp_flg] = 1;       // 表示フラグを立てる
	map[i].capsel_m_flg[cap_down_flg] = 0;       // 落下フラグを消す
	map[i].capsel_m_flg[cap_condition_flg] = 0;  // 状態を通常に
	map[i].capsel_m_flg[cap_virus_flg] = -1;     // ウイルス以外
	map[i].capsel_m_flg[cap_col_flg] = pal_save; // 内部計算色
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報設定関数
void set_virus(game_map *map, int x_pos, int y_pos, int virus, int virus_anime_no)
{
	int i, pal_save;

	pal_save = virus;
	if( pal_save > 2 ){
		// 暗い色だった場合
		pal_save -= 3;
	}

	// 設定する場所の計算
	i = ((y_pos - 1) * GAME_MAP_W) + x_pos;

	map[i].capsel_m_g = virus_anime_no;          // ウイルスアニメ番号
	map[i].capsel_m_p = virus;                   // 色
	map[i].capsel_m_flg[cap_disp_flg] = 1;       // 表示フラグを立てる
	map[i].capsel_m_flg[cap_down_flg] = 0;       // 落下フラグを消す
	map[i].capsel_m_flg[cap_condition_flg] = 0;  // 状態を通常に
	map[i].capsel_m_flg[cap_virus_flg] = virus;  // ウイルス番号をセットする
	map[i].capsel_m_flg[cap_col_flg] = pal_save; // 内部計算色
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成用補助関数
u16 y_point_set(void)
{
	u16 rnd_weight;
	u16 rnd_cnt;
	u16 idx;

	idx = dm_up_weight + dm_middle_weight + dm_low_weight;
	rnd_weight = random( idx );

	if ( rnd_weight < dm_up_weight ) {
			rnd_cnt = random(MFieldY / 3);
	} else {
		if ( rnd_weight < ( dm_up_weight + dm_middle_weight )) {
			rnd_cnt = ( random(MFieldY/3)+ MFieldY/3 );
		} else {
			rnd_cnt = ( random((MFieldY/3+MFieldY%3))+MFieldY/3*2 );
		}
	}

	return ( rnd_cnt );
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成用補助関数(ウイルス存在確認)
u8	dm_virus_check( virus_map *v_map,u16 x_pos,u16 y_pos )
{
	u8	i;

	//	情報取得先の計算( X,Y 座標から配列番号を割り出す )
	i = ((y_pos - 1) << 3) + x_pos;

	if( (v_map + i) -> virus_type < 0 ){
		return	FALSE;	//	無し
	}
	return	TRUE;	// 有り
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成用補助関数(上下左右、同じ色３つ以上並び判定)
u8	dm_check_color( virus_map *v_map,u16 x_pos,u16 y_pos,u8 check_color )
{
	u8	p,p_y[2];

	p = ((y_pos - 1) << 3) + x_pos;

	//	横左チェック
	if( x_pos > 1 ){
		if( (v_map + (p - 1)) -> virus_type == check_color ){		//	１つ左隣が同じ色だった
			if( (v_map + (p - 2)) -> virus_type == check_color ){	//	２つ左隣が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	//	左右チェック
	if( x_pos != 0 && x_pos != 7){
		if( (v_map + (p - 1)) -> virus_type == check_color ){		//	１つ左隣が同じ色だった
			if( (v_map + (p + 1)) -> virus_type == check_color ){	//	１つ右隣が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	//	横右チェック
	if( x_pos < 6 ){
		if( (v_map + (p + 1)) -> virus_type == check_color ){		//	１つ右隣が同じ色だった
			if( (v_map + (p + 2)) -> virus_type == check_color ){	//	２つ右隣が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	p_y[0] = (y_pos  << 3) + x_pos;
	p_y[1] = ((y_pos + 1) << 3) + x_pos;

	if( y_pos < 15 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	１つ下が同じ色だった
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	２つ下が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	p_y[1] = ((y_pos - 2) << 3) + x_pos;

	if( y_pos > 3 && y_pos < 16 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	１つ下が同じ色だった
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	１つ上が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	p_y[0] = ((y_pos - 2) << 3) + x_pos;
	p_y[1] = ((y_pos - 3) << 3) + x_pos;

	if( y_pos > 5 )
	{
		if( (v_map + p_y[0]) -> virus_type == check_color ){		//	１つ上が同じ色だった
			if( (v_map + p_y[1]) -> virus_type == check_color ){	//	２つ上が同じ色だった
				return	FALSE;	//	置けません
			}
		}
	}

	return	TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成用補助関数(上下左右、同じ色３つ以上並び判定)
u8	dm_check_color_2( virus_map *v_map,u16 x_pos,u16 y_pos,u8 check_color )
{
	u8	p;

	p = ((y_pos - 1) << 3) + x_pos;

	//	左チェック
	if( x_pos > 1 ){
		if( (v_map + (p - 2)) -> virus_type == check_color ){	//	２つ隣が同じ色だった
			return	FALSE;	//	置けません
		}
	}

	//	右チェック
	if( x_pos < 6 ){
		if( (v_map + (p + 2)) -> virus_type == check_color ){	//	２つ隣が同じ色だった
			return	FALSE;	//	置けません
		}
	}

	p = ((y_pos - 3) << 3) + x_pos;

	if( y_pos > 5 ){
		if( (v_map + p) -> virus_type == check_color ){	//	２つ上が同じ色だった
			return	FALSE;	//	置けません
		}
	}

	p = ((y_pos + 1) << 3) + x_pos;

	if( y_pos < 15 ){
		if( (v_map + p) -> virus_type == check_color ){	//	２つ下が同じ色だった
			return	FALSE;	//	置けません
		}
	}

	return	TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報複製(コピー)関数
void	dm_virus_map_copy( virus_map *v_map_a, virus_map *v_map_b, u8 *order_a, u8*order_b )
{
	int i;

	for(i = 0; i < 128; i++)
	{
		// コピー先データの初期化
		(v_map_b + i) -> virus_type = -1;
		(v_map_b + i) -> x_pos = (v_map_b + i) -> y_pos = 0;

		// コピー作業
		(v_map_b + i) -> virus_type = (v_map_a + i) -> virus_type;
		(v_map_b + i) -> x_pos = (v_map_a + i) -> x_pos;
		(v_map_b + i) -> y_pos = (v_map_a + i) -> y_pos;
	}

	// 描画順のコピー
	for(i = 0; i < 96; i++) {
		*(order_b + i) = *(order_a + i);
	}
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成関数
int dm_get_first_virus_count(GAME_MODE mode, game_state *state)
{
	int count;

	switch(mode) {
	case GMD_FLASH:
	case GMD_TIME_ATTACK:
		count = dm_get_first_virus_count_in_new_mode(state->game_level);
		break;

	default:
		count = MIN(LEVLIMIT, state->virus_level);
		count = (count + 1) << 2;
		break;
	}

	return count;
}
int dm_get_first_virus_count_in_new_mode(int level)
{
	static const u8 _n[] = { 30, 40, 50 };
	return _n[level];
}

//////////////////////////////////////////////////////////////////////////////
//## ウイルスマップ情報作成関数
void _dm_virus_init(GAME_MODE mode, game_state *state, virus_map *v_map, u8 *order, int special)
{
	s16	i,j;
	u16	virus_color[4];
	u16	cont_count;
	u16	dead_count;
	u16	x_set,y_set;
	s8	col_p;
	u8	color_set;
	u8	virus_work,virus_max,virus_count;
	u8	limit_line;
	u8	set_flg;

RESTART_POS:

	// ウイルスマップ情報の初期化
	for(i = 0; i < 128; i++) {
		(v_map + i) -> virus_type = -1;
		(v_map + i) -> x_pos = (v_map + i) -> y_pos = 0;
	}

	// 表示順情報のクリア
	for(i = 0; i < 96; i++) {
		*(order + i) = 0xff;
	}

	// 初期ウイルス数
	switch(mode) {
	case GMD_FLASH:
	case GMD_TIME_ATTACK: {
		static const u8 _l[] = { 6, 9, 12 };
		limit_line = limit_table[ MIN(LEVLIMIT, _l[state->game_level]) ];
		} break;
	default:
		limit_line = limit_table[ MIN(LEVLIMIT, state->virus_level) ];
		break;
	}
	virus_max = dm_get_first_virus_count(mode, state);

	// ﾚﾍﾞﾙによるvirus不在行数算出

	// 色別ウイルス数の計算
	virus_work = virus_max / 3;
	for(i = 0; i < 4; i++) {
		virus_color[i] = virus_work;
	}

	for(i = 0, j = virus_max % 3; i < j;) {
		int c = random(3);

		if(virus_color[c] <= virus_color[(c + 1) % 3] &&
			virus_color[c] <= virus_color[(c + 2) % 3])
		{
			virus_color[c]++;
			i++;
		}
	}

	virus_count = 0;

	// 操作説明用
	switch(special) {
	case 1:
		x_set = 3;
		y_set = limit_line + 1;
		color_set = j = 2;

		virus_color[j]--;
		j = ((y_set - 1)<< 3) + x_set;
		(v_map + j) -> virus_type = color_set;
		(v_map + j) -> x_pos = x_set;
		(v_map + j) -> y_pos = y_set;
		*(order + virus_count) = j;

		virus_count++;
		break;
	}

	for(; virus_count < virus_max; virus_count++) {

		cont_count = dead_count = 0;
		do {
			//	色指定
			do {
				j = random(3);
			} while(virus_color[j] == 0);
			color_set = j;

			if(cont_count++ == 2) {
				dead_count++;
				if(dead_count > 2){
					//	無限ループ回避
					goto RESTART_POS;
				}

				//	座標指定( 総当たり版 )
				cont_count = 0;
				for(x_set = 0; x_set < 8; x_set++) {
					for(y_set = 16; y_set > limit_line; y_set--) {
						set_flg = FALSE;

						if(dm_virus_check(v_map,x_set,y_set)) {
							continue;
						}

						set_flg = dm_check_color(v_map,x_set,y_set,color_set);
						if(!set_flg) {
							continue;
						}

						set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
						if(!set_flg) {
							continue;
						}

						goto OUT_POS;
					}
				}
OUT_POS:
			}
			else {
				//	座標指定
				do {
					x_set = random(8);
					do {
						y_set = random(17);
//						y_set = y_point_set();
					} while(y_set < limit_line + 1);
				} while(dm_virus_check(v_map,x_set,y_set));

				//	その色が置けるか
				set_flg = dm_check_color(v_map,x_set,y_set,color_set);
				if(set_flg) {
					set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
				}
			}
		} while(!set_flg);
		//	ウイルス設定
#if 0
/*
		if( color_set > 2 ){
			for(i = 0;i < 3;i++){
				if( virus_color[i] != 0 ){
					for( x_set = 0;x_set < 8;x_set ++ ){
						for(y_set = 16;y_set > limit_line;y_set --){
							set_flg = FALSE;
							if( !dm_virus_check(v_map,x_set,y_set) ){
								set_flg = dm_check_color(v_map,x_set,y_set,i);
								if( set_flg == TRUE){
									set_flg = dm_check_color_2(v_map,x_set,y_set,i);
									if( set_flg ){
										color_set = j = i;
										goto SET_POS;
									}
								}
							}
						}
					}
				}
			}
		}
SET_POS:
*/
#endif
		virus_color[j]--;
		j = ((y_set - 1)<< 3) + x_set;
		(v_map + j) -> virus_type = color_set;
		(v_map + j) -> x_pos = x_set;
		(v_map + j) -> y_pos = y_set;
		*(order + virus_count) = j;
	}

}

void dm_virus_init(GAME_MODE mode, game_state *state, virus_map *v_map, u8 *order)
{
	_dm_virus_init(mode, state, v_map, order, 0);
}

//////////////////////////////////////////////////////////////////////////////
//## フラッシュウィルスの位置を決める

typedef struct {
	virus_map *vmap;
	u8 *order;
	int virusCnt;

	u8 selected[96];

	u8 decideBuf[3][32];
	int decideCnt[3];

	u8 underBuf[3][32];
	int underCnt[3];

	u8 under2Buf[3][32];
	int under2Cnt[3];

	u8 under3Buf[3][32];
	int under3Cnt[3];

	u8 centerBuf[3][32];
	int centerCnt[3];

	u8 sideBuf[3][32];
	int sideCnt[3];
} _MakeFlash;

// 初期化
static void _makeFlash_init(_MakeFlash *st, virus_map *vmap, u8 *order, int virusCnt)
{
	bzero(st, sizeof(_MakeFlash));
	st->vmap = vmap;
	st->order = order;
	st->virusCnt = virusCnt;
}

// 各配置候補を検出
static void _makeFlash_checkOrdre(_MakeFlash *st)
{
	int i, c, o, y, under = -1, under2;

	// 最下層の座標を調べる
	for(i = 0; i < st->virusCnt; i++) {
		o = st->order[i];

		switch(st->vmap[o].x_pos) {
		case 0:
		case GAME_MAP_W - 1:
			break;

		default:
			// 最下層を検出
			under = MAX(under, st->vmap[o].y_pos);
			break;
		}
	}

	under = MIN(under, GAME_MAP_H - 2);
	under2 = under - 3;

	// 各配置候補をバッファーに保存
	for(i = 0; i < st->virusCnt; i++) {
		o = st->order[i];
		c = st->vmap[o].virus_type;

		switch(st->vmap[o].x_pos) {
		case 0:
		case GAME_MAP_W - 1:
			// 両端
			st->sideBuf[c][ st->sideCnt[c]++ ] = o;
			break;

		default:
			y = st->vmap[o].y_pos;

			// 最下層
			if(y >= under) {
				st->underBuf[c][ st->underCnt[c]++ ] = o;
			}
			// 二番目の下層
			else if(y >= under2) {
				st->under2Buf[c][ st->under2Cnt[c]++ ] = o;
			}
			// ３番目の下層
			else {
				st->under3Buf[c][ st->under3Cnt[c]++ ] = o;
			}

			// 中央
			st->centerBuf[c][ st->centerCnt[c]++ ] = o;
			break;
		}
	}
}

// ウィルスを決定
static int _makeFlash_decide(_MakeFlash *st, u8 (*buf)[32], int *cnt, int color)
{
	u8 *order = buf[color];
	int i, count = cnt[color];

	if(count == 0) {
		return -1;
	}

	for(i = 0; i < count; i++) {
		if(!st->selected[ order[i] ]) {
			st->selected[ order[i] ] = 1;
			st->decideBuf[color][ st->decideCnt[color]++ ] = order[i];
			return order[i];
		}
	}

	return -1;
}

//_MakeFlash watchMakeFlash[1];

// フラッシュウィルスの位置を決める
int make_flash_virus_pos(game_state *state, virus_map *vmap, u8 *order)
{
	_MakeFlash watchMakeFlash[1];
	_MakeFlash *st = watchMakeFlash;
	int i, j, k, pos, color, count;
	u8 (*buf)[32];
	int *cnt;

	count = state->flash_virus_count = 3;
	_makeFlash_init(st, vmap, order, dm_get_first_virus_count(GMD_FLASH, state));
	_makeFlash_checkOrdre(st);

	color = random(3);

	// ウィルスを決定
	for(i = 0; i < 5; i++) {
		static const int _n[][5] = {
			{ 1, 0, 2, 96, 96 },
			{ 1, 1, 1, 96, 96 },
			{ 1, 2, 0, 96, 96 },
//			{ 1, 0, 0, 96, 96 },
//			{ 1, 1, 0, 96, 96 },
//			{ 1, 1, 0, 96, 96 },
		};
		int k = MIN(count, _n[state->game_level][i]);

		switch(i) {
		case 0:
			buf = st->underBuf;
			cnt = st->underCnt;
			break;

		case 1:
			buf = st->under2Buf;
			cnt = st->under2Cnt;
			break;

		case 2:
			buf = st->under3Buf;
			cnt = st->under3Cnt;
			break;

		case 3:
			buf = st->centerBuf;
			cnt = st->centerCnt;
			break;

		case 4:
			buf = st->sideBuf;
			cnt = st->sideCnt;
			break;
		}

		for(j = 0; j < 3 && k > 0;) {
			color = (color + 1) % 3;

			if(st->decideCnt[color] > st->decideCnt[(color + 1) % 3] ||
				st->decideCnt[color] > st->decideCnt[(color + 2) % 3])
			{
				pos = -1;
			}
			else {
				pos = _makeFlash_decide(st, buf, cnt, color);
			}

			if(pos >= 0) {
				count--;
				k--;
				j = 0;
			}
			else {
				j++;
			}
		}
	}

	// ゲームステータスに反映
	for(color = i = 0; color < 3; color++) {
		for(j = 0; j < st->decideCnt[color]; j++) {
			pos = st->decideBuf[color][j];
			state->flash_virus_pos[i][0] =  pos & 7;
			state->flash_virus_pos[i][1] = (pos >> 3) & 15;
			state->flash_virus_pos[i][2] = color;
			state->flash_virus_bak[i] = color;
			i++;
		}
	}

	return st->decideCnt[0] + st->decideCnt[1] + st->decideCnt[2];
}

//////////////////////////////////////////////////////////////////////////////
