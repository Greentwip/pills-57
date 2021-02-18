#if !defined(_AIDEF_H_)
#define _AIDEF_H_

//////////////////////////////////////////////////////////////////////////////
// �}�N��

#define LEVLIMIT 23 // �������ُ��
#define MFieldX   8 // M̨���� X����
#define MFieldY  17 //         Y����( �����Ȃ���܂� )

//////////////////////////////////////////////////////////////////////////////
// ai work �\����

#define	ROOTCNT		50
enum {	// af->hei(wid)[][x]: ð��ٗv�f
	LnEraseLin,	// ����ײݐ�
	LnEraseVrs,	// ������ٽ��
	LnRinsetsu,	// �אڐ�( ��ٽ&��߾�&���ٶ�߾� )
	LnLinesAll,	// ���ѐ�( ��ٽ&��߾�&���ٶ�߾�,�����̊�1�Ȃ�ζ��� )
	LnOnLinVrs,	// ���т̒��̳�ٽ��
	LnLinSpace,	// ײݽ�߰�( 3�ȉ��Ȃ疳��? )
	LnEraseVrsSide,	// ��ٽ���ӏ�����(��Dummy)
	LnHighCaps,	// ������t�߂ɂ����߾ق̐�
	LnHighVrs,	// ������t�߂ɂ��鳨ٽ�̐�
	LnNonCount,	// ɰ����( ex.�Â������̎���ֺ )
	LnTableMax,	//------ MAX
};
typedef struct {
	u8	ok;			// �ړ��\�׸�
	u8	tory;		// �c/���׸�
	u8	x;			// X���W
	u8	y;			// Y���W
	u8	rev;		// ���]�׸�
	u8	ccnt;		// ���F��
					//------- re-make by hiru
	s32	pri;		// �D��x( �߲�Čv�Z )
	s32	dead;		// �댯�x(	  ..	 )
	u8	hei[2][LnTableMax];	// �cײ݌v��( Ͻ����ڰ�� )
	u8	wid[2][LnTableMax];	// ��  ..	(	  ..	 )
	u8	elin[2];	// ����ײݍ��v��( Ҳ�,��� )
	u8	only[2];	// Ͻ����ڰ�ޒP���׸�
	u8	wonly[2];	// Ͻ����ڰ�ޒP���׸�(��ײݐ�p)
	u8	sub;		// �ڰ�ޑ��� Ҳ�? ���?
	u8	rensa;		// �A�����邩�׸�
} AI_FLAG;

typedef struct {
	u8	x;
	u8	y;
} AI_ROOT;

typedef struct {
	AI_FLAG	aiFlagDecide;			//
	AI_ROOT	aiRootDecide[ROOTCNT];	// ������߾ق�ٰ�
	u8	aiKeyCount;					//
	u8	aiSpeedCnt;					//
	u8	aiKRFlag;					// CPU������ύX������
	u8	aiRollCnt;					// �����߾ى�]��������
	u8	aiRollFinal;				// �Ō�ɉ�]���삷�邩�ǂ����׸�
	u8	aiRollHabit;				// �����߾ق���]��������׸�
	u8	aiSpUpFlag;					// ��߰�ޱ��߂��邩�׸�
	u8	aiSpUpStart;				// ��߰�ޱ��߂�����Ĉʒu
//	u8	aiSpUpCnt;					// ��߰�ޱ��߂��鶳��
	u8	aivl;						// virus level
	u8	aiok;						// ٰČ������s�������ǂ����׸�
	u8	aiRandFlag;					// �߲�Ĕ��莞��������׽���邩�׸�
	u8	aiEX;						// �ړI�n�w���W
	u8	aiEY;						// �ړI�n�x���W
	u8	aiOldRollCnt;				// �������߾ى�]��������(�ړI�n�ɍs���Ȃ������`�F�b�N�p)
//	u8	aiNum;						// �L�����ʎv�l�p�ԍ�
//	u8	aiTimer;					// �L�����ʎv�l�p�^�C�}�[

	#define NUM_AI_EFFECT 16
	u8	aiEffectNo[NUM_AI_EFFECT];// ���s���̓��ꏈ���ԍ�
	s16	aiEffectParam[NUM_AI_EFFECT];// ���s���̓��ꏈ���̃p�����[�^
	s16	aiEffectCount[NUM_AI_EFFECT];// ���s���̓��ꏈ������J�E���^

	u8	aiState;					// COM�̏���׸�
		#define	AIF_DAMAGE	0x01
		#define	AIF_DAMAGE2	0x02
		#define	AIF_AGAPE	0x04

	u8	aiSelSpeed;					// COM�̑���X�s�[�h
	u8	aiRootP;					// �ړ������䗦(�ړ��͈͂������Ȃ��Ă��邱�Ƃ��`�F�b�N)
	u16	aiPriOfs;					// ����x
} AIWORK;

typedef struct {
	u8			mx,my;		// m̨����x,y(8x15)
	u8			ca;			// �װa�ԍ�					:COL_?(RED/YELLOW/...)
	u8			cb;			//	..b	   ..
	u8			sp;			// FallSpeed[���޸�], �������
	u8			cn;			//	   ..	  ����( next���� CapsMagazine[���޸�] )
} AICAPS;			// ���ޏ��

typedef struct {
	u8			st;			// ��ۯ��̎��				:MB_?(CAPS/VIRUS/...)
	u8			co;			// �װ�ԍ�					:COL_?(RED/YELLOW/...)
} AIBLK;

//////////////////////////////////////////////////////////////////////////////

#endif // _AIDEF_H_
