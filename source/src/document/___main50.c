// ///////////////////////////////////////////////////////////////////////////
//
//	Story-mode Program File
//
//	Created by Shizuoka?
//	Modifed by Katsuyuki Ohkura
//
//	Modifed by Hiroyuki Watanabe
//
//	Last Update: 1999/12/11
//
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"
#include "dm_code_data.h"

#include "o_segment.h"
#include "o_static.h"
#include "bg_dat_extern.h"
#include "story_data.h"
#include "st_message.h"


extern u8 face_number;			// 顔番号

enum {				//	内部制御の番号
	TSTAT_OPEN,
	TSTAT_ANIMA,
	TSTAT_SET,
	TSTAT_LOOP,
	TSTAT_GAME,
	TSTAT_DEMO,
	TSTAT_CLOSE,
	TSTAT_LOOPEND,
};
typedef struct {
	u8 *st_seg;		// セグメント始め
	u8 *ed_seg;		// セグメント終り
	PIC tile;		// タイルパターン
	u8 bg_num;		// 背景数
} ST_STAGE_DAT;

static uObjBg st_bg[12];			// 背景
static uObjBg st_ch[12];			// きゃら
static uObjBg st_cld;				// 雲
static u8 st_bg_all_num = 0;		// 今の面の絵は配列の何番目？
static u8 now_story = 0;			// ストーリー番号
static u8 st_mode_flow = 0;			// ストーリーモード流れ制御
static u8 tile_adj_y = 0;			// タイルＹ座標調整用
static u8 pic_num = 0;				// 背景枚数
static u8 *addr_pat[12];			// 背景テクスチャアドレス
static u16 *addr_pal[12];			// 背景パレットアドレス
static s16 comple_y = 0;			// 全体調整用Ｙ座標
static s16 p1_x = 200,p1_y = 50;	// キャラ１座標
static s16 bg_x = 0,  bg_y = 0;		// 背景座標
static s16 cl_x = 29, cl_y = 163;	// 雲座標
static u8 cannon_on = 0;			// 大砲発射！！
static s8 pino_mouth_flg;			// ピノキオクチパクフラグ
static s8 pino_mouth_count;			// ピノキオクチパクカウンタ
static ST_STAGE_DAT *st_p;			// ステージデータポインタ

// メッセージ( 文章の制御文字は st_message.c で)

static	u8	st_mes_01[] = "多ぽかぽか　おてんき改おひるね　マリオ。止改けれども　ルイージ　おおあわて。止改なにをそんなに　あわてているの。止消顔13名04ルイージ改にいさん！　にいさん！改ピーチひめがたいへんだ！止\
頁旗01消多すやすやねむるピーチひめ。止改キノピオたちは　しんぱいそう。改止そこへ　マリオが　やってきた。止消顔00名04キノピオ改あっマリオさん！改みてください止改ひめが改たいへんなことに・・。止\
頁旗01改ピーチひめが改おさんぽちゅう止改おねむそうの　はなを改かいじゃったんです。止改それからずっと　ひめは改ねむりっぱなし。止\
頁旗01改でも　おはようそうの改はながあれば　だいじょうぶ止ひめをおこすことが改できるはずです。止改ところが・・・。止\
頁旗01改おはようそうは　だれかに改もちさられていました。止改そのかわりに　このてがみが改のこされていたのです。止\
頁旗01消顔11名03クッパ改ガハハ！！改ざんねんだったな　マリオ！止おはようそうは　わがはいが改うばってやったもんね！止改おまえみたいなヤブいしゃに改ようはない！　ガハハ！止\
頁旗01消多またも　クッパのわるだくみ。止\
頁旗01消ドクターマリオの改でばんだぞ！！止\
頁旗01消多あわてて　とびだす改ドクターマリオ。止改めざすは改まおうクッパの　おしろ。止改おはようそうを　とりもどせ！止\
頁旗01消せっせと　いそぐ　マリオの改まえに　ひょっこり　ノコノコ改あらわれた！止消顔02名04ノコノコ改マリオさんじゃ改ないッスか！止改ひとつオイラと改しょうぶしませんか？止始旗01";

static u8 st_mes_02[] = "多クッパの　おしろは改まだまだ　とおい。止改まわりは　こだかい　おかだらけ。止改とつぜん　おかの改ほうだい　ドカーーン！！旗08止\
頁旗01消なにかが　こちらへ　とんでくる。止改とんで　きたのは改ボムへい　みたい！？止消顔03名02ボムへい改ここから　さきへは改とうしません！！止始旗01";

