/*--------------------------------------
	filename	:	dm_game_main.c

	create		:	1999/08/02
	modify		:	1999/12/02

	created		:	Hiroyuki Watanabe
--------------------------------------*/
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
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "static.h"
#include "sprite.h"
#include "aiset.h"

#include "dm_nab_include.h"
#include "o_segment.h"
#include "record.h"

#define		ANIME_RESTRT_TIME	600	//	5秒
#define		WINMAX	3	//	勝敗決定用の勝利数(通常は３)

#ifdef	DM_DEBUG_FLG

#include	"tex/nab/test/debug_font.bft"	//	デバック用フォントデータ


u16	debug_font_def_pal[100][2];
u16	debug_font_def_pal_p;

s16	num_keta[4] = {4,4,4,4};
s16	param_pos[4] = {0,0,0,0};

u8	keyRept[4] = {0,0,0,0};

#include	"aidebug.h"

/*--------------------------------------
	デバック用関数群
--------------------------------------*/
/*--------------------------------------
	フォント読込み関数(デバック用の関数を利用する場合、１回呼ぶ)
--------------------------------------*/
void	disp_font_load(void)
{
	//	フォント読み鋳込み
	load_TexBlock_4b(debug_font_bm0_0,96,30);

	//	読込みパレット番号の初期化
	debug_font_def_pal_p = 0;
}

/*--------------------------------------
	１文字描画関数
--------------------------------------*/
void	disp_debug_font(s16	x_pos,s16 y_pos,s8 font)
{
	s8	ascii,x,y;
	//	パレットはすでに呼ばれているものとする

	ascii = font - 0x20;
	x = ascii % 16;
	y = ascii >> 4;

	//	フォント書き込み
	draw_Tex( x_pos,y_pos,6,5,x * 6,y * 5);
}
/*--------------------------------------
	文字列描画関数
--------------------------------------*/
void	disp_debug_string(STRTBL *str)
{
	s8	i,*str_dat;
	s16	x_pos,y_pos;

	x_pos = str -> x_pos;
	y_pos = str -> y_pos;
	str_dat = str -> str;

	debug_font_def_pal[debug_font_def_pal_p][0] = str -> color;	//	描画色の設定
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	while(1){
		if( *str_dat == '\0' ){
			break;
		}
		switch( *str_dat ){
		case	' ':	//	スペース
			break;
		default:	//	描画
			disp_debug_font( x_pos,y_pos,*str_dat );
		}
		x_pos += 6;
		str_dat ++;
	}
}
/*--------------------------------------
	数字描画関数
--------------------------------------*/
void	disp_debug_num(NUMTBL *num)
{
	s8	i,start,zero_flg,ascii[5];
	s16	x_pos,y_pos,num_dat;

	x_pos = num -> x_pos;
	y_pos = num -> y_pos;
	num_dat = num -> num;



	debug_font_def_pal[debug_font_def_pal_p][0] = num -> color;	//	描画色の設定
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	if( num_dat < 0 ){	//	マイナス
		num_dat *= -1;
		disp_debug_font( x_pos,y_pos,0x2d );
		x_pos += 6;
	}
	ascii[0] = num_dat / 10000;							//	万
	ascii[1] = (num_dat % 10000) / 1000;				//	千
	ascii[2] = ((num_dat % 10000) % 1000) / 100;		//	百
	ascii[3] = (((num_dat % 10000) % 1000) % 100) / 10;	//	十
	ascii[4] = (((num_dat % 10000) % 1000) % 100) % 10;	//	一

	zero_flg = 0;
	start = 5 - (num -> flg & 0x0f);
	for(i =  start;i < 5;i++){
		if( ascii[i] == 0 ){	//	０のとき
			if( !zero_flg ){
				if( num -> flg & NUM_ZERO ){	//	０埋めフラグが立っていたら
					disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
					x_pos += 6;
				}else{
					if( num -> flg & NUM_RIGHT ){	//	右寄せフラグが立っていたら
						x_pos += 6;
					}
				}
			}else{
				disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
				x_pos += 6;
			}
		}else{
			disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
			zero_flg = 1;
			x_pos += 6;
		}
	}
}
/*--------------------------------------
	数字描画関数
--------------------------------------*/
void	disp_debug_param(u8 player_no,PARAMTBL *param, u8 ctrlno)
{
	s8	i,zero_flg,ascii[5];
	s16	x_pos,y_pos,num_dat;

	x_pos = param -> x_pos;
	y_pos = param -> y_pos;
	num_dat = *(param -> data);

	debug_font_def_pal[debug_font_def_pal_p][0] = param -> color;	//	描画色の設定
	debug_font_def_pal[debug_font_def_pal_p][1] = 0x0000;
	load_TexPal(debug_font_def_pal[debug_font_def_pal_p]);
	debug_font_def_pal_p++;
	if( debug_font_def_pal_p > 99 )
		debug_font_def_pal_p = 99;

	if( num_dat < 0 ){	//	マイナス
		num_dat *= -1;
		disp_debug_font( x_pos,y_pos,0x2d );
	}
	x_pos += 6;
	ascii[0] = num_dat / 10000;							//	万
	ascii[1] = (num_dat % 10000) / 1000;				//	千
	ascii[2] = (num_dat % 1000) / 100;					//	百
	ascii[3] = (num_dat % 100) / 10;					//	十
	ascii[4] = num_dat % 10;							//	一

	zero_flg = 0;
	for(i = 0;i < 5;i++){
		if( param_pos[player_no] == ctrlno && num_keta[player_no] == i ){	//	操作中の桁の場合
			disp_debug_font( x_pos,y_pos + 6,0x5e );

		}
		disp_debug_font( x_pos,y_pos,ascii[i] + 0x30 );
		x_pos += 6;
	}
}

/*--------------------------------------
	パラメータ操作関数
--------------------------------------*/
void	cnt_debug_param( s8 player_no,PARAMTBL *param )
{
	s16	num[] = { 10000,1000,100,10,1 };

	if( joyupd[player_no] & LEVER_LEFT ){ //	桁上げ
		if( num_keta[player_no] > 0 ){
			num_keta[player_no] --;
		}
	}else	if( joyupd[player_no] & LEVER_RIGHT ){	//	桁下げ
		if( num_keta[player_no] < 4 ){
			num_keta[player_no] ++ ;
		}
	}else	if( joyupd[player_no] & LEVER_UP ){ //	数値上げ
		if( *(param -> data) + num[num_keta[player_no]] <= param -> max ){
			*(param -> data) += num[num_keta[player_no]];
		}
	}else	if( joyupd[player_no] & LEVER_DOWN ){ //	数値下げ
		if( *(param -> data) - num[num_keta[player_no]] >= param -> min ){
			*(param -> data) -= num[num_keta[player_no]];
		}
	}
}

/*--------------------------------------
	デバック描画関数
--------------------------------------*/
void	cnt_debug_main(s8 player_no)
{
	cnt_debug_param(0,&dai_param_table[ param_pos[player_no] ]);
	if( joyold[player_no] & L_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] > 0 ) {
				param_pos[player_no] --;
			}
		}
		keyRept[player_no]++;
	} else if( joyold[player_no] & R_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] < 39 - 1 ) {
				param_pos[player_no] ++;
			}
		}
		keyRept[player_no]++;
	} else {
		keyRept[player_no] = 0;
	}

	if( joyold[player_no] & L_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] > 0 ) {
				param_pos[player_no] --;
			}
		}
		keyRept[player_no]++;
	} else if( joyold[player_no] & R_TRIG ){
		if ( keyRept[player_no] == 0 || keyRept[player_no] >= 30*evs_gamespeed ) {
			if ( keyRept[player_no] >= 30*evs_gamespeed ) keyRept[player_no] = 25*evs_gamespeed;
			if( param_pos[player_no] < 39 - 1 ) {
				param_pos[player_no] ++;
			}
		}
		keyRept[player_no]++;
	} else {
		keyRept[player_no] = 0;
	}
	dai_param_table[2].data = &(ai_param[daiLevel][daiMode].dt_LnOnLinVrs);
	dai_param_table[3].data = &(ai_param[daiLevel][daiMode].dt_LnEraseVrs);
	dai_param_table[4].data = &(ai_param[daiLevel][daiMode].dt_EraseLinP1);
	dai_param_table[5].data = &(ai_param[daiLevel][daiMode].dt_HeiEraseLinRate);
	dai_param_table[6].data = &(ai_param[daiLevel][daiMode].dt_WidEraseLinRate);
	dai_param_table[7].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_FallCap);
	dai_param_table[8].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_FallVrs);
	dai_param_table[9].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_Capsule);
	dai_param_table[10].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_Virus);
	dai_param_table[11].data = &(ai_param[daiLevel][daiMode].dt_P_ALN_HeightP);
	dai_param_table[12].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Bottom);
	dai_param_table[13].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_FallCap);
	dai_param_table[14].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_FallVrs);
	dai_param_table[15].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Capsule);
	dai_param_table[16].data = &(ai_param[daiLevel][daiMode].dt_WP_ALN_Virus);
	dai_param_table[17].data = &(ai_param[daiLevel][daiMode].dt_OnVirusP);
	dai_param_table[18].data = &(ai_param[daiLevel][daiMode].dt_RensaP);
	dai_param_table[19].data = &(ai_param[daiLevel][daiMode].dt_RensaMP);
	dai_param_table[20].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp2);
	dai_param_table[21].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp3);
	dai_param_table[22].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp4);
	dai_param_table[23].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp5);
	dai_param_table[24].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp6);
	dai_param_table[25].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp7);
	dai_param_table[26].data = &(ai_param[daiLevel][daiMode].dt_HeiLinesAllp8);
	dai_param_table[27].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp2);
	dai_param_table[28].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp3);
	dai_param_table[29].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp4);
	dai_param_table[30].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp5);
	dai_param_table[31].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp6);
	dai_param_table[32].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp7);
	dai_param_table[33].data = &(ai_param[daiLevel][daiMode].dt_WidLinesAllp8);
	dai_param_table[34].data = &(ai_param[daiLevel][daiMode].dt_LnHighCaps);
	dai_param_table[35].data = &(ai_param[daiLevel][daiMode].dt_LnHighVrs);
	dai_param_table[36].data = &(ai_param[daiLevel][daiMode].dt_aiWall_F);

}
/*--------------------------------------
	デバック描画関数
--------------------------------------*/
void	disp_debug_main_1p(void)
{
	int		i;

	disp_font_load();

	disp_debug_string( &daiLevelStr[daiLevel] );
	disp_debug_string( &daiModeStr[daiMode] );
	disp_debug_string( &daiDebugP1Str[debug_p1cpu] );

	for( i = 0; i < 26;i++) {
		disp_debug_string( &dai_str_table[i] );
	}

	for( i = 0; i < 39;i++) {
		disp_debug_param( 0,&dai_param_table[i],(u8)i );
	}
}

#endif

#ifdef NN_SC_PERF
static	s8	disp_meter_flg = 0;
#endif

//#include	"tex/nab/test_speed.bft"	//	デバック用フォントデータ

s8		testes_sound_no = 56;
s8		m_g;

#ifdef	DM_S2D_MAIN
		uObjSprite	s2d_dm_map_item[8 << 4];
#endif

static	uObjBg	s2d_dm_bg;						//	背景表示用変数
static	uObjBg	s2d_demo_4p;					//	ストーリー４Ｐ時に使用

static	game_anime	big_virus_anime[3];			//	巨大ウイルスアニメ制御用変数
static	game_anime	disp_anime_data[2][2];		//	アニメーションデータ表示用変数
static	coin_pos	dm_retry_coin_pos[3];		//	リトライ用のコインの変数
static	lv_score_pos	lv_sc_pos[10];			//	得点演出用変数
static	s32	seq_save;							//	再生音楽番号保存用変数
static	s16	virsu_num_size[4][3];				//	ウイルス数の大きさ
static	s16	big_virus_position[3][3];			//	巨大ウイルスの座標
static	u16	dm_rand_seed = 0;					//	ランダムの種
static	s16	dm_start_time;						//	第１投のタイマー
static	u16	dm_demo_time;						//	デモのタイマー
static	s16	dm_retry_alpha[2];					//	リトライの点滅用変数 0:カーソルのアルファ 1:カーソルの点滅用サイン値
static	s16	dm_vsmode_win_point[2];				//	2PLAY & VSCOM ポイント表示用変数
static	u16	*story_4p_demo_bg_tlut;				//	4P 時の 対戦前の顔表示剤座を選択用(パレットデータ)
static	s16	story_4p_demo_w[3];					//	ストーリーの 4P 時の 対戦前の顔表示縦幅
static	s16	story_4p_demo_h[3];					//	ストーリーの 4P 時の 対戦前の顔表示縦幅
static	s16	story_4p_demo_face_pos[5];			//	ストーリーの 4P 時の 対戦前の顔表示 Ｘ座標
static	s16	dm_anime_restart_count;				//	勝敗が付いた後アニメーションを再稼動させるためのカウンタ
static	s8	eep_rom_flg[2];						//	EEP 書き込み用フラグ
static	s8	dm_think_flg = 0;					//	思考開始フラグ
static	s8	dm_TouchDownWait;					//	ｶｰｿﾙｶﾌﾟｾﾙの下に何かあるときのｳｴｲﾄを加算
static	s8	dm_game_init_flg;					//	初期化を行ったときに、１ｲﾝﾄ 背景のみの描画を行うためのフラグ
static	s8	dm_peach_stage_flg;					//	ピーチステージを使用するフラグ
static	u8	dm_peach_get_flg[2];				//	ピーチ姫獲得の演出用
static	u8	dm_demo_time_flg;					//	デモのフラグ
static	s8	dm_retry_position[4][3];			//	リトライのカーソル位置 0:カーソルの上下位置 1:カーソルの左右位置 2:カーソルの左右移動用カウンタ
static	s8	dm_retry_pos[4];					//	黄色矢印に関する変数
static	u8	dm_retry_coin;						//	リトライ用のコインの枚数
static	u8	dm_retry_flg;						//	リトライ可能かどうか表示に反映させるためのフラグ
static	u8	win_count[4];						//	勝利数カウンタ
static	u8	vs_win_count[2];					//	勝利数カウンタ(３勝で１増える)
static	u8	vs_lose_count[2];					//	敗北数カウンタ(３負けで１増える)
static	u8	win_anime_count[2];					//	勝利数カウンタ
static	u8	dm_game_bg_no;						//	VSCOM 2PLAYのときの上下のタイル番号
static	u8	bgm_bpm_flg[2];						//	BGMの速さを変えるフラグ
static	u8	last_3_se_flg;						//	ウイルスが３個以下になったときにSEを鳴らすためのフラグ
static	s8	story_time_flg;						//	ストーリーのタイムカウントの開始フラグ
static	s8	big_virus_count[4];					//	ウイルス数の各色別の個数の変数
static	s8	big_virus_flg[3][2];				//	0:消滅アニメーションを一回だけ設定するフラグ 1:消滅音を一回だけ設定するフラグ
static	s8	story_4p_demo_flg[2];				//	ストーリーの 4P 時の 対戦前の顔表示
static	s8	dm_game_4p_team_flg;				//	チーム対抗戦のフラグ
static	u8	dm_game_4p_team_bit;				//	チーム対抗戦の判定用フラグ
static	u8	dm_game_4p_team_bits[2];			//	チーム対抗戦の判定用フラグ
static	u8	*story_4p_demo_bg;					//	4P 時の 対戦前の顔表示台座を選択用(グラフィックデータ)
static	s8	story_4p_name_flg[2];				//	ストーリーの 4P 時の MARIO.ENEMY を左右のどちらかに表示するかのフラグ
static	s8	story_4p_name_num[4];				//	ストーリーの 4P 時の チームのメンバー数
static	s8	story_4p_stock_cap[4][4];			//	ストーリーの 4P 時の ストックカプセル用配列( 何も無いときは -1 が入る)

u16	*capsel_pal[] = {	//	カプセルとウイルスのパレット配列(大)
	TLUT_mitem10r,
	TLUT_mitem10y,
	TLUT_mitem10b,
	TLUT_mitem10rx,
	TLUT_mitem10yx,
	TLUT_mitem10bx,
};

u16	*capsel_8_pal[] = {	//	カプセルとウイルスのパレット配列(小)
	TLUT_mitem08r,
	TLUT_mitem08y,
	TLUT_mitem08b,
	TLUT_mitem08rx,
	TLUT_mitem08yx,
	TLUT_mitem08bx
};

u16	**cap_pal[] = {	//	カプセルのパレット
	capsel_pal,
	capsel_8_pal
};

u8	*cap_tex[] = {	//	カプセルのグラフィック
	TEX_mitem10r_0_0,
	TEX_mitem08r_0_0
};

//	ウイルスのアニメーションパターン番号
u8	virus_anime_table[][4] = {
	{virus_a1,virus_a2,virus_a3,virus_a4},	//	赤用
	{virus_b1,virus_b2,virus_b3,virus_b4},	// 	黄用
	{virus_c1,virus_c2,virus_c3,virus_c4},	//	青用
};

//	2P時の攻撃ＳＥテーブル
s8	dm_chaine_se_table_vs[] = {
	dm_se_attack_2p,dm_se_attack_1p
};

//	4P時の攻撃ＳＥテーブル
s8	dm_chaine_se_table_4p[4][3] = {
	{dm_se_attack_4p,dm_se_attack_3p,dm_se_attack_2p},
	{dm_se_attack_1p,dm_se_attack_4p,dm_se_attack_3p},
	{dm_se_attack_2p,dm_se_attack_1p,dm_se_attack_4p},
	{dm_se_attack_3p,dm_se_attack_2p,dm_se_attack_1p},
};

//	アニメーションデータのロムアドレス
u8	*dm_anime_rom_address[][2] = {
	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	マリオ
	{_dm_anime_nokoSegmentRomStart,_dm_anime_nokoSegmentRomEnd},		//	ノコノコ
//	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	ノコノコ
	{_dm_anime_bomheiSegmentRomStart,_dm_anime_bomheiSegmentRomEnd},	//	ボム兵
	{_dm_anime_pukuSegmentRomStart,_dm_anime_pukuSegmentRomEnd},		//	プクプク
	{_dm_anime_choroSegmentRomStart,_dm_anime_choroSegmentRomEnd},		//	チョロブー
	{_dm_anime_heihoSegmentRomStart,_dm_anime_heihoSegmentRomEnd},		//	プロペラヘイホー
	{_dm_anime_hanaSegmentRomStart,_dm_anime_hanaSegmentRomEnd},		//	ハナチャン
//	{_dm_anime_marioSegmentRomStart,_dm_anime_marioSegmentRomEnd},		//	ハナチャン
	{_dm_anime_teresaSegmentRomStart,_dm_anime_teresaSegmentRomEnd},	//	テレサ
	{_dm_anime_pakuSegmentRomStart,_dm_anime_pakuSegmentRomEnd},		//	パックンフラワー
	{_dm_anime_kameSegmentRomStart,_dm_anime_kameSegmentRomEnd},		//	カメック
	{_dm_anime_kuppaSegmentRomStart,_dm_anime_kuppaSegmentRomEnd},		//	クッパ
	{_dm_anime_peachSegmentRomStart,_dm_anime_peachSegmentRomEnd},		//	ピーチ
	{_dm_anime_virusSegmentRomStart,_dm_anime_virusSegmentRomEnd},		//	ウイルス
};

//	背景データのロムアドレス
u8	*dm_bg_rom_address[] = {
	_bg_dat_01SegmentRomStart,
	_bg_dat_02SegmentRomStart,
	_bg_dat_03SegmentRomStart,
	_bg_dat_04SegmentRomStart,
	_bg_dat_05SegmentRomStart,
	_bg_dat_06SegmentRomStart,
	_bg_dat_07SegmentRomStart,
	_bg_dat_08SegmentRomStart,
	_bg_dat_09SegmentRomStart,
	_bg_dat_10SegmentRomStart
};

//	アニメーションデータのテーブル
game_anime_data	**game_anime_table[] = {
	game_anime_data_mario,		//	マリオ
	game_anime_data_noko,		//	ノコノコ
//	game_anime_data_mario,		//	ノコノコ
	game_anime_data_bomhei,		//	ボム兵
	game_anime_data_puku,		//	プクプク
	game_anime_data_choro,		//	チョロブー
	game_anime_data_heiho,		//	プロペラヘイホー
	game_anime_data_hana,		//	ハナチャン
//	game_anime_data_mario,		//	ノコノコ
	game_anime_data_teresa,		//	テレサ
	game_anime_data_paku,		//	パックンフラワー
	game_anime_data_kame,		//	カメック
	game_anime_data_kuppa,		//	クッパ
	game_anime_data_peach,		//	ピーチ
	game_anime_data_virus_r,	//	赤ウイルス
	game_anime_data_virus_y,	//	黄ウイルス
	game_anime_data_virus_b,	//	青ウイルス
};

//	4P用顔グラフィックデータ
u8	*face_4p_game_data[] = {
	face_4p_game_mario_bm0_0,
	face_4p_game_noko_bm0_0,
	face_4p_game_bomhei_bm0_0,
	face_4p_game_puku_bm0_0,
	face_4p_game_chor_bm0_0,
	face_4p_game_heiho_bm0_0,
	face_4p_game_hana_bm0_0,
	face_4p_game_tere_bm0_0,
	face_4p_game_pack_bm0_0,
	face_4p_game_kame_bm0_0,
	face_4p_game_kupp_bm0_0,
	face_4p_game_peach_bm0_0,
	face_4p_game_kinopio_bm0_0,
	face_4p_game_bomhei_2_bm0_0,
};

//	4P用顔パレットデータ
u16	*face_4p_game_pal_data[] = {
	face_4p_game_mario_bm0_0tlut,
	face_4p_game_noko_bm0_0tlut,
	face_4p_game_bomhei_bm0_0tlut,
	face_4p_game_puku_bm0_0tlut,
	face_4p_game_chor_bm0_0tlut,
	face_4p_game_heiho_bm0_0tlut,
	face_4p_game_hana_bm0_0tlut,
	face_4p_game_tere_bm0_0tlut,
	face_4p_game_pack_bm0_0tlut,
	face_4p_game_kame_bm0_0tlut,
	face_4p_game_kupp_bm0_0tlut,
	face_4p_game_peach_bm0_0tlut,
	face_4p_game_kinopio_bm0_0tlut,
	face_4p_game_bomhei_2_bm0_0tlut,
};

//	背景タイルグラフィックデータ
u8	*dm_bg_tile_data[] = {
	tile_01_bm0_0,
	tile_02_bm0_0,
	tile_03_bm0_0,
	tile_04_bm0_0,
	tile_05_bm0_0,
	tile_01_bm0_0,
	tile_07_bm0_0,
	tile_08_bm0_0,
	tile_08_bm0_0,
	tile_10_bm0_0,
};

//	背景タイルパレットデータ
u16	*dm_bg_tile_pal_data[] = {
	tile_01_bm0_0tlut,
	tile_02_bm0_0tlut,
	tile_03_bm0_0tlut,
	tile_04_bm0_0tlut,
	tile_05_bm0_0tlut,
	tile_01_bm0_0tlut,
	tile_07_bm0_0tlut,
	tile_08_bm0_0tlut,
	tile_08_bm0_0tlut,
	tile_10_bm0_0tlut,
};


#ifdef	DM_S2D_MAIN
/*--------------------------------------
	S2DEX 用スプライト設定関数
--------------------------------------*/
void	dm_set_sprite(uObjSprite *sp,s16 x_p,s16 y_p,s16 size_w,s16 size_h,u8 im_fmt,u8 im_siz,s16 wide,s16 t_x,s16 t_y,s8 pal_no )
{
	sp->s.objX = x_p << 2;
	sp->s.objY = y_p << 2;
	sp->s.imageW = size_w << 5;
	sp->s.imageH = size_h << 5;
	sp->s.scaleW = sp->s.scaleH = 1<<10;
	sp->s.paddingX = sp->s.paddingY = sp->s.imageFlags = 0;dm_4p_game_target_3p_bm0_0tlut
	sp->s.imageStride = GS_PIX2TMEM(wide,im_siz);
	sp->s.imageAdrs = GS_PIX2TMEM( ( t_y * wide ) + t_x,im_siz);
	sp->s.imageFmt = im_fmt;
	sp->s.imageSiz = im_siz;
	sp->s.imagePal = pal_no;
}
#endif
/*--------------------------------------
	得点関数
--------------------------------------*/
void	dm_score_make( game_state *state,u8 type )
{
	u8	i,sc_type;

	if( type ){
		sc_type = state -> player_state[1];
	}else{
		sc_type = state -> cap_def_speed;
	}

	for( i = 0;i < state -> erase_virus_count;i++ ){
		if( i < 6 ){
			state -> game_score += Score1p[sc_type][i] * 100;
		}else{
			state -> game_score += Score1p[sc_type][5] * 100;
		}
	}

	//	カンスト
	if( state -> game_score > 9999900 ){
		state -> game_score = 9999900;
	}
}

/*--------------------------------------
	LEVEL用得点設定関数
--------------------------------------*/
void	dm_level_score_make( game_state *state )
{
	s8	i,j,p;

	for( i = 0;i < 10;i++ ){
		if( !lv_sc_pos[i].flg ){	//	未使用の場合
			lv_sc_pos[i].mode = 1;
			lv_sc_pos[i].flg = 1;
			lv_sc_pos[i].score = 0;
			//	落下移動値のセット
			for(j = 0;j < 5;j++){
				lv_sc_pos[i].score_pos[j][1] = 12;
			}
			//	加算する得点の計算
			for(j = 0;j < state -> erase_virus_count;j++ ){
				if( j > 5 ){
					p = 5;
				}else{
					p = j;
				}
				lv_sc_pos[i].score += Score1p[state -> cap_def_speed][p] * 100;
			}
			//	得点別の設定
			if( lv_sc_pos[i].score < 1000 ){
				lv_sc_pos[i].num_max = 3;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 100;
				lv_sc_pos[i].num_g[1] = lv_sc_pos[i].num_g[2] = 0;
				for(j = 0;j < 3;j++){
					lv_sc_pos[i].score_pos[j][0] = 70 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}

			}else 	if( lv_sc_pos[i].score < 10000 ){
				lv_sc_pos[i].num_max = 4;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 1000;
				lv_sc_pos[i].num_g[1] = ( lv_sc_pos[i].score % 1000 ) / 100;
				lv_sc_pos[i].num_g[2] = lv_sc_pos[i].num_g[3] = 0;
				for(j = 0;j < 4;j++){
					lv_sc_pos[i].score_pos[j][0] = 62 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}
			}else{
				lv_sc_pos[i].num_max = 5;
				lv_sc_pos[i].num_g[0] = lv_sc_pos[i].score / 10000;
				lv_sc_pos[i].num_g[1] = ( lv_sc_pos[i].score % 10000 ) / 1000;
				lv_sc_pos[i].num_g[2] = ( lv_sc_pos[i].score % 1000 ) / 100;
				lv_sc_pos[i].num_g[3] = lv_sc_pos[i].num_g[4] = 0;
				for(j = 0;j < 5;j++){
					lv_sc_pos[i].score_pos[j][0] = 54 + (j << 3);
					lv_sc_pos[i].num_alpha[j] = 0;
					lv_sc_pos[i].num_alpha_flg[j] = 0;
				}
			}

			break;
		}
	}
}

/*--------------------------------------
	LEVEL用得点関数
--------------------------------------*/
void	dm_level_score_main( game_state *state )
{
	s8	i,j;

	for( i = 0;i < 10;i++ ){
		if( lv_sc_pos[i].flg ){	//	使用中の場合
			switch( lv_sc_pos[i].mode )
			{
			case	1:	//	アルファアップ
				for( j = 0;j < lv_sc_pos[i].num_max;j++ ){
					if( !lv_sc_pos[i].num_alpha_flg[j] ){
						lv_sc_pos[i].num_alpha[j] += 0x30;
						if( lv_sc_pos[i].num_alpha[j] >= 0xff ){
							lv_sc_pos[i].num_alpha[j] = 0xff;
							lv_sc_pos[i].num_alpha_flg[j] = 1;	//	その桁の透明処理終了
						}
						break;
					}
				}
				if( lv_sc_pos[i].num_alpha_flg[lv_sc_pos[i].num_max - 1] ){
					//	次の処理に移る
					lv_sc_pos[i].mode = 2;
				}
				break;
			case	2:	//	数字落下処理
				for( j = lv_sc_pos[i].num_max - 1;j >= 0;j-- ){
					if( lv_sc_pos[i].score_pos[j][1] ){
						if( j == lv_sc_pos[i].num_max - 1 ){

							lv_sc_pos[i].num_alpha[j] -= 0x20;
							lv_sc_pos[i].score_pos[j][1] -= 2;
							if( lv_sc_pos[i].score_pos[j][1] <= 0 ){
								lv_sc_pos[i].score_pos[j][1] = 0;
							}
						}else{
							if( lv_sc_pos[i].score_pos[j + 1][1] <= 4 ){
								lv_sc_pos[i].num_alpha[j] -= 0x20;
								lv_sc_pos[i].score_pos[j][1] -= 2;
								if( lv_sc_pos[i].score_pos[j][1] <= 0 ){
									lv_sc_pos[i].score_pos[j][1] = 0;
								}
							}
						}
					}
				}

				//	全ての数字が落下し終わったら得点の加算
				if(	lv_sc_pos[i].score_pos[0][1] == 0 )
				{
					lv_sc_pos[i].mode = 0;
					state -> game_score += lv_sc_pos[i].score;	//	得点加算
					//	カンスト
					if( state -> game_score > 9999900 ){
						state -> game_score = 9999900;
					}
					if( evs_high_score < state -> game_score ){
						evs_high_score = state -> game_score;	//	ハイスコアの更新
					}
					lv_sc_pos[i].flg = 0;						//	使用中フラグのOFF
				}
				break;
			}
		}
	}
}
/*--------------------------------------
	SE設定関数
--------------------------------------*/
void	dm_set_se( u16 senum ){
	if ( evs_gamespeed < 6 ){	//	高速処理用判定
		dm_set_menu_se(senum);
	}
}

/*--------------------------------------
	SE再生関数
--------------------------------------*/
void	dm_play_se( void )
{
	dm_menu_se_play();
}
/*--------------------------------------
	BGM 設定関数
--------------------------------------*/
void	dm_play_bgm_set( s8 bgm_no )
{
	evs_seSetPtr = evs_seGetPtr = 0;		//	SE 再生中止
	evs_seqnumb = bgm_no;					//	再生BGM設定
	auSeqPlayerStop(0);						//	BGM 停止
}

/*--------------------------------------
	攻撃音発生関数
--------------------------------------*/
void	dm_attack_se( game_state *state ,u8 player_no )
{
	u8	i,flg;

	if( state -> chain_line > 1 ){	//	連鎖数が２以上
		switch( evs_gamesel ){
		case	GSL_1PLAY:
			dm_set_se( dm_se_attack_1p );
			break;
		case	GSL_2PLAY:
		case	GSL_VSCPU:
			dm_set_se( dm_chaine_se_table_vs[player_no] );
			break;
		case	GSL_4PLAY:
			for(i = 0;i < 3;i++){
				if( state -> chain_color[3] & (1 << i) ){
					dm_set_se( dm_chaine_se_table_4p[player_no][i] );
				}
			}
			break;
		}
	}
}
/*--------------------------------------
	積み上げ警告音発生関数
--------------------------------------*/
void	dm_warning_h_line( game_state *state,game_map *map )
{
	u8	i,j,flg = 0;

	for( i = 1;i < 4;i++ ){	//	上の３段しか調べない
		for( j = 0;j < 8;j++ ){
			if( get_map_info( map,j,i) == cap_flg_on ){	//	上の３段に何か物があった
				flg = 1;
				break;
			}
		}
		if( flg ){
			break;
		}
	}

	if( flg ){
		if( ( state -> warning_flg & 0x80 ) == 0 ){
			//	鳴らしていないか再度積みあがった
			state -> warning_flg |= 0x80;	//	ビットを立てる( 連続鳴り防止 )
			dm_set_se( dm_se_warning );	//	警告 SE 再生
		}
	}else{
		state -> warning_flg &= 0x7f;	//	ビットを消す
	}
}

