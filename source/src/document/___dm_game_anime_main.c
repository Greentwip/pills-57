/*--------------------------------------
	filename	:	dm_game_anime_main.c

	create		:	1999/08/11
	modify		:	1999/12/02

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

/*--------------------------------------
	アニメーション用アドレス設定関数
--------------------------------------*/
u32	dm_anime_address_set( u32 address_bss,u32 address )
{
	return	(u32)(address_bss + address - (u32)GS1_SEGMENT * 0x01000000);
}

/*--------------------------------------
	アニメーションキャラクター設定関数
--------------------------------------*/
void	dm_anime_char_set( game_anime *ani ,game_anime_data **ani_dat)
{
	//	アニメデータの実アドレス設定
	ani -> anime_charcter = (game_anime_data **)dm_anime_address_set( ani -> cnt_anime_address,(u32)ani_dat );
}

/*--------------------------------------
	アニメーション強制設定関数
--------------------------------------*/
void	dm_anime_set( game_anime *ani ,u8 ani_type)
{
	ani -> cnt_anime_flg = cap_flg_off;	//	アニメーション割込み禁止
	ani	-> cnt_now_anime = ani_type;	//	アニメーション番号
	ani -> cnt_now_frame = 0;			//	アニメーションフレームを最初に
	ani -> cnt_anime_count = 0;			//	アニメーションカウントクリア
	ani -> cnt_anime_loop_count = 0;	//	アニメーションループカウントのクリア
	ani -> cnt_now_type = (game_anime_data *)dm_anime_address_set( ani -> cnt_anime_address,(u32)ani -> anime_charcter[ani_type] );	//	アニメーションのセット
}

/*--------------------------------------
	アニメーション設定関数
--------------------------------------*/
void	dm_anime_set_cnt( game_anime *ani ,u8 ani_type)
{
	if( ani -> cnt_anime_flg ){	//	アニメーション割込み可能
		dm_anime_set( ani,ani_type );	//	アニメーション設定
	}

}
/*--------------------------------------
	アニメーション再稼動関数(最初からアニメーションさせる)
--------------------------------------*/
void	dm_anime_restart( game_anime *ani )
{
	ani -> cnt_now_frame = 0;			//	アニメーションフレームを最初に
	ani -> cnt_anime_count = 0;			//	アニメーションカウントクリア
	ani -> cnt_anime_loop_count = 0;	//	アニメーションループカウントのクリア
}

/*--------------------------------------
	アニメーション制御関数
--------------------------------------*/
void	dm_anime_control( game_anime *ani )
{

	ani -> cnt_anime_count ++;
	if( ani -> cnt_anime_count > ( (ani -> cnt_now_type + ani -> cnt_now_frame) -> aniem_wait << 1) ){	//	アニメーション間隔を取った
		ani -> cnt_anime_count = 0;	//	カウント初期化
		switch(  (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg )
		{
		case	anime_next:	//	１コマ進める
			ani -> cnt_now_frame ++;
			break;
		case	anime_loop:	//	ループ(有限)
			ani -> cnt_anime_loop_count++;
			ani -> cnt_now_frame ++;
			if( ani -> cnt_anime_loop_count >= (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_wait){
				//	ループ終了
				dm_anime_set( ani,ANIME_nomal );		//	通常アニメーションの設定
				ani -> cnt_anime_flg = cap_flg_on;	//	アニメーション割込み許可
			}else{
				ani -> cnt_now_frame = (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg;	//	ループするコマのセット
			}
			break;
		case	anime_loop_infinity:	//	ループ(無限)
			ani -> cnt_now_frame ++;
			ani -> cnt_now_frame = (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg;	//	ループするコマのセット
			ani -> cnt_anime_flg = cap_flg_on;	//	アニメーション割込み許可
			break;
		case	anime_stop:	//	何もしない
			ani -> cnt_anime_flg = cap_flg_on;	//	アニメーション割込み許可
			break;
		case	anime_no_write://	何もしない
			break;
		case	anime_end:	//	通常状態に戻る
			dm_anime_set( ani,ANIME_nomal );		//	通常アニメーションの設定
			ani -> cnt_anime_flg = cap_flg_on;	//	アニメーション割込み許可
			break;
		}
	}
}

/*--------------------------------------
	アニメーション描画関数
--------------------------------------*/
void	dm_anime_draw( game_anime *ani,s16 x_pos,s16 y_pos,s8 flip_flg )
{
	s16	i,j,pos;
	game_anime_data *dat;
	u8	*address;
	u8	h_size[3];
	s8	vec = 1;

	dat = ani -> cnt_now_type + ani -> cnt_now_frame;

	if( (ani -> cnt_now_type + ani -> cnt_now_frame) ->  aniem_flg != anime_no_write){	//	描画禁止状態でなかった場合
		//	2P側
		if( flip_flg )
			vec = -1;

		//	サイズの移行
		h_size[0] = 0;
		h_size[1] = dat -> h_size[0];
		h_size[2] = dat -> h_size[0] + dat -> h_size[1];

		load_TexPal( (u16 *)dm_anime_address_set( ani -> cnt_anime_address,(u32)dat -> anime_pal ) );	//	パレットロード

		pos = x_pos - (dat -> def_w_size * flip_flg );
		pos += (dat -> pos_an_x * vec);
		address = (u8 *)dm_anime_address_set( ani -> cnt_anime_address,(u32)dat -> anime_dat );	//	グラフィックデータのアドレス
		for( i = j = 0;i < 3;i++ ){	//	キャラクター描画
			//	テクスチャロード
			load_TexTile_8b( address + dat -> def_w_size * h_size[i],dat -> def_w_size,dat -> h_size[i],0,0,dat -> def_w_size - 1,dat -> h_size[i] - 1 );
			//	描画
			draw_TexFlip(pos,y_pos + dat -> pos_an_y + j,dat -> def_w_size,dat -> h_size[i],0,0,flip_flg,flip_off );
			j += dat -> h_size[i];
		}
	}
}

