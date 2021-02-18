// ///////////////////////////////////////////////////////////////////////////
//
//	Story-mode Message Program File
//
//  Author		: Katsuyuki Ohkura
//
//	Last Update	: 1999/12/13
//
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "graphic.h"
#include "joy.h"
#include "evsworks.h"
#include "story_data.h"

#include "dm_code_data.h"
#include "dm_font_main.h"


#define MES_BUF			1500			// 最大メッセージ数
#define LINE_BUF		70				// メッセージラインバッファ
#define KEY_WAIT_NORMAL	7				// キーウェイト（通常）
#define KEY_WAIT_PUSH	0				// キーウェイト（早送り）

//	NAB ADD //////////////////////
typedef	struct{
	u8	str_pos[16];					//	Ｘ座標
	u8	*str_grp[16];					//	文字グラフィックへのポインタ
}str_line;

static	str_line	line_mes_tex[LINE_BUF];	//	70列
static	s8	name_line_flg;					//	名前表示フラグ
static	s8	line_mes_max;					//	1列あたりの最大文字数を格納する変数
s8	disp_face_flg;							//	顔グラフィック表示用フラグ
//	NAB ADD //////////////////////


u8 face_number = 9;
u8 st_flag = 0;							// ストーリーモード用フラグ
u8 st_page = 0;							// ストーリーページ
u8 st_scr_stop = 0;						// 文字列進行のフラグ
s16 comple_font_y = 0;					// フォントＹ座標補正用



static s8 mes_line = -1;

static u16 now_moji_num = 0;					// 現在表示している文字位置
u8 moji_work[MES_BUF];							// 文字列保存ワーク

static u8  now_line_moji_num = 0;				// 現在処理しているラインの現在表示している文字番号
static u8  now_line = 0;						// 現在処理しているライン番号
static u8  disp_mes_line_num = 1;				// 表示ライン数
static s16 disp_mes_line[4]  = {-1,0,-1,-1};	// 表示ライン番号

static s8 mes_scr_adj_y = 0;					// スクロール時Ｙ座標調整用
static u8 key_wait_mode = 0;					// キー入力待ち
static u16 mes_wait = 0;						// メッセージ表示待ち

static u8 pr = 0,pg = 0,pb = 0,er = 255,eg = 255,eb = 255;	//	インテンシティ用変数

// テーブル変換
u8 change_font( u16 font_num )
{
	u8 num;

	// 文字コード＞テーブル番号変換
	if( font_num == 0x8141 ){				// 、
		num = 0;
	} else if( font_num == 0x8142 ){		// 。
		num = 1;
	} else if( font_num == 0x8148 ){		// ？
		num = 2;
	} else if( font_num == 0x8149 ){		// ！
		num = 3;
	} else if( font_num == 0x8151 ){		// ＿
		num = 4;
	} else if( font_num == 0x815b ){		// ー
		num = 5;
	} else if( font_num == 0x8195 ){		// ＆（未使用）
		num = 6;
	} else if( font_num == 0x8199 ){		// ☆（未使用）
		num = 7;
	} else if( font_num == 0x81a0 ){		// □（未使用）
		num = 8;
	} else if( font_num == 0x81a2 ){		// △（未使用）
		num = 9;
	} else if( font_num == 0x8140 ){		// 　(スペース)
		num = 10;
	} else if( font_num == 0x8145 ){		// ・
		num = 0xdb;
	} else if( font_num >= 0x824f && font_num <= 0x8258 ){		// ０～９
		num = font_num - 0x824f + 11;
	} else if( font_num >= 0x8260 && font_num <= 0x8279 ){		// Ａ～Ｚ
		num = font_num - 0x8260 + 21;
	} else if( font_num >= 0x8281 && font_num <= 0x829a ){		// ａ～ｚ
		num = font_num - 0x8281 + 47;
	} else if( font_num >= 0x829f && font_num <= 0x82f2 ){		// ぁ～を
		num = font_num - 0x829f + 73;
	} else if( font_num >= 0x8340 && font_num <= 0x8394 ){		// ア～ヴ
		num = font_num - 0x8340 + 156;
	} else {
		num = 0xff;
	}
	return num;
}


