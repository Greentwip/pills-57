#ifndef _EVSWORKS_H_
#define _EVSWORKS_H_
// Search "�����" to jump routine's top( & keypoint ) : present by �����

//������������������������������������������������������������������� �ėp�I��`��, ����
#define NONE		0
#define TOPNUM		0
#define FIRST		1
#define NOT1ST		0
#define WBUFER		2
#define EOD			0xff
#define TAG			0xfe

#define VI_RETRACE	1								// ������ڰ��҂�
#define M_PI		3.14159265358979323846			// ��
#define DegToRad(a)	((float)(a) * M_PI / 180.0)		// �x�����烉�W�A��
#define RadToDeg(a)	((float)(a) / M_PI * 180.0)		// ���W�A������x��
#define SHIF(x)	(1<<x)


//������������������������������������������������������������������� ���۰׊֘A
#define SPRITE_ANTI	D_CBUTTONS		// ���ײı����ر�
#define	R_ROLL		A_BUTTON		// ��߾� R۰�
#define	L_ROLL		B_BUTTON		//	 ..	 L۰�
#define SELECT		A_BUTTON		// �I��	 selection
#define CANSEL		B_BUTTON		// ��ݾ� rejection
#define	LEVER_UP	U_JPAD			// ��
#define	LEVER_DOWN	D_JPAD			// ��
#define	LEVER_LEFT	L_JPAD			// ��
#define	LEVER_RIGHT	R_JPAD			// �E
#define	LEVER_MINUS	L_JPAD			// - (��)
#define	LEVER_PLUS	R_JPAD			// + (�E)

//������������������������������������������������������������������� �����

enum {
	SEQ_Fever,
	SEQ_Chill,
	SEQ_Dizzy,
	SEQ_Opening,
	SEQ_Menu,
	SEQ_VSEnd,
	SEQ_Win,
	SEQ_Lose,
	SEQ_Gingle,
	SEQ_Clear,
	SEQ_Clear2,
	SEQ_MAX,
};
enum {
	SE_gFreeFall,
	SE_gVrsErase,
	SE_gVrsErsAl,
	SE_gSpeedUp,
	SE_gCombo1P,
	SE_gPause,
	SE_gCapErase,
	SE_mDecide,
	SE_mLeftRight,
	SE_mUpDown,
	SE_gLeftRight,
	SE_gTouchDown,
	SE_gCapRoll,
	SE_mCANCEL,
	SE_mDispChg,
	SE_mDataErase,
	SE_mError,
	SE_gCount1,
	SE_gCountEnd,
	SE_gVirusStruggle,
	SE_gCombo2P,
	SE_gCombo3P,
	SE_gCombo4P,
	SE_gCoin,
	SE_gWarning,
	SE_gReach,
	SE_gREraseP11,
	SE_gREraseP12,
	SE_gREraseP13,
	SE_gDamageP1,
	SE_gREraseP21,
	SE_gREraseP22,
	SE_gREraseP23,
	SE_gDamageP2,
	SE_gDamageP22,
	SE_xNoRequest,
};

//������������������������������������������������������������������� �ްђ�`���l

#define LEVEL_MAX	  20	// �ް�����
#define PLAYER_MAX	   4	// ��ڲ԰��
#define MFx			  12	// m̨����MAX x,y
#define MFy			  21	//
#define FALL_CNT	  14	// ��������
#define	PIXEL_MAX	   6	// ��ۯ��߸��
#define ERASE_CNT1	  18	// �ڰ�����
#define ERASE_CNT2	  (13+14)	//	  ..
#define	PAUSE_NOREQ	  77	// ���߰�ޗv��
#define	CMB_THROUGH	   7	// "����"��Ԑ�
#define	SCORE_MAX	   6	// ������޸�
#define	MAGA_MAX	 256	// ϶޼��max
#define	MAG_NO		   3	// ϶޼�ݐ�
#define	ID_MAX		   9	// .id max
#define	IDMAXp	ID_MAX+1	// .id max +1
#define LEVLIMIT	  23	// �������ُ��
#define	STD_MFieldX	   8	// M̨���ޕW�� X����
#define	STD_MFieldY	  17	//			   Y����( �����Ȃ���܂� )
#define	VS4_MFieldX	   8	// M̨���� 4p X����
#define	VS4_MFieldY	  17	//			  Y����( �����Ȃ���܂� )
#define	MFITEM_SMALL   8	// M̨���ޱ��ѻ��� S
#define	MFITEM_LARGE  10	//		 ..		   L
#define ACCEL_DIV	  10	// ��߰�����ٱ������ݸ�( ��̫�� )
#define DELETE_LINE	   4	//
#define PINPOINT_CHG 120	// ����߲�čU���������Я�
#define JOYrTIMEvm 13500	// DrM�ްю����۰דǂ݂Ƃ����ݸ�( vsMEN )
#define JOYrTIMEvc 10500	//				..				( vsCPU )
#define PARAM_NUMx	   8	// ���Ұ��������� x
#define PARAM_NUMy	   8	//		..		  y
#define PARAM_ALPy	   8	// ���Ұ��p������ y
#define PARAML_NUMx	  12	// L���Ұ��������� x
#define PARAML_NUMy	  16	//		 ..		   y

#define KINGBODY_MAX 100		// �ݸ�����ް��߾يi�[����
#define KING_WID	  32		// �ݸ�����ް����w
#define KING_HEI	  32		//		..		 h
#define KING_COL	 256		//	  ..	 ��گĐ�
#define KING_SIZ_xb G_IM_SIZ_8b	//	  ..	 ��گ��ޯĐ�

enum { Xp,Yp,Zp,};			// XYZ�ް����޸�

//������������������������������������������������������������������� (�ڸ�)�ƭ��֘A

// �߰��, ����ƭ�
enum {
	SUB_TA,
	SUB_TATATA,
	SUB_COFFEE,
	SUB_MAX,
};

