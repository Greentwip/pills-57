
//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
// 外部から書き込まれるﾜｰｸ

u8	aiUsedFlag;				// 1int中に１回しか思考処理が使われないようにするﾌﾗｸﾞ
							// 1intごとにゲームプログラムがFALSEに初期化する

//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
// ＡＩ専用ワーク

// ﾌﾞﾛｯｸｽﾃｰﾀｽ pw->blk[][].st
enum {
	MB_CAPS_U,		//-ｶﾌﾟｾﾙ上向き
	MB_CAPS_D,		//	..	下 ..
	MB_CAPS_L,		//	..	左 ..
	MB_CAPS_R,		//	..	右 ..
	MB_CAPS_BALL,	//	..	玉
	MB_VIRUS_A,		//-ｳｲﾙｽA
	MB_VIRUS_B,		// ｳｲﾙｽB
	MB_VIRUS_C,		// ｳｲﾙｽC
	MB_ERASE_CAPS,	// ｶﾌﾟｾﾙ消滅中
	MB_ERASE_VIRUS,	//-ｳｲﾙｽ	  ..
	MB_NOTHING,		// 何もない
};

// ｶﾗｰ (属性) pw->blk[][].co, pw->cap(nex).co
enum {
	COL_RED,			// 順序固定
	COL_YELLOW,			//	  ..
	COL_BLUE,			//	  ..
	COL_NOTHING,
};

// ｷｬﾗｸﾀｰ番号
enum {
	CHR_MARIO,
	CHR_NOKO,
	CHR_BOMB,
	CHR_PUKU,
	CHR_CHOR,
	CHR_PROP,
	CHR_HANA,
	CHR_TERE,
	CHR_PACK,
	CHR_KAME,
	CHR_KUPP,
	CHR_PEACH,
	CHR_PLAIN1,
	CHR_PLAIN2,
	CHR_PLAIN3,
	CHR_PLAIN4,
};

#define	ROOTCNT		50										// ﾙｰﾄ検索ﾜｰｸ最大数

// ｶｰｿﾙｶﾌﾟｾﾙを置いたことにより作られる状態
enum {	// af->hei(wid)[][x]: ﾃｰﾌﾞﾙ要素
	LnEraseLin,	// 消去ﾗｲﾝ数
	LnEraseVrs,	// 消去ｳｲﾙｽ数
	LnRinsetsu,	// 隣接数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ )くっついた状態で並んだ数
	LnLinesAll,	// 並び数( ｳｲﾙｽ&ｶﾌﾟｾﾙ&ｶｰｿﾙｶﾌﾟｾﾙ,歯欠の間1ならばｶｳﾝﾄ )くっついた状態で並んだ数(空間が1つだけ空いてもOK)
	LnOnLinVrs,	// 並びの中のｳｲﾙｽ数
	LnLinSpace,	// ﾗｲﾝｽﾍﾟｰｽ( 3以下なら無効? )色を4個並べられるかﾁｪｯｸ用
	LnEraseVrsSide,	// ｳｨﾙｽ周辺消去数(旧Dummy)
	LnHighCaps,	// 入り口付近にあるｶﾌﾟｾﾙの数
	LnHighVrs,	// 入り口付近にあるｳｨﾙｽの数
	LnNonCount,	// TRUE：ﾉｰｶｳﾝﾄ( ex.ﾀﾃが消去の時のﾖｺのﾎﾟｲﾝﾄ計算をしないﾌﾗｸﾞ)
	LnTableMax,	//------ MAX
};

// 移動ｶﾌﾟｾﾙ移動先での状態
typedef struct {
	u8	ok;			// 移動可能ﾌﾗｸﾞ
	u8	tory;		// 縦/横ﾌﾗｸﾞ
	u8	x;			// 目的地X座標
	u8	y;			// 目的地Y座標
	u8	rev;		// ｶﾌﾟｾﾙの上下or左右反転ﾌﾗｸﾞ
	u8	ccnt;		// 同色数
					//------- re-make by hiru
	s32	pri;		// ﾎﾟｲﾝﾄ計算算出値 = 優先度
	s32	dead;		// ﾎﾟｲﾝﾄ計算算出用ﾃﾝﾎﾟﾗﾘｰﾜｰｸ
	u8	hei[2][LnTableMax];	// ｶｰｿﾙｶﾌﾟｾﾙに作られる縦ﾗｲﾝの状態（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ,ｽﾚｰﾌﾞ）
	u8	wid[2][LnTableMax];	// ｶｰｿﾙｶﾌﾟｾﾙに作られる横ﾗｲﾝの状態（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ,ｽﾚｰﾌﾞ）
	u8	elin[2];	// 消去ﾗｲﾝ合計数( ﾒｲﾝ,ｻﾌﾞ )
	u8	only[2];	// 上下左右に同じ色がない時「単一ｶﾌﾟｾﾙの状況」の数値が入る（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ､ｽﾚｰﾌﾞ）
	u8	wonly[2];	// 左右に同じ色がない時「単一ｶﾌﾟｾﾙの状況」の数値が入る（操作ｶﾌﾟｾﾙ２色分 ﾏｽﾀｰ､ｽﾚｰﾌﾞ）
	u8	sub;		// ﾏｽﾀｰｶﾌﾟｾﾙが消去しない時、ｽﾚｰﾌﾞも通常計算するためのﾌﾗｸﾞ
	u8	rensa;		// 連鎖するかﾌﾗｸﾞ（TRUE：連鎖する）
} AI_FLAG;

