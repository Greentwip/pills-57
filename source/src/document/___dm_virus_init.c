/*--------------------------------------
	filename	:	dm_virus_init.c

	create		:	1999/08/03
	modify		:	1999/12/09

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


u8	limit_table[] =		// ﾚﾍﾞﾙによるvirus積み込み上限ﾃｰﾌﾞﾙ, max = LEVLIMIT
	{ 6,6,6,6,6, 6,6,6,6,6, 6,6,6,6,6, 5,5,4,4,3, 3,3,3,3,3 };


/*--------------------------------------
	マップ情報クリア関数
--------------------------------------*/
void	clear_map_all( game_map *map )
{
	u8	i,j;

	for(i = 0;i < 128;i ++){
		//	各フラグの消去
		(map + i) -> capsel_m_g = (map + i) -> capsel_m_p = 0;
		for(j = 0;j < 6;j++)
			(map + i) -> capsel_m_flg[j] = 0;
	}
}
/*--------------------------------------
	マップ情報一部クリア関数
--------------------------------------*/
void	clear_map( game_map *map,s8 x_pos,s8 y_pos )
{
	u8	i,j;

	i = ((y_pos - 1) << 3) + x_pos;	//	消去する場所の計算
	(map + i) -> capsel_m_g = (map + i) -> capsel_m_p = 0;
	//	各フラグの消去
	for(j = 0;j < 6;j++){
		(map + i) -> capsel_m_flg[j] = 0;
	}
}

/*--------------------------------------
	マップ情報取得関数
--------------------------------------*/
s8	get_map_info( game_map *map,s8 x_pos,s8 y_pos )
{
	u8	i;

	i = ((y_pos - 1) << 3) + x_pos;	//	調査する場所の計算
	if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
		//	表示フラグが立っていた
		return	cap_flg_on;
	}else{
		//	表示フラグが立っていない
		return	cap_flg_off;
	}
}

/*--------------------------------------
	ウイルス情報取得関数
--------------------------------------*/
u8	get_virus_count( game_map *map )
{
	u8	i,ret;

	for(i = ret = 0;i < 128;i++)
	{
		if( (map + i) -> capsel_m_flg[cap_disp_flg] )
		{
			//	表示フラグが立っていた
			if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){
				//	消滅状態ではなかった
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 )
				{
					//	ウイルスの場合
					ret ++;
				}
			}
		}
	}
	return	ret;
}

/*--------------------------------------
	ウイルス情報取得関数
--------------------------------------*/
u8	get_virus_color_count( game_map *map ,u8 *red,u8 *yellow,u8 *blue )
{
	u8	i,ret;
	u8	color[3];

	for( i = 0;i < 3;i++ ){
		color[i] = 0;
	}

	for(i = ret = 0;i < 128;i++)
	{
		if( (map + i) -> capsel_m_flg[cap_disp_flg] )
		{
			//	表示フラグが立っていた
			if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){
				//	消滅状態ではなかった
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 )
				{
					//	ウイルスの場合
					color[(map + i) -> capsel_m_flg[cap_col_flg]]++;
					ret ++;
				}
			}
		}
	}
	//	それぞれの色のウイルス数の設定
	*red	= color[0];	//	赤
	*yellow = color[1];	//	黄
	*blue 	= color[2];	//	青

	return	ret;
}

/*--------------------------------------
	マップ情報設定関数
--------------------------------------*/
void	set_map( game_map *map,s8 x_pos,s8 y_pos,s8 item,s8 pal )
{
	u8	i,pal_save;

	pal_save = pal;
	if( pal_save > 2 ){				//	暗い色だった場合
		pal_save -= 3;
	}

	i = ((y_pos - 1) << 3) + x_pos;	//	設定する場所の計算

	(map + i) -> capsel_m_g = item;							//	設定物
	(map + i) -> capsel_m_p = pal;							//	表示色
	(map + i) -> capsel_m_flg[cap_disp_flg] = 1;			//	表示フラグを立てる
	(map + i) -> capsel_m_flg[cap_down_flg] = 0;			//	落下フラグを消す
	(map + i) -> capsel_m_flg[cap_condition_flg] = 0;		//	状態を通常に
	(map + i) -> capsel_m_flg[cap_virus_flg] = -1;			//	ウイルス以外
	(map + i) -> capsel_m_flg[cap_col_flg] = pal_save;		//	内部計算色
}

/*--------------------------------------
	ウイルスマップ情報設定関数
--------------------------------------*/
void	set_virus( game_map *map,s8 x_pos,s8 y_pos,u8 virus,u8 virus_anime_no )
{
	u8	i,pal_save;

	pal_save = virus;
	if( pal_save > 2 ){				//	暗い色だった場合
		pal_save -= 3;
	}

	i = ((y_pos - 1)<< 3) + x_pos;

	(map + i) -> capsel_m_g = virus_anime_no;				//	ウイルスアニメ番号
	(map + i) -> capsel_m_p = virus;						//	色
	(map + i) -> capsel_m_flg[cap_disp_flg] = 1;			//	表示フラグを立てる
	(map + i) -> capsel_m_flg[cap_down_flg] = 0;			//	落下フラグを消す
	(map + i) -> capsel_m_flg[cap_condition_flg] = 0;		//	状態を通常に
	(map + i) -> capsel_m_flg[cap_virus_flg] = virus;		//	ウイルス番号をセットする
	(map + i) -> capsel_m_flg[cap_col_flg] = pal_save;		//	内部計算色
}