// �߰�މ���
enum {
	PAUSE_CONT,	// �߰���ƭ��p��
	PAUSE_END,	//	   ..	�I��
	PAUSE_EXIT,	// �ްяI��
};

// ���žڸ�
enum {
	ERS_CAPSULE,
	ERS_VIRUS,
	ERS_MAX,
};

//������������������������������������������������������������������� �e��ð���֘A

// ��۸��ѽð�� evs_prgstat
enum {
	PRG_OPEN,		//-�����̪���
	PRG_READY,		// ��ި
	PRG_GAME,		// �ް�
	PRG_CLEAR,		// �ްѸر�
	PRG_OVER,		// �ްѵ��ް
	PRG_PAUSE,		//-�߰��
	PRG_PAUSE_NF,	// ̫��ż�߰��
	PRG_1P_WIN,		// 1P����
	PRG_2P_WIN,		// 2P����
	PRG_3P_WIN,		// 3P����
	PRG_4P_WIN,		//-4P����
	PRG_DRAW,		// ��۰
	PRG_EXITVS,		// �ΐ�Ӱ�ޏI��
	PRG_JUDGEWAIT,	// ���賴��
	PRG_CLOSE,		// �۰��̪���
};

// �ްѽð�� pw->gs
enum {
	GST_NO_USE,		// ��g�p
	GST_1ST,		// ̧-���ڰ�
	GST_INIT,		// �Ƽ�ײ�ޒ�
	GST_MFIELD,		// m̨���ޕ\��
	GST_GAME,		// �ްѐi�s��
	GST_PAUSE,		// �߰��
	GST_OVER,		// �G���ر���Ĺްѵ��ް
	GST_CLEAR,		// �����̗͂Ÿر�
	GST_TRAIN,		// 4pGAMEOVER���̗��K
	GST_OVPAUSE,	// �ްѵ��ް�҂��߰��
	GST_TRPAUSE,	// ���K�҂��߰��
};

// m̨���޽ð�� pw->mt
enum {
	MF_INIT,	// m̨���޲Ƽ�ײ�ޒ�
	MF_NORMAL,	// �ʏ폈����
	MF_ERASE,	// ��������y�я�����
	MF_FALL,	// ��������y�ї�����
	MF_BOUNCE,	// ���ٶ�߾��޳��ޒ�
};

// �ްѽ�߰�� pw->sp
enum {
	SPEED_LOW,
	SPEED_MID,
	SPEED_HIGH,
	SPEED_MAX,
};

// ��ۯ��ð�� pw->blk[][].st
enum {
	MB_CAPS_U,		//-��߾ُ����
	MB_CAPS_D,		//	..	�� ..
	MB_CAPS_L,		//	..	�� ..
	MB_CAPS_R,		//	..	�E ..
	MB_CAPS_BALL,	//	..	��
	MB_VIRUS_A,		//-��ٽA
	MB_VIRUS_B,		// ��ٽB
	MB_VIRUS_C,		// ��ٽC
	MB_ERASE_CAPS,	// ��߾ُ��Œ�
	MB_ERASE_VIRUS,	//-��ٽ	  ..
	MB_NOTHING,		// �����Ȃ�
};

// ���۰ٶ�߾ٽð�� pw->cap(nex).st
enum {
	ONES_NONE,	//-�\������
	ONES_FLYI,	// ��������ݏ���
	ONES_FLYN,	// ������ꒆ
	ONES_INIT,	// �V�K�\�� = ONES_ooAB
	ONES_ooAB,	// ����� A
	ONES_8_AB,	//-	 ..	 B
	ONES_ooBA,	//	 ..	 C
	ONES_8_BA,	//	 ..	 D
	ONES_STOP,	// ���̏�Œ�~
	ONES_BLOK,	// �d��
};

// �װ (����) pw->blk[][].co, pw->cap(nex).co
enum {
	COL_RED,			// �����Œ�
	COL_YELLOW,			//	  ..
	COL_BLUE,			//	  ..
	COL_NOTHING,
};
#define COL_MAX	 COL_NOTHING

// ��ظ���ۯ������׸� pw->blk[][].ff
enum {
	FALL_OFF,
	FALL_ON,
};

// �����蔻�� use "SHIF(x)"
enum {
	MBLK_UL,	//(U L),( U ),(U R),(URR)
	MBLK_U,		//( L ),( C ),( R ),(R R)
	MBLK_UR,	//(D L),( D ),(D R),(DRR)
	MBLK_URR,
	MBLK_L,
	MBLK_C,
	MBLK_R,
	MBLK_RR,
	MBLK_DL,
	MBLK_D,
	MBLK_DR,
	MBLK_DRR,
};

// pw->pu[x]
enum {
	PU_PLYFLG,
	PU_CPULEV,
	PU_DEMOno,
};

// pw->pu[PU_PLYFLG]
enum {
	PUF_PlayerMAN,
	PUF_PlayerCPU,
	PUF_PlayerDEMO,
};
// pw->pu[PU_CPULEV]
enum {
	PMD_CPU0,
	PMD_CPU1,
	PMD_CPU2,
};

// �ްѐݒ� evs_gamesel
enum {
	GSL_1PLAY,
	GSL_2PLAY,
	GSL_4PLAY,
	GSL_VSCPU,
	GSL_1DEMO,
	GSL_2DEMO,
	GSL_4DEMO,
	GSL_MAX,
//	�n�Ӓǉ���
	GSL_OPTION,
};

// ��׸���ԍ�
enum {
	CHR_MARIO,
	CHR_NOKO,
	CHR_BOMB,
	CHR_PUKU,
	CHR_CHOR,
	CHR_PROP,
	CHR_HANA,
	CHR_TERE,
	CHR_PACK,
	CHR_KAME,
	CHR_KUPP,
	CHR_PEACH,
	CHR_PLAIN1,
	CHR_PLAIN2,
	CHR_PLAIN3,
	CHR_PLAIN4,
};