// 移動ﾙｰﾄ保存用ﾜｰｸ
typedef struct {
	u8	x;
	u8	y;
} AI_ROOT;

typedef struct {
	AI_FLAG	aiFlagDecide;			// 操作ｶﾌﾟｾﾙの状態
	AI_ROOT	aiRootDecide[ROOTCNT];	// 操作ｶﾌﾟｾﾙのﾙｰﾄ
	u8	aiKeyCount;					// 操作ｶﾌﾟｾﾙのﾙｰﾄのｶｳﾝﾀ
	u8	aiSpeedCnt;					// 移動や回転操作を行った後次の操作ができるまでのｳｪｲﾄ
	u8	aiKRFlag;					// CPUｷｰﾘﾋﾟｰﾄ操作ﾌﾗｸﾞ（０：ｷｰﾘﾋﾟｰﾄ中）
	u8	aiRollCnt;					// 操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ
	u8	aiRollFinal;				// 移動終了時に１回だけ回転操作するかどうかﾌﾗｸﾞ（TRUE：１回右に回す）
	u8	aiRollHabit;				// 操作ｶﾌﾟｾﾙを回転させる癖ﾌﾗｸﾞ（0x02bit on:適当に回しながら落す、0x01bit on:半回転中）
	u8	aiSpUpFlag;					// 操作ｶﾌﾟｾﾙの落下操作を行うかどうかのﾌﾗｸﾞ（TRUE:行う）
	u8	aiSpUpStart;				// 操作ｶﾌﾟｾﾙの落下操作を行うﾀｲﾐﾝｸﾞを移動した距離と比較して行うための数値
	u8	aiSpUpCnt;					// ｽﾋﾟｰﾄﾞｱｯﾌﾟするｶｳﾝﾄ（未使用ﾜｰｸ）
	u8	aivl;						// virus level
	u8	aiok;						// ﾙｰﾄ検索思考処理を行いﾙｰﾄが決まったかどうかﾌﾗｸﾞ（TRUE:検索待ち）
	u8	aiRandFlag;					// ﾎﾟｲﾝﾄ判定時乱数をﾌﾟﾗｽするかﾌﾗｸﾞ（FALSE:４Ｐ対戦で３人同じＣＰＵのとき同じ操作をしないよう優先順位ﾎﾟｲﾝﾄに乱数値を足す）
	u8	aiEX;						// 目的地Ｘ座標
	u8	aiEY;						// 目的地Ｙ座標
	u8	aiOldRollCnt;				// 旧操作ｶﾌﾟｾﾙ回転方向ｶｳﾝﾀ(目的地に行けなかったチェック用)
	u8	aiNum;						// キャラ別思考用汎用ﾜｰｸ１
	u8	aiTimer;					// キャラ別思考用汎用ﾜｰｸ２
	u8	aiState;					// COMの状態ﾌﾗｸﾞ
	u8	aiSelSpeed;					// COMの操作ｽﾋﾟｰﾄﾞﾓｰﾄﾞ
	u8	aiRootP;					// 移動ｶﾌﾟｾﾙの移動距離の平均値(積みあがったり左右をｶﾌﾟｾﾙの壁に囲まれた時などに移動範囲が狭いので数値が小さくなる)
	u16	aiPriOfs;					// 下手なキャラクターに優先度のﾎﾟｲﾝﾄにﾗﾝﾀﾞﾑで足す数値（400から800程度CPUと戦ってみて決める）
} AIWORK;

// CPUの状態ﾌﾗｸﾞ
#define	AIF_DAMAGE	0x01	// 攻撃を受けた
#define	AIF_DAMAGE2	0x02	// 連続攻撃を受けた
#define	AIF_AGAPE	0x04	// COMの操作をストップさせる

