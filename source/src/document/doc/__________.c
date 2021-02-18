//*** 渡辺ﾜｰｸを思考用ﾜｰｸに変換 ************************
//	aifMakeWork
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	uupw			ﾌﾟﾚｲﾔｰ構造体
//		AICAPS		uupw->cap		AI用操作ｶﾌﾟｾﾙ情報を受け取るﾜｰｸ
//
void aifMakeWork(game_state *uupw)

//*** 渡辺ﾌｨｰﾙﾄﾞﾜｰｸを思考用ﾌｨｰﾙﾄﾞﾜｰｸに変換 ************************
//	aifMakeBlkWork
//in:	game_map	*game_map_data	ﾌﾟﾚｲﾔｰﾏｯﾌﾟﾌｨｰﾙﾄﾞ
//out:	AIBLK		uupw->blk		mﾌｨｰﾙﾄﾞ上のｶﾌﾟｾﾙ&ｳｨﾙｽの状態ﾜｰｸ
//
void aifMakeBlkWork(game_state *uupw)

//*** 思考開始フラグ設定（落下開始時思考処理を呼び出す）************************
//	aifMakeFlagSet
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifMakeFlagSet( game_state *uupw )

//*** ゲーム開始時初期化処理 ************************
//	aifGameInit
//in:	none
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifGameInit(void)

//*** プログラム開始時初期化処理 ************************
//	aifFirstInit
//in:	none
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifFirstInit(void)

//*** ＣＰＵＡＩ処理メインプログラム ************************
//	aifMake
//in:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	*uupw			ﾌﾟﾚｲﾔｰ構造体
//
void aifMake(game_state* uupw)

//*** 天井が近すぎてｶﾌﾟｾﾙを積んでも消せないｳｨﾙｽを塔を建てて消しに行く処理 ************************
//	aiHiruSideLineEraser
//in:	game_state	xpw				ﾌﾟﾚｲﾔｰ構造体
//out:	game_state	xpw				ﾌﾟﾚｲﾔｰ構造体
//
void aiHiruSideLineEraser
(
	game_state*	xpw
)

//*** 消去されたｶﾌﾟｾﾙの反対側を玉に変換する ************************
//	aif_MiniChangeBall
//in:	u8			x,y			消去されたｶﾌﾟｾﾙの座標位置
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//
aif_MiniChangeBall( u8 y, u8 x )

//*** 実際に消去した状況を作る、消去ｳｨﾙｽ数もｶｳﾝﾄ ************************
//	aifEraseLineCore
//in:	int			mx,my		消去ﾁｪｯｸ座標
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	int						(未使用)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//
int aifEraseLineCore
(
	int	mx, int my	// ﾀｰｹﾞｯﾄﾎﾟｼﾞｼｮﾝ
)

//*** 連鎖数ﾁｪｯｸ（同時に危険ゾーンのカプセルが消えるかどうかの計算も含む） ************************
//	aifRensaCheckCore
//in:	game_state	uupw		ﾌﾟﾚｲﾔｰ構造体
//		AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//		u8			mx,my		ﾏｽﾀｰｶﾌﾟｾﾙの座標x､y
//		u8			mco,mst		ﾏｽﾀｰｶﾌﾟｾﾙの色､形
//		u8			sx,sy		ｽﾚｰﾌﾞｶﾌﾟｾﾙの座標x､y
//		u8			sco,sst		ｽﾚｰﾌﾞｶﾌﾟｾﾙの色､形
//out:	int						消去で１、同時消しで２、消せない時は０を返す
//		s16			aiHiEraseCtr	上の方が崩れた所をｶｳﾝﾄ
//
int aifRensaCheckCore(game_state* uupw ,AI_FLAG* af ,u8 mx,u8 my,u8 mco,u8 mst,u8 sx,u8 sy,u8 sco,u8 sst)

