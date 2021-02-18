

#include <ultra64.h>



#include "def.h"

#include "message.h"



#include "main.h"

#include "util.h"

#include "dmacopy.h"

#include "code_entry.h"



u64 bootStack[STACKSIZE/sizeof(u64)];



static void idle(void *);



// �گ�ލ\���̂̒�`

static OSThread idleThread;

static OSThread mainThread;



// �گ�޽����̒�`

static u64 idleThreadStack[STACKSIZE/sizeof(u64)];

static u64 mainThreadStack[STACKSIZE/sizeof(u64)];



// �����̧ݸ

static void defaultIdleFunc() {}

static void (*idleFunc)() = defaultIdleFunc;



// �����Ă��Q��

MSegDecl(boot)

MSegDecl(code)

MSegDecl(code_entry)



//------------------------------------ ���C���X���b�h���~

void stopMainThread()

{

	osStopThread(&mainThread);

}



//------------------------------------ �����̧ݸ���ò

void setIdleFunc(void *func)

{

	idleFunc = (void (*)())func;

}



//------------------------------------ �ް�

void boot(void)

{

	// os�Ƽ�ײ��,

	osInitialize();



	// idle�گ�ލ쐬�ƽ���

	osCreateThread(&idleThread,IDLE_THREAD_ID, idle, (void *)0, (idleThreadStack+STACKSIZE/sizeof(u64)), IDLE_THREAD_PRI);

	osStartThread(&idleThread);

}



//------------------------------------ �ް�2

static void boot2(void *arg)

{

	CODE_ENTRY *entry = (CODE_ENTRY *)_bootSegmentEnd;



	auRomDataRead((u32)_code_entrySegmentRomStart, entry,

		(u32)_code_entrySegmentRomEnd - (u32)_code_entrySegmentRomStart);



	ExpandGZip(_codeSegmentRomStart, entry->start,

		(u32)_codeSegmentRomEnd - (u32)_codeSegmentRomStart);



	bzero(entry->bssStart, (u32)entry->bssEnd - (u32)entry->bssStart);



	entry->entry(arg);

}



//-------------------------------- �����ݸ�

static void idle(void *arg)

{

	// pi�Ȱ�ެ�̋N��

	osCreatePiManager((OSPri)OS_PRIORITY_PIMGR, &PiMessageQ, PiMessages, NUM_PI_MSGS);



#ifdef USE_USB

        {

            int i;

            osUsbInit();

            for (i=0; i<10000000; i++);

        }

#endif



	// Ҳ��ڰт̋N��

	osCreateThread(&mainThread, MAIN_THREAD_ID, boot2, arg,

		(mainThreadStack + STACKSIZE / sizeof(u64)), MAIN_THREAD_PRI);

	osStartThread(&mainThread);



	// �����ݸ�ٰ��( ����ٽگ�ނ���ײ��è���Ⴂ�̂Ŋe�ڰт̏I���ケ��ٰ�߂��g�p���Ă��鹰������� )

	for(;;) {

		idleFunc();

	}

}