// フォント初期化
void init_font( void )
{
	mes_line = -1;
	now_moji_num = 0;

	now_line_moji_num = 0;
	now_line = 0;
	disp_mes_line_num = 1;
	disp_mes_line[0] = -1;	//	非表示の設定
	disp_mes_line[1] =  0;	//	表示中の設定
	disp_mes_line[2] = -1;	//	非表示の設定
	disp_mes_line[3] = -1;	//	非表示の設定

	comple_font_y = 0;		//	Ｙ座標補正値のクリア
	mes_scr_adj_y = 0;		//	スクロール時のＹ座標補正値のクリア
	key_wait_mode = 0;		//	キー制御を通常処理に設定
	mes_wait = 0;			//	メッセージ進行カウンタのクリア

	st_page = 0;			//	表示背景番号のクリア
	st_flag = 0;			//	タイル処理番号のクリア

//	NAB ADD //////////////////////
	line_mes_max  = 16;		//	１行の表示文字数
	disp_face_flg = 0;		//	顔表示フラグのクリア
	name_line_flg = 0;		//	名前表示フラグのクリア
	bzero(&line_mes_tex[0],sizeof(str_line) * LINE_BUF);	//	配列領域の０クリア
//	NAB ADD //////////////////////

}

//	インテンシティで使う変数の設定
void set_intencity_rgb( u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2 )
{
	pr = r1;	//	プリミティブカラー( 赤 )
	pg = g1;	//	プリミティブカラー( 緑 )
	pb = b1;	//	プリミティブカラー( 青 )
	er = r2;	//	環境カラー( 赤 )
	eg = g2;	//	環境カラー( 緑 )
	eb = b2;	//	環境カラー( 青 )
}

//	インテンシティ使用の設定
void set_intencity_mode( void )
{
	gSPDisplayList( gp++, Intensity_XNZ_Texture_dl);	//	インテンシティ使用のための設定
	gDPSetPrimColor( gp++, 0,0, pr*(100-evs_fadecol)/100,pg*(100-evs_fadecol)/100,pb*(100-evs_fadecol)/100,255 );	//	プリミティブカラーの設定
	gDPSetEnvColor( gp++, er*(100-evs_fadecol)/100,eg*(100-evs_fadecol)/100,eb*(100-evs_fadecol)/100,255 );			//	環境カラーの設定
}

// フォントデータ作成(文字列制御コードを抜いて、文字列のグラフィックだけを設定する)
void font_make( u8 *moji )
{
	int n = 0,i,j;
	u16 num,font_num,font_next;
	u8 cnt = 0;
	u8 err_work[3];

	if( mes_line == -1 ){
		strcpy( moji_work,moji );				// 最初
		mes_line ++;
	} else {
		strcat( moji_work,moji );				// 追加
	}

	while( moji[n] != '\0' ){
		font_num = (moji[n] << 8) | moji[n+1];
		font_next = (moji[n+2] << 8) | moji[n+3];

		// 文字コード＞テーブル番号変換
		num = change_font( font_num );
		switch( font_num ){
		case	0x89fc:			// 改
		case	0x8fc1:			// 消
			cnt = 0;
			mes_line ++;
			break;
		case	0x8ae7:			//	顔
			line_mes_max = 12;
		case	0x8af8:			//	旗
		case	0x96bc:			//	名
			n += 2;
			break;
		case	0x91bd:			//	多
			line_mes_max = 16;
			break;
		case	0x8e6e:			//	始
		case	0x8e7e:			//	止
		case	0x8f49:			//	終
		case	0x95c5:			//	頁
			break;
		case	0x91d2:			//	待
			n += 4;
			break;
		default:				//	その他
			if( num == 255 ){	//	定義されていない文字の場合
				err_work[0] = moji[n];
				err_work[1] = moji[n+1];
				err_work[2] = '\0';
#ifdef	DM_DEBUG_FLG
				osSyncPrintf("[%s]は定義されていません！\n",err_work);
#endif
			}
			if( cnt == 0 ){	//	改行され、列の最初だった場合
				for(i = 0;i < 16;i++){
					line_mes_tex[mes_line].str_pos[cnt] = cnt << 4;				//	X座標( 16ドットずつずれる
					line_mes_tex[mes_line].str_grp[i] = &dm_font_16[0x0a][0];	//	透明グラフィック
				}
			}
			line_mes_tex[mes_line].str_pos[cnt] = cnt << 4;						//	X座標( 16ドットずつずれる
			line_mes_tex[mes_line].str_grp[cnt] = &dm_font_16[num][0];			//	文字グラフィックの設定
			// 自動改行判定
			if( cnt < line_mes_max ){		//	最大表示数以下の場合
				cnt ++;						//	１文字進める
			} else {						//	最大表示数以上の場合
				if( font_next != 0x89fc ){	//	次の文字コードが改行でない場合
					cnt = 0;				//	文字を最初に戻す( １行の先頭 )
					mes_line ++;			//	改行
				}
			}
			break;
		}
		n += 2;	//	ポインタの加算( n自体はポインタではない )
	}

}


