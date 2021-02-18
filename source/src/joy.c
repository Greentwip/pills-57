
#include <ultra64.h>
#include <PR/ramrom.h>
#include <assert.h>
#include "nnsched.h"
#include "def.h"
#include "joy.h"
#include "message.h"
#include "osHalt.h"
#include "debug.h"

#define CONT_HALT_ON (U_JPAD | D_JPAD)
#define CONT_HALT_OFF (L_JPAD | R_JPAD)

OSContStatus joystt[MAXCONTROLLERS];	 // ���۰׽ð��
OSContPad    joypad[MAXCONTROLLERS];	 //       �ް�
OSContPad   *joyptr[MAXCONTROLLERS];	 //       �ް��߲��
u16			 joynew[MAXCONTROLLERS + 1]; //       �ŐV
u16			 joyold[MAXCONTROLLERS + 1]; //       �O��
u16			 joyupd[MAXCONTROLLERS + 1]; //       �ض�
u16			 joycur[MAXCONTROLLERS + 1]; //       �...���
u16			 joyflg[MAXCONTROLLERS];	 // joycur,joycnt �N���ض�
u16			 joycnt[MAXCONTROLLERS][16]; // sabccccudlrzLR
u16			 joycur1,joycur2;			 // �������ݸ�1,2
int          joynum;					 //       no.(���޸�)
u8			 joygmf[MAXCONTROLLERS];	 // �ްїp�׸�
u16			 joygam[MAXCONTROLLERS];	 // �ްїp

/*----- �n�Ӓǉ��� -----*/
u8			main_joy[MAXCONTROLLERS];	//	���C���̓����������R���g���[���[�ԍ�
u8			link_joy[MAXCONTROLLERS];	//	�ڑ���Ԃ̃t���O
/*----------------------*/

#ifdef STICK_3D_ON
u8			 joystk = FALSE;			 // �ޮ��è���̏\������
#endif

OSMesgQueue 	joyMsgQ;	// ү���޷��
OSMesg      	joyMsgBuf;	//       �ޯ̧

int GameHalt = FALSE;

//////////////////////////////////////////////////////////////////////////////
// ���۰��ް��ǂݍ��ݏ����ݒ�
int joyInit(int maxjoys)
{
	u8          pattern;
	OSMesgQueue serialMsgQ;
	OSMesg      serialMsg;
	int i,j;

	osCreateMesgQueue(&serialMsgQ, &serialMsg, 1);			// ү����Q������
	osSetEventMesg(OS_EVENT_SI, &serialMsgQ, (OSMesg)0);	// ү���޷����SI����ĂƊ֘A�t����( OS_EVENT_SI����Ăɔ������� )
	osContSetCh(MAXCONTROLLERS);
	osContInit(&serialMsgQ, &pattern, &joystt[0]);
	osCreateMesgQueue(&joyMsgQ, &joyMsgBuf, 1);
	osSetEventMesg(OS_EVENT_SI, &joyMsgQ, (OSMesg)0);

	for(i = 0;i < MAXCONTROLLERS + 1;i++){
		joynew[i] = joyold[i] = joyupd[i] = joycur[i] = 0;
	}

	for(i=0;i<MAXCONTROLLERS;i++){
		for(j=0;j<16;j++) joycnt[i][j] = 0;
		joygmf[i] = joygam[i] = joyflg[i] = 0;
	}
	joycur1 = 20;
	joycur2 = 4;

#ifdef JOY_PADCOUNT_ON
	joynum = 0;
	for(i=0;i<MAXCONTROLLERS;i++){
		if((pattern & (1<<i)) && !(joystt[i].errno & CONT_NO_RESPONSE_ERROR)) {
			joyptr[i] = &joypad[i];
			joynum++;
			if(joynum == maxjoys) return joynum;
		}
	}
	return joynum;
#else
	return MAXCONTROLLERS;
#endif
}

