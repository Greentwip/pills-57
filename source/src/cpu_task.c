
#include <ultra64.h>
#include "def.h"
#include "cpu_task.h"

//////////////////////////////////////////////////////////////////////////////
// 臨時スレッド

// タスク
typedef struct SCpuTask {
	void (*proc)(void *);
	void  *arg;
} SCpuTask;

#define THREAD_STACK_SIZE 0x1000
#define THREAD_PRI        (IDLE_THREAD_PRI + 1)
#define THREAD_ID         6
#define THREAD_MESG_SIZE  9
#define THREAD_TASK_SIZE  (THREAD_MESG_SIZE + 1)

typedef struct SCpuTaskThread {
	u64         stack[THREAD_STACK_SIZE / sizeof(u64)];
	OSThread    thread;
	OSMesgQueue mesgQ;
	OSMesg      mesg[THREAD_MESG_SIZE];
	SCpuTask    task[THREAD_TASK_SIZE];
	int         taskIndex;
	int         taskCount;
} SCpuTaskThread;

SCpuTaskThread cpuTaskThread;
static int _created = 0;

// スレッドのメイン
static void threadEntry(void *arg)
{
	SCpuTaskThread *st = &cpuTaskThread;
	SCpuTask *task;

	while(1) {
		osRecvMesg(&st->mesgQ, (OSMesg *)&task, OS_MESG_BLOCK);
		task->proc(task->arg);
		st->taskCount--;
	}
}

// スレッドを作成済みか
int cpuTask_isCreated()
{
	return _created;
}

// スレッドを作成
void cpuTask_createThread(void)
{
	SCpuTaskThread *st = &cpuTaskThread;

	st->taskIndex = 0;
	st->taskCount = 0;

	osCreateMesgQueue(&st->mesgQ, st->mesg, THREAD_MESG_SIZE);

	osCreateThread(
		&st->thread,
		THREAD_ID,
		threadEntry,
		NULL,
		(u8 *)st->stack + THREAD_STACK_SIZE,
		THREAD_PRI);

	osStartThread(&st->thread);

	_created = 1;
}

// スレッドを破棄
void cpuTask_destroyThread(void)
{
	SCpuTaskThread *st = &cpuTaskThread;
	osDestroyThread(&st->thread);
	_created = 0;
}

// タスクを登録
void cpuTask_sendTask(void (*proc)(void *), void *arg)
{
	SCpuTaskThread *st = &cpuTaskThread;
	SCpuTask *task = &st->task[st->taskIndex];

	st->taskIndex = (st->taskIndex + 1) % THREAD_TASK_SIZE;
	st->taskCount++;

	task->proc = proc;
	task->arg = arg;

	osSendMesg(&st->mesgQ, (OSMesg)task, OS_MESG_BLOCK);
}

// タスクの残量を取得
int cpuTask_getTaskCount(void)
{
	SCpuTaskThread *st = &cpuTaskThread;
	return st->taskCount;
}