static u8 st_mes_03[] = "多とうとう　うみまで　やってきた。止改うみのむこうは　クッパとう。止改マリオは　うみに　とびこんだ！！止\
頁旗01消ふかい　ふかい　うみのそこ。止改きれいだなんて　おもっていたら改おおきな　おさかな　やってきた。止改おおきなふぐの　プクプクは改なんでも　たべちゃう改くいしんぼう。止\
消顔04名04プクプク改おなかが　すいたなー改たべもの　もってなーい？止改もってないなら改おまえを　たべちゃうぞ。止始旗01";

static u8 st_mes_04[] = "多やっとの　おもいで　うみを　こえ改クッパのしまに　たどりつく。止改ホッとひといき　おつかれマリオ。止改おしろはまだかと　めをこらす。止改おおきなおめめにうつるのは改くもまでとどく　たかいやま。止改そりゃないよ！！止\
頁旗01消そらまで　つづく　たかーい　やま改えっちら　おっちら改のぼっていくと止改とつぜん　じめんが改うごきだし止改なかから改チョロプー　とびだした！止\
消顔05名05チョロプー改ビックリした？！止改オレさまと　しょうぶだ！止始旗01";

static u8 st_mes_05[] = "多こんな　やまみち改へっちゃらマリオ　げんきいっぱい改かけのぼる！止改いきおいあまって　くものうえ。止\
頁旗01消おっこちないよう　ちゅういして改スイスイ　すすもう　くものみち。止改きこえて　くるのは　なんのおと？止改そらの　うえまで　おじゃまむし。止\
消顔06名08プロペラヘイホー改・・・・・・・。止始旗01";

static u8 st_mes_06[] = "多したまで　とどく　ながーいツタ。止改ゆっくり　ゆっくり　おりてみる。止改そこは　いちめん　ふかいもり。止\
頁旗01消ここまでくれば　だいじょうぶ。止改マリオは　ツタから　とびおりた！止改とびおりたまではよかったが改おさんぽハナちゃん　ふんづけた。止\
消顔07名05ハナちゃん改あーーん改ふんづけられちゃいました。止ハナちゃん　とっても改おこったです！止始旗01";

static u8 st_mes_07[] = "多ふかい　ふかい　もりのなか改ふるいやしきをみつけたよ。止\
頁旗01消あたりは　まっくら改なんにも　みえん。止改おっかなびっくり改たんけんマリオ。止改なんだか　せすじがさむくない？止改とってもとっても　ヤなかんじ。止\
消顔08名03テレサ改ケケケケケッ！止始旗01";

static u8 st_mes_08[] = "多やしきで　みつけた改ひみつの　つうろ。止改ここを　すすめば改ちかみち　かもね。止\
頁旗01消くらーい　くらーい　ちかつうろ改おくには　どかんが　ありました。止改イヤな　よかんは改スバリ　てきちゅう！改あらわれたのは　パックンフラワー止\
消顔09名08パックンフラワー改パックン　パックン改ペローリ　ペロリ止始旗01";

static u8 st_mes_09[] = "多どかんを　ぬけると改アッチッチッ！！改ここはいったい　どこだろう？止\
頁旗01消なんと　ビックリ改クッパの　おしろ。止改ここまで　くれば改もう　らくしょう？止改そうは　とんやが　おろしません！止\
消顔10名04カメック改キーッ！！止改まさか　こんなとこまで改やってくるなんて。止改クッパおぼっちゃまの改ところへは　いかせません！止始旗01";

static u8 st_mes_10[] = "多おしろの　なかは　ワナだらけ改ドクターマリオ　ききいっぱつ！！止\
頁旗01消ついたところは　おはようそうを改うばった　ちょうほんにん止改マリオの　しゅくてき改クッパの　おへや。止改さいごの　たたかいの改はじまりです！止\
消顔11名03クッパ改ムムム・・・　マリオ改もうやってきたのか。止改しかし改おまえなんかが止改わがはいに　かなうわけが改ないわ！　ガハハッ！！止\
改おはようそうも　ぜーったい改わたさんもんね！止改　　　　　　　いくぞ！！止始旗01";

static u8 st_mes_ed[] = "多おはようそうを　てにいれた改マリオは　さっそく改ピーチのしろに　むかい、止改ぶじ　ピーチを改めざめさせることが改できました。止始旗01";



