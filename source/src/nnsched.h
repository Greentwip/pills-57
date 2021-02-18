
#ifndef _NN_SCHED_
#define _NN_SCHED_
#include <ultra64.h>

#define NN_SC_STACKSIZE	   0x2000	// �گ�޽�������
#define NN_SC_SWAPBUFFER   0x0040	// �ڰ��ޯ̧�̐؂�ւ�

#define NN_SC_RETRACE_MSG  1  // ��ڰ�ү����
#define NN_SC_DONE_MSG	   2  // ����I��ү����
#define NN_SC_PRE_NMI_MSG  3  // NMIү����
#define NN_SC_GTASKEND_MSG 4  // ����I��ү����(���̨�����������̂ݏI���̏ꍇ)
#define NN_SC_MAX_MESGS	   8  // ү�����ޯ̧�̻���

// �گ�ނ���ײ��è�̒�`
#define NN_SC_PRI		   120	// ���ޭ��
#define NN_SC_AUDIO_PRI	   110	// ���ި�
#define NN_SC_GRAPHICS_PRI 100	// ���̨���

// �����ү���ނ̒�`
#define VIDEO_MSG		666	 // ��ڰ�ү����
#define RSP_DONE_MSG	667	 // RSP����̏I��
#define RDP_DONE_MSG	668	 // RDP�`��̏I��
#define PRE_NMI_MSG		669	 // NMIү����

// ���ޭ��ү��������
typedef short NNScMsg;

// �ײ���ؽč\����
typedef struct SCClient_s {
	struct SCClient_s	*next;	// ���̸ײ��Ăւ��߲��
	OSMesgQueue			*msgQ;	// �ײ��Ăɑ��M����ү����
} NNScClient;

// ����\����
typedef struct SCTask_s {
	struct SCTask_s		*next;			// ����: ��ɍ\���̂̐擪�ɔz�u����
	u32					state;
	u32					flags;
	void				*framebuffer;	// ���̨�������p
	OSTask				list;
	OSMesgQueue			*msgQ;
	OSMesg				msg;
} NNScTask;

// ���ޭ�׍\���̂̒�`
typedef struct {

	// ү����
	NNScMsg		retraceMsg;
	NNScMsg		prenmiMsg;

	// ���ظ��ķ���̒�`
	OSMesgQueue audioRequestMQ;
	OSMesg		audioRequestBuf[NN_SC_MAX_MESGS];
	OSMesgQueue graphicsRequestMQ;
	OSMesg		graphicsRequestBuf[NN_SC_MAX_MESGS];

	// ү���޷���̒�`
	OSMesgQueue retraceMQ;
	OSMesg		retraceMsgBuf[NN_SC_MAX_MESGS];
	OSMesgQueue rspMQ;
	OSMesg		rspMsgBuf[NN_SC_MAX_MESGS];
	OSMesgQueue rdpMQ;
	OSMesg		rdpMsgBuf[NN_SC_MAX_MESGS];

	// ������ڰ��M����҂��߂Ɏg�p
	OSMesgQueue waitMQ;
	OSMesg		waitMsgBuf[NN_SC_MAX_MESGS];

	// �گ�ނ̒�`
	OSThread	schedulerThread;	// Ҳݽگ��
	OSThread	audioThread;		// ���ި�
	OSThread	graphicsThread;		// ���̨���

	// �ײ���ؽ�
	NNScClient	*clientList;

	// ���s���̸��̨������
	NNScTask	*curGraphicsTask;
	NNScTask	*curAudioTask;
	NNScTask	*graphicsTaskSuspended;

	// ���̑�
	u32	 firstTime;	 // ��ʂ���ׯ���Đ���p�׸�

} NNSched;

// �֐��������ߐ錾
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

// ��̫��ݽ����Ұ��֘A
extern u8		nnp_restrig;	// osGetTime�ر�ɂ���𔃂��Ă���
extern s8		nnp_over_time;

// ��̫��ݽҰ� ...OFF == ���ı��
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
#define NN_SC_GTASK_N_NUM   8   /* �O���t�B�b�N�^�X�N�ő吔 */
#define NN_SC_AUTASK_N_NUM   4  /* �I�[�f�B�I�^�X�N�ő吔 */

/* �p�t�H�[�}���X�`�F�b�N�p�\���� */
typedef struct {

  u32 gtask_cnt;                        /*�O���t�B�b�N�^�X�N�J�E���^ */
  u32 autask_cnt;                       /* �I�[�f�B�I�^�X�N�J�E���^ */
  u64 retrace_time;                     /* �O���t�B�b�N�쐬���� */
  u64 gtask_stime[NN_SC_GTASK_N_NUM];   /* �^�X�N�̊J�n���� */
  u64 rdp_etime[NN_SC_GTASK_N_NUM];     /* RDP�̏I������ */
  u64 rsp_etime[NN_SC_GTASK_N_NUM];     /* RSP�̏I������ */
  u64 autask_stime[NN_SC_AUTASK_N_NUM]; /* �I�[�f�B�I�^�X�N�J�n���� */
  u64 autask_etime[NN_SC_AUTASK_N_NUM]; /* �I�[�f�B�I�^�X�N�I������ */
  u32 endflag;
} NNScPerf2;

extern	NNScPerf2	*nnsc_perf_ptr;				/* �O������Q�Ƃ���Ƃ��̃|�C���^ */

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