// ð��ٓ� 0,1,2�Ԃ̈Ӗ�pw->vsp.ac(.fo)
enum {
	BOMB_FORMT,
	BOMB_COUNT,
	BOMB_TABLE,
};
#define BOMBING_MIN 2
#define BOMBING_MAX 4

// ���莞�� pw->vsp.jd
enum {
	JDG_PLAY,
	JDG_CLEAR,
	JDG_OVER,
};

// ���ފĎ��׸�
enum {
	CMBChkStart,
	CMBChkCont,
	CMBChkEnd,
};

// ���n���󋵔��� pw->vsp.jf
#define JDW_ErsCaps		0x0001	// ��߾ق݂̂�ײݏ���
#define JDW_ErsVirus	0x0002	// ��ٽ�܂�ײݏ���
#define JDW_Combo		0x0004	// ����
								//---
#define	JDW_JudgeA		0x0100	// a.�P�F��߾ق̂�
#define	JDW_JudgeB		0x0200	// b.������߾ق̂�(��߾ق݂̘̂A��)
#define	JDW_JudgeC		0x0400	// c.�P�F��ٽ����
#define	JDW_JudgeD		0x0800	// d.������ٽ�܂�(��ٽ�݂̘̂A��)
#define	JDW_JudgeE		0x1000	// e.��ٽ���߾ق̑g�ݍ��킹�ŘA��
								//---
#define JDW_EndJudge	0x8000	// ����I��

// �ݸ�����ް�ړ�,�U���ضް
#define JDW_KingJud	( JDW_JudgeA|JDW_JudgeB|JDW_JudgeC|JDW_JudgeD|JDW_JudgeE )

// �ݸ�����ް�s���ð�� king_status
enum {
	KBom_Start,		// �����ݒ�
	KBom_Init,		//	  ..
	KBom_MoveF,		// ���Ɉړ�( ���� )
	KBom_MoveU,		//	  ..   ( �㏸ )
	KBom_MoveT,		//	  ..   ( �㏸�I�� )
	KBom_MoveD,		//	  ..   ( ���� )
	KBom_MoveX,		//	  ..   ( ���n )
	KBom_Damage,	// ��Ұ�ނ��󂯂�
	KBom_Wait1st,	// �҂�����
	KBom_Wait,		// �҂�
	KBom_Attack,	// �U����
	KBom_Gameover,	// �ްѵ��ް
	KBom_Max,
};


//������������������������������������������������������������������� �\����
// ai work �\����
#define	ROOTCNT		50										// ٰČ���ܰ��ő吔

// ���ٶ�߾ق�u�������Ƃɂ��������
enum {	// af->hei(wid)[][x]: ð��ٗv�f
	LnEraseLin,	// ����ײݐ�
	LnEraseVrs,	// ������ٽ��
	LnRinsetsu,	// �אڐ�( ��ٽ&��߾�&���ٶ�߾� )����������Ԃŕ��񂾐�
	LnLinesAll,	// ���ѐ�( ��ٽ&��߾�&���ٶ�߾�,�����̊�1�Ȃ�ζ��� )����������Ԃŕ��񂾐�(��Ԃ�1�����󂢂Ă�OK)
	LnOnLinVrs,	// ���т̒��̳�ٽ��
	LnLinSpace,	// ײݽ�߰�( 3�ȉ��Ȃ疳��? )�F��4���ׂ��邩�����p
	LnEraseVrsSide,	// ��ٽ���ӏ�����(��Dummy)
	LnHighCaps,	// ������t�߂ɂ����߾ق̐�
	LnHighVrs,	// ������t�߂ɂ��鳨ٽ�̐�
	LnNonCount,	// TRUE�Fɰ����( ex.�Â������̎���ֺ���߲�Čv�Z�����Ȃ��׸�)
	LnTableMax,	//------ MAX
};

// �ړ���߾وړ���ł̏��
typedef struct {
	u8	ok;			// �ړ��\�׸�
	u8	tory;		// �c/���׸�
	u8	x;			// �ړI�nX���W
	u8	y;			// �ړI�nY���W
	u8	rev;		// ��߾ق̏㉺or���E���]�׸�
	u8	ccnt;		// ���F��
					//------- re-make by hiru
	s32	pri;		// �߲�Čv�Z�Z�o�l = �D��x
	s32	dead;		// �߲�Čv�Z�Z�o�p�����ذܰ�
	u8	hei[2][LnTableMax];	// ���ٶ�߾قɍ����cײ݂̏�ԁi�����߾قQ�F�� Ͻ��,�ڰ�ށj
	u8	wid[2][LnTableMax];	// ���ٶ�߾قɍ���鉡ײ݂̏�ԁi�����߾قQ�F�� Ͻ��,�ڰ�ށj
	u8	elin[2];	// ����ײݍ��v��( Ҳ�,��� )
	u8	only[2];	// �㉺���E�ɓ����F���Ȃ����u�P���߾ق̏󋵁v�̐��l������i�����߾قQ�F�� Ͻ����ڰ�ށj
	u8	wonly[2];	// ���E�ɓ����F���Ȃ����u�P���߾ق̏󋵁v�̐��l������i�����߾قQ�F�� Ͻ����ڰ�ށj
	u8	sub;		// Ͻ����߾ق��������Ȃ����A�ڰ�ނ��ʏ�v�Z���邽�߂��׸�
	u8	rensa;		// �A�����邩�׸ށiTRUE�F�A������j
} AI_FLAG;

// �ړ�ٰĕۑ��pܰ�
typedef struct {
	u8	x;
	u8	y;
} AI_ROOT;