// 操作ｶﾌﾟｾﾙ情報を受け取るﾜｰｸ
typedef struct {
	u8			st;			// （未使用）状態						:ONES_????
	u8			wc;			// （未使用）非表示ｳｴｲﾄｶｳﾝﾄ( 1>=強制 )
	u8			mx,my;		// 移動経路検索前の操作ｶﾌﾟｾﾙの座標及び操作中の座標（mﾌｨｰﾙﾄﾞ上）x,y(8x17)
	u8			ca;			// 操作ｶﾌﾟｾﾙのｶﾗｰa(ﾏｽﾀｰ)番号			:COL_???
	u8			cb;			// 操作ｶﾌﾟｾﾙのｶﾗｰb(ｽﾚｰﾌﾞ)番号
	u8			sp;			// 現在の落下ｽﾋﾟｰﾄﾞ番号（FallSpeed[番号]）
	u8			cn;			// 現在の落下ｽﾋﾟｰﾄﾞのｶｳﾝﾀ(cn == FallSpeed[番号]の時操作ｶﾌﾟｾﾙが１段落ちる)
	u16			bc;			// （未使用）ﾌﾞﾛｯｸ化ｶｳﾝﾀ
	u8			fc;			// （未使用）ﾌﾗｲﾝｸﾞｶｳﾝﾀ
	u8			bd;			// （未使用）ﾌﾞﾛｯｸ化ｶｳﾝﾀ
} AICAPS;

// mﾌｨｰﾙﾄﾞ上のｶﾌﾟｾﾙ&ｳｨﾙｽの状態ﾜｰｸ
typedef struct {
	u8			st;			// ﾌﾞﾛｯｸの種類				:CAPS/VIRUS..
	u8			co;			// ｶﾗｰ番号					:COL_???
//	u8			ff;			// 落下中ｽﾃｰﾀｽ
//	u8			rf;			// 病原菌作成予約ﾌﾗｸﾞbyｳﾙﾋﾟｰ
//	u8			csf;		// コンボ起点フラグ	ogura
//	u8			dmy0;		// (dummy)			ogura
//	u16			wk[4];		// 操作ﾜｰｸ
//	float		fc;			// 浮動小数点ｶｳﾝﾀ
} AIBLK;



//----------------------------------------------------------
//	ゲームの制御用構造体
//
typedef	struct	{
	u16	game_score;			//	得点
	s16	map_x,map_y;		//	マップの座標
	s8	map_item_size;		//	マップのアイテムの大きさ
	u8	game_mode[2];		//	0:内部処理番号 1:内部処理番号保存変数
	s8	game_condition[3];	//	0:その時の状態 2:その時の状態保存変数 3:変化しない状況変数(トレーニング中だと０以外)
	u8	virus_number;		//	ウイルス数
	u8	virus_level;		//	ウイルスレベル
	u8	virus_anime;		//	ウイルスアニメーション番号
	s8	virus_anime_vec;	//	ウイルスアニメーション進行方向
	u8	virus_anime_count;	//	ウイルスアニメーションカウンタ
	u8	virus_anime_max;	//	ウイルスアニメーションの最大コマ数
	u8	virus_anime_spead;	//	ウイルスアニメーション速度
	u8	cap_def_speed;		//	カプセル速度(SPEED_?(LOW/MID/HIGH/MAX))
	u8	cap_speed;			//	カプセル速度
	u8	cap_count;			//	カプセル落下数
	u8	cap_speed_count;	//	カプセル落下用カウンタ
	u8	cap_speed_vec;		//	カプセル落下用カウンタ増加値
	u8	cap_speed_max;		//	カプセル落下速度(カウンタがこの数値以上になると１段落下)
	u8	cap_magazine_cnt;	//	カプセルマガジン参照用変数
	u8	cap_magazine_save;	//	カプセルマガジン参照用保存変数
	s8	cap_move_se_flg;	//	カプセル左右移動時のＳＥを鳴らすためのフラグ
	u8	erase_anime;		//	消滅アニメーションコマ数
	u8	erase_anime_count;	//	消滅アニメーションカウンタ	//	粒落下のカウンタも兼ねる
	u8	erase_virus_count;	//	消滅ウイルス数
	u8	chain_count;		//	連鎖数
	u8	chain_line;			//	消滅列数
	u8	chain_color[4];		//	0: 赤 1:黄 2:青 消した色をカウントする。 3: 連鎖開始時に消した色のビットを立てる 0x01:赤 0x02:黄 0x04:青 ウイルス含む場合 0x80 のビットを立てる
	u8	warning_flg;		//	警戒音とうを鳴らすためのフラグ (ビットで制御する) 0x08:ウイルスラスト３ 0x80 積み上げすぎ
	u8	work_flg;			//	汎用性フラグ
	u8	retire_flg[3];		//	リタイアフラグ
	u8	player_state[3];	//	0:TRUE == コンピュータ 1:LEVEL(0.1.2); 2:プレイヤー番号
#ifdef	DAMAGE_TYPE
	u16	cap_attack_work[DAMAGE_MAX];	//	自分に落下する攻撃カプセルのワーク(2bitで制御,00:赤,01:黄,10:青,11:無し)
#endif
#ifndef	DAMAGE_TYPE
	u8	cap_attack_work[DAMAGE_MAX][6];	//	自分に落下する攻撃カプセルのワーク
#endif
	game_anime	anime;		//	アニメーション制御
	game_cap	now_cap;	//	現在操作するカプセル
	game_cap	next_cap;	//	次のカプセル
	AIWORK	ai;			// COM思考用ﾜｰｸ
	u8		pn;			// ﾌﾟﾚｲﾔｰ no.
	u8		gs;			// ｹﾞｰﾑｽﾃｰﾀｽ
	u8		lv;			// 設定ﾚﾍﾞﾙ(0-29)
	u8		vs;			// ｳｲﾙｽ残数(ｶｳﾝﾀではなく、毎ｲﾝﾄごとにﾏｯﾌﾟﾃﾞｰﾀ上から探した数字が入る)
	u8		pu[3];		// 0:TRUE==ｺﾝﾋﾟｭｰﾀ, 1:LEVEL( 0,1,2 ), 2:ﾃﾞﾓ no.
	AIBLK	blk[STD_MFieldY+1][STD_MFieldX];//ﾌﾞﾛｯｸﾜｰｸ
	AICAPS	cap;

}game_state;

