/*--------------------------------------
	filename	:	dm_manual_main.c
	create		:	1999/09/04
	modify		:	1999/12/14

	created		:	Hiroyuki Watanabe
--------------------------------------*/
#define F3DEX_GBI
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include <assert.h>

#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "graphic.h"
#include "audio.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"

#include	"dm_nab_include.h"
#include	"o_segment.h"

static	s8	dm_manual_debug_bg_no;
static	s16	dm_manual_debug_timer;

static	game_a_cap	dm_manual_at_cap[4][4];			//	操作説明用攻撃カプセル

static	game_anime	dm_manual_big_virus_anime[3];	//	巨大ウイルスアニメ制御用変数
static	s16	dm_manual_big_virus_pos[3][3];		//	巨大ウイルスの座標
static	s8	dm_manual_big_virus_count[4];		//	巨大ウイルスの座標
static	s8	dm_manual_big_virus_flg[3][2];		//	0:消滅アニメーションを一回だけ設定するフラグ 1:消滅音を一回だけ設定するフラグ

static	s16	dm_manual_timer;				//	内部制御用タイマー
static	s16	dm_manual_timer_count;			//	内部制御用タイマーのカウンター
static	s16	dm_manual_mode[3];				//	内部制御変数
static	s16	dm_manual_waku_alpha[4];		//	操作説明用枠点滅用透明度
static	s8	dm_manual_mode_stop_flg;		//	内部処理進行を制御するフラグ
static	s8	dm_manual_move_vec;				//	内部処理用 回転・移動方向用フラグ
static	s8	dm_manual_move_count;			//	内部処理用 回転・移動 回数用フラグ
static	s8	dm_manual_last3_flg;			//	ウイルスが残り３個のときに鳴らすためのフラグ
static	s8	dm_manual_at_cap_count[4];		//	操作説明用攻撃カプセル落下カウンタ
static	s8	dm_manual_flash_count;			//	操作説明用点滅用カウンタ
static	s8	dm_manual_flash_color;			//	操作説明用点滅用カウンタ
static	s8	dm_manual_waku_flash_count[4];	//	操作説明用点滅用カウンタ
static	s8	dm_manual_waku_flash_flg[4];	//	操作説明用点滅用フラグ

static	s8	dm_manual_1_ef_flg;				//	操作説明１用汎用性フラグ
static	s8	dm_manual_3_ef_flg;				//	操作説明３用汎用性フラグ
static	s8	dm_manual_6_ef_flg;				//	操作説明６用汎用性フラグ

static	u8	manual_1_CapsMagazine_a[] = {	//	説明１ステージクリアまでのマガジン
	dm_manual_yy,
	dm_manual_ry,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_rr,

};

static	u8	manual_1_CapsMagazine_b[] = {	//	説明１ゲームオーバーまでのマガジン
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_by,
	dm_manual_rb,
	dm_manual_ry
};

static	u8	manual_2_CapsMagazine[] = {	//	説明２のマガジン
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
	dm_manual_br
};

static	u8	manual_3_CapsMagazine[] = {	//	説明３のマガジン
	dm_manual_ry,
	dm_manual_by,
	dm_manual_yr,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_rb,
	dm_manual_rb,
	dm_manual_rr,
	dm_manual_ry,
	dm_manual_yr,
	dm_manual_rr,
	dm_manual_by,

};


static	u8	manual_4_CapsMagazine[] = {	//	説明４のマガジン
	dm_manual_yy,
	dm_manual_yy,
	dm_manual_bb,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_yr,
	dm_manual_ry,
	dm_manual_br,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_br,
	dm_manual_yb,
	dm_manual_bb,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_rr,
	dm_manual_yy,
	dm_manual_yr,
	dm_manual_bb,
	dm_manual_rr,
	dm_manual_rb,
	dm_manual_yy
};

static	u8	manual_5_CapsMagazine[] = {	//	説明５のマガジン
	dm_manual_rr,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_rb,
	dm_manual_yr,
	dm_manual_yr,
	dm_manual_rb,
	dm_manual_rb,
	dm_manual_ry,
	dm_manual_bb,
	dm_manual_rb,
	dm_manual_yy,
	dm_manual_rb,
	dm_manual_yb,
	dm_manual_rr,
	dm_manual_rr,
	dm_manual_by,
	dm_manual_by,
	dm_manual_yy,
	dm_manual_yy
};

static	u8	manual_6_CapsMagazine[] = {	//	説明６のマガジン
	dm_manual_yb,
	dm_manual_by,
	dm_manual_yr,
	dm_manual_yy,
	dm_manual_rr,
	dm_manual_bb,
	dm_manual_br,
	dm_manual_yb,
	dm_manual_yy,
	dm_manual_by,
	dm_manual_yb,
	dm_manual_br,


	//	2Pマガジン
	dm_manual_yy,
	dm_manual_yy,
	dm_manual_br,

};


/*--------------------------------------
	操作説明初期化関数
--------------------------------------*/
void	dm_manual_all_init( void )
{
	s16	i,j,k;
	s16	map_x_table[][4] = {
		{dm_wold_x,dm_wold_x,dm_wold_x,dm_wold_x},							// 1P
		{dm_wold_x_vs_1p,dm_wold_x_vs_2p,dm_wold_x_vs_1p,dm_wold_x_vs_2p},	// 2P
		{dm_wold_x_4p_1p,dm_wold_x_4p_2p,dm_wold_x_4p_3p,dm_wold_x_4p_4p}	// 4P
	};
	u8	map_y_table[] = {dm_wold_y,dm_wold_y_4p};
	u8	size_table[] = {cap_size_10,cap_size_8};

	init_effect_all();		//	エフェクト初期化
	dm_game_init();			//	ゲーム用変数初期化

	dm_manual_last3_flg = cap_flg_off;			//	リーチSEフラグをＯＦＦにする
	manual_flash_alpha = cap_flg_off;			//	点滅のフラグＯＦＦにする
	manual_key_flash_count = cap_flg_off;		//	キー点滅時間ＯＦＦにする

	for(i = j = 0;i < 3;i++ ){
		dm_manual_big_virus_flg[i][0] = 0;
		dm_manual_big_virus_flg[i][1] = 0;
		//	巨大ウイルスの座標を計算する
		dm_manual_big_virus_pos[i][2] = j;	//	sin,cos用数値
		dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
		dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
		j += 120;
	}

	switch( evs_manual_no )
	{
	case	0:	//	その１(１人用)
	case	3:	//	その３(１人用)
		k = 0;	//	アイテムの大きさ
		j = 0;	//	基準Ｘ座標
		break;
	case	1:	//	その２(２人用)
	case	4:	//	その４(２人用)
		k = 0;	//	アイテムの大きさ
		j = 1;	//	基準Ｘ座標
		break;
	case	2:	//	その３(４人用)
	case	5:	//	その３(４人用)
		k = 1;	//	アイテムの大きさ
		j = 2;	//	基準Ｘ座標
		break;
	}


	for( i = 0;i < 4;i ++ ){
		//	座標系の設定
		game_state_data[i].map_x = map_x_table[j][i];		//	基準Ｘ座標
		game_state_data[i].map_y = map_y_table[k];			//	基準Ｙ座標
		game_state_data[i].map_item_size = size_table[k];	//	基準アイテムサイズ
	}

	for(i = 0;i < 4;i++)
	{
		dm_manual_waku_flash_flg[i] = cap_flg_off;

		//	カプセル落下速度の設定

		game_state_data[i].cap_def_speed = SPEED_MID;								//	マニュアルは SPEED_MID に固定
		game_state_data[i].cap_speed = GameSpeed[game_state_data[i].cap_def_speed];	//	落下速度の設定

		switch(evs_manual_no)
		{
		case	2:
		case	5:
			//	４Ｐの場合
			game_state_data[i].virus_anime_spead = v_anime_def_speed_4p;			//	アニメーション間隔の設定
			game_state_data[i].virus_anime_max = 0;									//	ウイルスアニメーション最大コマ数の設定
			break;
		default:
			//	それ以外の場合
			game_state_data[i].virus_anime_spead = v_anime_def_speed;				//	アニメーション間隔の設定
			game_state_data[i].virus_anime_max = 2;									//	ウイルスアニメーション最大コマ数の設定
			break;
		}

		//	攻撃カプセルの設定
		dm_manual_at_cap_count[i] = 0;
		for( j = 0;j < 4;j++ ){
			dm_manual_at_cap[i][j].pos_a_x = dm_manual_at_cap[i][j].pos_a_y = 0;
			dm_manual_at_cap[i][j].capsel_a_p = 0;
			for( k = 0;k < 3;k++ ){
				dm_manual_at_cap[i][j].capsel_a_flg[k] = 0;
			}
		}
	}

	for( i = 0;i < 3;i++ )
	{
		manual_bubble_col_flg[i] = 0;
		dm_manual_mode[i] = 0;
	}

	dm_manual_timer = dm_manual_mode_stop_flg = 0;

	evs_seqence = 2;	//	BGMは無し
}
/*--------------------------------------
	操作説明用文字列操作関数(１行の長さ取得)
--------------------------------------*/
s8		dm_get_manual_str_len(u16 *str)
{
	s8	len = 0;

	while(1){
		len++;
		if(*str == 0xffff || *str == 0xfffd){
			return	len;
		}
		str++;
	}
}

/*--------------------------------------
	操作説明用文字列操作関数(文字列データを表示用バッファへコピー)
--------------------------------------*/
void	dm_set_manual_str(u16 *str)
{
	u8	i = 0;

	manual_str_timer = manual_str_count = manual_str_len = 0;		//	メッセージ表示長さの初期化

	while(1){
		manual_str_buffer[i] = *str;
		if(*str == 0xffff ){
			break;
		}
		str++;
		i++;
	}
}


/*--------------------------------------
	操作説明用文字列操作関数(文字列データをバッファへの最後尾にコピー)
--------------------------------------*/
void	dm_set_manual_str_link(u16 *str)
{
	u8	i = 0;

	manual_str_timer = 0;

	while(1){
		if( manual_str_buffer[i] == 0xffff){
			break;
		}
		i++;
	}

	manual_str_buffer[i] = 0xfffd;	//	改行コード代入
	i ++;

	while(1){
		manual_str_buffer[i] = *str;
		if(*str == 0xffff ){
			break;
		}
		str++;
		i++;
	}
}

