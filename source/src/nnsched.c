#include "nnsched.h"
#include "joy.h"

// �گ�ޗp�̽����̊m��
u64 nnScStack[NN_SC_STACKSIZE/8];
u64 nnScAudioStack[NN_SC_STACKSIZE/8];
u64 nnScGraphicsStack[NN_SC_STACKSIZE/8];
u32 framecont = 0;

// ��̫��ݽҰ��֘A
u8	nnp_restrig = TRUE;	// osGetTime�ر�ɂ���𔃂��Ă���( NN_SC_PERF�e������Ȃ� )
s8			nnp_over_time = 0;

#ifdef NN_SC_PERF

u8			nnp_wf,nnp_wb;
u8			nnp_taskcnt[2];
NNScPerf	nnp_tasktbl[2][NN_SC_PERF_NUM];
u8			nnp_stakptr[2];
u8			nnp_staktbl[2][NN_SC_PERF_NUM];
NNScPerf	*nnp_ttblnow,*nnp_ttblbef;
u8			*nnp_staknow;

#ifdef NN_SC_PERF_2
u32      	nnsc_perf_index = 0; 			/* �����Ŏg�p���Ă���o�b�t�@�ʒu */
u32      	nnsc_perf_flag = 0;
NNScPerf2	nnsc_perf[NN_SC_PERF_N_NUM];	/* �v���f�[�^ */
NNScPerf2	*nnsc_perf_inptr;			/* �����Ŏg�p����|�C���^ */
NNScPerf2	*nnsc_perf_ptr;				/* �O������Q�Ƃ���Ƃ��̃|�C���^ */
#endif

#endif

//////////////////////////////////////////////////////////////////////////////
// ���ޭ�ׂ̍쐬
void nnScCreateScheduler(NNSched *sc, u8 videoMode, u8 numFields)
{
	// �ϐ��̏�����
	sc->curGraphicsTask			= 0;
	sc->curAudioTask			= 0;
	sc->graphicsTaskSuspended	= 0;
	sc->clientList				= 0;
	sc->firstTime				= 1;
	sc->retraceMsg				= NN_SC_RETRACE_MSG;
	sc->prenmiMsg				= NN_SC_PRE_NMI_MSG;

	// �e��ү���޷���̍쐬
	osCreateMesgQueue(&sc->retraceMQ, sc->retraceMsgBuf, NN_SC_MAX_MESGS);				// retrace
	osCreateMesgQueue(&sc->rspMQ, sc->rspMsgBuf, NN_SC_MAX_MESGS);						// rsp
	osCreateMesgQueue(&sc->rdpMQ, sc->rdpMsgBuf, NN_SC_MAX_MESGS);						// rdp
	osCreateMesgQueue(&sc->graphicsRequestMQ, sc->graphicsRequestBuf, NN_SC_MAX_MESGS);	// graphic
	osCreateMesgQueue(&sc->audioRequestMQ, sc->audioRequestBuf, NN_SC_MAX_MESGS);		// audio
	osCreateMesgQueue(&sc->waitMQ, sc->waitMsgBuf, NN_SC_MAX_MESGS);					// wait

	// ���޵Ӱ�ނ̐ݒ�
	osCreateViManager(OS_PRIORITY_VIMGR);	//
	osViSetMode(&osViModeTable[videoMode]);	// ���Ӱ��( LAN,HAF etc... )
	osViBlack(TRUE);

	// ���������ׂ̓o�^
	osViSetEvent(&sc->retraceMQ, (OSMesg)VIDEO_MSG, numFields);				// ү���޷��mq��ү����m��VI�Ȱ�ެ�ɓo�^ / numFields: ������ڲ���
	osSetEventMesg(OS_EVENT_SP, &sc->rspMQ, (OSMesg)RSP_DONE_MSG);			// rspү����
	osSetEventMesg(OS_EVENT_DP, &sc->rdpMQ, (OSMesg)RDP_DONE_MSG);			// rdp	..
	osSetEventMesg(OS_EVENT_PRENMI, &sc->retraceMQ, (OSMesg)PRE_NMI_MSG);	// prenmi ..

	// ���ޭ�׽گ�ނ̋N��
	osCreateThread(&sc->schedulerThread, 19, (void(*)(void*))nnScEventHandler,			// nnScEventHandler
		(void *)sc, nnScStack+NN_SC_STACKSIZE/sizeof(u64), NN_SC_PRI);
	osStartThread(&sc->schedulerThread);
#ifdef NN_SC_PERF
	nnScResetPerfMeter();
#endif

	// ���ި��گ�ނ̋N��
	osCreateThread(&sc->audioThread, 18, (void(*)(void *))nnScExecuteAudio,				// nnScExecuteAudio
		(void *)sc, nnScAudioStack+NN_SC_STACKSIZE/sizeof(u64), NN_SC_AUDIO_PRI);
	osStartThread(&sc->audioThread);

	// ���̨���گ�ނ̋N��
	osCreateThread(&sc->graphicsThread, 17,(void(*)(void*))nnScExecuteGraphics,			// nnScExecuteGraphics
		(void *)sc, nnScGraphicsStack+NN_SC_STACKSIZE/sizeof(u64), NN_SC_GRAPHICS_PRI);
	osStartThread(&sc->graphicsThread);
}