/*--------------------------------------
	落下情報設定関数
--------------------------------------*/
s8	set_down_flg( game_map *map )
{
	u8	k,l,flg[2];
	s8	ret;
	s16	i,j;

	for(i = 0;i < 120;i++){	//	落下フラグの設定
		(map + i) -> capsel_m_flg[cap_down_flg] = cap_flg_on;	//	落下フラグの設定
	}

	for( j = 14;j >= 0;j-- )	//	最下段の１つ上の段から始める
	{
		for( i = 0;i < 8;i++ )
		{
			k = (j << 3) + i;	//	配列番号の計算
			if( (map + k) -> capsel_m_flg[cap_disp_flg] )
			{
				//	表示されている
				if( (map + k) -> capsel_m_flg[cap_virus_flg] < 0)
				{
					//	ウイルスでない
					if( (map + k) -> capsel_m_g > capsel_d && (map + k) -> capsel_m_g < capsel_b)
					{
						//	横向きカプセルの場合
						if(  (map + k) -> capsel_m_g == capsel_l)
						{
							//	最下段ではない
							for(l = 0;l < 2;l++){
								flg[l] = cap_flg_on;
								if( get_map_info(map,(map + k+ l) -> pos_m_x,(map + k+ l) -> pos_m_y + 1) == cap_flg_on)
								{
									//	障害物があった場合
									if( (map + k + l + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
										//	その障害物は落下出来ない
										flg[l] = cap_flg_off;
									}
								}
							}

							if( !flg[0] || !flg[1] )
							{
								//	両方とも下に落ちることが出来ない
								for(l = 0;l < 2;l++)
								{
									(map + k + l) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	落下フラグを立消す
								}
							}
						}else	if( (map + k) -> capsel_m_g == capsel_r ){
							for(l = 0;l < 2;l++){
								flg[l] = cap_flg_on;
								if( get_map_info(map,(map + k - l) -> pos_m_x,(map + k - l) -> pos_m_y + 1) == cap_flg_on)
								{
									//	障害物があった場合
									if( (map + k - l + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
										//	その障害物は落下出来ない
										flg[l] = cap_flg_off;
									}
								}
							}
							if( !flg[0] || !flg[1] )
							{
								//	両方とも下に落ちることが出来ない
								for(l = 0;l < 2;l++)
								{
									(map + k - l) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	落下フラグを消す
								}
							}
						}
					}else{
						//	ボールの場合
						if( get_map_info(map,(map + k) -> pos_m_x, (map + k) -> pos_m_y + 1) == cap_flg_on)
						{
							//	物体の下に何かあった
							if( (map + k + 8) -> capsel_m_flg[cap_down_flg] != cap_flg_on ){
								(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	落下フラグの設定
							}
						}
					}
				}else{
					(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	落下フラグの設定
				}
			}else{
				(map + k) -> capsel_m_flg[cap_down_flg] = cap_flg_off;	//	落下フラグの設定
			}
		}
	}

	//	落下フラグの状況を返す
	for(i = ret = 0;i < 128;i++){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
			//	表示フラグが立っている
			if( (map + i) -> capsel_m_flg[cap_down_flg] ){
				//	落下フラグが立っている
				ret++;
			}
		}
	}

	return	ret;
}
/*--------------------------------------
	粒落下関数
--------------------------------------*/
void	go_down( game_state *state,game_map *map ,u8 cout)
{

	u8	k,se_flg = 0;
	s8	black_color[] = {0,3};
	s16	i,j,p;

	state -> erase_anime_count ++;
	if( state -> erase_anime_count >= cout ){	//	ウェイト時間経過
		state -> erase_anime_count = 0;

		for( j = 14;j >= 0;j-- ){	//	最下段の１つ上の段から始める
			for( i = 0;i < 8;i++ ){
				k = (j << 3) + i;	//	配列番号の計算
				if( (map + k) -> capsel_m_flg[cap_down_flg] ){
					//	落下フラグが立っている

					//	落下先に情報をコピー
					set_map( map ,(map + k) -> pos_m_x,(map + k) -> pos_m_y + 1,(map + k) -> capsel_m_g,(map + k) -> capsel_m_p );
					//	コピー元情報の削除
					clear_map(map,(map + k) -> pos_m_x, (map + k) -> pos_m_y);

					se_flg = 1;	//	落下音を鳴らす
				}
			}
		}

		if( se_flg ){
			dm_set_se( dm_se_cap_a_down );	//	落下ＳＥ再生
		}

		if( state -> retire_flg[dm_game_over_flg] ){
			//	リタイア状態
			if( state -> game_condition[2] == dm_cnd_training ){
				//	練習中
				for(i = 0;i < 128;i++){
					if( (map + i) -> capsel_m_flg[cap_disp_flg] )
					{
						//	表示フラグが立っている
						if( !(map + i) -> capsel_m_flg[cap_down_flg] )
						{
							//	落下フラグが立っていない
							if( (map + i) -> capsel_m_p < 3 ){
								//	暗くない場合
								(map + i) -> capsel_m_p += 3;
							}
						}
					}
				}

			}
		}

		j = set_down_flg( map );	//	落下フラグの再設定

		if( j == 0 ){	//	落下終了
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
	}
}

/*--------------------------------------
	マップ上消滅アニメーション関数
--------------------------------------*/
void	erase_anime( game_map *map )
{
	u8	i;

	for(i = 0;i < 128;i ++){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){	//	表示フラグが立っている
			if( (map + i) -> capsel_m_flg[cap_condition_flg] ){	//	消滅アニメーション状態だった
				(map + i) -> capsel_m_g ++;	//	消滅アニメーションの進行
				if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 ){	//	ウイルスの場合
					if( (map + i) -> capsel_m_g > erase_virus_b ){
						clear_map( map,(map + i) -> pos_m_x,(map + i) -> pos_m_y );	//	マップ情報の削除
					}
				}else{	//	ウイルス以外の場合
					if( (map + i) -> capsel_m_g > erase_cap_b ){
						clear_map( map,(map + i) -> pos_m_x,(map + i) -> pos_m_y );	//	マップ情報の削除
					}
				}
			}
		}
	}
}

/*--------------------------------------
	カプセル左右移動関数
--------------------------------------*/
void	translate_capsel( game_map *map,game_state *state,s8 move_vec,u8 joy_no)
{
	s8	vec = 0;
	game_cap *cap;

	cap = &state -> now_cap;

	if( cap->pos_y[0] > 0 ){
		if( cap -> capsel_flg[cap_disp_flg] ){	//	表示フラグ
			if( move_vec == cap_turn_r){
				//	右移動の場合
				if(cap -> pos_x[0] == cap -> pos_x[1]){
					//	カプセルが縦並びの場合
					if( cap->pos_x[0] < 7 ){
						if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[0] ) != cap_flg_on ){
							if( cap -> pos_y[1] == 0 ){
								//	移動先に障害物が無い場合
								vec = 1;
							}else{
								if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[1] ) != cap_flg_on ){
									//	移動先に障害物が無い場合
									vec = 1;
								}
							}
						}
					}
				}else{
					//	カプセルが横並びの場合
					if( cap->pos_x[1] < 7 && (get_map_info( map,cap -> pos_x[1] + 1,cap -> pos_y[0] ) != cap_flg_on)){
						//	移動先に障害物が無い場合
						vec = 1;
					}
				}
			}else	if( move_vec == cap_turn_l ){
				//	左移動の場合
				if(cap -> pos_x[0] == cap -> pos_x[1]){
					//	カプセルが縦並びの場合
					if( cap->pos_x[0] > 0 ){
						if( get_map_info( map,cap -> pos_x[0] - 1,cap -> pos_y[0] ) != cap_flg_on ){
							if( cap -> pos_y[1] == 0 ){
								//	移動先に障害物が無い場合
								vec = -1;
							}else{
								if( get_map_info( map,cap -> pos_x[0] - 1,cap -> pos_y[1] ) != cap_flg_on ){
									//	移動先に障害物が無い場合
									vec = -1;
								}
							}
						}
					}
				}else{
					if( cap -> pos_x[0] > 0){
						if( get_map_info( map,cap -> pos_x[0] - 1,cap->pos_y[0] ) != cap_flg_on ){
							//	移動方向に障害物が無い場合
							vec = -1;
						}
					}
				}
			}

			if( vec != 0 ){
				dm_set_se( dm_se_cap_trance );	//	移動SE再生
				state -> cap_move_se_flg = 0;
				cap -> pos_x[0] += vec;
				cap -> pos_x[1] += vec;
			}else{
				if( state -> cap_move_se_flg == 0 ){
					//	移動できなくても一回は鳴らす
					state -> cap_move_se_flg = 1;
					dm_set_se( dm_se_cap_trance );	//	移動SE再生
				}
				if( move_vec == cap_turn_r){
					joyCursorFastSet( LEVER_RIGHT,joy_no );
				}else	if( move_vec == cap_turn_l){
					joyCursorFastSet( LEVER_LEFT,joy_no );
				}
			}
		}
	}
}
/*--------------------------------------
	カプセル左右回転関数
--------------------------------------*/
void	rotate_capsel( game_map *map,game_cap *cap,s8 move_vec )
{
	s8	vec = 0;
	u8	i,save;
	u8	rotate_table[] = {1,3,4,2};
	u8	rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};

	if( cap -> pos_y[0] > 0 )	//	ＮＥＸＴの位置では回転させない
	{
		if( cap -> capsel_flg[cap_disp_flg] ){	//	表示フラグ
			if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	カプセルが縦になっていた場合
				if( cap -> pos_x[0] == 7 || ( get_map_info( map,cap->pos_x[0] + 1,cap->pos_y[0] ) == cap_flg_on) ){	//	右壁に張りついていたか横に障害物があった場合
					if( cap -> pos_x[0] != 0 && (get_map_info( map,cap->pos_x[0] - 1,cap->pos_y[0] ) != cap_flg_on) ){	//	左壁に張りついていないでずれたところに障害物が無かった場合
						cap -> pos_x[0] --;
						vec = 1;
					}
				}else{
					cap -> pos_x[1] ++;
					vec = 1;
				}
				if( vec != 0){
					cap -> pos_y[1] ++;
					if( move_vec == cap_turn_l ){
						//	パレットの入れ替え
						save = cap->capsel_p[0];
						cap->capsel_p[0] = cap->capsel_p[1];
						cap->capsel_p[1] = save;
					}
				}
			}else{	//	カプセルが横になっていた場合
				if( cap -> pos_y[0] == 1 ){	//	最上段だった場合
					cap -> pos_x[1] --;
					vec = -1;
				}else{
					if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] - 1) == cap_flg_on ){	//	回転軸の上に何かあった場合
						if( get_map_info( map,cap -> pos_x[0] + 1,cap -> pos_y[0] - 1) != cap_flg_on ){	//	移動先に何も無かった場合
							cap -> pos_x[0] ++;
							vec = -1;
						}
					}else{	//	回転軸の上に何も無かった場合
						//	座標変更
						cap -> pos_x[1] --;
						vec = -1;
					}
				}
				if( vec != 0 ){
					cap -> pos_y[1] --;
					//	パレットの入れ替え
					if( move_vec == cap_turn_r ){
						save = cap->capsel_p[0];
						cap -> capsel_p[0] = cap->capsel_p[1];
						cap -> capsel_p[1] = save;
					}
				}
			}
			if( vec != 0 ){
				dm_set_se( dm_se_cap_rotate );	//	回転SE再生
				//	グラフィックの入れ替え
				for(i = 0;i < 2;i++){
					save = rotate_table[cap -> capsel_g[i]];
					save += vec;
					cap -> capsel_g[i] = rotate_mtx[save];
				}
			}
		}
	}
}

/*--------------------------------------
	カプセルマガジン作成関数
--------------------------------------*/
void	dm_make_magazine( void )
{
	int	i;

	i = OS_CYCLES_TO_USEC(osGetTime()) + dm_rand_seed + RAND(5000);
	sgenrand(i);		// ｼｬｯﾌﾙ
	dm_rand_seed ++;
	if( dm_rand_seed >= 0xffff )
		dm_rand_seed = 0;

	for( i = 0;i < MAGA_MAX;i++ ){
		 CapsMagazine[i] = genrand( 0xffff )% MAGA_MAX;
	}
}
/*--------------------------------------
	カプセル初期化設定関数(落下フラグを立てない)
--------------------------------------*/
void	dm_init_capsel( game_cap *cap,u8 left_cap_col,u8 right_cap_col )
{
	cap->pos_x[0] = 3;	//	左カプセルＸ座標
	cap->pos_x[1] = 4;	//	右カプセルＸ座標
	cap->pos_y[0] = cap->pos_y[1] = 0;	//	カプセルＹ座標
	cap->capsel_g[0] = capsel_l;		//	グラフィック番号
	cap->capsel_g[1] = capsel_r;		//	グラフィック番号
	cap->capsel_p[0] = left_cap_col;	//	色（左）
	cap->capsel_p[1] = right_cap_col;	//	色（右）
	cap->capsel_flg[cap_disp_flg] = cap_flg_on;	//	表示フラグをＯＮに
	cap->capsel_flg[1] = cap->capsel_flg[2] = cap_flg_off;	//	今のところ未使用
}
/*--------------------------------------
	カプセル初期化設定関数(落下フラグを立てる)
--------------------------------------*/
void	dm_init_capsel_go( game_cap *cap,s8 left_cap_col,s8 right_cap_col )
{
	dm_init_capsel(cap,left_cap_col,right_cap_col);
	cap -> capsel_flg[cap_down_flg] = 1;
}

/*--------------------------------------
	カプセル設定関数
--------------------------------------*/
void	dm_set_capsel( game_state *state )
{
	state -> cap_move_se_flg = 0;
	dm_init_capsel_go( &state -> now_cap,(CapsMagazine[state -> cap_magazine_cnt] >> 4) % 3,CapsMagazine[state ->cap_magazine_cnt] % 3);
	state -> cap_magazine_save = state -> cap_magazine_cnt;	//	前のマガジン位置の保存
	state -> cap_magazine_cnt ++;
	if( state -> cap_magazine_cnt >= 0xfe )
		state -> cap_magazine_cnt = 1;

	dm_init_capsel( &state -> next_cap,(CapsMagazine[state ->cap_magazine_cnt] >> 4) % 3,CapsMagazine[state ->cap_magazine_cnt] % 3);
}
/*--------------------------------------
	カプセル速度上昇設定関数
--------------------------------------*/
void	dm_capsel_speed_up( game_state *state )
{
	state -> cap_count ++;
	if( state -> cap_count >= 10 ){	//	スピードアップ
		dm_set_se( dm_se_cap_speed_up );					//	スピードアップＳＥ再生
		state -> cap_count = 0;
		state -> cap_speed ++;
		if( state -> cap_speed > GameSpeed[3] ){	//	速度限界
			state -> cap_speed = GameSpeed[3];
		}
	}
}

/*--------------------------------------
	操作カプセル落下処理関数
--------------------------------------*/
void	dm_capsel_down( game_state *state,game_map *map )
{
	u8	i,j;
	s8	black_color[] = {0,3};
	game_cap *cap;

	cap = &state -> now_cap;


	if( cap -> pos_y[0] > 0 ){	//	落下中の速度設定
		i  = FallSpeed[state -> cap_speed];	//	カプセル落下速度の設定
		if( cap -> pos_y[0] >= 1 && cap -> pos_y[0] <= 3 ){
			i += BonusWait[cap -> pos_y[0] - 1][state -> cap_def_speed];	// ｶﾌﾟｾﾙの高さによるﾎﾞｰﾅｽｳｴｲﾄを加算
		}
		j = 0;
		if( cap -> pos_y[0] > 0 ){
			if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] + 1) ){
				//	カプセルの下にアイテムがあった場合
//				j = TouchDownWait[evs_playcnt-1];
				j = dm_TouchDownWait;
			}
		}
		state -> cap_speed_max = i + j;
	}else{	//	落下前の速度設定(一律30)
		state -> cap_speed_max = 30;
	}

	state -> cap_speed_count += state -> cap_speed_vec;	//	落下カウンタの増加
	if( state -> cap_speed_count >= state -> cap_speed_max ){	//	一定時間過ぎた？
		state -> cap_speed_count = 0;			//	カウンタクリア

		if( cap -> capsel_flg[cap_disp_flg] ){
			//	１段落下
			if( cap -> pos_y[0] > 0 ){	//	一段目は強制的に落とす

				//	落下カプセルとマップ情報の判定
				if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	落下カプセルは縦か？
					//	カプセルが縦の場合
					if( get_map_info( map,cap -> pos_x[0],cap -> pos_y[0] + 1) ){
						//	カプセルの下にアイテムがあった場合

						cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	落下フラグのクリア
					}
				}else{
					//	カプセルが横の場合
					for( j = 0;j < 2;j++ ){
						if( get_map_info( map,cap -> pos_x[j],cap -> pos_y[j] + 1) ){
							//	カプセルの下にアイテムがあった場合
							cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	落下フラグのクリア
							break;
						}
					}
				}
			}

			//	最下段判定
			for( i = 0;i < 2;i++ ){
				if( cap -> pos_y[i] == 16 ){
					cap -> capsel_flg[cap_down_flg] = cap_flg_off;	//	落下フラグのクリア
					break;
				}
			}

			//	落下処理
			if( cap -> capsel_flg[cap_down_flg] ){	//	落下フラグが立っているか？
				//	落下する
				for( i = 0;i < 2;i++ ){
					if( cap -> pos_y[i] < 16 ){	//	念のため
						cap -> pos_y[i] ++;
					}
				}
				for( i = 0;i < 2;i++ ){
					if( get_map_info( map,cap -> pos_x[i],cap -> pos_y[i]) ){
						//	落下した場所にアイテムがある
						state -> game_condition[dm_static_cnd] = dm_cnd_game_over;	//	ゲームオーバー設定
//						state -> game_condition[dm_mode_now] = dm_cnd_game_over;	//	ゲームオーバー設定
						state -> next_cap.capsel_flg[cap_disp_flg] = cap_flg_off;	//	ネクストカプセルの表示中止
						cap -> capsel_flg[cap_down_flg] = cap_flg_off;				//	落下フラグのクリア
						break;
					}
				}
			}
			if( !cap -> capsel_flg[cap_down_flg] ){
				//	落下できない
				dm_set_se( dm_se_cap_down );					//	着地ＳＥ再生
				state -> game_mode[dm_mode_now] = dm_mode_down_wait;	//	バウンド
				cap -> capsel_flg[cap_disp_flg] = cap_flg_off;	//	表示フラグのクリア
				for( i = 0;i < 2;i++ ){
					if( cap -> pos_y[i] != 0 ){
						//	マップデータに変更
						set_map( map,cap -> pos_x[i],cap -> pos_y[i],
									cap -> capsel_g[i],cap -> capsel_p[i] + black_color[state -> retire_flg[dm_game_over_flg]]);
					}
				}
			}
		}
	}
}
/*--------------------------------------
	ゲームオーバー(積みあがり)判定関数
--------------------------------------*/
s8	dm_check_game_over( game_state *state,game_map *map )
{
	if( state -> game_condition[dm_static_cnd] == dm_cnd_game_over ){
		return	cap_flg_on;		//	ゲームオーバー
	}else{
		return	cap_flg_off;	//	ゲームオーバーじゃない
	}
}

/*--------------------------------------
	ウイルス転送関数
--------------------------------------*/
void	dm_set_virus( game_state *state,game_map *map,virus_map *v_map,u8 *order )
{
	u16	x_pos,y_pos;
	s8	virus_max;
	u8	p,s;

	if( state -> game_condition[dm_mode_now] == dm_cnd_init )
	{
		//	ウイルス数の割り出し
		virus_max = state -> virus_level;
		//	上限オーバーを防ぐ
		if ( virus_max > LEVLIMIT ){
			virus_max = LEVLIMIT;
		}
		//	ウイルス数の割り出し
		virus_max = (virus_max + 1) << 2;

		while(1)
		{
			s = genrand(virus_max);
			if( *(order + s ) != 0xff ){
				p = *(order + s );
				 *(order + s ) = 0xff;
				break;
			}
		}
/*		while(1)
		{
			x_pos = genrand(8);
			y_pos = genrand(16) + 1;

			if( dm_virus_check(v_map,x_pos,y_pos) == TRUE )
				break;
		}
		//	ウイルスセット
		p = ((y_pos - 1) << 3) + x_pos;
		set_virus(map,x_pos,y_pos,(v_map + p) -> virus_type,virus_anime_table[(v_map + p) -> virus_type][state -> virus_anime] );
*/
		set_virus(map,(v_map + p) -> x_pos,(v_map + p) -> y_pos,(v_map + p) -> virus_type,virus_anime_table[(v_map + p) -> virus_type][state -> virus_anime] );
		//	ウイルス情報の削除
//		(v_map + p) -> virus_type = -1;

		state -> virus_number ++;
		if( state -> virus_number >= virus_max )
		{
			//	ウイルス出現終了
			state -> game_condition[dm_mode_now] = dm_cnd_wait;	//	待機状態へ
			state -> game_mode[dm_mode_now]		= dm_mode_wait;	//	待機状態へ
		}
	}
}
/*--------------------------------------
	ウイルスアニメ関数
--------------------------------------*/
void	dm_virus_anime( game_state *state,game_map *map )
{
	u8	i;

	state -> virus_anime_count ++;	// カウントの増加
	if( state -> virus_anime_count > state -> virus_anime_spead ){ // アニメーション間隔を取った
		state -> virus_anime_count = 0;	//	カウントのクリア
		state -> virus_anime += state -> virus_anime_vec;		//	アニメーションの進行
		if( state -> virus_anime > state -> virus_anime_max ){	//	アニメーションの最大コマ数を越えた
			state -> virus_anime_vec = -1;						//	アニメーション進行方向の反転
		}else	if( state -> virus_anime < 1 ){					//	アニメーションの最小コマ数を越えた
			state -> virus_anime_vec = 1;						//	アニメーション進行方向の反転
		}

		//	マップ上のウイルスをアニメーションさせる
		for( i = 0;i < 128;i++ ){
			if( (map + i) -> capsel_m_flg[cap_disp_flg] ){	//	表示されているか？
				//	表示されている
				if( !(map + i) -> capsel_m_flg[cap_condition_flg] ){	//	消滅状態か？
					//	通常状態だった
					if( (map + i) -> capsel_m_flg[cap_virus_flg] >= 0 ){	//	ウイルスか？
						//	ウイルスだった

						//	グラフィック変更
						(map + i) -> capsel_m_g = virus_anime_table[ (map + i) -> capsel_m_flg[cap_virus_flg] ][ state -> virus_anime ];
					}
				}
			}
		}
	}
}
/*--------------------------------------
	ウイルス(カプセル)消滅関数
--------------------------------------*/
void	dm_capsel_erase_anime( game_state *state,game_map *map )
{
	u8	i;

	state -> erase_anime_count ++;
	if( state -> erase_anime_count == dm_erace_speed_1 ){			//	第一段階
		erase_anime( map );	//	消滅アニメーション進行
		state -> erase_anime ++;
	}else	if( state -> erase_anime_count >= dm_erace_speed_2 ){	//	第二段階
		erase_anime( map );	//	消滅アニメーション進行
		set_down_flg( map );		//	落下設定
		state -> erase_anime = 0;
		state -> game_mode[dm_mode_now] = dm_mode_ball_down;
		state -> erase_anime_count = dm_down_speed;	//	消滅直後はすぐに落ちる

	}

}