// ステージデータ
static ST_STAGE_DAT st_dat[] = {
//	ロム開始アドレス・ロム終了アドレス・タイルグラフィックデータアドレス・タイルパレットデータアドレス・使用背景数
	_bg_dat_01SegmentRomStart, _bg_dat_01SegmentRomEnd, tile_01_bm0_0, tile_01_bm0_0tlut, 10,
	_bg_dat_02SegmentRomStart, _bg_dat_02SegmentRomEnd, tile_02_bm0_0, tile_02_bm0_0tlut, 2,
	_bg_dat_03SegmentRomStart, _bg_dat_03SegmentRomEnd, tile_03_bm0_0, tile_03_bm0_0tlut, 2,
	_bg_dat_04SegmentRomStart, _bg_dat_04SegmentRomEnd, tile_04_bm0_0, tile_04_bm0_0tlut, 2,
	_bg_dat_05SegmentRomStart, _bg_dat_05SegmentRomEnd, tile_05_bm0_0, tile_05_bm0_0tlut, 2,
	_bg_dat_06SegmentRomStart, _bg_dat_06SegmentRomEnd, tile_05_bm0_0, tile_05_bm0_0tlut, 2,
	_bg_dat_07SegmentRomStart, _bg_dat_07SegmentRomEnd, tile_07_bm0_0, tile_07_bm0_0tlut, 2,
	_bg_dat_08SegmentRomStart, _bg_dat_08SegmentRomEnd, tile_08_bm0_0, tile_08_bm0_0tlut, 2,
	_bg_dat_09SegmentRomStart, _bg_dat_09SegmentRomEnd, tile_08_bm0_0, tile_08_bm0_0tlut, 2,
	_bg_dat_10SegmentRomStart, _bg_dat_10SegmentRomEnd, tile_10_bm0_0, tile_10_bm0_0tlut, 3,
	_bg_dat_edSegmentRomStart, _bg_dat_edSegmentRomEnd, tile_10_bm0_0, tile_10_bm0_0tlut, 1,
};


// 顔配列
static PIC faces[] = {
///	{ st_face_ff_bm0_0, st_face_ff_bm0_0tlut },		// ×
	{ st_face_13_bm0_0, st_face_13_bm0_0tlut },		// キノピオ
	{ st_face_01_bm0_0, st_face_01_bm0_0tlut },		// マリオ
	{ st_face_02_bm0_0, st_face_02_bm0_0tlut },		// ノコノコ
	{ st_face_03_bm0_0, st_face_03_bm0_0tlut },		// ボム兵
	{ st_face_04_bm0_0, st_face_04_bm0_0tlut },		// プクプク
	{ st_face_05_bm0_0, st_face_05_bm0_0tlut },		// チョロブー
	{ st_face_06_bm0_0, st_face_06_bm0_0tlut },		// プロペラヘイホー
	{ st_face_07_bm0_0, st_face_07_bm0_0tlut },		// ハナちゃん
	{ st_face_08_bm0_0, st_face_08_bm0_0tlut },		// テレサ
	{ st_face_09_bm0_0, st_face_09_bm0_0tlut },		// パックンフラワー
	{ st_face_10_bm0_0, st_face_10_bm0_0tlut },		// カメック
	{ st_face_11_bm0_0, st_face_11_bm0_0tlut },		// クッパ
	{ st_face_12_bm0_0, st_face_12_bm0_0tlut },		// ピーチ
	{ st_face_14_bm0_0, st_face_14_bm0_0tlut },		// ルイージ
};