typedef struct {
	AI_FLAG	aiFlagDecide;			// �����߾ق̏��
	AI_ROOT	aiRootDecide[ROOTCNT];	// �����߾ق�ٰ�
	u8	aiKeyCount;					// �����߾ق�ٰĂ̶���
	u8	aiSpeedCnt;					// �ړ����]������s�����㎟�̑��삪�ł���܂ł̳���
	u8	aiKRFlag;					// CPU����߰đ����׸ށi�O�F����߰Ē��j
	u8	aiRollCnt;					// �����߾ى�]��������
	u8	aiRollFinal;				// �ړ��I�����ɂP�񂾂���]���삷�邩�ǂ����׸ށiTRUE�F�P��E�ɉ񂷁j
	u8	aiRollHabit;				// �����߾ق���]��������׸ށi0x02bit on:�K���ɉ񂵂Ȃ��痎���A0x01bit on:����]���j
	u8	aiSpUpFlag;					// �����߾ق̗���������s�����ǂ������׸ށiTRUE:�s���j
	u8	aiSpUpStart;				// �����߾ق̗���������s�����ݸނ��ړ����������Ɣ�r���čs�����߂̐��l
	u8	aiSpUpCnt;					// ��߰�ޱ��߂��鶳�āi���g�pܰ��j
	u8	aivl;						// virus level
	u8	aiok;						// ٰČ����v�l�������s��ٰĂ����܂������ǂ����׸ށiTRUE:�����҂��j
	u8	aiRandFlag;					// �߲�Ĕ��莞��������׽���邩�׸ށiFALSE:�S�o�ΐ�łR�l�����b�o�t�̂Ƃ�������������Ȃ��悤�D�揇���߲�Ăɗ����l�𑫂��j
	u8	aiEX;						// �ړI�n�w���W
	u8	aiEY;						// �ړI�n�x���W
	u8	aiOldRollCnt;				// �������߾ى�]��������(�ړI�n�ɍs���Ȃ������`�F�b�N�p)
	u8	aiNum;						// �L�����ʎv�l�p�ėpܰ��P
	u8	aiTimer;					// �L�����ʎv�l�p�ėpܰ��Q
	u8	aiState;					// COM�̏���׸�
	u8	aiSelSpeed;					// COM�̑����߰��Ӱ��
	u8	aiRootP;					// �ړ���߾ق̈ړ������̕��ϒl(�ς݂��������荶�E���߾ق̕ǂɈ͂܂ꂽ���ȂǂɈړ��͈͂������̂Ő��l���������Ȃ�)
	u16	aiPriOfs;					// ����ȃL�����N�^�[�ɗD��x���߲�Ă�����тő������l�i400����800���xCPU�Ɛ���Ă݂Č��߂�j
} AIWORK;

// CPU�̏���׸�
#define	AIF_DAMAGE	0x01	// �U�����󂯂�
#define	AIF_DAMAGE2	0x02	// �A���U�����󂯂�
#define	AIF_AGAPE	0x04	// COM�̑�����X�g�b�v������

// �����߾ُ����󂯎��ܰ�
typedef struct {
	u8			st;			// �i���g�p�j���						:ONES_????
	u8			wc;			// �i���g�p�j��\�����Ķ���( 1>=���� )
	u8			mx,my;		// �ړ��o�H�����O�̑����߾ق̍��W�y�ё��쒆�̍��W�im̨���ޏ�jx,y(8x17)
	u8			ca;			// �����߾ق̶װa(Ͻ��)�ԍ�			:COL_???
	u8			cb;			// �����߾ق̶װb(�ڰ��)�ԍ�
	u8			sp;			// ���݂̗�����߰�ޔԍ��iFallSpeed[�ԍ�]�j
	u8			cn;			// ���݂̗�����߰�ނ̶���(cn == FallSpeed[�ԍ�]�̎������߾ق��P�i������)
	u16			bc;			// �i���g�p�j��ۯ�������
	u8			fc;			// �i���g�p�j�ײݸ޶���
	u8			bd;			// �i���g�p�j��ۯ�������
} AICAPS;

// m̨���ޏ�̶�߾�&��ٽ�̏��ܰ�
typedef struct {
	u8			st;			// ��ۯ��̎��				:CAPS/VIRUS..
	u8			co;			// �װ�ԍ�					:COL_???
//	u8			ff;			// �������ð��
//	u8			rf;			// �a���ۍ쐬�\���׸�by���߰
//	u8			csf;		// �R���{�N�_�t���O	ogura
//	u8			dmy0;		// (dummy)			ogura
//	u16			wk[4];		// ����ܰ�
//	float		fc;			// ���������_����
} AIBLK;


// ��߾�ܰ�
typedef struct {
	u8			st;			// ���						:ONES_????
	u8			wc;			// ��\�����Ķ���( 1>=���� )
	u8			mx,my;		// m̨����x,y(8x15)
	u8			ca;			// �װa�ԍ�					:COL_???
	u8			cb;			//	..b	   ..
	u8			sp;			// FallSpeed[���޸�], �������
	u8			cn;			//	   ..	  ����( next���� CapsMagazine[���޸�] )
	u16			bc;			// ��ۯ�������
	u8			fc;			// �ײݸ޶���
	u8			bd;			// ��ۯ�������
} CONCAP;

// m̨���� 1��ۯ�ܰ�
typedef struct {
	u8			st;			// ��ۯ��̎��				:CAPS/VIRUS..
	u8			co;			// �װ�ԍ�					:COL_???
	u8			ff;			// �������ð��
	u8			rf;			// �a���ۍ쐬�\���׸�by���߰
	u8			csf;		// �R���{�N�_�t���O	ogura
	u8			dmy0;		// (dummy)			ogura
	u16			wk[4];		// ����ܰ�
	float		fc;			// ���������_����
} BLKCAP;