/*--------------------------------------
	操作説明用文字列操作関数(５行以上のときの１行改行)
--------------------------------------*/
void	dm_manual_str_next_line(void)
{
	s8	i,j,len;
	u16	*str_start,*str_end;
	u16	save_buffer[100];

	for(i = 0;i < 100;i++){
		save_buffer[i] = manual_str_buffer[i];	//	文字列データのセーブ
		manual_str_buffer[i] = 0xffff;			//	終端文字列データのセット
	}

	len = dm_get_manual_str_len(save_buffer);	//	1行目の長さを求める

	str_start = manual_str_buffer;
	str_end = save_buffer;

	str_end += len;	//	１行分進ませる

	for(i = 0;i < (100 - len);i++){
		*str_start = *str_end;
		str_start++;
		str_end++;
	}

	//	表示文字数の調整
	str_start = manual_str_buffer;
	for(i = manual_str_len = 0;i < 5;i++)
	{
		len = dm_get_manual_str_len(str_start);
		str_start += len;
		manual_str_len += len;
	}
}
/*--------------------------------------
	操作説明用文字列操作関数(文字列描画)
--------------------------------------*/
s8	dm_draw_manual_str(s16 x_pos,s16 y_pos,u16 *str,s8 max)
{
	s16	i,j;
	s16	x_p;
	s8	str_max;

	i = str_max = 0;
	x_p = x_pos;
	for(;;)
	{
		//	文字列終了
		if(*str == 0xffff)
			return	-1;

		if(i >= max)
			return	1;

		//	６行以上は描画しない
		if( str_max > 5)
			break;
		switch(*str)
		{
		case	0xfff0:
		case	0xfff1:
		case	0xfff2:
		case	0xfff3:
		case	0xfff4:
			//	パレット変更
			load_TexPal( dm_manual_font_pal[*str - 0xfff0] );
			break;
		case	0xfffd:
			//	改行
			str_max++;
			x_pos = x_p;
			y_pos += 12;
			break;
		case	0xfffe:
			//	スペース
			x_pos += 12;
			break;
		default:
			//	文字描画
			gDPLoadTextureTile_4b(gp++,dm_manual_font[*str], G_IM_FMT_I,12,12,0,0,11,11,0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
			gSPScisTextureRectangle(gp++,x_pos << 2,y_pos << 2,x_pos + 12 << 2,y_pos + 12 << 2,G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
			x_pos += 12;
			break;
		}
		i++;
		str++;
	}

	if( str_max > 4){
		//	改行作業
		dm_manual_str_next_line();
		return	1;
	}
}
/*--------------------------------------
	操作説明用文字列操作関数(文字列進行)
--------------------------------------*/
s8	dm_manual_str_main(void)
{
	//	文字列進行処理
	if( manual_str_stop_flg ){
		//	文章表示中の場合
		if(manual_str_end_flg > 0){
			//	文章が終わりに達していない
			manual_str_count ++;

			//	キノピオのクチパク
			if((manual_str_count % 4) == 0){
				manual_pino_anime_flg ^= 1;
			}
			if( manual_str_count > dm_manual_str_wait){
				manual_str_count = 0;
				manual_str_len ++;
			}
			return	1;
		}else{
			//	キノピオにクチパクさせない
			manual_pino_anime_flg = 0;

			manual_str_timer ++;
			if(manual_str_timer >= dm_manual_str_erace_wait)
			{
				manual_str_timer = dm_manual_str_erace_wait;
				return	-1;
			}else{
				return	1;
			}
		}
	}else{
		//	キノピオにクチパクさせない
		manual_pino_anime_flg = 0;
		return	-1;
	}
}
/*--------------------------------------
	操作説明攻撃カプセル設定関数
--------------------------------------*/
void	dm_manual_set_attack_capsel( game_a_cap *cap,s8 pos_x,s8 color )
{
	cap -> pos_a_x = pos_x;					//	Ｘ座標の設定
	cap -> pos_a_y = 1;						//	Ｙ座標の設定
	cap -> capsel_a_p = color;				//	色の設定
	cap -> capsel_a_flg[cap_disp_flg] = 1;	//	表示フラグを立てる
}
/*--------------------------------------
	操作説明攻撃カプセル落下処理関数
--------------------------------------*/
void	dm_manual_attack_capsel_down( void )
{
	s8	i,j,flg;

	//	攻撃カプセル落下処理
	flg = 0;
	for( i = 0;i < 4;i++ ){
		dm_manual_at_cap_count[i] ++;
		if( dm_manual_at_cap_count[i] >= dm_down_speed ){
			dm_manual_at_cap_count[i] = 0;

			for(j = 0;j < 4;j++){
				if( dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] ){
					//	表示フラグが立っていた場合

					//	落下カプセルとマップ情報の判定
					if( get_map_info( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,dm_manual_at_cap[i][j].pos_a_y + 1) ){
						//	もしカプセルの下にアイテムがあった場合

						//	マップ情報へ変更する
						set_map( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,
												dm_manual_at_cap[i][j].pos_a_y,capsel_b,dm_manual_at_cap[i][j].capsel_a_p);
						dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;	//	表示フラグの消去
					}else{
						flg = 1;
						if(dm_manual_at_cap[i][j].pos_a_y < 16){
							dm_manual_at_cap[i][j].pos_a_y ++;
						}

						if(dm_manual_at_cap[i][j].pos_a_y == 16){
						//	もし最下段だった場合

							//	マップ情報へ変更する
							set_map( game_map_data[i],dm_manual_at_cap[i][j].pos_a_x,
											dm_manual_at_cap[i][j].pos_a_y,capsel_b,dm_manual_at_cap[i][j].capsel_a_p);
							dm_manual_at_cap[i][j].capsel_a_flg[cap_disp_flg] = 0;	//	表示フラグの消去
						}
					}
				}
			}
		}
	}
	if( flg ){
		dm_set_se(dm_se_cap_a_down);
	}
}
/*--------------------------------------
	移動・回転設定関数
--------------------------------------*/
void	dm_manual_set_move( s16 now,s16 next,s8 count,s8 wait,s8 vec )
{
	dm_manual_timer = 0;					//	内部タイマークリア
	dm_manual_mode[dm_manual_now]	= now;	//	処理番号の設定( 移動・回転・高速落下のどれか )
	dm_manual_mode[dm_manual_next]	= next;	//	次の処理番号の設定
	dm_manual_move_count = count;			//	回転(移動)の回数の設定
	dm_manual_move_vec = vec;				//	回転(移動)方向の設定
	dm_manual_timer_count = wait;			//	回転(移動)の間隔(時間)の設定
}
/*--------------------------------------
	操作説明用演出処理関数
--------------------------------------*/
void	dm_manual_effect_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};

	if( state -> game_condition[dm_mode_now] != dm_cnd_wait && state -> game_condition[dm_mode_now] != dm_cnd_pause ){
		dm_black_up( state,map );		//	黒上がり処理
	}

	if( state -> game_condition[dm_mode_now] == dm_cnd_manual ){	//	通常状態だった
		bubble_main( player_no );	//	攻撃演出処理
		shock_main( player_no, state -> map_x + 10,207);		//	くらい演出設定
		humming_main( player_no, state -> map_x + 20 );			//	ハミング演出設定
	}
}
/*--------------------------------------
	ゲーム処理関数
--------------------------------------*/
s8	dm_manual_main_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j,out;
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};

	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:	//	ウイルス配置
		return	dm_ret_virus_wait;
	case	dm_mode_wait:
		return	dm_ret_virus_wait;
	case	dm_mode_down:	//	カプセル落下
		dm_capsel_down( state,map );
		break;
	case	dm_mode_down_wait:	//	着地ウェイト
		if( dm_check_game_over( state,map ) ){
			return	dm_ret_game_over;			//	積みあがり(ゲームオーバー)
		}else{
			if( dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off ){
				if( !state -> retire_flg[dm_game_over_flg] ){	//	リタイアしていない
					state -> game_mode[dm_mode_now] = dm_mode_erase_chack;	//	消滅判定
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	リタイアしている(練習中)
					state -> game_mode[dm_mode_now] = dm_mode_tr_erase_chack;	//	消滅判定
				}
				state -> cap_speed_count = 0;
			}else{
				if( !state -> retire_flg[dm_game_over_flg] ){	//	リタイアしていない
					state -> game_mode[dm_mode_now] = dm_mode_cap_set;	//	カプセルセット
				}else	if( state -> retire_flg[dm_game_over_flg] ){	//	リタイアしている(練習中)
					state -> game_mode[dm_mode_now] = dm_mode_tr_cap_set;	//	カプセルセット
				}
			}
		}
		break;
	case	dm_mode_erase_chack:	//	消滅判定
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){
			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;	//	消滅開始

			dm_h_erase_chack_set( state,map );	//	縦消し判定
			dm_w_erase_chack_set( state,map );	//	横消し判定
			dm_h_ball_chack( map );						//	縦粒化現象
			dm_w_ball_chack( map );						//	横粒化現象

			state -> virus_number = get_virus_color_count( map,&dm_manual_big_virus_count[0], &dm_manual_big_virus_count[1] ,&dm_manual_big_virus_count[2] );
			switch(dm_manual_debug_bg_no)
			{
			case	0:
			case	3:
				for( i = 0;i < 3;i++ )
				{
					if( dm_manual_big_virus_count[i] == 0 )
					{
						if( !dm_manual_big_virus_flg[i][0] ){
							dm_manual_big_virus_flg[i][0] = 1;
							dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_lose );	//	巨大ウイルス消滅アニメーションセット
							if( state -> virus_number != 0 ){
								dm_set_se( dm_se_big_virus_damage );			//	巨大ウイルスダメージ
							}
						}
					}else{
						if( state -> chain_color[3] & (0x10 << i) ){
							dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_damage );	//	巨大ウイルスやられアニメーションセット
							dm_set_se( dm_se_big_virus_damage );						//	巨大ウイルスダメージ

						}
					}
				}
			}
			state -> chain_color[3] &= 0x0f;	//	巨大ウイルスアニメーションリセット


			if( state -> virus_number == 0 ){
				//	ウイルス消滅
				dm_score_make( state,1 );					//	得点計算
				state -> game_condition[dm_mode_now] = dm_cnd_stage_clear;	//	ステージクリア
				state -> game_mode[dm_mode_now] = dm_mode_stage_clear;
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	ウイルスが残り３個以下のとき警告音を鳴らす
				if( !dm_manual_last3_flg ){
					//	１回ならしたら以後鳴らさない
					dm_manual_last3_flg = 1;
					dm_set_se( dm_se_last_3 );			//	ウイルス残り３個ＳＥ再生
				}
			}

			state ->chain_count ++;
			if( state -> chain_color[3] & 0x08 ){
				state -> chain_color[3] &= 0xF7;		//	ウイルスを含む消滅のリセット
				dm_set_se( dm_se_virus_erace );			//	ウイルスを含む消滅ＳＥ再生
			}else{
				dm_set_se( dm_se_cap_erace );			//	カプセルのみの消滅ＳＥ再生
			}
		}
		break;
	case	dm_mode_erase_anime:	//	消滅アニメーション
		dm_capsel_erase_anime( state,map );
		break;
	case	dm_mode_ball_down:
		go_down( state,map,dm_down_speed );	//	カプセル(粒)落下
		break;
	case	dm_mode_cap_set:	//	新カプセルセット
		out = 1;
		dm_attack_se( state,player_no );	//	攻撃ＳＥ再生
		dm_warning_h_line( state,map );		//	積みあがりチェック

		switch( evs_gamesel ){
		case	GSL_2PLAY:	//	2Ｐの時
		case	GSL_VSCPU:	//	VSCPUのとき
			set_bubble( state ,player_no,0,1,0,0,0 );			//	演出設定
			break;
		case	GSL_4PLAY:
			set_bubble( state ,player_no,1,0,manual_bubble_col_flg[0],manual_bubble_col_flg[1],manual_bubble_col_flg[2] );	//	演出設定
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	落下カプセル速度の計算
			state -> chain_line = 0;			//	消滅ライン数のリセット
			state -> chain_count = 0;			//	連鎖数のリセット
			state -> erase_virus_count = 0;		//	消滅ウイルス数のリセット
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;	//	消滅色のリセット
			}

			state -> game_mode[dm_mode_now] = dm_mode_down;	//	カプセル落下処理へ
			//	落下前思考追加位置
			for(i = 0;i < 3;i++){
				manual_bubble_col_flg[i] = 0;
			}
		}
		break;
	case	dm_mode_stage_clear:	//	ステージクリア演出
		i = try_next_stage_main();
		if( i > 0){
			return	dm_ret_next_stage;
		}
		break;
	case	dm_mode_game_over:		//	ゲームオーバー演出
		i = game_over_main();
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_win:	//	WIN演出
		i = win_main( center[flg][player_no] , player_no );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_lose:	//	LOSE演出
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_draw:	//	DRAW演出
		i = draw_main( player_no,center[flg][player_no] );
		if( i > 0){
			return	dm_ret_end;
		}
		break;
	case	dm_mode_tr_chaeck:	//	練習確認
		break;
	case	dm_mode_tr_erase_chack:
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){
			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;

			dm_h_erase_chack_set( state,map );	//	縦消し判定
			dm_w_erase_chack_set( state,map );	//	横消し判定
			dm_h_ball_chack( map );						//	縦粒化現象
			dm_w_ball_chack( map );						//	横粒化現象

			state ->chain_count ++;
			if( state -> chain_color[3] & 0x08 ){
				state -> chain_color[3] &= 0xF7;		//	ウイルスフラグ
			}else{
				dm_set_se( dm_se_cap_erace );			//	カプセルのみの消滅ＳＥ再生
			}
		}
		break;
	case	dm_mode_tr_cap_set:
		out = 1;
		dm_attack_se( state,player_no );			//	攻撃ＳＥ再生
		dm_warning_h_line( state,map );				//	積みあがりチェック
		if( game_state_data[i].retire_flg[dm_training_flg] ){	//	攻撃あり
			if( dm_broken_set( state , map )){		//	やられ判定
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	粒落下処理へ
				out = 0;
			}
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );					//	落下カプセル速度の計算
			state -> chain_line = 0;						//	消滅ライン数のリセット
			state -> chain_count = 0;						//	連鎖数のリセット
			state -> erase_virus_count = 0;					//	消滅ウイルス数のリセット
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;				//	消滅色のリセット
			}
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	カプセル落下処理へ
		}
		break;
	}
	return	0;
}