//////////////////////////////////////////////////////////////////////////////
// ���۰דǂݍ��ݽگ��,Core
int joyProcCore( void )
{
	int halt = GameHalt, ignore = FALSE;
	u16 i,j,f;
	int jx,jy;
	u16 button;

#ifdef NN_SC_PERF
	nnScPushPerfMeter(NN_SC_PERF_JOY);
#endif

	osContStartReadData(&joyMsgQ);
	(void)osRecvMesg(&joyMsgQ, NULL, OS_MESG_BLOCK);
	osContGetReadData(joypad);

#if defined(USE_HALT)
	// HALT����
	for (i=0; i<MAXCONTROLLERS; i++) {
		// 
#if defined(DEBUG)
		if(joypad[i].stick_y > 40) {
#else
		if((joypad[i].button & CONT_HALT_ON) == CONT_HALT_ON) {
#endif
			halt = TRUE;
			ignore = TRUE;
		}
#if defined(DEBUG)
		else if(joypad[i].stick_y < -40) {
#else
		else if((joypad[i].button & CONT_HALT_OFF) == CONT_HALT_OFF) {
#endif
			halt = FALSE;
			ignore = TRUE;
		}
	}

	// HALT���������������ꍇ�A�p�b�h�̓��͂𖳌��ɂ���B
	if(ignore) {
		for(i = 0; i < MAXCONTROLLERS; i++) {
			joypad[i].button = 0;
			joypad[i].stick_x = 0;
			joypad[i].stick_y = 0;
		}
	}
#endif // USE_HALT

	for (i=0; i<MAXCONTROLLERS; i++) {
		// joynew,joyupd �ݒ�
#ifdef JOY_PADCOUNT_ON
		joynew[i] = joyptr[i]->button;
#else
		joynew[i] = joypad[i].button;
#endif

#ifdef STICK_3D_ON
		if( joystk ) {
			jx=joyptr[i]->stick_x;
			jy=joyptr[i]->stick_y;
			if( jx < 0 ) jx*=-1;
			if( jy < 0 ) jy*=-1;
			if( jy >= jx ) {
				if( ( joyptr[i]->stick_y/8 )>0 ) joynew[i] |= U_JPAD;
				if( ( joyptr[i]->stick_y/8 )<0 ) joynew[i] |= D_JPAD;
			} else {
				if( ( joyptr[i]->stick_x/8 )<0 ) joynew[i] |= L_JPAD;
				if( ( joyptr[i]->stick_x/8 )>0 ) joynew[i] |= R_JPAD;
			}
		}
#endif
		joyupd[i] = ( joynew[i] | joyold[i] )&(~joyold[i]);
		joycur[i] = 0;

		// joycur �ݒ�
		for(j=0,f=0x8000;j<16;j++,f>>=1) {
			if( f & joyflg[i] ) {
				if( joynew[i] & f ) {
					joycnt[i][j]++;
					if(joycnt[i][j] == 1
					||(joycnt[i][j] >= joycur1 && ( joycnt[i][j] - joycur1 ) % joycur2 == 0 ))
					{
						joycur[i] |= f;
					}
				}
				else {
					joycnt[i][j] = 0;
				}
			}
		}

		// �㏈��
		joyold[i] = joynew[i];
	}

	i = 0;
	if(halt) i++;
	if(GameHalt) i++;

	if(i == 1) {
		GameHalt = halt;
		osHaltSet(GameHalt);
	}

#ifdef NN_SC_PERF
	nnScPopPerfMeter();
#endif
}

// �w�肳�ꂽ���ٶ�����Ta...Ta�̑O���l�ɍĐݒ�
void joyCursorFastSet
(
	u16	button,	// �����׸�
	u8	index	// joy???[���޸�]
){
	int	i,j=(u16)button;
	for(i=15;i>=0;i--) {
		if( j & 1 ) break;
		j>>=1;
	}
	joycnt[index][i]=joycur1+joycur2-1;
}

/*---------- �n�Ӓǉ��� ----------*/

/*--------------------------------------
	�R���g���[���[�ڑ��󋵊m�F�֐�
--------------------------------------*/
int	joyResponseCheck(void)
{
	OSContStatus	joyStatus[MAXCONTROLLERS];
	int	max_cont = 0;
	s8	i,j;

	//	�R���g���[���̐ڑ��󋵂̊m�F���߂̔��s
	osContStartQuery(&joyMsgQ);

	//	���b�Z�[�W�҂�
	(void)osRecvMesg(&joyMsgQ, NULL, OS_MESG_BLOCK);

	//	�R���g���[���̐ڑ��󋵂̎擾
	osContGetQuery(joyStatus);

	for(i = 0;i < MAXCONTROLLERS;i++){
		main_joy[i] = 4;
	}
	for(i = j = 0;i < MAXCONTROLLERS;i++)
	{
		link_joy[i] = 0;
		if( joyStatus[i].errno != CONT_NO_RESPONSE_ERROR ){
			//	�R���g���[���[�ڑ�����
			if( (joyStatus[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
			{
				//	�W���R���g���[��������
				link_joy[i] = 1;
				max_cont ++;
				main_joy[j] = i;
				j++;
			}
		}
	}

	return	max_cont;
}

/*---------- �n�Ӓǉ��� ----------*/

