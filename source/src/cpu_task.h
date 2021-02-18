
#if !defined(_CPU_TASK_H_)
#define _CPU_TASK_H_

extern int  cpuTask_isCreated();
extern void cpuTask_createThread(void);
extern void cpuTask_destroyThread(void);
extern void cpuTask_sendTask(void (*proc)(void *), void *arg);
extern int  cpuTask_getTaskCount(void);

#endif // _CPU_TASK_H_