//////////////////////////////////////////////////////////////////////////////
// ���ި�ү���޷���̎擾
OSMesgQueue *nnScGetAudioMQ(NNSched *sc)
{
	return( &sc->audioRequestMQ );
}

//////////////////////////////////////////////////////////////////////////////
// ���̨��ү���޷���̎擾
OSMesgQueue *nnScGetGfxMQ(NNSched *sc)
{
  return( &sc->graphicsRequestMQ );
}

//////////////////////////////////////////////////////////////////////////////
// ���Ѳ���Ă̏���
void nnScEventHandler(NNSched *sc)
{
	OSMesg msg = (OSMesg)0;

	while(1) {
		// ����Ă̎擾
		osRecvMesg(&sc->retraceMQ, &msg, OS_MESG_BLOCK);
		if(!GameHalt) framecont++;

		switch ( (int)msg ) {
			// ��ڰ��M���̏���
			case VIDEO_MSG:
				nnScEventBroadcast( sc, &sc->retraceMsg );	// ...��ڰ�ү���ނ��K�v�ȸײ��Ăɒʒm
//				if( nnp_restrig == TRUE ) {
					osSetTime(0);
//				}
#ifdef NN_SC_PERF
				nnScMyStartPerfMeter();
#ifdef NN_SC_PERF_2
				if( nnsc_perf_flag  == 0){
					/* �O���t�B�b�N�^�X�N�����ׂďI������܂ŏ��������Ȃ��悤�ɂ��� */
					nnsc_perf_flag++;

					/* �v�������o�b�t�@�̃|�C���^���O������Q�Ƃł���悤�ɂ��� */
					nnsc_perf_ptr = &nnsc_perf[nnsc_perf_index];

					nnsc_perf_index++;  /* �o�b�t�@�̐؂�ւ� */
					nnsc_perf_index %= NN_SC_PERF_N_NUM;

					nnsc_perf_inptr = &nnsc_perf[nnsc_perf_index];

					nnsc_perf_inptr->autask_cnt =  0;
					nnsc_perf_inptr->gtask_cnt = 0;

					/* ���g���[�X���Ԃ̎擾 */
					nnsc_perf_inptr->retrace_time = OS_CYCLES_TO_USEC(osGetTime());

				}
#endif
#endif
				break;
			// NMI�M���̏���
			case PRE_NMI_MSG:
				nnScEventBroadcast( sc, &sc->prenmiMsg );
				break;


		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// �ײ��Ă̓o�^
void nnScAddClient(NNSched *sc, NNScClient *c, OSMesgQueue *msgQ)
{
	OSIntMask mask;

	mask = osSetIntMask(OS_IM_NONE);	// ���荞�݋���Ͻ��̾��( OS_IM_NONE: ���荞�݋����� )
	c->msgQ = msgQ;						//
	c->next = sc->clientList;			//
	sc->clientList = c;					//
	osSetIntMask(mask);					//
}

//////////////////////////////////////////////////////////////////////////////
// �ײ��Ă̖���
void nnScRemoveClient(NNSched *sc, NNScClient *c)
{
	NNScClient *client = sc->clientList;
	NNScClient *prev   = 0;
	OSIntMask  mask;

	mask = osSetIntMask(OS_IM_NONE);

	while (client != 0) {
		if (client == c) {
			if(prev) prev->next		= c->next;
			else	 sc->clientList = c->next;
			break;
		}
		prev   = client;
		client = client->next;
	}
	osSetIntMask(mask);
}

//////////////////////////////////////////////////////////////////////////////
// �e�ײ��Ă�ү���ނ�]��
void nnScEventBroadcast(NNSched *sc, NNScMsg *msg)
{
	NNScClient *client;

	// ��ڰ�ү���ނ��K�v�ȸײ��Ăɒʒm
	for (client = sc->clientList; client != 0; client = client->next) {
		osSendMesg(client->msgQ, (OSMesg *)msg, OS_MESG_NOBLOCK);		// ү���ޑ��M
	}
}

//////////////////////////////////////////////////////////////////////////////
// ���ި�����̎��s
void nnScExecuteAudio(NNSched *sc)
{
	OSMesg	 msg		= (OSMesg)0;
	NNScTask *task		= (NNScTask *)0;
	NNScTask *gfxTask	= (NNScTask *)0;
	NNScTask *audioTask = (NNScTask *)0;
	u32		 yieldFlag	= 0;
#ifdef NN_SC_PERF
#ifdef NN_SC_PERF_2
	u32 task_cnt;
#endif
#endif

	while(1) {
		// ���ި��̎��s�v����҂�
		osRecvMesg(&sc->audioRequestMQ, (OSMesg *)&task, OS_MESG_BLOCK);	// ���ި��̎��s�v����҂�
		osWritebackDCacheAll();												// ��������ׯ��

		// ���݂� RSP�ð���̌���
		yieldFlag = 0;
		gfxTask = sc->curGraphicsTask;
		if( gfxTask ) {
			// ���̨�������̏I���iyield�j��҂�
			osSpTaskYield();								// SP"yield"��ظ���, / yield�͸��̨��������ĊJ�ł����߲�ĂŽį�߂�����.
			osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);	// rsp�I���҂�

			// ���ۂ������yield���ꂽ��������
			if (osSpTaskYielded(&gfxTask->list)) yieldFlag = 1;	// �^�X�N��yield������ = OS_TASK_YIELDED���Ԃ�, �����łȂ��ꍇ��0��Ԃ�.
			else								 yieldFlag = 2;	//	 ...yield����O�Ƀ^�X�N���ʏ�I��, ��ōĊJ���Ȃ��Ă��񂾂��Ƃ��Ӗ�.
		}

#ifdef NN_SC_PERF
		nnScPushPerfMeter(NN_SC_PERF_AUDIO);
#ifdef	NN_SC_PERF_2
		if(nnsc_perf_inptr->autask_cnt < NN_SC_AUTASK_N_NUM){
			task_cnt = nnsc_perf_inptr->autask_cnt;
			nnsc_perf_inptr->autask_stime[task_cnt] =
				OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
		}
#endif
#endif

		sc->curAudioTask = task;	//
		osSpTaskStart(&task->list); // ���ި�����̎��s

		// RSP����̏I����҂�
		osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
		sc->curAudioTask = (NNScTask *)0;

#ifdef NN_SC_PERF
		nnScPopPerfMeter();
#ifdef	NN_SC_PERF_2
		if(nnsc_perf_inptr->autask_cnt < NN_SC_AUTASK_N_NUM){
			nnsc_perf_inptr->autask_etime[task_cnt] =
				OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
			nnsc_perf_inptr->autask_cnt++;
		}
#endif
#endif

		// ���̨������̻�����ނȂ�wait��ү���ޑ��M
		if( sc->graphicsTaskSuspended ) {
			osSendMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );
		}

		// `yield'�������̨�������̍ĊJ
		if( yieldFlag == 1 ) {
			// yield����Ă����Ȃ�ؽ���
			osSpTaskStart(&gfxTask->list);
		}
		else if ( yieldFlag == 2 ) {
			// rspү���ޑ҂�
			osSendMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
		}
		// ���ި�������N�������گ�ނ�����̏I����ʒm
		osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);
	}
}

