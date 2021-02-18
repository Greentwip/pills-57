#ifndef _EVSWORKS_H_
#define _EVSWORKS_H_

//////////////////////////////////////////////////////////////////////////////
// �ėp�I��`��, ����

#define M_PI		3.14159265358979323846			// ��
#define DegToRad(a)	((float)(a) * M_PI / 180.0)		// �x�����烉�W�A��
#define RadToDeg(a)	((float)(a) / M_PI * 180.0)		// ���W�A������x��

//////////////////////////////////////////////////////////////////////////////
// �ްђ�`���l

#if 0 // �폜
/*
#define STD_MFieldX	   8	// M̨���ޕW�� X����
#define STD_MFieldY	  17	//			   Y����( �����Ȃ���܂� )
#define VS4_MFieldX	   8	// M̨���� 4p X����
#define VS4_MFieldY	  17	//			  Y����( �����Ȃ���܂� )
#define MFITEM_SMALL   8	// M̨���ޱ��ѻ��� S
#define MFITEM_LARGE  10	//		 ..		   L
#define LEVEL_MAX	  20	// �ް�����
#define PLAYER_MAX	   4	// ��ڲ԰��
*/
#endif

#define FRAME_PAR_MSEC  6	// 1/10�b�Ԃ̃t���[����
#define FRAME_PAR_SEC  60	// 1�b�Ԃ̃t���[����
#define SCORE_MAX	   6	// ������޸�
#define MAGA_MAX	 256	// ϶޼��max
#define DEFAULT_HIGH_SCORE 56600 // �f�t�H���g�̃n�C�X�R�A

//////////////////////////////////////////////////////////////////////////////
// �e��ð���֘A

#if 0 // �폜
/*
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
*/
#endif

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
	ONES_8_AB,	//-  ..  B
	ONES_ooBA,	//	 ..  C
	ONES_8_BA,	//	 ..  D
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

#if 0 // �폜
/*
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
*/
#endif

// �ްѐݒ� evs_gamesel
typedef enum {
	GSL_1PLAY,
	GSL_2PLAY,
	GSL_4PLAY,
	GSL_VSCPU,
	GSL_1DEMO,
	GSL_2DEMO,
	GSL_4DEMO,
	GSL_MAX,
} GAME_SELECT;

// �Q�[�����[�h
typedef enum {
	GMD_NORMAL,
	GMD_FLASH,
	GMD_TaiQ,
	GMD_TIME_ATTACK,
} GAME_MODE;

// �v�l�p��׸���ԍ�
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

#if 0 // �폜
/*
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
*/
#endif

//////////////////////////////////////////////////////////////////////////////
// �ėpܰ�� EVS

extern u8 evs_stereo;    // true:��ڵ or false:����
extern u8 evs_seqence;   // ���ݽ on/off
extern u8 evs_seqnumb;   // ���ݽ�ԍ�
extern u8 evs_playmax;   // ��ڲ԰��max( ���۰א� )
extern u8 evs_playcnt;   // ��ڲ԰��
extern u8 evs_keyrept[]; // ����߰�
extern u8 evs_gamespeed; // �Q�[���S�̂̑��x(�ʏ�͂P)
extern u8 evs_score_flag;// �X�R�A�\���t���O

extern GAME_SELECT evs_gamesel;  // ���قł̹ްѾڸ� 0-3 == 1P,2P,4P,vsCPU
extern GAME_MODE   evs_gamemode; // �Q�[�����[�h

/*----- �n�Ӓǉ��� -----*/
extern s8  evs_story_flg;         // �X�g�[���[���[�h���ʃt���O
extern s8  evs_story_no;          // �X�g�[���[���[�h�X�e�[�W�ԍ�
extern s8  evs_story_level;       // �X�g�[���[���[�h�̓�Փx
extern u8  evs_secret_flg[];      // �B��L�����g�p�t���O [0]:vwario, [1]:mmario
extern u8  evs_one_game_flg;      // �X�g�[���[�X�e�[�W�Z���N�g����ON
extern u8  evs_level_21;          // "�G���h���X�Q�P���x�����N���A����" �t���O
extern s8  evs_manual_no;         // ��������ԍ�
extern u8  evs_select_name_no[];  // �I�����ꂽ���O�̔z��ԍ�
extern u32 evs_high_score;        // �ō����_
extern int evs_vs_count;          // �ΐ��
extern u32 evs_game_time;         // �X�g�[���[�p����
extern int evs_default_name[];    // ���O�iNEW�j�̃f�[�^
/*----------------------*/
// �e���{�����ް�

extern u8 FlyingCnt[];          // �ײݸ޶���
extern u8 BonusWait[][3];       // ���ٶ�߾� y=1~3��, �]�ͳ���
extern u8 TouchDownWait[];      // ��߾ى�����ۯ����L�鎞�̒ǉ�����
extern u8 GameSpeed[];          // �ްѽ�߰��
extern u8 FallSpeed[];          // �~����߰��
extern u8 Score1p[][SCORE_MAX]; // ��ٽ���������_�\
extern u8 CapsMagazine[];       // ��߾�϶޼��

// �n�Ӓǉ���
#define DM_MAX_TIME (5999 * FRAME_PAR_SEC) // ���Ԃ̍ő�l( 99��59�b )

// �L�[�Ή��\
#define	DM_KEY_A		0x8000
#define	DM_KEY_B		0x4000
#define DM_KEY_Z		0x2000
#define DM_KEY_START	0x1000
#define	DM_KEY_UP		0x0800
#define	DM_KEY_DOWN		0x0400
#define	DM_KEY_LEFT		0x0200
#define	DM_KEY_RIGHT	0x0100
#define	DM_KEY_L		0x0020
#define	DM_KEY_R		0x0010
#define	DM_KEY_CU		0x0008
#define	DM_KEY_CD		0x0004
#define	DM_KEY_CL		0x0002
#define	DM_KEY_CR		0x0001

#define	DM_ROTATION_R	DM_KEY_A
#define	DM_ROTATION_L	DM_KEY_B
#define	DM_ANY_KEY		0xff3f
#define	DM_KEY_OK		0x9000

// �L�����N�^�[�ԍ�
enum {
	DMC_MARIO,		//  0:�}���I
	DMC_WARIO,		//  1:�����I
	DMC_MAYU,		//  2:�}���s�[
	DMC_YARI,		//  3:�����}��
	DMC_RINGO,		//  4:��񂲂낤
	DMC_FUSEN,		//  5:�t�E�Z���܂���
	DMC_KAERU,		//  6:�܂�J�G��
	DMC_KURAGE,		//  7:�ӂ����炰
	DMC_IKA,		//  8:�C�J�^�R�e���O
	DMC_KUMO,		//  9:�L�O����
	DMC_ROBO,		// 10:�n���}�[���{
	DMC_MAD,		// 11:�}�b�h���V�^�C��
	DMC_NAZO,		// 12:�Ȃ��̂���
	DMC_VWARIO,		// 13:�o���p�C�A�����I
	DMC_MMARIO,		// 14:���^���}���I
};

//////////////////////////////////////////////////////////////////////////////

#endif // _EVSWORKS_H_