/*--------------------------------------
	縦消し化処理関数
--------------------------------------*/
void	dm_make_erase_h_line( game_state *state,game_map *map,u8 start,u8 chain,u8 pos_x )
{
	u8	i,p;

	for( i = start;i < start + chain + 1;i++ ){
		p = (i << 3) + pos_x;
		(map + p) -> capsel_m_flg[cap_condition_flg] = cap_flg_on;	//	消滅状態にする
		if( (map + p) -> capsel_m_flg[cap_virus_flg] < 0){
			//	カプセルの場合
			(map + p) -> capsel_m_g = erase_cap_a;
		}else{
			//	ウイルスの場合
			(map + p) -> capsel_m_g = erase_virus_a;
			state -> erase_virus_count ++;	//	消滅ウイルス数の増加
			state -> chain_color[3] |= 0x08;	//	ウイルスを含む連鎖
			state -> chain_color[3] |= 0x10 << (map + p) -> capsel_m_flg[cap_col_flg] ;
		}
	}
}
/*--------------------------------------
	縦消し判定処理関数
--------------------------------------*/
s8	dm_h_erase_chack( game_map *map )
{
	s16	i,j;
	u8	p;
	s8	chain,chain_start,chain_col;

	for(i = 0;i < 8;i++){
		chain = 0;
		chain_col = chain_start = -1;
		for(j = 0;j < 16;j++){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){	//	何か表示されている
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					//	同じ色でなかった
					if( chain >= 3 ){
						//	４個以上だった
						return	cap_flg_on;
					}
					if(j > 12){	//	消滅判定が出来ないため(４個以上無い)
						break;
					}else{
						chain_start = j;									//	連結開始位置の変更
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	色の入れ替え
						chain = 0;
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col ){
					chain++;	//	連結追加
					if(j == 15){	//	最下段に達した場合
						if( chain >= 3 ){
							//	４個以上だった
							return	cap_flg_on;
						}
					}
				}
			}else{
				if( chain >= 3 ){
					//	４個以上だった
					return	cap_flg_on;
				}
				if(j > 12){	//	現在位置がした３段以内の場合中止
					break;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return	cap_flg_off;
}
/*--------------------------------------
	縦消し判定と消滅設定関数
--------------------------------------*/
void	dm_h_erase_chack_set( game_state *state,game_map *map )
{
	s16	i,j;
	u8	p,end;
	s8	chain,chain_start,chain_col;

	for(i = 0;i < 8;i++){
		chain = 0;
		chain_col = chain_start = -1;
		for(j = end = 0;j < 16;j++){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){	//	何か表示されている

				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					//	同じ色でなかった
					if( chain >= 3 ){
						//	４個以上だった
						dm_make_erase_h_line( state,map,chain_start,chain,i );	//	消滅状態にする

						if( state -> chain_count == 0 ){
							state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
						}
						state -> chain_color[chain_col] ++;				//	消滅色のカウント
						state -> chain_line ++;							//	消滅列数のカウント
					}
					if(j > 12){	//	消滅判定が出来ないため(４個以上無い)
						end = 1;
					}else{
						chain_start = j;									//	連結開始位置の変更
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	色の入れ替え
						chain = 0;
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col ){
					chain++;	//	連結追加
					if(j == 15){	//	最下段に達した場合
						if( chain >= 3 ){
							//	４個以上だった
							dm_make_erase_h_line( state,map,chain_start,chain,i );	//	消滅状態にする

							if( state -> chain_count == 0 ){
								state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
							}
							state -> chain_color[chain_col] ++;				//	消滅色のカウント
							state -> chain_line ++;							//	消滅列数のカウント
						}
					}
				}
			}else{
				if( chain >= 3 ){
					//	４個以上だった
					dm_make_erase_h_line( state,map,chain_start,chain,i );	//	消滅状態にする
					if( state -> chain_count == 0 ){
						state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
					}
					state -> chain_color[chain_col] ++;				//	消滅色のカウント
					state -> chain_line ++;							//	消滅列数のカウント
				}
				if(j > 12){	//	消滅判定が出来ないため(４個以上無い)
					end = 1;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
			if( end ){
				break;
			}
		}

	}
}
/*--------------------------------------
	横消し化処理関数
--------------------------------------*/
void	dm_make_erase_w_line( game_state *state,game_map *map,u8 start,u8 chain,u8 pos_y )
{
	u8	i,p;

	p = (pos_y << 3);

	for( i = start;i < start + chain + 1;i++ ){
		(map + p + i) -> capsel_m_flg[cap_condition_flg] = cap_flg_on;	//	消滅状態にする
		if( (map + p + i) -> capsel_m_flg[cap_virus_flg] < 0){
			//	カプセルの場合
			(map + p + i) -> capsel_m_g = erase_cap_a;
		}else{
			//	ウイルスの場合
			(map + p + i) -> capsel_m_g = erase_virus_a;
			state -> erase_virus_count ++;	//	消滅ウイルス数の増加
			state -> chain_color[3] |= 0x08;	//	ウイルスを含む連鎖
			state -> chain_color[3] |= 0x10 << (map + p + i) -> capsel_m_flg[cap_col_flg] ;
		}
	}
}
/*--------------------------------------
	横消し判定処理関数
--------------------------------------*/
s8	dm_w_erase_chack( game_map *map )
{
	s16	i,j;
	u8	p;
	s8	chain,chain_start,chain_col;

	//	横消し
	for( j = 0;j < 16;j++ ){
		chain = 0;
		chain_col = chain_start = -1;
		for( i = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	同じ色でなかった
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					if( chain >= 3 ){
						//	４個以上だった
						return	cap_flg_on;
					}
					if(i > 4){
						break;
					}else{
						chain_start = i;									//	連結開始位置の変更
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	色の入れ替え
						chain = 0;											//	連鎖数クリア
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col){
					chain++;
					if(i == 7){
						if( chain >= 3 ){
							return	cap_flg_on;
						}
					}
				}
			}else{
				//	４個以上だった
				if( chain >= 3 ){
					//	４個以上だった
					return	cap_flg_on;
				}
				if( i > 4 ){
					break;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
		}
	}
	return	cap_flg_off;
}

/*--------------------------------------
	横消し判定と消滅設定関数
--------------------------------------*/
void	dm_w_erase_chack_set( game_state *state,game_map *map )
{
	s16	i,j;
	u8	p,end;
	s8	chain,chain_start,chain_col;

	//	横消し
	for( j = 0;j < 16;j++ ){
		chain = 0;
		chain_col = chain_start = -1;
		for( i = end = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	同じ色でなかった
				if( (map + p) -> capsel_m_flg[cap_col_flg] != chain_col ){
					if( chain >= 3 ){
						//	４個以上だった
						dm_make_erase_w_line( state,map,chain_start,chain,j );	//	消滅状態にする

						if( state -> chain_count == 0 ){
							state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
						}
						state -> chain_color[chain_col] ++;				//	消滅色のカウント
						state -> chain_line ++;							//	消滅列数のカウント
					}
					if(i > 4){
						end = 1;
					}else{
						chain_start = i;									//	連結開始位置の変更
						chain_col = (map + p) -> capsel_m_flg[cap_col_flg];	//	色の入れ替え
						chain = 0;											//	連鎖数クリア
					}
				}else	if( (map + p) -> capsel_m_flg[cap_col_flg] == chain_col){
					chain++;
					if(i == 7){
						if( chain >= 3 ){
							//	４個以上だった
							dm_make_erase_w_line( state,map,chain_start,chain,j );	//	消滅状態にする
							if( state -> chain_count == 0 ){
								state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
							}
							state -> chain_color[chain_col] ++;				//	消滅色のカウント
							state -> chain_line ++;							//	消滅列数のカウント
						}
					}
				}
			}else{
				//	４個以上だった
				if( chain >= 3 ){
					//	４個以上だった
					dm_make_erase_w_line( state,map,chain_start,chain,j );	//	消滅状態にする

					if( state -> chain_count == 0 ){
						state -> chain_color[3] |= 1 << chain_col;	//	１回目の消した色のビットを立てる
					}
					state -> chain_color[chain_col] ++;				//	消滅色のカウント
					state -> chain_line ++;							//	消滅列数のカウント
				}
				if( i > 4 ){
					end = 1;
				}else{
					chain = 0;
					chain_col = chain_start = -1;
				}
			}
			if( end ){
				break;
			}
		}
	}
}
/*--------------------------------------
	縦粒判定処理関数
--------------------------------------*/
void	dm_h_ball_chack( game_map *map )
{
	u8	i,j,p,p2;

	for( i = 0;i < 8;i++ ){
		for( j = 0;j < 16;j++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	表示されている
				if( (map + p) -> capsel_m_g == capsel_u ){
					//	上側のカプセルだったら
					p2 = ((j + 1) << 3) + i;
					if( (map + p2 ) -> capsel_m_g != capsel_d ){	//	１段下を調べる
						(map + p) -> capsel_m_g = capsel_b;
					}
				}else	if( (map + p) -> capsel_m_g == capsel_d ){
					//	下側のカプセルだったら
					if( (map + p) -> pos_m_y == 1 ){	//	最上段で下側のカプセルは
						//	強制的に粒にする
						(map + p) -> capsel_m_g = capsel_b;
					}else{
						p2 = ((j - 1) << 3) + i;
						if( (map + p2 ) -> capsel_m_g != capsel_u ){	//	１段上を調べる
							(map + p) -> capsel_m_g = capsel_b;
						}
					}
				}
			}
		}
	}
}

/*--------------------------------------
	横粒判定処理関数
--------------------------------------*/
void	dm_w_ball_chack( game_map *map )
{
	u8	i,j,p;

	for( j = 0;j < 16;j++ ){
		for( i = 0;i < 8;i++ ){
			p = (j << 3) + i;
			if( (map + p) -> capsel_m_flg[cap_disp_flg] ){
				//	表示されている
				if( (map + p) -> capsel_m_g == capsel_l ){
					//	左側のカプセルの場合
					if( (map + p + 1) -> capsel_m_g != capsel_r ){	//	一つ右を調べる
						(map + p) -> capsel_m_g = capsel_b;
					}
				}else	if( (map + p) -> capsel_m_g == capsel_r ){
					//	右側のカプセルの場合
					if( (map + p - 1 ) -> capsel_m_g != capsel_l ){	//	一つ左を調べる
						(map + p) -> capsel_m_g = capsel_b;
					}
				}
			}
		}
	}
}
/*--------------------------------------
	ゲームオーバーの時の暗くなる関数
--------------------------------------*/
s8	dm_black_up( game_state *state,game_map *map )
{
	u8	i,p;

	if( state -> retire_flg[dm_retire_flg] && !state -> retire_flg[dm_game_over_flg] ){
		//	リタイアフラグが立っているが、ゲームオーバーフラグは立っていない状態

		state -> erase_anime_count ++;
		if( state -> erase_anime_count >= dm_black_up_speed ){	//	一定間隔を取る
			state -> erase_anime_count = 0;

			p = (state -> work_flg - 1) << 3;
			for( i = 0;i < 8;i++ ){
				if( ( map + p + i ) -> capsel_m_flg[cap_disp_flg] ){
					//	カプセル(ウイルス)が表示されていたら
					( map + p + i ) -> capsel_m_p += 3;	//	暗い色に設定
				}
			}
			state -> work_flg --;
			if( state -> work_flg == 0){	//	最上段まで処理し終わったら終了
				state -> retire_flg[dm_game_over_flg] = 1;
				return	cap_flg_on;	//	終了
			}
		}
	}
	return	cap_flg_off;	//	まだ作業中
}
/*--------------------------------------
	やられ設定
--------------------------------------*/
s8	dm_broken_set( game_state *state ,game_map *map )
{
	u16	i,j,chack;
	u16	work[DAMAGE_MAX][2];
	u8	work_b[DAMAGE_MAX][6];
	s8	ret = 0;


	//	やられ判定
#ifdef	DAMAGE_TYPE
	if( state -> cap_attack_work[0][0] != 0x0000 ){
		//	被害あり

		if( state -> max_chain_line < state -> chain_line ){	//	最大消滅列数の保管
			state -> max_chain_line = state -> chain_line;
		}

		state -> chain_line = 0;		//	消滅ライン数のリセット
		state -> chain_count = 0;		//	連鎖数のリセット
		state -> erase_virus_count = 0;	//	消滅ウイルス数のリセット

		for( i = 0,j = 7;i < 16;i += 2,j--){
			chack = state -> cap_attack_work[0][0] & ( 0x0003 << i );
			if( chack != 0 ){
				//	カプセルセット
				set_map( map,j,1,capsel_b,(chack >> i) - 1);
			}
		}
		set_down_flg( map );		//	落下設定

		//	やられデータのシフト
		for( i = 0;i < DAMAGE_MAX;i++ ){
			work[i][0] = state -> cap_attack_work[i][0];		//	データの保存
			work[i][1] = state -> cap_attack_work[i][1];		//	データの保存
			state -> cap_attack_work[i][0] = 0x0000;	//	データクリア
			state -> cap_attack_work[i][1] = 0x0000;	//	データクリア
		}
		for( i = 0,j = 1;i < DAMAGE_MAX - 1;i++,j++ ){
			state -> cap_attack_work[i][0] = work[j][0];
			state -> cap_attack_work[i][1] = work[j][1];
		}
		ret = 1;	//	やられ
	}
#endif
#ifndef	DAMAGE_TYPE
	if( state -> cap_attack_work[0][5] != 0x00 ){
		state -> chain_line = 0;		//	消滅ライン数のリセット
		state -> chain_count = 0;		//	連鎖数のリセット
		state -> erase_virus_count = 0;	//	消滅ウイルス数のリセット

		for( i = j = 0;i < 8;i++ ){
			if( state -> cap_attack_work[0][4] & (0x80 >> i) ){
				//	カプセルセット
				set_map( map,i,1,capsel_b,state -> cap_attack_work[0][j]);
				j ++;
			}
		}
		set_down_flg( map );		//	落下設定

		//	やられデータのシフト
		for( i = 0;i < DAMAGE_MAX;i++ ){
			for( j = 0;j < 6;j++ ){
				work_b[i][j] = state -> cap_attack_work[i][j];
				state -> cap_attack_work[i][j] = 0;
			}
		}

		for( i = 0;i < DAMAGE_MAX - 1;i++ ){
			for( j = 0;j < 6;j++ ){
				state -> cap_attack_work[i][j] = work_b[i + 1][j];
			}
		}
		ret = 1;	//	やられ
	}
#endif
	return	ret;
}
/*--------------------------------------
	攻撃位置を計算する関数
--------------------------------------*/
u8	dm_make_attack_pattern( u8 max )
{
	u8	i,pattern = 0;
	u8	max_pattern[] = {0xaa,0x55};
	s8	flg;

	if( max >= 4 ){	//	４個の場合
		pattern = max_pattern[genrand(2)];
	}else{	//	それ以下の場合
		while( max )
		{
			flg = 1;
			i = genrand(7);
			if( pattern & (1 << i ) ){	//	同じ場所にカプセルがある
				flg = 0;
			}
			if( i != 7 ){
				if( pattern & (1 << (i + 1) ) ){	//	隣にカプセルがある
					flg = 0;
				}
			}
			if( i != 0 ){
				if( pattern & (1 << (i - 1) ) ){	//	隣にカプセルがある
					flg = 0;
				}
			}
			if( flg ){	//	左右にカプセルが無かったらセット
				pattern |= ( 1 << i );
				max --;
			}
		}
	}
	return	pattern	;
}

/*--------------------------------------
	攻撃設定
--------------------------------------*/
s8	dm_attack_set_2p( game_state *state ,u8 player_no )
{
	game_state *enemy_state;
	u8	bom_pattern,bom_pattern_save;
	s8	i,j,k,l,m,n,flg,ret = 0;
	s8	stock_save[4];
	s8	attack_table[][3] = {
		{3,2,1},
		{0,3,2},
		{1,0,3},
		{2,1,0},
	};
	s8	col_work[3];
	u16	chain_count;

	chain_count = state -> chain_line;

	//	攻撃判定
#ifdef	DAMAGE_TYPE
	if( state  -> chain_line > 1 )	//	攻撃
	{
		if( chain_count >= 4 ){
			chain_count = 4;
		}

		bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
		enemy_state = &game_state_data[player_no ^ 1];
		for( i = 0;i < 1;i++ ){
			if( enemy_state -> cap_attack_work[i][0] == 0x0000 ){	//	空きがあったら
				enemy_state -> cap_attack_work[i][1] = player_no;	//	攻撃したプレイヤー番号
				for( j = l = 0;j < 8;j++ ){
					if( bom_pattern & 1 ){	//	座標決定
						for( k = l;k < 3;k++ ){
							if( state -> chain_color[k] > 0 ){
								state -> chain_color[k] --;
								enemy_state -> cap_attack_work[i][0] |= (1 + k) << (j << 1);
								l = k;
								break;
							}
						}
					}
					bom_pattern >>= 1;
				}
				ret = 1;
				break;
			}
		}
	}
#endif
	return	ret;
}

/*--------------------------------------
	攻撃設定( 4P用 )
--------------------------------------*/
void	dm_attack_set_4p( game_state *state ,u8 player_no ,s8 *c_r,s8 *c_y, s8 *c_b )
{
	game_state *enemy_state;
	u16	chain_count;
	u8	bom_pattern,bom_pattern_save;
	s8	i,j,k,l,m,n,flg,ret = 0;
	s8	stock_save[4];
	s8	attack_col[3];
	s8	col_work[3];
	s8	attack_table[][3] = {
		{3,2,1},
		{0,3,2},
		{1,0,3},
		{2,1,0},
	};


	chain_count = state -> chain_line;
	for(i = 0;i < 3;i++){
		attack_col[i] = 0;
	}

	//	攻撃判定
#ifdef	DAMAGE_TYPE
	if( state  -> chain_line > 1 )	//	攻撃
	{
		//	４個所攻撃の場合は最初に計算しておく
		if( chain_count >= 4 ){
			chain_count = 4;
			bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
		}

		for( i = 0;i < 3;i++ ){
			if( state -> chain_color[3] & (1 << i) ){	//	攻撃相手を決める
				flg = 0;
				enemy_state = &game_state_data[attack_table[player_no][i]];
				if( enemy_state -> player_state[PS_TEAM_FLG] != state -> player_state[PS_TEAM_FLG] ){	//	同じチームでない場合攻撃
					if( !enemy_state -> retire_flg[dm_retire_flg] ){	//	リタイアしていなかったら
						flg = 1;
					}else{
						if( enemy_state -> game_condition[dm_training_flg] ){
							if( enemy_state -> retire_flg[dm_training_flg] ){
								flg = 1;
							}
						}
					}
					if( flg ){
						chain_count = state -> chain_line;
						if( chain_count >= 4 ){
							bom_pattern = bom_pattern_save;
							chain_count = 4;
						}else{
							for( j = 0;j < 4;j++ ){
								if( story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] != -1 ){
									//	ストックがあった場合使用
									state -> chain_color[ story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] ]++;	//	ストック色の追加
									story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = -1;	//	ストック色の消去
									chain_count ++;
									if( chain_count >= 4 ){
										break;
									}
								}
							}
							//	攻撃パターンの作成
							bom_pattern_save = bom_pattern = dm_make_attack_pattern( chain_count );
						}
						for(m = 0;m < 3;m++){
							col_work[m] = state -> chain_color[m];
						}
						for( m = 0;m < DAMAGE_MAX;m++ ){
							if( enemy_state -> cap_attack_work[m][0] == 0x0000 ){	//	空きがあったら
								if( !enemy_state -> game_condition[dm_retire_flg] ){	//	リタイアしていなかったら
									set_shock( attack_table[player_no][i], enemy_state -> map_x + 10,207);	//	くらい演出設定
								}
								enemy_state -> cap_attack_work[m][1] = player_no;	//	攻撃したプレイヤー番号
								for( j = l = 0;j < 8;j++ ){
									if( bom_pattern & 1 ){	//	座標決定
										for( k = l;k < 3;k++ ){
											if( col_work[k] > 0 ){
												col_work[k] --;
												enemy_state -> cap_attack_work[m][0] |= (1 + k) << (j << 1);
												l = k;
												break;
											}
										}
									}
									bom_pattern >>= 1;
								}
								attack_col[i] = 1;	//	攻撃した
								break;
							}
						}
					}
				}
			}
		}

		//	ストックのシフト
		for(j = 0;j < 4;j++){
			stock_save[j] = story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j];
			story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = -1;
		}
		for(j = k = 0;j < 4;j++){
			if( stock_save[j] >= 0 ){
				story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][k] = stock_save[j];
				k++;
			}
		}
		//	ストック計算
		for( i = 0;i < 3;i++ ){
			if( state -> chain_color[3] & (1 << i) ){	//	攻撃相手を調べる
				flg = 0;
				enemy_state = &game_state_data[attack_table[player_no][i]];
				if( enemy_state -> player_state[PS_TEAM_FLG] == state -> player_state[PS_TEAM_FLG] ){	//	同じチームの場合ストック計算
					if( !enemy_state -> game_condition[dm_retire_flg] ){	//	リタイアしていなかったら
						for(m = 0;m < 3;m++){
							col_work[m] = state -> chain_color[m];
						}
						for( j = l = 0;j < 4;j++ ){
							if( story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG] ]][j] == -1 ){
								//	もしストックが無かったら
								for( k = l;k < 3;k++ ){
									if( col_work[k] > 0 ){
										col_work[k] --;
										story_4p_stock_cap[story_4p_name_flg[ state -> player_state[PS_TEAM_FLG]]][j] = k;
										l = k;
										break;
									}
								}
							}
						}
					}
				}
			}
		}

	}

#endif
	*c_r = attack_col[0];	//	赤攻撃
	*c_y = attack_col[1];	//	黄攻撃
	*c_b = attack_col[2];	//	青攻撃

}

/*--------------------------------------
	ゲーム処理関数(１Ｐ用)
--------------------------------------*/
s8	dm_game_main_cnt_1P( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j;
	s8	chain_flg[] = {0,0,1};
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};
#ifdef	DM_DEBUG_FLG
	u16	debug_key = 0x0000 | DM_KEY_CU | DM_KEY_CD ;
	//| L_CBUTTONS | R_CBUTTONS;

	//	次のステージへ
	if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){
		if( joyupd[main_joy[0]] & DM_KEY_L ){
			return	dm_ret_next_stage;
		}else	if( joyupd[main_joy[0]] & DM_KEY_R ){
			return	dm_ret_clear;
		}
	}
#endif

	//	ポーズ処理
	if( joyupd[main_joy[0]] & DM_KEY_START ){
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){	//	通常状態だった
			if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
				dm_set_se( dm_se_pause );				//	ポーズＳＥ再生
				return	dm_ret_pause;
			}
		}
	}

//	dm_virus_anime( state,map );	//	ウイルスアニメーション

	for(i = 0;i < dm_retry_coin;i++ ){
		//	コイン回転
		if( !dm_retry_coin_pos[i].flg ){
			//	コインが通常状態だった場合
			dm_retry_coin_pos[i].anime[0]++;
			if( dm_retry_coin_pos[i].anime[0] >= 20 ){
				dm_retry_coin_pos[i].anime[0] = 0;
				dm_retry_coin_pos[i].anime[1] ++;
				if( dm_retry_coin_pos[i].anime[1] > 3 ) {
					dm_retry_coin_pos[i].anime[1] = 0;
				}
			}
		}
	}


	if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	通常状態だった

		dm_level_score_main( state );	//	得点アニメ処理

		//	時間計算
		if( story_time_flg ){
			if( evs_game_time < DM_MAX_TIME ){	//	99分59秒
				evs_game_time ++;
			}
		}

		//	巨大ウイルスアニメーション
		for(i = j = 0;i < 3;i++){
			if( big_virus_anime[i].cnt_now_anime == ANIME_lose ){	//	負けアニメーションの場合
				if( big_virus_anime[i].cnt_now_type[big_virus_anime[i].cnt_now_frame].aniem_flg != anime_no_write ){
					j++;
					if( big_virus_anime[i].cnt_now_frame == 20 ){
						if( !big_virus_flg[i][1] ){				//	巨大ウイルス消滅 SE を再生していない
							big_virus_flg[i][1] = 1;			//	２回鳴らさないためのフラグセット
							dm_set_se( dm_se_big_virus_erace );	//	巨大ウイルス消滅 SE セット
						}
					}
				}
			}else	if( big_virus_anime[i].cnt_now_anime != ANIME_nomal ){
				j ++;
			}
		}
		//	回転（円を描く）処理
		if( j == 0 ){
			big_virus_count[3] ++;
			if( big_virus_count[3] >= 10 ){
				big_virus_count[3] = 0;

				for(i = 0;i < 3;i++){
					big_virus_position[i][2] ++;
					if( big_virus_position[i][2] >= 360 ){
						big_virus_position[i][2] = 0;
					}
					big_virus_position[i][0] = (( 10 * sinf( DEGREE( big_virus_position[i][2] ) ) ) * -2 ) + 45;
					big_virus_position[i][1] = (( 10 * cosf( DEGREE( big_virus_position[i][2] ) ) ) * 2  ) + 155;
				}
			}
		}
//		chain_main( state,player_no ,state -> chain_line,chain_flg[flg],center[flg][player_no] );	//	連鎖演出処理
	}

	//	内部処理部分
	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:	//	ウイルス配置
		dm_set_virus( state,map,virus_map_data[player_no],virus_map_disp_order[player_no] );
		return	dm_ret_virus_wait;
	case	dm_mode_wait:	//	ただの待ち
		return	dm_ret_virus_wait;
	case	dm_mode_throw:	//	投げ中
		state -> cap_speed_count ++;
		if( state -> cap_speed_count == FlyingCnt[state -> cap_def_speed]){	//	カプセルが瓶内部に入ったら
			dm_init_capsel_go( &state -> now_cap,(CapsMagazine[state ->cap_magazine_save] >> 4) % 3,CapsMagazine[state ->cap_magazine_save] % 3);
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	カプセル落下開始
			state -> cap_speed_count = 30;					//	初段落下
			dm_capsel_down( state,map );
		}
		break;
	case	dm_mode_down_wait:	//	着地ウェイト
		if( dm_check_game_over( state,map ) ){	//	ゲームオーバーの場合
			for( i = 0;i < 3;i++ ){
				if( big_virus_anime[i].cnt_now_anime != ANIME_lose ){	//	消滅していなかったら
					dm_anime_set( &big_virus_anime[i],ANIME_win );	//	巨大ウイルス笑いアニメーションセット
				}
			}
			return	dm_ret_game_over;			//	積みあがり(ゲームオーバー)
		}else{
			if( dm_h_erase_chack( map ) != cap_flg_off || dm_w_erase_chack( map ) != cap_flg_off ){	//	消滅があった場合
				state -> game_mode[dm_mode_now] = dm_mode_erase_chack;	//	消滅開始
				state -> cap_speed_count = 0;
			}else{
				state -> game_mode[dm_mode_now] = dm_mode_cap_set;	//	カプセルセットを指定しておく
			}
		}
		break;
	case	dm_mode_erase_chack:	//	消滅判定
		state -> cap_speed_count ++;
		if( state -> cap_speed_count >= dm_bound_wait ){	//	着地ウェイトを取った

			state -> cap_speed_count = 0;
			state -> game_mode[dm_mode_now] = dm_mode_erase_anime;	//	消滅開始

			dm_h_erase_chack_set( state,map );	//	縦消し判定
			dm_w_erase_chack_set( state,map );	//	横消し判定
			dm_h_ball_chack( map );				//	縦粒化処理
			dm_w_ball_chack( map );				//	横粒化処理


			//	各色のウイルス数の取得
			state -> virus_number = get_virus_color_count( map,&big_virus_count[0], &big_virus_count[1] ,&big_virus_count[2] );
			for( i = 0;i < 3;i++ )
			{
				if( big_virus_count[i] == 0 ){	//	各色のウイルスが全滅した場合
					if( !big_virus_flg[i][0] ){		//	まだ消滅フラグが立っていない場合
						big_virus_flg[i][0] = 1;	//	消滅フラグ立をたてる
						dm_anime_set( &big_virus_anime[i],ANIME_lose );	//	巨大ウイルス消滅アニメーションセット
						if( state -> virus_number != 0 ){
							dm_set_se( dm_se_big_virus_damage );			//	巨大ウイルスダメージ
						}
					}
				}else{
					if( state -> chain_color[3] & (0x10 << i) ){
						dm_anime_set( &big_virus_anime[i],ANIME_damage );	//	巨大ウイルスやられアニメーションセット
						dm_set_se( dm_se_big_virus_damage );				//	巨大ウイルスダメージ
					}
				}
			}
			state -> chain_color[3] &= 0x0f;	//	消滅色のリセット


			if( state -> virus_number == 0 ){
				//	ウイルス全滅
				dm_score_make( state,0 );									//	得点計算
				if( evs_high_score < state -> game_score ){
					evs_high_score = state -> game_score;	//	ハイスコアの更新
				}
				state -> game_condition[dm_mode_now]	= dm_cnd_stage_clear;	//	ステージクリア
				state -> game_mode[dm_mode_now] 		= dm_mode_stage_clear;
				//	最大消滅列数の保管
				if( state -> max_chain_line < state -> chain_line ){
					state -> max_chain_line = state -> chain_line;
				}
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	ウイルスが残り３個以下のとき警告音を鳴らす
				if( !last_3_se_flg ){
					//	１回ならしたら以後鳴らさない
					last_3_se_flg = 1;
					dm_set_se( dm_se_last_3 );			//	ウイルス残り３個ＳＥ再生
				}
				//	音楽速度アップのフラグを立てる
				if( bgm_bpm_flg[0] == 0){
					bgm_bpm_flg[0] = 1;					//	２回処理しないためのフラグ
					bgm_bpm_flg[1] = 1;					//	処理を行うフラグを立てる
				}
			}
			state ->chain_count ++;
			if( state ->chain_line < 2 ){
				if( state -> chain_color[3] & 0x08 ){
					dm_level_score_make( state );			//	得点計算
					state -> chain_color[3] &= 0xF7;		//	消滅色のリセット
					dm_set_se( dm_se_virus_erace );			//	ウイルスを含む消滅ＳＥ再生
				}else{
					dm_set_se( dm_se_cap_erace );			//	カプセルのみの消滅ＳＥ再生
				}
			}else{
				if( state -> chain_color[3] & 0x08 ){
					dm_level_score_make( state );			//	得点計算
					state -> chain_color[3] &= 0xF7;		//	消滅色のリセット
				}
				i = state ->chain_line - 2;
				if( i > 2 )
					i = 2;
				dm_set_se( dm_se_attack_a + i );
			}
		}
		break;
	case	dm_mode_erase_anime:	//	消滅アニメーション
		dm_capsel_erase_anime( state,map );
		break;
	case	dm_mode_ball_down:		//	カプセル(粒)落下
		go_down( state,map,dm_down_speed );
		break;
	case	dm_mode_cap_set:		//	新カプセルセット
		dm_warning_h_line( state,map );	//	積みあがりチェック

		dm_set_capsel( state );							//	新カプセルセット
		dm_capsel_speed_up( state );					//	落下カプセル速度の計算
		dm_anime_set( &state -> anime,ANIME_attack );	//	攻撃アニメーションセット(仮)
		dm_attack_se( state,player_no );				//	攻撃ＳＥ再生

		//	落下前思考追加位置
		if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){	// CPU の場合
			aifMakeFlagSet( state );
		}

		//	最大消滅列数の保管
		if( state -> max_chain_line < state -> chain_line ){
			state -> max_chain_line = state -> chain_line;
		}

		state -> game_mode[dm_mode_now] = dm_mode_throw;	//	カプセル落下処理へ
		state -> cap_speed_count = 0;	//	カウントのリセット
		state -> cap_speed_max = 0;		//	落下速度のリセット
		state -> chain_line = 0;		//	消滅ライン数のリセット
		state -> chain_count = 0;		//	連鎖数のリセット
		state -> erase_virus_count = 0;	//	消滅ウイルス数のリセット
		for( i = 0;i < 4;i++ )
			state -> chain_color[i] = 0;		//	消滅色のリセット
		break;
	case	dm_mode_get_coin:		//	コイン獲得
		if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] < 60 ){
			dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] ++;	//	移動値の増加
			dm_retry_coin_pos[dm_retry_coin - 1].pos += dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0];	//	コイン座標の変更

			//	コインの回転
			dm_retry_coin_pos[dm_retry_coin - 1].anime[0]++;
			if( dm_retry_coin_pos[dm_retry_coin - 1].anime[0] >= ( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] >> 2 ) ){
				dm_retry_coin_pos[dm_retry_coin - 1].anime[0] = 0;
				dm_retry_coin_pos[dm_retry_coin - 1].anime[1] ++;
				if( dm_retry_coin_pos[dm_retry_coin - 1].anime[1] > 3 ) {
					dm_retry_coin_pos[dm_retry_coin - 1].anime[1] = 0;
				}
			}

			if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 60 ){
				dm_retry_coin_pos[dm_retry_coin - 1].flg = 0;	//	回転を同じ物にする
				//	コインの座標と回転の調節
				for( i = 1;i < 3;i++ ){
					dm_retry_coin_pos[i].pos = dm_retry_coin_pos[0].pos;
					dm_retry_coin_pos[i].anime[0] = dm_retry_coin_pos[0].anime[0];
					dm_retry_coin_pos[i].anime[1] = dm_retry_coin_pos[0].anime[1];
				}
			}else	if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 30 ){
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = 1;
			}
		}
	case	dm_mode_stage_clear:	//	ステージクリア演出
		i = try_next_stage_main();
		if( i > 0){
			if( joyupd[main_joy[0]] & DM_ANY_KEY ){
				return	dm_ret_next_stage;
			}
		}
		break;
	case	dm_mode_game_over:			//	ゲームオーバー演出
	case	dm_mode_game_over_retry:	//	ゲームオーバー & リトライ
		i = game_over_main();
		if( i > 0 )
		{
			if( dm_retry_flg ){
				//	リトライ可能の場合
				if( joyupd[main_joy[0]] & DM_KEY_UP ){
					if( dm_retry_position[0][0] > 0 )
						dm_retry_position[0][0] = 0;
					}else	if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
					if( dm_retry_position[0][0] < 1 )
						dm_retry_position[0][0] = 1;
				}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
					if( dm_retry_position[0][0] ){
						return	dm_ret_end;			//	終了
					}else{
						if( state -> game_retry < 999 ){
							state -> game_retry ++;	//	リトライ数の追加
						}
						dm_set_se( dm_se_get_coin );			//	コイン使用音
						if( state -> virus_level >= 22 ){
							dm_retry_coin_pos[dm_retry_coin - 1].flg = 1;			//	統一回転をしないようにする
							dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = -1;	//	移動方向の設定
							dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] = 0;	//	カウンタの設定
							state -> game_mode[dm_mode_now] = dm_mode_use_coin;
						}else{
							return	dm_ret_retry;		//	リトライ
						}
					}
				}
			}else{
				//	ゲームオーバー
				if( joyupd[main_joy[0]] & DM_ANY_KEY ){
					return	dm_ret_end;
				}
			}
		}
		break;
	case	dm_mode_use_coin:	//	コイン使用デモ
		if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] < 30 ){
			dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] ++;	//	移動値の増加
			dm_retry_coin_pos[dm_retry_coin - 1].pos += dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0];	//	コイン座標の変更
			//	コインの回転
			dm_retry_coin_pos[dm_retry_coin - 1].anime[0]++;
			if( dm_retry_coin_pos[dm_retry_coin - 1].anime[0] >= ( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] >> 2 ) ){
				dm_retry_coin_pos[dm_retry_coin - 1].anime[0] = 0;
				dm_retry_coin_pos[dm_retry_coin - 1].anime[1] ++;
				if( dm_retry_coin_pos[dm_retry_coin - 1].anime[1] > 3 ) {
					dm_retry_coin_pos[dm_retry_coin - 1].anime[1] = 0;
				}
			}
			if( dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] == 30 ){
				//	処理終了
//				dm_retry_coin_pos[dm_retry_coin - 1].flg = 0;	//	統一回転をするようにする
				dm_retry_coin --;			// 	コイン数減少
				return	dm_ret_retry;		//	リトライ
			}
		}

		break;
	case	dm_mode_pause:	//	ポーズ処理
		pause_main( player_no,center[flg][player_no] );
#ifdef	DM_DEBUG_FLG
		//	デバック用操作
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	デバック処理に
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
		}
#endif
		break;
	case	dm_mode_pause_retry:
		pause_main( player_no,center[flg][player_no] );

#ifdef	DM_DEBUG_FLG

		//	デバック用操作
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	デバック処理に
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
			break;
		}
#endif
		if( dm_retry_flg ){
			//	リトライ可能の場合
			if(  joyupd[main_joy[0]] &  DM_KEY_START  ){
				return	dm_ret_pause;		//	ポーズ解除
			}else	if( joyupd[main_joy[0]] & DM_KEY_UP ){
				if( dm_retry_position[0][0] > 0 )
					dm_retry_position[0][0] = 0;
				}else	if( joyupd[main_joy[0]] & DM_KEY_DOWN ){
				if( dm_retry_position[0][0] < 1 )
					dm_retry_position[0][0] = 1;
			}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
				if( dm_retry_position[0][0] ){
					return	dm_ret_end;			//	終了
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;	//	リトライ数の追加
					}
					if( state -> virus_level >= 22 ){
						dm_set_se( dm_se_get_coin );							//	コイン使用音
						dm_retry_coin_pos[dm_retry_coin - 1].flg = 1;			//	統一回転をしないようにする
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] = -1;	//	移動方向の設定
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] = 0;	//	カウンタの設定
						state -> game_mode[dm_mode_now] = dm_mode_use_coin;
					}else{
						return	dm_ret_retry;		//	リトライ
					}
				}
			}
		}else{
			if(  joyupd[main_joy[0]] & DM_KEY_START ){
				return	dm_ret_pause;		//	ポーズ解除
			}else	if( joyupd[main_joy[0]] & DM_KEY_A ){
				if( dm_retry_position[0][0] ){
					return	dm_ret_game_end;	//	完全終了
				}
			}
		}
		break;
#ifdef	DM_DEBUG_FLG
	case	dm_mode_debug:
		//	デバック用操作
		cnt_debug_main( player_no );
		if( (joyupd[main_joy[0]] & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_pause;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_pause;						//	デバック処理に
			evs_gamespeed = debug_gamespeed;
			if ( debug_p1cpu ) {
				game_state_data[0].player_state[0] = PUF_PlayerCPU;
				aiDebugP1 = debug_p1cpu - 1;
			} else {
				game_state_data[0].player_state[0] = PUF_PlayerMAN;
			}
		}
		break;
#endif
	case	dm_mode_no_action:	//	何もしない
		break;
	}
	return	0;
}

/*--------------------------------------
	ゲーム処理関数(多人数用)
--------------------------------------*/
s8	dm_game_main_cnt( game_state *state,game_map *map,u8 player_no ,u8 flg )
{
	s8	i,j,out;
	s8	chain_flg[] = {0,0,1};
	u8	damage_se[] = {dm_se_dmage_a,dm_se_dmage_b};
	u8	chain_se[] = {dm_se_attack_a,dm_se_attack_b};
	s8	attack_col[] = {0,0,0};
	u16	key_data;
	s16	center[][4] = {
		{160,160,160,160},
		{68,251,0,0},
		{52,124,196,268}
	};
#ifdef	DM_DEBUG_FLG
	u16	debug_key = 0x0000 | DM_KEY_CU | DM_KEY_CD ;
	//| L_CBUTTONS | R_CBUTTONS;
#endif

	//	キー情報の取得
	if( flg == 1 ){
		//	2P時
		key_data = joyupd[main_joy[player_no]];
	}else	if( flg == 2 ){
		//	4P時
		key_data = joyupd[player_no];
	}



	//	PAUSE 要求 or PAUSE 解除要求処理
	if( key_data & DM_KEY_START ){
#ifndef	DM_DEBUG_FLG
		if( state -> player_state[PS_PLAYER] != PUF_PlayerCPU ){
			//	プレイヤーの場合
			if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
				if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
					dm_set_se( dm_se_pause );				//	ポーズＳＥ再生
					return	dm_ret_pause;
				}
			}
		}