//////////////////////////////////////////////////////////////////////////////
// ���̨�������̎��s
void nnScExecuteGraphicsCore(NNSched *sc,NNScTask *task)
{
	OSMesg msg = (OSMesg)0;
	int	i;
#ifdef NN_SC_PERF
#ifdef NN_SC_PERF_2
	u32 task_cnt;
#endif
#endif

		// �ڰ��ޯ̧�����p�\�ɂȂ�̂�҂�
		nnScWaitTaskReady(sc, task);

		if( sc->curAudioTask ) {
			sc->graphicsTaskSuspended = task;
			osRecvMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );
			sc->graphicsTaskSuspended = (NNScTask *)0;
		}

#ifdef NN_SC_PERF
		nnScPushPerfMeter(NN_SC_PERF_RSP);
#ifdef NN_SC_PERF_2
		if(nnsc_perf_inptr->gtask_cnt < NN_SC_GTASK_N_NUM) {
			task_cnt = nnsc_perf_inptr->gtask_cnt;
			nnsc_perf_inptr->gtask_stime[task_cnt] =
				OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
		}
#endif
#endif

		sc->curGraphicsTask = task;
		osSpTaskStart(&task->list);		   // ���̨������̎��s

		// RSP����̏I����҂�
		osRecvMesg(&sc->rspMQ, &msg, OS_MESG_BLOCK);
		sc->curGraphicsTask = (NNScTask *)0;
