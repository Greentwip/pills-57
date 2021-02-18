
#if !defined(_HARDCOPY_H_)
#define _HARDCOPY_H_

typedef enum {
	HC_NULL,
	HC_CURTAIN, // カーテン
	HC_SPHERE,  // 球
//	HC_OVI,
} HC_EFFECT;

// 画面切替エフェクトを初期化
// 戻値
//     確保したワークメモリーの最後尾
extern void *HCEffect_Init(
	void      *ptr,  // ワークメモリーの先頭
	HC_EFFECT effect // エフェクト番号
	);

// 画面切替メイン
// 戻値
//     0 : 既にエフェクトが終了している
//     1 : エフェクト実行中
extern int HCEffect_Main();

// 画面切替描画
extern void HCEffect_Grap(
	Gfx **gpp        // 構築先
	);

#endif