#else
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait || state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
			if( state -> game_condition[dm_mode_now] != dm_cnd_init ){
				dm_set_se( dm_se_pause );				//	ポーズＳＥ再生
				return	dm_ret_pause;
			}
		}
#endif
	}

	//	強制勝利
#ifdef	DM_DEBUG_FLG
	if( key_data & DM_KEY_R ){
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){
			return	dm_ret_clear;
		}
	}
#endif



	if( state -> game_condition[dm_mode_now] != dm_cnd_wait && state -> game_condition[dm_mode_now] != dm_cnd_pause ){
		dm_black_up( state,map );		//	黒上がり処理
	}

	if( state -> game_condition[dm_mode_now] == dm_cnd_wait ){	//	通常状態だった
		bubble_main( player_no );	//	攻撃演出処理
//		chain_main( state ,player_no ,state -> chain_line,chain_flg[flg],center[flg][player_no] );	//	連鎖演出処理
		shock_main( player_no, state -> map_x + 10,207);		//	くらい演出処理
		humming_main( player_no, state -> map_x + 20 );			//	ハミング演出処理
	}

#ifdef	DM_DEBUG_FLG
//	if ( game_state_data[0].player_state[0] == PUF_PlayerCPU && win_count[0] < 10 && win_count[1] < 10 ) {
	if ( game_state_data[0].player_state[0] == PUF_PlayerCPU && win_count[0] < WINMAX && win_count[1] < WINMAX ) {
		switch( state -> game_mode[dm_mode_now] )
		{
		case	dm_mode_game_over:		//	ゲームオーバー演出
		case	dm_mode_win:	//	WIN演出
		case	dm_mode_lose:	//	LOSE演出
		case	dm_mode_draw:	//	DRAW演出
//			joyupd[player_no] |= DM_ANY_KEY;
			key_data |= DM_ANY_KEY;
			break;
		}
	}

#endif

	switch( state -> game_mode[dm_mode_now] )
	{
	case	dm_mode_init:		//	ウイルス配置
		dm_set_virus( state,map,virus_map_data[player_no],virus_map_disp_order[player_no] );
		return	dm_ret_virus_wait;
	case	dm_mode_wait:		//	ウェイト
		return	dm_ret_virus_wait;
	case	dm_mode_down_wait:	//	着地ウェイト
		if( dm_check_game_over( state,map ) ){	//	積みあがり判定
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
			dm_h_ball_chack( map );				//	縦粒化処理
			dm_w_ball_chack( map );				//	横粒化処理

			state -> virus_number = get_virus_count( map ); //	ウイルス数の取得
			if( state -> virus_number == 0 ){
				//	ウイルス消滅
				dm_score_make( state,1 );	//	得点計算
				//	最大消滅列数の保管
				if( state -> max_chain_line < state -> chain_line ){
					state -> max_chain_line = state -> chain_line;
				}
				return	dm_ret_clear;
			}else	if( state -> virus_number > 0 && state -> virus_number < 4 ){
				//	ウイルスが残り３個以下のとき警告音を鳴らす
				if( !last_3_se_flg ){
					//	１回ならしたら以後鳴らさない
					last_3_se_flg = 1;
					dm_set_se( dm_se_last_3 );			//	ウイルス残り３個ＳＥ再生
				}
				//	音楽速度アップのフラグを立てる
				if( bgm_bpm_flg[0] == 0){
					bgm_bpm_flg[0] = 1;					//	２回行わないためのフラグ
					bgm_bpm_flg[1] = 1;					//	処理を行うフラグを立てる
				}
			}

			state ->chain_count ++;

			switch( evs_gamesel )
			{
			case	GSL_2PLAY:	//	2Ｐの時
			case	GSL_2DEMO:	//	2Ｐデモ
			case	GSL_VSCPU:	//	VSCPUのとき
				if( state ->chain_line < 2 ){

					if( state -> chain_color[3] & 0x08 ){
						dm_score_make( state,1 );				//	得点計算
						state -> chain_color[3] &= 0xF7;		//	ウイルス消滅のリセット
						dm_set_se( dm_se_virus_erace );			//	ウイルスを含む消滅ＳＥ再生
					}else{
						dm_set_se( dm_se_cap_erace );			//	カプセルのみの消滅ＳＥ再生
					}
				}else{
					j = state ->chain_line - 2;
					if( j > 2 )
						j = 2;
					dm_set_se( chain_se[player_no] + j );

					if( state -> chain_color[3] & 0x08 ){
						state -> chain_color[3] &= 0xF7;		//	ウイルス消滅のリセット
						dm_score_make( state,1 );				//	得点計算
					}
				}
				break;
			case	GSL_4PLAY:	//	4Ｐの時
			case	GSL_4DEMO:	//	4Ｐデモ
				if( state -> chain_line > 1 ){
					set_humming( player_no,state -> map_x + 20,206 ); // ハミング開始
				}

				if( state -> chain_color[3] & 0x08 ){
					state -> chain_color[3] &= 0xF7;		//	ウイルスを含む消滅のリセット
					dm_score_make( state,1 );				//	得点計算
					dm_set_se( dm_se_virus_erace );			//	ウイルスを含む消滅ＳＥ再生
				}else{
					dm_set_se( dm_se_cap_erace );			//	カプセルのみの消滅ＳＥ再生
				}
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
		dm_warning_h_line( state,map );	//	積みあがりチェック

		switch( evs_gamesel ){
		case	GSL_2DEMO:	//	2Ｐデモの時
		case	GSL_2PLAY:	//	2Ｐの時
		case	GSL_VSCPU:	//	VSCPUの時
			i =  dm_attack_set_2p( state ,player_no );	//	攻撃判定
			if( i ){
				dm_anime_set_cnt( &state -> anime,ANIME_attack );	//	攻撃アニメーションセット
			}
			set_bubble( state ,player_no,0 ,i,attack_col[0],attack_col[1],attack_col[2] );			//	演出設定
			if( dm_broken_set( state , map ) ){			//	やられ判定
				dm_anime_set( &state -> anime,ANIME_damage );		//	やられアニメーションセット(強制)
				dm_set_se( damage_se[player_no] );		//	ウイルスを含む消滅ＳＥ再生
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	粒落下処理へ
				out = 0;
				if ( state->ai.aiState & AIF_DAMAGE ) {
					state->ai.aiState |= AIF_DAMAGE2;
				} else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
			break;
		case	GSL_4PLAY:	//	4Ｐモードの時
		case	GSL_4DEMO:	//	4Ｐデモの時
			dm_attack_set_4p( state ,player_no,&attack_col[0],&attack_col[1],&attack_col[2] );	//	攻撃判定
			set_bubble( state ,player_no,1,0,attack_col[0],attack_col[1],attack_col[2]);		//	演出設定
			if( dm_broken_set( state , map )){		//	やられ判定
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	粒落下処理へ
				out = 0;
				if ( state->ai.aiState & AIF_DAMAGE ) {
					state->ai.aiState |= AIF_DAMAGE2;
				} else {
					state->ai.aiState |= AIF_DAMAGE;
				}
			}
		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	落下カプセル速度の計算

			//	最大消滅列数の保管
			if(	state -> max_chain_line < state -> chain_line ){
				state -> max_chain_line = state -> chain_line;
			}

			state -> chain_line = 0;			//	消滅ライン数のリセット
			state -> chain_count = 0;			//	連鎖数のリセット
			state -> erase_virus_count = 0;		//	消滅ウイルス数のリセット
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;	//	消滅色のリセット
			}

			state -> game_mode[dm_mode_now] = dm_mode_down;	//	カプセル落下処理へ
			//	落下前思考追加位置
			if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifMakeFlagSet( state );
			}
		}
		break;
	case	dm_mode_win_retry:
		i = win_main( center[flg][player_no] ,player_no );
		if( i > 0 ){
			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	ポーズ解除
			}else	if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	終了
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	リトライ数の追加
					}
					return	dm_ret_retry;		//	リトライ
				}
			}
		}
		break;
	case	dm_mode_win:	//	WIN演出
		i = win_main( center[flg][player_no],player_no );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_lose_retry:	//	LOSE & RERY
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0 ){
			if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	終了
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	リトライ数の追加
					}
					return	dm_ret_retry;		//	リトライ
				}
			}
		}
		break;
	case	dm_mode_lose:	//	LOSE演出
		i = lose_main( player_no,center[flg][player_no] );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_draw:	//	DRAW演出
		i = draw_main( player_no,center[flg][player_no] );
		if( i > 0){
			if( key_data & DM_ANY_KEY ){
				return	dm_ret_end;
			}
		}
		break;
	case	dm_mode_tr_chaeck:	//	練習確認
		if( key_data & DM_KEY_START ){	//	攻撃無し版
			return	dm_ret_tr_a;
		}else	if( key_data & DM_KEY_Z ){	//	攻撃あり版
			return	dm_ret_tr_b;
		}
		break;
	case	dm_mode_training:	//	トレーニング初期化
		state -> retire_flg[dm_game_over_flg] = 1;				//	ゲームオーバーフラグを立てる

		clear_map_all( map );		//	マップ情報削除
		state -> virus_number = 0;	//	ウイルス数のクリア
		state -> warning_flg = 0;	//	警告音フラグのクリア

		//	カプセル落下速度の設定
		state -> cap_speed = GameSpeed[state -> cap_def_speed];
		state -> cap_speed_max = 0;								//	随時設定される
		state -> cap_speed_vec = 1;								//	落下カウンタ増加値
		state -> cap_magazine_cnt = 1;							//	マガジン残照を1にする
		state -> cap_count = state ->cap_speed_count = 0;		//	カウンタの初期化
		//	カプセル情報の初期設定
		dm_set_capsel( state );

		//	ウイルスアニメーションの設定
		state -> erase_anime = 0;								//	消滅アニメーションコマ数の初期化
		state -> erase_anime_count = 0;							//	消滅アニメーションカウンタの初期化
		state -> erase_virus_count = 0;							//	消滅ウイルスカウンタの初期化

		//	連鎖数の初期化
		state -> chain_count = state -> chain_line =  0;

		//	攻撃カプセルデータの初期化
		for( i = 0;i < 4;i++ ){
			state -> chain_color[i] = 0;
		}
		//	被爆カプセルデータの初期化
#ifdef	DAMAGE_TYPE
		for( i = 0;i < DAMAGE_MAX;i++ ){
			state -> cap_attack_work[i][0] = 0;	//	攻撃無し
			state -> cap_attack_work[i][1] = 0;	//	攻撃無し
		}
#endif
#ifndef	DAMAGE_TYPE
		for( i = 0;i < DAMAGE_MAX;i++ ){
			for( j = 0;j < 5;j++ ){
				state -> cap_attack_work[i][j] = 0;
			}
		}
#endif
		stop_chain_main(player_no);	//	連鎖数表示の強制終了
		state -> game_mode[dm_mode_now] = dm_mode_down;		//	通常処理に
		state -> game_condition[dm_mode_now] = dm_cnd_wait;		//	通常状態に
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
		dm_warning_h_line( state,map );				//	積みあがりチェック
		if( state -> retire_flg[dm_training_flg] ){	//	攻撃あり
			dm_attack_se( state,player_no );			//	攻撃ＳＥ再生
			dm_attack_set_4p( state ,player_no,&attack_col[0],&attack_col[1],&attack_col[2] );				//	攻撃判定
			set_bubble( state ,player_no,1,0,attack_col[0],attack_col[1],attack_col[2] );					//	演出設定
			if( dm_broken_set( state , map )){					//	やられ判定
				state -> game_mode[dm_mode_now] = dm_mode_ball_down;	//	粒落下処理へ
				out = 0;
			}
		}else{

#ifdef	DAMAGE_TYPE
			for( j = 0;j < DAMAGE_MAX;j++ ){
				state -> cap_attack_work[j][0] = 0;	//	攻撃無し
				state -> cap_attack_work[j][1] = 0;	//	攻撃無し
			}
#endif

		}
		if( out ){
			dm_set_capsel( state );
			dm_capsel_speed_up( state );		//	落下カプセル速度の計算
			state -> chain_line = 0;		//	消滅ライン数のリセット
			state -> chain_count = 0;		//	連鎖数のリセット
			state -> erase_virus_count = 0;	//	消滅ウイルス数のリセット
			for( i = 0;i < 4;i++ ){
				state -> chain_color[i] = 0;		//	消滅色のリセット
			}
			state -> game_mode[dm_mode_now] = dm_mode_down;	//	カプセル落下処理へ
		}
		break;
	case	dm_mode_pause:	//	ポーズ処理
		pause_main( player_no,center[flg][player_no] );
		if(  key_data & DM_KEY_START ){
			return	dm_ret_pause;		//	ポーズ解除
		}
#ifdef	DM_DEBUG_FLG
		//	デバック用操作
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	デバック処理に
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
		}
#endif
		break;
	case	dm_mode_pause_retry:
		pause_main( player_no,center[flg][player_no] );

		if( dm_retry_flg ){

			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	ポーズ解除
			}else	if( key_data & DM_KEY_UP ){
				if( dm_retry_position[player_no][0] > 0 )
					dm_retry_position[player_no][0] = 0;
			}else	if( key_data & DM_KEY_DOWN ){
				if( dm_retry_position[player_no][0] < 1 )
					dm_retry_position[player_no][0] = 1;
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	終了
				}else{
					if( state -> game_retry < 999 ){
						state -> game_retry ++;		//	リトライ数の追加
					}
					return	dm_ret_retry;		//	リトライ
				}
			}
		}else{
			if(  key_data & DM_KEY_START ){
				return	dm_ret_pause;		//	ポーズ解除
			}else	if( key_data & DM_KEY_A ){
				if( dm_retry_position[player_no][0] ){
					return	dm_ret_game_end;	//	完全終了
				}
			}

		}

#ifdef	DM_DEBUG_FLG
		//	デバック用操作
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_debug;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_debug;						//	デバック処理に
			debug_gamespeed = evs_gamespeed;
			if ( game_state_data[0].player_state[0] == PUF_PlayerMAN ) {
				debug_p1cpu = 0;
			} else {
				debug_p1cpu = aiDebugP1 + 1;
			}
			break;
		}
#endif

		break;
#ifdef	DM_DEBUG_FLG
	case	dm_mode_debug:
		//	デバック用操作
		cnt_debug_main( player_no );
		if( (key_data & debug_key) == debug_key )
		{
			state -> game_condition[dm_mode_now] = dm_cnd_pause;					//	デバック状態に
			state -> game_mode[dm_mode_now] = dm_mode_pause;						//	デバック処理に
			evs_gamespeed = debug_gamespeed;
			if ( debug_p1cpu ) {
				game_state_data[0].player_state[0] = PUF_PlayerCPU;
				aiDebugP1 = debug_p1cpu - 1;
			} else {
				game_state_data[0].player_state[0] = PUF_PlayerMAN;
			}
		}
		break;
#endif
	case	dm_mode_no_action:	//	何もしない
		break;
	}
	return	0;
}
/*--------------------------------------
	ゲーム処理関数(1P用)
--------------------------------------*/
s8	dm_game_main_1p(void)
{
	s8	i,j,flg;
	game_state	*state	= &game_state_data[0];
	game_map	*map	= game_map_data[0];

	flg = dm_game_main_cnt_1P( state,map,0,0 );
	dm_anime_control( &state -> anime );	//	アニメーション
	for(i = 0;i < 3;i++){
		dm_anime_control( &big_virus_anime[i] );	//	巨大ウイルスアニメーション
	}

	//	アニメーションの再稼動のカウント
	if( state -> game_condition[dm_static_cnd] != dm_cnd_wait && state -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		//	勝敗が決定していた場合カウントする
		dm_anime_restart_count ++;
		if( dm_anime_restart_count >= ANIME_RESTRT_TIME ){	//	5秒
			//	再稼動
			dm_anime_restart_count = 0;
			dm_anime_restart( &state -> anime );
		}
	}

	if( flg == dm_ret_virus_wait ){		//	ウイルス配置完了
		if( dm_start_time > 0 ){
			dm_start_time --;
		}else{
			if( state -> game_mode[dm_mode_now] == dm_mode_wait ){
				story_time_flg = 1;									//	タイマー作動
				state -> game_mode[dm_mode_now] = dm_mode_throw;	//	カプセル落下開始
			}
		}
	}else	if( flg == dm_ret_pause ){	//	ポーズ要求 or ポーズ解除要求
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	通常状態ならば
			auSeqpVolumeDown();						//	音量減少
			init_pause();							//	PAUSE 演出処理初期化

			state -> game_condition[dm_static_cnd]	=	dm_cnd_pause;									//	絶対的コンディションを PASUE 状態にする
			state -> game_condition[dm_mode_old]	=	game_state_data[0].game_condition[dm_mode_now];	//	現在の状態を保存
			state -> game_mode[dm_mode_old]			=	game_state_data[0].game_mode[dm_mode_now];		//	現在の処理の保存

			state -> game_condition[dm_mode_now]	=	dm_cnd_pause_re;		//	状態を PAUSE & RETRY に設定
			state -> game_mode[dm_mode_now]			=	dm_mode_pause_retry;	//	処理を PAUSE & RETRY に設定
			dm_retry_flg 							=	0;	//	あらかじめ RETRY 不能に設定しておく
			dm_retry_position[0][0]					=	1;	//	カーソル位置を 終わり に設定

			if( state -> virus_level < 22 || dm_retry_coin > 0 ){	//	LEVEL 21以下 か RETRY 用コインがある場合
				dm_retry_flg 						=	1;	//	RETRY 可能に設定
				dm_retry_position[0][0]				=	0;	//	カーソル位置を RETRY に設定
			}
		}else	if( state -> game_condition[dm_static_cnd] == dm_cnd_pause ){
			auSeqpVolumeInit();						//	音量を戻す
			state -> game_condition[dm_static_cnd]	=	dm_cnd_wait;							//	絶対的コンディションを通常状態にする
			state -> game_condition[dm_mode_now]	=	state -> game_condition[dm_mode_old];	//	PAUSE 前の状態に戻す
			state -> game_mode[dm_mode_now]			=	state -> game_mode[dm_mode_old];		//	PAUSE 前の処理に戻す
		}
	}else	if( flg == dm_ret_clear ){	//	クリア
		dm_play_bgm_set( SEQ_Win );					//	クリア BGM の設定
		story_time_flg 							=	0;							//	タイマーカウント停止
		state -> game_condition[dm_static_cnd]	=	dm_cnd_win;					//	絶対的コンディションを勝利状態にする
		state -> game_condition[dm_mode_now]	=	dm_cnd_stage_clear;			//	状態を STAGE CLEAR に設定
		state -> game_mode[dm_mode_now]			=	dm_mode_stage_clear;		//	処理を STAGE CLEAR に設定

		//	コイン計算
		if( state -> virus_level >= 21 ){	//	LEVEL 21 以上の場合
			if( game_state_data[0].virus_level == 21 ){	//	LEVEL 21 の時
				dm_retry_coin ++;						//	コイン獲得
				dm_set_se( dm_se_get_coin );			//	コイン獲得音
				state -> game_mode[dm_mode_now] 					= dm_mode_get_coin;		//	処理をコイン獲得演出に設定
				dm_retry_coin_pos[dm_retry_coin - 1].flg 			= 1;					//	コインの回転制御方法を変える
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0]	= -1;					//	コインの初期移動方向(上)
				dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1]	= 0;					//	コインの初期移動方向(移動カウンタ)
			}else{
				j = state -> virus_level - 21;			//	コイン獲得のための差分を求める
				if( j % 3 == 0 ){						//	３面クリア
					if( dm_retry_coin < 3 ){			//	現在のコインが２枚以下の場合
						//	コイン獲得
						dm_retry_coin ++;				//	コイン獲得
						dm_set_se( dm_se_get_coin );	//	コイン獲得音
						state -> game_mode[dm_mode_now]			 			= dm_mode_get_coin;	//	処理をコイン獲得演出に設定
						dm_retry_coin_pos[dm_retry_coin - 1].flg 			= 1;	//	コインの回転制御方法を変える
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[0] 	= -1;	//	コインの初期移動方向(上)
						dm_retry_coin_pos[dm_retry_coin - 1].move_vec[1] 	= 0;	//	コインの初期移動方向(移動カウンタ)
					}
				}
			}
		}

		dm_anime_set( &state -> anime,ANIME_win );		//	勝利アニメーションセット
	}else	if( flg == dm_ret_game_over ){	//	ゲームオーバー
		dm_play_bgm_set( SEQ_Lose );		//	GAME OVER 音設定
		dm_anime_set( &state -> anime,ANIME_draw );								//	引き分けアニメーションセット
		state -> virus_anime_spead 				=	v_anime_speed;				//	アニメーション速度を早くする
		state -> game_condition[dm_static_cnd]	=	dm_cnd_lose;				//	絶対的コンディションを負け状態にする
		state -> game_condition[dm_mode_now] 	=	dm_cnd_game_over_retry;		//	状態を GAME OVER & RETRY に設定
		state -> game_mode[dm_mode_now] 		=	dm_mode_game_over_retry;	//	処理を GAME OVER & RETRY に設定
		dm_retry_position[0][0]					=	0;							//	カーソル位置を RETRY に設定
		story_time_flg 							=	0;							//	タイマーカウント停止

		if( state -> virus_level < 22 ){	//	LEVEL 21 以下の場合
			dm_retry_flg = 1;		//	RETRY 可能に設定
		}else	if( state -> virus_level >= 22 ){	//	LEVEL 22 以上の場合
			if( dm_retry_coin > 0 ){	//	RETRY コインを持っていた場合
				dm_retry_flg = 1;		//	RETRY 可能に設定
			}else{
				state -> game_condition[dm_mode_now]	=	dm_cnd_game_over;		//	状態を GAME OVER に設定する
				state -> game_mode[dm_mode_now]			=	dm_mode_game_over;		//	処理を GAME OVER に設定する
				dm_retry_flg = 0;													//	RETRY 不能に設定
			}
		}
	}else	if( flg == dm_ret_next_stage || flg == dm_ret_retry ){	//	次の NEXT STAGE か RETRY の場合
		return	flg;
	}else	if( flg == dm_ret_end ){		//	ゲーム終了
		return	dm_ret_game_over;
	}else	if( flg == dm_ret_game_end ){	//	完全終了
		return	dm_ret_game_end;
	}

	disp_anime_data[0][wb_flag ^ 1] = stat -> anime;	//	描画用データに移植

	return	cap_flg_off;
}
/*--------------------------------------
	ゲーム処理関数(2P用)
--------------------------------------*/
s8	dm_game_main_2p(void)
{
	s8	i,j,l,sound;
	s8	flg[2],game_over_flg,black_up_flg,win_flg;
	s16	center[] = {68,251};
	game_state	*state[2];
	game_map	*map[2];

	game_over_flg = black_up_flg = win_flg = 0;

	//	制御処理
	for( i = 0;i < 2;i++ ){
		state[i]	=	&game_state_data[i];
		map[i]		=	game_map_data[i];

		flg[i] = dm_game_main_cnt( state[i],map[i],i,1 );
		dm_anime_control( &state[i] -> anime );					//	アニメーション
	}
	//	アニメーションの再稼動のカウント
	if( state[0] -> game_condition[dm_static_cnd] != dm_cnd_wait && state[0] -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		//	勝敗が決定していた場合カウントする
		dm_anime_restart_count ++;
		if( dm_anime_restart_count >= ANIME_RESTRT_TIME ){	//	10秒
			//	再稼動
			dm_anime_restart_count = 0;
			for( i = 0;i < 2;i++ ){
				dm_anime_restart( &state[i] -> anime );
			}
		}
	}


	//	ゲーム時間カウント
	if( evs_gamesel == GSL_VSCPU ) {
		if( evs_story_flg ){	//	ストーリーモードの場合
			if( state[0] -> game_condition[dm_static_cnd] == dm_cnd_wait && story_time_flg ){
				if( evs_game_time < DM_MAX_TIME ){	//	99分59秒
					evs_game_time ++;
				}
			}
		}
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait ){
		//	ウイルス設定完了
		if( dm_start_time > 0 ){	//	一定時間のウェイト
			dm_start_time -- ;
		}else{
			j = 1;
			for( i = 0;i < 2;i ++ ){
				if( state[i] -> game_mode[dm_mode_now] != dm_mode_wait ){
					j = 0;	//	まだ配置が終わっていない
					break;
				}
			}
			if( j ){	//	配置完了
				story_time_flg = 1;			//	タイマーカウント開始
				for( i = 0;i < 2;i ++ ){
					state[i] -> game_mode[dm_mode_now] = dm_mode_down;	//カプセル落下開始
					//	落下前思考追加位置
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){	//	CPU の場合
						aifMakeFlagSet( state[i] );
					}
				}
			}
		}
	}else	if( flg[0] == dm_ret_game_over && flg[1] ==  dm_ret_game_over ){
		//	DRAW
		for( i = 0;i < 2;i ++ ){
			state[i] -> game_mode[dm_mode_now]		=	dm_mode_draw;			//	処理を DRAW に設定
			state[i] -> virus_anime_spead 			=	v_anime_speed;			//	アニメーション速度を早くする
			state[i] -> work_flg 					=	16;						//	黒上がりの設定
			state[i] -> retire_flg[dm_retire_flg]	=	1;						//	リタイアフラグを立てる
			if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN の場合
				state[i] -> game_condition[dm_mode_now] = dm_cnd_draw_any_key;	//	DRAW & ANY KEY
			}else{																//	CPU の場合
				state[i] -> game_condition[dm_mode_now] = dm_cnd_draw;			//	DRAW
			}
			dm_anime_set( &state[i] -> anime,ANIME_draw );					//	引き分けアニメーションセット
		}
	}else{
		for( i = 0;i < 2;i++ ){
			if( flg[i] == dm_ret_clear ){
				//	クリア
				win_flg = 1;
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_win;		//	絶対的コンディションを WIN に設定
			}else	if( flg[i] == dm_ret_game_over ){
				//	ゲームオーバー側の設定
				game_over_flg = 1;
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;	//	絶対的コンディションを LOSE に設定
				state[i] -> virus_anime_spead = v_anime_speed;					//	アニメーション速度を早くする
				state[i] -> work_flg = 16;										//	黒上がり処理のカウンタの設定
				state[i] -> retire_flg[dm_retire_flg] = 1;						//	リタイアフラグを立てる
				stop_chain_main(i);												//	連鎖数表示の強制終了

			}else	if( flg[i] == dm_ret_pause ){	//	PAUSE 要求 or PAUSE 解除要求 処理
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){	// 通常状態の場合 PAUSE 要求
					auSeqpVolumeDown();				//	音量減少
					init_pause();					//	PAUSE 処理初期化
					dm_retry_flg = 1;				//	通常はリトライ可能
					for( j = 0;j < 2;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_pause;								//	絶対的コンディションを PAUSE に設定
						state[j] -> game_condition[dm_mode_old]		=	state[j] -> game_condition[dm_mode_now];	//	現在の状態の保存
						state[j] -> game_mode[dm_mode_old]			=	state[j] -> game_mode[dm_mode_now];			//	現在の処理の保存

						if( i == j ){
							//	ポーズを要求した人がリトライ・中断選択権を得る
							state[j] -> game_mode[dm_mode_now]			=	dm_mode_pause_retry;	//	PAUSE & RETRY 処理に設定
							state[j] -> game_condition[dm_mode_now]		=	dm_cnd_pause_re;		//	PAUSE & RETRY 状態に設定
							if( evs_story_flg ){	//	ストーリーモードの場合
								if( !evs_one_game_flg ){	//	ストーリー面セレクト以外
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_pause_re_sc;		//	PAUSE & RETRY & SCORE 状態に設定

									if( dm_peach_stage_flg ){	//	ピーチ姫戦
										dm_retry_flg = 0;				//	リトライ 不能に設定する
										dm_retry_position[i][0] = 1;	//	カーソル位置を 終わるに設定する
									}
								}
							}
						}else{
							state[j] -> game_condition[dm_mode_now]		= 	dm_cnd_pause;			//	PAUSE 状態に設定
							state[j] -> game_mode[dm_mode_now]			= 	dm_mode_pause;			//	PAUSE 処理に設定
						}
					}
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_pause ){	//	PAUSE 状態ならば PAUSE 解除要求

					auSeqpVolumeInit();	//	音量回復
					for( j = 0;j < 2;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;								//	絶対的コンディションを通常状態に設定
						state[j] -> game_condition[dm_mode_now]		=	state[j] -> game_condition[dm_mode_old];	//	前の状態に戻す
						state[j] -> game_mode[dm_mode_now] 			=	state[j] -> game_mode[dm_mode_old];			//	前の処理に戻す
					}
				}
				break;
			}else	if( flg[i] == dm_ret_retry ){
				return	dm_ret_retry;			//	RETRY
			}else	if( flg[i] == dm_ret_end ){	//	終了
				return	-1;
			}else	if( flg[i] == dm_ret_game_end ){	//	完全終了
				return	dm_ret_game_end;
			}
		}
		if( win_flg ){
			dm_retry_flg = 1;			//	通常はリトライ可能
			for( i = j = 0;i < 2;i++ ){	//	クリア人数を調べる
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){
					j ++;
				}
			}
			if( j == 1 ){	// 	クリア人数が １ の場合
				for( i = j = 0;i < 2;i++ ){
					//	サウンドの設定とゲーム終了判定の前設定
					if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){
						win_count[i]++;				//	勝ちポイントの増加
						if( evs_gamesel == GSL_VSCPU ) {
							if( evs_story_flg ){	//	ストーリーモードの場合
								j = 1;				//	終了
								if( i == 0 ){
									sound = SEQ_Win;	//	プレイヤーが勝った場合
								}else{
									sound = SEQ_Lose;	//	プレイヤーが負けた場合
								}
							}else{					//	VSCOM の場合
								if( win_count[i] == WINMAX ){	//	3本取った
									j = 1;
								}
								if( i == 0 ){	//	プレイヤーが勝った場合
									if( j ){	//	３本取得したか？
										sound = SEQ_VSEnd;	//	３本取得の場合
									}else{
										sound = SEQ_Win;	//	１本取得の場合
									}
								}else{
									sound = SEQ_Lose;		//	負け
								}
							}
						}else{	//	VSMAN
							if( win_count[i] == WINMAX ){	//	3本取った
 								j = 1;
 								sound = SEQ_VSEnd;	//	３本取得の場合
							}else{
								sound = SEQ_Win;	//	１本取得の場合
							}
						}
					}
				}
				for( i = 0;i < 2;i++ ){
					if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_win ){	//	勝利者の場合
						win_main( center[i] ,i);									//	勝利演出の初期化
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_win;	//	処理を WIN に設定
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win;		//	表示を WIN に設定
						dm_anime_set( &state[i] -> anime,ANIME_win );				//	勝利アニメーションセット
						if( evs_gamesel == GSL_VSCPU ) {	//	VSCOM or STORY
							if( !evs_story_flg ){	//	ストーリーモード以外の場合
								if( i == 0 ){
									if( j ){
										//	勝敗が付いた場合リトライ受付付きにする
										state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定
										state[i] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定
									}else{
										state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;		//	表示を WIN & ANY KEY に設定
									}
								}
							}else{					//	ストーリーモードの場合
								if( i == 0 ){
									//	プレイヤーだった場合
									if( evs_one_game_flg ){	//	ストーリーEXTRA
										state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定
										state[i] -> game_mode[dm_mode_now]			=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定
									}else{
										state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_any_key_sc;	//	表示を WIN & ANY KEY & SCORE に設定
										if( dm_peach_stage_flg ){	//	ピーチ姫戦{
											if( !evs_secret_flg ){
												dm_peach_get_flg[0] = 1;									//	デモ演出開始(ＧＥＴ ＰＥＡＣＨ)
											}
										}
									}
								}
							}
						}else{	//	VSMAN
							if( j ){
								//	勝敗が付いた場合リトライ受付付きにする
								state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_win_retry;			//	表示を WIN & RETRY に設定
								state[i] -> game_mode[dm_mode_now] 			=	dm_mode_win_retry;			//	処理を WIN & RETRY に設定
							}else{
								state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_win_any_key;			//	表示を WIN & ANY KEY に設定
							}
						}
					}else{	//	敗者の場合

						lose_main( i,center[i] );								//	負け演出の初期化
						dm_anime_set( &state[i] -> anime,ANIME_lose );	//	負けアニメーションセット
						state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;	//	絶対的コンディションを LOSE に設定する
						state[i] -> game_condition[dm_mode_now]		=	dm_cnd_lose;	//	表示を LOSE に設定する
						state[i] -> game_mode[dm_mode_now] 			=	dm_mode_lose;	//	処理を LOSE に設定する

						if( evs_story_flg ){	//	ストーリーモードの場合
							if( i == 0 ){		//	プレイヤーの場合
								state[i] ->	game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
								if( evs_one_game_flg ){	//	ストーリーEXTRA
									state[i] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
								}else{
									state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_retry_sc;	//	表示を LOSE & RETRY & SCORE に設定する
									if( dm_peach_stage_flg ){	//	ピーチ姫戦{
										dm_retry_flg = 0;		//	リトライ不能に設定する
										state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_sc;		//	表示を LOSE & SCORE に設定する
										state[i] ->	game_mode[dm_mode_now]		=	dm_mode_lose;		//	処理を LOSE に設定する
									}
								}
							}
						}else{
							if( evs_gamesel == GSL_VSCPU ) {	//	VSCOM の場合
								if( i == 0 )
								{
									if( j ){	//	３本とっていたら
										//	勝敗が付いた場合リトライ受付付きにする
										state[i] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
										state[i] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
									}else{
										state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	表示を LOSE & ANY KEY にする
									}
								}
							}else{	//	VSMAN の場合
								if( j ){
									//	勝敗が付いた場合リトライ受付付きにする
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
									state[i] -> game_mode[dm_mode_now] 		=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
								}else{
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	表示を LOSE & ANY KEY にする
								}
							}
						}
					}
				}
			}else{	//	引き分け
				sound = SEQ_Lose;
				for( i = 0;i < 2;i++ ){
					draw_main( i,center[i] );												//	DRAW 演出処理の初期化
					state[i] -> game_mode[dm_mode_now]	=	dm_mode_draw;					//	処理を DRAW に設定する
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_draw_any_key;	//	表示を DRAW & ANY KEY に設定する
					}else{
						state[i] -> game_condition[dm_mode_now] =	dm_cnd_draw;			//	表示を DRAW に設定する
					}
					dm_anime_set( &state[i] -> anime,ANIME_draw );	//	DRAW アニメーションセット
				}
			}
			dm_play_bgm_set( sound );	//	鳴らす音楽の設定
		}
		if( game_over_flg ){
			for( i = j = 0;i < 2;i++ ){
				//	サウンドの設定とゲーム終了判定の前設定
				if( state[i] -> game_condition[dm_static_cnd] != dm_cnd_lose ){
					win_count[i]++;
					if( evs_gamesel == GSL_VSCPU ) {
						if( evs_story_flg ){	//	ストーリーモードの場合
							j = 1;				//	終了
							if( i == 0 ){
								sound = SEQ_Win;	//	プレイヤーが勝った場合
							}else{
								sound = SEQ_Lose;	//	プレイヤーが負けた場合
							}
						}else{
							if( win_count[i] == WINMAX ){	//	3本取った
								j = 1;
							}
							if( i == 0 ){	//	プレイヤーが勝った場合
								if( j ){	//	3本取った
									sound = SEQ_VSEnd;
								}else{
									sound = SEQ_Win;
								}
							}else{			//	プレイヤーが負けた場合
								sound = SEQ_Lose;
							}
						}
					}else{
						if( win_count[i] == WINMAX ){	//	3本取った
 							j = 1;
 							sound = SEQ_VSEnd;
						}else{
							sound = SEQ_Win;
						}
					}
				}
			}
			for( i = 0;i < 2;i++ ){
				if( state[i] -> game_condition[dm_static_cnd] != dm_cnd_lose ){		//	勝利者の場合
					win_main( center[i],i );										//	勝利演出の初期化
					state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_win;		//	絶対的コンディションを WIN に設定する
					state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win;		//	表示を WIN に設定する
					state[i] -> game_mode[dm_mode_now]			=	dm_mode_win;	//	処理を WIN に設定する

					dm_anime_set( &state[i] -> anime,ANIME_win );				//	勝利アニメーションセット
					if( evs_gamesel == GSL_VSCPU ) {	// VSCPU & STORY
						if( !evs_story_flg ){	//	ストーリーモード以外の場合
							if( i == 0 ){
								if( j ){
									//	勝敗が付いた場合リトライ受付付きにする
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_retry;	//	表示を WIN & RETRY に設定する
									state[i] -> game_mode[dm_mode_now] 		=	dm_mode_win_retry;	//	処理を WIN & RETRY に設定する
								}else{
									state[i] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key;	//	表示を WIN & ANYKEY に設定する
								}
							}
						}else{		//	ストーリーモードの場合
							if( i == 0 ){
								//	プレイヤーだった場合
								if( evs_one_game_flg ){	//	ストーリーEXTRA
									state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定
									state[i] -> game_mode[dm_mode_now]			=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定
								}else{
									state[i] -> game_condition[dm_mode_now]		=	dm_cnd_win_any_key_sc;	//	表示を WIN & ANY KEY & SCORE に設定
									if( dm_peach_stage_flg ){	//	ピーチ姫戦
										if( !evs_secret_flg ){
											dm_peach_get_flg[0] = 1;												//	デモ演出開始
										}
									}
								}
							}
						}
					}else{	//	VSMAN
						if( j ){
							//	勝敗が付いた場合リトライ受付付きにする
							state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定する
							state[i] -> game_mode[dm_mode_now] 		=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定する
						}else{
							state[i] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;		//	表示を WIN & ANYKEY に設定する
						}
					}
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_lose ){	//	敗者の場合
					lose_main( i,center[i] );									//	負け演出処理の初期化
					dm_anime_set( &state[i] -> anime,ANIME_lose );				//	負けアニメーションセット
					state[i] -> game_condition[dm_mode_now] = dm_cnd_lose;		//	表示を LOSE に設定する
					state[i] -> game_mode[dm_mode_now]		= dm_mode_lose;		//	処理を LOSE に設定する

					if( evs_gamesel == GSL_VSCPU ) {	//	VSCPU の場合
						if( i == 0 )	//	プレイヤーの場合
						{
							if( evs_story_flg ){	//	ストリーモードの場合
								state[i] -> game_mode[dm_mode_now] 		=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
								if( evs_one_game_flg ){	//	ストーリーEXTRA
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;	//	表示を LOSE & RETRY に設定する
								}else{
									state[i] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry_sc;	//	表示を LOSE & RETRY & SCORE に設定する
									if( dm_peach_stage_flg ){	//	ピーチ姫戦{
										dm_retry_flg = 0;		//	リトライ不能に設定する
										state[i] ->	game_condition[dm_mode_now]	=	dm_cnd_lose_sc;	//	表示を LOSE & SCORE に設定する
									}
								}
							}else{	// VSCCOM
								if( j ){
									//	勝敗が付いた場合リトライ受付付きにする
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;	//	表示を LOSE & RETRY に設定する
									state[i] -> game_mode[dm_mode_now]		= dm_mode_lose_retry;	//	処理を LOSE & RETRY に設定する
								}else{
									state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_any_key;	//	表示を LOSE & ANY KEY に設定する
								}
							}
						}
					}else{
						if( j ){
							//	勝敗が付いた場合リトライ受付付きにする
							state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
							state[i] -> game_mode[dm_mode_now] 		= dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
						}else{
							state[i] -> game_condition[dm_mode_now] = dm_cnd_lose_any_key;	//	表示を LOSE & ANY KEY に設定する
						}
					}
				}
			}
			dm_play_bgm_set( sound );	//	BGM の設定
		}
	}
	for(i = 0;i < 2;i++){
		disp_anime_data[i][wb_flag ^ 1] = game_state_data[i].anime;	//	描画用データに移植
	}

	return	cap_flg_off;
}