// �O���ް��擾ܰ�( �����ؑ֓��ł���ڲ԰�ݒ�L���ر )
typedef struct {
	u8		mt[2];		// .mt
	u8		vp;			// .vir.vm
	u8		vm;			// .vir.vm
	u8		mp;			// .mp
	u8		cs;			// .cap.st
	u8		vs;			// .vs
	short	rx;			// .rx
} PREVWK;

// �ΐ�ܰ�
typedef struct {
	u16		hi,sc;		// ʲ���, ���
	u8		wi,lo;		// ������, �������׸�
	u8		mcb,mec;	// (max)���޶���, �A��������
	u8		ncb,nec;	// (now)���޶���, �A��������
	u8		si,ei;		// �����J�n����߾�&��ٽ��, ������߾�&��ٽ��
	u8		sv,ev;		// �����J�n����ٽ��, ������ٽ��
	u16		jf;			// ���ٶ�߾ْ��n�������׸�
	u16		jc;			// �ްѵ��ް����( M̨���ނ��Â����� )
	u8		jd;			// ���莞��( JDG_??? )
	u8		fe;			// �����I�����͗����Ɨ����̍���( �ر�͗���Ҳ݂�į�� )
	u8		ee;			// �����I��( �ر�͏���Ҳ݂�į�� )
	u8		an;			// �U������( ����߲�Ď� )
	int		pc;			// �U���p����( ����߲�Ď� )
	u8		aapc;		// �U�����萔 ogura
	u8		aap[3];		// �U������U���t���O(1:�U������,0:���Ȃ�)(��,��,��) ogura
	u8		aapf;		// �U������\���w��t���O(b0:1p,b1:2p,b2:3p,b3,4p���U��)
	u8		ac[18];		// �U����߾ُW�v
	u8		fo[10];		// �픚��߾پ��
} VSPLAY;

// 1��ڲԑ���ܰ�
typedef struct {
	int		aa;			//( �ް��ی�ܰ� )
	u8		pn;			// ��ڲ԰ no.
	u8		gs;			// �ްѽð��
	u8		mt;			// ��ظ��ð�� MB_???
	u8		pe;			// �߸�ِ�
	u16		wx,wy;		// ܰ��ލ�����W(��߾ٗp)
	u16		nx,ny;		// ȸ�� x,y���W(wx,wy+��)
	u8		sx,sy;		// m̨���޻���x,y(8x15)
	u8		lv;			// �ݒ�����(0-29)
	u8		sp;			// �ݒ��߰��				:SPEED_???
	u8		mp;			// M̨���ލ쐬��۸���		:MFP_???
	u8		vs;			// ��ٽ�c��(�����ł͂Ȃ��A����Ă��Ƃ�ϯ���ް��ォ��T��������������)
	u8		id;			// �ݒ�l�������޸�
	u8		dg;			// ���ޯ�޸�د�� on/off
	u8		pu[3];		// 0:TRUE==���߭��, 1:LEVEL( 0,1,2 ), 2:��� no.
	u8		ef;			// ����,�����p����
	CONCAP	cap;		// ���ݑ��삷���߾�
	CONCAP	nex;		// ȸ�Ķ�߾�
	BLKCAP	blk[MFy][MFx];//��ۯ�ܰ�
	PREVWK	prv;		// �O���ް��擾ܰ�
	VSPLAY*	ene[4];		// �G�ΐ�ܰ��߲��
	VSPLAY	vsp;		// �ΐ�pܰ�
	AIWORK	ai;			// COM�v�l�pܰ�
	int		zz;			//( �ް��ی�ܰ� )
} PWORK;

//������������������������������������������������������������������� �ėpܰ��
// EVS
extern u8		evs_fram1st;	// 1st�ڰ�
extern u8		evs_seqence;	// ���ݽ on/off
extern u8		evs_seqnumb;	// ���ݽ�ԍ�
extern u16		evs_seffect[];	// SE no.
extern u16		evs_seSetPtr;
extern u16		evs_seGetPtr;
extern u8		evs_selmenu;	// �ڸ��ƭ�
extern u8		evs_selmen2;	// �ڸ��ƭ�2P
extern u8		evs_playmax;	// ��ڲ԰��max( ���۰א� )
extern u8		evs_playcnt;	// ��ڲ԰��
extern PWORK	evs_playwrk[];	// �ްё�������
extern u8		evs_prgstat;	// �ްѽð��
extern u8		evs_taskbar;	// ����ް
extern u8		evs_bground;	// �w�i
extern u8		evs_pauplay;	// �߰�ޗD����ڲ�
extern short	evs_fadecol;	// �װ̪��ް
extern u8		evs_dsjudge;	// ����`��
extern u8		evs_gamesel;	// ���قł̹ްѾڸ� 0-3 == 1P,2P,4P,vsCPU
extern u8		evs_keyrept[];	// ����߰�
extern int		evs_maincnt;	// Ҳݶ���
extern int		evs_grphcnt;	// ���̨������
extern u8		evs_mainx10;	// main10�������׸�
extern u8		evs_graph10;	// graphic10�����׸�
extern u8		evs_aceldiv;	// �������x���ٱ���( �����ɽ�߰�ޱ��߂��邩 )
extern u8		evs_nextprg;	// �ްяI�����A���
extern u8		evs_atk4pkb;	// �ݸ�����ް�ʒu
extern int		evs_story;		// �İذӰ�ޔԍ�
extern int		evs_4pnum;		// 4p mode MAN�l��
extern u8		evs_trainf;		// 4p mode ���K���[�h�U����:TRUE/ż:FALSE
extern u8		evs_gamespeed;	// �Q�[���S�̂̑��x(�ʏ�͂P)

