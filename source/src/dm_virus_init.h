//////////////////////////////////////////////////////////////////////////////
//	filename	:	dm_virus_init.h
//	create		:	1999/08/03
//	modify		:	1999/11/26
//	created		:	Hiroyuki Watanabe
//////////////////////////////////////////////////////////////////////////////

#ifndef	__dm_virus_init_h__
#define	__dm_virus_init_h__

//////////////////////////////////////////////////////////////////////////////
//	宣言

#define		dm_up_weight		1		// 上1/3の重み
#define		dm_middle_weight 	2		// 中  ..
#define		dm_low_weight		3		// 下  ..

//////////////////////////////////////////////////////////////////////////////
//	関数の参照

//## マップ情報を初期化
extern void init_map_all(game_map *map);

//	マップ情報全部クリア関数
//	引数	map	:クリアするマップへのポインタ
extern	void	clear_map_all( game_map *map );

//	マップ情報一部クリア関数
//	引数	map	:クリアするマップへのポインタ
//			x	:Ｘ座標(0～7)
//			y	:Ｙ座標(1～16)
extern	void	clear_map( game_map *map, int x, int y );

//	マップ情報取得関数
//	引数	map	:クリアするマップへのポインタ
//			x	:Ｘ座標(0～7)
//			y	:Ｙ座標(1～16)
//	返値	0:何も無し
//			1:何か有り
extern	int		get_map_info( game_map *map, int x, int y );

// 指定したＸ座標軸上で、一番上にある
// ウィルス or カプセルの色を取得
extern int get_map_top_color(
	game_map *map, // マップデータ配列の先頭アドレス
	int       x    // 色を取得するＸ座標
	);
// 戻り値 : 色番号 (-1 は無色)

// マップ上の一番上の段にある色を取得
extern void get_map_top_colors(
	game_map *map,         // マップデータ配列の先頭アドレス
	int colors[GAME_MAP_W] // 色データ出力先
	);

//	ウイルス個数取得関数
//	引数	map:情報を取得するマップへのポインタ
//	返値	ウイルスの総数
extern	int		get_virus_count( game_map *map );

//	色別ウイルス個数取得関数
//	引数	map	:情報を取得するマップへのポインタ
//			r	:赤色ウイルスの数を格納する変数のポインタ
//			y	:黄色ウイルスの数を格納する変数のポインタ
//			b	:青色ウイルスの数を格納する変数のポインタ
//	返値 ウイルスの総数
extern	int		get_virus_color_count(game_map *map, u8 *r, u8 *y, u8 *b);

//	マップ情報設定関数
//	引数	map	:情報を設定するマップへのポインタ
//			x	:Ｘ座標(0～7)
//			y	:Ｙ座標(1～16)
//			item:設定するもの( dm_game_main.h で列挙されているもの )
//			col :色( dm_game_main.h で列挙されているもの )
extern	void	set_map( game_map *map, int x, int y, int item, int col);

//	ウイルスマップ情報設定関数
//	引数	map	:ウイルス情報を設定するマップへのポインタ
//			x	:Ｘ座標(0～7)
//			y	:Ｙ座標(1～16)
//			vi	:ウイルス( dm_game_main.h で列挙されているもの )
//			an	:アニメ番号
extern	void	set_virus( game_map *map, int x, int y, int vi, int an );

//	マップ情報複製関数
//	引数	m_a	:コピー元マップへのポインタ
//			m_b	:コピー先マップへのポインタ
//			o_a	:コピー元表示順
//			o_b	:コピー先表示順
extern	void	dm_virus_map_copy( virus_map *m_a,virus_map *m_b,u8 *o_a,u8 *o_b);

// ゲーム開始時のウィルス数を取得
extern int dm_get_first_virus_count(
	GAME_MODE mode, // ゲームモード
	game_state *state // ゲームステータス
	);
// 戻り値: ゲーム開始時のウィルス数

// ゲーム開始時のウィルス数を取得(フラッシュ、スコアアタック)
extern int dm_get_first_virus_count_in_new_mode(
	int level // ゲームレベル
	);
// 戻り値: ゲーム開始時のウィルス数

//	ウイルス情報作成関数
//	引数	st	:プレイヤー情報のポインタ
//			map	:作成したウイルス情報を格納する場所のポインタ
//			od	:表示順格納先ポインタ
extern void _dm_virus_init(GAME_MODE mode, game_state *state, virus_map *v_map, u8 *order, int special);
extern void dm_virus_init(GAME_MODE mode, game_state *st, virus_map *map, u8 *order);


// フラッシュウィルスの位置を決める
extern int make_flash_virus_pos(game_state *state, virus_map *vmap, u8 *order);

//////////////////////////////////////////////////////////////////////////////

#endif