/*--------------------------------------
	ゲーム処理関数(4P用)
--------------------------------------*/
s8	dm_game_main_4p(void)
{
	s8	i,j,k,l,sound,win_team;
	s8	flg[4],game_over_flg,black_up_flg,win_flg;
	u8	bit,end_flg;
	s16	center[] = {52,124,196,268};
	game_state *state[4];
	game_map *map[4];


	for( i = 0;i < 4;i++ ){
		state[i]	=	&game_state_data[i];
		map[i]		=	game_map_data[i];
		flg[i] 		=	dm_game_main_cnt( state[i],map[i],i,2 );	//	処理
	}

	//	時間計算
	if( evs_story_flg ){	//	ストーリーモードの場合
		for(i = j = 0;i < 4;i++ ){
			if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){
				j ++;
			}
		}
		if( j != 0 && story_time_flg ){
			if( evs_game_time < DM_MAX_TIME ){	//	99分59秒
				evs_game_time ++;
			}
		}
	}

	game_over_flg = black_up_flg = win_flg = 0;	//	フラグクリア

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait &&
		flg[2] == dm_ret_virus_wait && flg[3] == dm_ret_virus_wait  ){

		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			j = 1;
			for( i = 0;i < 4;i ++ ){
				if( state[i] -> game_mode[dm_mode_now] != dm_mode_wait ){
					j = 0;	//	まだ配置が終わっていない
					break;
				}
			}
			if( j ){
				//	ウイルス設定完了
				story_time_flg = 1;	//	タイマーカウント開始
				for( i = 0;i < 4;i ++ ){
					state[i] -> game_mode[dm_mode_now] = dm_mode_down;	//カプセル落下開始
					//	落下前思考追加位置
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
						aifMakeFlagSet( state[i] );
					}
				}
			}
		}
	}else{
		for( i = 0;i < 4;i++ ){
			if( flg[i] == dm_ret_clear ){
				//	クリア
				win_flg = 1;
				state[i] -> game_condition[dm_static_cnd] = dm_cnd_win;		//	絶対的コンディションを WIN に設定する
			}else	if( flg[i] == dm_ret_game_over ){
				//	ゲームオーバー側の設定
				stop_chain_main(i);	//	連鎖数表示の強制終了
				if( !state[i] -> retire_flg[dm_retire_flg] ){	//	リタイア状態じゃない場合
					state[i] -> virus_anime_spead 				=	v_anime_speed_4p;	//	アニメーション速度を早くする
					state[i] -> game_condition[dm_mode_now] 	=	dm_cnd_retire;		//	リタイア状態にする
					state[i] -> game_condition[dm_training_flg] =	dm_cnd_retire;		//	リタイア状態にする
					state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;		//	絶対的コンディションを LOSE に設定する
					state[i] -> work_flg = 16;											//	黒上がり用カウンタのセット
					state[i] -> retire_flg[dm_retire_flg] = 1;							//	リタイアフラグを立てる
					game_over_flg = 1;
					if( state[i] -> player_state[PS_PLAYER] == PUF_PlayerCPU ){			//	ＣＰＵなら練習しない
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_no_action;	//	処理を NO ACTION (何もしない) に設定する
					}else{
						state[i] -> game_condition[dm_mode_now]	=	dm_cnd_tr_chack;	//	表示を練習確認に設定する
						state[i] -> game_mode[dm_mode_now]		=	dm_mode_tr_chaeck;	//	処理を練習確認に設定する
					}
				}else{	//	練習中の場合
					state[i] -> game_condition[dm_mode_now]		=	dm_cnd_tr_chack;	//	表示を練習確認に設定する
					state[i] -> game_mode[dm_mode_now]			=	dm_mode_tr_chaeck;	//	処理を練習確認に設定する
				}
			}else	if( flg[i] == dm_ret_pause ){	//	PAUSE 要求 or PAUSE 解除要求
				if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	通常状態ならば
					auSeqpVolumeDown();				//	ボリュームダウン
					init_pause();					//	ポーズ処理初期化
					dm_retry_flg = 1;				//	リトライ可能に設定する
					for( j = 0;j < 4;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_pause;								//	絶対的コンディションを PAUSE に設定する
						state[j] -> game_condition[dm_mode_old]		=	state[j] -> game_condition[dm_mode_now];	//	現在の状態の保存
						state[j] -> game_mode[dm_mode_old] 			=	state[j] -> game_mode[dm_mode_now];			//	現在の処理の保存
						if( i == j ){	//	ポーズ要求をした人
							state[j] -> game_mode[dm_mode_now]	=	dm_mode_pause_retry;					//	処理を PAUSE & RETRY に設定する
							state[j] -> game_condition[dm_mode_now] = dm_cnd_pause_tar_re;					//	状態を PAUSE & TARGET & RETRY に設定する
							if( evs_story_flg ){
								//	スト^-リーモードの場合
								if( !evs_one_game_flg ){	//	以外ストーリーEXTRA
									state[j] -> game_condition[dm_mode_now]	= dm_cnd_pause_tar_re_sc;		//	状態を PAUSE & TARGET & RETRY & SCORE に設定する
								}
							}
						}else{
							state[j] -> game_condition[dm_mode_now]		= dm_cnd_pause_tar;				//	状態を PAUSE & TARGET に設定する
							state[j] -> game_mode[dm_mode_now]			= dm_mode_pause;				//	処理を PAUSE に 設定する
						}
					}
					break;
				}else	if( state[i] -> game_condition[dm_static_cnd] == dm_cnd_pause ){
					auSeqpVolumeInit();	//	ボリュームを戻す
					for( j = 0;j < 4;j++ ){
						state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;								//	絶対的コンディションを通常状態に設定する
						state[j] -> game_condition[dm_mode_now]		=	state[j] -> game_condition[dm_mode_old];	//	前の状態に戻す
						state[j] -> game_mode[dm_mode_now]			=	state[j] -> game_mode[dm_mode_old];			//	前の処理に戻す
					}
					break;
				}
			}else	if( flg[i] == dm_ret_retry ){	//	リトライ
				return	dm_ret_retry;
			}else	if( flg[i] == dm_ret_game_end ){
				return	dm_ret_game_end;
			}else	if( flg[i] == dm_ret_end ){		//	終了
				return	-1;
			}else	if( flg[i] == dm_ret_tr_a ){	//	練習攻撃無し
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;		//	絶対的コンディションを 通常 に設定する
				state[i] -> retire_flg[dm_training_flg] 	=	0;					//	攻撃（被弾）無しに設定する
				state[i] -> game_condition[dm_training_flg] =	dm_cnd_training;	//	状態を練習に設定する
				state[i] -> game_mode[dm_mode_now]			=	dm_mode_training;	//	処理を練習に設定する
			}else	if( flg[i] == dm_ret_tr_b ){	//	練習攻撃あり
				state[i] -> game_condition[dm_static_cnd]	=	dm_cnd_wait;		//	絶対的コンディションを 通常 に設定する
				state[i] -> retire_flg[dm_training_flg] 	=	1;					//	攻撃（被弾）有りに設定する
				state[i] -> game_condition[dm_training_flg] =	dm_cnd_training;	//	状態を練習に設定する
				state[i] -> game_mode[dm_mode_now]			=	dm_mode_training;	//	処理を練習に設定する
			}
		}
		if( win_flg ){
			//	誰かがクリアした
			for(j = k = l = 0;j < 4;j++ ){
				if( state[j] -> game_condition[dm_static_cnd] == dm_cnd_win ){
					//	クリア人数を調べる
					win_team = state[j] -> player_state[PS_TEAM_FLG];	//	勝利チームの設定
					k ++;
				}
			}

			if( k == 1 ){	//	ひとりだけクリア
				if( evs_story_flg ){	//	ストーリーモードの場合
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){
							//	勝利者と同じチームの物
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	絶対的コンディションを WIN に設定する
							state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;			//	処理を WIN に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){		//	勝者にプレイヤーがいた場合
								sound = SEQ_VSEnd;													//	勝敗確定音の設定
								if( evs_one_game_flg ){	//	ストーリーEXTRA
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;	//	表示を WIN & ANY KEY & SCORE に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	処理を WIN に設定する
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key_sc;	//	表示を WIN & ANY KEY & SCORE に設定する
								}
							}else{
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;				//	表示を	WIN に設定する
							}
							win_main( center[j] ,j );												//	WIN 演出初期化
						}else{
							//	負けチーム
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;			//	絶対的コンディションを LOSE に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	敗者にプレイヤーがいた場合
								//	リトライ受付付き
								sound = SEQ_Lose;													//	敗確定音の設定
								if( evs_one_game_flg ){	//	ストーリーEXTRA
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;	//	表示を LOSE & RETRY  に設定する
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry_sc;	//	表示を LOSE & RETRY & SCORE に設定する
								}
								state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
							}else{
								state[j] -> game_condition[dm_mode_now] = dm_cnd_lose;				//	表示を LOSE に設定する
								state[j] -> game_mode[dm_mode_now]		= dm_mode_lose;				//	処理を LOSE に設定する
							}
							lose_main( j,center[j] );												//	LOSE 演出初期化
						}
					}
				}else{	//	それ以外の場合
					for(j = 0;j < 4;j++){
						if( state[j] -> game_condition[dm_static_cnd] == dm_cnd_win ){
							win_count[ state[j] -> player_state[PS_TEAM_FLG]] ++;				//	勝利チームのカウントを追加する(チーム対抗戦でない場合はチーム番号が別々なので問題なし)
						}
					}
					for(j = end_flg = 0;j < 4;j++ )
					{
						if( win_count[j] == WINMAX ){	//	勝負が付いた( ３本先取 )
							end_flg = 1;				//	勝負が付いたフラグを立てる
							sound = SEQ_VSEnd;
						}else{
							sound = SEQ_Win;
						}
					}

					if( end_flg ){	//	勝敗が決まっていた場合(誰かが3本取った)
						for(j = 0;j < 4;j++ )
						{
							if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	勝利したものと同じチームなら
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN だった場合
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;	//	表示を WIN & RETRY に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;	//	処理を WIN & RETRY に設定する
								}else{																//	CPU だった場合
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win;			//	表示を WIN に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win;		//	処理を WIN に設定する
								}
								win_main( center[j] ,j);											//	WIN 演出初期化
							}else{
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN だった場合

									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_lose_retry;	//	表示を LOSE & RETRY に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose_retry;	//	処理を LOSE & RETRY に設定する
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;		//	表示を LOSE に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_lose;		//	処理を LOSE に設定する
								}
								lose_main( j,center[j] );											//	LOSE 演出初期化
							}
						}
					}else{		//	勝敗が決まっていない場合
						for(j = 0;j < 4;j++ )
						{
							if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	勝利したものと同じチームなら
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN の場合
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key;	//	表示を WIN & ANY KEY に設定する
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;			//	表示を WIN に設定する
								}
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;		//	処理を WIN に設定する
								win_main( center[j],j );											//	WIN 演出初期化
							}else{
								if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){			//	MAN の場合
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;//	表示を LOSE & ANY KEY に設定する
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;		//	表示を LOSE に設定する
								}
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_lose;		//	処理を LOSE に設定する
								lose_main( j,center[j] );											//	LOSE 演出初期化
							}
						}
					}
				}
			}else{	//	多人数の場合は、DRAW
				for(j = 0;j < 4;j++ )
				{
					sound = SEQ_Lose;
					if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN の場合
						state[j] -> game_condition[dm_mode_now]	=	dm_cnd_draw_any_key;	//	表示を DRAW & ANY KEY に設定する
					}else{
						state[j] -> game_condition[dm_mode_now]	=	dm_cnd_draw;			//	表示を DRAW に設定する
					}
					state[j] -> game_mode[dm_mode_now]			=	dm_mode_draw;			//	処理を DRAW に設定する
					draw_main( j,center[j] );												//	DRAW 演出初期化
				}
			}
			dm_play_bgm_set( sound );	//	BGM の設定
		}
		if( game_over_flg ){
			//	ゲームオーバーの知らせが合ったら
			for(j = k = l = bit = 0;j < 4;j++ ){
				if( state[j] -> retire_flg[dm_retire_flg] ){
					//	リタイアしている
					bit |= 1 << j;	//	ビットを立てる
					k ++;
				}
			}
			if( k == 4 ){
				//	全員リタイア(DRAW)
				dm_play_bgm_set( SEQ_Lose );
				for(j = 0;j < 4;j++ )
				{
					if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN の場合
						state[j] -> game_condition[dm_mode_now] = dm_cnd_draw_any_key;	//	表示を DRAW & ANY KEY に設定する
					}else{
						state[j] -> game_condition[dm_mode_now] = dm_cnd_draw;			//	表示を DRAW に設定する
					}
					state[j] -> game_mode[dm_mode_now] = dm_mode_draw;					//	処理を DRAW に設定する
					draw_main( j,center[j] );											//	DRAW 演出初期化
				}
				return	cap_flg_off;
			}else{
				end_flg = 0;			//	勝負が決まっていないに設定しておく
				if( !dm_game_4p_team_flg ){	//	バトルロイヤル( 1P vs 2P vs 3P vs 4P )の場合
					if( k == 3 ){	//	リタイアが ３人のとき
						for(j = 0;j < 4;j++){
							if( !state[j] -> retire_flg[dm_retire_flg] ){	//	リタイアしていないもの
								l = j;
								win_count[ state[j] -> player_state[PS_TEAM_FLG] ] ++;	//	勝ちカウントを増やす
								win_team = state[j] -> player_state[PS_TEAM_FLG];		//	勝利チーム番号の設定
							}
						}
						end_flg = 2;			//	勝負が決まっていないに設定しておく
						for(j = 0;j < 4;j++ )
						{
							if( win_count[j] == WINMAX ){	//	誰かが３本とっていた場合
								end_flg = 1;	//	勝負が決まったに設定する
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Win;
							}
						}
					}
				}else{	//	チーム対抗戦	の場合
					end_flg = 0;
					if( ( bit & dm_game_4p_team_bits[TEAM_MARIO] ) == dm_game_4p_team_bits[TEAM_MARIO] ){	//	Ａチームの全員がリタイアした場合
						win_count[TEAM_ENEMY] ++;						//	ENEMY チームの取得ポイント加算
						win_team = TEAM_ENEMY;							//	勝利チームの設定をする
						end_flg = 2;									//	１試合終了の設定をする
					}else	if( ( bit & dm_game_4p_team_bits[TEAM_ENEMY] ) == dm_game_4p_team_bits[TEAM_ENEMY] ){	//	Ｂチームの全員がリタイアした場合
						win_count[TEAM_MARIO] ++;						//	MARIO チームの取得ポイント加算
						win_team = TEAM_MARIO;							//	勝利チームの設定をする
						end_flg = 2;									//	１試合終了の設定をする
					}

					if( evs_story_flg ){	//	ストーリーモードの場合
						if( end_flg ){	//	end_flg が 0 以外の場合勝負が付いていることになる
							end_flg = 1;					//	試合終了の設定をする
							if( win_count[TEAM_MARIO] ){	//	プレイヤーチームが勝利した場合
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Lose;
							}
						}
					}else{		//	それ以外の場合
						for(j = 0;j < 4;j++ )
						{
							if( win_count[j] == WINMAX ){	//	勝敗が決まった場合
								end_flg = 1;				//	試合終了の設定をする
								sound = SEQ_VSEnd;
							}else{
								sound = SEQ_Win;
							}
						}
					}
				}
				if( end_flg == 1 ){	//	試合終了( 3本先取 )の場合
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	リタイアしていないものと同じチームなら
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	絶対的コンディションを WIN に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN の場合
								if( evs_story_flg ){	//	ストーリーモードの場合
									if( evs_one_game_flg ){	//	ストーリーモードの面セレクトの場合
										state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定する
										state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定する
									}else{
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_win_any_key_sc;	//	表示を WIN & ANY KEY & SCORE に設定する
										state[j] -> game_mode[dm_mode_now]		=	dm_mode_win;			//	処理を WIN に設定する
									}
								}else{
									state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_retry;		//	表示を WIN & RETRY に設定する
									state[j] -> game_mode[dm_mode_now]		=	dm_mode_win_retry;		//	処理を WIN & RETRY に設定する
								}
							}else{														//	CPU の場合
								state[j] -> game_condition[dm_mode_now]		=	dm_cnd_win;				//	表示を WIN に設定する
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_win;			//	処理を WIN に設定する
							}
							win_main( center[j],j );													//	WIN 演出初期化
						}else{
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;				//	絶対的コンディションを LOSE に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){	//	MAN の場合
								if( evs_story_flg ){	//	ストーリーモードの場合
									if( evs_one_game_flg ){	//	ストーリーEXTRA
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
									}else{
										state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry_sc;	//	表示を LOSE & RETRY & SCORE に設定する
									}
								}else{
									state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_retry;		//	表示を LOSE & RETRY に設定する
								}
								state[j] -> game_mode[dm_mode_now] 			=	dm_mode_lose_retry;		//	処理を LOSE & RETRY に設定する
							}else{
								state[j] -> game_condition[dm_mode_now]		=	dm_cnd_lose;			//	表示を LOSE に設定する
								state[j] -> game_mode[dm_mode_now]			=	dm_mode_lose;			//	処理を LOSE に設定する
							}
							lose_main( j,center[j] );													//	LOSE 演出初期化
						}
					}
				}else	if( end_flg == 2 ){	//	1試合終了の場合
					for(j = 0;j < 4;j++ )
					{
						if( state[j] -> player_state[PS_TEAM_FLG] == win_team ){	//	リタイアしていないものと同じチームなら
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_win;				//	絶対的コンディションを WIN に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){				//	MAN の場合
								state[j] -> game_condition[dm_mode_now]	=	dm_cnd_win_any_key;		//	表示を WIN & ANY KEY に設定する
							}else{																	//	CPUの場合
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_win;				//	表示を WIN に設定する
							}
							state[j] -> game_mode[dm_mode_now] 			=	dm_mode_win;			//	処理を WIN に設定する
							win_main( center[j],j );												//	WIN 演出初期化
						}else{
							state[j] -> game_condition[dm_static_cnd]	=	dm_cnd_lose;			//	絶対的コンディションを LOSE に設定する
							if( state[j] -> player_state[PS_PLAYER] == PUF_PlayerMAN ){				//	MANの場合
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose_any_key;	//	表示を LOSE & ANY KEY に設定する
							}else{																	//	CPUの場合
								state[j] -> game_condition[dm_mode_now] =	dm_cnd_lose;			//	表示を LOSE に設定する
							}
							state[j] -> game_mode[dm_mode_now]			= dm_mode_lose;				//	処理を LOSE に設定する
							lose_main( j,center[j] );
						}
					}
				}

				if( end_flg != 0 ){
					dm_play_bgm_set( sound );
				}
				return	cap_flg_off;
			}
		}
	}

	return	cap_flg_off;
}


/*--------------------------------------
	ゲームデモ処理関数(1P用)
--------------------------------------*/
s8	dm_game_demo_1p(void)
{
	s8	i,flg;

	flg = dm_game_main_cnt_1P( &game_state_data[0],game_map_data[0],0,0 );
	dm_anime_control( &game_state_data[0].anime );	//	アニメーション
	for(i = 0;i < 3;i++){
		dm_anime_control( &big_virus_anime[i] );	//	巨大ウイルスアニメーション
	}


	if( flg == dm_ret_virus_wait ){	//	ウイルス配置完了

		if( dm_start_time > 0 ){
			dm_start_time --;
		}else{
			dm_demo_time_flg = 1;
			game_state_data[0].game_mode[dm_mode_now] = dm_mode_throw;	//カプセル落下開始
			if( game_state_data[0].player_state[0] == PUF_PlayerCPU ){
				aifMakeFlagSet( &game_state_data[0] );
			}
		}
	}
	disp_anime_data[0][wb_flag ^ 1] = game_state_data[0].anime;	//	描画用データに移植

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	１分
			return	cap_flg_on;		//	デモ中断
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	デモ中断
			}
		}
	}

	return	cap_flg_off;
}

/*--------------------------------------
	ゲームデモ処理関数(2P用)
--------------------------------------*/
s8	dm_game_demo_2p(void)
{
	s8	i,j,l;
	s8	flg[2];


	for( i = 0;i < 2;i++ ){
		flg[i] = dm_game_main_cnt( &game_state_data[i],game_map_data[i],i,1 );
		dm_anime_control( &game_state_data[i].anime );					//	アニメーション
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait ){

		//	ウイルス設定完了
		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			dm_demo_time_flg = 1;
			for( i = 0;i < 2;i ++ ){
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;	//カプセル落下開始
				//	落下前思考追加位置
				if( game_state_data[i].player_state[0] == PUF_PlayerCPU ){
					aifMakeFlagSet( &game_state_data[i] );
				}
			}
		}
	}
	for(i = 0;i < 2;i++){
		disp_anime_data[i][wb_flag ^ 1] = game_state_data[i].anime;	//	描画用データに移植
	}

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	１分
			return	cap_flg_on;		//	デモ中断
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	デモ中断
			}
		}
	}

	return	cap_flg_off;
}
/*--------------------------------------
	ゲームデモ処理関数(4P用)
--------------------------------------*/
s8	dm_game_demo_4p(void)
{
	u8	i;
	s8	flg[4];

	for( i = 0;i < 4;i++ ){
		flg[i] = dm_game_main_cnt( &game_state_data[i],game_map_data[i],i,2 );
	}

	if( flg[0] == dm_ret_virus_wait && flg[1] == dm_ret_virus_wait &&
		flg[2] == dm_ret_virus_wait && flg[3] == dm_ret_virus_wait  ){

		if( dm_start_time > 0 ){
			dm_start_time -- ;
		}else{
			//	ウイルス設定完了
			dm_demo_time_flg = 1;
			for( i = 0;i < 4;i ++ ){
				game_state_data[i].game_mode[dm_mode_now] = dm_mode_down;	//カプセル落下開始
				//	落下前思考追加位置
				if( game_state_data[i].player_state[0] == PUF_PlayerCPU ){
					aifMakeFlagSet( &game_state_data[i] );
				}
			}
		}
	}

	if( dm_demo_time_flg ){
		dm_demo_time ++;
		if( dm_demo_time > 1800 ){	//	１分
			return	cap_flg_on;		//	デモ中断
		}
		for( i = 0;i < 4;i++ ){
			if( joyupd[i] & DM_ANY_KEY ){
				return	cap_flg_on;		//	デモ中断
			}
		}
	}

	return	cap_flg_off;

}
/*--------------------------------------
	ゲーム描画関数マップ描画
--------------------------------------*/
void	dm_map_draw(game_map *map,u8 col_no,s16 x_p,s16 y_p,s8 size)
{
	u8	i,j;

	for( i = 0;i < 128;i++ ){
		if( (map + i) -> capsel_m_flg[cap_disp_flg] ){
			//	ウイルス(カプセル)の表示フラグがＯＮのとき
			if( (map + i) -> capsel_m_p == col_no ){
				//	色（Ｒ・Ｂ・Ｙ）が等しいとき
				gSPTextureRectangle(gp++,(map + i) -> pos_m_x * size + x_p << 2,(map + i) -> pos_m_y * size + y_p << 2,
												(map + i) -> pos_m_x * size + x_p + size << 2,(map + i) -> pos_m_y * size + y_p + size << 2,
														G_TX_RENDERTILE, 0, (map + i) -> capsel_m_g * size << 5,1 << 10,1 << 10);
			}
		}
	}
}
/*--------------------------------------
	(勝利数)描画関数
--------------------------------------*/
void	dm_star_draw(s16 x_pos ,s16 y_pos, u8 win ,s8 flg)
{

	s8 i;

	if( !flg ){	//	横ならべ
		for( i = 0;i < win && i < 3;i++ ){
			draw_Tex( x_pos,y_pos,16,16,0,win_anime_count[0] << 4);
			x_pos += 22;
		}
	}else	if( flg ){	//	縦ならべ
		for( i = 0;i < win && i < 3;i++ ){
			draw_Tex( x_pos,y_pos,16,16,0,win_anime_count[0] << 4);
			y_pos += 20;
		}
	}
}