/*----- �n�Ӓǉ��� -----*/
extern	s8		evs_vs_com_flg;		//	VSCOM���[�h�g�p�\�t���O		�n�Ӓǉ���	99/03/18
extern	s8		evs_seaclet_flg;	//	�B��L�����g�p�t���O			�n�Ӓǉ���	99/03/18
extern	s8		evs_story_flg;		//	�X�g�[���[���[�h���ʃt���O		�n�Ӓǉ���	99/03/30
extern	s8		evs_story_no;		//	�X�g�[���[���[�h�X�e�[�W�ԍ�	�n�Ӓǉ���	99/04/06
extern	u8		evs_sound_flg;		//	�X�e���I�E���m�����p�t���O		�n�Ӓǉ���	99/04/06
extern	s8		evs_story_level;	//	�X�g�[���[���[�h�̓�Փx		�n�Ӓǉ���	99/04/06
extern	s8		evs_vscom_level;	//	VSCOM�̓�Փx					�n�Ӓǉ���	99/04/06
extern	s8		evs_level_flg;		//	���x���Z���N�g�t���O			�n�Ӓǉ���	99/05/07
extern	s8		evs_stage_no;		//	�w�i�ԍ�						�n�Ӓǉ���	99/05/07
extern	s8		evs_manual_no;		//	��������ԍ�					�n�Ӓǉ���	99/07/30
extern	u16		evs_high_score;		//	�ō����_						�n�Ӓǉ���	99/09/09
extern	u32		evs_game_time;		//	�X�g�[���[�p����				�n�Ӓǉ���	99/09/10
/*-----	�e�X�g -----*/
extern	u8		name_test[8][4];	//	���O�f�[�^
extern	u8		name_use_flg[8];	//	���O�g�p�t���O
/*------------------*/

/*----------------------*/


// �e���{�����ް�
typedef struct {
	u16		wx,wy;
	s16		nx,ny;
} PERSON;

extern u8	GameSpeed[];			// �ްѽ�߰��
extern u8	FlyingCnt[];			// �ײݸ޶���
extern u8	BonusWait[][3];			// ���ٶ�߾� y=1~3��, �]�ͳ���
extern u8	TouchDownWait[];		// ��߾ى�����ۯ����L�鎞�̒ǉ�����
extern u8	NextWait[];				// ����߾قւ̳���
extern u8	FallSpeed[];			// �~����߰��
extern u8	Score1p[][SCORE_MAX];	// ��ٽ���������_�\
extern u8	Gameselindex[];			// �ްѾڸĎ���id�ݒ��߲��
extern u8	CapsMagazine[];			// ��߾�϶޼��
extern PERSON	Personalinit[];		// ܰ��ވʒu, ȸ�Ĉʒu
extern u8	GameSize[][6];			// �ްѻ���( �l���ABG�ԍ�, ��߾ٻ���, M̨����x, y )
extern u8	AttackPattern[];		// �U�������

extern u8	king_status;			// �ݸ�����ް���ݽð��
extern u8	king_playnx;			// �ݸ�����ް��ڲ԰�񐶈ʒu next keep
extern u8	king_player;			// �ݸ�����ް��ڲ԰�񐶈ʒu
extern float	tray_posx[];				// ���݂��ڲx�߼޼��( �`��p�Ȃ̂� graphic4p.c �ɂď���������Ă��� )
extern float	tray_posy[];				//	   ..	y	..	(						 ..						)
extern float	tray_next[];				// ����	  ..		(						 ..						)
extern float	tray_move[];				// x���W�ړ��l		(						 ..						)
extern u8	king_count[];					// ���@�\����
extern u8	king_index[];					// KingBomBody[x]
extern u8	KingBomBody[][KINGBODY_MAX];	// ��߾يi�[

extern u8	story_virlv[][11];

// �����ݽ: �ް�[REC]���Ȃ��̂Ȃ牺�L����ı��
//#define PAD_SEQ_KEEP

#define	JOYKEP	3600
#define	STARTJ	255
#define	ENDOFJ	254

typedef struct {
	u8	st,co;
} VIBLOCKm;
typedef struct {
	u8	count;
	u8	joykey;
} KEYSEQ;
typedef struct	tagLVSPD {
	u8	lv;
	u8	sp;
} LVSPD, *PLVSPD;

extern VIBLOCKm	viskeep[ID_MAX][MFy][MFx];	// M̨����
extern u8		magkeep[MAG_NO][MAGA_MAX];	// ϶޼��
extern KEYSEQ	joykeep[ID_MAX][JOYKEP];	// ���۰�
extern LVSPD	levkeep[IDMAXp];			// level,speed

// ���۰�
extern u8		joygmf[];		// ���L�g�p�׸�
extern u16		joygam[];		// �ްїp���۰��׸�
extern u8		keepjoy[];		// ���۰׊֘A
extern u8		counjoy[];
extern u16		idexjoy[];

// �İذӰ�ފ֘A, ܰ�

extern u8		story_flag;

// Ҳݸ�۰���
extern PWORK	*pw;			//( �ق���Ҳ݂ł��g�p��,���� )
extern CONCAP	*caps,*next;
extern BLKCAP	*block;
extern VSPLAY	*vsp;

// ���ޯ�޶���
extern u16	roll_invalid[];
extern u16	coffee_cnt;

// BG������ð��� (Ҳݗp ) :��؊Ǘ�
// BG������ð��� (Ҳݗp ) :��؊Ǘ�
//extern u32	SegBGaddr[][2];

extern	u64		BGBuffer[];

enum {
	BG_1P,
	BG_VS,
	BG_VS4P,
	BG_2PS1,
	BG_2PS2,
	BG_2PS3,
	BG_2PS4,
	BG_2PS5,
	BG_2PS6,
	BG_2PS7,
	BG_2PS8,
	BG_2PS9,
	BG_2PS10,
	BG_2PS11,
};

#define		BGB_SIZE	(256*2+320*240)

enum {
	GSA_KIHON,
	GSA_ATTACK,
	GSA_DAMAGE,
	GSA_WIN,
	GSA_LOSE,
	GSA_DRAW,
	GSA_TOUJOU,
};