// メッセージキー入力待ちの描画
void wait_button( void )
{
	static u8 cnt = 0;

	if( key_wait_mode ){	//	キー入力待ちになっていた場合
		disp_tex_4bt( &w_button_bm0_0[32*(cnt/8)],w_button_bm0_0tlut,8,8,288,216,SET_PTD );	//ボタン入力を描画する
		cnt = ( cnt + 1 ) % 24;	//	アニメーションをさせる処理
	}
}


// メッセージ制御
s8 calc_message( void )
{
	int i;
	u16 font_num,font_next;

	if( st_scr_stop ){	//	文字列進行フラグが １(停止)だったら -1を返す(処理を行わない)
		return -1;
	}

	// 文字列終了チェック
	if( moji_work[now_moji_num] != '\0' ){
		font_num  = (moji_work[now_moji_num  ] << 8) | moji_work[now_moji_num+1];	//	現在の文字コード(フォント番号)の取得
		font_next = (moji_work[now_moji_num+2] << 8) | moji_work[now_moji_num+3];	//	次の文字コード(フォント番号)の取得
		switch( font_num ){
		case	0x8ae7:			//	顔
			face_number = (moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0');			//	表示顔番号の取得
			disp_face_flg = 1;													//	顔表示フラグを立てる
			line_mes_max = 12;													//	１行の最大表示文字数を１２に設定
			now_moji_num += 4;													//	文字列ポインタを４バイト進める( now_moji_num自体はポインタではない )
			break;
		case	0x8af8:			//	旗											//	タイル制御フラグの設定( タイルを上に移動させる設定をしている )
			st_flag |=	(moji_work[now_moji_num+2]-'0') * 10 +
						(moji_work[now_moji_num+3]-'0');
			now_moji_num += 4;													//	文字列ポインタを４バイト進める( now_moji_num自体はポインタではない )
			return -1;															//	この処理の時は文字列を進ませないためにここで処理を中止する。
		case	0x96bc:			//	名
			now_moji_num += ((moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0')) * 2;	//	名前の表示文字数分だけ文字列を進める
			now_moji_num += 4;													//	文字列ポインタを４バイト進める
			name_line_flg = 1;													//	名前表示フラグを立てる
			break;
		case	0x91bd:			//	多
			disp_face_flg = 0;													//	顔表示フラグをクリア
			line_mes_max = 16;													//	１行の最大表示文字数を１６に設定
			now_moji_num += 2;													//	文字列ポインタを２バイト進める
			break;
		case	0x8e6e:			//	始
			st_page = 255;														//	タイル制御フラグの設定( タイルが上間であがったら、メインループを抜けるようになる )
			now_moji_num += 2;													//	文字列ポインタを２バイト進める
			break;
		case	0x8e7e:			//	止
			key_wait_mode = 1;													//	キー入力待ちフラグを立てる
			if( joyupd[main_joy[0]] & CONT_A ){									//	もし、Ａボタンが押されていた場合
				key_wait_mode = 0;												//	キー入力待ちフラグをクリア
				now_moji_num += 2;												//	文字列ポインタを２バイト進める
			}
			return -1;
		case	0x8fc1:			//	消
			now_line_moji_num = 0;												//	文字列ポインタを０に設定する( １行の先頭にする )
			now_line ++;														//	現在表示中のライン番号を進める

			disp_mes_line[0] = -1;												//	非表示の設定
			disp_mes_line[1] =  now_line;										//	表示中の設定
			disp_mes_line[2] = -1;												//	非表示の設定
			disp_mes_line[3] = -1;												//	非表示の設定
			disp_mes_line_num = 1;												//	表示行数を１に設定
			name_line_flg = 0;													//	名前表示フラグのクリア

			now_moji_num += 2;													//	文字列ポインタを２バイト進める
			break;
		case	0x91d2:			// 待(現在未使用)
			mes_wait =	(moji_work[now_moji_num+2]-'0') * 1000 +
						(moji_work[now_moji_num+3]-'0') * 100 +
						(moji_work[now_moji_num+4]-'0') * 10 +
						(moji_work[now_moji_num+5]-'0');
			now_moji_num += 6;
			break;
		case	0x95c5:			//	貢
			st_page ++;															//	次の背景番号の設定
			now_moji_num += 2;													//	文字列ポインタを２バイト進める
			break;
		default:	//	その他
			mes_scr_adj_y = ( mes_scr_adj_y - 4 >= 0 )?mes_scr_adj_y - 4:0;
			if( !mes_wait ){
				// 文字改行 ＆ 終了 ＆ 自動改行
				if( font_num == 0x89fc || (font_num == 0x8f49 && moji_work[now_moji_num+2] != '\0') || now_line_moji_num >= line_mes_max ){
					// １３文字目の後に改行があったら無視する為に２bit先に送る
					if( now_line_moji_num >= line_mes_max  && font_next == 0x89fc ){
						now_moji_num += 2;
					}
					// １３文字目の後に止めが無かったら無視する為に２bit先に送る
					if( now_line_moji_num >= line_mes_max  && font_next == 0x8e7e ){
						now_line_moji_num ++;
					}else{
						now_line_moji_num = 0;	//	文字列ポインタを０に設定する( １行の先頭にする )
						now_line ++;			//	現在処理しているライン番号の変更
						// 表示ライン更新
						if( disp_mes_line_num >= 3 ){							//	3行以上表示されていた場合
							if( !name_line_flg ){								//	名前表示フラグが立っていない場合
								for( i = 0; i < 3; i ++ ){						//	表示行を１行づつずらす
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}else{												//	名前表示フラグが立っていた場合
								for( i = 2; i < 3; i ++ ){						//	下二行だけ表示行をずらす
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}
							disp_mes_line[3] = now_line;						//	表示行に表示する行番号の設定
							mes_scr_adj_y = 16;									//	文字列をスクロールさせるカウンタの設定
						} else {
							disp_mes_line_num ++;								//	次の行を表示するように設定
							disp_mes_line[disp_mes_line_num] = now_line;		//	表示行に表示する行番号の設定
						}
					}
				} else {						// 文字送り
					now_line_moji_num ++;
				}
				now_moji_num += 2;

				// 早送り
				if( joynew[main_joy[0]] & CONT_A ){	//	Ａボタンが押されていた場合
					mes_wait = KEY_WAIT_PUSH;		//	ウェイト時間の設定KEY_WAIT_PUSH( 0 )
				} else {
					mes_wait = KEY_WAIT_NORMAL;		//	ウェイト時間の設定KEY_WAIT_NORMAL( 7 )
				}

			} else {
				mes_wait --;
			}
			break;
		}
		return	1;	//	文字列進行中
	}else{
		return	-1;	//	文字列停止中
	}
}

#ifdef	DM_DEBUG_FLG
// メッセージ制御
s8 calc_skip_message( void )
{
	int i;
	u16 font_num,font_next;

	while(1){
	// 文字列終了チェック
	if( moji_work[now_moji_num] != '\0' ){
		font_num  = (moji_work[now_moji_num  ] << 8) | moji_work[now_moji_num+1];
		font_next = (moji_work[now_moji_num+2] << 8) | moji_work[now_moji_num+3];
		switch( font_num ){
		case	0x8ae7:			//	顔
			line_mes_max = 12;
			now_moji_num += 4;
			break;
		case	0x8af8:			//	旗
			now_moji_num += 4;
			return -1;
		case	0x96bc:			//	名
			now_moji_num += ((moji_work[now_moji_num+2]-'0') * 10 + (moji_work[now_moji_num+3]-'0')) * 2;
			now_moji_num += 4;
			break;
		case	0x91bd:			//	多
			disp_face_flg = 0;
			line_mes_max = 16;
			now_moji_num += 2;
			break;
		case	0x8e6e:			//	始
			st_page = 255;
			now_moji_num += 2;
			return	-1;
		case	0x8e7e:			//	止
			now_moji_num += 2;
			break;
		case	0x8fc1:			//	消
			now_line_moji_num = 0;
			now_line ++;

			disp_mes_line[0] = -1;
			disp_mes_line[1] =  now_line;
			disp_mes_line[2] = -1;
			disp_mes_line[3] = -1;
			disp_mes_line_num = 1;
			name_line_flg = 0;

			now_moji_num += 2;
			break;
		case	0x91d2:			// 待
			now_moji_num += 6;
			break;
		case	0x95c5:			//	貢
			st_page ++;
			now_moji_num += 2;
			return	-1;
		default:	//	その他
			mes_scr_adj_y = ( mes_scr_adj_y - 4 >= 0 )?mes_scr_adj_y - 4:0;
				// 文字改行 ＆ 終了 ＆ 自動改行
				if( font_num == 0x89fc || (font_num == 0x8f49 && moji_work[now_moji_num+2] != '\0') || now_line_moji_num >= line_mes_max ){
					// １３文字目の後に改行があったら無視する為に２bit先に送る
					if( now_line_moji_num >= line_mes_max  && font_next == 0x89fc ){
						now_moji_num += 2;
					}
					// １３文字目の後に止めが無かったら無視する為に２bit先に送る
					if( now_line_moji_num >= line_mes_max  && font_next == 0x8e7e ){
						now_line_moji_num ++;
					}else{
						now_line_moji_num = 0;
						now_line ++;
						// 表示ライン更新
						if( disp_mes_line_num >= 3 ){
							if( !name_line_flg ){
								for( i = 0; i < 3; i ++ ){
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}else{
								for( i = 2; i < 3; i ++ ){
									disp_mes_line[i] = disp_mes_line[i+1];
								}
							}
							disp_mes_line[3] = now_line;
							mes_scr_adj_y = 16;
						} else {
							disp_mes_line_num ++;
							disp_mes_line[disp_mes_line_num] = now_line;
						}
					}
				} else {						// 文字送り
					now_line_moji_num ++;
				}
				now_moji_num += 2;
			break;
		}
	}else{
		return	-1;
	}
	}
}
#endif


// メッセージ表示
void disp_message( void )
{
	int i,j,st;
	s8	x_p;
	s8	adj_y[4] = {0,0,0,0};

	wait_button();							//	キー待ち状態の時のボタン描画

	set_intencity_mode();					//	インテンシティ使用のための設定

	if( mes_scr_adj_y <= 0 ){				//	文字列がスクロール中でない場合
		st = 1;								//	表示開始行を１にする
	} else {								//	文字列がスクロール中の場合
		if( name_line_flg ){				//	名前表示フラグが立っていた場合
			st = 1;							//	表示開始行を１にする
			for(i = 2;i < 3;i++){
				adj_y[i] = mes_scr_adj_y;	//	Ｙ座標移動値の設定
			}
		}else{
			st = 0;							//	表示開始行を０にする
			for(i = 0;i < 3;i++){
				adj_y[i] = mes_scr_adj_y;	//	Ｙ座標移動値の設定
			}
		}
	}

	//	文字列描画開始Ｘ座標
	if( disp_face_flg ){					//	顔表示フラグが立っていた場合
		x_p = 80;
	}else{									//	顔表示フラグが立っていない場合
		x_p = 32;
	}



	// 処理済みライン( １行全部表示されている )
	for( i = st; i < disp_mes_line_num; i ++ ){
		if( disp_mes_line[i] != -1 ){		//	非表示状態でない場合
			for(j = 0;j < 16;j++){
				load_TexBlock_i( line_mes_tex[disp_mes_line[i]].str_grp[j],16,16 );											//	フォント読込み
				draw_Tex(x_p + line_mes_tex[disp_mes_line[i]].str_pos[j],176+i*16-16+adj_y[i]+comple_font_y,16,16,0,0 );	//	フォント描画
			}
		}
	}
	// 処理中ライン
	for(j = 0;j < now_line_moji_num;j++){
		load_TexBlock_i( line_mes_tex[disp_mes_line[disp_mes_line_num]].str_grp[j],16,16 );											//	フォント読込み
		draw_Tex(x_p + line_mes_tex[disp_mes_line[disp_mes_line_num]].str_pos[j],176+i*16-16+adj_y[2]+comple_font_y,16,16,0,0 );	//	フォント描画
	}
}
