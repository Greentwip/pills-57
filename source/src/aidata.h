/***********************************************************
	ai data header file		Tadashi Ogura

	Modified by Yasunori Kobayashi.
***********************************************************/

// virus ｷｰ操作ﾓｰﾄﾞ
enum {
	AI_SPEED_SUPERSLOW,			// とても遅い（現在未使用）
	AI_SPEED_VERYSLOW,			// とても遅い
	AI_SPEED_SLOW,				// 遅い
	AI_SPEED_FAST,				// 速い
	AI_SPEED_VERYFAST,			// とても速い
	AI_SPEED_MAX,				// 最高速
	AI_SPEED_FLASH,				// ﾉｰｳｴｲﾄ移動
	AI_SPEED_VERYFASTNW,			// とても速い（落下時躊躇無し）
	_AI_SPEED_SUM,
} AI_SPEED;

// 思考ロジックの種類
typedef enum {
	AI_LOGIC_FastErase,
	AI_LOGIC_SmallRensa,
	AI_LOGIC_RensaAndErase,
	AI_LOGIC_RensaAndAttack,
	AI_LOGIC_WidErase,
	AI_LOGIC_MajorityRensa,
	_AI_LOGIC_SUM,
} AI_LOGIC;

// キャラのプレイ状況
enum {
	AI_STATE_DEAD,			// 死んだ後練習モード用
	AI_STATE_NARROW,		// 移動可能範囲が狭くなってきた時
	AI_STATE_LASTVS,		// 終盤でｳｨﾙｽがある時
	AI_STATE_LASTVSNH,		// 終盤でｳｲﾙｽがない時で高く積んだｶﾌﾟｾﾙを消す時
	AI_STATE_LASTVSN,		// 終盤でｳｲﾙｽがない時
	AI_STATE_BADLINE2,		// 消せる高積みﾗｲﾝが中央２ﾗｲﾝにある時
	AI_STATE_BADLINE1,		// 消せない高積みﾗｲﾝが中央６ﾗｲﾝにある時
	AI_STATE_NORMAL,		//
	_AI_STATE_SUM,
} AI_STATE;

// 思考ロジックデータ
typedef struct {
	s16		dt_LnOnLinVrs;
	s16		dt_LnEraseVrs;
	s16		dt_EraseLinP1;
	s16		dt_EraseLinP3;
	s16		dt_HeiEraseLinRate;
	s16		dt_WidEraseLinRate;

	s16		dt_P_ALN_HeightP;
	s16		dt_P_ALN_FallCap;
	s16		dt_P_ALN_FallVrs;
	s16		dt_P_ALN_Capsule;
	s16		dt_P_ALN_Virus;

	s16		dt_WP_ALN_Bottom;
	s16		dt_WP_ALN_FallCap;
	s16		dt_WP_ALN_FallVrs;
	s16		dt_WP_ALN_Capsule;
	s16		dt_WP_ALN_Virus;

	s16		dt_OnVirusP;
	s16		dt_RensaP;
	s16		dt_RensaMP;
	s16		dt_LnHighCaps;
	s16		dt_LnHighVrs;
	s16		dt_aiWall_F;

	s16		dt_HeiLinesAllp2;
	s16		dt_HeiLinesAllp3;
	s16		dt_HeiLinesAllp4;

	s16		dt_WidLinesAllp2;
	s16		dt_WidLinesAllp3;
	s16		dt_WidLinesAllp4;
} AI_DATA;

// キャラの行動パターンデータ
typedef struct {

	// 相手におじゃまカプセルのストックがあるとき、
	// 攻撃を待つ処理を実行するかしないか設定するフラグ。 （0:OFF,1:ON）
	s8 wait_attack;

	// 操作速度(AI_SPEED_????)
	s8 speed;

	// 自分へのダメージカプセルが都合のよい所に落ちる率
	s16 luck;

	// 状況(AI_STATE_????)毎で使用する思考ロジック(AI_LOGIC_????)
	s8 logic[_AI_STATE_SUM];

	// 特殊処理発生条件(AI_CONDITION_????)
	s8 condition[NUM_AI_EFFECT];
	s16 condition_param[NUM_AI_EFFECT];

	// 実行する特殊処理(AI_EFFECT_????)
	s8 effect[NUM_AI_EFFECT];
	s16 effect_param[NUM_AI_EFFECT];

} AI_CHAR;