//	�n�Ӓǉ���

#define	DAMAGE_MAX	0x10	//	�ő��e��(4P��)
#define	DAMAGE_TYPE			//	4P���̍U�����@�ύX�p�t���O

/*
//	�L�����N�^�[�ԍ�
enum	{
	DMC_MARIO,	//	0:�}���I
	DMC_NOKO,	//	1:�m�R�m�R
	DMC_BOM,	//	2:�{����
	DMC_PUKU,	//	3:�v�N�v�N
	DMC_CHORO,	//	4:�`�����u�[
	DMC_PRO,	//	5:�v���y���w�C�z�[
	DMC_HANA,	//	6:�n�i�����
	DMC_TERE,	//	7:�e���T
	DMC_PAK,	//	8:�p�b�N���t�����[
	DMC_KAME,	//	9:�J���b�N
	DMC_KUPPA,	//	10:�N�b�p
	DMC_PEACH,	//	11:�s�[�`�P
};
*/
//	��{�A�j���[�V�����̔z��ԍ�
enum	{
	ANIME_opening,	//	�o��
	ANIME_nomal,	//	�ʏ�
	ANIME_attack,	//	�U��
	ANIME_damage,	//	��e
	ANIME_win,		//	����
	ANIME_lose,		//	�s�k
	ANIME_draw,		//	��������
};


//	�Q�[�����p�\����
typedef	struct	{
	s8	pos_m_x,pos_m_y;	//	�\�����W(8 x 16)
	s8	capsel_m_g;			//	�J�v�Z���̃O���t�B�b�N�ԍ�
	s8	capsel_m_p;			//	�J�v�Z���̃p���b�g�ԍ�
	s8	capsel_m_flg[6];	//	�J�v�Z���̕\���t���O�Ɨ�������t���O
}game_map;

//	�����J�v�Z���p�\����
typedef	struct	{
	s8	pos_x[2],pos_y[2];	//	�\�����W(8 x 16)
	s8	capsel_g[2];		//	�J�v�Z���̃O���t�B�b�N�ԍ�
	s8	capsel_p[2];		//	�J�v�Z���̃p���b�g�ԍ�
	s8	capsel_flg[4];		//	�J�v�Z���̕\���t���O�Ɨ�������t���O
}game_cap;

//	�U���J�v�Z���p�\����
typedef	struct	{
	s8	pos_a_x,pos_a_y;	//	�\�����W(8 x 16)
	s8	capsel_a_p;			//	�J�v�Z���̃p���b�g�ԍ�
	s8	capsel_a_flg[3];	//	�J�v�Z���̕\���t���O�Ɨ�������t���O
}game_a_cap;

//	�L�����N�^( �}���I�E�m�R�m�R�� )�̃f�[�^�\����
typedef	struct{
	u16	*anime_pal;			//	�p���b�g�|�C���^
	u8	*anime_dat;			//	�O���t�B�b�N�|�C���^(�L�����N�^���Q��ɕ����ĕ`������)
	s8	pos_an_x,pos_an_y;	//	���W
	u8	def_w_size;			//	����
	u8	def_h_size;			//	�c��
	u8	h_size[3];			//	0:�����P 1:�����Q 2:�����R
	u8	aniem_wait;			//	�\������
	s8	aniem_flg;			//	�t���O
}game_anime_data;

//	�L�����N�^( �}���I�E�m�R�m�R�� )�̐���p�\����
typedef	struct{
	u8	cnt_charcter_no;		//	�L�����N�^�[�ԍ�(��L�� DMC_???)
	u8	cnt_now_frame;			//	���݃A�j���[�V�����z��̉��Ԗڂ��w���Ă��邩
	u8	cnt_anime_count;		//	���̃A�j���܂ł̃J�E���^�[
	u8	cnt_anime_loop_count;	//	���[�v�p�J�E���^�[
	s8	cnt_anime_flg;			//	�t���O
	u32	cnt_anime_address;		//	�L�����N�^�O���t�B�b�N�̓Ǎ��݃A�h���X
	game_anime_data	**anime_charcter;	//	�P�L�����̑S�A�j���[�V�����f�[�^�̔z����w��
	game_anime_data	*cnt_now_type;	//	���݂ǂ̃A�j���[�V���������Ă��邩
}game_anime;