// 背景データ
static PIC2 st_bg_data[] = {
	{ st_back_op_1_bm0_0, st_back_op_1_bm0_0tlut, 240, 160,40, 0 },		//	1-1
	{ st_back_op_2_bm0_0, st_back_op_2_bm0_0tlut, 240, 160,40, 0 },		//	1-2
	{ st_back_op_3_bm0_0, st_back_op_3_bm0_0tlut, 160, 160,80, 0 },		//	1-3
	{ st_back_op_4_bm0_0, st_back_op_4_bm0_0tlut, 240, 160,40, 0 },		//	1-4
	{ st_back_op_5_bm0_0, st_back_op_5_bm0_0tlut, 240, 160,40, 0 },		//	1-5
	{ st_back_op_6_bm0_0, st_back_op_6_bm0_0tlut, 240, 160,40, 0 },		//	1-6
	{ st_back_op_7_bm0_0, st_back_op_7_bm0_0tlut, 160, 160,80, 0 },		//	1-7
	{ st_back_op_8_bm0_0, st_back_op_8_bm0_0tlut, 240, 160,40, 0 },		//	1-8
	{ st_back_01_1_bm0_0, st_back_01_1_bm0_0tlut, 240, 160,40, 0 },		//	1-9
	{ st_back_01_2_bm0_0, st_back_01_2_bm0_0tlut, 320, 160, 0, 0 },		//	1-10
	{ st_back_02_1_bm0_0, st_back_02_1_bm0_0tlut, 160, 160,80, 0 },		//	2-1
	{ st_back_02_2_bm0_0, st_back_02_2_bm0_0tlut, 320, 160, 0, 0 },		//	2-2
	{ st_back_03_1_bm0_0, st_back_03_1_bm0_0tlut, 240, 160,40, 0 },		//	3-1
	{ st_back_03_2_bm0_0, st_back_03_2_bm0_0tlut, 320, 160, 0, 0 },		//	3-2
	{ st_back_04_1_bm0_0, st_back_04_1_bm0_0tlut, 240, 160,40, 0 },		//	4-1
	{ st_back_04_2_bm0_0, st_back_04_2_bm0_0tlut, 320, 160, 0, 0 },		//	4-2
	{ st_back_05_1_bm0_0, st_back_05_1_bm0_0tlut, 240, 160,40, 0 },		//	5-1
	{ st_back_05_2_bm0_0, st_back_05_2_bm0_0tlut, 320, 160, 0, 0 },		//	5-2
	{ st_back_06_1_bm0_0, st_back_06_1_bm0_0tlut, 160, 160,80, 0 },		//	6-1
	{ st_back_06_2_bm0_0, st_back_06_2_bm0_0tlut, 320, 160, 0, 0 },		//	6-2
	{ st_back_07_1_bm0_0, st_back_07_1_bm0_0tlut, 240, 160,40, 0 },		//	7-1
	{ st_back_07_2_bm0_0, st_back_07_2_bm0_0tlut, 320, 160, 0, 0 },		//	7-2
	{ st_back_08_1_bm0_0, st_back_08_1_bm0_0tlut, 160, 160,80, 0 },		//	8-1
	{ st_back_08_2_bm0_0, st_back_08_2_bm0_0tlut, 320, 160, 0, 0 },		//	8-2
	{ st_back_09_1_bm0_0, st_back_09_1_bm0_0tlut, 160, 160,80, 0 },		//	9-1
	{ st_back_09_2_bm0_0, st_back_09_2_bm0_0tlut, 320, 160, 0, 0 },		//	9-2
	{ st_back_10_1_bm0_0, st_back_10_1_bm0_0tlut, 240, 160,40, 0 },		//	10-1
	{ st_back_10_2_bm0_0, st_back_10_2_bm0_0tlut, 320, 160, 0, 0 },		//	10-2
	{ st_back_03_2_bm0_0, st_back_03_2_bm0_0tlut, 320, 160, 0, 0 },		//	10-3(ダミー)
	{ st_back_ed_1_bm0_0, st_back_ed_1_bm0_0tlut, 240, 160,40, 0 },		//	ED
};


// キャラデータ
static PIC2 st_ch_data[] = {
	{ st_char_01_21_bm0_0, st_char_01_21_bm0_0tlut, 96,140, 181, 18 },		// 1-2 マリオ
	{ st_char_01_22_bm0_0, st_char_01_22_bm0_0tlut, 72, 94,	 56, 63 },		// 1-2 ノコノコ
	{ st_char_02_21_bm0_0, st_char_02_21_bm0_0tlut,104,130, 182, 25 },		// 2-2 マリオ
	{ st_char_02_22_bm0_0, st_char_02_22_bm0_0tlut, 88,117,	 61, 44 },		// 2-2 ボム兵
	{ st_char_03_21_bm0_0, st_char_03_21_bm0_0tlut,128, 84,	 24, 38 },		// 3-2 マリオ
	{ st_char_03_22_bm0_0, st_char_03_22_bm0_0tlut, 96, 93, 203, 23 },		// 3-2 プクプク
	{ st_char_04_21_bm0_0, st_char_04_21_bm0_0tlut,128,130, 168, 28 },		// 4-2 マリオ
	{ st_char_04_22_bm0_0, st_char_04_22_bm0_0tlut, 88, 69,	 42, 50 },		// 4-2 チョロブー
	{ st_char_05_21_bm0_0, st_char_05_21_bm0_0tlut,120,138, 165, 14 },		// 5-2 マリオ
	{ st_char_05_22b_bm0_0, st_char_05_22b_bm0_0tlut,48,29,	 60,120 },		// 5-2 ヘイホー影
	{ st_char_05_22_bm0_0, st_char_05_22_bm0_0tlut, 64, 83,	 56, 32 },		// 5-2 ヘイホー
	{ st_char_06_21_bm0_0, st_char_06_21_bm0_0tlut,120,116, 144,  2 },		// 6-2 マリオ
	{ st_char_06_22_bm0_0, st_char_06_22_bm0_0tlut,224,149,	 58, 12 },		// 6-2 はなちゃん
	{ st_char_07_21_bm0_0, st_char_07_21_bm0_0tlut,176,158, 142,  3 },		// 7-2 マリオ
	{ st_char_07_22_bm0_0, st_char_07_22_bm0_0tlut,120,113,	 61, 41 },		// 7-2 テレサ
	{ st_char_08_21_bm0_0, st_char_08_21_bm0_0tlut,128,160, 142,  1 },		// 8-2 マリオ
	{ st_char_08_22_bm0_0, st_char_08_22_bm0_0tlut, 88, 72,	 52,  2 },		// 8-2 パックンフラワー
	{ st_char_09_21_bm0_0, st_char_09_21_bm0_0tlut,112,131, 195, 30 },		// 9-2 マリオ
	{ st_char_09_22_bm0_0, st_char_09_22_bm0_0tlut,128, 99,	 33, 54 },		// 9-2 カメック
	{ st_char_10_31_bm0_0, st_char_10_31_bm0_0tlut,128,144, 195, 17 },		//10-3 マリオ
	{ st_char_10_32_bm0_0, st_char_10_32_bm0_0tlut,152,160,	 20,  1 },		//10-3 クッパ
};


