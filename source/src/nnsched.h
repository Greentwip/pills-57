
#ifndef _NN_SCHED_
#define _NN_SCHED_
#include <ultra64.h>

#define NN_SC_STACKSIZE	   0x2000	// ｽﾚｯﾄﾞｽﾀｯｸｻｲｽﾞ
#define NN_SC_SWAPBUFFER   0x0040	// ﾌﾚｰﾑﾊﾞｯﾌｧの切り替え

#define NN_SC_RETRACE_MSG  1  // ﾘﾄﾚｰｽﾒｯｾｰｼﾞ
#define NN_SC_DONE_MSG	   2  // ﾀｽｸ終了ﾒｯｾｰｼﾞ
#define NN_SC_PRE_NMI_MSG  3  // NMIﾒｯｾｰｼﾞ
#define NN_SC_GTASKEND_MSG 4  // ﾀｽｸ終了ﾒｯｾｰｼﾞ(ｸﾞﾗﾌｨｯｸﾀｽｸでﾀｽｸのみ終了の場合)
#define NN_SC_MAX_MESGS	   8  // ﾒｯｾｰｼﾞﾊﾞｯﾌｧのｻｲｽﾞ

// ｽﾚｯﾄﾞのﾌﾟﾗｲｵﾘﾃｨの定義
#define NN_SC_PRI		   120	// ｽｹｼﾞｭｰﾗ
#define NN_SC_AUDIO_PRI	   110	// ｵｰﾃﾞｨｵ
#define NN_SC_GRAPHICS_PRI 100	// ｸﾞﾗﾌｨｯｸｽ

// ｲﾍﾞﾝﾄﾒｯｾｰｼﾞの定義
#define VIDEO_MSG		666	 // ﾘﾄﾚｰｽﾒｯｾｰｼﾞ
#define RSP_DONE_MSG	667	 // RSPﾀｽｸの終了
#define RDP_DONE_MSG	668	 // RDP描画の終了
#define PRE_NMI_MSG		669	 // NMIﾒｯｾｰｼﾞ

// ｽｹｼﾞｭｰﾗﾒｯｾｰｼﾞﾀｲﾌﾟ
typedef short NNScMsg;

// ｸﾗｲｱﾝﾄﾘｽﾄ構造体
typedef struct SCClient_s {
	struct SCClient_s	*next;	// 次のｸﾗｲｱﾝﾄへのﾎﾟｲﾝﾀ
	OSMesgQueue			*msgQ;	// ｸﾗｲｱﾝﾄに送信するﾒｯｾｰｼﾞ
} NNScClient;

// ﾀｽｸ構造体
typedef struct SCTask_s {
	struct SCTask_s		*next;			// 注意: 常に構造体の先頭に配置する
	u32					state;
	u32					flags;
	void				*framebuffer;	// ｸﾞﾗﾌｨｯｸｽﾀｽｸ用
	OSTask				list;
	OSMesgQueue			*msgQ;
	OSMesg				msg;
} NNScTask;

// ｽｹｼﾞｭｰﾗ構造体の定義
typedef struct {

	// ﾒｯｾｰｼﾞ
	NNScMsg		retraceMsg;
	NNScMsg		prenmiMsg;

	// ﾀｽｸﾘｸｴｽﾄｷｭｰの定義
	OSMesgQueue audioRequestMQ;
	OSMesg		audioRequestBuf[NN_SC_MAX_MESGS];
	OSMesgQueue graphicsRequestMQ;
	OSMesg		graphicsRequestBuf[NN_SC_MAX_MESGS];

	// ﾒｯｾｰｼﾞｷｭｰの定義
	OSMesgQueue retraceMQ;
	OSMesg		retraceMsgBuf[NN_SC_MAX_MESGS];
	OSMesgQueue rspMQ;
	OSMesg		rspMsgBuf[NN_SC_MAX_MESGS];
	OSMesgQueue rdpMQ;
	OSMesg		rdpMsgBuf[NN_SC_MAX_MESGS];

	// 次のﾘﾄﾚｰｽ信号を待つために使用
	OSMesgQueue waitMQ;
	OSMesg		waitMsgBuf[NN_SC_MAX_MESGS];

	// ｽﾚｯﾄﾞの定義
	OSThread	schedulerThread;	// ﾒｲﾝｽﾚｯﾄﾞ
	OSThread	audioThread;		// ｵｰﾃﾞｨｵ
	OSThread	graphicsThread;		// ｸﾞﾗﾌｨｯｸｽ

	// ｸﾗｲｱﾝﾄﾘｽﾄ
	NNScClient	*clientList;

	// 実行中のｸﾞﾗﾌｨｯｸｽﾀｽｸ
	NNScTask	*curGraphicsTask;
	NNScTask	*curAudioTask;
	NNScTask	*graphicsTaskSuspended;

	// その他
	u32	 firstTime;	 // 画面のﾌﾞﾗｯｸｱｳﾄ制御用ﾌﾗｸﾞ

} NNSched;