/*--------------------------------------
	投げ状態カプセル左回転関数
--------------------------------------*/
void	throw_rotate_capsel( game_cap *cap )
{
	s8	vec;
	u8	i,save;
	u8	rotate_table[] = {1,3,4,2};
	u8	rotate_mtx[] = {capsel_l,capsel_u,capsel_r,capsel_d,capsel_l,capsel_u};

	if( cap -> pos_x[0] == cap -> pos_x[1] ){	//	カプセルが縦になっていた場合
		cap -> pos_x[1] ++;
		cap -> pos_y[1] ++;
		save = cap->capsel_p[0];
		cap->capsel_p[0] = cap->capsel_p[1];
		cap->capsel_p[1] = save;
		vec = 1;
	}else{	//	カプセルが横になっていた場合
		//	座標変更
		cap -> pos_x[1] --;
		cap -> pos_y[1] --;
		vec = -1;
	}
	//	グラフィックの入れ替え
	for(i = 0;i < 2;i++){
		save = rotate_table[cap -> capsel_g[i]];
		save += vec;
		cap -> capsel_g[i] = rotate_mtx[save];
	}
}

/*--------------------------------------
	ウイルス数描画関数
--------------------------------------*/
void	dm_disp_virus_number( game_state *state,s16 x_pos, s16 y_pos,s8 player_no )
{
	s8	i;
	u8	fspd[4] = { 2,8,6,4};
	float	fa;

	//	数字パレットデータの読込み
	if( state -> virus_number < 4 ){
		load_TexPal( TLUT_virus_number_r );	//	赤い色
	}else{
		load_TexPal( TLUT_virus_number );	//	通常色
	}
	if( state -> virus_number < 10 ){	//	ウイルスの数が１桁
		x_pos += 6;
		if( state -> virus_number > 3 ){	//	４個以上は通常表示
			virsu_num_size[player_no][0] = 12;
			virsu_num_size[player_no][1] = 16;
			virsu_num_size[player_no][2] = 1;
		}else{								//	３個以下は拡縮表示
			virsu_num_size[player_no][2] += fspd[state -> virus_number];
			if( virsu_num_size[player_no][2] > 360 ){
				virsu_num_size[player_no][2] -= 360;
			}
			fa = sinf( DEGREE( virsu_num_size[player_no][2] ) );
			virsu_num_size[player_no][0] = (s16)(16 + (fa * 5 ));
			virsu_num_size[player_no][1] = (s16)(16 + (fa * 5 ));

			for( i = 0;i < 2;i++ ){
				if( virsu_num_size[player_no][i] % 2 ){
					virsu_num_size[player_no][i] ++;
				}
			}

			x_pos += 6;
			x_pos -= virsu_num_size[player_no][0] >> 1;
			y_pos += 8;
			y_pos -= virsu_num_size[player_no][1] >> 1;
		}
		draw_ScaleTex( x_pos,y_pos,12,16,virsu_num_size[player_no][0],virsu_num_size[player_no][1],0,state -> virus_number << 4 );
	}else{
		draw_Tex( x_pos,y_pos,12,16,0,(state -> virus_number / 10) << 4 );
		draw_Tex( x_pos + 12,y_pos,12,16,0,(state -> virus_number % 10) << 4 );
	}
}
/*--------------------------------------
	数字描画関数
--------------------------------------*/
void	dm_disp_number( s16 num,s16 x_pos, s16 y_pos ,u16 *pal)
{
	s8	i;

	//	数字グラフィックデータは読み込んであるものとする

	//	数字パレットデータの読込み
	load_TexPal( pal );

	if( num < 10 ){
		x_pos += 6;
		draw_Tex( x_pos,y_pos,12,16,0,num << 4 );
	}else{
		draw_Tex( x_pos,y_pos,12,16,0,(num / 10) << 4 );
		draw_Tex( x_pos + 12,y_pos,12,16,0,(num % 10) << 4 );
	}
}

/*--------------------------------------
	得点描画関数
--------------------------------------*/
void	dm_disp_score( u32	score,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[7];

	//	数字パレットデータの読込み
	load_TexPal( lv_num_bm0_0tlut );
	//	数字グラフィックデータの読込み
	load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );

	num[6] = score / 1000000;
	num[5] = (score % 1000000) / 100000;
	num[4] = (score % 100000) / 10000;
	num[3] = (score % 10000) / 1000;
	num[2] = (score % 1000) / 100;
	num[1] = (score % 100) / 10;
	num[0] = score % 10;

	for( i = 0;i < 7;i++ ){
		x_pos -= 8;
		draw_Tex( x_pos,y_pos,8,12,0,num[i] * 12 );
	}
}
/*--------------------------------------
	SCORE 文字付き得点描画関数
--------------------------------------*/
void	dm_disp_score_plus( game_state *state )
{
	u8	x_p,y_p;
	u8	s_xp,s_yp;

	if( evs_gamesel == GSL_4PLAY ){
		x_p = 14;
		y_p = 10;
		s_xp = 60;
		s_yp = 21;
	}else{
		x_p = 21;
		y_p = 12;
		s_xp = 68;
		s_yp = 24;
	}


	//	SCORE
	load_TexPal( dm_4p_game_target_base_bm0_0tlut );
	load_TexTile_4b( dm_4p_game_score_bm0_0,36,10,0,0,35,9 );
	draw_Tex( state -> map_x + x_p,state -> map_y + y_p,36,10,0,0 );

	//	得点
	dm_disp_score( state -> game_score,state -> map_x + s_xp,state -> map_y + s_yp );
}
/*--------------------------------------
	ポイント描画関数
--------------------------------------*/
void	dm_disp_point( s16	point,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[3];

	if( point > 999 ){
		point = 999;
	}
	num[2] = point / 100;
	num[1] = (point % 100) / 10;
	num[0] = point % 10;

	for( i = 0;i < 3;i++ ){
		x_pos -= 8;
		draw_Tex( x_pos,y_pos,8,12,0,num[i] * 12 );
	}
}
/*--------------------------------------
	加算得点描画関数
--------------------------------------*/
void	dm_disp_add_score( void )
{
	s8	i,j;


	gDPSetRenderMode( gp++, G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2);
	for( i = 0;i < 10;i++ ){
		if( lv_sc_pos[i].flg ){	//	使用中の場合
			for( j = 0;j < lv_sc_pos[i].num_max;j++ )
			{
				//	透明度の設定
				if( lv_sc_pos[i].num_alpha[j] ){	//	0以外のとき
					gDPSetPrimColor( gp++,0,0,255,255,255,lv_sc_pos[i].num_alpha[j] );
					draw_Tex( lv_sc_pos[i].score_pos[j][0],66 - lv_sc_pos[i].score_pos[j][1],8,12,0,lv_sc_pos[i].num_g[j] * 12 );
				}
			}
		}
	}
	gDPPipeSync(gp++);
	gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
	gDPSetPrimColor(gp++,0,0,255,255,255,255);

}


/*--------------------------------------
	時間描画関数
--------------------------------------*/
void	dm_disp_time( u32 time,s16 x_pos, s16 y_pos )
{
	s8	i,j;
	s8 num[6];

	//	数字グラフィックデータは読み込んであるものとする

	//	数字パレットデータの読込み
	load_TexPal( TLUT_virus_number );

	num[5] = time / 3600;
	num[4] = (time % 3600) / 60;

	num[3] = num[5] / 10;
	num[2] = num[5] % 10;
	num[1] = num[4] / 10;
	num[0] = num[4] % 10;

	x_pos -= 12;
	//	秒
	for( i = 0;i < 2;i++ ){
		draw_Tex( x_pos,y_pos,12,16,0,num[i] << 4 );
		x_pos -= 12;
	}
	x_pos -= 6;
	//	分
	for( i = 2;i < 4;i++ ){
		draw_Tex( x_pos,y_pos,12,16,0,num[i] << 4 );
		x_pos -= 12;
	}
}

/*--------------------------------------
	ストーリー４Ｐ時用 時間・レベル・ステージ 描画関数
--------------------------------------*/
void	dm_disp_time_4p( u32 time,u8 level )
{
	s8	i,j;
	s8 num[6];
	s16	x_pos;

	//	数字パレットデータの読込み
	load_TexPal( dm_4p_game_level_bm0_0tlut );

	//	レベル描画
	load_TexTile_4b( &dm_4p_game_level_bm0_0[0],28,15,0,0,27,14 );
	draw_Tex( 194,30,28,5,0,level * 5);

	//	数字グラフィックデータの読込み
	load_TexTile_4b( &dm_story_num_bm0_0[0],4,55,0,0,3,54 );

	//	ステージ描画
	draw_Tex( 160,30,4,5,0,7 * 5 );

	//	時間描画
	draw_Tex( 113,30,4,5,0,10 * 5 );	//	コンマ

	num[5] = time / 3600;
	num[4] = (time % 3600) / 60;

	num[3] = num[5] / 10;
	num[2] = num[5] % 10;
	num[1] = num[4] / 10;
	num[0] = num[4] % 10;

	//	秒
	x_pos = 122;
	for( i = 0;i < 2;i++ ){
		draw_Tex( x_pos,30,4,5,0,num[i] * 5 );
		x_pos -= 5;
	}
	x_pos -= 4;
	//	分
	for( i = 2;i < 4;i++ ){
		draw_Tex( x_pos,30,4,5,0,num[i] * 5 );
		x_pos -= 5;
	}
}


/*--------------------------------------
	4PLAY 用顔描画関数
--------------------------------------*/
void	dm_4p_game_face_draw( game_state *state,s16 x_pos,s16 y_pos )
{
	//	顔パレットデータの読込み
	load_TexPal( face_4p_game_pal_data[state -> anime.cnt_charcter_no] );

	//	顔データの読込み
	load_TexBlock_4b( face_4p_game_data[state -> anime.cnt_charcter_no],16,16 );

	draw_Tex( x_pos,y_pos,16,16,0,0 );
}

/*--------------------------------------
	1PLAY 用速度描画関数
--------------------------------------*/
void	dm_speed_draw( game_state *state,s16 x_pos,s16 y_pos )
{
	//	SPEED
	load_TexPal( dm_data_mode_game_spead_bm0_0tlut );
	load_TexTile_4b( dm_data_mode_game_spead_bm0_0, 24,48,0,0,23,47 );

	draw_Tex( x_pos,y_pos,24,16,0,(state -> cap_def_speed) << 4 );
}

/*--------------------------------------
	ゲーム描画関数
--------------------------------------*/
void	dm_game_graphic_p( game_state *state, game_map *map )
{
	s16	i,j,x_p,y_p;
	s8	size_flg = 0;
	u8	cap_size[] = {220,128};

	//	ポーズ中は描画しない
	if( state -> game_condition[dm_static_cnd] != dm_cnd_pause ){
		if( state -> map_item_size == cap_size_8 ){
			size_flg = 1;
		}

		//	ウイルスグラフィックデータの読込み
		load_TexTile_4b( cap_tex[size_flg],16,cap_size[size_flg],0,0,15,cap_size[size_flg] - 1 );


#ifndef	DM_S2D_MAIN

		//	ウイルス描画
		for( i = 0;i < 6;i++ ){
			//	ウイルスパレットデータの読込み
			load_TexPal( cap_pal[size_flg][i] );
			dm_map_draw( map ,i,state -> map_x,state -> map_y,state -> map_item_size );
		}
#endif

		//	操作カプセル描画(ここで描かれるのは、ゲームの決着が付いたときのみ)
		if( state -> now_cap.capsel_flg[cap_disp_flg] ){	//	表示フラグが立っているか？
			//	立っていた場合
			if( state -> now_cap.pos_y[0] < 1 ){
				for( i = 0;i < 2;i++ ){
					//	パレットロード
					load_TexPal( cap_pal[size_flg][state -> now_cap.capsel_p[i]] );
					//	カプセル描画
					draw_Tex( (state -> now_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
							(state -> now_cap.pos_y[i] * state -> map_item_size) + state -> map_y - 10,
								state -> map_item_size,state -> map_item_size,0,state -> now_cap.capsel_g[i] * state -> map_item_size );
				}
			}else	if( state -> game_condition[dm_mode_now] != dm_cnd_pause && state -> game_condition[dm_mode_now] != dm_cnd_wait ){
				for( i = 0;i < 2;i++ ){
					//	パレットロード
					load_TexPal( cap_pal[size_flg][state -> now_cap.capsel_p[i]] );
					//	カプセル描画
					draw_Tex( (state -> now_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
							(state -> now_cap.pos_y[i] * state -> map_item_size) + state -> map_y + 1,
								state -> map_item_size,state -> map_item_size,0,state -> now_cap.capsel_g[i] * state -> map_item_size );
				}
			}
		}

		//	NEXTカプセル描画
		if( state -> next_cap.capsel_flg[cap_disp_flg] ){	//	表示フラグが立っているか？
			//	立っていた場合
			if( state -> now_cap.pos_y[0] > 0 ){	//	操作カプセルがNEXTの高さにあるか？
				//	落下していた
				for( i = 0;i < 2;i++ ){
					//	パレットロード
					load_TexPal( cap_pal[size_flg][state -> next_cap.capsel_p[i]] );
					//	カプセル描画
					draw_Tex( (state -> next_cap.pos_x[i] * state -> map_item_size) + state -> map_x,
								(state -> next_cap.pos_y[i] * state -> map_item_size) + state -> map_y - 10,
									state -> map_item_size,state -> map_item_size,0,state -> next_cap.capsel_g[i] * state -> map_item_size );
				}
			}
		}
	}
}

/*--------------------------------------
	ゲーム描画関数(1P用)
--------------------------------------*/
void	dm_game_graphic_1p( game_state *state, game_map *map )
{
	s16	i,j,x_p,y_p;


	//	ポーズ中は描画しない
	if( state -> game_condition[dm_static_cnd] != dm_cnd_pause ){

		//	ウイルスグラフィックデータの読込み
		load_TexTile_4b( &TEX_mitem10r_0_0[0],16,220,0,0,15,219 );
#ifndef	DM_S2D_MAIN
		//	ウイルス描画
		for( i = 0;i < 3;i++ ){
			//	ウイルスパレットデータの読込み
			load_TexPal(capsel_pal[i]);
			dm_map_draw( map ,i,state -> map_x,state -> map_y,state -> map_item_size );
		}

#endif
		//	操作カプセル描画
		if( state -> now_cap.capsel_flg[cap_disp_flg] ){	//	表示フラグが立っているか？
			//	第１投のときだけ
			if( state -> game_mode[dm_mode_now] == dm_mode_wait ){
				for( i = 0;i < 2;i++ ){
					load_TexPal( capsel_pal[state -> now_cap.capsel_p[i]] );	//	パレットロード
					//	カプセル描画

					draw_Tex( i * cap_size_10 + 222,77,
								cap_size_10,cap_size_10,0,state -> now_cap.capsel_g[i] * cap_size_10);
				}
			}
		}

		//	NEXTカプセル描画
		if( state -> next_cap.capsel_flg[cap_disp_flg] ){	//	表示フラグが立っているか？
			//	立っていた場合
			if( state -> now_cap.pos_y[0] > 0 ){	//	操作カプセルがNEXTの高さにあるか？
				//	落下していた
				for( i = 0;i < 2;i++ ){
					load_TexPal( capsel_pal[state -> next_cap.capsel_p[i]] );	//	パレットロード
					//	カプセル描画

					draw_Tex( i * cap_size_10 + 222,77,
								cap_size_10,cap_size_10,0,state -> next_cap.capsel_g[i] * cap_size_10);
				}
			}
		}

	}
}
/*--------------------------------------
	リトライ描画関数
--------------------------------------*/
void	retry_draw( game_state *state ,s8 palyer_no )
{
	u8	allow_pos[] = {4,18};
	u8	x_p,y_p;

	if( evs_gamesel == GSL_4PLAY ){
		x_p = 2;
		y_p = 153;
	}else{
		x_p = 10;
		y_p = 180;
	}

	gSPDisplayList(gp++, prev_halftrans_Ab_dl );

	//	リトライプレートの描画
	if( dm_retry_flg ){
		//	リトライ可能
		load_TexPal( dm_retry_bm0_0tlut );
	}else{
		//	リトライ不能
		load_TexPal( dm_no_retry_bm0_0tlut );
	}
	load_TexTile_4b( dm_retry_bm0_0,60,34,0,0,59,33 );
	draw_Tex( state -> map_x + x_p,y_p,60,34,0,0 );

//	dm_retry_position

	//	矢印の描画
	gDPSetPrimColor( gp++,0,0,dm_retry_alpha[0],dm_retry_alpha[0],dm_retry_alpha[0],255);
	load_TexPal( arrow_r_bm0_0tlut );
	load_TexTile_4b( arrow_r_bm0_0,8,11,0,0,7,10 );
	draw_Tex( state -> map_x + x_p + 2 + dm_retry_pos[0],y_p + allow_pos[dm_retry_position[palyer_no][0]],8,11,0,0 );
}
/*--------------------------------------
	４Ｐ時の顔処理関数
--------------------------------------*/
void	demo_story_4p_main(void)
{
	s8	i,j;

	switch( story_4p_demo_flg[1] )
	{
	case	0:	//	拡大
		story_4p_demo_w[0] += 10;
		if( story_4p_demo_w[0] >= 192 ){
			story_4p_demo_w[0] = 192;
			story_4p_demo_flg[1] = 1;
		}
		S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,story_4p_demo_w[0],58,159 - (story_4p_demo_w[0] / 2 ),102,0,0 );
		break;
	case	1:	//	キー待ち
		for(i = 0;i < 4;i++){
			if( joyupd[i] & DM_ANY_KEY ){
				story_4p_demo_h[0] = 58;
				story_4p_demo_flg[1] = 2;
			}
		}
		break;
	case	2:	//	縮小
		story_4p_demo_h[0] -= 8;
		if( story_4p_demo_h[0] <= 2 ){
			story_4p_demo_h[0] = 2;
			story_4p_demo_flg[1] = 0;	//	処理終了
			story_4p_demo_flg[0] = 0;	//	処理終了
		}
		S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,192,story_4p_demo_h[0],63,131 - (story_4p_demo_h[0] / 2),0,0 );
		break;
	}
}