//*** 現在の操作カプセルを設定して連鎖チェック（同時に危険ゾーンのカプセルが消えるかどうかの計算も含む） ************************
//	aifRensaCheck
//in:	game_state	uupw		ﾌﾟﾚｲﾔｰ構造体
//		AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//out:	int						消去で１、同時消しで２、消せない時は０を返す
//		s16			aiHiEraseCtr	上の方が崩れた所をｶｳﾝﾄ
//
int aifRensaCheck(game_state* uupw ,AI_FLAG* af)

//*** 操作カプセルを置いた所の周辺ラインを検索 ************************
//	aifSearchLineCore
//in:	int			mx,my		検索する座標
//		int			fg			縦､横どこを検索するかﾌﾗｸﾞ(SL_???)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//out:	int						TRUE：消去でFALSE：未消去、NULL：消す前から何もない
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//
int aifSearchLineCore
(
	int	mx,int my,	// ﾀｰｹﾞｯﾄﾎﾟｼﾞｼｮﾝ
	int	fg			// SL_???
)

//*** ラインごとのﾎﾟｲﾝﾄ計算処理 ************************
//	aifMiniPointK3
//in:	u8			*tbl		消去ﾁｪｯｸ座標のラインの情報のﾎﾟｲﾝﾀ
//		u8			sub			このラインはﾎﾟｲﾝﾄ算出上ﾒｲﾝかｻﾌﾞか？（TRUE:ｻﾌﾞ）
//		u8			*elin		消去ﾗｲﾝ数保存ﾜｰｸのﾎﾟｲﾝﾀ
//		u8			flag		縦横どちらのﾎﾟｲﾝﾄを算出するか(TRUE:縦)
//		u8			tory		操作ｶﾌﾟｾﾙの向き
//		u8			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//out:	int						算出ポイント数
//
int	aifMiniPointK3( u8* tbl,u8 sub,u8* elin,u8 flag,u8 tory,u8 ec)

//*** 色の違う所にカプセルを置いてしまったかどうかのチェック ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			置いた位置をﾁｪｯｸする座標
//		u8			f			落下したかどうか(TRUE:落下した)
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			hei_data	消去ﾁｪｯｸ座標の縦ラインの情報
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//out:	int						違う色の所においてしまった時の状態番号(ALN_????)
//
int	aifMiniAloneCapNumber
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = 落下した
	int	ec
)

//*** 色の違う所にカプセルを置いてしまったかどうかのチェック（横ライン専用） ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			置いた位置をﾁｪｯｸする座標
//		u8			f			落下したかどうか(TRUE:落下した)
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//		AI_FIELD	aif_field	mﾌｨｰﾙﾄﾞ
//		u8			wid_data	消去ﾁｪｯｸ座標の横ラインの情報
//out:	int						違う色の所においてしまった時の状態番号(ALN_????)
//
int	aifMiniAloneCapNumberW
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = 落下した
	int	ec
)

//*** 優先順位決定用ポイント数計算処理メイン ************************
//	aifSearchLineMS
//in:	AI_FLAG		af			移動ｶﾌﾟｾﾙの状態
//		int			mx,my,mco	操作ｶﾌﾟｾﾙのﾏｽﾀｰの座標x,yと色
//		int			sx,sy,sco	操作ｶﾌﾟｾﾙのｽﾚｰﾌﾞの座標x,yと色
//		int			ec			操作ｶﾌﾟｾﾙ同色ﾌﾗｸﾞ(TRUE:同色)
//out:	int						1:ﾏｽﾀｰが消去、2:ｽﾚｰﾌﾞが消去、0:消去していない
//		AI_FLAG		pri			ﾎﾟｲﾝﾄ計算算出値
//
int aifSearchLineMS
(
	AI_FLAG*  af,
	int	mx,	int	my,	int	mco,	// master
	int	sx,	int	sy,	int	sco,	// slave
	int	ec						// 同色 FALSE/TRUE
)

//*** 置き場所決定 ************************
//	aiHiruAllPriSet
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//		AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//out:	u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//
void aiHiruAllPriSet(game_state *xpw)

//*** キャラクターの性格設定 ************************
//	aiSetCharacter
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//		AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//		AI_DATA		ai_param	思考ﾊﾟﾗﾒｰﾀﾃｰﾌﾞﾙ
//out:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//
void aiSetCharacter(game_state *xpw)