#ifdef NN_SC_PERF
		nnScPushPerfMeter(NN_SC_PERF_RDP);
#ifdef NN_SC_PERF_2
		if(nnsc_perf_inptr->gtask_cnt < NN_SC_GTASK_N_NUM){
			nnsc_perf_inptr->rsp_etime[task_cnt] =
				OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
		}
#endif
#endif

		// RDP����̏I����҂�
		osRecvMesg(&sc->rdpMQ, &msg, OS_MESG_BLOCK);

#ifdef NN_SC_PERF
			nnScPopPerfMeter();
			nnScPopPerfMeter();
#ifdef NN_SC_PERF_2
			if(nnsc_perf_inptr->gtask_cnt < NN_SC_GTASK_N_NUM) {
				nnsc_perf_inptr->rdp_etime[task_cnt] =
					OS_CYCLES_TO_USEC(osGetTime()) - nnsc_perf_inptr->retrace_time;
				nnsc_perf_inptr->gtask_cnt++;
			}
#endif
#endif

		// ���񂾂����޵����ׯ���Ă𖳌��ɂ���
		if (sc->firstTime) {
			osViBlack(FALSE);
			sc->firstTime = 0;
		}

		// ���ɕ\��������ڰ��ޯ̧�̎w��
		if ( task->flags & NN_SC_SWAPBUFFER ){
			osViSwapBuffer(task->framebuffer);
			nnp_restrig = TRUE;
#ifdef NN_SC_PERF
#ifdef NN_SC_PERF_2
			nnsc_perf_flag = 0;
#endif
#endif
		}

		// ���̨���������N�������گ�ނ�����̏I����ʒm
		osSendMesg(task->msgQ, task->msg, OS_MESG_BLOCK);

}

void nnScExecuteGraphics(NNSched *sc)
{
	NNScTask *task;
	while(1) {
		// ���̨������̎��s�v����҂�
		osRecvMesg(&sc->graphicsRequestMQ, (OSMesg *)&task, OS_MESG_BLOCK);
		nnScExecuteGraphicsCore((void*)sc,(void*)task);
	}
}