/*--------------------------------------
	操作説明その１処理メイン関数
--------------------------------------*/
s8	dm_manual_1_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;

	state = &game_state_data[0];
	map = game_map_data[0];

	//	キー点滅カウンター
	if(manual_key_flash_count > 0)
		manual_key_flash_count--;

	//	落下や着地判定等を行っている
	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,0 );
	}


	//	タイマー処理
	dm_manual_timer ++;

	dm_virus_anime( state,map );	//	ウイルスアニメーション
	for(i = 0;i < 3;i++){
		dm_anime_control( &dm_manual_big_virus_anime[i] );	//	巨大ウイルスアニメーション
	}
	//	巨大ウイルスアニメーション
	for(i = j = 0;i < 3;i++){
		if( dm_manual_big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	負けアニメーションの場合
			if( dm_manual_big_virus_anime[i].cnt_now_type[dm_manual_big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
				j++;
				if( dm_manual_big_virus_anime[i].cnt_now_frame == 20 ){
					if( !dm_manual_big_virus_flg[i][1] ){			//	巨大ウイルス消滅 SE を再生していない
						dm_manual_big_virus_flg[i][1] = 1;			//	２回鳴らさないためのフラグセット
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_erace );	//	巨大ウイルス消滅 SE セット
						}
					}
				}
			}
		}else	if( dm_manual_big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
			j ++;
		}
	}
	//	回転（円を描く）処理
	if( j == 0 ){
		dm_manual_big_virus_count[3] ++;
		if( dm_manual_big_virus_count[3] >= 10 ){
			dm_manual_big_virus_count[3] = 0;

			for(i = 0;i < 3;i++){
				dm_manual_big_virus_pos[i][2] ++;
				if( dm_manual_big_virus_pos[i][2] >= 360 ){
					dm_manual_big_virus_pos[i][2] = 0;
				}
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
			}
		}
	}



	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 190;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 30;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 210;							//	キノピオの座標

		manual_str_win_flg = cap_flg_off;			//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;			//	文字列進行フラグをOFFにする

		dm_manual_1_ef_flg = 0;							//	汎用性フラグの初期化

		dm_manual_mode[dm_manual_now] = 1;				//	次の処理に

		for( i = 0;i < 6;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_1_CapsMagazine_a[i];
		}
		state -> cap_magazine_cnt = 1;					//	マガジン残照を1にする
		dm_set_capsel( state );							//	カプセル設定

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置
		set_virus( map,manual_main_1_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
						manual_main_1_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
							manual_main_1_virus_table_a[state -> virus_number][0],	//	ウイルスの色
								virus_anime_table[manual_main_1_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号

		state -> virus_number ++;
		if( state -> virus_number > 3 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_1[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_1[1] );				//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;				//	内部制御進行開始
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
			state -> game_mode[dm_mode_now] = dm_mode_down;		//	カプセル落下開始
			dm_manual_mode[dm_manual_now] = 11;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	11:	//	文書２途中まで表示
		if( manual_str_len == 9 ){
			//	"まずはカプセル。"まで描画したらカプセル落下中止
			dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
			dm_manual_timer = 0;							//	内部処理タイマーのクリア
			dm_manual_mode[dm_manual_now] = 12;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	12:	//	カプセル点滅処理
		if( dm_manual_timer > 10 ){
			dm_manual_timer = 0;							//	内部処理タイマーのクリア
			for( i = 0;i < 2;i++ ){
				if( state -> now_cap.capsel_p[i] < 3 ){	//	明るかったら暗くする
					state -> now_cap.capsel_p[i] += 3;
				}else{									//	暗かったら明るくする
					state -> now_cap.capsel_p[i] -= 3;
				}
			}
		}
		if( manual_str_flg < 0 ){
			//	文章表示終了
			for( i = 0;i < 2;i++ ){
				state -> now_cap.capsel_p[i] = capsel_yellow;	//	カプセル点滅中止
			}
			dm_set_manual_str(manual_txt_1[2]);				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 20;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	20:	//	右へ説明
		if( manual_str_len == 4 ){
			//	"右へ、"まで表示したら文章進行停止
			manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをＯＦＦにする
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始

			dm_manual_1_ef_flg = 1;							//	矢印描画開始
			dm_manual_set_move( dm_manual_translate,21,3,manual_main_time_table_1[0],cap_turn_r);	//	移動方向の設定(右３回)
		}
		return	dm_manual_work_on;
	case	21:	//	左へ説明
		dm_manual_1_ef_flg = 0;
		dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
		manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
		if( manual_str_len == 10 ){
			//	"左へ、"まで表示したら文章進行停止
			manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをＯＦＦにする
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_1_ef_flg = 2;							//	矢印描画開始
			dm_manual_set_move( dm_manual_translate,22,6,manual_main_time_table_1[1],cap_turn_l);	//	移動方向の設定(左６回)
		}
		return	dm_manual_work_on;
	case	22:	//	下へ説明
		dm_manual_1_ef_flg = 0;							//	矢印描画開始
		dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
		manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
		if( manual_str_len == 13 ){
			//	"下へ、"まで表示したら高速落下開始
			dm_manual_timer = 0;							//	内部処理タイマーのクリア
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
			dm_manual_1_ef_flg = 3;							//	矢印描画開始
			manual_key_flash_pos = 1;						//	点滅させるキーの設定（下）
			dm_manual_mode[dm_manual_now] = 23;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	23:
		//	落下待ち
		manual_key_flash_count = 4;						//	キー点滅時間の設定
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	キー点滅時間の設定
			dm_manual_1_ef_flg = 0;							//	矢印描画終了
			state -> cap_speed_vec = 1;						//	カプセル落下速度を戻す
			dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
			dm_manual_mode[dm_manual_now] = 24;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	24:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_set_manual_str(manual_txt_1[3]);				//	説明文のデータをバッファにセット
			dm_manual_set_move( dm_manual_wait,30,0,30,cap_turn_r);	//	落下までの待ち時間
		}
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,3,manual_main_time_table_1[2],cap_turn_r);	//	回転方向の設定(右３回)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_rotate,32,3,manual_main_time_table_1[2],cap_turn_l);	//	回転方向の設定(左３回)
		return	dm_manual_work_on;
	case	32:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){			//	カプセルが落下したら
			dm_set_manual_str(manual_txt_1[4]);						//	説明文のデータをバッファにセット
			dm_manual_1_ef_flg = 4;									//	点滅枠( カプセルの周り )の表示開始
			dm_manual_set_move( dm_manual_wait,35,0,20,cap_turn_r);	//	20ｲﾝﾄの待ち時間
		}
		return	dm_manual_work_on;
	case	35:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_1[3],cap_turn_r);	//	回転方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 36;		//	高速落下後の処理設定
		return	dm_manual_work_on;
	case	36:
		if(game_map_data[0][(13 << 3) + 3].capsel_m_flg[cap_condition_flg]){	//	消滅状態待ち
			manual_str_stop_flg = cap_flg_on;									//	文字列進行開始
			dm_manual_mode_stop_flg = cap_flg_off;								//	内部制御進行停止
			dm_manual_mode[dm_manual_now] = 37;									//	次の処理に
		}
		return	dm_manual_work_on;
	case	37:
		if( manual_str_flg < 0 ){
			dm_manual_1_ef_flg = 0;							//	点滅枠( カプセルの周り )の表示終了
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_mode[dm_manual_now] = 40;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	38:
		if( !game_map_data[0][(12 << 3) + 4].capsel_m_flg[cap_disp_flg] ){
			dm_manual_mode[dm_manual_now] = 40;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	40:
		//	消滅待ち
		if( game_map_data[0][(15 << 3) + 4].capsel_m_g == capsel_b ){
			dm_set_manual_str(manual_txt_1[5]);				//	説明文のデータをバッファにセット
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			dm_manual_mode[dm_manual_now] = 41;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	41:
		//	"次は、"まで表示したら高速落下開始 & 文章進行停止
		if( manual_str_len == 4 ){
			dm_manual_set_move( dm_manual_wait,42,0,20,cap_turn_r);	//	20ｲﾝﾄの待ち時間
			manual_str_stop_flg = cap_flg_off;						//	文字列進行停止
			dm_manual_1_ef_flg = 5;									//	点滅枠( カプセルの周り )の表示開始
		}
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move(dm_manual_translate,dm_manual_down,1,manual_main_time_table_1[4],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 43;		//	高速落下後の処理設定
		return	dm_manual_work_on;
	case	43:
		//	消滅待ち(カプセル状態から消滅状態になるまで待つ)
		if( game_map_data[0][(15 << 3) + 5].capsel_m_flg[cap_condition_flg] ){
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
			dm_manual_mode[dm_manual_now] = 44;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	44:
		if( manual_str_flg < 0 ){
			manual_str_stop_flg = cap_flg_off;				//	文字列進行停止
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_mode[dm_manual_now] = 50;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	50:
		if( !game_map_data[0][(15 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			dm_manual_1_ef_flg = 0;							//	点滅枠( カプセルの周り )の表示終了
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
			dm_set_manual_str(manual_txt_1[6]);				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 51;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	51:
		if( manual_str_len == 20 ){
			//	"瓶の中のウイルスを全て消したら"まで表示したら
			manual_str_stop_flg = cap_flg_off;				//	文字列進行停止
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_1[5],cap_turn_r);	//	回転方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,60,2,manual_main_time_table_1[6],cap_turn_l);	//	移動方向の設定(左２回)
		return	dm_manual_work_on;
	case	60:
		if( ret == dm_ret_clear ){
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 61;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	61:
		if( ret == dm_ret_next_stage ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 62;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	62:
		if(dm_manual_timer > manual_main_time_table_1[7] && manual_str_flg < 0 ){
			//	ゲームオーバー用初期化
			for( i = 0;i < 9;i ++ ){						//	落下カプセル作成
				CapsMagazine[i + 1] = manual_1_CapsMagazine_b[i];
			}
			state -> game_mode[dm_mode_now] = dm_mode_init;		//	内部制御を初期化モードに設定
			state -> game_condition[dm_mode_now] = dm_cnd_init;	//	状態を初期化状態に設定

			state -> virus_number = 0;	//	ウイルス数のクリア
			state -> warning_flg = 0;	//	警告音フラグのクリア

			//	カプセル落下速度の設定
			state -> cap_speed = GameSpeed[state -> cap_def_speed];
			state -> cap_speed_vec = 1;								//	落下カウンタ増加値
			state -> cap_magazine_cnt = 1;							//	マガジン残照を1にする
			state -> cap_count = state ->cap_speed_count = 0;		//	カウンタの初期化
			//	カプセル情報の初期設定
			dm_set_capsel( state );

			//	ウイルスアニメーションの設定
			state -> erase_anime = 0;								//	消滅アニメーションコマ数の初期化
			state -> erase_anime_count = 0;							//	消滅アニメーションカウンタの初期化
			state -> erase_virus_count = 0;							//	消滅ウイルスカウンタの初期化

			for(i = j = 0;i < 3;i++ ){
				dm_manual_big_virus_flg[i][0] = 0;
				dm_manual_big_virus_flg[i][1] = 0;
				dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_nomal );
				//	巨大ウイルスの座標を計算する
				dm_manual_big_virus_pos[i][2] = j;	//	sin,cos用数値
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
				j += 120;
			}


			//	連鎖数の初期化
			state -> chain_count = state -> chain_line =  0;

			clear_map_all( map );									//	マップ情報のクリア

			manual_str_win_flg = cap_flg_off;						//	説明表示しないようにする
			manual_str_stop_flg = cap_flg_off;						//	文字列進行停止
			dm_manual_mode_stop_flg = cap_flg_off;					//	内部制御進行開始

			dm_manual_mode[dm_manual_now] = 70;						//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	70:	//	ゲームオーバー説明用ウイルス配置
		set_virus( map,manual_main_1_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
			manual_main_1_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
				manual_main_1_virus_table_b[state -> virus_number][0],	//	ウイルスの色
					virus_anime_table[manual_main_1_virus_table_b[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号

		state -> virus_number ++;
		if( state -> virus_number > 43 ){
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			manual_str_win_flg = cap_flg_on;						//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;						//	文字列進行フラグをＯＮにする
			dm_set_manual_str(manual_txt_1[7]);						//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御進行開始
			dm_manual_mode[dm_manual_now] = 71;						//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	71:
		if( ret == dm_ret_game_over ){
			state -> game_condition[dm_mode_now] = dm_cnd_game_over;	//	ゲームオーバー
			state -> game_mode[dm_mode_now] = dm_mode_game_over;
			state -> virus_anime_spead 		= v_anime_speed;			//	アニメーション速度を早くする

			auSeqPlayerPlay(0,SEQ_Lose);						//	音楽再生
			manual_bgm_flg = 1;									//	再生フラグをＯＮにする
			manual_bgm_timer = 0;								//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_a;			//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 72;					//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	72:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 73;
		}
		return	dm_manual_work_on;
	case	73:
		if(dm_manual_timer > manual_main_time_table_1[8] && manual_str_flg < 0 ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_set_manual_str(manual_txt_1[8]);				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 80;
		}
		return	dm_manual_work_on;
	case	80:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	キー点滅時間の設定
			//	落下しなくなった
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
			manual_key_flash_count = 4;						//	キー点滅時間の設定
			manual_key_flash_pos = 1;						//	点滅させるキーの設定（下）
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	キー点滅時間の設定
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 2;								//	点滅させるキーの設定（右）
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 0;								//	点滅させるキーの設定（左）
			}

			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	キー点滅時間の設定
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 4;								//	点滅させるキーの設定（Ａ）
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 3;								//	点滅させるキーの設定（Ｂ）
			}

			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}

}

/*--------------------------------------
	操作説明その２処理メイン関数
--------------------------------------*/
s8	dm_manual_2_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,1 );
		dm_manual_main_cnt( &game_state_data[1],game_map_data[1],1,1 );
	}
	dm_manual_effect_cnt( state,map,0,1 );	//	連鎖等の特種表示の処理
	dm_manual_attack_capsel_down();			//	攻撃カプセルの落下処理

	//	タイマー処理
	dm_manual_timer ++;

	for(i = 0;i < 2;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	ウイルスアニメーション
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 102;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 30;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 128;							//	キノピオの座標


		manual_str_win_flg = cap_flg_off;			//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;			//	文字列進行フラグをOFFにする

		dm_manual_mode[dm_manual_now] = 1;				//	次の処理に

		for( i = 0;i < 12;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_2_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	マガジン残照を1にする
		dm_set_capsel( state );							//	カプセル設定

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置

		set_virus( map,manual_main_2_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
						manual_main_2_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
							manual_main_2_virus_table_a[state -> virus_number][0],	//	ウイルスの色
								virus_anime_table[manual_main_2_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号

		if( game_state_data[1].virus_number < 4 ){
			set_virus( game_map_data[1],manual_main_2_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_2_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_2_virus_table_b[state -> virus_number][0],	//	ウイルスの色
									virus_anime_table[manual_main_2_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	アニメ番号
			game_state_data[1].virus_number ++;
		}

		state -> virus_number ++;

		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_2[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_2[1] );			//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 11;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	11:	//	文書２表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_2[2] );				//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;				//	内部制御進行開始
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
			state -> game_mode[dm_mode_now] = dm_mode_down;		//	カプセル落下開始
			dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);	//	一段目落下までのウェイト
		}
		return	dm_manual_work_on;
	case	20:	//	連鎖とは
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_2[0],cap_turn_r);	//	移動方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 21;			//	次の処理に
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_wait,22,1,20,cap_turn_r);	//	一段目落下までのウェイト
		return	dm_manual_work_on;
	case	22:
		dm_manual_set_move( dm_manual_rotate,23,2,manual_main_time_table_2[1],cap_turn_r);	//	回転方向の設定(右２回)
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_2[1],cap_turn_r);	//	移動方向の設定(右１回)
		return	dm_manual_work_on;
	case	24:
		//	消滅待ち
		if( !game_map_data[0][(8 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 1;										//	キノピオの向き変更（右向き）

			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],2,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_red );
			dm_manual_at_cap_count[1] = 0;

			dm_manual_set_move( dm_manual_rotate,25,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	25:
		dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_set_manual_str(manual_txt_2[3]);				//	説明文のデータをバッファにセット
			manual_pino_flip_flg = 0;						//	キノピオの向き変更（左向き）
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_set_move( dm_manual_rotate,26,1,manual_main_time_table_2[2],cap_turn_r);	//	回転方向の設定(右１回)
		}
		return	dm_manual_work_on;dm_manual_wait;
	case	26:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_2[3],cap_turn_r);	//	移動方向の設定(右４回)
		dm_manual_mode[dm_manual_next_next] = 27;			//	次の処理に
		return	dm_manual_work_on;
	case	27:
		dm_manual_set_move( dm_manual_rotate,30,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,2,manual_main_time_table_2[4],cap_turn_r);	//	回転方向の設定(右２回)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_2[5],cap_turn_r);	//	移動方向の設定(右３回)
		dm_manual_mode[dm_manual_next_next] = 32;			//	次の処理に
		return	dm_manual_work_on;
	case	32:
		dm_set_manual_str_link(manual_txt_2[4]);		//	説明文のデータをバッファにくっつける
		dm_manual_mode[dm_manual_now] = 33;				//	次の処理に
		return	dm_manual_work_on;
	case	33:
		if( state -> now_cap.pos_y[0] == 13){
			dm_manual_timer = 0;												//	タイマークリア
			dm_manual_set_move( dm_manual_translate,34,2,manual_main_time_table_2[6],cap_turn_r);	//	移動方向の設定(右２回)
		}
		return	dm_manual_work_on;
	case	34:
		//	消滅待ち
		if( !game_map_data[0][(14 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 1;										//	キノピオの向き変更（右向き）

			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],4,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;
			dm_manual_timer = 0;										//	タイマークリア
			dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		// 30ｲﾝﾄ
		}
		return	dm_manual_work_on;
	case	35:
		dm_manual_mode_stop_flg = cap_flg_off;			//	内部制御進行停止
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_set_manual_str_link(manual_txt_2[5]);		//	説明文のデータをバッファにくっつける

			manual_pino_flip_flg = 0;						//	キノピオの向き変更（左向き）
			dm_manual_mode_stop_flg = cap_flg_on;			//	内部制御進行開始
			dm_manual_set_move( dm_manual_rotate,36,1,manual_main_time_table_2[7],cap_turn_r);	//	回転方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_2[8],cap_turn_l);	//	移動方向の設定(左３回)
		dm_manual_mode[dm_manual_next_next] = 37;			//	次の処理に
		return	dm_manual_work_on;
	case	37:
		dm_manual_set_move( dm_manual_wait,38,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	38:
		if( state -> now_cap.pos_y[0] == 6){
			dm_manual_set_move( dm_manual_translate,40,3,manual_main_time_table_2[9],cap_turn_l);	//	移動方向の設定(左３回)
		}
		return	dm_manual_work_on;
	case	40:
		//	操作カプセル落下待ち
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_2[10],cap_turn_r);		//	回転方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 42;			//	次の処理に
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_wait,43,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move(dm_manual_rotate,44,2,manual_main_time_table_2[11],cap_turn_r);					//	回転方向の設定(右２回)
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_2[12],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 45;			//	次の処理に
		return	dm_manual_work_on;
	case	45:
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	46:
		if( state -> now_cap.pos_y[0] == 8){
			dm_manual_set_move( dm_manual_translate,50,2,manual_main_time_table_2[13],cap_turn_l);			//	移動方向の設定(左２回)
		}
		return	dm_manual_work_on;
	case	50:
		//	消滅待ち
		if( !game_map_data[0][(13 << 3)].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;								//	説明表示フラグをＯＦＦにする
			manual_pino_flip_flg = 1;										//	キノピオの向き変更（右向き）

			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],0,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],2,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],5,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][3],7,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			dm_manual_timer = 0;										//	タイマークリア

			dm_manual_mode[dm_manual_now] = 51;				//	次の処理に

		}
		return	dm_manual_work_on;
	case	51:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 0;										//	キノピオの向き変更（左向き）
		}
		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 60;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	60:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 61;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	61:
		if( dm_manual_timer > manual_main_time_table_2[11]){
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			manual_str_win_flg = cap_flg_on;				//	メッセージウインドウの描画ＯＮ
			dm_set_manual_str(manual_txt_2[6]);	//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 70;	//	次の処理へ
		}
		return	dm_manual_work_on;
	case	70:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	落下しなくなった
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	操作説明その３処理メイン関数
--------------------------------------*/
s8	dm_manual_3_main(void)
{
	s16	i,j;
	s8	ret;
	s8	map_table[] = {0,2,3};
	game_state	*state;
	game_map	*map;
	s8	flash_cap_pos_table_blue[] = {59,67,83,91};	//	点滅させるカプセル(青)の位置情報(配列番号)
	s8	flash_cap_pos_table_red[] = {62,86,94,102};	//	点滅させるカプセル(赤)の位置情報(配列番号)
	s8	flash_cap_pos_table_yellow[] = {58,82,90};	//	点滅させるカプセル(黄)の位置情報(配列番号)


	state = &game_state_data[1];
	map = game_map_data[1];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,1,2 );
		for( i = 0;i < 3;i++ ){
			dm_manual_main_cnt( &game_state_data[map_table[i]],game_map_data[map_table[i]],map_table[i],2 );
		}
	}
	dm_manual_effect_cnt( state,map,1,2 );	//	連鎖等の特種表示の処理
	dm_manual_attack_capsel_down();

	//	タイマー処理
	dm_manual_timer ++;

	for(i = 0;i < 4;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	ウイルスアニメーション
	}

	//	点滅処理
	switch( dm_manual_3_ef_flg )
	{
	case	1:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_blue ){
				dm_manual_flash_color = capsel_b_blue;	//	暗い色
			}else{
				dm_manual_flash_color = capsel_blue;
			}
			for( i = 0;i < 4;i++ ){
				( map + flash_cap_pos_table_blue[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	case	2:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_yellow ){
				dm_manual_flash_color = capsel_b_yellow;	//	暗い色
			}else{
				dm_manual_flash_color = capsel_yellow;
			}
			for( i = 0;i < 3;i++ ){
				( map + flash_cap_pos_table_yellow[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	case	3:
		dm_manual_flash_count ++;
		if( dm_manual_flash_count % 10 == 0 ){
			if( dm_manual_flash_color == capsel_red ){
				dm_manual_flash_color = capsel_b_red;	//	暗い色
			}else{
				dm_manual_flash_color = capsel_red;
			}
			for( i = 0;i < 4;i++ ){
				( map + flash_cap_pos_table_red[i] ) -> capsel_m_p = dm_manual_flash_color;
			}
		}
		break;
	default:
		dm_manual_flash_count = 0;
		break;
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 175;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 15;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 240;							//	キノピオの座標


		manual_str_win_flg = cap_flg_off;				//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをOFFにする

		dm_manual_3_ef_flg = 0;							//	点滅処理無し
		dm_manual_flash_count = 0;						//	点滅用カウンタ初期化
		dm_manual_mode[dm_manual_now] = 1;				//	次の処理に

		for( i = 0;i < 12;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_3_CapsMagazine[i];
		}
		for( i = 0;i < 4;i++ ){
			game_state_data[i].cap_magazine_cnt = 1;	//	マガジン残照を1にする
			dm_set_capsel( &game_state_data[i] );		//	カプセル設定
		}

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置

		set_virus( map,manual_main_3_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
						manual_main_3_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
							manual_main_3_virus_table_a[state -> virus_number][0],	//	ウイルスの色
								virus_anime_table[manual_main_3_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号

		for( i = 0;i < 3;i++ ){
			if( state -> virus_number < 4 ){
				set_virus( game_map_data[map_table[i]],
							manual_main_3_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
								manual_main_3_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
									manual_main_3_virus_table_b[state -> virus_number][0],		//	ウイルスの色
										virus_anime_table[manual_main_3_virus_table_b[state -> virus_number][0]][game_state_data[map_table[i]].virus_anime] );	//	アニメ番号
			}
		}

		state -> virus_number ++;

		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_3[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_3[1] );			//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 11;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	11:	//	文書２表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_3[2] );				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 12;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	12:	//	文書３表示
		if( manual_str_flg < 0 ){
			dm_manual_3_ef_flg = 1;									//	点滅開始
			dm_set_manual_str( manual_txt_3[3] );					//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御進行開始
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_translate,21,1,manual_main_time_table_3[0],cap_turn_r);	//	移動方向の設定(右１回)
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_3[1],cap_turn_r);	//	回転方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 22;					//	次の処理に
		return	dm_manual_work_on;
	case	22:
		dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_3[2],cap_turn_r);	//	移動方向の設定(右１回)
		return	dm_manual_work_on;
	case	24:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_translate,25,1,manual_main_time_table_3[3],cap_turn_l);	//	移動方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	25:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;												//	消滅のため点滅中止
			for(i = 0;i < 4;i++){
				( map + flash_cap_pos_table_blue[i] ) -> capsel_m_p = capsel_blue;	//	元の色に戻す
			}
			dm_manual_mode[dm_manual_now] = 26;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	26:
		//	消滅判定
		if( game_map_data[1][(14 << 3) + 5].capsel_m_g == capsel_b ){
			manual_bubble_col_flg[2] = 1;	//	青色バブル打ち上げ
			dm_manual_set_attack_capsel( &dm_manual_at_cap[2][0],0,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[2][1],7,capsel_yellow );
			dm_manual_at_cap_count[2] = 0;

			dm_manual_3_ef_flg = 2;										//	点滅色変更
			manual_str_win_flg = cap_flg_off;							//	説明表示フラグをＯＦＦにする
			dm_set_manual_str(manual_txt_3[4]);							//	説明文のデータをバッファにセット
			dm_manual_set_move( dm_manual_wait,27,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	27:
		dm_manual_set_move( dm_manual_rotate,28,2,manual_main_time_table_3[4],cap_turn_r);	//	回転方向の設定(右２回)
		return	dm_manual_work_on;
	case	28:
		if( dm_manual_at_cap[2][0].pos_a_y >= 8){
			manual_str_win_flg = cap_flg_on;														//	説明表示フラグをＯＮにする
		}
		if( state -> now_cap.pos_y[0] == 9){
			dm_manual_set_move( dm_manual_translate,30,2,manual_main_time_table_3[5],cap_turn_l);	//	移動方向の設定(左２回)
		}
		return	dm_manual_work_on;
	case	30:
		if( manual_str_len == 33){
			manual_str_stop_flg = cap_flg_off;																//	文字列進行停止
			dm_manual_set_move( dm_manual_rotate,31,1,manual_main_time_table_3[6],cap_turn_r);				//	回転方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_3[7],cap_turn_l);	//	移動方向の設定(左２回)
		dm_manual_mode[dm_manual_next_next] = 32;			//	次の処理に
		return	dm_manual_work_on;
	case	32:
		dm_manual_set_move( dm_manual_wait,33,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_rotate,34,1,manual_main_time_table_3[8],cap_turn_r);					//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	34:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			manual_str_stop_flg = cap_flg_on;					//	文字列進行再開
			dm_manual_set_move( dm_manual_translate,35,1,manual_main_time_table_3[9],cap_turn_l);		//	移動方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	35:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;														//	消滅のため点滅中止
			for(i = 0;i < 3;i++){
				( map + flash_cap_pos_table_yellow[i] ) -> capsel_m_p = capsel_yellow;	//	元の色に戻す
			}
			dm_manual_mode[dm_manual_now] = 36;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	36:
		//	消滅待ち
		if( !game_map_data[1][(12 << 3) + 1].capsel_m_flg[cap_disp_flg] ){

			manual_str_win_flg = cap_flg_off;					//	メッセージウインドウの描画ＯＦＦ

			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_blue );
			dm_manual_at_cap_count[3] = 0;
			manual_bubble_col_flg[1] = 1;	//	黄色バブル打ち上げ

			dm_manual_mode[dm_manual_now] = 37;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	37:
		//	キノピオ逃げ
		manual_pino_pos -= 3;
		if( manual_pino_pos <= 160)
		{
			dm_manual_3_ef_flg = 3;				//	消滅開始赤
			manual_pino_flip_flg = 1;			//	キノピオの向き（右向き）
			dm_set_manual_str(manual_txt_3[5]);	//	説明文のデータをバッファにセット
			manual_pino_pos = 160;
			dm_manual_mode[dm_manual_now] = 40;	//	次の処理に
		}
		return	dm_manual_work_on;
	case	40:
		if( dm_manual_at_cap[3][0].pos_a_y >= 8){							//	攻撃カプセルのＹ座標が８以上になったら
			manual_str_win_flg = cap_flg_on;								//	説明表示フラグをＯＮにする
			manual_pino_flip_flg = 0;										//	キノピオの向き（左向き）
		}
		if( state -> now_cap.pos_y[0] == 9){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_3[10],cap_turn_r);	//	移動方向の設定(右３回)
			dm_manual_mode[dm_manual_next_next] = 41;
		}
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_rotate,42,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	42:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_3[11],cap_turn_r);		//	移動方向の設定(右３回)
			dm_manual_mode[dm_manual_next_next] = 43;
		}
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move( dm_manual_wait,44,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_rotate,45,1,manual_main_time_table_3[12],cap_turn_l);	//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	45:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_translate,46,3,manual_main_time_table_3[13],cap_turn_r);		//	移動方向の設定(右３回)
		}
		return	dm_manual_work_on;
	case	46:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_3_ef_flg = 0;													//	消滅のため点滅中止
			for(i = 0;i < 4;i++){
				( map + flash_cap_pos_table_red[i] ) -> capsel_m_p = capsel_red;	//	元の色に戻す
			}
			dm_manual_mode[dm_manual_now] = 50;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	50:
		//	消滅待ち
		if( !game_map_data[1][(14 << 3) + 7].capsel_m_flg[cap_disp_flg] ){

			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][0],2,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][1],4,capsel_yellow );
			dm_manual_at_cap_count[0] = 0;
			manual_bubble_col_flg[0] = 1;	//	赤色バブル打ち上げ

			dm_set_manual_str(manual_txt_3[6]);				//	説明文のデータをバッファにセット

			dm_manual_set_move( dm_manual_wait,51,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	51:
		dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_3[14],cap_turn_r);		//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,53,1,manual_main_time_table_3[15],cap_turn_l);		//	回転方向の設定(左１回)
		return	dm_manual_work_on;
	case	53:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_set_manual_str(manual_txt_3[7]);						//	説明文のデータをバッファにセット
			dm_manual_set_move( dm_manual_wait,54,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	54:
		dm_manual_set_move( dm_manual_rotate,55,1,manual_main_time_table_3[16],cap_turn_l);		//	回転方向の設定(左１回)
		return	dm_manual_work_on;
	case	55:
		//	消滅待ち
		if( !game_map_data[1][(15 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;							//	説明表示フラグをＯＦＦにする

			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[0][1],6,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_yellow );
			dm_manual_at_cap_count[0] = 0;
			dm_manual_at_cap_count[3] = 0;
			manual_bubble_col_flg[0] = 1;	//	赤色バブル打ち上げ
			manual_bubble_col_flg[1] = 1;	//	黄色バブル打ち上げ

			dm_manual_set_move( dm_manual_wait,60,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	60:
		dm_manual_set_move( dm_manual_translate,61,3,manual_main_time_table_3[17],cap_turn_l);		//	移動方向の設定(左３回)
		return	dm_manual_work_on;
	case	61:
		dm_manual_set_move( dm_manual_rotate,62,1,manual_main_time_table_3[18],cap_turn_r);		//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	62:
		//	消滅待ち
		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			for( i = 0;i < 3;i++ ){
				game_state_data[map_table[i]].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
				game_state_data[map_table[i]].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE
			}

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 63;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	63:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 64;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	64:
		if( dm_manual_timer > manual_main_time_table_3[19]){
			manual_str_win_flg = cap_flg_on;				//	メッセージウインドウの描画ＯＮ
			dm_set_manual_str(manual_txt_3[8]);				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 70;				//	次の処理へ
		}
		return	dm_manual_work_on;
	case	70:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	落下しなくなった
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;


			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}

		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;


			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	操作説明その４処理メイン関数
--------------------------------------*/
s8	dm_manual_4_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;

	state = &game_state_data[0];
	map = game_map_data[0];

	//	キー点滅カウンター
	if(manual_key_flash_count > 0)
		manual_key_flash_count--;

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,0 );
	}
	dm_manual_effect_cnt( state,map,0,0 );	//	連鎖等の特種表示の処理

	//	タイマー処理
	dm_manual_timer ++;

	dm_virus_anime( state,map );	//	ウイルスアニメーション
	for(i = 0;i < 3;i++){
		dm_anime_control( &dm_manual_big_virus_anime[i] );	//	巨大ウイルスアニメーション
	}
	//	巨大ウイルスアニメーション
	for(i = j = 0;i < 3;i++){
		if( dm_manual_big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	負けアニメーションの場合
			if( dm_manual_big_virus_anime[i].cnt_now_type[dm_manual_big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
				j++;
				if( dm_manual_big_virus_anime[i].cnt_now_frame == 20 ){
					if( !dm_manual_big_virus_flg[i][1] ){			//	巨大ウイルス消滅 SE を再生していない
						dm_manual_big_virus_flg[i][1] = 1;			//	２回鳴らさないためのフラグセット
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_erace );	//	巨大ウイルス消滅 SE セット
						}
					}
				}
			}
		}else	if( dm_manual_big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
			j ++;
		}
	}
	//	回転（円を描く）処理
	if( j == 0 ){
		dm_manual_big_virus_count[3] ++;
		if( dm_manual_big_virus_count[3] >= 10 ){
			dm_manual_big_virus_count[3] = 0;

			for(i = 0;i < 3;i++){
				dm_manual_big_virus_pos[i][2] ++;
				if( dm_manual_big_virus_pos[i][2] >= 360 ){
					dm_manual_big_virus_pos[i][2] = 0;
				}
				dm_manual_big_virus_pos[i][0] = (( 10 * sinf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * -2 ) + 45;
				dm_manual_big_virus_pos[i][1] = (( 10 * cosf( DEGREE( dm_manual_big_virus_pos[i][2] ) ) ) * 2  ) + 155;
			}
		}
	}


	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 190;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 30;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 210;							//	キノピオの座標

		manual_str_win_flg = cap_flg_off;				//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをOFFにする

		dm_manual_mode[dm_manual_now] = 1;				//	次の処理に

		for( i = 0;i < 27;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_4_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	マガジン残照を1にする
		dm_set_capsel( state );							//	カプセル設定

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置
		set_virus( map,manual_main_4_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
						manual_main_4_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
							manual_main_4_virus_table_a[state -> virus_number][0],	//	ウイルスの色
								virus_anime_table[manual_main_4_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号

		state -> virus_number ++;
		if( state -> virus_number > 19 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_4[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_4[1] );				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 12;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	11:	//	文書２表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_4[2] );				//	説明文のデータをバッファにセット
			dm_manual_timer = 0;								//	タイマーのクリア
			dm_manual_mode[dm_manual_now] = 12;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	12:	//	カプセル点滅処理
		if( dm_manual_timer > 10 ){
			dm_manual_timer = 0;							//	内部処理タイマーのクリア
			for( i = 0;i < 2;i++ ){
				if( state -> now_cap.capsel_p[i] < 3 ){	//	明るかったら暗くする
					state -> now_cap.capsel_p[i] += 3;
				}else{									//	暗かったら明るくする
					state -> now_cap.capsel_p[i] -= 3;
				}
			}
		}
		if( manual_str_flg < 0 ){
			//	文章表示終了
			for( i = 0;i < 2;i++ ){
				state -> now_cap.capsel_p[i] = capsel_yellow;	//	カプセル点滅中止
			}
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御進行開始
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			dm_set_manual_str( manual_txt_4[2] );					//	説明文のデータをバッファにセット
			dm_manual_set_move( dm_manual_wait,13,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	13:
		dm_manual_set_move( dm_manual_translate,14,2,manual_main_time_table_4[0],cap_turn_l);	//	移動方向指定(左２回)
		return	dm_manual_work_on;
	case	14:
		dm_manual_set_move( dm_manual_rotate,15,1,manual_main_time_table_4[1],cap_turn_r);	//	回転方向指定(右１回)
		return	dm_manual_work_on;
	case	15:
		manual_key_flash_count = 4;										//	キー点滅時間の設定
		manual_key_flash_pos = 1;										//	点滅させるキーの設定（下）
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 12 ){
			manual_key_flash_count = 4;										//	キー点滅停止
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,16,2,manual_main_time_table_4[2],cap_turn_r);	//	移動方向指定(右２回)
		}
		return	dm_manual_work_on;
	case	16:
		if( state -> now_cap.pos_y[0] == 16 ){
			dm_manual_set_move( dm_manual_rotate,17,1,manual_main_time_table_4[3],cap_turn_l);	//	回転方向指定(左１回)
		}
		return	dm_manual_work_on;
	case	17:
		//	１個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,20,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_translate,21,3,manual_main_time_table_4[4],cap_turn_r);	//	移動方向指定(右３回)
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[5],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 22;					//	次の処理に
		return	dm_manual_work_on;
	case	22:
		//	２個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	23:
		dm_manual_set_move( dm_manual_translate,24,2,manual_main_time_table_4[6],cap_turn_l);	//	移動方向指定(左２回)
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,25,1,manual_main_time_table_4[7],cap_turn_l);	//	回転方向指定(左１回)
		return	dm_manual_work_on;
	case	25:
		manual_key_flash_count = 4;										//	キー点滅時間の設定
		manual_key_flash_pos = 1;										//	点滅させるキーの設定（下）
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 12 ){
			manual_key_flash_count = 0;									//	キー点滅停止
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[8],cap_turn_l);	//	移動方向指定(左１回)
			dm_manual_mode[dm_manual_next_next] = 30;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	30:
		//	３個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,31,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_4[9],cap_turn_r);	//	移動方向指定(右２回)
		dm_manual_mode[dm_manual_next_next] = 32;					//	次の処理に
		return	dm_manual_work_on;
	case	32:
		//	４個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,33,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[10],cap_turn_r);	//	回転方向指定(右１回)
		dm_manual_mode[dm_manual_next_next] = 34;					//	次の処理に
		return	dm_manual_work_on;
	case	34:
		//	５個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	35:
		manual_str_win_flg = cap_flg_off;				//	説明表示フラグをＯＦＦにする
		dm_manual_set_move( dm_manual_translate,36,2,manual_main_time_table_4[10],cap_turn_l);	//	移動方向指定(左２回)
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[11],cap_turn_r);	//	回転方向指定(右１回)
		dm_manual_mode[dm_manual_next_next] = 40;					//	次の処理に
		return	dm_manual_work_on;
	case	40:
		//	６個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,42,1,manual_main_time_table_4[12],cap_turn_r);	//	移動方向指定(右１回)
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[13],cap_turn_r);	//	回転方向指定(右２回)
		dm_manual_mode[dm_manual_next_next] = 43;					//	次の処理に
		return	dm_manual_work_on;
	case	43:
		//	７個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,44,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[14],cap_turn_r);	//	回転方向指定(右３回)
		dm_manual_mode[dm_manual_next_next] = 45;					//	次の処理に
		return	dm_manual_work_on;
	case	45:
		//	８個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	46:
		dm_manual_set_move( dm_manual_translate,47,2,manual_main_time_table_4[15],cap_turn_l);	//	移動方向指定(左２回)
		return	dm_manual_work_on;
	case	47:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[16],cap_turn_r);	//	回転方向指定(右２回)
		dm_manual_mode[dm_manual_next_next] = 50;					//	次の処理に
		return	dm_manual_work_on;
	case	50:
		//	９個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,51,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	51:
		dm_manual_set_move( dm_manual_rotate,52,1,manual_main_time_table_4[17],cap_turn_l);	//	回転方向指定(左１回)
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_4[18],cap_turn_r);	//	移動方向指定(右２回)
		dm_manual_mode[dm_manual_next_next] = 53;					//	次の処理に
		return	dm_manual_work_on;
	case	53:
		//	10個目のカプセル終了
		dm_set_manual_str(manual_txt_4[3]);							//	説明文のデータをバッファにセット
		dm_manual_set_move( dm_manual_wait,54,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	54:
		dm_manual_mode_stop_flg = cap_flg_off;						//	内部制御停止
		manual_str_win_flg = cap_flg_on;							//	説明表示フラグをＯＮにする
		dm_manual_mode[dm_manual_now] = 55;							//	次の処理に
		return	dm_manual_work_on;
	case	55:
		//	説明終了
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;						//	内部制御開始
			manual_str_win_flg = cap_flg_off;							//	説明表示フラグをoffにする
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[19],cap_turn_l);	//	移動方向指定(左３回)
			dm_manual_mode[dm_manual_next_next] = 56;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	56:
		//	11個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,57,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	57:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[20],cap_turn_r);	//	移動方向指定(右１回)
		dm_manual_mode[dm_manual_next_next] = 60;					//	次の処理に
		return	dm_manual_work_on;
	case	60:
		//	12個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,61,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	61:
		dm_manual_set_move( dm_manual_rotate,62,1,manual_main_time_table_4[21],cap_turn_r);	//	回転方向指定(右１回)
		return	dm_manual_work_on;
	case	62:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[22],cap_turn_l);	//	移動方向指定(左３回)
		dm_manual_mode[dm_manual_next_next] = 63;					//	次の処理に
		return	dm_manual_work_on;
	case	63:
		//	13個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[23],cap_turn_r);	//	回転方向指定(右１回)
		dm_manual_mode[dm_manual_next_next] = 65;					//	次の処理に
		return	dm_manual_work_on;
	case	65:
		//	14個目のカプセル終了
		dm_manual_set_move( dm_manual_wait,66,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	66:
		//	15個目のカプセル終了
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,67,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	67:
		dm_manual_mode_stop_flg = cap_flg_off;					//	内部制御停止
		manual_str_win_flg = cap_flg_on;						//	説明表示フラグをＯＮにする
		dm_set_manual_str(manual_txt_4[4]);						//	説明文のデータをバッファにセット
		dm_manual_timer = 0;									//	タイマーのクリア
		manual_flash_pos[0] = dm_wold_x;						//	点滅エリア開始Ｘ座標
		manual_flash_pos[1] = dm_wold_y + 10;					//	点滅エリア開始Ｙ座標
		manual_flash_alpha = 1;									//	点滅のフラグ
		dm_manual_mode[dm_manual_now] = 68;						//	次の処理に
		return	dm_manual_work_on;
	case	68:
		if( manual_str_flg < 0 ){
			manual_flash_alpha = 0;
			dm_manual_mode_stop_flg = cap_flg_on;						//	内部制御開始
			dm_set_manual_str(manual_txt_4[5]);							//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 69;							//	次の処理に
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;
				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	69:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[24],cap_turn_r);	//	回転方向指定(右１回)
		dm_manual_mode[dm_manual_next_next] = 70;					//	次の処理に
		return	dm_manual_work_on;
	case	70:
		//	16個目のカプセル終了
		if( game_map_data[0][(15 << 3) + 7].capsel_m_g == capsel_b ){
					//	15個目のカプセル終了
			dm_manual_set_move( dm_manual_wait,71,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	71:
		dm_manual_set_move( dm_manual_rotate,72,1,manual_main_time_table_4[25],cap_turn_l);	//	回転方向指定(左１回)
		return	dm_manual_work_on;
	case	72:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[26],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 73;					//	次の処理に
		return	dm_manual_work_on;
	case	73:
		if( !game_map_data[0][(15 << 3) + 4].capsel_m_flg[cap_disp_flg] ){
			manual_str_win_flg = cap_flg_off;							//	説明表示フラグをＯＦＦにする
			dm_manual_set_move( dm_manual_wait,74,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	74:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[27],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 75;					//	次の処理に
		return	dm_manual_work_on;
	case	75:
		if( !game_map_data[0][(11 << 3) + 5].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,76,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	76:
		dm_manual_set_move( dm_manual_rotate,77,1,manual_main_time_table_4[28],cap_turn_l);	//	回転方向指定(左１回)
		return	dm_manual_work_on;
	case	77:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_4[29],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 80;					//	次の処理に
		return	dm_manual_work_on;
	case	80:
		if( !game_map_data[0][(13 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			dm_set_manual_str(manual_txt_4[6]);							//	説明文のデータをバッファにセット
			manual_str_win_flg = cap_flg_on;							//	説明表示フラグをＯＮにする
			dm_manual_set_move( dm_manual_wait,81,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	81:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_4[30],cap_turn_r);	//	回転方向指定(右３回)
		dm_manual_mode[dm_manual_next_next] = 82;					//	次の処理に
		return	dm_manual_work_on;
	case	82:
		dm_manual_set_move( dm_manual_wait,83,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	83:
		dm_manual_set_move( dm_manual_rotate,84,1,manual_main_time_table_4[31],cap_turn_l);	//	回転方向指定(左１回)
		return	dm_manual_work_on;
	case	84:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,4,manual_main_time_table_4[32],cap_turn_r);	//	回転方向指定(右４回)
		dm_manual_mode[dm_manual_next_next] = 85;					//	次の処理に
		return	dm_manual_work_on;
	case	85:
		dm_manual_set_move( dm_manual_wait,90,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	90:
		dm_manual_set_move( dm_manual_translate,91,1,manual_main_time_table_4[33],cap_turn_r);	//	移動方向指定(右１回)
		return	dm_manual_work_on;
	case	91:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[34],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 92;					//	次の処理に
		return	dm_manual_work_on;
	case	92:
		dm_manual_set_move( dm_manual_wait,93,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	93:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_4[35],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 94;					//	次の処理に
		return	dm_manual_work_on;
	case	94:
		//	消滅待ち
		if( game_map_data[0][(15 << 3) + 3].capsel_m_g == capsel_b ){
			manual_str_stop_flg = cap_flg_off;								//	文字列進行の停止
			manual_str_win_flg = cap_flg_off;								//	説明表示フラグをＯＦＦにする
			dm_manual_set_move( dm_manual_wait,95,1,30,cap_turn_r);			//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	95:
		dm_manual_set_move( dm_manual_translate,96,3,manual_main_time_table_4[36],cap_turn_r);	//	移動方向指定(右３回)
		return	dm_manual_work_on;
	case	96:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[37],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 100;					//	次の処理に
		return	dm_manual_work_on;
	case	100:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,101,1,30,cap_turn_r);			//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	101:
		dm_manual_set_move( dm_manual_translate,102,1,manual_main_time_table_4[38],cap_turn_l);	//	移動方向指定(左１回)
		return	dm_manual_work_on;
	case	102:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[39],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 103;					//	次の処理に
		return	dm_manual_work_on;
	case	103:
		dm_manual_set_move( dm_manual_wait,104,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	104:
		dm_manual_set_move( dm_manual_translate,105,1,manual_main_time_table_4[40],cap_turn_l);	//	移動方向指定(左１回)
		return	dm_manual_work_on;
	case	105:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_4[41],cap_turn_l);	//	回転方向指定(左１回)
		dm_manual_mode[dm_manual_next_next] = 110;					//	次の処理に
		return	dm_manual_work_on;
	case	110:
		if( ret == dm_ret_clear ){
			dm_set_manual_str(manual_txt_4[7]);				//	説明文のデータをバッファにセット
			manual_str_stop_flg = cap_flg_on;				//	文字列進行の開始
			manual_str_win_flg = cap_flg_on;				//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;				//	文字列進行開始
			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 111;			//	次の処理へ
		}
		return	dm_manual_work_on;
	case	111:
		if( ret == dm_ret_next_stage ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 112;			//	次の処理へ
		}
		return	dm_manual_work_on;
	case	112:
		if(dm_manual_timer > manual_main_time_table_4[42] && manual_str_flg < 0 ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_set_manual_str(manual_txt_4[8]);				//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 120;
		}
		return	dm_manual_work_on;
	case	120:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			manual_key_flash_count = 0;						//	キー点滅停止
			//	落下しなくなった
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
			manual_key_flash_count = 4;						//	キー点滅時間の設定
			manual_key_flash_pos = 1;						//	点滅させるキーの設定（下）
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	キー点滅時間の設定
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 2;								//	点滅させるキーの設定（右）
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 0;								//	点滅させるキーの設定（左）
			}

			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;

			manual_key_flash_count = 8;										//	キー点滅時間の設定
			if( dm_manual_move_vec == cap_turn_r ){
				manual_key_flash_pos = 4;								//	点滅させるキーの設定（Ａ）
			}else	if( dm_manual_move_vec == cap_turn_l ){
				manual_key_flash_pos = 3;								//	点滅させるキーの設定（Ｂ）
			}

			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}

}

/*--------------------------------------
	操作説明その５処理メイン関数
--------------------------------------*/
s8	dm_manual_5_main(void)
{
	s16	i,j;
	s8	ret;
	game_state	*state;
	game_map	*map;


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		ret = dm_manual_main_cnt( state,map,0,1 );
		dm_manual_main_cnt( &game_state_data[1],game_map_data[1],1,1 );
	}
	dm_manual_effect_cnt( state,map,0,1 );	//	連鎖等の特種表示の処理
	dm_manual_attack_capsel_down();

	//	タイマー処理
	dm_manual_timer ++;

	for(i = 0;i < 2;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	ウイルスアニメーション
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 102;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 30;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 128;							//	キノピオの座標


		manual_str_win_flg = cap_flg_off;				//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをOFFにする

		dm_manual_mode[dm_manual_now] = 1;				//	次の処理に

		for( i = 0;i < 20;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_5_CapsMagazine[i];
		}
		state -> cap_magazine_cnt = 1;					//	マガジン残照を1にする
		dm_set_capsel( state );							//	カプセル設定

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置

		//	１P側
		if( state -> virus_number < 27 ){
			set_virus( map,manual_main_5_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_5_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_5_virus_table_a[state -> virus_number][0],	//	ウイルスの色
									virus_anime_table[manual_main_5_virus_table_a[state -> virus_number][0]][state -> virus_anime] );	//	アニメ番号
			state -> virus_number ++;
		}

		//	2P側
		set_virus( game_map_data[1],manual_main_5_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
						manual_main_5_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
							manual_main_5_virus_table_b[state -> virus_number][0],	//	ウイルスの色
								virus_anime_table[manual_main_5_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	アニメ番号

		game_state_data[1].virus_number ++;

		if( game_state_data[1].virus_number > 27 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_5[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			state -> game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
			state -> game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御開始
			dm_set_manual_str( manual_txt_5[1] );					//	説明文のデータをバッファにセット
			dm_manual_set_move( dm_manual_wait,11,1,30,cap_turn_r);	//	一段目落下までのウェイト
		}
		return	dm_manual_work_on;
	case	11:
		dm_manual_set_move( dm_manual_translate,12,1,manual_main_time_table_5[0],cap_turn_r);	//	移動方向の設定(右１回)
		return	dm_manual_work_on;
	case	12:
		dm_manual_set_move( dm_manual_rotate,13,1,manual_main_time_table_5[1],cap_turn_r);	//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	13:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる( 一時的 )
		if( state -> now_cap.pos_y[0] == 10 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_translate,14,1,manual_main_time_table_5[2],cap_turn_l);	//	移動方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	14:
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,15,1,30,cap_turn_r);	//	一段目落下までのウェイト
		}
		return	dm_manual_work_on;
	case	15:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[3],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 20;			//	次の処理に
		return	dm_manual_work_on;
	case	20:
		dm_manual_set_move( dm_manual_wait,21,1,30,cap_turn_r);	//	一段目落下までのウェイト
		return	dm_manual_work_on;
	case	21:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[4],cap_turn_l);	//回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 22;			//	次の処理に
		return	dm_manual_work_on;
	case	22:
		//	消滅待ち
		if( !game_map_data[0][(8 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],4,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			dm_set_manual_str( manual_txt_5[2] );					//	説明文のデータをバッファにセット
			manual_pino_flip_flg = 1;								//	キノピオの向き（右向き）
			dm_manual_set_move( dm_manual_wait,23,1,30,cap_turn_r);	//	一段目落下までのウェイト
		}
		return	dm_manual_work_on;
	case	23:
		dm_manual_mode_stop_flg = cap_flg_off;						//	内部制御開始
		if( manual_str_flg < 0 ){
			manual_pino_flip_flg = 0;								//	キノピオの向き（左向き）
			dm_set_manual_str( manual_txt_5[3] );					//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御開始
			dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_5[5],cap_turn_r);	//	移動方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[6],cap_turn_r);	//	回転方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 25;					//	次の処理に
		return	dm_manual_work_on;
	case	25:
		dm_manual_set_move( dm_manual_wait,30,1,30,cap_turn_r);		//	一段目落下までのウェイト
		return	dm_manual_work_on;dm_manual_wait;
	case	30:
		dm_manual_set_move( dm_manual_rotate,31,1,manual_main_time_table_5[7],cap_turn_r);	//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_translate,32,4,manual_main_time_table_5[8],cap_turn_r);	//	移動方向の設定(右４回)
		return	dm_manual_work_on;
	case	32:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる( 一時的 )
		if( state -> now_cap.pos_y[0] == 11 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_rotate,33,1,manual_main_time_table_5[9],cap_turn_l);	//	回転方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	33:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_5[10],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 34;					//	次の処理に
		return	dm_manual_work_on;
	case	34:
		dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);		//	一段目落下までのウェイト
		return	dm_manual_work_on;
	case	35:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[11],cap_turn_l);	//	移動方向の設定(左３回)
		dm_manual_mode[dm_manual_next_next] = 36;					//	次の処理に
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_wait,37,1,30,cap_turn_r);		//	一段目落下までのウェイト
		return	dm_manual_work_on;
	case	37:
		dm_manual_set_move( dm_manual_translate,38,1,manual_main_time_table_5[12],cap_turn_l);	//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	38:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[13],cap_turn_r);	//	回転方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 40;					//	次の処理に
		return	dm_manual_work_on;
	case	40:
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);		//	一段目落下までのウェイト
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_5[14],cap_turn_l);	//	移動方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 42;			//	次の処理に
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_wait,43,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	43:
		dm_manual_set_move( dm_manual_translate,44,3,manual_main_time_table_5[15],cap_turn_l);	//	移動方向の設定(左３回)
		return	dm_manual_work_on;
	case	44:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[16],cap_turn_r);	//	回転方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 45;			//	次の処理に
		return	dm_manual_work_on;
	case	45:
		dm_manual_set_move( dm_manual_wait,46,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	46:
		dm_manual_set_move( dm_manual_translate,50,1,manual_main_time_table_5[21],cap_turn_l);			//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	50:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[22],cap_turn_r);	//	回転方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 51;			//	次の処理に
		return	dm_manual_work_on;
	case	51:
		if( !game_map_data[0][(12 << 3)].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],3,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][2],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;

			dm_set_manual_str_link(manual_txt_5[4]);						//	説明文のデータをバッファにセット
			manual_pino_flip_flg = 1;									//	キノピオの向き（左向き）
			dm_manual_set_move( dm_manual_wait,52,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	52:
		dm_manual_set_move( dm_manual_rotate,53,1,manual_main_time_table_5[17],cap_turn_r);		//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	53:
		dm_manual_set_move( dm_manual_translate,54,4,manual_main_time_table_5[18],cap_turn_r);	//	移動方向の設定(右４回)
		return	dm_manual_work_on;
	case	54:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる( 一時的 )
		if( state -> now_cap.pos_y[0] == 9 ){
			state -> cap_speed_vec = 1;
			dm_manual_set_move( dm_manual_rotate,55,1,manual_main_time_table_5[19],cap_turn_l);	//	回転方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	55:
		dm_manual_set_move( dm_manual_translate,56,1,manual_main_time_table_5[20],cap_turn_l);	//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	56:
		//	消滅待ち
		if( game_map_data[0][(9 << 3) + 4].capsel_m_g == capsel_b ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_blue );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],3,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;									//	キノピオの向き（左向き）
			dm_manual_set_move( dm_manual_wait,57,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	57:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,2,manual_main_time_table_5[21],cap_turn_r);	//	移動方向の設定(右２回)
		dm_manual_mode[dm_manual_next_next] = 60;			//	次の処理に
		return	dm_manual_work_on;
	case	60:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg = 0;										//	キノピオの向き変更（左向き）
			manual_str_win_flg = cap_flg_off;								//	説明表示フラグをＯＦＦにする
		}
		//	消滅待ち
		if( game_map_data[0][(13 << 3) + 3].capsel_m_g == capsel_b ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],0,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],7,capsel_yellow );
			dm_manual_at_cap_count[1] = 0;

			manual_pino_flip_flg = 1;									//	キノピオの向き（左向き）
			dm_manual_mode[dm_manual_now] = 61;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	61:
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	内部制御停止
			manual_str_win_flg = cap_flg_on;						//	説明表示フラグをＯＮにする
			dm_set_manual_str( manual_txt_5[5] );					//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 62;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	62:
		//	カプセル点滅処理
		if( manual_str_flg < 0 ){
			dm_set_manual_str(manual_txt_5[6]);						//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御開始
			manual_pino_flip_flg = 0;								//	キノピオの向き（左向き）
			dm_manual_mode[dm_manual_now]		= dm_manual_down;	//	高速落下
			dm_manual_mode[dm_manual_next_next] = 63;				//	次の処理に
			for(i = 0;i < 12;i++){
				j = ((manual_main_5_virus_table_c[i][2] - 1) << 3) + manual_main_5_virus_table_c[i][1];
				game_map_data[1][j].capsel_m_p = manual_main_5_virus_table_c[i][0];	//	元の色に戻す
			}
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				for( i = 0;i < 12;i++ ){
					j = ((manual_main_5_virus_table_c[i][2] - 1) << 3) + manual_main_5_virus_table_c[i][1];
					if( game_map_data[1][j].capsel_m_p < 3 ){	//	明るかったら暗くする
						game_map_data[1][j].capsel_m_p += 3;
					}else{									//	暗かったら明るくする
						game_map_data[1][j].capsel_m_p -= 3;
					}
				}
			}
		}
		return	dm_manual_work_on;
	case	63:
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);		//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_translate,65,1,manual_main_time_table_5[25],cap_turn_r);	//	移動方向の設定(右１回)
		return	dm_manual_work_on;
	case	65:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,2,manual_main_time_table_5[26],cap_turn_l);		//	回転方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 66;			//	次の処理に
		return	dm_manual_work_on;
	case	66:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3)+3].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],4,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;										//	キノピオの向き（左向き）
			dm_manual_mode[dm_manual_now]		= 67;	//	次の処理へ
		}
		return	dm_manual_work_on;
	case	67:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	内部制御停止
			manual_str_win_flg = cap_flg_off;						//	説明表示フラグをＯＦＦにする
			game_state_data[1].virus_number = 0;					//	ウイルス数を０にする
			clear_map_all( game_map_data[1] );						//	2Pの瓶の中をクリア
			dm_manual_mode[dm_manual_now] = 70;						//	次の処理へ
		}
		return	dm_manual_work_on;
	case	70:
		//	ウイルス再配置
		set_virus( game_map_data[1],manual_main_5_virus_table_d[ game_state_data[1].virus_number ][1],		//	ウイルスＸ座標
					manual_main_5_virus_table_d[ game_state_data[1].virus_number ][2],		//	ウイルスＹ座標
						manual_main_5_virus_table_d[ game_state_data[1].virus_number ][0],	//	ウイルスの色
							virus_anime_table[manual_main_5_virus_table_d[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	アニメ番号

		game_state_data[1].virus_number ++;
		if( game_state_data[1].virus_number > 80 ){
			manual_flash_pos[0] = dm_wold_x_vs_1p + 184;			//	点滅エリア開始Ｘ座標
			manual_flash_pos[1] = dm_wold_y + 40;					//	点滅エリア開始Ｙ座標
			manual_flash_alpha = cap_flg_on;						//	点滅のフラグ

			manual_str_win_flg = cap_flg_on;						//	説明表示フラグをＯＮにする
			dm_set_manual_str( manual_txt_5[7] );					//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 71;						//	次の処理へ
			dm_manual_timer = 0;									//	タイマークリア
		}
		return	dm_manual_work_on;
	case	71:
		//	点滅処理
		if( manual_str_flg < 0 ){
			manual_flash_alpha = cap_flg_off;						//	点滅終了
			dm_set_manual_str(manual_txt_5[8]);						//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御開始
			dm_manual_mode[dm_manual_next_next] = 72;				//	次の処理に
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[27],cap_turn_r);	//	移動方向の設定(右３回)
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	72:
		dm_manual_set_move( dm_manual_wait,73,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	73:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_5[28],cap_turn_r);	//	移動方向の設定(右３回)
		dm_manual_mode[dm_manual_next_next] = 74;				//	次の処理に
		return	dm_manual_work_on;
	case	74:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3) + 7].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_red );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],5,capsel_red );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;								//	キノピオの向き（左向き）
			dm_manual_mode[dm_manual_now] = 75;						//	次の処理へ
		}
		return	dm_manual_work_on;
	case	75:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][0].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_off;					//	内部制御停止
			game_state_data[1].virus_number = 0;					//	ウイルス数を０にする
			clear_map_all( game_map_data[1] );						//	マップ情報初期化
			dm_manual_mode[dm_manual_now] = 80;						//	次の処理へ
		}
		return	dm_manual_work_on;
	case	80:
		set_virus( game_map_data[1],manual_main_3_virus_table_b[ game_state_data[1].virus_number ][1],		//	ウイルスＸ座標
					manual_main_3_virus_table_b[ game_state_data[1].virus_number ][2],		//	ウイルスＹ座標
						manual_main_3_virus_table_b[ game_state_data[1].virus_number ][0],	//	ウイルスの色
							virus_anime_table[manual_main_3_virus_table_b[game_state_data[1].virus_number][0]][game_state_data[1].virus_anime] );	//	アニメ番号

		game_state_data[1].virus_number ++;
		if( game_state_data[1].virus_number > 3 ){
			manual_flash_pos[0] = dm_wold_x_vs_1p + 184;			//	点滅エリア開始Ｘ座標
			manual_flash_pos[1] = dm_wold_y + 120;					//	点滅エリア開始Ｙ座標
			manual_flash_alpha = 1;									//	点滅のフラグ
			dm_manual_timer = 0;									//	タイマークリア
			dm_set_manual_str( manual_txt_5[9] );					//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 81;						//	次の処理へ
		}
		return	dm_manual_work_on;
	case	81:
		//	点滅処理
		if( manual_str_flg < 0 ){
			manual_flash_alpha = cap_flg_off;						//	点滅終了
			dm_set_manual_str(manual_txt_5[10]);					//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;					//	内部制御開始
			dm_manual_mode[dm_manual_next_next] = 82;				//	次の処理に
			dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_5[29],cap_turn_r);		//	回転方向の設定(右１回)
		}else{
			if( dm_manual_timer >= 10 ){
				dm_manual_timer = 0;

				manual_flash_alpha ^= 1;
			}
		}
		return	dm_manual_work_on;
	case	82:
		dm_manual_set_move( dm_manual_wait,83,1,30,cap_turn_r);												//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	83:
		dm_manual_set_move( dm_manual_rotate,84,1,manual_main_time_table_5[30],cap_turn_r);		//	回転方向の設定(右１回)
		return	dm_manual_work_on;
	case	84:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_5[31],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 85;			//	次の処理に
		return	dm_manual_work_on;
	case	85:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3)].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],1,capsel_yellow );
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );
			dm_manual_at_cap_count[1] = 0;
			manual_pino_flip_flg = 1;					//	キノピオの向き（右向き）
			dm_manual_mode[dm_manual_now]		= 86;	//	次の処理へ
		}
		return	dm_manual_work_on;
	case	86:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){
			manual_pino_flip_flg =	0;										//	キノピオの向き（左向き）
			manual_str_win_flg = cap_flg_off;								//	説明表示フラグをＯＦＦにする
		}

		if( ret == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 90;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	90:
		if( ret == dm_ret_end ){
			dm_manual_timer = 0;							//	タイマークリア
			dm_manual_mode[dm_manual_now] = 91;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	91:
		if( dm_manual_timer > manual_main_time_table_5[32]){
			manual_str_win_flg = cap_flg_on;			//	メッセージウインドウの描画ＯＮ
			dm_set_manual_str(manual_txt_5[11]);			//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 92;			//	次の処理へ
		}
		return	dm_manual_work_on;
	case	92:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			//	落下しなくなった
			state -> cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( state -> cap_speed_vec % 2 )
					state -> cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,state,dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&state -> now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}

}
/*--------------------------------------
	操作説明その６処理メイン関数
--------------------------------------*/
s8	dm_manual_6_main(void)
{
	s16	i,j;
	s8	ret[4];
	s8	map_table[] = {0,2,3};
	game_state	*state;
	game_map	*map;
	s8	flash_cap_pos_table_blue[] = {59,67,83,91};
	s8	flash_cap_pos_table_red[] = {62,86,94,102};
	s8	flash_cap_pos_table_yellow[] = {58,82,90};


	state = &game_state_data[0];
	map = game_map_data[0];

	if( dm_manual_mode_stop_flg ){
		for( i = 0;i < 4;i++ ){
			ret[i] =  dm_manual_main_cnt( &game_state_data[i],game_map_data[i],i,2 );
		}
	}
	dm_manual_effect_cnt( state,map,0,2 );	//	連鎖等の特種表示の処理
	dm_manual_effect_cnt( &game_state_data[1],game_map_data[1],1,2 );	//	連鎖等の特種表示の処理
	dm_manual_attack_capsel_down();


	//	タイマー処理
	dm_manual_timer ++;

	for(i = 0;i < 4;i++){
		dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	ウイルスアニメーション
	}

	switch( dm_manual_mode[dm_manual_now] )
	{
	case	0:	//	初期化
		manual_str_win_pos[0] = 175;					//	メッセージウインドウの初期Ｘ座標
		manual_str_win_pos[1] = 15;						//	メッセージウインドウの初期Ｙ座標
		manual_pino_flip_flg = 0;						//	キノピオの向き（左向き）
		manual_pino_pos	= 160;							//	キノピオの座標


		manual_str_win_flg = cap_flg_off;				//	説明表示フラグをOFFにする
		manual_str_stop_flg = cap_flg_off;				//	文字列進行フラグをOFFにする

		dm_manual_6_ef_flg = 0;							//	点滅処理無し
		dm_manual_flash_count = 0;						//	操作する瓶の番号の設定
		dm_manual_mode[dm_manual_now] = 1;				//	次の処理へ

		for( i = 0;i < 15;i ++ ){						//	落下カプセル作成
			CapsMagazine[i + 1] = manual_6_CapsMagazine[i];
		}
		for( i = 0;i < 4;i++ ){
			game_state_data[i].cap_magazine_cnt = 1;	//	マガジン残照を1にする
			dm_set_capsel( &game_state_data[i] );		//	カプセル設定
		}

		//	グラフィックの設定
		graphic_no = GFX_MANUAL;

		return	dm_manual_work_on;
	case	1:	//	ウイルス配置
		//	１Pウイルス
		if( state -> virus_number < 20 ){
			set_virus( game_map_data[0],
						manual_main_6_virus_table_a[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_6_virus_table_a[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_6_virus_table_a[state -> virus_number][0],		//	ウイルスの色
									virus_anime_table[manual_main_6_virus_table_a[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	アニメ番号
		}

		//	2Pウイルス
		if( state -> virus_number < 83 ){
			set_virus( game_map_data[1],
						manual_main_6_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_6_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_6_virus_table_b[state -> virus_number][0],		//	ウイルスの色
									virus_anime_table[manual_main_6_virus_table_b[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	アニメ番号
		}

		//	3Pウイルス
		if( state -> virus_number < 44 ){
			set_virus( game_map_data[2],
						manual_main_6_virus_table_c[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_6_virus_table_c[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_6_virus_table_c[state -> virus_number][0],		//	ウイルスの色
									virus_anime_table[manual_main_6_virus_table_c[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	アニメ番号
		}

		//	4Pウイルス
		if( state -> virus_number < 4 ){
			set_virus( game_map_data[3],
						manual_main_3_virus_table_b[state -> virus_number][1],		//	ウイルスＸ座標
							manual_main_3_virus_table_b[state -> virus_number][2],		//	ウイルスＹ座標
								manual_main_3_virus_table_b[state -> virus_number][0],		//	ウイルスの色
									virus_anime_table[manual_main_3_virus_table_b[state -> virus_number][0]][game_state_data[0].virus_anime] );	//	アニメ番号
		}


		state -> virus_number ++;

		if( state -> virus_number > 82 ){
			manual_str_win_flg = cap_flg_on;			//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;			//	文字列進行フラグをＯＮにする
			dm_set_manual_str( manual_txt_6[0] );		//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 10;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	10:	//	文書１表示
		if( manual_str_flg < 0 ){
			dm_set_manual_str( manual_txt_6[1] );			//	説明文のデータをバッファにセット
			dm_manual_mode[dm_manual_now] = 11;				//	次の処理に
		}
		return	dm_manual_work_on;
	case	11:	//	文書２表示
		if( manual_str_flg < 0 ){
			dm_manual_mode[dm_manual_now] = 12;					//	次の処理に
			dm_manual_mode_stop_flg = cap_flg_on;				//	内部制御進行開始
			for( i = 0;i < 2;i++ ){
				game_state_data[i].game_condition[dm_mode_now] = dm_cnd_manual;	//	カプセル落下開始
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			}
		}
		return	dm_manual_work_on;
	case	12:
		//	２Ｐのカプセルが落下したら
		if( !game_state_data[1].now_cap.capsel_flg[cap_down_flg] ){
			dm_set_manual_str( manual_txt_6[2] );				//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_off;				//	内部制御進行開始
			dm_manual_waku_flash_flg[1] = cap_flg_on;			//	点滅フラグを立てる
			dm_manual_mode[dm_manual_now] = 13;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	13:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;				//	アニメーション進行再開
			dm_set_manual_str(manual_txt_6[3]);					//	説明文のデータをバッファにセット
			dm_manual_waku_flash_flg[1] = cap_flg_off;			//	点滅フラグを消す
			dm_manual_mode[dm_manual_now] = 14;					//	次の処理に
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_init;	//	2Pのカプセル落下停止
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_init;		//	2Pのカプセル落下停止
		}
		return	dm_manual_work_on;
	case	14:
		if( state -> now_cap.pos_y[0] == 8 ){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_6[0],cap_turn_r);	//	移動方向の設定(右３回)
			dm_manual_mode[dm_manual_next_next] = 15;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	15:
		dm_manual_set_move( dm_manual_wait,16,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	16:
		dm_manual_set_move( dm_manual_rotate,17,2,manual_main_time_table_6[1],cap_turn_r);	//	回転方向の設定(右２回)
		return	dm_manual_work_on;
	case	17:
		if( state -> now_cap.pos_y[0] == 8 ){
			dm_manual_set_move( dm_manual_translate,dm_manual_down,3,manual_main_time_table_6[2],cap_turn_r);	//	移動方向の設定(右３回)
			dm_manual_mode[dm_manual_next_next] = 20;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	20:
		//	消滅待ち
		if( !game_map_data[0][(12 << 3) + 6].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][0],3,capsel_yellow );	//	左から２番目の瓶に攻撃カプセル設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[1][1],6,capsel_blue );	//	左から２番目の瓶に攻撃カプセル設定

			manual_bubble_col_flg[2] = 1;	//	青色バブル打ち上げ

			dm_manual_mode[dm_manual_now] = 21;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	21:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] )
		{
			dm_manual_mode_stop_flg = cap_flg_off;				//	アニメーション進行停止
			dm_set_manual_str(manual_txt_6[4]);					//	説明文のデータをバッファにセット
			dm_manual_waku_flash_flg[3] = cap_flg_on;			//	点滅フラグを立てる
			manual_pino_flip_flg = 1;							//	キノピオの向き（右向き）
			dm_manual_mode[dm_manual_now] = 22;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	22:
		if( manual_str_flg < 0 ){
			dm_manual_mode_stop_flg = cap_flg_on;				//	アニメーション進行開始
			dm_set_manual_str(manual_txt_6[5]);					//	説明文のデータをバッファにセット
			manual_pino_flip_flg = 0;							//	キノピオの向き（左向き）
			dm_manual_set_move( dm_manual_rotate,23,1,manual_main_time_table_6[3],cap_turn_r);	//	回転方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	23:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 8 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_translate,24,1,manual_main_time_table_6[4],cap_turn_l);	//	移動方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	24:
		dm_manual_set_move( dm_manual_rotate,25,1,manual_main_time_table_6[5],cap_turn_l);	//	回転方向の設定(左１回)
		return	dm_manual_work_on;
	case	25:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_6[6],cap_turn_l);	//	移動方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 26;			//	次の処理に
		return	dm_manual_work_on;
	case	26:
		dm_manual_set_move( dm_manual_wait,27,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;dm_manual_wait;
	case	27:
		dm_manual_set_move( dm_manual_translate,28,3,manual_main_time_table_6[7],cap_turn_l);	//	移動方向の設定(左３回)
		return	dm_manual_work_on;
	case	28:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[8],cap_turn_l);	//	回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 30;			//	次の処理に
		return	dm_manual_work_on;
	case	30:
		dm_manual_set_move( dm_manual_wait,31,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	31:
		dm_manual_set_move( dm_manual_rotate,32,1,manual_main_time_table_6[9],cap_turn_l);	//	回転方向の設定(左１回)
		return	dm_manual_work_on;
	case	32:
		dm_manual_set_move( dm_manual_translate,33,1,manual_main_time_table_6[10],cap_turn_l);	//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	33:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 11 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_translate,34,1,manual_main_time_table_6[11],cap_turn_l);	//	移動方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	34:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3)].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][0],1,capsel_red );	//	左から４番目の瓶に攻撃カプセル設定
			dm_manual_set_attack_capsel( &dm_manual_at_cap[3][1],6,capsel_yellow );	//	左から４番目の瓶に攻撃カプセル設定
			dm_manual_at_cap_count[3] = 0;	//	一番右攻撃用攻撃カプセルカウンターの初期化
			manual_bubble_col_flg[0] = 1;	//	赤色バブル打ち上げ

			manual_str_win_flg = cap_flg_off;							//	説明表示フラグをＯＦＦにする
			manual_pino_flip_flg = 1;									//	キノピオの向き（右向き）
			dm_manual_set_move( dm_manual_wait,35,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	35:
		//	攻撃カプセル落下待ち
		dm_manual_mode_stop_flg = cap_flg_off;					//	アニメーション進行停止
		if( !dm_manual_at_cap[3][1].capsel_a_flg[cap_disp_flg] ){
			dm_manual_mode_stop_flg = cap_flg_on;					//	アニメーション進行再開
			dm_set_manual_str(manual_txt_6[6]);						//	説明文のデータをバッファにセット
			manual_pino_flip_flg = 0;								//	キノピオの向き（左向き）
			manual_str_win_flg = cap_flg_on;						//	説明表示フラグをＯＮにする
			dm_manual_waku_flash_flg[3] = cap_flg_off;				//	点滅フラグを消す
			dm_manual_set_move( dm_manual_rotate,36,1,manual_main_time_table_6[12],cap_turn_l);	//	回転方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	36:
		dm_manual_set_move( dm_manual_translate,37,4,manual_main_time_table_6[13],cap_turn_r);	//	移動方向の設定(右４回)
		return	dm_manual_work_on;
	case	37:
		state -> cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる(一時的)
		if( state -> now_cap.pos_y[0] == 16 ){
			state -> cap_speed_vec = 1;							//	落下速度を戻す
			dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[14],cap_turn_l);	//	回転方向の設定(左１回)
			dm_manual_mode[dm_manual_next_next] = 40;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	40:
		dm_manual_set_move( dm_manual_wait,41,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	41:
		dm_manual_set_move( dm_manual_translate,42,2,manual_main_time_table_6[15],cap_turn_r);	//	移動方向の設定(右２回)
		return	dm_manual_work_on;
	case	42:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[16],cap_turn_l);	//	回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 43;					//	次の処理に
		return	dm_manual_work_on;
	case	43:
		//	消滅待ち
		if( !game_map_data[0][(15 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			//	攻撃カプセルの設定
			for( i = 1;i < 4;i += 2 ){
				dm_manual_set_attack_capsel( &dm_manual_at_cap[i][0],3,capsel_blue );	//	左から２・４番目の瓶に攻撃カプセル設定
				dm_manual_set_attack_capsel( &dm_manual_at_cap[i][1],7,capsel_yellow );	//	左から２・４番目の瓶に攻撃カプセル設定
				dm_manual_at_cap_count[i] = 0;
			}
			manual_bubble_col_flg[2] = 1;	//	青色バブル打ち上げ
			manual_bubble_col_flg[0] = 1;	//	赤色バブル打ち上げ

			dm_manual_mode[dm_manual_now] = 44;			//	次の処理に
		}
		return	dm_manual_work_on;
	case	44:
		//	攻撃カプセル落下待ち
		if( !dm_manual_at_cap[1][1].capsel_a_flg[cap_disp_flg] ){				//	2Pにカプセルを落下させゲームオーバーにする
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_manual;		//	カプセル落下開始
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_down;			//	カプセル落下開始
			dm_set_capsel( &game_state_data[1] );								//	カプセル設定
			dm_manual_mode[dm_manual_now] = 45;									//	次の処理に
		}
		return	dm_manual_work_on;
	case	45:
		if( ret[1] == dm_ret_game_over ){
			// 2P ゲームオーバー
			game_state_data[1].virus_anime_spead = v_anime_speed_4p;		//	アニメーション速度を早くする
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_tr_chaeck;	//	リタイア状態にする
			game_state_data[1].game_condition[2] = dm_cnd_retire;			//	リタイア状態にする
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_tr_chack;	//	リタイア状態にする
			game_state_data[1].work_flg = 16;								//	黒上がり用カウンタのセット
			game_state_data[1].retire_flg[dm_retire_flg] = 1;				//	リタイアフラグを立てる
			game_state_data[1].now_cap.capsel_flg[cap_disp_flg] = 0;		//	ネクストを表示しないようにする
			game_state_data[1].next_cap.capsel_flg[cap_disp_flg] = 0;		//	ネクストを表示しないようにする

			dm_manual_mode[dm_manual_now] = 46;			//	次の処理に

		}
		return	dm_manual_work_on;
	case	46:
		if( dm_manual_at_cap[3][0].pos_a_y >= 8 ){
			dm_set_manual_str(manual_txt_6[7]);					//	説明文のデータをバッファにセット
			manual_str_win_flg = cap_flg_on;					//	説明表示フラグをＯＮにする
			dm_manual_mode_stop_flg = cap_flg_off;				//	アニメーション進行停止
			dm_manual_mode[dm_manual_now] = 50;					//	次の処理に
		}
		return	dm_manual_work_on;
	case	50:
		if( manual_str_flg < 0 ){
			dm_set_manual_str(manual_txt_6[8]);					//	説明文のデータをバッファにセット
			dm_manual_mode_stop_flg = cap_flg_on;				//	アニメーション進行再開
			dm_manual_set_move( dm_manual_translate,51,2,manual_main_time_table_6[17],cap_turn_l);	//	移動方向の設定(左２回)
		}
		return	dm_manual_work_on;
	case	51:
		if( state -> now_cap.pos_y[0] == 11 ){
			dm_manual_set_move( dm_manual_translate,52,1,manual_main_time_table_6[18],cap_turn_r);	//	移動方向の設定(右１回)
		}
		return	dm_manual_work_on;
	case	52:
		//	着地待ち
		if( !state -> now_cap.capsel_flg[cap_down_flg] ){
			dm_manual_set_move( dm_manual_wait,53,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	53:
		dm_manual_set_move( dm_manual_translate,54,1,manual_main_time_table_6[19],cap_turn_l);	//	移動方向の設定(左１回)
		return	dm_manual_work_on;
	case	54:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[20],cap_turn_l);	//	回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 55;			//	次の処理に
		return	dm_manual_work_on;
	case	55:
		dm_manual_set_move( dm_manual_down,56,1,30,cap_turn_r);
		dm_manual_mode[dm_manual_next_next] = 56;			//	次の処理に
		return	dm_manual_work_on;
	case	56:
		if( !game_map_data[0][(13 << 3) + 2].capsel_m_flg[cap_disp_flg] ){
			dm_manual_set_move( dm_manual_wait,60,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	60:
		state -> game_mode[dm_mode_now] = dm_mode_init;	//	一時的に動けなくする
		dm_set_manual_str(manual_txt_6[9]);				//	説明文のデータをバッファにセット
		dm_manual_mode[dm_manual_now] = 61;				//	高速落下後の処理設定
		return	dm_manual_work_on;
	case	61:
		if( manual_str_flg < 0 ){
			dm_manual_6_ef_flg  = 1;							//	操作するカプセルの切り替え
			dm_set_manual_str(manual_txt_6[10]);				//	説明文のデータをバッファにセット

			//	練習用設定
			clear_map_all( game_map_data[1] );									//	2Pのマップ情報のクリア
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_down;			//	リタイア状態にする
			game_state_data[1].game_condition[dm_mode_now] = dm_cnd_manual;		//	リタイア状態にする
			game_state_data[1].game_condition[dm_training_flg] = dm_cnd_training;		//	練習に切りかえる
			game_state_data[1].game_condition[dm_static_cnd] = dm_cnd_lose;				//	練習に切りかえる
			game_state_data[1].retire_flg[dm_game_over_flg] = 1;	//	ゲームオーバーフラグを立てる( 念のため )
			game_state_data[1].warning_flg = 0;						//	警告音フラグのクリア
			game_state_data[1].cap_magazine_cnt = 13;				//	マガジン残照の設定(2Pに落ちてくるものに設定)
			dm_manual_mode[dm_manual_now] = 62;						//	次の処理へ
			dm_set_capsel( &game_state_data[1] );					//	カプセル設定

			dm_manual_set_move( dm_manual_wait,62,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		}
		return	dm_manual_work_on;
	case	62:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[21],cap_turn_l);	//	回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 63;			//	次の処理に
		return	dm_manual_work_on;
	case	63:
		dm_manual_set_move( dm_manual_wait,64,1,30,cap_turn_r);	//	30ｲﾝﾄの待ち
		return	dm_manual_work_on;
	case	64:
		dm_manual_set_move( dm_manual_rotate,dm_manual_down,1,manual_main_time_table_6[22],cap_turn_l);	//	回転方向の設定(左１回)
		dm_manual_mode[dm_manual_next_next] = 65;			//	次の処理に
		return	dm_manual_work_on;
	case	65:
		//	消滅待ち
		if( !game_map_data[1][(15 << 3) + 3].capsel_m_flg[cap_disp_flg] ){
			dm_manual_6_ef_flg  = 0;										//	操作するカプセルの切り替え
			state -> game_mode[dm_mode_now] = dm_mode_down;					//	1P:落下再開
			game_state_data[1].game_mode[dm_mode_now] = dm_mode_init;		//	2P:動けなくする
			manual_str_win_flg = cap_flg_off;								//	説明表示フラグをＯＦＦにする
			manual_str_stop_flg = cap_flg_off;								//	文字列進行の停止
			dm_manual_set_move( dm_manual_rotate,70,1,manual_main_time_table_6[23],cap_turn_l);	//	回転方向の設定(左１回)
		}
		return	dm_manual_work_on;
	case	70:
		dm_manual_set_move( dm_manual_translate,dm_manual_down,1,manual_main_time_table_6[24],cap_turn_r);	//	移動方向の設定(右１回)
		dm_manual_mode[dm_manual_next_next] = 71;			//	次の処理に
		return	dm_manual_work_on;
	case	71:
		//	消滅待ち
		if( ret[0] == dm_ret_clear ){
			state -> game_condition[dm_mode_now] = dm_cnd_win;	//	WIN
			state -> game_mode[dm_mode_now] = dm_mode_win;		//	WIN
			for( i = 1;i < 4;i++ ){
				game_state_data[i].game_condition[dm_mode_now] = dm_cnd_lose;	//	LOSE
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_lose;		//	LOSE
			}

			auSeqPlayerPlay(0,SEQ_VSEnd);					//	音楽再生
			manual_bgm_flg = 1;								//	再生フラグをＯＮにする
			manual_bgm_timer = 0;							//	再生時間カウンタをクリア
			manual_bgm_timer_max = manual_bgm_time_b;		//	再生時間の設定
			dm_manual_mode[dm_manual_now] = 72;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	72:
		if( ret[0] == dm_ret_end ){
			dm_set_manual_str(manual_txt_6[11]);			//	説明文のデータをバッファにセット
			manual_str_win_flg = cap_flg_on;				//	説明表示フラグをＯＮにする
			manual_str_stop_flg = cap_flg_on;				//	文字列進行の開始
			dm_manual_mode[dm_manual_now] = 73;				//	高速落下後の処理設定
		}
		return	dm_manual_work_on;
	case	73:
		if( manual_str_flg < 0 ){
			return	dm_manual_work_off;
		}else{
			return	dm_manual_work_on;
		}
	case	dm_manual_wait:				//	ウェイト処理
		if( dm_manual_timer >= dm_manual_timer_count ){
			dm_manual_timer = 0;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
		}
		return	dm_manual_work_on;
	case	dm_manual_down:				//	高速落下
		if( !game_state_data[dm_manual_6_ef_flg].now_cap.capsel_flg[cap_down_flg] ){
			//	落下しなくなった
			game_state_data[dm_manual_6_ef_flg].cap_speed_vec = 1;
			dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next_next];
		}else{
			if( dm_manual_timer >= dm_dilemma_wait ){
				game_state_data[dm_manual_6_ef_flg].cap_speed_vec = FallSpeed[state -> cap_def_speed] >> 1	;//	高速落下のためカプセル落下速度を上げる
				if( game_state_data[dm_manual_6_ef_flg].cap_speed_vec % 2 )
					game_state_data[dm_manual_6_ef_flg].cap_speed_vec ++;
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_translate:		//	左右移動
		if( dm_manual_timer >= dm_manual_timer_count ){
			translate_capsel(map,&game_state_data[dm_manual_6_ef_flg],dm_manual_move_vec,0);	//	カプセル左右移動
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	case	dm_manual_rotate:			//	回転
		if( dm_manual_timer >= dm_manual_timer_count ){
			rotate_capsel(map,&game_state_data[dm_manual_6_ef_flg].now_cap,dm_manual_move_vec);	//	カプセル回転
			dm_manual_timer = 0;
			dm_manual_move_count --;
			if( !dm_manual_move_count ){	//	指定回数終了
				dm_manual_mode[dm_manual_now] = dm_manual_mode[dm_manual_next];	//	次の処理
			}
		}
		return	dm_manual_work_on;
	}
}

/*--------------------------------------
	操作説明その１追加描画関数
--------------------------------------*/
void	dm_manual_1_graphic(void)
{
	switch( dm_manual_1_ef_flg )
	{
	case	1:	//	右向き矢印
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	パレットロード
		load_TexTile_4b( dm_manual_arrow_side_bm0_0,12,16,0,0,11,15 );	//	グラフィックデータロード
		draw_Tex( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 + 24,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 - 2,
						12,16,0,0 );

		break;
	case	2:	//	左向き矢印
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	パレットロード
		load_TexTile_4b( dm_manual_arrow_side_bm0_0,12,16,0,0,11,15 );	//	グラフィックデータロード
		draw_TexFlip( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 - 16,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 - 2,
						12,16,0,0,flip_on,flip_off );
		break;
	case	3:	//	下向き矢印
		load_TexPal( dm_manual_arrow_bm0_0tlut );					//	パレットロード
		load_TexTile_4b( dm_manual_arrow_bm0_0,16,12,0,0,15,11 );	//	グラフィックデータロード
		draw_Tex( game_state_data[0].map_x + game_state_data[0].now_cap.pos_x[0] * cap_size_10 + 2,
					game_state_data[0].map_y + game_state_data[0].now_cap.pos_y[0] * cap_size_10 + 14,
						16,12,0,0 );
		break;
	case	4:	//	横枠
		load_TexPal( manual_cap_waku_a_bm0_0tlut );					//	パレットロード
		load_TexTile_4b( manual_cap_waku_a_bm0_0,10,44,0,0,9,43 );	//	グラフィックデータロード
		draw_Tex( game_state_data[0].map_x + cap_size_10 * 3,
					game_state_data[0].map_y + cap_size_10 * 11 - 2,
						10,44,0,0 );
		break;
	case	5:	//	縦枠
		load_TexPal( manual_cap_waku_a_bm0_0tlut );					//	パレットロード
		load_TexBlock_4b( manual_cap_waku_b_bm0_0,64,10);	//	グラフィックデータロード
		draw_Tex( game_state_data[0].map_x - 1,
					game_state_data[0].map_y + cap_size_10 * 16,
						64,10,0,0 );

	}
}

/*--------------------------------------
	操作説明その３追加描画関数
--------------------------------------*/
void	dm_manual_3_graphic( void )
{
	s8	i,p_no,flg;
	game_state *enemy_state;

	switch( dm_manual_3_ef_flg )	//	枠を描画する瓶番号の設定
	{
	case	1:		//	左から３番目
		p_no = 2;
		break;
	case	2:		//	左から４番目
		p_no = 3;
		break;
	case	3:		//	左端
		p_no = 0;
		break;
	default:		//	その他の場合は、変数の初期化を行う
		for( i = 0;i < 4;i ++ ){
			dm_manual_waku_flash_count[i] = 0;
			dm_manual_waku_alpha[i] = 0xff;
		}
	}

	if( dm_manual_3_ef_flg ){	//	0以外の場合

		if( dm_manual_waku_flash_count[p_no] > 10  ){	//	10イント経過したか？
			dm_manual_waku_flash_count[p_no] = 0;		//	カウントのクリア
			if( dm_manual_waku_alpha[p_no] == 0xff ){	//	明るい場合
				dm_manual_waku_alpha[p_no] = 128;		//	暗くする
			}else{										//	暗い場合
				dm_manual_waku_alpha[p_no] = 0xff;		//	明るくする
			}
		}

		enemy_state = &game_state_data[p_no];			//	枠表示を行う瓶の情報の取得
		//	透明度の設定
		gDPSetPrimColor(gp++,0,0,dm_manual_waku_alpha[p_no],dm_manual_waku_alpha[p_no],dm_manual_waku_alpha[p_no],255);

		load_TexPal( manual_bin_waku_a_bm0_0tlut );	//	パレットロード
		//	データの読込み
		load_TexTile_4b( manual_bin_waku_a_bm0_0,70,6,0,0,69,5 );

		draw_Tex( enemy_state -> map_x - 3,57 ,70,3,0,0);	//	上
		draw_Tex( enemy_state -> map_x - 3,188,70,3,0,3);	//	下

		//	データの読込み
		load_TexTile_4b( manual_bin_waku_b_bm0_0,6,128,0,0,5,127 );

		draw_Tex( enemy_state -> map_x - 3,60 ,3,128,0,0);	//	上
		draw_Tex( enemy_state -> map_x + 64,60,3,128,3,0);	//	下

		dm_manual_waku_flash_count[p_no]  ++;				//	点滅カウンタの加算
		gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);		//	透明度(明るさ)を元に戻す
	}
}
/*--------------------------------------
	操作説明その６追加描画関数
--------------------------------------*/
void	dm_manual_6_graphic( game_state *state,game_a_cap *cap,s8 player_no )
{
	s8	i,flg;

	if( dm_manual_waku_flash_flg[ player_no ] ){			//	描画フラグが立っていた場合

		if( dm_manual_waku_flash_count[player_no] > 10  ){	//	10イント経過したか？
				dm_manual_waku_flash_count[player_no] = 0;	//	カウントのクリア
			if( dm_manual_waku_alpha[player_no] == 0xff ){	//	明るい場合
				dm_manual_waku_alpha[player_no] = 128;		//	暗くする
			}else{											//	暗い場合
				dm_manual_waku_alpha[player_no] = 0xff;		//	明るくする
			}
		}

		//	透明度の設定
		gDPSetPrimColor(gp++,0,0,dm_manual_waku_alpha[player_no],dm_manual_waku_alpha[player_no],dm_manual_waku_alpha[player_no],255);

		load_TexPal( manual_bin_waku_a_bm0_0tlut );	//	パレットロード
		//	データの読込み
		load_TexTile_4b( manual_bin_waku_a_bm0_0,70,6,0,0,69,5 );

		draw_Tex( state -> map_x - 3,57 ,70,3,0,0);	//	上
		draw_Tex( state -> map_x - 3,188,70,3,0,3);	//	下

		//	データの読込み
		load_TexTile_4b( manual_bin_waku_b_bm0_0,6,128,0,0,5,127 );

		draw_Tex( state -> map_x - 3,60 ,3,128,0,0);	//	上
		draw_Tex( state -> map_x + 64,60,3,128,3,0);	//	下

		flg = 1;
	}
	if( flg ){	//	枠表示をしている場合
		dm_manual_waku_flash_count[player_no]  ++;		//	点滅カウンタの増加
		gDPSetPrimColor(gp++,0,0,0xff,0xff,0xff,0xff);	//	透明度(明るさ)を元に戻す
	}else{		//	枠表示をしていない場合
		dm_manual_waku_flash_count[player_no] = 0;		//	点滅カウンタを０にする。
		dm_manual_waku_alpha[player_no] = 0xff;			//	透明度を0xff(不透明 = 明るい)に設定
	}
}


/*--------------------------------------
	操作説明その４追加描画関数
--------------------------------------*/
void	dm_manual_4_graphic( void )
{
	//	点滅描画
	if( manual_flash_alpha ){
		gDPSetRenderMode(  gp++,G_RM_XLU_SURF, G_RM_XLU_SURF2);
		disp_rect(255,0,0,128,manual_flash_pos[0],manual_flash_pos[1],80,30);
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );
	}

	gDPSetPrimColor(gp++,0,0,255,255,255,255);
}

/*--------------------------------------
	操作説明用攻撃カプセル描画関数
--------------------------------------*/
void	dm_manual_at_cap_draw( game_state *state,game_a_cap *cap,s8 size_flg )
{
	s8	i,j;
	u8	cap_size[] = {220,128};

	for( i = 0;i < 3;i++ ){	//	色の分だけループをまわす
		load_TexPal( cap_pal[size_flg][i] );	//	パレットロード
		for( j = 0;j < 4;j++ ){
			if( (cap + j) -> capsel_a_flg[cap_disp_flg] ){	//	表示フラグが立っている
				if( (cap + j) -> capsel_a_p == i ){				//	同じ色の場合
						draw_Tex( state -> map_x + ( cap + j ) -> pos_a_x *  state -> map_item_size,
									state -> map_y + ( cap + j ) -> pos_a_y * state -> map_item_size,
										state -> map_item_size,state -> map_item_size,0,capsel_b * state -> map_item_size );	//	ボール描画
				}
			}
		}
	}
}

/*--------------------------------------
	操作説明攻撃カプセル矢印描画関数
--------------------------------------*/
void	dm_manual_atarrow_draw( game_state *state,game_a_cap *cap,s8 flg )
{
	s8	j;
	s8	sub[] = {3,4};

	//	矢印パレットロード
	load_TexPal(dm_manual_arrow_bm0_0tlut);

	//	矢印グラフィックデータの読込み
	load_TexBlock_4b(&dm_manual_arrow_bm0_0[0],16,12);

	for( j = 0;j < 4;j++ ){
		if( (cap + j) -> capsel_a_flg[cap_disp_flg] ){	//	表示フラグが立っていた場合。
			draw_Tex( state -> map_x + ( cap + j ) -> pos_a_x *  state -> map_item_size - sub[flg],
						state -> map_y + ( cap + j ) -> pos_a_y * state -> map_item_size - 13,
								16,12,0,0 );
		}
	}
}
/*--------------------------------------
	キノピオ描画関数
--------------------------------------*/
void	disp_kinopio(s16 x_pos,s16 y_pos,s8	flip)
{
	//	キノピオ描画
	s8	i;

	gDPLoadTLUT_pal256(gp++,dm_mode_select_pino_bm0_0tlut);
	if( flip ){	//	反転フラグが立っていた場合
		for(i = 0;i < 2;i++){
			load_TexBlock_4b(&dm_mode_select_pino_bm0_0[i * 1344],64,42);
			gSPScisTextureRectangle(gp++,x_pos << 2,y_pos + i * 42 << 2,x_pos + 64 << 2,y_pos + 42 + i * 42 << 2,G_TX_RENDERTILE,63 << 5,0,((-1)<<10)&0xffff, 1 << 10);
		}
	}else{		//	反転フラグが立っていない場合
		for(i = 0;i < 2;i++){
			load_TexBlock_4b(&dm_mode_select_pino_bm0_0[i * 1344],64,42);
			draw_Tex(x_pos,y_pos + i * 42,64,42,0,0);
		}
	}

	//	クチパク
	if(manual_pino_anime_flg){
		load_TexTile_4b(dm_mode_select_pino_mouth_bm0_0,8,5,0,0,7,4);
		if( flip ){	//	反転フラグが立っていた場合
			gSPScisTextureRectangle(gp++,x_pos + 32 << 2,y_pos + 34 << 2,x_pos + 40 << 2,y_pos + 39 << 2,G_TX_RENDERTILE,7 << 5, 0,((-1)<<10)&0xffff, 1 << 10);
		}else{		//	反転フラグが立っていない場合
			draw_Tex(x_pos + 24,y_pos + 34,8,5,0,0);
		}
	}

}
/*--------------------------------------
	コントローラ描画関数
--------------------------------------*/
void	disp_cont(void)
{
	load_TexPal(dm_manual_b_flash_bm0_0tlut);
	load_TexTile_4b(&dm_manual_b_flash_bm0_0[0],14,14,0,0,13,13);

	//	キー点滅
	if(manual_key_flash_count > 0)
	{
		switch(manual_key_flash_pos)
		{
			//	キー
			case	0:	//	左
				draw_Tex(32,81,14,14,0,0);
				break;
			case	1:	//	下
				draw_Tex(40,88,14,14,0,0);
				break;
			case	2:	//	右
				draw_Tex(48,81,14,14,0,0);
				break;
			//	ボタンの場合
			case	3:	//	Ｂボタン	反時計回り
				draw_Tex(69,75,14,14,0,0);
				break;
			case	4:	//	Ａボタン	時計回り
				draw_Tex(80,87,14,14,0,0);
				break;
		}
	}
}

/*--------------------------------------
	操作説明処理メイン関数
--------------------------------------*/
int		dm_manual_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	u32	bg_no = 0;
	s16	i,j,k;
	s16	sec_count = 0;	//	デモ用カウンター
	s8	sec_30_flg = 0;	//	内部３０フレーム制御用
	s8	out_flg = 1;	//	ループ脱出用フラグ
	s8	flg_f = 0;


	//	メッセージキューの作成
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	メッセージＱノ割り当て
	//	クライアントに登録
	nnScAddClient(sched, &client, &msgQ);

	//	グラフィックデータの読込み(カプセル等)
	auRomDataRead((u32)_spriteSegmentRomStart, gfx_freebuf[GFX_ANIME_PAGE_1], (u32)_spriteSegmentRomEnd-(u32)_spriteSegmentRomStart);

	//	グラフィックデータの読込み(背景等)
	auRomDataRead((u32)_dm_manual_dataSegmentRomStart,(void *)_dm_manual_dataSegmentStart, (u32)_dm_manual_dataSegmentRomEnd-(u32)_dm_manual_dataSegmentRomStart);

	//	グラフィックデータの読込み(キノピオ)
	auRomDataRead((u32)_dm_mode_select_dataSegmentRomStart,(void *)_dm_mode_select_dataSegmentStart, (u32)(_dm_mode_select_dataSegmentRomEnd - _dm_mode_select_dataSegmentRomStart));

	dm_manual_debug_bg_no = evs_manual_no;
	switch(dm_manual_debug_bg_no)
	{
	case	0:
	case	3:
		evs_gamesel = GSL_1PLAY;	//	SE の設定のため
		bg_no = 0;
		auRomDataRead((u32)_dm_bg_manual_aSegmentRomStart, BGBuffer, (u32)_dm_bg_manual_aSegmentRomEnd - (u32)_dm_bg_manual_aSegmentRomStart);
		//	ウイルスアニメデータ読込み
		auRomDataRead( (u32)dm_anime_rom_address[12][0],gfx_freebuf[GFX_ANIME_PAGE_2],(u32)dm_anime_rom_address[12][1] - (u32)dm_anime_rom_address[12][0] );	//	ウイルス
		//	ウイルスアニメーション設定
		for( i = 0;i < 3;i++ ){
			dm_manual_big_virus_anime[i].cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];
			dm_anime_char_set(&dm_manual_big_virus_anime[i],game_anime_table[i + 12] );
			dm_anime_set( &dm_manual_big_virus_anime[i],ANIME_nomal );
		}
		break;
	case	1:
	case	4:
		evs_gamesel = GSL_2PLAY;	//	SE の設定のため
		bg_no = 1;
		auRomDataRead((u32) _bg_dat_exSegmentRomStart, BGBuffer, (u32)_bg_dat_exSegmentRomEnd - (u32)_bg_dat_exSegmentRomStart);

		break;
	case	2:
	case	5:
		evs_gamesel = GSL_4PLAY;	//	SE の設定のため
		bg_no = 2;
		auRomDataRead((u32)_dm_bg_vs4p_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_vs4p_modeSegmentRomEnd - (u32)_dm_bg_vs4p_modeSegmentRomStart);
		break;
	}

	dm_manual_all_init();	//	初期化


	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);


	//	メインループ
	while(out_flg > 0)
	{
		#ifndef THREAD_JOY
		joyProcCore();
		#endif

		//	リトレースメッセージ
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);

		//	文字列進行処理
		manual_str_flg = dm_manual_str_main();
		//	PUSH ANY KEY の揺れ処理
		push_any_key_main();
		switch(dm_manual_debug_bg_no)
		{
		case	0:
			out_flg = dm_manual_1_main();
			break;
		case	1:
			out_flg = dm_manual_2_main();
			break;
		case	2:
			out_flg = dm_manual_3_main();
			break;
		case	3:
			out_flg = dm_manual_4_main();
			break;
		case	4:
			out_flg = dm_manual_5_main();
			break;
		case	5:
			out_flg = dm_manual_6_main();
			break;
		}

		dm_play_se();//	ＳＥ再生

		//	音楽再生中
		if( manual_bgm_flg ){
			manual_bgm_timer++;
			if( manual_bgm_timer >= manual_bgm_timer_max){
				//	再生時間終了
				manual_bgm_timer = 0;
				manual_bgm_flg = 0;
				auSeqPlayerStop(0);	//	ＢＧＭ停止
			}
		}

		dm_manual_debug_timer ++;
		if( dm_manual_debug_timer > 20000)
			dm_manual_debug_timer = 0;

		//	内部３０フレーム制御用
		sec_30_flg ^= 1;

#ifdef	M_DEBUG

		if( joyupd[0] & A_BUTTON ){
			flg_f = 1;
			break;
		}else	if( joyupd[0] & START_BUTTON ){
			dm_manual_all_init();
		}else	if( joyupd[0] & R_CBUTTONS ){
			evs_manual_no++;
			if(	evs_manual_no > 5){
				evs_manual_no = 0;
			}
			flg_f = 0;
			break;
		}else	if( joyupd[0] & L_CBUTTONS ){
			evs_manual_no--;
			if(	evs_manual_no < 0){
				evs_manual_no = 5;
			}
			flg_f = 0;
			break;
		}
#else
		for(i = 0;i < 4;i++){
			if( joyupd[i] & DM_ANY_KEY ){
				out_flg = 0;
				flg_f = 1;
				break;
			}
		}
		if( out_flg == 0 )
			break;
#endif


	}

	//	音楽停止
	auSeqPlayerStop(0);

	//	表示停止
	graphic_no = GFX_NULL;

	//	グラフィック・オーディオタスク終了待ち
	while((auSeqPlayerState(0) != AL_STOPPED) || (pendingGFX != 0)) {
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	//	クライアントの抹消
	nnScRemoveClient(sched, &client);

#ifdef	M_DEBUG

	if(flg_f){
		if(main_old == MAIN_TITLE);
			return	MAIN_TITLE;	//	タイトル
	}else{
		return	MAIN_MANUAL;	//	モードセレクトへ
	}
#else
	if(main_old == MAIN_TITLE){
		return	MAIN_TITLE;			//	タイトル
	}else	if( main_old == MAIN_MODE_SELECT ){
		main_old = MAIN_MANUAL;
		return	MAIN_MODE_SELECT;	//	モードセレクト
	}
#endif

}

/*--------------------------------------
	操作説明描画メイン関数
--------------------------------------*/
void	dm_manual_graphic(void)
{
	NNScTask*	gt;
	int			i,j,k,p;

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧ･ﾀｽｸﾊﾞｯﾌｧの指定
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	// RSP 初期設定


	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_1]));	//	アイテム（カプセル等）のセグメント設定
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_2]));	//	2Pキャラクタのセグメント設定
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	// フレームバッファのクリア
	gSPDisplayList(gp++, ClearCfb );

	//	描画範囲の指定
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	半透明設定
	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	//	透明度の設定
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

	//	デバック用タイマー表示

	//	背景描画
	gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	load_TexPal( (u16 * )&BGBuffer[1] );
	for(i = 0;i < 40;i++)
	{
		load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
		draw_Tex(0,(i * 6),320,6,0,0);
	}


	//	特殊背景描画
	switch( dm_manual_debug_bg_no )
	{
	case	0:
	case	3:
		//	巨大ウイルス
		dm_anime_draw( &dm_manual_big_virus_anime[2],dm_manual_big_virus_pos[2][0],dm_manual_big_virus_pos[2][1],0 );				//	巨大青ウイルス描画
		dm_anime_draw( &dm_manual_big_virus_anime[0],dm_manual_big_virus_pos[0][0],dm_manual_big_virus_pos[0][1],0 );				//	巨大赤ウイルス描画
		dm_anime_draw( &dm_manual_big_virus_anime[1],dm_manual_big_virus_pos[1][0],dm_manual_big_virus_pos[1][1],0 );				//	巨大黄ウイルス描画
		break;
	case	2:
	case	5:
		//	マスク
		for(i = 0;i < 6;i++)
		{
			load_TexTile_8b(&dm_bg_manual_4p_bm0_0[282 * 6 * i],282,6,0,0,281,5);
			draw_Tex(18,192 + i * 6,282,6,0,0);
		}
		load_TexTile_8b(&dm_bg_manual_4p_bm0_0[282 * 6 * 6],282,3,0,0,281,2);
		draw_Tex(18,228,282,3,0,0);
		break;
	}


	//	瓶の中の情報描画
	switch( dm_manual_debug_bg_no )
	{
	case	0:	//	操作説明その１
		dm_game_graphic_p( &game_state_data[0],game_map_data[0] );	//	カプセル描画
		dm_game_graphic_effect( &game_state_data[0],0,0 );			//	演出描画
		dm_manual_1_graphic();										//	操作説明その１追加描画
		disp_cont();												//	○描画
		break;
	case	3:	//	操作説明その４
		dm_game_graphic_p( &game_state_data[0],game_map_data[0] );	//	カプセル描画
		dm_manual_4_graphic( );										//	赤点滅
		dm_game_graphic_effect( &game_state_data[0],0,0 );			//	演出描画
		disp_cont();												//	○描画
		break;
	case	1:	//	操作説明その２
	case	4:	//	操作説明その５
		for( i = 0;i < 2;i++ ){
			dm_game_graphic_p( &game_state_data[i],game_map_data[i] );				//	カプセル描画
			dm_manual_at_cap_draw( &game_state_data[i],dm_manual_at_cap[i],0);		//	攻撃カプセル描画
			dm_manual_atarrow_draw( &game_state_data[i],dm_manual_at_cap[i],0 );	//	攻撃カプセル用矢印描画
			dm_game_graphic_effect( &game_state_data[i],i,0);						//	演出描画
		}
		dm_manual_4_graphic( );	//	赤点滅
		break;
	case	2:	//	操作説明その３
	case	5:	//	操作説明その６
		for( i = 0;i < 4;i++ ){
			dm_game_graphic_p( &game_state_data[i],game_map_data[i] );				//	カプセル描画
			dm_manual_at_cap_draw( &game_state_data[i],dm_manual_at_cap[i],1);		//	攻撃カプセル描画

			switch( dm_manual_debug_bg_no ){
			case	2:	//	操作説明その３
				if( i == 0 ){
					dm_manual_3_graphic( );											//	操作説明その３追加描画
				}
				break;
			case	5:	//	操作説明その６
				dm_manual_6_graphic( &game_state_data[i],dm_manual_at_cap[i],i );	//	操作説明その６追加描画
				break;
			}
			dm_manual_atarrow_draw( &game_state_data[i],dm_manual_at_cap[i],1 );	//	攻撃カプセル用矢印描画
		}
		for( i = 0;i < 4;i++ ){
			dm_game_graphic_effect( &game_state_data[i],i,1 );						//	演出描画
		}


		break;
	}

	//	キノピオ描画
	disp_kinopio(manual_pino_pos,137,manual_pino_flip_flg);

	//	PUSH ANY KEY描画
	switch( dm_manual_debug_bg_no )
	{
	case	0:
	case	3:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY
		break;
	case	1:
	case	4:
		push_any_key_draw( 128,210 );	// PUSH ANY KEY
		break;
	case	2:
	case	5:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY

		break;
	}


	if( manual_str_win_flg ){
		//	メッセージウインド背景の描画
		gSPDisplayList(gp++, prev_halftrans_Ab_dl );

		//	フォントパレットデータの読込み
		load_TexPal(dm_manual_mes_win_bm0_0tlut);

		//	メッセージ背景の読込み
		for(i = 0;i < 4;i++){
			load_TexTile_4b(&dm_manual_mes_win_bm0_0[i * 1160],116,20,0,0,115,19);
			draw_Tex(manual_str_win_pos[0],manual_str_win_pos[1] + i * 20,116,20,0,0);
		}

		//	文章データの描画
		load_TexPal( dm_manual_font_pal[0] );//デフォルトで呼んでおく
		manual_str_end_flg = dm_draw_manual_str(manual_str_win_pos[0] + 4,manual_str_win_pos[1] + 4,manual_str_buffer,manual_str_len);
	}

	/*------------------*/
	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了, ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	gDPFullSync(gp++);			// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ最終処理
	gSPEndDisplayList(gp++);	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ終端
	osWritebackDCacheAll();		// dinamicｾｸﾞﾒﾝﾄのﾌﾗｯｼｭ

	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);

}