//*** ﾌｨｰﾙﾄﾞﾃﾞｰﾀの再設定（コピー） ************************
//	aifFieldCopy
//in:	game_state	xpw			ﾌﾟﾚｲﾔｰ構造体
//out:	AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//
void aifFieldCopy(game_state* uupw)

//*** ﾙｰﾄ検索専用ﾌｨｰﾙﾄﾞﾃﾞｰﾀの再設定（コピー） ************************
//	aifRecurCopy
//in:	AI_FIELD	aiFieldData	mﾌｨｰﾙﾄﾞ
//out:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//
void aifRecurCopy(void)

//*** ｶﾌﾟｾﾙを置ける場所の検索 ************************
//	aifPlaceSearch
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//
void aifPlaceSearch(void)

//*** ｶﾌﾟｾﾙを置ける所から移動開始位置まで移動可能検索 ************************
//	aifMoveCheck
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//out:	AI_FLAG		aiFlag[].ok	移動可能かどうかフラグ(1:成功､0:失敗)
//		float		aiRootP		全移動量の平均値（移動範囲が狭すぎるのをﾁｪｯｸするためのﾜｰｸ）
//
void aifMoveCheck(void)

//*** 縦置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ ************************
//	aifTRecur
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifTRecur(u8 x,u8 y,u8 cnt)

//*** 縦置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ(上移動専用) ************************
//	aifTRecurUP
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifTRecurUP(u8 x,u8 y,u8 cnt)

//*** 横置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ ************************
//	aifYRecur
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifYRecur(u8 x,u8 y,u8 cnt)

//*** 横置きｶﾌﾟｾﾙ 移動可能ｻｰﾁ(上移動専用) ************************
//	aifYRecurUP
//in:	u8			x,y			ﾌｨｰﾙﾄﾞ移動検索中の現在地
//		u8			cnt			現在検索しているｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報の番号
//		AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//out:	u8			success		1:経路検索が成功した時
//		AI_ROOT		aiRoot		成功時移動経路保存
//		u8			aiRootCnt	移動経路保存ｶｳﾝﾀ
//
void aifYRecurUP(u8 x,u8 y,u8 cnt)

//*** 落す位置決定後もう一度ﾙｰﾄ検索 ************************
//	aifReMoveCheck
//in:	AI_FIELD	aiRecurData	mﾌｨｰﾙﾄﾞ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			aiFlagCnt	ｶﾌﾟｾﾙを置ける位置の数
//		u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//out:	AI_FLAG		aiFlag[].ok	移動可能かどうかフラグ(1:成功､0:失敗)
//		float		aiRootP		全移動量の平均値（移動範囲が狭すぎるのをﾁｪｯｸするためのﾜｰｸ）
//
void aifReMoveCheck(void)

//*** 移動位置決定後キー情報作成 ************************
//	aifKeyMake
//in:	game_state	*uupw		ﾌﾟﾚｲﾔｰ構造体ﾎﾟｲﾝﾀ
//		AI_FLAG		aiFlag[]	ｶﾌﾟｾﾙを置ける位置＆ﾙｰﾄ情報
//		u8			decide		最終的決定するﾌﾗｸﾞﾃﾞｰﾀ番号
//		s8			aiSelSpeedRensa		ＣＯＭの操作スピード速度番号（連鎖出来る時だけｽﾋﾟｰﾄﾞを変えたいとき使用）
//out:	AIWORK		uupw->ai	各種ｷｰ操作情報
//		s8			aiSelSpeed	ＣＯＭの操作スピード速度番号
//
void aifKeyMake(game_state* uupw)

//*** ＣＯｷｰ操作情報出力 ************************
//	aifKeyOut
//in:	game_state	*uupw		ﾌﾟﾚｲﾔｰ構造体ﾎﾟｲﾝﾀ
//		AIWORK		uupw->ai	各種ｷｰ操作情報
//out:	u16			joygam		ｺﾝﾄﾛｰﾗｰ仕様のｷｰ情報出力
//
void aifKeyOut(game_state* uupw)