//////////////////////////////////////////////////////////////////////////////
// �ڰ��ޯ̧�����p�\�ɂȂ�̂�҂B
void nnScWaitTaskReady(NNSched *sc, NNScTask *task)
{
	OSMesg msg = (OSMesg)0;
	NNScClient client;
	void *fb = task->framebuffer;

	// �ڰ��ޯ̧���󂢂Ă��Ȃ���Ύ�����ڰ��܂ő҂�
	while( osViGetCurrentFramebuffer() == fb || osViGetNextFramebuffer() == fb ) {	// �����ި���ڲ����Ă����ڰ��ޯ̧�ւ��߲����Ԃ�
		nnScAddClient( sc, &client, &sc->waitMQ );		// wait�ײ��Ă̓o�^
		osRecvMesg( &sc->waitMQ, &msg, OS_MESG_BLOCK );	//	.. ү���ޑ҂�
		nnScRemoveClient( sc, &client );				//	.. �ײ��Ă̖���
	}
}

//////////////////////////////////////////////////////////////////////////////
// ��̫��ݽҰ�����
#ifdef NN_SC_PERF

void nnScResetPerfMeter( void )
{
	int i;
	nnp_wb = nnp_wf = 1;
	nnp_ttblbef = nnp_tasktbl[nnp_wb];
	for( i=0;i<2;i++ ) {
		nnp_taskcnt[i] = nnp_stakptr[i] = 0;
	}
}

void nnScMyStartPerfMeter( void )
{
	int i;
	if( nnp_restrig == TRUE ) {
		nnp_ttblbef = nnp_ttblnow;
		nnp_wb = nnp_wf;
		nnp_wf ^=1;
		nnp_ttblnow = nnp_tasktbl[nnp_wf];
		nnp_staknow = nnp_staktbl[nnp_wf];
		for(i=0;i<NN_SC_PERF_NUM;i++) {
			nnp_ttblnow[i].task = NN_SC_PERF_NOP;
			nnp_ttblnow[i].start = nnp_ttblnow[i].stop = 0;
			nnp_staknow[i] = NN_SC_PERF_NOP;
		}
		nnp_taskcnt[nnp_wf] = 1;
		nnp_ttblnow[0].task = NN_SC_PERF_START;	// Promise: start from "EventHandler"
		nnp_ttblnow[0].start = OS_CYCLES_TO_USEC(osGetTime());
		nnp_stakptr[nnp_wf] = 0;
		nnp_restrig = FALSE;
		nnScCountUpPerfMeter( NN_SC_PERF_CPU,NN_SC_PERF_INFO_Count );
	}
}

void nnScCountUpPerfMeter( u8 num,u8 inf )
{
	nnp_ttblnow[nnp_taskcnt[nnp_wf]-1].stop = OS_CYCLES_TO_USEC(osGetTime());
	if( nnp_taskcnt[nnp_wf] < NN_SC_PERF_NUM ) {
		nnp_ttblnow[nnp_taskcnt[nnp_wf]].task = num;
		nnp_ttblnow[nnp_taskcnt[nnp_wf]].info = inf;
		nnp_ttblnow[nnp_taskcnt[nnp_wf]].start = OS_CYCLES_TO_USEC(osGetTime());
		nnp_taskcnt[nnp_wf]++;
	}
}

void nnScPushPerfMeter( u8 num )
{
	if( nnp_stakptr[nnp_wf] < NN_SC_PERF_NUM ) {
		nnp_staknow[nnp_stakptr[nnp_wf]] = nnp_ttblnow[nnp_taskcnt[nnp_wf]-1].task;
		nnp_stakptr[nnp_wf]++;
		nnScCountUpPerfMeter(num,NN_SC_PERF_INFO_Push);
	} else {
		nnScCountUpPerfMeter(NN_SC_PERF_ERROR,NN_SC_PERF_INFO_Push);
		nnScCountUpPerfMeter(num,NN_SC_PERF_INFO_Push);
	}
}

void nnScPopPerfMeter( void )
{
	if( nnp_stakptr[nnp_wf] > 0 ) {
		nnp_stakptr[nnp_wf]--;
		nnScCountUpPerfMeter(nnp_staknow[nnp_stakptr[nnp_wf]],NN_SC_PERF_INFO_Pop);
	} else {
		nnScCountUpPerfMeter(NN_SC_PERF_ERROR,NN_SC_PERF_INFO_Pop);
	}
}