//	�Q�[���̐���p�\����
typedef	struct	{
	u16	game_score;			//	���_
	s16	map_x,map_y;		//	�}�b�v�̍��W
	s8	map_item_size;		//	�}�b�v�̃A�C�e���̑傫��
	u8	game_mode[2];		//	0:���������ԍ� 1:���������ԍ��ۑ��ϐ�
	s8	game_condition[3];	//	0:���̎��̏�� 2:���̎��̏�ԕۑ��ϐ� 3:�ω����Ȃ��󋵕ϐ�(�g���[�j���O�����ƂO�ȊO)
	u8	virus_number;		//	�E�C���X��
	u8	virus_level;		//	�E�C���X���x��
	u8	virus_anime;		//	�E�C���X�A�j���[�V�����ԍ�
	s8	virus_anime_vec;	//	�E�C���X�A�j���[�V�����i�s����
	u8	virus_anime_count;	//	�E�C���X�A�j���[�V�����J�E���^
	u8	virus_anime_max;	//	�E�C���X�A�j���[�V�����̍ő�R�}��
	u8	virus_anime_spead;	//	�E�C���X�A�j���[�V�������x
	u8	cap_def_speed;		//	�J�v�Z�����x(SPEED_?(LOW/MID/HIGH/MAX))
	u8	cap_speed;			//	�J�v�Z�����x
	u8	cap_count;			//	�J�v�Z��������
	u8	cap_speed_count;	//	�J�v�Z�������p�J�E���^
	u8	cap_speed_vec;		//	�J�v�Z�������p�J�E���^�����l
	u8	cap_speed_max;		//	�J�v�Z���������x(�J�E���^�����̐��l�ȏ�ɂȂ�ƂP�i����)
	u8	cap_magazine_cnt;	//	�J�v�Z���}�K�W���Q�Ɨp�ϐ�
	u8	cap_magazine_save;	//	�J�v�Z���}�K�W���Q�Ɨp�ۑ��ϐ�
	s8	cap_move_se_flg;	//	�J�v�Z�����E�ړ����̂r�d��炷���߂̃t���O
	u8	erase_anime;		//	���ŃA�j���[�V�����R�}��
	u8	erase_anime_count;	//	���ŃA�j���[�V�����J�E���^	//	�������̃J�E���^�����˂�
	u8	erase_virus_count;	//	���ŃE�C���X��
	u8	chain_count;		//	�A����
	u8	chain_line;			//	���ŗ�
	u8	chain_color[4];		//	0: �� 1:�� 2:�� �������F���J�E���g����B 3: �A���J�n���ɏ������F�̃r�b�g�𗧂Ă� 0x01:�� 0x02:�� 0x04:�� �E�C���X�܂ޏꍇ 0x80 �̃r�b�g�𗧂Ă�
	u8	warning_flg;		//	�x�����Ƃ���炷���߂̃t���O (�r�b�g�Ő��䂷��) 0x08:�E�C���X���X�g�R 0x80 �ςݏグ����
	u8	work_flg;			//	�ėp���t���O
	u8	retire_flg[3];		//	���^�C�A�t���O
	u8	player_state[3];	//	0:TRUE == �R���s���[�^ 1:LEVEL(0.1.2); 2:�v���C���[�ԍ�
#ifdef	DAMAGE_TYPE
	u16	cap_attack_work[DAMAGE_MAX];	//	�����ɗ�������U���J�v�Z���̃��[�N(2bit�Ő���,00:��,01:��,10:��,11:����)
#endif
#ifndef	DAMAGE_TYPE
	u8	cap_attack_work[DAMAGE_MAX][6];	//	�����ɗ�������U���J�v�Z���̃��[�N
#endif
	game_anime	anime;		//	�A�j���[�V��������
	game_cap	now_cap;	//	���ݑ��삷��J�v�Z��
	game_cap	next_cap;	//	���̃J�v�Z��
	AIWORK	ai;			// COM�v�l�pܰ�
	u8		pn;			// ��ڲ԰ no.
	u8		gs;			// �ްѽð��
	u8		lv;			// �ݒ�����(0-29)
	u8		vs;			// ��ٽ�c��(�����ł͂Ȃ��A����Ă��Ƃ�ϯ���ް��ォ��T��������������)
	u8		pu[3];		// 0:TRUE==���߭��, 1:LEVEL( 0,1,2 ), 2:��� no.
	AIBLK	blk[STD_MFieldY+1][STD_MFieldX];//��ۯ�ܰ�
	AICAPS	cap;

}game_state;

//	�Q�[���ݒ�ۑ��p�\����
typedef	struct{
	u8	virus_level;		//	�E�C���X���x��
	u8	cap_def_speed;		//	�J�v�Z�����x(SPEED_?(LOW/MID/HIGH/MAX))
	u8	player_state[2];	//	0:TRUE == �R���s���[�^ 1:LEVEL(0.1.2);
	u8	cnt_charcter_no;	//	�L�����N�^�[�ԍ�(��L�� DMC_???)
}game_state_sub;

//	�Q�[���ݒ�ۑ��p�\����
typedef	struct{
	game_state_sub	play_1p_mode;
	game_state_sub	play_vs_com_mode;
	game_state_sub	play_2p_mode[2];
	game_state_sub	play_4p_mode[4];
}game_state_old;


//	�E�C���X�z�u�p�\����
typedef	struct	{
	s8	virus_type;			//	�E�C���X�̎��(�F�����˂�)
	u8	x_pos,y_pos;		//	�w�E�x���W
}virus_map;

extern	game_state_old	evs_state_old;			//	�e�ݒ�̕ۑ��p�ϐ�
extern	game_state	game_state_data[4];			//	�e�v���C���[�̏�ԋy�ѐ���
extern	game_map	game_map_data[4][8 << 4];	//	�r�̒��̏��
extern	virus_map	virus_map_data[4][8 << 4];	//	�E�C���X�ݒ�p�z��


extern	u8	dm_mode_select_back_bm0_0[];			//	���[�h�Z���N�g�p�w�i�O���t�B�b�N�f�[�^
extern	u16	dm_mode_select_back_bm0_0tlut[];		//	���[�h�Z���N�g�p�w�i�p���b�g�f�[�^


#define	DM_DEBUG_FLG	//	�f�o�b�N�@�\�p�t���O

#ifdef	DM_DEBUG_FLG

#define	NUM_RIGHT	0x80
#define	NUM_ZERO	0x40

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	s8	*str;
}STRTBL;

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	u8	flg;
	s16	num;
}NUMTBL;

typedef	struct{
	s16	x_pos,y_pos;
	u16	color;
	s16	max;
	s16	min;
	s16	*data;
}PARAMTBL;

extern		s16		num_keta[4];
extern		s16		param_pos[4];
extern		void	disp_font_load(void);
extern		void	disp_debug_font(s16	x_pos,s16 y_pos,s8 font);
extern		void	disp_debug_string(STRTBL *str);
extern		void	disp_debug_num(NUMTBL *num);
extern		void	disp_debug_param(u8 player_no,PARAMTBL *param, u8 ctrlno);
extern		void	cnt_debug_param( s8 player_no,PARAMTBL *param );
extern		void	cnt_debug_main(s8 player_no);
extern		void	disp_debug_main_1p(void);


#endif
// ON == �e�����ޯ�ޑ���,�\��
#define DEBUG_ButtonAct_on

#endif