// 背景とキャラの対応
static u8 st_scene_data[][3] = {
	 0,255,255,		// op-1
	 0,255,255,		// op-2
	 0,255,255,		// op-3
	 0,255,255,		// op-4
	 0,255,255,		// op-5
	 0,255,255,		// op-6
	 0,255,255,		// op-7
	 0,255,255,		// op-8
	 1,255,255,		//	1-1
	 2,0,1,			//	1-2
	 3,255,255,		//	2-1
	 4,2,3,			//	2-2
	 5,255,255,		//	3-1
	 6,4,5,			//	3-2
	 7,255,255,		//	4-1
	 8,6,7,			//	4-2
	 9,255,255,		//	5-1
	10,8,9,			//	5-2
	11,255,255,		//	6-1
	12,11,12,		//	6-2
	13,255,255,		//	7-1
	14,13,14,		//	7-2
	15,255,255,		//	8-1
	16,15,16,		//	8-2
	17,255,255,		//	9-1
	18,17,18,		//	9-2
	19,255,255,		// 10-1
	20,19,20,		// 10-2
	21,255,255,		// 10-3
	22,255,255,		// ed-1
};


// メッセージテーブル
static u8 *st_meses[11] = {
	st_mes_01,
	st_mes_02,
	st_mes_03,
	st_mes_04,
	st_mes_05,
	st_mes_06,
	st_mes_07,
	st_mes_08,
	st_mes_09,
	st_mes_10,
	st_mes_ed,
};




// ＢＧパラメータセqット
void set_bg_param( uObjBg *bg, u8 *tex, u16 load, u8 size, u16 width, u16 height, s16 posx, s16 posy, u16 scx, u16 scy )
{
	bg->s.imageX		= 0		<< 5;		// BG image の左上位置 X 座標  (u10.5)
	bg->s.imageW		= width << 2;		// BG image の幅			   (u10.2)
	bg->s.frameX		= posx	<< 2;		// 転送フレームの左上位置	   (s10.2)
	bg->s.frameW		= width << 2;		// 転送フレームの幅			   (u10.2)
	bg->s.imageY		= 0		 << 5;		// BG image の左上位置 Y 座標  (u10.5)
	bg->s.imageH		= height << 2;		// BG image の高さ			   (u10.2)
	bg->s.frameY		= posy	 << 2;		// 転送フレームの左上位置	   (s10.2)
	bg->s.frameH		= height << 2;		// 転送フレームの高さ		   (u10.2)
	bg->s.imagePtr		= (u64 *)tex;		// BG image 左上角のテクスチャアドレス
	bg->s.imageLoad		= load;				// LoadBlock, LoadTile のどちらを使用するか
	bg->s.imageFmt		= G_IM_FMT_CI;		// BG image のフォーマット	   G_IM_FMT_*
	bg->s.imageSiz		= size;				// BG image のサイズ		   G_IM_SIZ_*
	bg->s.imagePal		= 0;				// パレット番号
	bg->s.imageFlip		= 0;				// イメージの左右反転 G_BG_FLAG_FLIPS で反転
	bg->s.scaleW		= scx;				// X 方向スケール値			   (u5.10)
	bg->s.scaleH		= scy;				// Y 方向スケール値			   (u5.10)
	bg->s.imageYorig	= 0 << 5;			// image における描画始点	   (s20.5)
}

// 拡縮時座標補正
static void correct_pos( uObjBg *bg, s16 pos, u16 size, u8 vh)
{
	s16 correct;
	f32 f_scale;

	if( !vh ){	// 横
		f_scale = 1024.0 / (f32)bg->s.scaleW;
		correct = (s16)(size - size * f_scale) / 2;
		bg->s.frameX = ( pos + correct ) << 2;
		bg->s.frameW = (u16)( size * f_scale ) << 2;
	} else {	// 縦
		f_scale = 1024.0 / (f32)bg->s.scaleH;
		correct = (s16)(size - size * f_scale) / 2;
		bg->s.frameY = ( pos + correct ) << 2;
		bg->s.frameH = (u16)( size * f_scale ) << 2;
	}
}