/*--------------------------------------
	４Ｐ時の顔描画関数
--------------------------------------*/
void	demo_story_4p_draw(void)
{
	s8	i,j;
	s16	w_size;

	//	台座
	load_TexPal( story_4p_demo_bg_tlut );
 	gSPBgRect1Cyc( gp++, &s2d_demo_4p );

	//	PUSH ANY KEY
	if( story_4p_demo_flg[1] == 1 ){
		push_any_key_draw( 128 ,165 );

		load_TexPal( dm_4p_story_gage_bm0_0tlut );	//	中央部 パレットロード
		load_TexTile_4b( &dm_4p_story_gage_bm0_0[0],154,24,0,0,153,23 );	//	中央部 グラフィックロード

		//	台座描画
		for(i = 0;i < 4;i++ ){
			draw_Tex( story_4p_demo_face_pos[i],124,24,24,130,0);
		}

		//	顔描画
		for(i = j = 0;i < 4;i++ ){
			if( !game_state_data[i].player_state[PS_TEAM_FLG] ){
				//	顔パレットデータの読込み
				load_TexPal( face_4p_game_pal_data[game_state_data[i].anime.cnt_charcter_no] );
				//	顔データの読込み
				load_TexBlock_4b( face_4p_game_data[game_state_data[i].anime.cnt_charcter_no],16,16 );
				draw_TexFlip( story_4p_demo_face_pos[j] + 5,127,16,16,0,0,flip_on,flip_off);
				j++;
			}
		}
		//	顔描画
		for(i = 0;i < 4;i++ ){
			if( game_state_data[i].player_state[PS_TEAM_FLG] ){
				//	顔パレットデータの読込み
				load_TexPal( face_4p_game_pal_data[game_state_data[i].anime.cnt_charcter_no] );
				//	顔データの読込み
				load_TexBlock_4b( face_4p_game_data[game_state_data[i].anime.cnt_charcter_no],16,16 );
				draw_Tex( story_4p_demo_face_pos[j] + 5,127,16,16,0,0);
				j++;
			}
		}

		//	VS
		if( evs_story_flg ){
			load_TexPal( dm_4p_game_face_back_vs_b_bm0_0tlut );
		}else{
			load_TexPal( dm_4p_team_face_back_vs_bm0_0tlut );
		}
		load_TexBlock_4b( &dm_4p_game_face_back_vs_bm0_0[0],16,10);
		draw_Tex( story_4p_demo_face_pos[4],130,16,10,0,0);


	}
}
/*--------------------------------------
	攻撃相手描画関数
--------------------------------------*/
void	draw_target( game_state *state,u8 player_no )
{
	s8	i;
	u8	target[4][3] = {
		{1,2,3},
		{2,3,0},
		{3,0,1},
		{0,1,2},
	};

	//	台座描画
	load_TexPal( dm_4p_game_target_base_bm0_0tlut );
	load_TexTile_4b( dm_4p_game_target_base_bm0_0,60,36,0,0,59,35 );
	draw_Tex( state -> map_x + 2,115,60,36,0,0);

	//	攻撃相手
	load_TexTile_4b( dm_4p_game_target_p_bm0_0 ,56,10,0,0,55,9 );
	for(i = 0;i < 3;i++){
		draw_Tex( state -> map_x + 6 + i * 19,136,14,10,target[player_no][i] * 14,0);
	}

	//	攻撃相手カプセル
	load_TexTile_4b( cap_tex[1],16,128,0,0,15,128 - 1 );
	for(i = 2;i >= 0;i--){
		load_TexPal( capsel_8_pal[i] );
		draw_Tex( state -> map_x + 43 - (i * 19),120,8,8,0,capsel_l * 8);
		draw_Tex( state -> map_x + 51 - (i * 19),120,8,8,0,capsel_r * 8);
	}

	//	矢印の描画
	gDPSetPrimColor( gp++,0,0,dm_retry_alpha[0],dm_retry_alpha[0],dm_retry_alpha[0],255);
	load_TexPal( arrow_d_bm0_0tlut );
	load_TexTile_4b( arrow_d_bm0_0,16,7,0,0,15,6 );
	for(i = 0;i < 3;i++){
		if( dm_retry_pos[3] == i ){
			draw_Tex( state -> map_x + 7 + i * 19,129,16,7,0,0 );
		}else{
			draw_Tex( state -> map_x + 7 + i * 19,128,16,7,0,0 );
		}
	}

}
/*--------------------------------------
	ゲーム描画関数(演出関係用)
--------------------------------------*/
void	dm_game_graphic_effect( game_state *state,u8 player_no,u8 type )
{
	s16	i,j;

	switch( state -> game_condition[dm_training_flg] )
	{
	case	dm_cnd_retire:			//	リタイア
		load_TexPal( TLUT_retire );
		load_TexBlock_8b( &TEX_retire_0_0[0],56,29 );
		draw_Tex( state -> map_x + 4,201,56,29,0,0 );
		break;
	case	dm_cnd_training:		//	練習中
		load_TexPal( TLUT_training );
		load_TexBlock_8b( &TEX_training_0_0[0],56,29 );
		draw_Tex( state -> map_x + 4,201,56,29,0,0 );
		break;
	}

	switch( state -> game_condition[dm_mode_now] )
	{
	case	dm_cnd_stage_clear:		//	TRY NEXT STAGE
		try_next_stage_draw( 2 );
		push_any_key_draw( 128,200 );
		break;
	case	dm_cnd_game_over:		//	GAME OVER
		game_over_draw( 160 );
		push_any_key_draw( 128,200 );
		break;
	case	dm_cnd_game_over_retry:	//	GAME OVER & RETRY
		game_over_draw( 160 );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_win_retry:		//	WIN & RETRY
		retry_draw( state, player_no );
		win_draw( player_no );
		break;
	case	dm_cnd_win_any_key:		//	WIN & PUSH ANY KEY
		win_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_win_any_key_sc:	//	WIN & PUSH ANY KEY & SCORE
		dm_disp_score_plus( state );
		win_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_win:				//	WIN
		win_draw( player_no );
		break;
	case	dm_cnd_lose_retry:		//	LOSE & RETRY
		retry_draw( state, player_no );
		lose_draw( player_no );
		break;
	case	dm_cnd_lose_any_key:	//	LOSE & PUSH ANY KEY
		lose_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_lose_retry_sc:	//	LOSE & RETRY & SCORE
		dm_disp_score_plus( state );
		retry_draw( state, player_no );
		lose_draw( player_no );
		break;
	case	dm_cnd_lose_sc:			//	LOSE & PUSH ANY KEY & SCORE
		dm_disp_score_plus( state );
		lose_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_lose:			//	LOSE
		lose_draw( player_no );
		break;
	case	dm_cnd_draw_any_key:	//	DRAW & PUSH ANY KEY
		draw_draw( player_no );
		push_any_key_draw( (state -> map_x + (state -> map_item_size << 2))  - 32,(state -> map_y + (state -> map_item_size << 4)) - 6 );
		break;
	case	dm_cnd_draw:			//	DRAW
		draw_draw( player_no );
		break;
	case	dm_cnd_tr_chack:		//	練習確認(仮)
		load_TexPal( TLUT_mes06 );
		load_TexBlock_4b( &TEX_mes06_0_0[0],64,64 );
		draw_Tex( state -> map_x,100,64,64,0,0 );
		break;
	case	dm_cnd_pause:			//	PAUSE
		pause_draw( player_no );
		break;
	case	dm_cnd_pause_tar:	//	PAUSE & TARGET
		pause_draw( player_no );
		draw_target( state,player_no );
		break;
	case	dm_cnd_pause_re:		//	PAUSE & RETRRY
		pause_draw( player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_re_sc:		//	PAUSE & RETRRY & SCORE
		dm_disp_score_plus( state );
		pause_draw( player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_tar_re:		//	PAUSE & RETRRY & TARGET
		pause_draw( player_no );
		draw_target( state,player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_pause_tar_re_sc:	//	PAUSE & RETRRY & TARGET & SCORE
		dm_disp_score_plus( state );
		pause_draw( player_no );
		draw_target( state,player_no );
		retry_draw( state,player_no );
		break;
	case	dm_cnd_wait:			//	通常状態
	case	dm_cnd_manual:			//	操作説明
		bubble_draw( state ,player_no );		//	攻撃演出
//		draw_chaine( player_no,type );	//	連鎖表示
		shock_draw( player_no );		//	ダメージ
		humming_draw( player_no );		//	ハミング
	}

	//	描画範囲を戻す
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	半透明設定
	gSPDisplayList(gp++, Texture_TE_dl );

}

/*--------------------------------------
	操作カプセル描画関数
--------------------------------------*/
void	dm_draw_capsel( game_state *state ,u8 player_no )
{
	game_cap *n_cap;
	int		z;
	int		gg, hh=0;
	int		a,b,c;
	u32		xx[2],yy[2],zz[2],i,j,k,m,n,p,q;
	float	aa,bb,cc,dd,ee,ff,ii;
	u8		curve = 36,disp_flg = 0;
	u8*		tex;
	u16*	pal;


	if( state -> game_mode[dm_mode_now] != dm_mode_pause ){	//	ポーズ中は描画しない
		n_cap = &state -> now_cap;
		if( n_cap -> capsel_flg[cap_disp_flg] ){	//	表示フラグが立っている
			if( state -> game_mode[dm_mode_now] == dm_mode_throw   ){
				//	投げ中
				if( state -> cap_def_speed == SPEED_LOW ) {
					gg = ( state -> cap_speed_count >> 2 );
				} else {
					gg = ( state -> cap_speed_count >> 1 );
				}
				gg = gg % 4;
				gg = ONES_ooAB + gg;
				hh = 0;
				if( ONES_8_AB == gg || ONES_8_BA == gg ){
					hh = 5;
				}
				//	回転
				if( state -> now_cap.pos_x[0] == state -> now_cap.pos_x[1] ){
					//	もしカプセルが縦だった場合
					if( gg == ONES_ooAB || gg == ONES_ooBA ){
						throw_rotate_capsel( &state -> now_cap );
					}
				}else{
					//	もしカプセルが横だった場合
					if( gg == ONES_8_AB || gg == ONES_8_BA ){
						throw_rotate_capsel( &state -> now_cap );
					}
				}
				//	弧を描く
				if( state -> cap_speed_count < ( FlyingCnt[state -> cap_def_speed] / 3 ) ){
					ii = curve / (FlyingCnt[state -> cap_def_speed] / 3) * state -> cap_speed_count; // 1/3 出だし
				}else	if( state -> cap_speed_count < ( FlyingCnt[state -> cap_def_speed] / 3 << 1) ){	// 2/3 中間位置
					ii = curve;
				}else{
					ii = (FlyingCnt[state -> cap_def_speed] << 1) / 3;
					ii = state -> cap_speed_count - ii;
					ii = curve - (ii * 2);
				}
				aa = ( state -> map_x + ( state -> map_item_size * n_cap -> pos_x[0] ));		// 着弾地点 x,y
				bb = ( state -> map_y + ( state -> map_item_size * ( n_cap -> pos_y[0] - 1 )));
				cc = state -> map_x + 101;														// 開始地点 x,y
				dd = state -> map_y + 16;
				ee = aa+(((cc-aa)/(FlyingCnt[state -> cap_def_speed]-1))*(FlyingCnt[state -> cap_def_speed] - state -> cap_speed_count ));	// 左上位置 x,y
				ff = bb+(((dd-bb)/(FlyingCnt[state -> cap_def_speed]-1))*(FlyingCnt[state -> cap_def_speed] - state -> cap_speed_count ));

				for( i = 0;i < 2;i++ ){
					xx[i] = (u32)ee + hh + (state -> map_item_size * (n_cap -> pos_x[i] - 3));
					yy[i] = ((u32)ff + hh - (u16)ii) + 1 + (state -> map_item_size * (n_cap -> pos_y[i] + 1));
					zz[i] = (n_cap -> capsel_g[i] << 3) *  state -> map_item_size ;
				}
				disp_flg = 1;
			}else{
				//	落下中
				if( n_cap -> pos_y[0] > 0 ){
					//	カーソル位置の割り出し
					for( i = 0;i < 2;i ++ ){
						xx[i] = state -> map_x + (state -> map_item_size * n_cap -> pos_x[i]);
						yy[i] = state -> map_y + (state -> map_item_size * n_cap -> pos_y[i]) + 1;
						zz[i] = (n_cap -> capsel_g[i] << 3) *  state -> map_item_size ;
					}
					disp_flg = 1;
				}
			}
			if( disp_flg ){
				// フレームバッファへの点描画処理
				if( state -> map_item_size == cap_size_10 ){	//	大きい場合
					q = 0;
				}else{
					q = 1;
				}

				a = (int)&( cap_tex[q][0] );						//	グラフィックアドレス
				c = (int)&( gfx_freebuf[GFX_SPRITE_PAGE] );			//	実アドレス
				tex = (u8 *)(( a & 0xfffff )+ c );					//	テクスチャアドレス取得

				for( i = 0;i < 2;i++ ){
					b = (int)&( cap_pal[q][n_cap -> capsel_p[i]][0] );	//	パレットアドレッス
					pal = (u16*)(( b & 0xfffff )+ c );		// パレットアドレス取得

					for( j = 0;j < state -> map_item_size;j ++ ){		//	縦ループ
						for( k = 0;k < state -> map_item_size;k++ ){	//	横ループ
							m = (u8)tex[ zz[i] + (j << 3) + (k >> 1) ];	//	テクスチャデータより１バイト取得
							if( !( k & 1) ){							//	偶数ドットなら条４ビットが有効
								m = m >> 4;
							}
							m &= 0x0f;									//	m = パレット番号
							if( m ){
								n = (u16)pal[m];						//	フレームバッファに打ち込み
								fbuffer[wb_flag^1][(yy[i] + j ) * 320 + xx[i] + k] = n;
							}
						}
					}
				}
			}
		}
	}

}
/*--------------------------------------
	ゲーム用キー情報作成初期設定関数
--------------------------------------*/
void	key_cntrol_init( void )
{
	u8	i;

	for( i = 0;i < 4;i ++ ){
		joyflg[i] =( 0x0000 | DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT | DM_KEY_A | DM_KEY_B | DM_KEY_CU | DM_KEY_CD | DM_KEY_CL | DM_KEY_CR );
		keepjoy[i] = STARTJ;
		counjoy[i] = idexjoy[i] = 0;
		joygmf[i] = TRUE;
		joygam[i] = 0x0000;
	}
	joycur1 = evs_keyrept[0];
	joycur2 = evs_keyrept[1];
	joystp = FALSE;
}

/*--------------------------------------
	ゲーム用キー情報作成関数
--------------------------------------*/
void	dm_make_key(void)
{
	u8	i;

	// joygam[x] に ｹﾞｰﾑ用ｷｰ を作成
	switch( evs_gamesel ){
	case	GSL_1PLAY:
	case	GSL_VSCPU:
		if( joygmf[main_joy[0]] ){
			joygam[0] = ( joyupd[main_joy[0]] &( DM_KEY_A|DM_KEY_B ))|( joycur[main_joy[0]] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
			if( !( joynew[main_joy[0]] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
				joygam[0] = joygam[0] | ( joynew[main_joy[0]] & DM_KEY_DOWN );
			}
		}
		break;
	case	GSL_2PLAY:
		for(i = 0;i < 2;i++) {
			if( joygmf[main_joy[i]] ) {
				joygam[i] = ( joyupd[main_joy[i]] &( DM_KEY_A|DM_KEY_B ))|( joycur[main_joy[i]] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
				if( !( joynew[main_joy[i]] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
					joygam[i] = joygam[i]|( joynew[main_joy[i]] & DM_KEY_DOWN );
				}
			}
		}
		break;
	default:	//	4PLAY & DEMO
		for(i=0;i < 4;i++) {
			if( !game_state_data[i].player_state[PS_PLAYER] ){	//	人間のとき
				if( joygmf[i] ) {
					joygam[i] = ( joyupd[i] &( DM_KEY_A|DM_KEY_B ))|( joycur[i] &( DM_KEY_LEFT|DM_KEY_RIGHT ));
					if( !( joynew[i] &( DM_KEY_LEFT|DM_KEY_RIGHT )) ) {
						joygam[i] = joygam[i]|( joynew[i] & DM_KEY_DOWN );
					}
				}
			}
		}
		break;
	}
}
/*--------------------------------------
	カプセル操作関数
--------------------------------------*/
void	key_control_main( game_state *state,game_map *map, u8 player_no ,u8 joy_no )
{
	u8	i;
	game_cap *cap;

	if( state -> game_mode[dm_mode_now] == dm_mode_throw  ){	//	投げ
		dm_draw_capsel(  state,player_no );
		// ｺﾝﾄﾛｰﾗが効かない時の処理 :ｷｰﾘﾋﾟｰﾄを直前のﾃﾞｰﾀにする
		if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
			if( joynew[joy_no] & DM_KEY_RIGHT ) {
				joyCursorFastSet( DM_KEY_RIGHT,joy_no );
			}
			if( joynew[joy_no] & DM_KEY_LEFT ) {
				joyCursorFastSet( DM_KEY_LEFT, joy_no );
			}
		}
	}else	if( state -> game_mode[dm_mode_now] == dm_mode_down ){	//	操作カプセル落下
		if( state -> game_condition[dm_static_cnd] == dm_cnd_wait ){	//	通常状態


			//	落下中思考追加位置
			if( state -> player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifKeyOut( state );
			}


			cap = &state -> now_cap;
			if( joygam[player_no] & DM_ROTATION_L ){	//	左回転
				rotate_capsel( map,cap,cap_turn_l );
			}else	if( joygam[player_no] & DM_ROTATION_R ){	//	右回転
				rotate_capsel( map,cap,cap_turn_r );
			}
			if( joygam[player_no] & DM_KEY_LEFT ){	//	左移動
				translate_capsel( map,state,cap_turn_l,joy_no );
			}else	if( joygam[player_no] & DM_KEY_RIGHT ){	//	右移動
				translate_capsel( map,state,cap_turn_r,joy_no );
			}

			if( cap -> pos_y[0] > 0 ){
				if( joygam[player_no] & DM_KEY_DOWN ){	//	高速落下
					i = FallSpeed[state -> cap_speed] >> 1;
					if( FallSpeed[state -> cap_speed] % 2 ){
						i ++;
					}
					state -> cap_speed_vec = i;
				}else{
					state -> cap_speed_vec = 1;
				}
			}else{
				state -> cap_speed_vec = 1;
			}
		}else{
			// ｺﾝﾄﾛｰﾗが効かない時の処理 :ｷｰﾘﾋﾟｰﾄを直前のﾃﾞｰﾀにする
			if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
				if( joynew[joy_no] & DM_KEY_RIGHT ) {
					joyCursorFastSet( DM_KEY_RIGHT,joy_no );
				}
				if( joynew[joy_no] & DM_KEY_LEFT	) {
					joyCursorFastSet( DM_KEY_LEFT, joy_no );
				}
			}
		}
		dm_draw_capsel(  state,player_no );
		dm_capsel_down( state,map );
	}else{
		if( state -> player_state[PS_PLAYER] == PUF_PlayerMAN ){
			joyCursorFastSet( DM_KEY_RIGHT,joy_no );
			joyCursorFastSet( DM_KEY_LEFT, joy_no );
		}
	}
}
/*--------------------------------------
	思考錯誤関数
--------------------------------------*/
void	make_ai_main(void)
{
	s8	i;

	if( dm_think_flg ){
		switch( evs_gamesel )
		{
		case	GSL_VSCPU:
		case	GSL_2DEMO:
			for( i = 0;i < 2;i++ ){
				if( game_state_data[i].player_state[PS_PLAYER] == PUF_PlayerCPU ){
					if( game_state_data[i].game_condition[dm_static_cnd] == dm_cnd_wait ){
						aifMake( &game_state_data[i] );
					}
				}
			}
			break;
		case	GSL_4PLAY:
		case	GSL_4DEMO:
			for( i = 0;i < 4;i++ ){
				if( game_state_data[i].player_state[PS_PLAYER] == PUF_PlayerCPU ){
					if( game_state_data[i].game_condition[dm_static_cnd] == dm_cnd_wait ){
						aifMake( &game_state_data[i] );
					}
				}
			}
			break;
		case	GSL_1DEMO:
			if( game_state_data[0].player_state[PS_PLAYER] == PUF_PlayerCPU ){
				aifMake( &game_state_data[0] );
			}
			break;
		}
	}
}
/*--------------------------------------
	共通演出計算関数
--------------------------------------*/
void	dm_effect_make(void)
{
	//	push_any_key
	push_any_key_main();

	//	リトライの点滅計算
	dm_retry_alpha[0] =(s16)(sinf( DEGREE( dm_retry_alpha[1] ) ) * 64 ) + 191;
	dm_retry_alpha[1] += 10;
	if( dm_retry_alpha[1] >= 360 ){
		dm_retry_alpha[1] -= 360;
	}

	//	リトライの矢印左右移動の計算
	dm_retry_pos[1] ++;
	if( dm_retry_pos[1] >= 15 ){
		dm_retry_pos[1] = 0;
		dm_retry_pos[0] ^= 1;
	}
	//	リトライの攻撃相手表示の矢印上下の計算
	dm_retry_pos[2] ++;
	if( dm_retry_pos[2] > 30 )
	{
		dm_retry_pos[2] = 0;
		dm_retry_pos[3]++;
		if( dm_retry_pos[3] > 2 )
		{
			dm_retry_pos[3] = 0;
		}
	}

	//	ピーチ姫獲得デモ計算
	if( dm_peach_get_flg[0] ){
		dm_peach_get_flg[1] += 4;
		if( dm_peach_get_flg[1] >= 152 ){
			dm_peach_get_flg[1] = 152;
		}
	}

	//	勝利数の計算
	win_anime_count[1] ++;
	if( win_anime_count[1] > 4 ){
		win_anime_count[1] = 0;
		win_anime_count[0] ++;
		if( win_anime_count[0] > 14 ){
			win_anime_count[0] = 0;
		}
	}

}
/*--------------------------------------
	ゲーム用 EEP 書き込み関数
--------------------------------------*/
void	dm_game_eep_write(u32 err_add)
{
	EepRomErr err = EepRomErr_NoError;
	s8	i;

	for(i = 0;i < 2;i++){
		if( eep_rom_flg[i] ){
			if(err == EepRomErr_NoError) {
				err = EepRom_WritePlayer(evs_select_name_no[i]);
			}
		}
	}
	*(EepRomErr *)err_add = err;
}
/*--------------------------------------
	ゲーム初期化関数
--------------------------------------*/
void	dm_game_init(void)
{
	s16	i,j,k;
	s16	map_x_table[][4] = {
		{dm_wold_x,dm_wold_x,dm_wold_x,dm_wold_x},							// 1P
		{dm_wold_x_vs_1p,dm_wold_x_vs_2p,dm_wold_x_vs_1p,dm_wold_x_vs_2p},	// 2P
		{dm_wold_x_4p_1p,dm_wold_x_4p_2p,dm_wold_x_4p_3p,dm_wold_x_4p_4p}	// 4P
	};
	u8	map_y_table[] = {dm_wold_y,dm_wold_y_4p};
	u8	size_table[] = {cap_size_10,cap_size_8};


	auSeqPlayerStop(0);		//	ＢＧＭを再再生するため
	init_effect_all();		//	エフェクト初期化
	dm_make_magazine();		//	カプセル作成
	auSeqpVolumeInit();		//	ボリュームを戻す

	evs_mainx10 = FALSE;			// 描画ﾀｲﾐﾝｸﾞ調節
	evs_graph10 = FALSE;			//		..

	dm_game_init_flg		= 0;						//	1ｲﾝﾄ 背景だけしか描画しない
	evs_seqnumb 			= seq_save;					//	BGMの設定
	dm_start_time 			= dm_mode_wait_count;		//	第１投までのウェイト
	evs_seSetPtr 			= evs_seGetPtr = 0;			//	SE初期化
	bgm_bpm_flg[0] 			= bgm_bpm_flg[1] = 0;		//	BGMの速さを１回変えるためのフラグ
	dm_demo_time_flg 		= dm_demo_time = 0;			//	デモのときのタイマーとフラグのクリア
	last_3_se_flg 			= 0;						//	リーチ音を鳴らすフラグのクリア
	story_time_flg 			= 0;						//	タイム加算を開始するフラグ
	story_4p_demo_flg[0] 	= story_4p_demo_flg[1] = 0;	//	4P時のチーム表示のフラグのクリア
	dm_anime_restart_count	= 0;						//	勝敗決定後のアニメーション再稼動フラグのクリア

	dm_retry_flg = 1;								//	通常はリトライ可能

	for( i = 0;i < 2;i++ ){
		dm_retry_alpha[i] =  0;		//	リトライカーソルの点滅用変数の初期化

	}

	//	1P用の初期化
	for(i = j = 0;i < 3;i++){

		//	コインアニメーション用設定初期化
		for( k = 0;k < 3; k++ ){
			dm_retry_coin_pos[k].flg = 0;
			dm_retry_coin_pos[k].pos = 0;
			dm_retry_coin_pos[k].move_vec[0] = 0;
			dm_retry_coin_pos[k].move_vec[1] = 0;
			dm_retry_coin_pos[k].anime[0] = 0;
			dm_retry_coin_pos[k].anime[1] = 0;
		}

		big_virus_flg[i][0] = 0;	//	消滅アニメーションフラグ
		big_virus_flg[i][1] = 0;	//	消滅SEフラグ

		//	巨大ウイルスの座標を計算する
		big_virus_position[i][2] = j;	//	sin,cos用数値
		big_virus_position[i][0] = (( 10 * sinf( DEGREE( big_virus_position[i][2] ) ) ) * -2 ) + 45;
		big_virus_position[i][1] = (( 10 * cosf( DEGREE( big_virus_position[i][2] ) ) ) * 2  ) + 155;
		j += 120;
	}
	//	得点演出用変数の初期化
	for(i = 0;i < 10;i++){
		lv_sc_pos[i].score = 0;					//	内部性後変数のクリア
		lv_sc_pos[i].num_max = 0;				//	内部性後変数のクリア
		lv_sc_pos[i].flg = 0;					//	内部性後変数のクリア
		lv_sc_pos[i].mode = 0;					//	内部性後変数のクリア
		for( j = 0;j < 5;j++ ){
			lv_sc_pos[i].num_alpha[j] = 0;		//	透明度のクリア
			lv_sc_pos[i].num_alpha_flg[j] = 0;	//	透明処理終了判定変数のクリア
			lv_sc_pos[i].num_g[j] = 0;			//	数字グラフィックを指す変数クリア
			lv_sc_pos[i].score_pos[j][0] = lv_sc_pos[i].score_pos[j][1] = 0;	//	座標クリア
		}
	}

	//	ウイルス数表示サイズの初期化
	for( i = 0;i < 4;i++ ){

		for(j = 0;j < 4;j++){
			story_4p_stock_cap[i][j] = -1;	//	4Pチーム対抗戦の攻撃カプセルストックのクリア
		}

		for( j = 0;j < 3;j++ ){
			dm_retry_position[i][j] = 0;	//	リトライカーソルに関する変数の初期化
		}
		dm_retry_pos[i] =  0;		//	リトライカーソルの座標用変数の初期化

		big_virus_count[i] = 0;	//	各色のウイルス数
		virsu_num_size[i][0] = 12;
		virsu_num_size[i][1] = 16;
		virsu_num_size[i][2] = 0;
	}

	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
		k = 0;	//	アイテムの大きさ
		j = 0;	//	基準Ｘ座標
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		k = 0;	//	アイテムの大きさ
		j = 1;	//	基準Ｘ座標
		break;
	case	GSL_4PLAY:
	case 	GSL_4DEMO:
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
		//	内部制御変数の設定
		game_state_data[i].player_state[PS_PLAYER_NO]		= i;			//	プレイヤー番号
		game_state_data[i].game_mode[dm_mode_now] 			= dm_mode_init;	//	内部制御を初期化モードに設定
		game_state_data[i].game_condition[dm_mode_now] 		= dm_cnd_init;	//	状態を初期化状態に設定
		game_state_data[i].game_condition[dm_training_flg]	= dm_cnd_wait;	//	トレーニング状態を通常状態(トレーニング無し)に設定
		game_state_data[i].game_condition[dm_static_cnd]	= dm_cnd_wait;	//	絶対的状態を通常状態に設定
		game_state_data[i].virus_number						= 0;			//	ウイルス数のクリア
		game_state_data[i].warning_flg 						= 0;			//	警告音フラグのクリア

		for( j = 0;j < 3;j++ ){
			game_state_data[i].retire_flg[j] = 0;				//	リタイアフラグのクリア
		}

		//	カプセル落下速度の設定
		game_state_data[i].cap_speed 						= GameSpeed[game_state_data[i].cap_def_speed];	//	初期落下カウントの設定
		game_state_data[i].cap_speed_max 					= 0;											//	随時設定される
		game_state_data[i].cap_speed_vec 					= 1;											//	落下カウンタ増加値
		game_state_data[i].cap_magazine_cnt 				= 1;											//	マガジン残照を1にする
		game_state_data[i].cap_count = game_state_data[i].cap_speed_count = 0;								//	カウンタの初期化
		game_state_data[i].cap_move_se_flg 					= 0;											//	移動時のＳＥ再生用フラグのクリア

		//	カプセル情報の初期設定
		dm_set_capsel( &game_state_data[i] );

		//	ウイルスアニメーションの設定
		game_state_data[i].virus_anime = game_state_data[i].virus_anime_count = 0;	//	カウンタとアニメション番号の初期化
		game_state_data[i].virus_anime_vec = 1;										//	アニメーション進行方向の指定
		game_state_data[i].erase_anime = 0;											//	消滅アニメーションコマ数の初期化
		game_state_data[i].erase_anime_count = 0;									//	消滅アニメーションカウンタの初期化
		game_state_data[i].erase_virus_count = 0;									//	消滅ウイルスカウンタの初期化

		//	連鎖数の初期化
		game_state_data[i].chain_count = game_state_data[i].chain_line =  0;

		switch(evs_gamesel)
		{
		case	GSL_4PLAY:
		case	GSL_4DEMO:
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

		//	攻撃カプセルデータの初期化
		for( j = 0;j < 4;j++ ){
			game_state_data[i].chain_color[j] = 0;
		}

		//	被爆カプセルデータの初期化
#ifdef	DAMAGE_TYPE
		for( j = 0;j < DAMAGE_MAX;j++ ){
			game_state_data[i].cap_attack_work[j][0] = 0;	//	攻撃無し
			game_state_data[i].cap_attack_work[j][1] = 0;	//	攻撃無し
		}
#endif
#ifndef	DAMAGE_TYPE
		for( j = 0;j < DAMAGE_MAX;j++ ){
			for( k = 0;k < 5;k++ ){
				game_state_data[i].cap_attack_work[j][k] = 0;
			}
		}
#endif
		//	マップ情報（瓶の中）の初期化
		for(j = 0;j < 128;j ++){
			game_map_data[i][j].capsel_m_g = game_map_data[i][j].capsel_m_p = 0;
			for(k = 0;k < 6;k++){
				game_map_data[i][j].capsel_m_flg[k] = 0;
			}
		}
		for(j = 0;j < 16;j++){
			for(k = 0;k < 8;k++){
				game_map_data[i][(j << 3) + k].pos_m_x = k;		//	Ｘ座表
				game_map_data[i][(j << 3) + k].pos_m_y = j + 1;	//	Ｙ座標
			}
		}

	}
	switch(evs_gamesel)
	{
	case	GSL_4PLAY:
	case	GSL_4DEMO:
		for( i = 0;i < 4;i++ ){
			//	ウイルスデータの作成
			dm_virus_init(&game_state_data[i],virus_map_data[i],virus_map_disp_order[i]);
		}
		for( i = 0;i < 3;i++ )
		{
			for( j = i + 1;j < 4;j++ ){
				if( game_state_data[i].virus_level == game_state_data[j].virus_level ){
					//	レベルが同じだった場合ウイルス情報をコピーする
					dm_virus_map_copy(virus_map_data[i],virus_map_data[j],virus_map_disp_order[i],virus_map_disp_order[j] );
					break;
				}
			}
		}
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		for( i = 0;i < 2;i++ ){
			//	ウイルスデータの作成
			dm_virus_init(&game_state_data[i],virus_map_data[i],virus_map_disp_order[i]);
		}
		if( game_state_data[0].virus_level == game_state_data[1].virus_level ){
			//	レベルが同じだった場合ウイルス情報をコピーする
			dm_virus_map_copy(virus_map_data[0],virus_map_data[1],virus_map_disp_order[0],virus_map_disp_order[1] );
		}
		break;
	case	GSL_1PLAY:
	case	GSL_1DEMO:
		//	ウイルスデータの作成
		dm_virus_init(&game_state_data[0],virus_map_data[0],virus_map_disp_order[0]);
		break;
	}


	aifGameInit();
}

/*--------------------------------------
	ゲーム処理メイン関数
--------------------------------------*/
int		dm_game_main(NNSched*	sched)
{
	OSMesgQueue	msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	EepRomErr	eep_err;
	int	ret;
	u32	address;
	u32	score_save;
	s16	*msgtype;
	u8	i,j;
	s8	loop_flg = 0;
	s8	gs;
	u16	joybak;


	//	メッセージキューの作成
  	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	//	メッセージＱノ割り当て
	//	クライアントに登録
	nnScAddClient(sched, &client, &msgQ);

	//	初期化
	seq_save = evs_seqnumb;	//	BGMの保管

	dm_game_init();									//	ゲームの初期化
	key_cntrol_init();
	win_anime_count[0]  = win_anime_count[1] = 0;
	dm_retry_coin = 0;
	dm_peach_stage_flg = 0;
	dm_TouchDownWait = 2;
	dm_think_flg = 1;

	//	変数の初期化
	for(i = 0;i < 2;i++){
		dm_peach_get_flg[i] = 0;
		story_4p_name_flg[i] = i;
		vs_win_count[i] = 0;
		vs_lose_count[i] = 0;
	}

	for( i = 0;i < 4;i++ ){
		win_count[i] = 0;	//	勝利数カウントの初期化
	}


	//	グラフィックデータの読込み(背景等)
	switch( evs_gamesel )
	{
	case	GSL_1DEMO:
		score_save = evs_high_score;	//	ハイスコアを保存してクリアする
		evs_high_score = 0;
	case	GSL_1PLAY:
		dm_TouchDownWait = 1;
		auRomDataRead( (u32)dm_anime_rom_address[0][0],gfx_freebuf[GFX_ANIME_PAGE_1],(u32)dm_anime_rom_address[0][1] - (u32)dm_anime_rom_address[0][0] );		//	マリオ
		auRomDataRead( (u32)dm_anime_rom_address[12][0],gfx_freebuf[GFX_ANIME_PAGE_2],(u32)dm_anime_rom_address[12][1] - (u32)dm_anime_rom_address[12][0] );	//	ウイルス

		game_state_data[0].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_1];
		dm_anime_char_set(&game_state_data[0].anime,game_anime_data_mario );
		dm_anime_set( &game_state_data[0].anime,ANIME_opening );
		for( j = 0;j < 2;j++ ){
			disp_anime_data[0][j] = game_state_data[0].anime;
		}
		//	ウイルスアニメーション設定
		for( i = 0;i < 3;i++ ){
			big_virus_anime[i].cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];
			dm_anime_char_set(&big_virus_anime[i],game_anime_table[i + 12] );
			dm_anime_set( &big_virus_anime[i],ANIME_nomal );
		}

		//	背景読込み
		auRomDataRead((u32)_dm_bg_level_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_level_modeSegmentRomEnd - (u32)_dm_bg_level_modeSegmentRomStart);
		S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		//	アニメーションデータ読込み
		auRomDataRead( (u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][0],gfx_freebuf[GFX_ANIME_PAGE_1],
										(u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][1] - (u32)dm_anime_rom_address[game_state_data[0].anime.cnt_charcter_no][0] );
		auRomDataRead( (u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][0],gfx_freebuf[GFX_ANIME_PAGE_2],
										(u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][1] - (u32)dm_anime_rom_address[game_state_data[1].anime.cnt_charcter_no][0] );

		//	アドレス設定
		game_state_data[0].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_1];
		game_state_data[1].anime.cnt_anime_address = (u32)gfx_freebuf[GFX_ANIME_PAGE_2];

		//	初期アニメーションデータの設定
		dm_anime_char_set(&game_state_data[0].anime,game_anime_table[game_state_data[0].anime.cnt_charcter_no] );
		dm_anime_char_set(&game_state_data[1].anime,game_anime_table[game_state_data[1].anime.cnt_charcter_no] );

		//	初期アニメーションの設定
		for( i = 0;i < 2; i++ ){
			dm_anime_set( &game_state_data[i].anime,ANIME_opening );
			for( j = 0;j < 2;j++ ){
				disp_anime_data[i][j] = game_state_data[i].anime;
			}
		}
		//	背景読込み
		if( evs_story_flg ){	//	ストーリーモードの場合
			if( evs_story_no == 11 ){
				dm_peach_stage_flg = 1;	//	エクストラステージ
			}else{
				dm_game_bg_no = evs_story_no - 1;
			}
		}else{
			//	ポイント表示の数値をあらかじめセットしておく
			for(i = 0;i < 2;i++){
				dm_vsmode_win_point[i] = 0;
			}
			switch( evs_gamesel ){
			case	GSL_2PLAY:
				for(i = 0;i < 2;i++){
					if( evs_select_name_no[i] != 8 ){	//	自分がゲストではない
						if( evs_select_name_no[i ^ 1] != 8 ){	//	相手もゲストではない
							dm_vsmode_win_point[i] = evs_mem_data[evs_select_name_no[i]].vs_data[evs_select_name_no[i ^ 1]][0];
						}
					}
				}
				break;
			case	GSL_VSCPU:
				if( evs_select_name_no[0] != 8 ){	//	自分がゲストではない
					j = game_state_data[1].anime.cnt_charcter_no;	//	相手のキャラクター番号
					dm_vsmode_win_point[0] = evs_mem_data[evs_select_name_no[0]].vscom_data[j][0];
					dm_vsmode_win_point[1] = evs_mem_data[evs_select_name_no[0]].vscom_data[j][1];
				}
				break;
			}

			if( evs_stage_no > 0 ){	//	ピーチステージでなかったら
				dm_game_bg_no = evs_stage_no - 1;//	仮
			}else{
				dm_peach_stage_flg = 1;
			}
		}
		if( dm_peach_stage_flg ){
			//	ピーチステージ
			auRomDataRead((u32) _bg_dat_exSegmentRomStart, BGBuffer, (u32)_bg_dat_exSegmentRomEnd - (u32)_bg_dat_exSegmentRomStart);
			S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		}else{
			//	通常ステージ
			auRomDataRead((u32)dm_bg_rom_address[dm_game_bg_no], BGBuffer,51720);
			S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,160,320,160,0,40,0,0 );
		}
		break;
	case	GSL_4PLAY:
	case	GSL_4DEMO:

		//	背景読込み
		auRomDataRead((u32)_dm_bg_vs4p_modeSegmentRomStart, BGBuffer, (u32)_dm_bg_vs4p_modeSegmentRomEnd - (u32)_dm_bg_vs4p_modeSegmentRomStart);
		S2d_ObjBg_InitTile( &s2d_dm_bg,(u8 *)&BGBuffer[66],G_IM_FMT_CI,G_IM_SIZ_8b,320,240,320,240,0,0,0,0 );

		for( i = 0;i < 4;i++ ){
			story_4p_name_num[i] = 0;
		}
		//	チームのメンバー数計算
		for( i = 0;i < 4;i++ ){
			story_4p_name_num[ game_state_data[i].player_state[PS_TEAM_FLG] ] ++;
		}

		if( evs_story_flg ){	//	ストーリーモードの場合
			//	チーム対抗戦
			dm_game_4p_team_flg = 1;
			//	デモ処理を行う
			story_4p_demo_flg[0] = 1;
			story_4p_demo_w[0] = 2;
			//	デモに使う小道具の設定
			S2d_ObjBg_InitTile( &s2d_demo_4p,dm_4p_game_face_back_bm0_0,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,2,58,62,102,96,0 );

			//	背景(台座)の選択
			story_4p_demo_bg = dm_4p_game_face_back_bm0_0;
			story_4p_demo_bg_tlut = dm_4p_game_face_back_bm0_0tlut;

			story_4p_demo_face_pos[0] = 91;		//	1p
			story_4p_demo_face_pos[3] = 205;	//	4p

			switch( evs_story_level )
			{
			case	0:	//	3 VS 1	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_3_VS_1;
				dm_game_4p_team_bits[0] = 0x07;
				dm_game_4p_team_bits[1] = 0x08;
				story_4p_demo_face_pos[1] = 119;	//	2p
				story_4p_demo_face_pos[2] = 147;	//	3p
				story_4p_demo_face_pos[4] = 180;	//	vs
				break;
			case	1:	//	2 VS 2	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_2_VS_2;
				dm_game_4p_team_bits[0] = 0x03;
				dm_game_4p_team_bits[1] = 0x0c;
				story_4p_demo_face_pos[1] = 119;	//	2p
				story_4p_demo_face_pos[2] = 177;	//	3p
				story_4p_demo_face_pos[4] = 153;	//	vs
				break;
			case	2:	//	1 VS 3	( MARIO : ENEMY )
				dm_game_4p_team_bit = dm_1_VS_3;
				dm_game_4p_team_bits[0] = 0x01;
				dm_game_4p_team_bits[1] = 0x0e;
				story_4p_demo_face_pos[1] = 147;	//	2p
				story_4p_demo_face_pos[2] = 177;	//	3p
				story_4p_demo_face_pos[4] = 124;	//	vs
				break;
			}
		}else{
			if( story_4p_name_num[0] == 1 && story_4p_name_num[1] == 1
					&& story_4p_name_num[2] == 1 && story_4p_name_num[3] == 1 ){

				//	バトルロイヤル
				dm_game_4p_team_flg = 0;
				dm_game_4p_team_bit = 0;
			}else{
				//	チーム対抗戦
				dm_game_4p_team_flg = 1;

				//	デモ処理を行う
				story_4p_demo_flg[0] = 1;
				story_4p_demo_w[0] = 2;

				//	判定用ビットの作成
				dm_game_4p_team_bits[0] = dm_game_4p_team_bits[1] = 0;
				for(i = 0;i < 4;i++){
					if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
						dm_game_4p_team_bits[TEAM_MARIO] |= 1 << i;
					}
				}
				for(i = 0;i < 4;i++){
					if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_ENEMY ){
						dm_game_4p_team_bits[TEAM_ENEMY] |= 1 << i;
					}
				}

				story_4p_demo_face_pos[0] = 91;		//	1p
				story_4p_demo_face_pos[3] = 205;	//	4p

				switch( story_4p_name_num[0] )
				{
				case	1:	//	1 VS 3	( A : B )
					dm_game_4p_team_bit = dm_1_VS_3;
					story_4p_demo_face_pos[1] = 146;	//	2p
					story_4p_demo_face_pos[2] = 177;	//	3p
					story_4p_demo_face_pos[4] = 124;	//	vs
					break;
				case	2:	//	2 VS 2	( A : B )
					dm_game_4p_team_bit = dm_2_VS_2;
					story_4p_demo_face_pos[1] = 119;	//	2p
					story_4p_demo_face_pos[2] = 177;	//	3p
					story_4p_demo_face_pos[4] = 153;	//	vs
					break;
				case	3:	//	3 VS 1	( A : B )
					dm_game_4p_team_bit = dm_3_VS_1;
					story_4p_demo_face_pos[1] = 119;	//	2p
					story_4p_demo_face_pos[2] = 147;	//	3p
					story_4p_demo_face_pos[4] = 180;	//	vs
					break;
				}

				story_4p_demo_bg = dm_4p_team_face_back_bm0_0;
				story_4p_demo_bg_tlut = dm_4p_team_face_back_bm0_0tlut;
				S2d_ObjBg_InitTile( &s2d_demo_4p,story_4p_demo_bg,G_IM_FMT_CI,G_IM_SIZ_4b,192,58,2,58,62,102,96,0 );
			}
		}

		break;
	}
	//	グラフィックデータの読込み(カプセル等)
	auRomDataRead((u32)_spriteSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)_spriteSegmentRomEnd-(u32)_spriteSegmentRomStart);

	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	graphic_no = GFX_GAME;