// キャラの特殊処理発生条件  Commented by Mr.Maeta.
typedef enum {

// 発生条件が無い状態
	AI_CONDITION_Ignore,

// 無条件で効果が発生する。
	AI_CONDITION_Unconditional,

// ランダムで効果が発生する。
// Subパラメータが存在し、発生率を設定する。
	AI_CONDITION_Random,

// 対戦相手から攻撃を受けると効果が発生する。
	AI_CONDITION_Damage,

// 対戦相手よりも自分のウィルスが多いときに効果が発生する。
// Subパラメータが存在し、発生するウィルス差を設定する。
	AI_CONDITION_ManyVir,

// 対戦相手よりも自分のウィルスが少ないときに効果が発生する。
// Subパラメータが存在し、発生するウィルス差を設定する。
	AI_CONDITION_FewVir,

// 対戦相手のカプセルが積み上がると効果が発生する。
	AI_CONDITION_Pile,

// ＣＯＭレベルがＥＡＳＹのときに効果が発生する。
	AI_CONDITION_Easy,

// ＣＯＭレベルがＮＯＲＭＡＬのとき効果が発生する。
	AI_CONDITION_Normal,

// ＣＯＭレベルがＨＡＲＤのときに効果が発生する。
	AI_CONDITION_Hard,

// 自分の残りウィルスが少なくなると効果が発生する。
// Subパラメータが存在し、ウィルスが何匹以下になると発生するか設定する。
	AI_CONDITION_Last,

// 連鎖をさせると効果が発生する。
// Subパラメータが存在し、発生する連鎖数を設定する。
	AI_CONDITION_Rensa,

// ゲーム開始時のウィルスが指定した数以上の時に発生する
// Subパラメータが存在し、ウィルス数を設定する。
	AI_CONDITION_PreemPtive,

	_AI_CONDITION_SUM,

// ※ Subパラメータとは、AI_CHAR 構造体の condition_param のこと。

} AI_CONDITION;

// キャラの特殊処理  Commented by Mr.Maeta.
typedef enum {

// エフェクトが無効な状態。
	AI_EFFECT_Ignore,

// カプセルを無意味に回転させる。
	AI_EFFECT_Rotate,

// カプセルを無意味に横移動させて、迷っているように見せる。
	AI_EFFECT_Waver,

// カプセルを操作できなくなる。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
	AI_EFFECT_NotMove,

// カプセルの操作速度が速くなる。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
	AI_EFFECT_BeFast,

// カプセルの操作速度が遅くなる。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
	AI_EFFECT_BeSlow,

// カプセルの操作速度がノーウエイトになる。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
	AI_EFFECT_BeNoWait,

// カプセルの操作速度がどんどん速くなる。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
	AI_EFFECT_Rapid,

// 消去ロジックが“攻撃”方向へ１段階進む。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
// 消去ロジックは　「早消し」　FastErase-SmallRensa-Rensa&Erase-Rensa&Attack　「攻撃」　と、並ぶ。
	AI_EFFECT_BeAttack,

// 消去ロジックが“早消し”方向へ１段階進む。
// Subパラメータが存在し、効果が持続する時間を設定する。（0は∞。）
// 消去ロジックは　「早消し」　FastErase-SmallRensa-Rensa&Erase-Rensa&Attack　「攻撃」　と、並ぶ。
	AI_EFFECT_BeErase,

// 発生している特殊処理を全て無効にする。
// Subパラメータが存在し、無効にする特殊処理を指定する。（0は全て。）
	AI_EFFECT_Lose,

	_AI_EFFECT_SUM,

// ※ Subパラメータとは、AI_CHAR 構造体の effect_param のこと。

} AI_EFFECT;

///////////////////////////////////////////////////////////

// 思考パラメータデータワーク
//（パラメータデータをデバッグモードで可変にするため元データから転送して使う）
extern AI_DATA	ai_param[_AI_LOGIC_SUM][_AI_STATE_SUM];

// 思考パラメータデータ
extern AI_DATA	ai_param_org[_AI_LOGIC_SUM][_AI_STATE_SUM];

// キャラの思考パターン
// （デバッグモードで可変にするため元データから転送して使う）
extern AI_CHAR ai_char_data[16];

// キャラの思考パターン [ キャラの番号 (CHR_????) ]
extern AI_CHAR ai_char_data_org[16];
