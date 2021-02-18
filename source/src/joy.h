#ifndef _JOY_H_
#define _JOY_H_

#define CONTROLLER_THREAD_PRI  115

extern OSContStatus joystt[MAXCONTROLLERS];	 	// ���۰׽ð��
extern OSContPad    joypad[MAXCONTROLLERS];	 	//       �ް�
extern OSContPad   *joyptr[MAXCONTROLLERS];	 	//       �ް��߲��
extern u16			joynew[MAXCONTROLLERS + 1];	//       �ŐV
extern u16			joyold[MAXCONTROLLERS + 1];	//       �O��
extern u16			joyupd[MAXCONTROLLERS + 1];	//       �ض�
extern u16			joycur[MAXCONTROLLERS + 1];	//       �...���
extern u16			joyflg[MAXCONTROLLERS];	 	// joycur,joycnt �N���ض�
extern u16			joycnt[MAXCONTROLLERS][16];	// sabccccudlrzLR
extern u16			joycur1,joycur2;			// �������ݸ�1,2
extern int			joynum;						//       no.(���޸�)
extern u8			joygmf[MAXCONTROLLERS];	 	// �ްїp�׸�
extern u16			joygam[MAXCONTROLLERS];	 	// �ްїp
#ifdef STICK_3D_ON
extern u8			joystk;			 			// �ޮ��è���̏\������
#endif
extern int GameHalt;

/*----- �n�Ӓǉ��� -----*/
extern	u8			main_joy[MAXCONTROLLERS];	//	���C���̓����������R���g���[���[�ԍ�
extern	u8			link_joy[MAXCONTROLLERS];	//	�ڑ���Ԃ̃t���O
/*----------------------*/


extern int  joyInit(int);				// initialize controller
extern void joyReadStart(void);  		// controller read start
extern int  joyProcCore(void);
extern void joyCursorFastSet(u16,u8);

/*----- �n�Ӓǉ��� -----*/
extern int joyResponseCheck(void);
/*----------------------*/

#endif // _JOY_H_
