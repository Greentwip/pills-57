
#define VOL_CHAR_DEF	0x6c
#define VOL_GAME1_DEF	0x74
#define VOL_GAME2_DEF	0x6a
#define VOL_GAME3_DEF	0x60
#define VOL_MENU_DEF	0x78

typedef struct {
	u8 no;     // 音番号
	s8 coarse; // 音程?(T's から頂いた値)
	s8 fine;   //     ?(T's から頂いた値)
	u8 pri;    // プライオリティ
	u8 time;   // 再生時間(1/10秒単位, 0で無限)
	u8 vol;    // 音量
} FxTune;

static const FxTune _fxTuneTbl[] = {
	// No         Coarse Fine  Pri Time
	{ FX_01_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // イカタコテング
	{ FX_01_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_02_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // キグモン
	{ FX_02_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_03_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // ハンマーロボ
	{ FX_03_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_04_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // フウセンまじん
	{ FX_04_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_05_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // ふっくらげ
	{ FX_05_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_06_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // つまりカエル
	{ FX_06_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_07_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // マッド＝シタイン
	{ FX_07_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_08_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // マユピー
	{ FX_08_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_09_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // ヤリマル
	{ FX_09_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_10_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // りんごろう
	{ FX_10_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_11_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // なぞのぞう
	{ FX_11_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_SE01,       -12,   0,  10,   0, VOL_GAME1_DEF }, // カプセル落下音の番号
	{ FX_SE02,         0,   0,  10,   0, VOL_GAME1_DEF }, // ウイルス消去音の番号
	{ FX_SE03,        -6, -45,  10,   0, VOL_GAME1_DEF }, // 巨大ウイルス消滅御
	{ FX_SE04,       -12,   0,  10,   0, VOL_MENU_DEF }, // 落下速度アップ音の番号
	{ FX_SE05,        -6, -45,  10,   0, VOL_MENU_DEF }, // １Ｐ連鎖時の音
	{ FX_SE06,         0,   0,  10,   0, VOL_MENU_DEF }, // ポーズ効果音
	{ FX_SE07,       -12,   0,  10,   0, VOL_GAME1_DEF }, // カプセル消去音の番号
	{ FX_SE08,         0,   0,  10,   0, VOL_MENU_DEF }, // 決定
	{ FX_SE09,        -6, -45,  10,   0, VOL_MENU_DEF }, // メニュー時のカーソル移動
	{ FX_SE10,        -6, -45,  10,   0, VOL_MENU_DEF }, // メニュー切り替え
	{ FX_SE11,       -12,   0,  10,   0, VOL_GAME2_DEF }, // 左右移動音の番号
	{ FX_SE12,       -12,   0,  10,   0, VOL_GAME1_DEF }, // 着地音の番号
	{ FX_SE13,        -6, -45,  10,   0, VOL_GAME3_DEF }, // 回転音の番号
	{ FX_SE14,         0,   0,  10,   0, VOL_MENU_DEF }, // キャンセル
	{ FX_SE15,         5,  55,  10,   0, VOL_MENU_DEF }, // 画面きり変え
	{ FX_SE16,       -12,   0,  10,   0, VOL_MENU_DEF }, // バックアップデータ削除
	{ FX_SE17,       -12,   0,  10,   0, VOL_MENU_DEF }, // エラー
	{ FX_SE18,        -6, -45,  10,   0, VOL_MENU_DEF }, // カウントダウン,
	{ FX_SE19,        -6, -45,  10,   0, VOL_MENU_DEF }, // カウントダウン終了
	{ FX_SE20,         0,   0,  10,  20, VOL_GAME1_DEF }, // 巨大ウイルスダメージ
	{ FX_SE21,        -6, -45,  40,   0, VOL_MENU_DEF }, // ２Ｐ連鎖時の音
	{ FX_SE22,        -6, -45,  40,   0, VOL_MENU_DEF }, // ３Ｐ連鎖時の音
	{ FX_SE23,        -6, -45,  40,   0, VOL_MENU_DEF }, // ４Ｐ連鎖時の音
	{ FX_SE24,        -6, -45,  40,   0, VOL_GAME1_DEF }, // コイン獲得
	{ FX_SE25,        -6, -45,  40,   0, VOL_MENU_DEF }, // カプセル積み上げ警告音
	{ FX_SE26,        -6, -45,  40,   0, VOL_MENU_DEF }, // ウイルスが残り３個のときの音

	{ FX_12_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // ワリオ
	{ FX_12_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_13_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // バンパイアワリオ
	{ FX_13_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_14_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // マリオ
	{ FX_14_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_14_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_14_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_14_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_15_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // メタルマリオ
	{ FX_15_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_15_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_15_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_15_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_SE27,         0,   0,  10,   0, VOL_MENU_DEF }, // 
	{ FX_SE28,         0,   0,  10,   0, VOL_MENU_DEF }, // 
	{ FX_SE29,         0,   0,  10,   0, VOL_MENU_DEF }, // 
};