u16	perf_ypos[] = {
	 1,	// cpu
	 3,	// main
	 5,	// graph -in main
	 7,	// graph -in graph
	 9,	// joy
	11,	// rsp
	13,	// rdp
	15,	// audio
	17,	// check1
	 6,	// check2
	 0,	// nop
	 0,	// err
	17,	// start
	17,	// end
};
u32	perf_color[] = {
	GPACK_RGBA5551(255,255,  0,1) << 16 | GPACK_RGBA5551(255,255,  0,1),	// cpu
	GPACK_RGBA5551(255,  0,255,1) << 16 | GPACK_RGBA5551(255,  0,255,1),	// main
	GPACK_RGBA5551(128,128,255,1) << 16 | GPACK_RGBA5551(128,128,255,1),	// graph -in main
	GPACK_RGBA5551(128,128,128,1) << 16 | GPACK_RGBA5551(128,128,128,1),	// graph -in graph
	GPACK_RGBA5551(  0,255,255,1) << 16 | GPACK_RGBA5551(  0,255,255,1),	// joy
	GPACK_RGBA5551(  0,  0,255,1) << 16 | GPACK_RGBA5551(  0,  0,255,1),	// rsp
	GPACK_RGBA5551(  0,255,  0,1) << 16 | GPACK_RGBA5551(  0,255,  0,1),	// rdp
	GPACK_RGBA5551(255,  0,  0,1) << 16 | GPACK_RGBA5551(255,  0,  0,1),	// audio
	GPACK_RGBA5551(252,171,  0,1) << 16 | GPACK_RGBA5551(252,171,  0,1),	// check1
	GPACK_RGBA5551(255,255,255,1) << 16 | GPACK_RGBA5551(255,255,255,1),	// check2
	GPACK_RGBA5551(  0,  0,  0,1) << 16 | GPACK_RGBA5551(  0,  0,  0,1),	// nop
	GPACK_RGBA5551(  0,  0,  0,1) << 16 | GPACK_RGBA5551(  0,  0,  0,1),	// err
	GPACK_RGBA5551(255,  0,  0,1) << 16 | GPACK_RGBA5551(255,  0,  0,1),	// start
	GPACK_RGBA5551(255,255,255,1) << 16 | GPACK_RGBA5551(255,255,255,1),	// end
};

