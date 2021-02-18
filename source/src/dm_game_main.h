//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_game_main.h
//	create		:	1999/08/02
//	modify		:	1999/11/11
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#ifndef __dm_game_main_h__
#define __dm_game_main_h__

#include "dm_game_def.h"

//////////////////////////////////////////////////////////////////////////////
// 関数の参照

// カプセルのテクスチャ＆パレットを取得
extern STexInfo *dm_game_get_capsel_tex(int sizeIndex);
extern STexInfo *dm_game_get_capsel_pal(int sizeIndex, int colorIndex);

// 得点を計算
extern u32 dm_make_score(game_state *state);

extern void dm_attack_se(game_state *state, int player_no); // 攻撃音発生関数
extern void dm_warning_h_line(game_state *, game_map *);    // 積み上げ警告チェック
extern void dm_warning_h_line_se();                         // 積み上げ警告音発生関数

extern void go_down(game_state *, game_map *, int); // 粒落下関数
extern void erase_anime(game_map *);                // マップ上消滅アニメーション関数

extern void translate_capsel(game_map *, game_state *, int, int); // カプセル左右移動関数
extern void rotate_capsel(game_map *, game_cap *, int);           // カプセル左右回転関数

extern void dm_set_capsel(game_state *state);               // カプセル設定関数
extern void dm_capsel_speed_up(game_state *);               // カプセル速度上昇設定関数
extern void dm_capsel_down(game_state *, game_map *);       // 操作カプセル落下処理関数
extern int  dm_check_game_over(game_state *,game_map *);    // ゲームオーバー(積みあがり)判定関数
extern void dm_virus_anime(game_state *, game_map *);       // ウイルスアニメ関数
extern void dm_capsel_erase_anime(game_state *, game_map *);// ウイルス(カプセル)消滅関数
extern int  dm_h_erase_chack(game_map *);                   // 縦消し判定関数
extern void dm_h_erase_chack_set(game_state *, game_map *); // 縦消し判定と処理開始関数
extern int  dm_w_erase_chack(game_map *);                   // 横消し判定関数
extern void dm_w_erase_chack_set(game_state *, game_map *); // 横消し判定と処理開始関数
extern void dm_h_ball_chack(game_map *);                    // 縦粒判定処理関数
extern void dm_w_ball_chack(game_map *);                    // 横粒判定処理関数
extern int  dm_black_up(game_state *,game_map *);           // ゲームオーバーの時の暗くなる関数

// 瓶を描画
extern void dm_calc_bottle_2p();
extern void dm_draw_bottle_2p(Gfx **gpp);

// カサマルを描画
extern void dm_draw_KaSaMaRu(Gfx **gpp, Mtx **mpp, Vtx **vpp,
	int speaking, int x, int y, int dir, int alpha);

// 巨大ウィルスの位置を計算
extern void dm_calc_big_virus_pos(game_state *state);

// 巨大ウィルスを描画
extern void dm_draw_big_virus(Gfx **gpp);

extern void dm_game_graphic_effect(game_state *, int, int); // ゲーム描画関数(演出関係用)

extern void dm_game_graphic_p(game_state *, int, game_map *);    // マップ情報描画関数
extern void key_control_main(game_state *, game_map *, int, int);// カプセル操作関数
extern void make_ai_main();                                      // 思考部分

// 共通計算
void dm_effect_make(void);

// ヒープメモリを取得
extern void **dm_game_heap_top();

// ウィルスキャラアニメーション
extern SAnimeState *get_virus_anime_state(int index);

// ウィルス消滅時の煙アニメーション
extern SAnimeSmog *get_virus_smog_state(int index);

// ジオメトリバッファを取得
extern Mtx *dm_get_mtx_buf();
extern Vtx *dm_get_vtx_buf();

extern void dm_game_init_heap();
extern void dm_game_init_static();
extern void dm_game_init_snap_bg();
extern void dm_game_draw_snap_bg(Gfx **gpp, Mtx **mpp, Vtx **vpp, int drawPanelFlag);
extern void dm_game_draw_snapped_bg(Gfx **gpp);

extern void dm_game_init(int reinit);
extern int  dm_game_main(NNSched *);
extern void dm_game_graphic();
extern void dm_game_graphic_onDoneSawp();
extern void dm_game_graphic_onDoneTask();

extern int  main_techmes(void *arg);
extern void graphic_techmes();

//////////////////////////////////////////////////////////////////////////////
//	変数の参照

// ウイルスのアニメーションパターン番号
extern u8 virus_anime_table[][4];

// 瓶の中の情報
extern game_map game_map_data[4][GAME_MAP_W * (GAME_MAP_H + 1)];

// ウイルス設定用配列
extern virus_map virus_map_data[4][GAME_MAP_W * GAME_MAP_H];

// 各プレイヤーの状態及び制御
extern game_state game_state_data[4];

// ウイルス設定順用配列
extern u8 virus_map_disp_order[4][96];

//////////////////////////////////////////////////////////////////////////////

#endif