//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
// aiset.c 内 ワーク

typedef struct {
	u8	ca;			// 初期左ｶﾌﾟｾﾙ色
	u8	cb;			//	〃 右　〃
} AI_NEXT;
AI_NEXT		aiNext;		// 次ｶﾌﾟｾﾙ色

typedef struct {
	u8	co;			// 色情報
	u8	st;			// ｽﾃｰﾀｽ情報
} AI_FIELD;
AI_FIELD	aiFieldData[FIELDY][FIELDX];		// ﾌｨｰﾙﾄﾞ情報
AI_FIELD	aiRecurData[FIELDY+1][FIELDX+2];	// ﾌｨｰﾙﾄﾞ情報(再帰検索用)

typedef struct {
	u8	tory;		// 縦/横ﾌﾗｸﾞ
	u8	x;			// X座標
	u8	y;			// Y座標
} AI_EDGE;			// ｴｯｼﾞ情報

u8	aiTEdgeCnt;		// 縦置き可能ｴｯｼﾞ数
u8	aiYEdgeCnt;		// 横置き	 〃

AI_FLAG	aiFlag[100];	// ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
u8	aiFlagCnt;			// ｶﾌﾟｾﾙを置ける位置数
u8	success;			// 経路okﾌﾗｸﾞ
u8	decide;				// 最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号

AI_ROOT	aiRoot[ROOTCNT];	// 投下ｶﾌﾟｾﾙのﾙｰﾄ
u8	aiRootCnt;				// ﾙｰﾄのｶｳﾝﾀ
u8	aiRollFinal;			// 最後に回転操作するかどうかﾌﾗｸﾞ

u8	aiUsedFlag;				// 1int中に１回しか思考処理が使われないようにするﾌﾗｸﾞ

u8	aipn;					// player 番号

u8	aiDebugP1 = 0;			// ﾃﾞﾊﾞｯｸﾞﾓｰﾄﾞ時の１ＰのＣＯＭ番号

float	aiRootP;			// 移動距離比率(移動範囲が狭くなっていることを警告する)

s16	aiHiEraseCtr;			// 連鎖ﾁｪｯｸ時に上の方が崩れた所をｶｳﾝﾄ
s16	aiHiErR,aiHiErB,aiHiErY;

u16	GOALX,GOALY;			// 検索開始座標(通常は4,1)

u8	aiWall;					// 左右の壁を検出

u8	aiMoveSF;				// 落下速度が速くなった時左右移動ステップを制限する

s8	aiSelSpeed;				// ＣＯＭの操作スピード速度番号
u8	aiSelCom;				// ＣＯＭの思考番号
s8	aiSelSpeedRensa;		// ＣＯＭの操作スピード速度番号（連鎖出来る時だけｽﾋﾟｰﾄﾞを変えたいとき使用）

u16	aiPriOfs;				// ＣＯＭの下手さ（優先順位にﾗﾝﾀﾞﾑで足す）

extern	AI_DATA	ai_param[4][8];			// 思考パラメータデータワーク（パラメータデータをデバッグモードで可変にするため元データから転送して使う）
extern	AI_DATA	ai_param_org[4][8];		// 思考パラメータデータ元

enum {
	AGST_GAME,		// ゲーム中
	AGST_TRAIN,		// 練習中
};