//	auSeqPlayerPlay(0,evs_seqnumb);	//	音楽再生
	while( loop_flg  == 0 )
	{
		//	リトレースメッセージ
		(void) osRecvMesg(&msgQ,(OSMesg*)&msgtype, OS_MESG_BLOCK);
		// ＣＰＵ処理落ち
		if (!MQ_IS_EMPTY(&msgQ)) {
			nnp_over_time = 1;
		}


		if( *msgtype == NN_SC_RETRACE_MSG ) {
#ifdef	DM_DEBUG_FLG
			if( joyupd[main_joy[0]] & DM_KEY_L ){	//	面変更
				if( evs_story_flg ){	//	ストーリーモーの場合
					//	テスト用強制終了
					game_state_data[0].game_condition[dm_static_cnd] = dm_cnd_win;
//					game_state_data[main_joy[0]].game_condition[dm_static_cnd] = dm_cnd_win;
					break;
				}else{
					if( evs_gamesel != GSL_1PLAY ){
						break;
					}
				}
			}

			if( evs_gamesel == GSL_1PLAY ){
				if( joyupd[main_joy[0]] & DM_KEY_Z ){	//	コイン追加
					if( dm_retry_coin < 3){
						dm_retry_coin ++;
					}
				}
			}
#endif

			#ifdef NN_SC_PERF
				if( joyupd[main_joy[0]] & Z_TRIG ){	//	パフォーマンスメーター表示処理
					nnp_over_time = 0;
					disp_meter_flg ^= 1;
				}
			#endif

			evs_mainx10 = TRUE;
			#ifdef NN_PERF_Main_Graph
				#ifdef NN_SC_PERF
					nnScPushPerfMeter(NN_SC_PERF_MAIN);
				#endif
			#endif

#ifdef	DM_DEBUG_FLG

			joybak = joyupd[0];
			for(gs = 0;gs < evs_gamespeed;gs++) {
				if ( gs ) joyupd[0] = 0;
#endif
				if( bgm_bpm_flg[1] ){
					auSeqpSetTempo(0.75);				//	曲のＢＰＭを変える
					bgm_bpm_flg[1] = 0;
				}


				dm_effect_make();	//	共通計算

				switch( evs_gamesel )
				{
				case	GSL_1PLAY:
					loop_flg = dm_game_main_1p();	//	1P PLAY
					if( loop_flg > 0 ){	//	ステージクリア
						if( loop_flg == dm_ret_next_stage ){
							if( game_state_data[0].virus_level < 99 ){
								game_state_data[0].virus_level ++;
							}
						}
						loop_flg = 0;
						dm_anime_set( &game_state_data[0].anime,ANIME_opening );
						for( i = 0;i < 3;i++ ){
							dm_anime_set( &big_virus_anime[i],ANIME_nomal );
						}

						dm_game_init();
					}else{
						//	効果音再生
						dm_play_se();
					}
					break;
				case	GSL_2PLAY:
				case	GSL_VSCPU:
					//	勝利数のアニメーション

					loop_flg = dm_game_main_2p();
					if( loop_flg == dm_ret_game_end )
					{
						for( i = 0;i < 2;i++ ){
							if( win_count[i] == WINMAX ){	//	誰かが３勝している
								vs_win_count[i] ++;
								vs_lose_count[i ^ 1] ++;
								break;
							}
						}
					}else	if( loop_flg == -1 ){
						if( !evs_story_flg ){	//	ストーリーモードでない場合
							loop_flg = 0;
							for( i = 0;i < 2;i++ ){
								if( win_count[i] == WINMAX ){	//	誰かが３勝している
									vs_win_count[i] ++;
									vs_lose_count[i ^ 1] ++;
									loop_flg = -1;	//	終了
									break;
								}
							}
							if( loop_flg == 0 ){
								dm_game_init();	//	再試合
								for( i = 0;i < 2; i++ ){
									dm_anime_set( &game_state_data[i].anime,ANIME_opening );
								}
							}
						}
					}else	if( loop_flg == dm_ret_retry ){	//	リトライの場合
						loop_flg = 0;
						dm_game_init();	//	再試合

						//	勝敗判定
						if( evs_story_flg ){	//	ストーリーモードの場合
							for( i = 0;i < 2; i++ ){
								win_count[i] = 0;
							}
						}else{
							for( i = 0;i < 2;i++ ){
								if( win_count[i] == WINMAX ){	//	誰かが３勝している
									vs_win_count[i] ++;
									vs_lose_count[i ^ 1] ++;
									for( j = 0;j < 2;j++ ){
										win_count[j] = 0;
									}
									break;
								}
							}
							if( evs_gamesel == GSL_VSCPU ){
								for( i = 0;i < 2; i++ ){
									win_count[i] = 0;
								}
							}
						}
						for( i = 0;i < 2; i++ ){
							dm_anime_set( &game_state_data[i].anime,ANIME_opening );
						}
					}else{
						//	効果音再生
						dm_play_se();
					}
					break;
				case	GSL_4PLAY:
					if( story_4p_demo_flg[0] ){
						demo_story_4p_main();
					}else{
						loop_flg = dm_game_main_4p();
					}
					if( loop_flg == -1 ){
						loop_flg = 0;
						if( evs_story_flg ){	//	ストーリーモードの場合
							loop_flg = -1;	//	とりあえず１ゲームで終了
						}else{
							for( i = 0;i < 4;i++ ){
								if( win_count[i] == WINMAX ){	//	誰かが３勝している
									loop_flg = -1;	//	終了
									break;
								}
							}
							if( loop_flg == 0 ){
								dm_game_init();	//	再試合
							}
						}
					}else	if( loop_flg == dm_ret_retry ){
						for( i = 0;i < 4;i++ ){
							win_count[i] = 0;
						}
						loop_flg = 0;
						dm_game_init();	//	再試合
					}else{
						if( loop_flg != dm_ret_game_end ){
							dm_play_se();
						}
					}
					break;
				case	GSL_1DEMO:
					loop_flg = dm_game_demo_1p();
					break;
				case	GSL_2DEMO:
					loop_flg = dm_game_demo_2p();
					break;
				case	GSL_4DEMO:
					loop_flg = dm_game_demo_4p();
					break;
				}
#ifdef	DM_DEBUG_FLG

				if( loop_flg != 0 )
					break;
			}
			joyupd[0] = joybak;
#endif
			evs_mainx10 = FALSE;
			#ifdef NN_PERF_Main_Graph
				#ifdef NN_SC_PERF
					nnScPopPerfMeter();	//	NN_SC_PERF_MAIN
				#endif
			#endif
		}
	}

	dm_think_flg = 0;	//	思考停止

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


//	evs_seqnumb = seq_save;	//	BGMを元に戻す

	//	EEPROM の書き込み処理フラグ
	eep_rom_flg[0] = eep_rom_flg[1] = 0;

	//	現在のメイン番号の保存
	main_old = MAIN_GAME;

	//	次にどの処理に飛ぶか設定する
	switch( evs_gamesel )
	{
	case	GSL_4PLAY:
		if( evs_story_flg ){	//	ストーリーモード
			evs_gamesel = GSL_VSCPU;
			if( evs_one_game_flg ){
				ret	=	MAIN_60;
			}else{
				if( game_state_data[0].game_condition[dm_static_cnd] ==  dm_cnd_win ){
					//	ゲームスコアとリタイア回数をコピーする
					if( !evs_clear_stage_flg[evs_story_no] ){
						evs_clear_stage_flg[evs_story_no] = 1;
					}
					ret	=	MAIN_50;
				}else{	//	ゲームオーバー
					//	セーブデータセット
					if( evs_select_name_no[0] != DM_MEM_GUEST ){
						eep_rom_flg[0] = 1;
						dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,evs_story_no  - 1 );
					}

					ret	=	MAIN_60;
				}
			}
		}else{
			ret	=	MAIN_60;
		}
		break;
	case	GSL_2PLAY:
		for(i = 0;i < 2;i++){
			//	記録の保存
			if( evs_select_name_no[i] != DM_MEM_GUEST ){
				eep_rom_flg[i] = 1;
				dm_vsman_set(&game_state_data[i],vs_win_count[i],vs_lose_count[i],evs_select_name_no[i],evs_select_name_no[i ^ 1]);
			}
		}

		ret	=	MAIN_60;
		break;
	case	GSL_VSCPU:
		if( evs_story_flg ){	//	ストーリーモード
			if( evs_one_game_flg ){
				ret	=	MAIN_60;
			}else{
				if( game_state_data[0].game_condition[dm_static_cnd] ==  dm_cnd_win ){

					if( evs_story_no == 11 ){	//	EXステージ
						if( !evs_secret_flg ){
							evs_secret_flg = 1;	//	ピーチ姫使用可能
						}
						//	セーブデータセット
						if( evs_select_name_no[0] != DM_MEM_GUEST ){
							eep_rom_flg[0] = 1;
							dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,0xff);
						}
						evs_story_no = 0;
						ret	=	MAIN_STAFF;
					}else{
						if( !evs_clear_stage_flg[evs_story_no] ){
							evs_clear_stage_flg[evs_story_no] = 1;
						}
						ret	=	MAIN_50;
					}
				}else{
					//	セーブデータセット
					if( evs_select_name_no[0] != DM_MEM_GUEST ){
						dm_story_sort_set( &game_state_data[0],evs_game_time,evs_select_name_no[0],evs_story_level,evs_story_no - 1 );
						eep_rom_flg[0] = 1;
					}
					if( evs_story_no == 11 ){	//	EXステージ
						ret	=	MAIN_STAFF;
					}else{
						ret	=	MAIN_60;
					}
				}
			}
		}else{
			//	記録の保存
			if( evs_select_name_no[0] != DM_MEM_GUEST ){
				eep_rom_flg[0] = 1;
				dm_vscom_set(&game_state_data[0],vs_win_count[0],vs_lose_count[0],evs_select_name_no[0],game_state_data[1].anime.cnt_charcter_no);
			}

			ret	=	MAIN_60;
		}
		break;
	case	GSL_1PLAY:
		if( evs_high_score < game_state_data[0].game_score ){
			evs_high_score = game_state_data[0].game_score;	//	ハイスコアの更新
		}
		//	セーブデータセット
		if( evs_select_name_no[0] != DM_MEM_GUEST ){
			eep_rom_flg[0] = 1;
			dm_level_sort_set( &game_state_data[0],evs_select_name_no[0] );
		}

		evs_mem_data[evs_select_name_no[0]].state_old.p_1p_lv = game_state_data[0].virus_level;

		ret	=	MAIN_60;
		break;
	case	GSL_1DEMO:
		 evs_high_score = score_save;	//	ハイスコアを元に戻す
	case	GSL_2DEMO:
	case	GSL_4DEMO:
		ret	=	MAIN_TITLE;
		break;
	}

#ifdef	_DM_EEP_ROM_USE_
		//	EEP 書き込みスタート
		if( eep_rom_flg[0] || eep_rom_flg[1] ){
			EepRom_CreateThread();
			EepRom_SendTask(dm_game_eep_write,(u32)&eep_err);

			//	書き込み終了待ち
			while(1){
				if( !EepRom_GetTaskCount() ){
					EepRom_DestroyThread();
				}
			}

		}
#endif	//	_DM_EEP_ROM_USE_



	return	ret;
}

/*--------------------------------------
	ゲーム描画メイン関数
--------------------------------------*/
void	dm_game_graphic(void)
{
	NNScTask*	gt;
	s16	i,j,k;
	s16	n_pos[2] = {46,236};
	s16	s_pos[2] = {49,239};
	s16	s_pos_4p[2] = {109,179};

	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄﾊﾞｯﾌｧ･ﾀｽｸﾊﾞｯﾌｧの指定
	gp = &gfx_glist[wb_flag][0];
	gt = &gfx_task[wb_flag];

	// RSP 初期設定

	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, OBJ_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_SPRITE_PAGE]));	//	アイテム（カプセル等）のセグメント設定
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_1]));	//	1Pキャラクタのセグメント設定
	gSPSegment(gp++, GS1_SEGMENT, osVirtualToPhysical(gfx_freebuf[GFX_ANIME_PAGE_2]));	//	2Pキャラクタのセグメント設定

	// フレームバッファのクリア
	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);

	gSPDisplayList(gp++, S2Spriteinit_dl);

//	gSPDisplayList(gp++, ClearCfb );

	//	描画範囲の指定
	gDPSetScissor(gp++,G_SC_NON_INTERLACE, 0, 0, SCREEN_WD-1, SCREEN_HT-1);

	//	半透明設定
	gSPDisplayList(gp++, Texture_TE_dl );

	//	背景描画

#ifdef	DM_DEBUG_FLG
	if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
	case	GSL_4PLAY:
	case	GSL_4DEMO:
		//	背景描画

		load_TexPal( (u16 * )&BGBuffer[1] );
 		gSPBgRect1Cyc( gp++, &s2d_dm_bg );

/*		for(i = 0;i < 40;i++)
		{
			load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
			draw_Tex(0,(i * 6),320,6,0,0);
		}
*/		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		//	背景描画
		load_TexPal( (u16 * )&BGBuffer[1] );
 		gSPBgRect1Cyc( gp++, &s2d_dm_bg );

/*
		for(i = 0;i < 26;i++)
		{
			load_TexTile_8b((u8 *)&BGBuffer[(240 * i) + 66],320,6,0,0,319,5);
			draw_Tex(0,(i * 6) + 40,320,6,0,0);
		}
		load_TexTile_8b((u8 *)&BGBuffer[(240 * 26) + 66],320,4,0,0,319,3);
		draw_Tex(0,(26 * 6) + 40,320,4,0,0);
*/
		//	ピーチステージ以外の場合
		if( !dm_peach_stage_flg ){

			//	背景の上下タイル描画
			load_TexPal( dm_bg_tile_pal_data[dm_game_bg_no] );
			load_TexTile_4b( dm_bg_tile_data[dm_game_bg_no],80,40,0,0,79,39 );
			for( i = 0;i < 4;i++ ){
				draw_Tex(80 * i,0,80,40,0,0 );
				draw_Tex(80 * i,200,80,40,0,0 );
			}

			gDPSetRenderMode( gp++,G_RM_AA_XLU_SPRITE, G_RM_AA_XLU_SPRITE2);
			gDPSetPrimColor( gp++,0,0,255,255,255,128);
			//	瓶の影描画

			load_TexPal( dm_bin_shadow_bm0_0tlut );
			for( i = 0;i < 4;i++ ){
				load_TexBlock_4b(&dm_bin_shadow_bm0_0[48 * 42 * i],96,42 );
				for( j = 0;j < 2;j++ ){
					draw_Tex( j * 184 + 20,i * 42 + 16,96,42,0,0);
				}
			}
			//	あまり
			load_TexBlock_4b(&dm_bin_shadow_bm0_0[48 * 42 * 4],96,40 );
			for( j = 0;j < 2;j++ ){
				draw_Tex( j * 184 + 20,4 * 42 + 16,96,40,0,0);
			}

			gDPSetRenderMode(  gp++,G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
			gDPSetPrimColor(gp++,0,0,255,255,255,255);
			//	瓶の描画
			load_TexPal( dm_bin_bm0_0tlut );
			for( i = 0;i < 4;i++ ){
				load_TexBlock_4b(&dm_bin_bm0_0[48 * 42 * i],96,42 );
				for( j = 0;j < 2;j++ ){
					draw_Tex( j * 184 + 20,i * 42 + 16,96,42,0,0);
				}
			}
			//	あまり
			load_TexBlock_4b(&dm_bin_bm0_0[48 * 42 * 4],96,40 );
			for( j = 0;j < 2;j++ ){
				draw_Tex( j * 184 + 20,4 * 42 + 16,96,40,0,0);
			}
		}
		//	画面中央の飾り
		if( evs_story_flg ){	//	ストーリーモードの場合
			load_TexPal( dm_parts_story_bm0_0tlut );
			for( i = 0;i < 6;i++ ){
				load_TexTile_8b(dm_parts_story_bm0_0 + (86 * 20 * i),86,20,0,0,85,19 );
				draw_Tex( 116,20 * i + 95,86,20,0,0);
			}
			load_TexTile_8b(dm_parts_story_bm0_0 + (86 * 20 * 6),86,6,0,0,85,5 );
			draw_Tex( 116,20 * 6 + 95,86,6,0,0);
		}else{
			load_TexPal( dm_parts_vscom_bm0_0tlut );
			for( i = 0;i < 5;i++ ){
				load_TexTile_8b(&dm_parts_vscom_bm0_0[74 * 24 * i],74,24,0,0,73,23 );
				draw_Tex( 122,24 * i + 93,74,24,0,0);
			}
			load_TexTile_8b(&dm_parts_vscom_bm0_0[74 * 24 * 5],74,13,0,0,73,12 );
			draw_Tex( 122,24 * 5 + 93,74,13,0,0);
		}
		break;
	}
#ifdef	DM_DEBUG_FLG
	}
#endif




	switch( evs_gamesel )
	{
	case	GSL_1PLAY:
	case	GSL_1DEMO:
#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			dm_virus_anime( &game_state_data[0],game_map_data[0] );		//	ウイルスアニメーション

			dm_disp_score( game_state_data[0].game_score,94,66 );		//	現在得点
			dm_disp_score( evs_high_score,94,40 );						//	最高得点
			dm_disp_add_score();										//	加算得点
			load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );		//	数字データロード
			dm_disp_number( game_state_data[0].virus_level,239,132,TLUT_virus_number );	//	ウイルスレベル
			dm_disp_virus_number( &game_state_data[0],239,208,0 );		//	残りウイルス数
			dm_disp_time(evs_game_time,279,182);						//	時間描画
			dm_speed_draw( &game_state_data[0],239,157 );				//	速度
			dm_anime_draw( &disp_anime_data[0][wb_flag],220,24,0 );		//	マリオ描画

			//	コイン
			load_TexPal( dm_game_coin_bm0_0tlut );
			load_TexBlock_4b( &dm_game_coin_bm0_0[0],16,64 );
			for( i = 0;i < dm_retry_coin;i++ ){
				draw_Tex( (i * 16) + 40,92 + dm_retry_coin_pos[i].pos,16,16,0,dm_retry_coin_pos[i].anime[1] * 16 );
			}
			if( game_state_data[0].game_condition[dm_static_cnd] != dm_cnd_pause ){	//	通常状態だった
				dm_anime_draw( &big_virus_anime[2],big_virus_position[2][0],big_virus_position[2][1],0 );				//	巨大青ウイルス描画
				dm_anime_draw( &big_virus_anime[0],big_virus_position[0][0],big_virus_position[0][1],0 );				//	巨大赤ウイルス描画
				dm_anime_draw( &big_virus_anime[1],big_virus_position[1][0],big_virus_position[1][1],0 );				//	巨大黄ウイルス描画
			}
			dm_game_graphic_1p( &game_state_data[0],game_map_data[0] );	//	瓶の中身描画
			dm_game_graphic_effect( &game_state_data[0],0,0 );			//	演出描画

#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif
		break;
	case	GSL_2PLAY:
	case	GSL_VSCPU:
	case	GSL_2DEMO:
		j = 92;
		if( !evs_story_flg ){	//	ストーリーモードでない場合
			k = 129;
		}else{
			k = 121;
		}
#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			//	数字データ読込み
			load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );

			if( evs_story_flg ){	//	ストーリーモードの場合
				//	ステージ番号
				dm_disp_number( evs_story_no,174,99,dm_stage_num_b_bm0_0tlut);
				//	時間描画
				dm_disp_time(evs_game_time,187,162);
				//	残りウイルス
				for(i = 0;i < 2;i++){
					dm_disp_virus_number( &game_state_data[i],130 + (i * 36),198,i );	//	残りウイルス数
				}

				//	レベル描画
				load_TexPal( dm_4p_game_level_bm0_0tlut );
				load_TexTile_4b( &dm_4p_game_level_bm0_0[0],28,15,0,0,27,14 );
				draw_Tex( 146,136,28,5,0,evs_story_level * 5);
			}else{
				for(i = 0;i < 2;i++){
					dm_disp_virus_number( &game_state_data[i],130 + (i * 36),203,i );	//	残りウイルス数
				}
				load_TexPal( lv_num_bm0_0tlut );
				load_TexTile_4b( &lv_num_bm0_0[0],8,120,0,0,7,119 );
				for(i = 0;i < 2;i++){
					dm_disp_point( dm_vsmode_win_point[i] + vs_win_count[i],153 + i * 39,170 );	//	ポイント
				}
			}

			load_TexPal( TLUT_star );						//	星テクスチャロード
			load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	星テクスチャロード
			if( !evs_story_flg ){	//	ストーリーモードでない場合
				for(i = 0;i < 2;i++){
					dm_star_draw( k,96,win_count[i],1);
					k += 46;
				}
			}else{					//	ストーリーモードの場合
				for(i = 0;i < 2;i++){
					dm_star_draw(k,127,win_count[i],0);
					k += 62;
				}
			}

			for( i = 0;i < 2;i++ ){
				dm_virus_anime( &game_state_data[i],game_map_data[i] );		//	ウイルスアニメーション
				dm_game_graphic_p( &game_state_data[i],game_map_data[i] );	//	瓶の中身描画
				dm_game_graphic_effect( &game_state_data[i],i,0 );			//	演出描画
				dm_anime_draw( &game_state_data[i].anime,j,10,i );			//	アニメーション描画
				j += 136;
			}

			if( dm_peach_get_flg[0] ){
				//	PEACH GET 描画
				load_TexPal( dm_get_peach_bm0_0tlut );
				load_TexTile_4b( &dm_get_peach_bm0_0[0],152,16,0,0,151,15 );
				draw_Tex( 84,120,dm_peach_get_flg[1],16,0,0 );
			}
#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif

		break;
	case	GSL_4PLAY:
	case	GSL_4DEMO:

#ifdef	DM_DEBUG_FLG
		if( game_state_data[0].game_mode[dm_mode_now] != dm_mode_debug ){
#endif
			if( evs_story_flg ){	//	ストーリーモーの場合

					load_TexPal( dm_4p_game_virus_s_bm0_0tlut );	//	Virus パレットロード
					load_TexBlock_4b( dm_4p_game_virus_s_bm0_0,48,10 );	//	Virus グラフィックロード
					for( i = 0,j = 46;i < 4;i ++ ){
						draw_Tex( j,221,34,10,0,0);
						j += 72;
					}

					//	チーム描画
					for(i = 0,j = 24;i < 4;i++){
						if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
							load_TexPal( dm_4p_game_team_aa_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_aa_bm0_0,64,13 );
							draw_Tex( j,191,55,13,0,0);
						}else{
							load_TexPal( dm_4p_game_team_bb_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_bb_bm0_0,64,13 );
							draw_Tex( j,191,52,13,0,0);
						}
						j += 72;
					}

				load_TexPal( dm_4p_story_name_bm0_0tlut );						//	MARIO & ENEMY パレットロード
				load_TexTile_4b( &dm_4p_story_name_bm0_0[0],38,20,0,0,37,19 );	//	MARIO & ENEMY グラフィックロード
				draw_Tex( n_pos[story_4p_name_flg[1]],16,38,10,0,0);	//	ENEMY
				draw_Tex( n_pos[story_4p_name_flg[0]],16,34,10,0,10);	//	MARIO

				load_TexPal( dm_4p_cap_stock_bm0_0tlut );	//	カプセルストック パレットロード
				load_TexTile_4b( &dm_4p_cap_stock_bm0_0[0],40,16,0,0,39,15 );	//	カプセルストック グラフィックロード
				draw_Tex( 44,26,40,16,0,0);			//	MARIO側
				draw_Tex( 234,26,40,16,0,0);		//	ENEMY側

				load_TexPal( dm_4p_story_gage_bm0_0tlut );	//	中央部 パレットロード
				load_TexTile_4b( &dm_4p_story_gage_bm0_0[0],154,24,0,0,153,23 );	//	中央部 グラフィックロード
				draw_Tex( 96,18,130,24,0,0);		//	中央部
				draw_Tex( 17,18,24,24,130,0);	//	勝ち星ケージ
				draw_Tex( 277,18,24,24,130,0);	//	勝ち星ケージ


				//	時間描画
				dm_disp_time_4p( evs_game_time,evs_story_level );

				load_TexPal( TLUT_star );						//	星テクスチャロード
				load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	星テクスチャロード
				for(i = 0,j = 22;i < 2;i++ ){
					dm_star_draw( j,21,win_count[i],0 );
					j += 260;
				}

				//	ウイルスグラフィックデータの読込み
				load_TexTile_4b( cap_tex[1],16,128,0,0,15,127 );
				for( i = 0;i < 2;i++ ){
					for(j = k = 0;j < 4;j++){
						if( story_4p_stock_cap[story_4p_name_flg[i]][j] >= 0 ){
							//	ストックがある場合
							load_TexPal( capsel_8_pal[story_4p_stock_cap[story_4p_name_flg[i]][j]] );	//	パレットロード
							//	カプセル描画
							draw_Tex( s_pos[story_4p_name_flg[i]] + (k << 3),29,cap_size_8,cap_size_8,0,capsel_b << 3);
							k++;
						}
					}
				}
			}else{
				if( dm_game_4p_team_flg ){
					//	チーム対抗戦
					load_TexPal( dm_4p_game_virus_s_bm0_0tlut );	//	Virus パレットロード
					load_TexBlock_4b( dm_4p_game_virus_s_bm0_0,48,10 );	//	Virus グラフィックロード
					for( i = 0,j = 46;i < 4;i ++ ){
						draw_Tex( j,221,34,10,0,0);
						j += 72;
					}

					//	チーム描画
					for(i = 0,j = 24;i < 4;i++){
						if( game_state_data[i].player_state[PS_TEAM_FLG] == TEAM_MARIO ){
							load_TexPal( dm_4p_game_team_aa_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_aa_bm0_0,64,13 );
							draw_Tex( j,191,55,13,0,0);
						}else{
							load_TexPal( dm_4p_game_team_bb_bm0_0tlut );
							load_TexBlock_4b( dm_4p_game_team_bb_bm0_0,64,13 );
							draw_Tex( j,191,52,13,0,0);
						}
						j += 72;
					}

					//	TEAM A 描画
					load_TexBlock_4b( &dm_4p_game_team_bm0_0[0],128,32 );
					load_TexPal( dm_4p_game_team_a_bm0_0tlut );
					draw_Tex( 24,9,128,32,0,0);
					draw_TexFlip( 24,41,128,2,0,0,flip_off,flip_on);
					//	TEAM B 描画
					load_TexPal( dm_4p_game_team_b_bm0_0tlut );
					draw_Tex( 168,9,128,32,0,0);
					draw_TexFlip( 168,41,128,2,0,0,flip_off,flip_on);
					//	勝利ゲージ
					load_TexPal( dm_4p_win_gage_bm0_0tlut );		//	勝数ゲージパレットロード
					load_TexTile_4b( &dm_4p_win_gage_bm0_0[0],68,24,0,0,67,23 );	//	勝数ゲーグラフィックロード
					draw_Tex( 32,16,66,22,2,0);
					draw_Tex(222,16,66,22,2,0);
					//	カプセルストック
					load_TexPal( dm_4p_cap_stock_bm0_0tlut );	//	カプセルストック パレットロード
					load_TexTile_4b( &dm_4p_cap_stock_bm0_0[0],40,16,0,0,39,15 );	//	カプセルストック グラフィックロード
					draw_Tex( 106,24,38,14,2,0);
					draw_Tex( 176,24,38,14,2,0);


					//	ウイルスグラフィックデータの読込み
					load_TexTile_4b( cap_tex[1],16,128,0,0,15,127 );
					for( i = 0;i < 2;i++ ){
						for(j = k = 0;j < 4;j++){
							if( story_4p_stock_cap[story_4p_name_flg[i]][j] >= 0 ){
								//	ストックがある場合
								load_TexPal( capsel_8_pal[story_4p_stock_cap[story_4p_name_flg[i]][j]] );	//	パレットロード
								//	カプセル描画
								draw_Tex( s_pos_4p[story_4p_name_flg[i]] + (k << 3),27,cap_size_8,cap_size_8,0,capsel_b << 3);
								k++;
							}
						}
					}

					j = 35;
					load_TexPal( TLUT_star );						//	星テクスチャロード
					load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	星テクスチャロード
					for(i = 0;i < 2;i++){
						dm_star_draw( j,19,win_count[i],0 );
						j += 190;
					}
				}else{
					//	バトルロイヤル
					load_TexPal( dm_4p_game_virus_bm0_0tlut );	//	Virus パレットロード
					load_TexBlock_4b( dm_4p_game_virus_bm0_0,48,12 );	//	Virus グラフィックロード
					for( i = 0,j = 31;i < 4;i ++ ){
						draw_Tex( j,192,40,12,0,0);
						j += 72;
					}

					j = 17;
					load_TexPal( dm_4p_win_gage_bm0_0tlut );		//	勝数ゲージパレットロード
					load_TexTile_4b( &dm_4p_win_gage_bm0_0[0],68,24,0,0,67,23 );	//	勝数ゲーグラフィックロード
					for(i = 0;i < 4;i++){
						draw_Tex( j,13,68,24,0,0);
						j += 72;
					}
					j = 22;
					load_TexPal( TLUT_star );						//	星テクスチャロード
					load_TexBlock_4b( &TEX_star_0_0[0],16,240 );	//	星テクスチャロード
					for(i = 0;i < 4;i++){
						dm_star_draw( j,16,win_count[i],0 );
						j += 72;
					}
				}
			}

			if( story_4p_demo_flg[0] ){
				demo_story_4p_draw();
			}else{
				for( i = 0;i < 4;i++ ){
					dm_virus_anime( &game_state_data[i],game_map_data[i] );	//	ウイルスアニメーション

					dm_game_graphic_p( &game_state_data[i],game_map_data[i] );

					load_TexBlock_4b( &TEX_virus_number_0_0[0],16,160 );
					dm_disp_virus_number( &game_state_data[i],game_state_data[i].map_x + 30,207,i);	//	残りウイルス数
					dm_4p_game_face_draw( &game_state_data[i],game_state_data[i].map_x + 7,206);	//	顔
				}
				for(i = 0;i < 4;i++){
					dm_game_graphic_effect( &game_state_data[i],i,1 );	//	WIN LOSE DRAW 連鎖数等の描画
				}
			}
#ifdef	DM_DEBUG_FLG
		}
		if( game_state_data[0].game_mode[dm_mode_now] == dm_mode_debug ){
			disp_debug_main_1p();
		}
#endif
		break;
	}
	//	デモ表示
	switch( evs_gamesel )
	{
	case	GSL_1DEMO:
	case	GSL_4DEMO:
		push_any_key_draw( 230,210 );	// PUSH ANY KEY
		break;
	case	GSL_2DEMO:
		push_any_key_draw( 128,210 );	// PUSH ANY KEY
		break;
	}

	/*------------------*/
	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ作成終了, ｸﾞﾗﾌｨｯｸﾀｽｸの開始
	gDPFullSync(gp++);			// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ最終処理
	gSPEndDisplayList(gp++);	// ﾃﾞｨｽﾌﾟﾚｲﾘｽﾄ終端
	osWritebackDCacheAll();		// dinamicｾｸﾞﾒﾝﾄのﾌﾗｯｼｭ

	gfxTaskStart(gt, gfx_glist[wb_flag], (s32)(gp - gfx_glist[wb_flag]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);


}