void nnScWritePerfMeter( Gfx **gptr, u16 x, u16 y )
{
	Gfx		*gp = *gptr;
	int		i,j,n,xx = x+182;
	u32		s,e;

	gDPPipeSync(gp++);
#if defined(NDEBUG)
	return;
#endif

	//---- �y�� ----//
	gDPSetCycleType(gp++, G_CYC_FILL);
	gDPSetRenderMode(gp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
	if(!nnp_over_time) {
		// sitaji
		gDPSetFillColor(gp++, (GPACK_RGBA5551(	 64, 64, 64,1) << 16 | GPACK_RGBA5551(  64, 64, 64,1)));
	} else {
		// sitaji
		nnp_over_time = 0;
		gDPSetFillColor(gp++, (GPACK_RGBA5551(	 255,	 0,	 0,1) << 16 | GPACK_RGBA5551(  255,  0,  0,1)));
	}
	gDPFillRectangle(gp++, x -	10, y + 0, x + 190, y + 30 );

	// line
	gDPSetFillColor(gp++, (GPACK_RGBA5551(255,255,255,1) << 16 | GPACK_RGBA5551(255,255,255,1)));
	gDPFillRectangle(gp++, x - 1,y + 1, x - 1, y + 29 );
	gDPFillRectangle(gp++, x + 180,y + 1, x + 180, y + 29 );

	gDPFillRectangle(gp++, x + 90, y + 19, x + 90, y + 29 );
	for(i = 1;i < 30;i++){
		gDPFillRectangle(gp++, x + i * 3, y + 25, x + i * 3, y + 29 );
	}
	for(i = 31;i < 60;i++){
		gDPFillRectangle(gp++, x + i * 3, y + 25, x + i * 3, y + 29 );
	}

	// 60
	gDPFillRectangle(gp++, xx+ 0, y + 25, xx+ 2, y + 29 );
	gDPFillRectangle(gp++, xx+ 4, y + 25, xx+ 6, y + 29 );
	// sitaji
	gDPSetFillColor(gp++, (GPACK_RGBA5551(	 0,	 0,	 0,1) << 16 | GPACK_RGBA5551(  0,  0,  0,1)));
	// 60
	gDPFillRectangle(gp++, xx+ 1, y + 26, xx+ 2, y + 26 );
	gDPFillRectangle(gp++, xx+ 1, y + 28, xx+ 1, y + 28 );
	gDPFillRectangle(gp++, xx+ 5, y + 26, xx+ 5, y + 28 );

	//---- �y�� ----//

	//---- ���[�^�[ ----//
	for(i=0;i < nnp_taskcnt[nnp_wb];i++) {				// time
		n = nnp_ttblbef[i].task;
		if(n == NN_SC_PERF_NOP) {
			continue;
		}

		s = (nnp_ttblbef[i].start - nnp_ttblbef[0].start) * 180 / 16666;
		if(nnp_ttblbef[i].stop != 0) {
			e = ( nnp_ttblbef[i].stop  - nnp_ttblbef[0].start ) * 180 / 16666;
			if(e == s) {
				e = s + 2;
			}
		}
		else {
			e = s + 1;
		}

		gDPPipeSync(gp++);
		gDPSetFillColor(gp++, perf_color[n] );
		gDPFillRectangle(gp++, x+s, y + perf_ypos[n], x + e, y + 1 + perf_ypos[n] );
	}

#ifdef NN_SC_PERF_2
	// AUDIO
	for(i = 0;i < nnsc_perf_ptr->autask_cnt;i++){
		s = nnsc_perf_ptr->autask_stime[i] * 180/16666;
		if( nnsc_perf_ptr->autask_etime[i] != 0){
			e = nnsc_perf_ptr->autask_etime[i] * 180/16666;
			if( e == s ){
				e = s + 1;
			}
		}else{
			e = s;
		}
		gDPPipeSync(gp++);
		gDPSetFillColor(gp++, perf_color[NN_SC_PERF_AUDIO]);
		gDPFillRectangle(gp++, x+s, y + perf_ypos[NN_SC_PERF_AUDIO], x + e, y + 1 + perf_ypos[NN_SC_PERF_AUDIO]);
	}
	// RSP
	for(i = 0;i < nnsc_perf_ptr->gtask_cnt;i++){
		s = nnsc_perf_ptr->gtask_stime[i] * 180/16666;
		if( nnsc_perf_ptr->rsp_etime[i] != 0){
			e = nnsc_perf_ptr->rsp_etime[i] * 180/16666;
			if( e == s ){
				e = s + 1;
			}
		}else{
			e = s;
		}
		gDPPipeSync(gp++);
		gDPSetFillColor(gp++, perf_color[NN_SC_PERF_RSP] );
		gDPFillRectangle(gp++, x+s, y + perf_ypos[NN_SC_PERF_RSP], x + e, y + 1 + perf_ypos[NN_SC_PERF_RSP]);
	}
	// RDP
	for(i = 0;i < nnsc_perf_ptr->gtask_cnt;i++){
		s = nnsc_perf_ptr->gtask_stime[i] * 180/16666;
		if( nnsc_perf_ptr->rdp_etime[i] != 0){
			e = nnsc_perf_ptr->rdp_etime[i] * 180/16666;
			if( e == s ){
				e = s + 1;
			}
		}else{
			e = s;
		}
		gDPPipeSync(gp++);
		gDPSetFillColor(gp++, perf_color[NN_SC_PERF_RDP] );
		gDPFillRectangle(gp++, x+s, y + perf_ypos[NN_SC_PERF_RDP], x + e, y + 1 + perf_ypos[NN_SC_PERF_RDP] );
	}
#endif
	//---- ���[�^�[ ----//

	*gptr = gp;
}

#endif // NN_SC_PERF