// 関数ﾌﾟﾛﾄﾀｲﾌﾟ宣言
extern void			nnScCreateScheduler(NNSched *sc, u8 videoMode, u8 numFields);
extern void			nnScAddClient(NNSched *sc, NNScClient *, OSMesgQueue *mq);
extern void			nnScRemoveClient(NNSched *sc, NNScClient *client);
extern void			nnScEventHandler(NNSched *sc);
extern void			nnScEventBroadcast(NNSched *sc, NNScMsg *msg);
extern void			nnScExecuteAudio(NNSched *sc);
extern void			nnScExecuteGraphics(NNSched *sc);
extern void			nnScExecuteGraphicsCore(NNSched*,NNScTask*);
extern void			nnScWaitTaskReady(NNSched *sc, NNScTask *task);
extern OSMesgQueue*	nnScGetGfxMQ(NNSched *sc);
extern OSMesgQueue*	nnScGetAudioMQ(NNSched *sc);

// ﾊﾟﾌｫｰﾏﾝｽﾁｪｯｸﾒｰﾀ関連
extern u8		nnp_restrig;	// osGetTimeｸﾘｱにも一役買っている
extern s8		nnp_over_time;

// ﾊﾟﾌｫｰﾏﾝｽﾒｰﾀ ...OFF == ｺﾒﾝﾄｱｳﾄ
//#if defined(DEBUG)
#define NN_SC_PERF
#define NN_SC_PERF_2
//#endif

#ifdef NN_SC_PERF

#define NN_SC_PERF_NUM 32

// NNScPerf.task
enum {
	NN_SC_PERF_CPU,
	NN_SC_PERF_MAIN,
	NN_SC_PERF_GRPHm,
	NN_SC_PERF_GRPHg,
	NN_SC_PERF_JOY,
	NN_SC_PERF_RSP,
	NN_SC_PERF_RDP,
	NN_SC_PERF_AUDIO,
	NN_SC_PERF_CHECK1,
	NN_SC_PERF_CHECK2,
	NN_SC_PERF_NOP,
	NN_SC_PERF_ERROR,
	NN_SC_PERF_START,
	NN_SC_PERF_OVER,
};

// NNScPerf.info
enum {
	NN_SC_PERF_INFO_Count,
	NN_SC_PERF_INFO_Push,
	NN_SC_PERF_INFO_Pop,
};

typedef struct {
	u8	task;
	u8	info;
	u32 start;
	u32 stop;
} NNScPerf;

#define NN_SC_PERF_N_NUM    4
#define NN_SC_GTASK_N_NUM   8   /* グラフィックタスク最大数 */
#define NN_SC_AUTASK_N_NUM   4  /* オーディオタスク最大数 */

/* パフォーマンスチェック用構造体 */
typedef struct {

  u32 gtask_cnt;                        /*グラフィックタスクカウンタ */
  u32 autask_cnt;                       /* オーディオタスクカウンタ */
  u64 retrace_time;                     /* グラフィック作成時間 */
  u64 gtask_stime[NN_SC_GTASK_N_NUM];   /* タスクの開始時間 */
  u64 rdp_etime[NN_SC_GTASK_N_NUM];     /* RDPの終了時間 */
  u64 rsp_etime[NN_SC_GTASK_N_NUM];     /* RSPの終了時間 */
  u64 autask_stime[NN_SC_AUTASK_N_NUM]; /* オーディオタスク開始時間 */
  u64 autask_etime[NN_SC_AUTASK_N_NUM]; /* オーディオタスク終了時間 */
  u32 endflag;
} NNScPerf2;

extern	NNScPerf2	*nnsc_perf_ptr;				/* 外部から参照するときのポインタ */

extern s8		nnp_over_time;
extern u8		nnp_wf,nnp_wb;
extern u8		nnp_taskcnt[2];
extern NNScPerf	nnp_tasktbl[2][NN_SC_PERF_NUM];
extern u8		nnp_stakptr[2];
extern u8		nnp_staktbl[2][NN_SC_PERF_NUM];
extern NNScPerf	*nnp_ttblnow,*nnp_ttblbef;
extern u8		*nnp_staknow;

extern void nnScResetPerfMeter(void);
extern void nnScMyStartPerfMeter(void);
extern void nnScCountUpPerfMeter(u8,u8);
extern void nnScPushPerfMeter(u8);
extern void nnScPopPerfMeter(void);
extern void nnScWritePerfMeter(Gfx**,u16,u16);

#endif // NN_SC_PERF

#endif // _NN_SCHED_