/*--------------------------------------
	ウイルスマップ情報作成用補助関数
--------------------------------------*/
u16	y_point_set(void)
{
	u16		rnd_weight;
	u16		rnd_cnt;
	u16		idx;

	idx = dm_up_weight + dm_middle_weight + dm_low_weight;
	rnd_weight = genrand( idx );
	if ( rnd_weight < dm_up_weight ) {
			rnd_cnt = genrand(STD_MFieldY / 3);
	} else {
		if ( rnd_weight < ( dm_up_weight + dm_middle_weight )) {
			rnd_cnt = ( genrand(STD_MFieldY/3)+ STD_MFieldY/3 );
		} else {
			rnd_cnt = ( genrand((STD_MFieldY/3+STD_MFieldY%3))+STD_MFieldY/3*2 );
		}
	}

	return ( rnd_cnt );
}
/*--------------------------------------
	ウイルスマップ情報作成用補助関数(ウイルス存在確認)
--------------------------------------*/
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

/*--------------------------------------
	ウイルスマップ情報作成用補助関数(上下左右、同じ色３つ以上並び判定)
--------------------------------------*/
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

/*--------------------------------------
	ウイルスマップ情報作成用補助関数(上下左右、同じ色３つ以飛び色判定)
--------------------------------------*/
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
/*--------------------------------------
	ウイルスマップ情報複製(コピー)関数
--------------------------------------*/
void	dm_virus_map_copy( virus_map *v_map_a,virus_map *v_map_b,u8 *order_a,u8*order_b )
{
	s16	i;

	for(i = 0;i < 128;i++)
	{
		//	コピー先データの初期化
		(v_map_b + i) -> virus_type = -1;
		(v_map_b + i) -> x_pos = (v_map_b + i) -> y_pos = 0;

		//	コピー作業
		(v_map_b + i) -> virus_type = (v_map_a + i) -> virus_type;
		(v_map_b + i) -> x_pos = (v_map_a + i) -> x_pos;
		(v_map_b + i) -> y_pos = (v_map_a + i) -> y_pos;
	}

	//	描画順のコピー
	for(i = 0;i < 96;i++){
		*(order_b + i) = *(order_a + i);
	}
}

/*--------------------------------------
	ウイルスマップ情報作成関数
--------------------------------------*/
void	dm_virus_init(game_state *state,virus_map *v_map,u8 *order)
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

	//	ウイルスマップ情報の初期化
	for(i = 0;i < 128;i++)
	{
		(v_map + i) -> virus_type = -1;
		(v_map + i) -> x_pos = (v_map + i) -> y_pos = 0;
	}
	//	表示順情報のクリア
	for(i = 0;i < 96;i++){
		*(order + i) = 0xff;
	}

	virus_work = state -> virus_level;	//	ウイルスレベルのセット
	if( virus_work > LEVLIMIT ){	//	上限値の判定
		virus_work = LEVLIMIT;
	}


	//	初期ウイルス数
	virus_max = ( virus_work + 1 ) << 2;


	// ﾚﾍﾞﾙによるvirus不在行数算出
	limit_line = limit_table[virus_work];

	//	色別ウイルス数の計算
	virus_work = virus_max / 3;
	for(i = 0;i < 4;i++){
		virus_color[i] = virus_work;
	}
	for(i = 0;i < (virus_max % 3);i++){
		virus_color[i] ++;
	}


	for( virus_count = dead_count = col_p = 0;virus_count < virus_max;virus_count++ ){

		cont_count = dead_count = 0;
		do{
			//	色指定
			do{
				j = genrand(3);
			}while( virus_color[j] == 0 );
			color_set = j;

			if( cont_count++ == 2){
				dead_count ++;
				if( dead_count > 2 ){
					goto RESTART_POS;	//	無限ループ回避
				}
				//	座標指定( 総当たり版 )
				cont_count = 0;
				for( x_set = 0;x_set < 8;x_set ++ ){
					for(y_set = 16;y_set > limit_line;y_set --){
						set_flg = FALSE;
						if( !dm_virus_check(v_map,x_set,y_set) ){
							set_flg = dm_check_color(v_map,x_set,y_set,color_set);
							if( set_flg == TRUE){
								set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
								if( set_flg ){
									goto OUT_POS;
								}
							}
						}
					}
				}
OUT_POS:
			}else{
				//	座標指定
				do{
					x_set = genrand(8);
					do{
						y_set = genrand(17);
//						y_set = y_point_set();
					}while(y_set < limit_line + 1);
				}while(dm_virus_check(v_map,x_set,y_set));
				//	その色が置けるか
				set_flg = dm_check_color(v_map,x_set,y_set,color_set);
				if( set_flg == TRUE){
					set_flg = dm_check_color_2(v_map,x_set,y_set,color_set);
				}
			}
		}while(!set_flg);
		//	ウイルス設定

SET_POS:

		virus_color[j]--;
		j = ((y_set - 1)<< 3) + x_set;
		(v_map + j) -> virus_type = color_set;
		(v_map + j) -> x_pos = x_set;
		(v_map + j) -> y_pos = y_set;
		*(order + virus_count) = j;
	}

}