// 画面揺れ
void make_pos( void )
{
	s16 data[] = {6,5,-3,-3,-2,-2,-1,0,3,2,-2,-1,-1,-1,0};
	static u8 cnt = 0;

	if( cannon_on ){
		bg_y += data[cnt++];
		if( cnt >= 15 ){
			cnt = 0;
			cannon_on = 0;
		}
	}
}

// タイル表示
void disp_tile( void )
{
	int i,j;

	gDPLoadTLUT_pal256( gp++,st_p->tile.pal );
	gDPLoadTextureTile_4b( gp++, st_p->tile.pat, G_IM_FMT_CI,80,40,0,0,80-1,40-1,
					0, G_TX_WRAP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

	st_scr_stop = 1;	//	文字列の進行停止
	switch( (st_flag & 0x7) ){
		case 0:			// 下にさがる
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			comple_y = 0;
			tile_adj_y = ( tile_adj_y + 5 >160 )?160:tile_adj_y + 5;
			if( tile_adj_y == 160 ){	//	定位置まで下がったら
				st_scr_stop = 0;		//	文字列の進行開始
			}
			break;
		case 1:			// 上にあがる
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			if( tile_adj_y == 0 ){		//	一番上まであがったら
				if( st_page != 255 ){	//	終了状態でない場合
					pic_num = st_page;	//	背景番号を次のものと入れ替える
					st_flag = 0;		//	フラグのクリア
				} else {				//	終了状態場合
					st_flag = 4;		//	そのままアウト
				}
			}
			comple_y = 0;
			tile_adj_y = ( tile_adj_y - 5 < 0 )?0:tile_adj_y - 5;
			break;
		case 3:
		case 4:
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			break;
	}

};

// メイン
void main50(NNSched* sched)
{
	OSMesgQueue msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	PIC2 *picp;
	int i,j;
	s16	w_size,h_size,sin_f;
	u8	loop_flg = TRUE;

	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	// create message queue for VI retrace
	nnScAddClient(sched, &client, &msgQ);			// add client to shceduler


FIRST_POS:

	st_mode_flow = 0;
	tile_adj_y = 0;
	pic_num = 0;
	comple_y = 0;
	p1_x = 200;
	p1_y = 50;
	bg_x = 0;
	bg_y = 0;
	cl_x = 29;
	cl_y = 167;
	sin_f = 0;
	pino_mouth_flg = 0;
	pino_mouth_count = 0;

	init_font();								//	文字列表示に関する変数の初期化
	set_intencity_rgb( 0,0,0,255,255,255 );		//	文字列表示に関する変数の初期化

	loop_flg =TRUE;
	face_number = 0;

	now_story = evs_story_no;	//	現在のストーリー番号を内部変数にセットする
	st_p = &st_dat[now_story];	//	ストーリーの文字列を内部変数にセットする

	// データ読み込み
	auRomDataRead( (u32)_story_dataSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)(_story_dataSegmentRomEnd - _story_dataSegmentRomStart));	// 小物ロード
	auRomDataRead( (u32)st_p->st_seg,gfx_freebuf[GFX_SPRITE_PAGE + 2], (u32)(st_p->ed_seg - st_p->st_seg ));		// 背景ロード

	//	現在のステージまでの背景の枚数を計算している。
	for( i = 0,st_bg_all_num = 0; i < now_story; i ++ ){
		st_bg_all_num += st_dat[i].bg_num;
	}
	// 絵パラメータセット
	for( i = 0; i < st_p->bg_num; i ++ ){
		j = st_bg_all_num + i;
		picp = &st_bg_data[st_bg_all_num + i];
		// 背景セット
		set_bg_param( &st_bg[i],picp->pat,G_BGLT_LOADTILE,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1<<10,1<<10 );

		// キャラセット
		if( st_scene_data[j][1] != 255 ){
			picp = &st_ch_data[st_scene_data[j][1]];
			set_bg_param( &st_ch[i*2],picp->pat,G_BGLT_LOADBLOCK,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1024,1024 );
		}
		if( st_scene_data[j][2] != 255 ){
			picp = &st_ch_data[st_scene_data[j][2]];
			set_bg_param( &st_ch[i*2+1],picp->pat,G_BGLT_LOADBLOCK,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1024,1024 );
		}
	}
	// 雲セット
	set_bg_param( &st_cld,st_mes_cloud_bm0_0,G_BGLT_LOADTILE,G_IM_SIZ_4b,272,64,cl_x,cl_y,1024-100,1024-100 );

	i = ( cl_y+comple_y >= 240 )?239:cl_y+comple_y;		//	雲を表示するＹ座標の計算
	h_size = (s16)( sinf( DEGREE( sin_f ) ) * 5 );		//	横幅増減値の計算
	w_size = (s16)( sinf( DEGREE( sin_f ) ) * 15 );		//	縦幅増減値の計算
	st_cld.s.scaleW = (272 << 10) / (300 + w_size);		//	横幅を S2DEX の変数に設定
	st_cld.s.scaleH = (64 << 10) / (70 + h_size);		//	縦幅を S2DEX の変数に設定
	correct_pos( &st_cld, cl_x, 272, 0 );				//	Ｘ座標の調整
	correct_pos( &st_cld, i, 64, 1 );					//	Ｙ座標の調整

	// メッセージセット
	font_make( st_meses[now_story] );					//	文字列データのみをグラフィックに置き換える処理

	// 初期化いろいろ
	for( i=0;i<MAXCONTROLLERS;i++ ) joyflg[i] =( DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT );
	joycur1 = 24;
	joycur2 = 6;
	joystp = FALSE;			// start button pause ... off.
	#ifdef STICK_3D_ON
	joystk = TRUE;			// joy stick key.
	#endif

	evs_prgstat = TSTAT_OPEN;
	evs_maincnt = 0;
	evs_grphcnt = 0;
	evs_fadecol = 0;
	auSeqPlayerStop(0);
	//	念のため
	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.
	graphic_no = GFX_50;

	// メインループ
	while( loop_flg ) {

		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.


		evs_maincnt ++;
		evs_grphcnt ++;
		sin_f += 6;
		if( sin_f >= 360 ){
			sin_f -= 360;
		}
		switch( evs_prgstat ) {
			case TSTAT_OPEN:
				if( evs_maincnt == 60 -50 ) {
					evs_prgstat = TSTAT_SET;
					evs_maincnt = 0;
				}
				break;
			case TSTAT_SET:
				auSeqPlayerPlay(0,SEQ_Clear);					//	ＢＧＭ再生開始
				evs_prgstat = TSTAT_LOOP;						//	処理を文章を進める処理に切り替える
				break;
			case TSTAT_LOOP:
				if( ((st_flag & 0x4 ) == 4) || joyupd[main_joy[0]] & DM_KEY_START ){	//	終了ビットが立っているか、スタートボタンを押した場合
					evs_prgstat = TSTAT_CLOSE;					//	内部処理をフェードアウト処理に設定
					evs_maincnt = 0;							//	メインカウンタのクリア
					auSeqPlayerStop(0);							//	音楽停止
					auSndPlay( SE_gCombo1P );					//	SE設定
				}
#ifdef	DM_DEBUG_FLG
				if( joyupd[main_joy[0]] & DM_KEY_L ){
					if( !st_scr_stop ){
						calc_skip_message();
					}
				}
#endif
				make_pos();											//	画面揺れの計算を行っている( ボム兵ステージで大砲発射のところ )
				if( calc_message() > 0 ){							//	文章を進める処理( calc_message() ) 文章の途中の場合 １ 終わりなら -1が返ってくる
					pino_mouth_count ++;							//	キノピオのクチパクをさせるための処理
					if( pino_mouth_count >= 5 ){
						pino_mouth_count = 0;
						pino_mouth_flg ^= 1;
					}
				}else{												//	文章停止(キー待ち等)のためクチパクさせない
					pino_mouth_flg = 0;
				}

				// 背景位置
				st_bg[pic_num].s.frameY = bg_y << 2;

				// 雲情報更新
				i = ( cl_y+comple_y >= 240 )?239:cl_y+comple_y;		//	雲を表示するＹ座標の計算
				h_size = (s16)( sinf( DEGREE( sin_f ) ) * 5 );		//	横幅増減値の計算
				w_size = (s16)( sinf( DEGREE( sin_f ) ) * 15 );		//	縦幅増減値の計算
				st_cld.s.scaleW = (272 << 10) / (300 + w_size);		//	横幅を S2DEX の変数に設定
				st_cld.s.scaleH = (64 << 10) / (70 + h_size);		//	縦幅を S2DEX の変数に設定
				correct_pos( &st_cld, cl_x, 272, 0 );				//	Ｘ座標の調整
				correct_pos( &st_cld, i, 64, 1 );					//	Ｙ座標の調整
				if( now_story == 4 ){								//	５面の場合、プロペラヘイホーを上下させるため、座表計算を行っている
					p1_y = (s16)(sinf( ((evs_maincnt*4) % 360) * 3.14159265 /180.0 ) * 10.0) + st_ch_data[10].pos_y;
				}

				if( st_flag & 0x8 ){								//	このビットが立っていたら
					cannon_on = 1;									//	画面揺れフラグを立てる( ボム兵ステージで大砲発射のところ )
					st_flag &= 0xf7;								//	ビットをクリアする
				}
				break;
			case TSTAT_CLOSE:	//	フェードアウト処理
				if( evs_fadecol < 100 ) {							//	evs_fadecol が 99以下の場合
					evs_fadecol+=10;								//	増加させる
					if( evs_fadecol > 100 ) evs_fadecol = 100;
				} else {
					evs_prgstat = TSTAT_LOOPEND;					//	処理終了へ
				}
				break;
			case TSTAT_LOOPEND:
				graphic_no = GFX_NULL;								// graphics finish.
				loop_flg = FALSE;									//	ループ脱出処理
				break;
		}
		#ifndef THREAD_JOY
		joyProcCore();
		#endif
	}
	auSeqPlayerStop(0);													// stop the audio.
	graphic_no = GFX_NULL;												// graphics finish.
	while((auSeqPlayerState(0) != AL_STOPPED) || (pendingGFX != 0)) {	// task finish wait.
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	for(i = 0;i < 10;i++ ){
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	nnScRemoveClient(sched, &client);									// remove client to shceduler

	evs_story_no = (evs_story_no+1)%12;							//	ストーリー番号を進める処理

	evs_seqnumb = evs_story_no % 3;								//	ゲームの曲を設定する

	return;
}

// グラフィック
void graphic50(void)
{
	NNScTask *t;
	int i;

	// graphic display list buffer. task buffer.
	gp = &gfx_glist[gfx_gtask_no][0];
	t  = &gfx_task[gfx_gtask_no];

	// RSP initialize.
	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, ST_SEGMENT,	 osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE]));
	gSPSegment(gp++, BG_SEGMENT,	 osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE + 2]));

	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	i = 255 * (100 - evs_fadecol) / 100;
	gDPSetPrimColor( gp++, 0,0, i,i,i,255 );

	// BGの描画
	gDPSetTextureLUT(gp++, G_TT_RGBA16);
	gDPSetCycleType(gp++, G_CYC_1CYCLE);
	gDPSetTextureFilter(gp++, G_TF_POINT );
	gDPLoadTLUT_pal256(gp++, st_bg_data[st_bg_all_num+pic_num].pal );
	gSPBgRect1Cyc(gp++, &st_bg[pic_num]);

	// キャラ表示
	if( (st_flag & 0x7) < 3 ){
		i = st_scene_data[st_bg_all_num+pic_num][2];
		if( i != 255 ){
			gDPLoadTLUT_pal256(gp++, st_ch_data[i].pal );
			gSPBgRect1Cyc(gp++, &st_ch[pic_num*2+1]);
		}
		i = st_scene_data[st_bg_all_num+pic_num][1];
		if( i != 255 ){
			gDPLoadTLUT_pal256(gp++, st_ch_data[i].pal );
			gSPBgRect1Cyc(gp++, &st_ch[pic_num*2]);
		}
	}

	// プロペラボム兵表示
	if( now_story == 4 && pic_num == 1 ){
		if( (st_flag & 0x7) < 3 ){
			gDPSetTextureFilter(gp++, G_TF_BILERP );
			disp_tex_8bt( st_ch_data[10].pat,st_ch_data[10].pal,st_ch_data[10].width,st_ch_data[10].height,st_ch_data[10].pos_x,p1_y,SET_PTD );
		}
	}

	// タイル表示
	disp_tile();

	// 雲表示
	gDPSetTextureLUT(gp++, G_TT_RGBA16);
	gDPSetRenderMode(gp++, G_RM_SPRITE, G_RM_SPRITE2);
	gDPSetCycleType(gp++, G_CYC_1CYCLE);
	gDPSetTextureFilter(gp++, G_TF_POINT );
	gDPLoadTLUT_pal256(gp++, st_mes_cloud_bm0_0tlut );	//	パレットロード
	gSPBgRect1Cyc(gp++, &st_cld);						//	雲描画


	// 顔表示
	if( disp_face_flg ){
		gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
		if( face_number != 0 ){	//	キノピオ以外の場合
			disp_tex_4bt( faces[face_number].pat,faces[face_number].pal,48,48,32,176+comple_y,SET_PTD );
		}else{					//	キノピオの場合
			disp_tex_4bt( faces[face_number].pat,faces[face_number].pal,48,48,32,176+comple_y,SET_PTD );
			//	クチパク
			if( pino_mouth_flg ){
				load_TexTile_4b( st_face_13b_bm0_0,8,5,0,0,7,4 );
				draw_Tex( 59,210 + comple_y,8,5,0,0 );
			}
		}
	}

	// メッセージ表示
	disp_message();

	// end of display list & go the graphic task.
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();
	gfxTaskStart(t, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);
}

