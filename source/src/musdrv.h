
#if !defined(_MUSDRV_H_)
#define _MUSDRV_H_

#include <libmus.h>
#include <libmus_data.h>

// サウンドドライバーを初期化する
extern int muInitDriver(
	NNSched *sched,			// ＮＮスケジューラのアドレス
	void    *heap,			// オーディオヒープのアドレス
	int      heapSize,		// オーディオヒープのサイズ
	int      pbkBufSize,	// サンプルポインタバンクのサイズ
	int      seqBufSize,	// シーケンスバッファのサイズ
	int      seqBufCount,	// シーケンスバッファの数
	int      fbkBufSize,	// サウンドエフェクトバンクのサイズ
	int      sndBufCount,	// サウンドバッファの数
	int      audioPri		// オーディオスレッドのプライオリティ
	);
// 戻値:
//   ミュージックプレイヤーとオーディオマネージャによって使用される、
//   ヒープからのメモリの量。



// サンプルポインタバンクを読み込む
extern int muSetPbkData(
	void *pbkRomAddr,	// サンプルポインタバンクのROMアドレス
	int   pbkSize,		// サンプルポインタバンクのサイズ
	void *wbkRomAddr	// サンプルウェーブバンクのROMアドレス
	);
// 戻値:
//   false: シーケンスを再生中であったため、データを読み込まなかった。
//   true : サンプルポインタバンクを読み込んだ。


// シーケンスデータを読み込む
extern int muSeqSetData(
	int   seqBufNo,		// シーケンスバッファの番号
	void *seqRomAddr,	// シーケンスデータのＲＯＭアドレス
	int   seqSize		// シーケンスデータのサイズ
	);
// 戻値:
//   false: 指定したシーケンスバッファは、シーケンスを再生中であったため、データを読み込まなかった。
//   true : シーケンスデータを読み込んだ。


// シーケンスデータを再生する
extern void muSeqPlay(
	int seqBufNo		// シーケンスバッファの番号
	);


// シーケンスハンドラを取得
extern musHandle muSeqGetHandle(
	int seqBufNo		// シーケンスバッファの番号
	);
// 戻値: シーケンスハンドラ


// いくつチャンネルが処理されているか調べる
extern int muSeqAsk(
	int seqBufNo		// シーケンスバッファの番号
	);
// 戻値: 処理されているチャンネル数


// シーケンスの再生を停止する
extern int muSeqStop(
	int seqBufNo,		// シーケンスバッファの番号
	int speed			// 停止するまでのフレーム数
	);
// 戻値: コマンドが実行されたチャンネル数


// シーケンスのボリュームを設定する
extern int muSeqSetVol(
	int seqBufNo,		// シーケンスバッファの番号
	int volume			// 設定するボリュームの値(0~256, 128で100%のボリューム)
	);
// 戻値: コマンドが実行されたチャンネル数


// シーケンスのパンを設定する
extern int muSeqSetPan(
	int seqBufNo,		// シーケンスバッファの番号
	int pan				// ステレオパンスケール値(0~256, 128でデフォルトの値)
	);
// 戻値: コマンドが実行されたチャンネル数


// シーケンスのテンポを変更する
extern int muSeqSetTempo(
	int seqBufNo,		// シーケンスバッファの番号
	int tempo			// テンポ(0~256, 128でデフォルトの値)
	);
// 戻値: コマンドが実行されたチャンネル数


// シーケンスの一時停止を行なう
extern int muSeqPause(
	int seqBufNo		// シーケンスバッファの番号
	);
// 戻値: もしコマンドが無視されれば0, そうでなければ0以外


// 一時停止したシーケンスを再開
extern int muSeqUnPause(
	int seqBufNo		// シーケンスバッファの番号
	);
// 戻値: もしコマンドが無視されれば0, そうでなければ0以外


// サウンドエフェクトバンクを読み込む
extern int muSetFbkData(
	void *fbkRomAddr,	// サウンドエフェクトバンクのROMアドレス
	int   fbkSize		// サウンドエフェクトバンクのサイズ
	);
// 戻値:
//   false: サウンドを再生中であったため、データを読み込まなかった。
//   true : サウンドエフェクトバンクを読み込んだ。


// サウンドエフェクトバンクのアドレスを返す
extern void *muGetFbkAddr();
// 戻値:
//   サウンドエフェクトバンクのアドレス


// サウンドデータを再生する
extern void muSndPlay(
	int sndBufNo,		// サウンドバッファの番号
	int number			// サウンドエフェクトの番号
	);


// サウンドデータを再生する2
extern void muSndPlay2(
	int sndBufNo,		// サウンドバッファの番号
	int number,			// サウンドエフェクトの番号
	int volume,			// ボリュームスケール
	int pan,			// パンスケール
	int overwrite,		// 同じ番号のオーバーライト（上書き用）サウンドエフェクト
	int priority		// プライオリティレベル
	);


// サウンドハンドラを取得
extern musHandle muSndGetHandle(
	int sndBufNo		// サウンドバッファの番号
	);
// 戻値: サウンドハンドラ


// いくつチャンネルが処理されているか調べる
extern int muSndAsk(
	int sndBufNo		// サウンドバッファの番号
	);
// 戻値: 処理されているチャンネル数


// サウンドの再生を停止する
extern int muSndStop(
	int sndBufNo,		// サウンドバッファの番号
	int speed			// 停止するまでのフレーム数
	);
// 戻値: コマンドが実行されたチャンネル数


// サウンドのボリュームを設定する
extern int muSndSetVol(
	int sndBufNo,		// サウンドバッファの番号
	int volume			// 設定するボリュームの値(0~256, 128で100%のボリューム)
	);
// 戻値: コマンドが実行されたチャンネル数


// サウンドのパンを設定する
extern int muSndSetPan(
	int sndBufNo,		// サウンドバッファの番号
	int pan				// ステレオパンスケール値(0~256, 128でデフォルトの値)
	);


// サウンドの周波数を設定
extern int muSndSetFreq(
	int sndBufNo,		// サウンドバッファの番号
	float offset		// 周波数のオフセット
	);
// 戻値: 周波数オフセットが適応されたチャンネル数


#endif // _MUSDRV_H_

