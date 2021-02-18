//����� word search
/**********************************************************

	ai	of	dr.mario	with	vs.	cpu

**********************************************************/
#include <ultra64.h>
#include "evsworks.h"
#include "aiset.h"
#include "aidata.h"
#include "nnsched.h"
#include "vr_init.h"
#include "dm_game_main.h"

/***********************************************
	define
***********************************************/

#define	FIELDX		8				// ̨���޻��ށi��̂P���C���͏�ɋ󔒁j
#define	FIELDY		16+1
#define	WALL		0xff			// ٰČ����p�ް�
#define	WALK		0x0f
//#define	GOALX		4			// ��߾ٽ��Ĉʒu�i���݂͉ς̂���ܰ��ɕύX�j
//#define	GOALY		1

#define	BADLINE		4

// �c or ���ð��
enum {
	tate,
	yoko
};
// ��ޗL���x
enum {
	inval,	// ����
	val,	// �L��
	non,	// �e���Ȃ�
};
// Ҳ݌��o
enum {
	left,	// ��
	right,	// �E
	both	// ����
};

// CPU �����쑬�x
#define	ROLL_SPEED		5	// �J�v�Z����]���̃X�s�[�h
#define	VIRUSSPEEDMAX	1	// �e�푀��̍ő����̃X�s�[�h
//#define	DOWN_SPEED		3
//#define	DEFAULT_SPEED	30

// virus ������Ӱ��
enum {
	SPD_SUPERSLOW,			// �ƂĂ��x���i���ݖ��g�p�j
	SPD_VERYSLOW,			// �ƂĂ��x��
	SPD_SLOW,				// �x��
	SPD_FAST,				// ����
	SPD_VERYFAST,			// �ƂĂ�����
	SPD_MAX,				// �ō���
	SPD_FLASH,				// ɰ���Ĉړ�
	SPD_VERYFASTNW,			// �ƂĂ������i�������S�O�����j
};

u8	aiVirusLevel[][3] =	  { { 30, 24, 18 },		// EASY,NORMAL,HARD
							{ 30, 24, 18 },
							{ 24, 19, 14 },
							{ 18, 14, 10 },
							{ 14, 11,  8 },
							{  6,  6,  6 },
							{  1,  1,  1 },
							{  6,  6,  6 }, };
u8	aiDownSpeed[][3] =	  { {  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  7,  4,  4 },
							{  4,  3,  2 },
							{  1,  1,  1 },
							{  7,  4,  4 }, };
u8	aiSlideFSpeed[][3] =  { { 30, 24, 18 },
							{ 30, 24, 18 },
							{ 25, 20, 16 },
							{ 21, 17, 13 },
							{ 17, 14, 11 },
							{ 10,  6,  4 },
							{  1,  1,  1 },
							{ 17, 14, 11 }, };
u8	aiSlideSpeed[][3] =	  { { 30, 22, 14 },
							{ 30, 22, 14 },
							{ 15, 12, 10 },
							{  8,  7,  6 },
							{  5,  5,  5 },
							{  5,  3,  2 },
							{  1,  1,  1 },
							{  5,  5,  5 }, };

typedef struct {
	u8	ca;			// ��������߾ِF
	u8	cb;			//	�V �E�@�V
} AI_NEXT;
AI_NEXT		aiNext;		// ����߾ِF

typedef struct {
	u8	co;			// �F���
	u8	st;			// �ð�����
} AI_FIELD;
AI_FIELD	aiFieldData[FIELDY][FIELDX];		// ̨���ޏ��
AI_FIELD	aiRecurData[FIELDY+1][FIELDX+2];	// ̨���ޏ��(�ċA�����p)

typedef struct {
	u8	tory;		// �c/���׸�
	u8	x;			// X���W
	u8	y;			// Y���W
} AI_EDGE;			// ���ޏ��

u8	aiTEdgeCnt;		// �c�u���\���ސ�
u8	aiYEdgeCnt;		// ���u��	 �V

AI_FLAG	aiFlag[100];	// ��߾ق�u����ʒu��ٰď��
u8	aiFlagCnt;			// ��߾ق�u����ʒu��
u8	success;			// �o�Hok�׸�
u8	decide;				// �ŏI�I���肷���׸��ް��ԍ�

AI_ROOT	aiRoot[ROOTCNT];	// ������߾ق�ٰ�
u8	aiRootCnt;				// ٰĂ̶���
u8	aiRollFinal;			// �Ō�ɉ�]���삷�邩�ǂ����׸�

u8	aiUsedFlag;				// 1int���ɂP�񂵂��v�l�������g���Ȃ��悤�ɂ����׸�

u8	aipn;					// player �ԍ�

u8	aiDebugP1 = 0;			// ���ޯ��Ӱ�ގ��̂P�o�̂b�n�l�ԍ�

float	aiRootP;			// �ړ������䗦(�ړ��͈͂������Ȃ��Ă��邱�Ƃ��x������)

s16	aiHiEraseCtr;			// �A���������ɏ�̕������ꂽ������
s16	aiHiErR,aiHiErB,aiHiErY;

u16	GOALX,GOALY;			// �����J�n���W(�ʏ��4,1)

u8	aiWall;					// ���E�̕ǂ����o

u8	aiMoveSF;				// �������x�������Ȃ��������E�ړ��X�e�b�v�𐧌�����

s8	aiSelSpeed;				// �b�n�l�̑���X�s�[�h���x�ԍ�
u8	aiSelCom;				// �b�n�l�̎v�l�ԍ�
s8	aiSelSpeedRensa;		// �b�n�l�̑���X�s�[�h���x�ԍ��i�A���o���鎞������߰�ނ�ς������Ƃ��g�p�j

u16	aiPriOfs;				// �b�n�l�̉��肳�i�D�揇�ʂ�����тő����j

extern	AI_DATA	ai_param[4][8];			// �v�l�p�����[�^�f�[�^���[�N�i�p�����[�^�f�[�^���f�o�b�O���[�h�ŉςɂ��邽�ߌ��f�[�^����]�����Ďg���j
extern	AI_DATA	ai_param_org[4][8];		// �v�l�p�����[�^�f�[�^��

enum {
	AGST_GAME,		// �Q�[����
	AGST_TRAIN,		// ���K��
};

//*** �n��ܰ����v�l�pܰ��ɕϊ� ************************
//	aifMakeWork
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	uupw			��ڲ԰�\����
//		AICAPS		uupw->cap		AI�p�����߾ُ����󂯎��ܰ�
//
void aifMakeWork(game_state *uupw)
{
	uupw->cap.mx = uupw->now_cap.pos_x[0];
	uupw->cap.my = uupw->now_cap.pos_y[0];
	uupw->cap.cn = uupw->cap_speed_count;
	uupw->cap.sp = uupw->cap_speed;
	uupw->cap.ca = uupw->now_cap.capsel_p[0];
	uupw->cap.cb = uupw->now_cap.capsel_p[1];

	uupw->vs = uupw->virus_number;
	uupw->lv = uupw->virus_level;
	uupw->gs = (uupw->game_condition[0] == dm_cnd_training)?AGST_TRAIN:AGST_GAME;
	uupw->pn = uupw->player_state[2];

	if ( uupw->pn == 0 ) game_state_data[0].player_state[1] = game_state_data[1].player_state[1];

	uupw->pu[PU_CPULEV] = uupw->player_state[1];

}

//*** �n��̨����ܰ����v�l�p̨����ܰ��ɕϊ� ************************
//	aifMakeBlkWork
//in:	game_map	*game_map_data	��ڲ԰ϯ��̨����
//out:	AIBLK		uupw->blk		m̨���ޏ�̶�߾�&��ٽ�̏��ܰ�
//
void aifMakeBlkWork(game_state *uupw)
{
	u8	capsGCnv[] = {
		MB_CAPS_U,		// capsel_u,		//	�J�v�Z��	��
		MB_CAPS_D,		// capsel_d,		//				��
		MB_CAPS_L,		// capsel_l,		//				��
		MB_CAPS_R,		// capsel_r,		//				�E
		MB_CAPS_BALL,	// capsel_b,		//				�{�[��
		MB_ERASE_CAPS,	// erase_cap_a,	//				���łP
		MB_ERASE_CAPS,	// erase_cap_b,	//				���łQ
		MB_VIRUS_A,		// virus_a1,		//	�E�C���X	a1
		MB_VIRUS_A,		// virus_a2,		//				a2
		MB_VIRUS_B,		// virus_b1,		//				b1
		MB_VIRUS_B,		// virus_b2,		//				b2
		MB_VIRUS_C,		// virus_c1,		//				c1
		MB_VIRUS_C,		// virus_c2,		//				c2
		MB_ERASE_VIRUS,	// erase_virus_a,	//				���łP
		MB_ERASE_VIRUS,	// erase_virus_b,	//				���łQ
		MB_NOTHING,		// no_item,		//	��������
		MB_VIRUS_A,		// virus_a3,		//				a3
		MB_VIRUS_A,		// virus_a4,		//				a4
		MB_VIRUS_B,		// virus_b3,		//				b3
		MB_VIRUS_B,		// virus_b4,		//				b4
		MB_VIRUS_C,		// virus_c3,		//				c3
		MB_VIRUS_C,		// virus_c4		//				c4
	};

	u8	capsCCnv[] = {
		COL_RED,	// capsel_red,			//	�J�v�Z��	��
		COL_YELLOW,	// capsel_yellow,		//				��
		COL_BLUE,	// capsel_blue,		//				��
		COL_RED,	// capsel_b_red,		//			�Â���
		COL_YELLOW,	// capsel_b_yellow,	//			�Â���
		COL_BLUE,	// capsel_b_blue,		//			�Â���
	};

	int		x,y;

#if 1
	for ( x = 0;x < FIELDX;x++) {
		uupw->blk[0][x].st = MB_NOTHING;
		uupw->blk[0][x].co = COL_NOTHING;
	}
	for ( y = 1;y < FIELDY;y++) {
		for ( x = 0;x < FIELDX;x++) {
			if ( game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_flg[0] ) {
				uupw->blk[y][x].st = capsGCnv[game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_g];
				uupw->blk[y][x].co = capsCCnv[game_map_data[uupw->pn][((y-1)<<3) + x].capsel_m_p];
			} else {
				uupw->blk[y][x].st = MB_NOTHING;
				uupw->blk[y][x].co = COL_NOTHING;
			}
		}
	}
#else
	for ( y = 0;y < FIELDY;y++) {
		for ( x = 0;x < FIELDX;x++) {
			if ( game_map_data[uupw->pn][(y<<3) + x].capsel_m_flg[0] ) {
				uupw->blk[y][x].st = capsGCnv[game_map_data[uupw->pn][(y<<3) + x].capsel_m_g];
				uupw->blk[y][x].co = capsCCnv[game_map_data[uupw->pn][(y<<3) + x].capsel_m_p];
			} else {
				uupw->blk[y][x].st = MB_NOTHING;
				uupw->blk[y][x].co = COL_NOTHING;
			}
		}
	}
#endif

	if ( uupw->now_cap.pos_y[0] ) {
		uupw->blk[uupw->now_cap.pos_y[0]-1][uupw->now_cap.pos_x[0]].st = MB_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[0]-1][uupw->now_cap.pos_x[0]].co = COL_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[1]-1][uupw->now_cap.pos_x[1]].st = MB_NOTHING;
		uupw->blk[uupw->now_cap.pos_y[1]-1][uupw->now_cap.pos_x[1]].co = COL_NOTHING;
	}

}

//*** �v�l�J�n�t���O�ݒ�i�����J�n���v�l�������Ăяo���j************************
//	aifMakeFlagSet
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifMakeFlagSet( game_state *uupw )
{
	uupw->ai.aiok = FALSE;
	uupw->ai.aiOldRollCnt = 0;
	uupw->ai.aiRollCnt = 0;
	uupw->ai.aiRollFinal = 0;
	uupw->ai.aiRollHabit = 0;
}


//*** �Q�[���J�n������������ ************************
//	aifGameInit
//in:	none
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifGameInit(void)
{
	int		i;

	for(i=0;i<4;i++) {
		game_state_data[i].ai.aiok = TRUE;
		game_state_data[i].ai.aiRandFlag = TRUE;
		game_state_data[i].ai.aiNum = 0;
		game_state_data[i].ai.aiState = 0;
		game_state_data[i].ai.aiRootP = 100;
	}

//	game_state_data[0].player_state[0] = PUF_PlayerCPU;
}

//*** �v���O�����J�n������������ ************************
//	aifFirstInit
//in:	none
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifFirstInit(void)
{
	int		i,j;

	for(i = 0;i < 8;i++) {
		for(j = 0;j < 4;j++) {
			ai_param[j][i] = ai_param_org[j][i];
		}
	}

	aifGameInit();
}


/***********************************************
	cpu ai set
***********************************************/
//*** �b�o�t�`�h�������C���v���O���� ************************
//	aifMake
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifMake(game_state* uupw)
{
	if ( uupw->ai.aiok ) return;							// �����v�l�����H

	if ( aiUsedFlag ) return;								// �P�C���g���ɒN�����v�l�������s�Ȃ����H

	aifMakeWork(uupw);										// ���[�N�̏�����
	aifMakeBlkWork(uupw);

	GOALX = uupw->cap.mx+1;									// �J�v�Z���ʒu�̐ݒ�
	GOALY = uupw->cap.my;
	if ( GOALY == 0 ) GOALY++;								// y=0�̎��͗����J�n�O�Ȃ̂�1�ɐݒ�

	aipn = uupw->pn;

	aiUsedFlag = TRUE;
	uupw->ai.aivl = uupw->pu[PU_CPULEV];

	if ( FallSpeed[uupw->cap.sp] > aiSlideFSpeed[aiSelSpeed][uupw->ai.aivl] ) {
		aiMoveSF = 1;										// �����o�H�{���������鑬�x�����쑬�x���͏Ă��Ƃ��͉��ړ����P�����ɂ��P��݂̂Ƃ��Č���
	} else {
		aiMoveSF = 0;
	}

	// ���� a
	aifFieldCopy(uupw);	// ̨�����ް���߰
	aifPlaceSearch();	// ��߾ْu���껰�
	aifMoveCheck();		// �ړ��\����

	// ����
	aiSetCharacter(uupw);	// �L�����N�^�[�ʎv�l�p�����[�^�ݒ�

	// ����
	aiHiruAllPriSet(uupw);		// �u���ꏊ����!
	aiHiruSideLineEraser(uupw);	// ���̔j��

	// ���� b
	aifReMoveCheck();	// �ړ�ٰċL�^
	aifKeyMake(uupw);		// �����쐬
	uupw->ai.aiKeyCount = 0;

	uupw->ai.aiok = TRUE;
}

//������������������������������������������������������������������� �e�ʒu�ɉ�����D��x ���
enum {	// af->hei(wid)[x][]:
	LnMaster,	// ��: �y��
	LnSlave,	// �]: ����(�y��)
};
/*
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
*/
enum {	// ��~�׸�
	SL_Both,	// �c������
	SL_OnlyHei,	// �c�̂�
	SL_OnlyWid,	// ���̂�
};
#define NonTouch -1000000

// ��۰���
AI_FIELD	aif_field[FIELDY][FIELDX];	// ��ʋ^�����
u8	hei_data[LnTableMax];						// �×�����
u8	wid_data[LnTableMax];						// ֺ������

//������������������������������������������������������������������� ��������j��w�߂Ƃ��̎��s
// �V�䂪�߂����Ķ�߾ق�ς�ł������Ȃ���ٽ�𓃂����Ăď����ɍs������
//

//#define DontTouch	0xff
//enum {
//	SRH_Virus,
//	SRH_Caps,
//	SRH_None,
//	SRH_Decide,
//};
u8	aiLinePri[] = { 4,3,5,2,6,1,7,0 };	// ����������
//u8	aiVirusCnt	= 0;	// ��ٽ����
//u8	aiVrsEqCnt	= 0;	// ��ٽ���̕ϓ����Ȃ�������
//u8	aiVrsEqMax	= VRS_EQ_CNTa;
#define CAPS_HIGH 5			// ���C���ȏ�ɶ�߾ق������ā�
#define VIRUS_HIGH 14		// ���C���ȏ�ɂ�����ٽ����������������

//*** �V�䂪�߂����Ķ�߾ق�ς�ł������Ȃ���ٽ�𓃂����Ăď����ɍs������ ************************
//	aiHiruSideLineEraser
//in:	game_state	xpw				��ڲ԰�\����
//out:	game_state	xpw				��ڲ԰�\����
//
void aiHiruSideLineEraser
(
	game_state*	xpw
){
	int		x,y,z,i,j,f,f2,fc,r,h[2],w[2],m,k,p,v,c,a,b,e,g,g2,n,xx,cf[2];
	u8		low_virus[MFx];		// ����4ײ݈ȏ��Ԃ����鳨ٽ��Y���W
	u8		low_capsx[MFx];		// low_virus[]�̉��̶�߾ق�Y���W
	u8		lst_virus[MFx];		// ײݏ�̈�ԉ��̳�ٽ��Y���W
	u8		lst_capsx[MFx];		// ײݏ�̈�ԏ�̶�߾ق�Y���W

	u8		chk_line[8];

//	if ( aiFlag[decide].hei[LnMaster][LnOnLinVrs] && aiFlag[decide].tory == tate ) return;
	if ( aiFlag[decide].hei[LnMaster][LnOnLinVrs] || aiFlag[decide].hei[LnSlave][LnOnLinVrs] ) return;


	// ���̏�Ԃ̌���
	fc = FALSE;
	f2 = TRUE;
	f = FALSE;
	for( x=0;x<8;x++ ) {
		low_virus[x] = low_capsx[x] = lst_capsx[x] = 17;
		lst_virus[x] = 0;
	}
	for( x=0;x<8;x++ ) {
		for( y=16,z=g=g2=0;y>1;y-- ) {
			if( MB_NOTHING == xpw->blk[y][x].st ) {						// �����Ȃ���
				z++;
			} else if( MB_VIRUS_A <= xpw->blk[y][x].st && xpw->blk[y][x].st <= MB_VIRUS_C ) {
				if ( !g2 ) {											// ��ٽ��������
//					if( !g ) {
						lst_virus[x] = y;
//						g++;
//					}
					if( z >= 3 ) {
						low_virus[x] = y;
					} else {
						e = xpw->blk[y][x].co;
						if(( xpw->blk[y+2][x].st == MB_NOTHING || xpw->blk[y+2][x].co == e )&& xpw->blk[y+3][x].co == e ) {
							low_virus[x] = y;
						} else {
							if ( (x == 2) || (x == 3) || (x == 4) || (x == 5) ) {
								lst_virus[x] = 0;
								g2 = -1;
								low_capsx[x] = y;
								z = 0;
								lst_capsx[x] = y;
							}
						}
					}
					g2++;
				}
			} else {													// ��߾ق�������
				if ( !g2 ) {
					low_capsx[x] = y;
					z=0;
				}
				lst_capsx[x] = y;
			}
		}
		if( (lst_virus[x] != 0) && (lst_capsx[x] <= CAPS_HIGH) ) {		// ��ٽ�̏�̃J�v�Z���������Ȃ����������
			f2 = FALSE;
			if ( (x == 2) || (x == 3) || (x == 4) || (x == 5) ) {
				fc = TRUE;
				f = FALSE;
				break;
			}
		}
		if( lst_virus[x] >= VIRUS_HIGH ) {								// ��ٽ�����̂ق��Ɏc���Ă��鎞�͂܂����͍��Ȃ�
			f = TRUE;
		}
	}

	if( !(f|f2) ) {

//if ( aipn == 0 ) osSyncPrintf("\npass");
//osSyncPrintf("\npass %d",aipn);

		for( x=0;x<8;x++ ) chk_line[x] = TRUE;
		cf[0] = cf[1] = TRUE;
		f = FALSE;
		for( p=0;p<8 && !f;p++ ) {										// �Sײ݌���
			x = aiLinePri[p];											// ��ײ��è��̍�����
			v = low_virus[x];
			if( VIRUS_HIGH > v ) chk_line[x] = FALSE;					// ����ײ݂�?
			if ( VIRUS_HIGH > v && cf[x/4] ) {
				cf[x/4] = FALSE;										// ��������ײ݂��O�����������Ȃ�
//				if ( x > 0 ) chk_line[x-1] = TRUE;
//				if ( x < 7 ) chk_line[x+1] = TRUE;
				c = low_capsx[x];										// c : ��߾ق�ςݏグ��ʒu��Y���W
				r = c - v;												// r : �ςވʒu���糨ٽ�܂ł̊Ԋu
				if( x >= 4 ) xx = x - 1;								// ���u���̎��A���E�ǂ���ɂ͂ݏo����
				else		 xx = x;
				for( m=h[0]=h[1]=w[0]=w[1]=0;m<aiFlagCnt;m++ ) {		// �u���ʒu�܂ł̈ړ���߾ق̌o�H������
					if( aiFlag[m].ok == TRUE && aiFlag[m].y == c - 1 ) {	// �i�޸� : �O�� && �� || �ɂȂ��Ă��j
						if( aiFlag[m].tory == tate ) {
							if( aiFlag[m].x -1 == x ) {
								h[aiFlag[m].rev] = m+1;
							}
						} else {
							if( aiFlag[m].x -1 == xx ) {
								w[aiFlag[m].rev] = m+1;
							}
						}
					}
				}
				if( h[0] + w[0] + h[1] + w[1] ) {
					e = xpw->blk[v][x].co;									// e : ����������ٽ�̐F
					g = n = FALSE;
					if( r == 5 ) {											// 5ײݗ���Ă��鎞�͓K���ɉ��ɒu��
						if( w[0] + w[1] ) {
							f = w[0];
						}
						if ( h[0] + h[1] ) {								// ��ٽ�Ɠ����F�����鎞�͂����������ɏc�ɒu��
							if( aiNext.ca == e ) {
								f = h[0];
							} else if( aiNext.cb == e ) {
								f = h[1];
							}
						}
					} else if( r <= 4 ) {									// 4ײ݈ȉ��̎�
						if( w[0] + w[1] ) {									// �ِF��߾قų�ٽ�Ɠ����F�����鎞�͐F�����킹�ĉ��ɒu��
							if( aiNext.ca == e ) {
								if( x < 4 ) f = w[0];
								else		f = w[1];
							} else if( aiNext.cb == e ) {
								if( x < 4 ) f = w[1];
								else		f = w[0];
							}
						}
						if( h[0] + h[1] ) {
							if( aiNext.ca == aiNext.cb && e == aiNext.ca ) {// ���F��߾قų�ٽ�Ɠ����F�Ȃ�c�ɒu��
								f = h[0];
							}
						}
					} else {												// 5ײ݂�藣��Ă��鎞�͓K���ɏc�ɒu��
						if( h[0] ) {
							f = h[0];
						}
					}
				}
			}
		}
		if( f ) {
			decide = f - 1;											// �u���ꏊ�����肵�Ă�����o�H�ԍ�������

//if ( aipn == 0 ) osSyncPrintf(" set!!!");

		} else if ( !fc ) {
			z = -1;
			for( i=0,x=NonTouch-1;i<aiFlagCnt;i++ ) {
				if ( aiFlag[i].tory == tate ) {
					if( (chk_line[aiFlag[i].x-1] || (low_virus[aiFlag[i].x-1] > aiFlag[i].y)) && aiFlag[i].pri > x ) {
						x = aiFlag[i].pri;
						z = i;
					}
				} else {

if ( aiFlag[i].x == 0 ) osSyncPrintf("Error !!");

					if( (chk_line[aiFlag[i].x-1] || (low_virus[aiFlag[i].x-1] > aiFlag[i].y)) && (chk_line[aiFlag[i].x] || (low_virus[aiFlag[i].x] > aiFlag[i].y)) &&	 aiFlag[i].pri > x ) {
						x = aiFlag[i].pri;
						z = i;
					}
				}
			}
			if ( z != -1 ) decide = z;
		}
	}
}

//������������������������������������������������������������������� �K�E���F�͂��Ƃ��̎��s
// ���E�Rײݕ��ɶ�߾قœV������A����̍U����h�䂵����s�\���Ԃ�Z������K�E�Z�I(���g�p)
//

//#define DontTouch	0xff
//enum {
//	SRH_Virus,
//	SRH_Caps,
//	SRH_None,
//	SRH_Decide,
//};
//u8	aiLinePri[] = { 4,3,5,2,6,1,7,0 };
//u8	aiVirusCnt	= 0;	// ��ٽ����
//u8	aiVrsEqCnt	= 0;	// ��ٽ���̕ϓ����Ȃ�������
//u8	aiVrsEqMax	= VRS_EQ_CNTa;
//#define CAPS_HIGH 4			// ���C���ȏ�ɶ�߾ق������ā�
//#define VIRUS_HIGH 12		// ���C���ȏ�ɂ�����ٽ����������������

/*
void aiKumaSideLineblocker
(
	game_state*	xpw
){
	int		x,y,z,i,j,f,f2,r,h[2],w[2],m,k,p,v,c,a,b,e,g,g2,n,xx;
	u8		low_virus[MFx];		// ����4ײ݈ȏ��Ԃ����鳨ٽ��Y���W
	u8		low_capsx[MFx];		// low_virus[]�̉��̶�߾ق�Y���W
	u8		lst_virus[MFx];		// ײݏ�̈�ԉ��̳�ٽ��Y���W
	u8		lst_capsx[MFx];		// ײݏ�̈�ԏ�̶�߾ق�Y���W

	// ���̏�Ԃ̌���
	f2 = TRUE;
	f = FALSE;
	for( x=0;x<8;x++ ) {
		low_virus[x] = low_capsx[x] = lst_capsx[x] = 17;
		lst_virus[x] = 0;
		for( y=16,z=g=g2=0;y>1;y-- ) {
			if( MB_NOTHING == xpw->blk[y][x].st ) {
				z++;
			} else if( MB_VIRUS_A <= xpw->blk[y][x].st && xpw->blk[y][x].st <= MB_VIRUS_C ) {
				if ( !g2 ) {
					if( !g ) {
						lst_virus[x] = y;
						g++;
					}
					if( z >= 3 ) {
						low_virus[x] = y;
					} else {
						e = xpw->blk[y][x].co;
						if(( xpw->blk[y+2][x].st == MB_NOTHING || xpw->blk[y+2][x].co == e )&& xpw->blk[y+3][x].co == e ) {
							low_virus[x] = y;
						}
					}
					g2++;
				}
			} else {
				if ( !g2 ) {
					low_capsx[x] = y;
					z=0;
				}
				lst_capsx[x] = y;
			}
		}
		if( (lst_virus[x] != 0) && (lst_capsx[x] <= CAPS_HIGH) ) {		// ��ٽ�̏�̃J�v�Z���������Ȃ����������
			f2 = FALSE;
		}
		if( lst_virus[x] >= VIRUS_HIGH ) {								// ��ٽ�����̂ق��Ɏc���Ă��鎞�͂܂����͍��Ȃ�
			f = TRUE;
		}
	}

	if( !(f|f2) ) {

//osSyncPrintf("\npass");

		f = FALSE;
		for( p=0;p<8 && !f;p++ ) {										// �Sײ݌���
			x = aiLinePri[p];											// ��ײ��è��̍�����
			v = low_virus[x];
			if( VIRUS_HIGH > v ) {										// ����ײ݂�?
				c = low_capsx[x];										// c : ��߾ق�ςݏグ��ʒu��Y���W
				r = c - v;												// r : �ςވʒu���糨ٽ�܂ł̊Ԋu
				if( x >= 4 ) xx = x - 1;								// ���u���̎��A���E�ǂ���ɂ͂ݏo����
				else		 xx = x;
				for( m=h[0]=h[1]=w[0]=w[1]=0;m<aiFlagCnt;m++ ) {		// �u���ʒu�܂ł̈ړ���߾ق̌o�H������
					if( aiFlag[m].ok == TRUE && aiFlag[m].y == c - 1 ) {	// �i�޸� : �O�� && �� || �ɂȂ��Ă��j
						if( aiFlag[m].tory == tate ) {
							if( aiFlag[m].x -1 == x ) {
								h[aiFlag[m].rev] = m+1;
							}
						} else {
							if( aiFlag[m].x -1 == xx ) {
								w[aiFlag[m].rev] = m+1;
							}
						}
					}
				}
				if( !( h[0] + w[0] + h[1] + w[1] )) break;				// �u���Ȃ����ʏ�̂܂�
				e = xpw->blk[v][x].co;									// e : ����������ٽ�̐F
				g = n = FALSE;
				if( r == 5 ) {											// 5ײݗ���Ă��鎞�͓K���ɉ��ɒu��
					if( w[0] + w[1] ) {
						f = w[0];
					}
				} else if( r <= 4 ) {									// 4ײ݈ȉ��̎�
					if( h[0] + h[1] ) {
						if( aiNext.ca == aiNext.cb && e == aiNext.ca ) {// ���F��߾قų�ٽ�Ɠ����F�Ȃ�c�ɒu��
							f = h[0];
							break;
						}
					}
					if( w[0] + w[1] ) {									// �ِF��߾قų�ٽ�Ɠ����F�����鎞�͐F�����킹�ĉ��ɒu��
						if( aiNext.ca == e ) {
							if( x < 4 ) f = w[0];
							else		f = w[1];
						} else if( aiNext.cb == e ) {
							if( x < 4 ) f = w[1];
							else		f = w[0];
						}
					}
				} else {												// 5ײ݂�藣��Ă��鎞�͓K���ɏc�ɒu��
					if( h[0] ) {
						f = h[0];
					}
				}
			}
		}
		if( f ) decide = f - 1;											// �u���ꏊ�����肵�Ă�����o�H�ԍ�������
	}
}
*/

//�������������������������	 �ڰ�ײ�

//*** �������ꂽ��߾ق̔��Α����ʂɕϊ����� ************************
//	aif_MiniChangeBall
//in:	u8			x,y			�������ꂽ��߾ق̍��W�ʒu
//		AI_FIELD	aif_field	m̨����
//out:	AI_FIELD	aif_field	m̨����
//
aif_MiniChangeBall( u8 y, u8 x )
{
	static s8 srh[4][2] = {{ 1,0,},{ -1,0,},{ 0,1,},{ 0,-1,}};
	s8	yy,xx,f;

	if( MB_CAPS_BALL != aif_field[y][x].st ) {
		f = aif_field[y][x].st - MB_CAPS_U;
		yy = y + srh[f][0];
		xx = x + srh[f][1];
		if( yy > 0 && yy < 17 && xx >= 0 && xx < 8 ) {
			aif_field[yy][xx].st = MB_CAPS_BALL;
		}
	}
}

//*** ���ۂɏ��������󋵂����A������ٽ���ඳ�� ************************
//	aifEraseLineCore
//in:	int			mx,my		�����������W
//		AI_FIELD	aif_field	m̨����
//out:	int						(���g�p)
//		AI_FIELD	aif_field	m̨����
//		u8			hei_data	�����������W�̏c���C���̏��
//		u8			wid_data	�����������W�̉����C���̏��
//
int aifEraseLineCore
(
	int	mx, int my	// ���ޯ��߼޼��
){
	int	x,y,z = FALSE,f;
	u8	tc = aif_field[my][mx].co;

	if( hei_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		for( y=my-1;y>0 && y>my-4;y-- ) {
			if( aif_field[y][mx].co == tc ) {
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( y,mx );
				}
				aif_field[y][mx].st = MB_NOTHING;
				aif_field[y][mx].co = COL_NOTHING;
			} else {
				y = 0;
			}
		}
		for( y=my+1;y<my+4 && y<17;y++ ) {
			if( aif_field[y][mx].co == tc ) {
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( y,mx );
				}
				aif_field[y][mx].st = MB_NOTHING;
				aif_field[y][mx].co = COL_NOTHING;
			} else {
				y = 17;
			}
		}
	}
	if( wid_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		for( x=mx-1;x>-1 && x>mx-4;x-- ) {
			if( aif_field[my][x].co == tc ) {
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( my,x );
				}
				aif_field[my][x].st = MB_NOTHING;
				aif_field[my][x].co = COL_NOTHING;
			} else {
				x = -1;
			}
		}
		for( x=mx+1;x<mx+4 && x<8;x++ ) {
			if( aif_field[my][x].co == tc ) {
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnEraseVrs]++;
				} else {
					aif_MiniChangeBall( my,x );
				}
				aif_field[my][x].st = MB_NOTHING;
				aif_field[my][x].co = COL_NOTHING;
			} else {
				x = 8;
			}
		}
	}
	if( z ) {
		aif_MiniChangeBall( my,mx );									// �������������ꍇ����������
		aif_field[my][mx].st = MB_NOTHING;
		aif_field[my][mx].co = COL_NOTHING;
	}
}

//������������������������� ײݻ��
#define	Sub_Divide 3	// ��ގw�肳�ꂽ�߲�Ă̊���Z�l

enum {	// �P���߾ق̏�: AloneCapP[af->only[x]]
	ALN_FALSE,	// �P���߾قł͂Ȃ�
	ALN_Bottom,	// ̨���ލŉ��w
	ALN_FallCap,// �������Ķ�߾ق̏�
	ALN_FallVrs,//	  ..   ��ٽ�̏�
	ALN_Capsule,// ��߾ق̏�
	ALN_Virus,	// ��ٽ�̏�
	ALN_MAX,	//--- Max = 6
};

// ��۰����ް�
s16	bad_point[] = { -90,-180-90,-270-90,-900,-900,-270-90,-180-90,-90 };
s16	bad_point2[] = { -90,-180-90,-270-90,-9000,-9000,-270-90,-180-90,-90 };
s16	pri_point[] = {
	 NULL,	// Erase: Line			  -> EraseLinP[x]
	 30+150,	//		  Virus
	  9,	//		  BombCount
	 NULL,	// No Erase: InLineCount  -> LinesAllp[x]
	 15+16,	//			 Virus
	 NULL,	//			 LineSpace( < 4:BadLine )
	 0,		//
	 0,		//
	 0,		//
	 NULL,
};
/*
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
*/
s16	EraseLinP[] = { NULL,  30, 90,180,270,360,540,540,540, };	// [LnEraseLin]	�������������Z�����|�C���g
float	HeiEraseLinRate = 1.0;
float	WidEraseLinRate = 1.0;
s16	HeiLinesAllp[] = { NULL,NULL, 18-4, 45-5, 45, 45, 45, 45, 45, };	// [LnLinesAll]	���F���C���̒����ŉ��Z�����|�C���g�i�cײ݁j
s16	WidLinesAllp[] = { NULL,NULL, 18-4, 45-5, 45, 45, 45, 45, 45, };	// [LnLinesAll]	���F���C���̒����ŉ��Z�����|�C���g�i��ײ݁j
s16	AloneCapP[] = { NULL, -60,-70,-90,-80,-100 };				// af->only[] �u���ꏊ���Ȃ����ɓK���ȏ��ɒu�����̌��Z�߲��
s16	AloneCapWP[] = { NULL, 0, 0, 0, 0, 0 };						// af->wonly[] �u���ꏊ���Ȃ����ɓK���ȏ��ɒu�����̌��Z�߲��(��ײݐ�p)
s16	OnVirusP = 0;
s16	HiEraseP = 40;
s16	RensaP = 300;
s16	RensaMP = -300;

s16	LPriP = 0;

u8	BadLineRate[][8] = { { 6,7,8,9,9,8,7,6,},
						 { 6,7,8,9,9,8,7,6,},
						 { 2,2,4,7,7,4,2,2,},
						 { 1,1,2,4,4,2,1,1,} };

s16	WallRate[][8] = { { 10,10,10,10,10,10,10,10, },
					  { 64,64,32,16, 8, 4, 2, 1, },
					  {	 1, 2, 4, 8,16,32,64,64, },
					  { 64,64,16, 4, 4,16,64,64, } };


//*** �A���������i�����Ɋ댯�]�[���̃J�v�Z���������邩�ǂ����̌v�Z���܂ށj ************************
//	aifRensaCheckCore
//in:	game_state	uupw		��ڲ԰�\����
//		AI_FLAG		af			�ړ���߾ق̏��
//		u8			mx,my		Ͻ����߾ق̍��Wx�y
//		u8			mco,mst		Ͻ����߾ق̐F��`
//		u8			sx,sy		�ڰ�޶�߾ق̍��Wx�y
//		u8			sco,sst		�ڰ�޶�߾ق̐F��`
//out:	int						�����łP�A���������łQ�A�����Ȃ����͂O��Ԃ�
//		s16			aiHiEraseCtr	��̕������ꂽ������
//
int aifRensaCheckCore(game_state* uupw ,AI_FLAG* af ,u8 mx,u8 my,u8 mco,u8 mst,u8 sx,u8 sy,u8 sco,u8 sst)
{
	int	idx,idy;
	game_state*	upw;
	u8		ctr = 0;
	u8		co;
	u8		c,i;
	u8		x,y;

	upw = uupw;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		aif_field[idy][idx].co = upw->blk[idy][idx].co;	// �F����߰
		aif_field[idy][idx].st = upw->blk[idy][idx].st;	// �ð������߰
	}}

	aiHiEraseCtr = 0;										// �댯�]�[���̃J�v�Z�����𐔂��Ă���
	for ( idx=0 ; idx < FIELDX ; idx++ ) {
	for ( idy=1 ; idy < 4 ; idy++ ) {
//		if ( (idx != mx || idy != my) && (idx != sx || idy != sy) && aif_field[idy][idx].st <= MB_VIRUS_C ) {
		if ( aif_field[idy][idx].st <= MB_VIRUS_C ) {
			aiHiEraseCtr += BadLineRate[idy][idx];
		}
	}}

	if ( my ) {
		aif_field[my][mx].st = mst;
		aif_field[my][mx].co = mco;
	}
	if ( sy ) {
		aif_field[sy][sx].st = sst;
		aif_field[sy][sx].co = sco;
	}

	// �����O��������
	for(idy = 1;idy < FIELDY;idy++) {
		for(idx = 0;idx < FIELDX-3;idx++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idx+c < FIELDX;c++) {
					if ( (aif_field[idy][idx+c].st == MB_NOTHING) || (aif_field[idy][idx+c].co != co) ) break;
				}
				if ( c >= DELETE_LINE ) {
					ctr += c / DELETE_LINE;
//					if ( ctr > 1 ) return 2;
					for(i = 0;i < c;i++) {
						aif_MiniChangeBall( idy, idx+i);
						aif_field[idy][idx+i].st = MB_ERASE_CAPS;
					}
				}
				idx += c - 1;
			}
		}
	}
	for(idx = 0;idx < FIELDX;idx++) {
		for(idy = 1;idy < FIELDY-3;idy++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idy+c < FIELDY;c++) {
					if ( (aif_field[idy+c][idx].st == MB_NOTHING) || (aif_field[idy+c][idx].co != co) ) break;
				}
				if ( c >= DELETE_LINE ) {
					ctr += c / DELETE_LINE;
//					if ( ctr > 1 ) return 2;
					for(i = 0;i < c;i++) {
						aif_MiniChangeBall( idy+i, idx);
						aif_field[idy+i][idx].st = MB_ERASE_CAPS;
					}
				}
				idy += c - 1;
			}
		}
	}

	// ��������
	c = TRUE;
	for(idy = FIELDY-1;idy >= 1;idy--) {
		for(idx = FIELDX-1;idx >= 0;idx--) {
			switch( aif_field[idy][idx].st ) {
			case MB_ERASE_CAPS:
				aif_field[idy][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_BALL:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_D:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				aif_field[i-1][idx].st = aif_field[idy-1][idx].st;
				aif_field[i-1][idx].co = aif_field[idy-1][idx].co;
				aif_field[idy-1][idx].st = MB_NOTHING;
				break;
			case MB_CAPS_L:
				if ( idy == FIELDY-1 ) break;
				if ( aif_field[idy+1][idx].st != MB_NOTHING || aif_field[idy+1][idx+1].st != MB_NOTHING ) break;
				c = FALSE;
				for(i = idy+1;i < FIELDY && aif_field[i][idx].st == MB_NOTHING && aif_field[i][idx+1].st == MB_NOTHING;i++ );
				i--;
				aif_field[i][idx].st = aif_field[idy][idx].st;
				aif_field[i][idx].co = aif_field[idy][idx].co;
				aif_field[idy][idx].st = MB_NOTHING;
				aif_field[i][idx+1].st = aif_field[idy][idx+1].st;
				aif_field[i][idx+1].co = aif_field[idy][idx+1].co;
				aif_field[idy][idx+1].st = MB_NOTHING;
				break;
			}
		}
	}

	for ( idx=0 ; idx < FIELDX ; idx++ ) {					// �댯�]�[������ǂꂾ���J�v�Z�������������J�E���g
	for ( idy=1 ; idy < 4 ; idy++ ) {
		if ( aif_field[idy][idx].st <= MB_VIRUS_C ) {
			aiHiEraseCtr -= BadLineRate[idy][idx];
		}
	}}

	if ( ctr > 1 ) {
		if ( c ) return 2;
		return 1;
	}

	// �������������
	for(idy = 1;idy < FIELDY;idy++) {
		for(idx = 0;idx < FIELDX-3;idx++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idx+c < FIELDX && aif_field[idy][idx+c].st != MB_NOTHING && aif_field[idy][idx+c].co == co;c++);
				if ( c >= DELETE_LINE ) return 1;
			}
		}
	}
	for(idx = 0;idx < FIELDX;idx++) {
		for(idy = 1;idy < FIELDY-3;idy++) {
			if ( aif_field[idy][idx].st != MB_NOTHING ) {
				co = aif_field[idy][idx].co;
				for(c = 1;idy+c < FIELDY && aif_field[idy+c][idx].st != MB_NOTHING && aif_field[idy+c][idx].co == co;c++);
				if ( c >= DELETE_LINE ) return 1;
			}
		}
	}

	return 0;
}

//*** ���݂̑���J�v�Z����ݒ肵�ĘA���`�F�b�N�i�����Ɋ댯�]�[���̃J�v�Z���������邩�ǂ����̌v�Z���܂ށj ************************
//	aifRensaCheck
//in:	game_state	uupw		��ڲ԰�\����
//		AI_FLAG		af			�ړ���߾ق̏��
//out:	int						�����łP�A���������łQ�A�����Ȃ����͂O��Ԃ�
//		s16			aiHiEraseCtr	��̕������ꂽ������
//
int aifRensaCheck(game_state* uupw ,AI_FLAG* af)
{
	int	idx,idy;
	game_state*	upw;
	u8		ctr = 0;
	u8		co;
	u8		c,i;
	u8		x,y;
	u8		mx,my,mco,mst;
	u8		sx,sy,sco,sst;

	upw = uupw;

	x = af->x - 1;
	y = af->y;

	if( af->tory == tate ) {
		mx = x;
		my = y;
		mst = MB_CAPS_D;
		sx = x;
		sy = y-1;
		sst = MB_CAPS_U;
		if( af->rev == FALSE ) {
			mco = aiNext.cb;
			if( y - 1 > 0 ) {
				sco = aiNext.ca;
			}
		} else {
			mco = aiNext.ca;
			if( y - 1 > 0 ) {
				sco = aiNext.cb;
			}
		}
	} else {
		mx = x;
		my = y;
		mst = MB_CAPS_L;
		sx = x+1;
		sy = y;
		sst = MB_CAPS_R;
		if( af->rev == FALSE ) {
			mco = aiNext.ca;
			sco = aiNext.cb;
		} else {
			mco = aiNext.cb;
			sco = aiNext.ca;
		}
	}

	return aifRensaCheckCore(upw ,af ,mx,my,mco,mst, sx,sy,sco,sst);
}

//�������������������������	 ײݻ�� Core

//*** ����J�v�Z����u�������̎��Ӄ��C�������� ************************
//	aifSearchLineCore
//in:	int			mx,my		����������W
//		int			fg			�c����ǂ����������邩�׸�(SL_???)
//		AI_FIELD	aif_field	m̨����
//out:	int						TRUE�F������FALSE�F�������ANULL�F�����O���牽���Ȃ�
//		u8			hei_data	�����������W�̏c���C���̏��
//		u8			wid_data	�����������W�̉����C���̏��
//
int aifSearchLineCore
(
	int	mx,int my,	// ���ޯ��߼޼��
	int	fg			// SL_???
){
	int	i,x,y,z,w,p;
	u8	tc = aif_field[my][mx].co;
	s8	rx,ry;

	for( i=0;i<LnTableMax;i++ ) {
		hei_data[i] = wid_data[i] = 0;
	}
	if( aif_field[my][mx].st == MB_NOTHING ) {
		return NULL;
	}

	// Height����
	if( SL_OnlyWid != fg ) {
		for( y=my-1,z=TRUE;y>0 && y>my-4;y-- ) {						// �����ʒu�����3���
			if( aif_field[y][mx].co != tc ) {
				if( aif_field[y][mx].co != COL_NOTHING ) {
					y = 0;												// �Ⴄ�F�����鎞�ͻ���I��
				} else {
					z = FALSE;											// �����Ȃ��Ƃ��͗א�ײݐ������׸�ON
				}
			} else {
				hei_data[LnLinesAll]++;									// ���F������
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					hei_data[LnOnLinVrs]++;								// ���F�̳�ٽ�̂Ƃ��A��ٽ������
					if ( y < BADLINE ) hei_data[LnHighVrs] += BadLineRate[y][mx];			// ����������
				} else {
					if ( y < BADLINE ) hei_data[LnHighCaps] += BadLineRate[y][mx];			// ����������
				}
				if( z ) hei_data[LnRinsetsu]++;							// ��x�󔒂����񂾂Ƃ��אڐ��ඳ��
			}
			if( y > 0 ) hei_data[LnLinSpace]++;							// ���F�������u����ײݐ����āi4���ׂ��邩�������p�j
		}
		for( y=my+1,z=TRUE,w=0;y<17;y++,w++ ) {							// �����ʒu���炵���S������i������ɑ����ꍇ���������邽�߁j
			if( aif_field[y][mx].co != tc ) {
				if( aif_field[y][mx].co != COL_NOTHING ) {
					y = 17;
				} else {
					z = FALSE;
				}
			} else {
				hei_data[LnLinesAll]++;
				if( MB_VIRUS_A <= aif_field[y][mx].st && aif_field[y][mx].st <= MB_VIRUS_C ) {
					if( w < 3 ) {											// ��ٽ�͗����Ȃ��̂������ʒu����3�̂�����
						hei_data[LnOnLinVrs]++;
						if ( y < BADLINE ) hei_data[LnHighVrs] += BadLineRate[y][mx];			// ����������
					}
				} else {
					if ( y < BADLINE ) hei_data[LnHighCaps] += BadLineRate[y][mx];				// ����������
				}
				if( z ) hei_data[LnRinsetsu]++;
				hei_data[LnLinSpace]++;
			}
		}
		hei_data[LnRinsetsu]++;											// �����̐��𑫂�
		hei_data[LnLinesAll]++;
		hei_data[LnLinSpace]++;
	}

	// Width����
	if( SL_OnlyHei != fg ) {
		for( x=mx-1,z=w=TRUE;x>-1 && x>mx-4;x-- ) {						// �����ʒu���獶3���
			if( aif_field[my][x].co != tc ) {
				if( aif_field[my][x].co != COL_NOTHING ) {
					x = -1;												// �Ⴄ�F������Ƃ��ͻ���I��
				} else {												// �����Ȃ��Ƃ�
					z = FALSE;											//	�אڐ������׸�ON
					if( w ) {
						for(i=p=0;i<aiFlagCnt;i++) {					// ���̋󔒈ʒu�ɕ����u���邩�ǂ�������
							if( aiFlag[i].ok == TRUE ) {
								if( aiFlag[i].tory == tate ) {
									if( aiFlag[i].x -1 == x ) {
										if(	aiFlag[i].y == my || aiFlag[i].y == my+1 ) {
											p = TRUE;					// �����u����ꍇ�A
											wid_data[LnLinSpace]++;		// �����u����ײݐ����āi4���ׂ��邩�������p�j
											i=aiFlagCnt;				// �������I��
										}
									}
								}
							}
						}
						if( !p ) {										// �󔒂ŕ����u���Ȃ��ꍇ�A�������͘A�����Ȃ��ꏊ�Ƃ���
							w = FALSE;
						}
					}
				}
			} else {
				wid_data[LnLinesAll]++;									// ���F������
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnOnLinVrs]++;								// ���F��ٽ������
					if ( my < BADLINE ) wid_data[LnHighVrs] += BadLineRate[my][x];			// ����������
				} else {
					if ( my < BADLINE ) wid_data[LnHighCaps] += BadLineRate[my][x];			// ����������
				}
				if( z ) wid_data[LnRinsetsu]++;							// �אڐ�����
				if( w ) wid_data[LnLinSpace]++;							// ���F�������u����ײݐ�����
			}
		}
		for( x=mx+1,z=w=TRUE;x<mx+4 && x<8;x++ ) {						// �����ʒu����E3���
			if( aif_field[my][x].co != tc ) {
				if( aif_field[my][x].co != COL_NOTHING ) {
					x = 8;
				} else {
					z = FALSE;
					if( w ) {
						for(i=p=0;i<aiFlagCnt;i++) {
							if( aiFlag[i].ok == TRUE ) {
								if( aiFlag[i].tory == tate ) {
									if( aiFlag[i].x -1 == x ) {
										if(	aiFlag[i].y == my || aiFlag[i].y == my+1 ) {
											p = TRUE;
											wid_data[LnLinSpace]++;
											i=aiFlagCnt;
										}
									}
								}
							}
						}
						if( !p ) {
							w = FALSE;
						}
					}
				}
			} else {
				wid_data[LnLinesAll]++;
				if( MB_VIRUS_A <= aif_field[my][x].st && aif_field[my][x].st <= MB_VIRUS_C ) {
					wid_data[LnOnLinVrs]++;
					if ( my < BADLINE ) wid_data[LnHighVrs] += BadLineRate[my][x];				// ����������
				} else {
					if ( my < BADLINE ) wid_data[LnHighCaps] += BadLineRate[my][x];				// ����������
				}
				if( z ) wid_data[LnRinsetsu]++;
				if( w ) wid_data[LnLinSpace]++;
			}
		}
		wid_data[LnRinsetsu]++;											// �����̐��𑫂�
		wid_data[LnLinesAll]++;
		wid_data[LnLinSpace]++;
	}

	// ����ײݐ�����
	z = FALSE;
	if( hei_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		hei_data[LnEraseLin] = 1;
		if( hei_data[LnRinsetsu] == DELETE_LINE*2 ) {
			hei_data[LnEraseLin]++;
		}
	}
	if( wid_data[LnRinsetsu] >= DELETE_LINE ) {
		z = TRUE;
		wid_data[LnEraseLin] = 1;
		if( wid_data[LnRinsetsu] == DELETE_LINE*2 ) {
			wid_data[LnEraseLin]++;
		}
	}

	// ������"��������ײ�"���߲�Čv�Z�𖳌��ɂ���
	if( z ) {
		if( hei_data[LnEraseLin] ) {
			if( !wid_data[LnEraseLin] ) {
				wid_data[LnNonCount] = TRUE;
			}
		} else {
			if( !hei_data[LnEraseLin] ) {
				hei_data[LnNonCount] = TRUE;
			}
		}
	}

	return z;

}

//*** ���C�����Ƃ��߲�Čv�Z���� ************************
//	aifMiniPointK3
//in:	u8			*tbl		�����������W�̃��C���̏����߲��
//		u8			sub			���̃��C�����߲�ĎZ�o��Ҳ݂���ނ��H�iTRUE:��ށj
//		u8			*elin		����ײݐ��ۑ�ܰ����߲��
//		u8			flag		�c���ǂ�����߲�Ă��Z�o���邩(TRUE:�c)
//		u8			tory		�����߾ق̌���
//		u8			ec			�����߾ٓ��F�׸�(TRUE:���F)
//out:	int						�Z�o�|�C���g��
//
int	aifMiniPointK3( u8* tbl,u8 sub,u8* elin,u8 flag,u8 tory,u8 ec)
{
	int ex = 0,i;
	if( tbl[LnEraseLin] != 0 ) {							// ��������Ƃ������|�C���g
		*elin += tbl[LnEraseLin];

		tbl[LnHighCaps] = 0;
		tbl[LnHighVrs] = 0;

		for( i=LnEraseVrs;i<=LnHighVrs;i++ ) {
			ex = ex +( (int)tbl[i] * (int)pri_point[i] );
		}
	} else {												// �������Ȃ��Ƃ��ɑ����|�C���g
		if( !tbl[LnNonCount] ) {
			if( tbl[LnLinSpace] >= DELETE_LINE ) {
				if ( flag ) {
					if ( (!ec) || (tory != tate) || (tbl[LnLinesAll] > 2) )
						ex = ex +( HeiLinesAllp[tbl[LnLinesAll]] );
				} else {
					if ( (!ec) || (tory != yoko) || (tbl[LnLinesAll] > 2) )
						ex = ex +( WidLinesAllp[tbl[LnLinesAll]] );
				}
				ex = ex +( (int)tbl[LnOnLinVrs] * (int)pri_point[LnOnLinVrs] );
			}
		}
	}
	if( sub == TRUE ) {
		ex /= Sub_Divide;
	}
	return ex;
}

//*** �F�̈Ⴄ���ɃJ�v�Z����u���Ă��܂������ǂ����̃`�F�b�N ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			�u�����ʒu������������W
//		u8			f			�����������ǂ���(TRUE:��������)
//		int			ec			�����߾ٓ��F�׸�(TRUE:���F)
//		AI_FIELD	aif_field	m̨����
//		u8			hei_data	�����������W�̏c���C���̏��
//		u8			wid_data	�����������W�̉����C���̏��
//out:	int						�Ⴄ�F�̏��ɂ����Ă��܂������̏�Ԕԍ�(ALN_????)
//
int	aifMiniAloneCapNumber
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = ��������
	int	ec
){
	int	m = ALN_FALSE;
	int d = aif_field[y+1][x].st;
	if( (( hei_data[LnRinsetsu] == 1 || hei_data[LnRinsetsu] != 0 && hei_data[LnLinSpace] < DELETE_LINE )&&
		( wid_data[LnRinsetsu] == 1 || wid_data[LnRinsetsu] != 0 && wid_data[LnLinSpace] < DELETE_LINE )) ||
		( (ec == TRUE) && (hei_data[LnRinsetsu] > 0) && (wid_data[LnRinsetsu] > 0) && (((hei_data[LnLinSpace]<DELETE_LINE)?0:hei_data[LnRinsetsu])+((wid_data[LnLinSpace]<DELETE_LINE)?0:wid_data[LnRinsetsu]) <= 3) ) ) {
//		( (ec == TRUE) && (hei_data[LnRinsetsu]+wid_data[LnRinsetsu] > 9) ) ) {
			if( y == 16 ) {
				m = ALN_Bottom;
			} else {
				if( f ) {
					if( d < MB_VIRUS_A ) {
						m = ALN_FallCap;
					} else {
						m = ALN_FallVrs;
					}
				} else {
					if( d < MB_VIRUS_A ) {
						m = ALN_Capsule;
					} else {
						m = ALN_Virus;
					}
				}
			}
	}
	return m;
}

//*** �F�̈Ⴄ���ɃJ�v�Z����u���Ă��܂������ǂ����̃`�F�b�N�i�����C����p�j ************************
//	aifMiniAloneCapNumber
//in:	u8			x,y			�u�����ʒu������������W
//		u8			f			�����������ǂ���(TRUE:��������)
//		int			ec			�����߾ٓ��F�׸�(TRUE:���F)
//		AI_FIELD	aif_field	m̨����
//		u8			wid_data	�����������W�̉����C���̏��
//out:	int						�Ⴄ�F�̏��ɂ����Ă��܂������̏�Ԕԍ�(ALN_????)
//
int	aifMiniAloneCapNumberW
(
	u8 x,u8 y,	//
	u8 f,		// TRUE = ��������
	int	ec
){
	int	m = ALN_FALSE;
	int d = aif_field[y+1][x].st;
//	if( ( wid_data[LnRinsetsu] == 1 || wid_data[LnRinsetsu] != 0 && wid_data[LnLinSpace] < DELETE_LINE ) ||
//		( (ec == TRUE) && ((wid_data[LnLinSpace] < DELETE_LINE) || (wid_data[LnRinsetsu] <= 2)) ) ) {
	if( ( wid_data[LnRinsetsu] == 1 ) ||
		( (ec == TRUE) && (wid_data[LnRinsetsu] == 2) ) ) {
			if( y == 16 ) {
				m = ALN_Bottom;
			} else {
				if( f ) {
					if( d < MB_VIRUS_A ) {
						m = ALN_FallCap;
					} else {
						m = ALN_FallVrs;
					}
				} else {
					if( d < MB_VIRUS_A ) {
						m = ALN_Capsule;
					} else {
						m = ALN_Virus;
					}
				}
			}
	}
	return m;
}

//*** �D�揇�ʌ���p�|�C���g���v�Z�������C�� ************************
//	aifSearchLineMS
//in:	AI_FLAG		af			�ړ���߾ق̏��
//		int			mx,my,mco	�����߾ق�Ͻ���̍��Wx,y�ƐF
//		int			sx,sy,sco	�����߾ق̽ڰ�ނ̍��Wx,y�ƐF
//		int			ec			�����߾ٓ��F�׸�(TRUE:���F)
//out:	int						1:Ͻ���������A2:�ڰ�ނ������A0:�������Ă��Ȃ�
//		AI_FLAG		pri			�߲�Čv�Z�Z�o�l
//
int aifSearchLineMS
(
	AI_FLAG*  af,
	int	mx,	int	my,	int	mco,	// master
	int	sx,	int	sy,	int	sco,	// slave
	int	ec						// ���F FALSE/TRUE
){
	int	i,f,z,z2,x,y,j,m,s;
	int	my2,sy2;
	u8	ss[8];

	my2 = my;
	sy2 = sy;

	for(j=0;j<LnTableMax;j++) {
		af->wid[LnMaster][j] = af->hei[LnMaster][j] = af->wid[LnSlave][j] = af->hei[LnSlave][j] = 0;
	}
	af->elin[FALSE] = af->elin[TRUE] = 0;
	af->sub = FALSE;
	af->only[LnMaster] = af->only[LnSlave] = FALSE;
	af->wonly[LnMaster] = af->wonly[LnSlave] = FALSE;

	z = aifSearchLineCore(mx,my,SL_Both);								// ��߾ق�Ͻ������ײ݂�� (Z=0:�����Ȃ������A>0:������)
	z2 = FALSE;
	aifEraseLineCore(mx,my);											// Ͻ�������������Ă݂�
	if ( !z ) {
		af->only[LnMaster] = aifMiniAloneCapNumber(mx,my,FALSE,ec);			// Ͻ���̎~�܂�ʒu���
		af->wonly[LnMaster] = aifMiniAloneCapNumberW(mx,my,FALSE,ec);			// Ͻ���̎~�܂�ʒu���
	}

	for(i=0;i<LnTableMax;i++) {
		af->hei[LnMaster][i] = hei_data[i];
		af->wid[LnMaster][i] = wid_data[i];
	}
	if( aif_field[sy][sx].st != MB_NOTHING ) {							// Ͻ�������Žڰ�ނ������ĂȂ���
		if( ec ) {														// ���F��߾ق�Ͻ���������Ŷ�߾ٌ�����ײ݂͌����ςݎc��ײ݂̂݌���
			if( af->tory == tate ) {
				f = SL_OnlyWid;
			} else {
				f = SL_OnlyHei;
			}
		} else {
			f = SL_Both;
		}
		m = FALSE;
		if( FALSE == (z2 = aifSearchLineCore(sx,sy,f)) ) {				// ������O�ɂͽڰ�ނ͏����Ȃ�������
			if( z ) {
				for( y=16;y>sy;y-- ) {									// Ͻ���ŏ����Ă��Ľڰ�ނ������Ȃ��������A�ڰ�މ��̶�߾٦����
					if( aif_field[y][sx].st == MB_CAPS_BALL ) {			// Ͻ�������ɂ��ڰ�މ��̶�߾ق������邩������Ȃ��̂ŗ�����Ȃ痎��
						for( i=y+1;i<17 && aif_field[i][sx].st == MB_NOTHING;i++ );
						if( i!=y+1 ) {
							aif_field[i-1][sx].st = MB_CAPS_BALL;
							aif_field[i-1][sx].co = aif_field[y][sx].co;
							aif_field[ y ][sx].st = MB_NOTHING;
							aif_field[ y ][sx].co = COL_NOTHING;
						}
					}
				}
			}
			for( i=sy+1;i<17 && aif_field[i][sx].st == MB_NOTHING;i++ );// �ڰ�ނ𗎂�
			if( i!=sy+1 ) {
				m = TRUE;
				aif_field[i-1][sx].st = MB_CAPS_BALL;
				aif_field[i-1][sx].co = aif_field[sy][sx].co;
				aif_field[sy ][sx].st = MB_NOTHING;
				aif_field[sy ][sx].co = COL_NOTHING;
				sy=i-1;
				if( !z ) af->sub = TRUE;
				z2 = aifSearchLineCore(sx,sy,SL_Both);
			}
		}
		aifEraseLineCore(sx,sy);										// ײݏ������Ă݂�
		if ( !z2 ) {
			af->only[LnSlave] = aifMiniAloneCapNumber(sx,sy,m,ec);			// �ڰ�ނ��~�܂�ʒu�Ɉړ�
			af->wonly[LnSlave] = aifMiniAloneCapNumberW(sx,sy,m,ec);		// �ڰ�ނ��~�܂�ʒu�Ɉړ�
		}
		for(i=0;i<LnTableMax;i++) {
			af->hei[LnSlave][i] = hei_data[i];
			af->wid[LnSlave][i] = wid_data[i];
		}
	}

	// �߲�Čv�Z
	af->pri += aifMiniPointK3( &af->hei[LnMaster][0],FALSE	,&af->elin[FALSE  ] ,TRUE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->hei[LnSlave ][0],af->sub,&af->elin[af->sub] ,TRUE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->wid[LnMaster][0],FALSE	,&af->elin[FALSE  ] ,FALSE ,af->tory ,ec );
	af->pri += aifMiniPointK3( &af->wid[LnSlave ][0],af->sub,&af->elin[af->sub] ,FALSE ,af->tory ,ec );

	if ( AloneCapP[af->only[LnMaster]] ) af->pri += AloneCapP[af->only[LnMaster]];
	if ( AloneCapP[af->only[LnSlave ]] ) af->pri += AloneCapP[af->only[LnSlave ]];
	if ( AloneCapP[af->only[LnMaster]] != 0 && AloneCapP[af->only[LnSlave ]] != 0 ) af->pri -= (FIELDY - my2)*LPriP;

	if ( AloneCapWP[af->wonly[LnMaster]] ) af->pri += AloneCapWP[af->only[LnMaster]];
	if ( AloneCapWP[af->wonly[LnSlave ]] ) af->pri += AloneCapWP[af->only[LnSlave ]];
//	af->pri += EraseLinP[af->elin[FALSE]];
//	af->pri += EraseLinP[af->elin[TRUE]]/Sub_Divide;
	af->pri += (s32)((float)EraseLinP[af->hei[LnMaster][LnEraseLin]+af->hei[LnSlave][LnEraseLin]] * HeiEraseLinRate);
	af->pri += (s32)((float)EraseLinP[af->wid[LnMaster][LnEraseLin]+af->wid[LnSlave][LnEraseLin]] * WidEraseLinRate);

	// ��ٽ��u�����_�i�I�ճ�ٽ�̏��D��I�ɏ��������j
	if ( OnVirusP && my2 < FIELDY -1 ) {
		m = TRUE;
		for(y = my2-1;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {		// �l�̏�̕��ɳ�ٽ�����鎞
				m = FALSE;
				break;
			}
		}
		if ( m == TRUE ) {
			m = FALSE;
			for(y = my2+1;y < FIELDY;y++) {
				if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {		// �l�̉��̕��ɳ�ٽ�����鎞
					m = TRUE;
					break;
				}
			}
		}
		s = TRUE;
		for(y = sy2-1;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {		// �r�̏�̕��ɳ�ٽ�����鎞
				s = FALSE;
				break;
			}
		}
		if ( s ) {
			s = FALSE;
			for(y = sy2+1;y < FIELDY;y++) {
				if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {		// �r�̉��̕��ɳ�ٽ�����鎞
					s = TRUE;
					break;
				}
			}
		}
		if ( aiFieldData[my+1][mx].st <= MB_VIRUS_C ) {			// Ͻ���ƽڰ�ނ̂ǂ��炪������������?
			if ( m ) {
				if ( af->hei[LnMaster][LnRinsetsu] > ((ec && af->tory == tate)?2:1) && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
					if ( af->tory == tate ) {
						if ( ec ) {
							af->pri += OnVirusP * 2;	// ���F�̏ꍇ�Q�{�_������
						} else if ( !z ) {
							af->pri -= OnVirusP * 2;
						}
					} else {
						if ( s ) {
							if ( af->hei[LnSlave][LnLinesAll] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
								af->pri += OnVirusP;
							} else {
								af->pri -= OnVirusP * 2;
							}
						} else {
							af->pri += OnVirusP;
						}
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			} else if ( s ) {
				if ( af->hei[LnSlave][LnLinesAll] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
					if ( af->hei[LnMaster][LnRinsetsu] >= 3 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
						af->pri += OnVirusP;
					} else {
						af->pri -= OnVirusP * 2;
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			}
		} else {
			if ( s ) {
				if ( af->hei[LnSlave][LnRinsetsu] > 1 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
					if ( m ) {
						if ( af->hei[LnMaster][LnLinesAll] > 1 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
							af->pri += OnVirusP;
						} else {
							af->pri -= OnVirusP * 2;
						}
					} else {
						af->pri += OnVirusP;
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			} else if ( m ) {
				if ( af->hei[LnMaster][LnLinesAll] > 1 && af->hei[LnMaster][LnLinSpace] >= DELETE_LINE ) {
					if ( af->hei[LnSlave][LnRinsetsu] >= 3 && af->hei[LnSlave][LnLinSpace] >= DELETE_LINE ) {
						af->pri += OnVirusP;
					} else {
						af->pri -= OnVirusP * 2;
					}
				} else {
					af->pri -= OnVirusP * 2;
				}
			}
		}

		if ( z && !ec && af->tory == tate && my2 < FIELDY - 1 ) {
			i = FALSE;
			for(m = my2+1;m < FIELDY;m++) {
				if ( aiFieldData[m][sx].st > MB_VIRUS_C || aiFieldData[m][sx].co != mco ) {
					i = TRUE;
					break;
				}
			}
			if ( i ) {
				i = FALSE;
				for(y = m;y < FIELDY;y++) {
					if ( MB_VIRUS_A <= aiFieldData[y][sx].st && aiFieldData[y][sx].st <= MB_VIRUS_C ) {
						i = TRUE;
						break;
					}
				}
			}
			if ( i ) {
				for(y = m;y < FIELDY;y++) {
					if ( aiFieldData[y][sx].st <= MB_VIRUS_C ) {
						if ( aiFieldData[y][sx].co == sco ) {
							af->pri += OnVirusP * 4;
						} else {
							af->pri -= OnVirusP * 2;
						}
						break;
					}
				}
			}
		}
	}

/*
	// ��ٽ����P��J�v�Z���u�����_
	if ( (AloneCapP[af->only[LnSlave]] == ALN_Capsule || AloneCapP[af->only[LnSlave]] == ALN_Virus)
	 && (my < FIELDY - 1 && ) ) {
		for(y = my+1;y < FIELDY;y++) {
			if ( MB_VIRUS_A <= aiFieldData[y][mx].st && aiFieldData[y][mx].st <= MB_VIRUS_C ) {
				af->pri += OnVirusP;
				break;
			}
		}
	}
*/





/*
	// ���������_	���܂�Ӗ����Ȃ�
	if ( my <= BADLINE || sy <= BADLINE ) {
		if ( af->hei[LnMaster][LnEraseLin] + af->hei[LnSlave][LnEraseLin] + af->wid[LnMaster][LnEraseLin] + af->wid[LnSlave][LnEraseLin] ) {
			af->pri += HiEraseP;
		}
	}

	// ���������_	�t�Ɏキ�Ȃ�
	af->pri -= abs(mx*20 - 70) + abs(sx*20 - 70);

*/
	// ���ς݌��_
	for(x=0;x<8;x++) {
		for(y=1;y<17;y++) {
			if( aif_field[y][x].st != MB_NOTHING ) {
				ss[x] = y-1;
				break;
			}
		}
	}
	af->dead = 0;
	m = FALSE;
	if( ss[sx] < BADLINE && sy <= BADLINE && sy > 0 ) {
		if( aif_field[sy][sx].st != MB_NOTHING ) {
			if( af->hei[LnSlave][LnRinsetsu] + ss[sx] < 4 ) {
				af->dead += bad_point[sx] / (sy*2-1);
				m = TRUE;
			}
		}
	}
	if( ss[mx] < BADLINE && my < BADLINE && my > 0 && (!ec || af->tory != tate) ) {
		if( aif_field[my][mx].st != MB_NOTHING ) {
			if( af->hei[LnMaster][LnRinsetsu] + ss[mx] < 4 ) {
				if ( m ) {
					af->dead += bad_point2[mx] + bad_point2[sx] - bad_point[sx] / (sy*2-1);
				} else {
					af->dead += bad_point[mx] / (my*2-1);
				}
			}
		}
	}
	af->pri += af->dead;

	// ���E�ɕǂ��o�����Ƃ��A�ǂ�D��I�ɏ�������
	if ( aiWall ) {
		if ( WallRate[aiWall][mx] > WallRate[aiWall][sx] ) {
			af->pri = af->pri*(s32)WallRate[aiWall][mx]/10;
		} else {
			af->pri = af->pri*(s32)WallRate[aiWall][sx]/10;
		}
	}

	if ( z ) return 1;
	if ( z2 ) return 2;
	return	0;
}

//*** �u���ꏊ���� ************************
//	aiHiruAllPriSet
//in:	game_state	xpw			��ڲ԰�\����
//		AI_FIELD	aiFieldData	m̨����
//out:	u8			decide		�ŏI�I���肷���׸��ް��ԍ�
//
void aiHiruAllPriSet(game_state *xpw)
{
	AI_FLAG*	af;
	int	i,j,x,y,z,f,t,r;
	u8		mx,my,mco,mst;
	u8		sx,sy,sco,sst;
	u8		rcr,rcy,rcb,rco;

	// �v�f�v�Z
	for( i=0;i<aiFlagCnt;i++ ) {
		aiFlag[i].pri = 0;
		if( aiFlag[i].ok != 0 ) {
			af = &aiFlag[i];
			bcopy(&aiFieldData,&aif_field,sizeof(AI_FIELD)*(FIELDY)*(FIELDX));
			x = af->x-1;
			y = af->y;
			f = FALSE;
			if( af->tory == tate ) {
				mx = x;
				my = y;
				mst = MB_CAPS_D;
				sx = x;
				sy = y - 1;
				if( y - 1 > 0 ) {
					sst = MB_CAPS_U;
				}
				if( af->rev == FALSE ) {
					mco = aiNext.cb;
					if( y - 1 > 0 ) {
						sco = aiNext.ca;
					}
				} else {
					mco = aiNext.ca;
					if( y - 1 > 0 ) {
						sco = aiNext.cb;
					}
				}
				if ( my ) {
					aif_field[my][mx].st = mst;
					aif_field[my][mx].co = mco;
				}
				if ( sy ) {
					aif_field[sy][sx].st = sst;
					aif_field[sy][sx].co = sco;
				}
				if( aif_field[y-1][x].co == aif_field[y][x].co ) f = TRUE;
				z = aifSearchLineMS(af,x,y,mco,x,y-1,sco,f);
			} else {
				if( aif_field[y+1][x].st != MB_NOTHING ) {
					mx = x;
					my = y;
					mst = MB_CAPS_L;
					sx = x+1;
					sy = y;
					sst = MB_CAPS_R;
					if( af->rev == FALSE ) {
						mco = aiNext.ca;
						sco = aiNext.cb;
					} else {
						mco = aiNext.cb;
						sco = aiNext.ca;
					}
				} else {
					mx = x+1;
					my = y;
					mst = MB_CAPS_R;
					sx = x;
					sy = y;
					sst = MB_CAPS_L;
					if( af->rev == FALSE ) {
						mco = aiNext.cb;
						sco = aiNext.ca;
					} else {
						mco = aiNext.ca;
						sco = aiNext.cb;
					}
				}
				if( mco == sco ) f = TRUE;
				if ( my ) {
					aif_field[my][mx].st = mst;
					aif_field[my][mx].co = mco;
				}
				if ( sy ) {
					aif_field[sy][sx].st = sst;
					aif_field[sy][sx].co = sco;
				}
				z = aifSearchLineMS(af,mx,my,mco,sx,sy,sco,f);
			}
			if (z && RensaP) {													// ��߾ق������鎞�R��ނ̏����Ȃ����̐F�ŘA������
				if ( z == 2 ) {
					rco = mx;mx = sx;sx = rco;
					rco = my;my = sy;sy = rco;
					rco = mco;mco = sco;sco = rco;
					rco = mst;mst = sst;sst = rco;
				}
				rcr = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
				aiHiErR = aiHiEraseCtr;
				rcy = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
				aiHiErY = aiHiEraseCtr;
				rcb = aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
				aiHiErB = aiHiEraseCtr;
				rco = FALSE;
				if ( mx == sx ) {
					if ( my > sy ) {
						mst = MB_CAPS_R;
						sx--;
						sy++;
						sst = MB_CAPS_L;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
						mst = MB_CAPS_L;
						sx += 2;
						sst = MB_CAPS_R;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
					} else {
						mst = MB_CAPS_R;
						sx--;
						sy--;
						sst = MB_CAPS_L;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
						mst = MB_CAPS_L;
						sx += 2;
						sst = MB_CAPS_R;
						if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
							rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
						}
					}
				} else if ( mx > sx ) {
					mst = MB_CAPS_L;
					sx += 2;
					sst = MB_CAPS_R;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
					mst = MB_CAPS_D;
					sx--;
					sy--;
					sst = MB_CAPS_U;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
				} else {
					mst = MB_CAPS_R;
					sx -= 2;
					sst = MB_CAPS_L;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
					mst = MB_CAPS_D;
					sx++;
					sy--;
					sst = MB_CAPS_U;
					if ( xpw->blk[sy][sx].st == MB_NOTHING ) {
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_RED,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_YELLOW,sst);
						rco |= aifRensaCheckCore(xpw,af,mx,my,mco,mst,sx,sy,COL_BLUE,sst);
					}
				}
				switch ( sco ) {
				case COL_RED:
					af->pri += aiHiErR * pri_point[LnHighCaps];
					if ( rcr ) {
						if ( rcy || rcb || rco ) {
							af->pri += RensaP * rcr;
						} else {
							af->pri += RensaP * rcr;
//osSyncPrintf("+ %d,%d\n",rcy|rcb,rco);
						}
					} else {
						if ( rcy || rcb || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcy|rcb,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				case COL_YELLOW:
					af->pri += aiHiErY * pri_point[LnHighCaps];
					if ( rcy ) {
						if ( rcr || rcb || rco ) {
							af->pri += RensaP * rcy;
						} else {
							af->pri += RensaP * rcy;
//osSyncPrintf("+ %d,%d\n",rcr|rcb,rco);
						}
					} else {
						if ( rcr || rcb || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcr|rcb,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				case COL_BLUE:
					af->pri += aiHiErB * pri_point[LnHighCaps];
					if ( rcb ) {
						if ( rcy || rcr || rco ) {
							af->pri += RensaP * rcb;
						} else {
							af->pri += RensaP * rcb;
//osSyncPrintf("+ %d,%d\n",rcy|rcy,rco);
						}
					} else {
						if ( rcy || rcr || rco ) {
							if ( my > 2 ) af->pri += RensaMP;
//osSyncPrintf("- %d,%d\n",rcy|rcy,rco);
						} else {
							af->pri += 0;
						}
					}
					break;
				}
			}
		} else {
			aiFlag[i].pri = NonTouch;
		}
	}

	// ��ԉ��̂�1�_����Ă���(���͂������Ӗ�)
/*	for( i=j=y=0;i<aiFlagCnt;i++ ) {
		if( aiFlag[i].pri != NonTouch && aiFlag[i].y >= y ) {
			y = aiFlag[i].y;
			j = i;
		}
	}
	aiFlag[j].pri += 1;
*/
	// �ŏ���߲�Ă͒N��
	if ( xpw->ai.aiRandFlag ) {								// �S�o�p�����v�l�̂Ƃ��S��������������Ȃ��悤�P�񂾂������������鏈��
		for( i=z=0,x=NonTouch-1;i<aiFlagCnt;i++ ) {
			if ( aiPriOfs ) {
				y = RAND(aiPriOfs);
			} else {
				y = 0;
			}
			if( aiFlag[i].pri + y > x ) {
				x = aiFlag[i].pri + y;
				z = i;
			}
		}
	} else {
		for( i=z=0,x=t=NonTouch-1;i<aiFlagCnt;i++ ) {
			if ( aiPriOfs ) {
				y = RAND(100+aiPriOfs);
			} else {
				y = RAND(100);
			}
			if( aiFlag[i].pri + y > t ) {
				t = aiFlag[i].pri + y;
				if ( aiFlag[i].pri > x ) x = aiFlag[i].pri;
				z = i;
			}
			if ( aiFlag[z].pri < x ) xpw->ai.aiRandFlag = TRUE;
		}
	}

	y = aiFlag[z].y;
	x = aiFlag[z].x;
	t = aiFlag[z].tory;
	r = aiFlag[z].rev;
	decide = z;

}

//*** �L�����N�^�[�̐��i�ݒ� ************************
//	aiSetCharacter
//in:	game_state	xpw			��ڲ԰�\����
//		AI_FIELD	aiFieldData	m̨����
//		AI_DATA		ai_param	�v�l���Ұ�ð���
//out:	game_state	xpw			��ڲ԰�\����
//
void aiSetCharacter(game_state *xpw)
{
	int		i,j,x,y,z,f,t,r;
	u8		badLine,leading,wall;
	u8		hicaps,lowvrs;
	int		vrsMax,vrsCnt;
	u8		char_no;

	AI_DATA	(*aiDataPtr)[][8];

	vrsMax = xpw->lv;			// ��ٽ�����쐬
	if ( vrsMax > LEVLIMIT ) vrsMax = LEVLIMIT;
	vrsMax = (vrsMax+1)*4;

	// ���ς݃`�F�b�N
	// badLine = 1:�ォ������ł���ςݏグ���C��
	// badLine = 2:�ォ������ł��Ȃ��ςݏグ���C��
	badLine = 0;
	for(x = 2;x < FIELDX - 2;x++) {
		i = 0;
		for(y = 1;y < BADLINE;y++) {
			if ( !i ) {
				if ( aiFieldData[y][x].st != MB_NOTHING ) {
					i = 1;
					j = aiFieldData[y][x].co;
				}
			}
			if ( i ) {
				if ( (aiFieldData[y+1][x].st != MB_NOTHING && aiFieldData[y+1][x].co != j) || (aiFieldData[y+1][x].st == MB_NOTHING) ) {
					if ( (x == 3) || (x == 4) ) i = 2;
					else i = 0;
					break;
				}
			}
		}
		if ( i ) {
			badLine = i;
			if ( i == 2 ) break;
		}
	}

	// �ǃ`�F�b�N�ivrsCnt�ɕǓ��̳�ٽ�����v�Z�j
	wall = 0;
	hicaps = FIELDY;
	vrsCnt = 0;
	for(x = 4;x < FIELDX;x++) {
		for(y = 1;y < 4;y++) {
			if ( aiFieldData[y][x].st != MB_NOTHING ) {
				break;
			}
		}
		if ( y < 4 ) {
			wall |= 2;
			j = x;
			break;
		} else {
			for(;y < FIELDY;y++) {
				if ( aiFieldData[y][x].st != MB_NOTHING && hicaps > y ) hicaps = y;
				if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
					vrsCnt++;
				}
			}
		}
	}
	for(x = 3;x >= 0;x--) {
		for(y = 1;y < 4;y++) {
			if ( aiFieldData[y][x].st != MB_NOTHING ) {
				break;
			}
		}
		if ( y < 4 ) {
			wall |= 1;
			if ( j - x <= 3 ) {
				wall = 0;
				vrsCnt = xpw->vs;
			}
			break;
		} else {
			for(;y < FIELDY;y++) {
				if ( aiFieldData[y][x].st != MB_NOTHING && hicaps > y ) hicaps = y;
				if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
					vrsCnt++;
				}
			}
		}
	}

	// ��ԉ��ɂ��鳨ٽ�̈ʒu
	lowvrs = 0;
	for(x = 0;x < FIELDX;x++) {
		for(y = FIELDY;y > 3;y--) {
			if ( MB_VIRUS_A <= aiFieldData[y][x].st && aiFieldData[y][x].st <= MB_VIRUS_C ) {
				if ( lowvrs < y ) lowvrs = y;
				break;
			}
		}
	}

	// ���ݎ������g�b�v���ǂ����H
	leading = TRUE;
	if ( evs_gamesel == GSL_VSCPU ) {
		for(i=0;i<evs_playcnt;i++) {
			if ( game_state_data[i].vs < game_state_data[aipn].vs ) leading = FALSE;
		}
	}

	// �L�����ʎv�l�I��
	//�m�R�m�R		������		��			�U�������Ǝ��̑��삪�����~�܂�
	//�{����		������		����������	�U�������ƈ����ԑ����Ȃ�
	//�v�N�v�N		������		����		���܂ɘA����_���Ă���
	//�`�����v�[	��A������	��			�A���U���������ԑ����Ȃ�
	//������Ͳΰ	������		������		����J�v�Z�������邭���
	//�n�i�����	������		������		�Q�A���ō~�炳���Ɠ{���čU�����x�����オ��
	//�e���T		���A������	����������	�U�������ƒx���Ȃ�A����Ȃ��Ƃ������������Ȃ�
	//�߯����ܰ		������		������		�U�������ƘA����_���Ă���
	//�J���b�N		���A���_��	������		�A���U�����̂ݍ����ňړ��A�I�՘A�����Ȃ��Ȃ�B
	//�N�b�p		���A������	��������	HARD�̂ݒ��Ոȍ~���R�������Ԃ��Ȃ��Ȃ�A���肪�シ���i��ٽ���j�Œx���Ȃ�
	//�s�[�`		���A���_��	��������	���肪�����ςݏグ��ƘA�������Ȃ��Ȃ�
	//�}���I		���A������	������

	aiDataPtr = &ai_param;

	char_no = xpw -> anime.cnt_charcter_no;
	if ( aipn == 0 && evs_gamesel <= GSL_VSCPU ) {		// GSL_1PLAY,GSL_2PLAY,GSL_4PLAY,GSL_VSCPU
		char_no = aiDebugP1;
		if ( char_no > CHR_PLAIN4 ) {
			char_no -= CHR_PLAIN4 + 1;
			aiDataPtr = &ai_param_org;
		}
	}

	xpw->ai.aiRollHabit &= 0x01;
	xpw->ai.aiPriOfs = 0;
	aiPriOfs = 0;
	aiSelSpeedRensa = -1;

	switch(char_no) {
	case CHR_NOKO:
		aiPriOfs = 600;						// ������������
		aiSelCom = 0;
		aiSelSpeed = SPD_VERYSLOW;
		// �U�������Ǝ��̑��삪�����~�܂�
		xpw->ai.aiState &= ~AIF_AGAPE;
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiState |= AIF_AGAPE;
			xpw->ai.aiTimer = 2 + genrand(4);
		}
		break;
	case CHR_BOMB:
		aiPriOfs = 600;						// ������������
		aiSelCom = 0;
		// �U�������ƈ����ԑ����Ȃ�
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelSpeed = SPD_VERYSLOW;
			break;
		case 1:
			aiSelSpeed = SPD_MAX;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_PUKU:
		// ���܂ɘA����_���Ă���
		if ( xpw->ai.aiNum == 0 && xpw->vs < vrsMax*75/100 && xpw->vs > vrsMax*25/100 && genrand(10) == 0 ) {
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(4);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = 0;
			aiSelSpeed = SPD_SLOW;
			break;
		case 1:
			aiSelCom = 2;
			aiSelSpeed = SPD_SLOW;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_CHOR:
		// �A���U��������ԑ����Ȃ�
		if ( xpw->ai.aiFlagDecide.rensa ) {
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 3 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = 1;
			aiSelSpeed = SPD_SLOW;
			break;
		case 1:
			aiSelCom = 1;
			aiSelSpeed = SPD_VERYFAST;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_PROP:
		// �����߾ق𖳈Ӗ��ɉ�
		xpw->ai.aiRollHabit |= 0x02;
		aiSelCom = 0;
		aiSelSpeed = SPD_FAST;
		break;
	case CHR_HANA:
		// �Q�A���ō~�炳���Ɠ{���čU�����x�����オ��
		xpw->ai.aiState &= ~AIF_DAMAGE;
		if ( xpw->ai.aiState & AIF_DAMAGE2 ) {
			xpw->ai.aiState &= ~AIF_DAMAGE2;
			xpw->ai.aiNum++;
			if ( xpw->ai.aiNum > 3 ) xpw->ai.aiNum = 3;
		}
		aiSelCom = xpw->ai.aiNum;
		aiSelSpeed = SPD_FAST;
		break;
	case CHR_TERE:
		// �U�������ƒx���Ȃ餂���Ă��Ȃ��Ə����������Ȃ�
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 0;
		}
		aiSelCom = 2;
		aiSelSpeed = xpw->ai.aiNum / 5 + SPD_VERYSLOW;
		if ( aiSelSpeed > SPD_MAX ) aiSelSpeed = SPD_MAX;
		xpw->ai.aiNum++;
		break;
	case CHR_PACK:
		// �U�������ƘA����_���Ă���
		if ( xpw->ai.aiState & AIF_DAMAGE ) {
			xpw->ai.aiState &= ~AIF_DAMAGE;
			xpw->ai.aiNum = 1;
			xpw->ai.aiTimer = 5 + genrand(5);
		}
		switch(xpw->ai.aiNum) {
		case 0:
			aiSelCom = 0;
			aiSelSpeed = SPD_FAST;
			break;
		case 1:
			aiSelCom = 3;
			aiSelSpeed = SPD_FAST;
			xpw->ai.aiTimer--;
			if ( xpw->ai.aiTimer == 0 ) xpw->ai.aiNum = 0;
			break;
		}
		break;
	case CHR_KAME:
		// �I�՘A����_��Ȃ��Ȃ�
		if ( xpw->vs <= 8 ) {
			aiSelCom = 0;
			aiSelSpeed = SPD_FAST;
		} else {
			aiSelCom = 3;
			aiSelSpeed = SPD_FAST;
		}
		// �A���U�����ő��ŗ���
		aiSelSpeedRensa = SPD_FLASH;
		break;
	case CHR_KUPP:
		// �㔼�͏W���͂����܂莩�R�������Ԃ������Ȃ�(HARD�̂�)
		if ( xpw->vs <= vrsMax/2 && xpw->ai.aivl == 2 ) {
			aiSelCom = 2;
			aiSelSpeed = SPD_VERYFASTNW;
		} else {
			aiSelCom = 2;
			aiSelSpeed = SPD_VERYFAST;
		}
		// ���肪�シ���i��ٽ���j�Œx���Ȃ�(VSCPU�̂�)
		if ( evs_gamesel == GSL_VSCPU && game_state_data[1 - aipn].vs - 15 >= xpw->vs ) {
			aiSelSpeed = SPD_VERYSLOW;
		}
		break;
	case CHR_PEACH:
		aiSelSpeed = SPD_VERYFAST;
		// ���Ոȍ~���A�������ɕς��(HARD�̂�)
		if ( xpw->vs <= vrsMax/2 && xpw->ai.aivl == 2 ) {
			aiSelCom = 2;
		} else {
			aiSelCom = 3;
		}
		// ���肪�����ςݏグ��ƘA�������Ȃ��Ȃ�
		if ( evs_gamesel == GSL_VSCPU && game_state_data[1 - aipn].ai.aiRootP < 5 ) {
			aiSelCom = 0;
		}
		break;
	case CHR_MARIO:
		aiSelCom = 2;
		aiSelSpeed = SPD_FAST;
		break;
	case CHR_PLAIN1:										// �f�o�b�O�p
		aiSelCom = 0;
		aiSelSpeed = SPD_VERYFASTNW;
		break;
	case CHR_PLAIN2:
		aiSelCom = 1;
		aiSelSpeed = SPD_VERYFASTNW;
		break;
	case CHR_PLAIN3:
		aiSelCom = 2;
		aiSelSpeed = SPD_VERYFASTNW;
		break;
	case CHR_PLAIN4:
		aiSelCom = 3;
		aiSelSpeed = SPD_VERYFASTNW;
		break;
	}

	// ��Ԑݒ�
	switch(aiSelCom) {
	case 0:					// ������
		if ( xpw->gs != AGST_GAME ) {								// ���񂾌�̗��K���[�h
			i = AIDT_DEAD;
		} else {
			if ( aiRootP < 4.0 ) {
				i = AIDT_NARROW;
			} else if ( xpw->vs <= 6 || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					if ( vrsCnt ) {
						i = AIDT_LASTVS;
					} else {
						i = AIDT_LASTVSNH;
					}
				} else {
					i = AIDT_LASTVSN;
				}
			} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				i = AIDT_BADLINE2;
			} else {											// ���Տ���
				i = AIDT_NORMAL;
			}
		}
		break;
	case 1:					// ��A��
	case 2:					// ���A������
	case 3:					// ���A���_��
		if ( xpw->gs != AGST_GAME ) {								// ���񂾌�̗��K���[�h
			i = AIDT_DEAD;
		} else {
			if ( aiRootP < 4.0 ) {
				i = AIDT_NARROW;
			} else if ( (xpw->vs <= 6 && leading) || vrsCnt <= 2 ) {					// �����̳�ٽ�����W�ȉ��ɂȂ������I�Տ���
				if ( vrsCnt || (hicaps < lowvrs - 4) || (hicaps < FIELDY - 8) ) {
					if ( vrsCnt ) {
						i = AIDT_LASTVS;
					} else {
						i = AIDT_LASTVSNH;
					}
				} else {
					i = AIDT_LASTVSN;
				}
			} else if ( badLine == 2 ) {								// ��߾ٓ�������ɶ�߾ق��c���Ă��鎞
				i = AIDT_BADLINE2;
			} else {											// ���Տ���
				if ( badLine == 1 ) {
					i = AIDT_BADLINE1;
				} else {
					i = AIDT_NORMAL;
				}
			}
		}
		break;
	}

//if (aipn == 0) osSyncPrintf("selcom mode %d\n",i);

	// �p�����[�^���[�N�ݒ�
	pri_point[LnOnLinVrs] = (*aiDataPtr)[aiSelCom][i].dt_LnOnLinVrs;
	pri_point[LnEraseVrs] = (*aiDataPtr)[aiSelCom][i].dt_LnEraseVrs;
	EraseLinP[1] = (*aiDataPtr)[aiSelCom][i].dt_EraseLinP1;
	HeiEraseLinRate = (float)(*aiDataPtr)[aiSelCom][i].dt_HeiEraseLinRate / 100.0;
	WidEraseLinRate = (float)(*aiDataPtr)[aiSelCom][i].dt_WidEraseLinRate / 100.0;
	AloneCapP[ALN_FallCap] = (*aiDataPtr)[aiSelCom][i].dt_P_ALN_FallCap;
	AloneCapP[ALN_FallVrs] = (*aiDataPtr)[aiSelCom][i].dt_P_ALN_FallVrs;
	AloneCapP[ALN_Capsule] = (*aiDataPtr)[aiSelCom][i].dt_P_ALN_Capsule;
	AloneCapP[ALN_Virus] = (*aiDataPtr)[aiSelCom][i].dt_P_ALN_Virus;
	AloneCapWP[ALN_Bottom] = (*aiDataPtr)[aiSelCom][i].dt_WP_ALN_Bottom;
	AloneCapWP[ALN_FallCap] = (*aiDataPtr)[aiSelCom][i].dt_WP_ALN_FallCap;
	AloneCapWP[ALN_FallVrs] = (*aiDataPtr)[aiSelCom][i].dt_WP_ALN_FallVrs;
	AloneCapWP[ALN_Capsule] = (*aiDataPtr)[aiSelCom][i].dt_WP_ALN_Capsule;
	AloneCapWP[ALN_Virus] = (*aiDataPtr)[aiSelCom][i].dt_WP_ALN_Virus;
	LPriP = (*aiDataPtr)[aiSelCom][i].dt_P_ALN_HeightP;
	OnVirusP = (*aiDataPtr)[aiSelCom][i].dt_OnVirusP;
	RensaP = (*aiDataPtr)[aiSelCom][i].dt_RensaP;
	RensaMP = (*aiDataPtr)[aiSelCom][i].dt_RensaMP;
	pri_point[LnHighCaps] = (*aiDataPtr)[aiSelCom][i].dt_LnHighCaps;
	pri_point[LnHighVrs] = (*aiDataPtr)[aiSelCom][i].dt_LnHighVrs;
	HeiLinesAllp[2] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp2;
	HeiLinesAllp[3] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp3;
	HeiLinesAllp[4] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp4;
	HeiLinesAllp[5] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp5;
	HeiLinesAllp[6] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp6;
	HeiLinesAllp[7] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp7;
	HeiLinesAllp[8] = (*aiDataPtr)[aiSelCom][i].dt_HeiLinesAllp8;
	WidLinesAllp[2] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp2;
	WidLinesAllp[3] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp3;
	WidLinesAllp[4] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp4;
	WidLinesAllp[5] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp5;
	WidLinesAllp[6] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp6;
	WidLinesAllp[7] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp7;
	WidLinesAllp[8] = (*aiDataPtr)[aiSelCom][i].dt_WidLinesAllp8;
	aiWall = 0;
	if ( (*aiDataPtr)[aiSelCom][i].dt_aiWall_F ) aiWall = wall;
}


//*** ̨�����ް��̍Đݒ�i�R�s�[�j ************************
//	aifFieldCopy
//in:	game_state	xpw			��ڲ԰�\����
//out:	AI_FIELD	aiFieldData	m̨����
//		AI_FIELD	aiRecurData	m̨����
//
void aifFieldCopy(game_state* uupw)
{
	int	idx,idy;
	game_state*	upw;

	upw = uupw;

	/*----- next capsule data copy -----*/
	aiNext.ca = upw->cap.ca;
	aiNext.cb = upw->cap.cb;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		// for search
		aiRecurData[idy][idx+1].co = aiFieldData[idy][idx].co = upw->blk[idy][idx].co;	// �F����߰
		aiRecurData[idy][idx+1].st = aiFieldData[idy][idx].st = upw->blk[idy][idx].st;	// �ð������߰
	}}

//	aifRecurCopy();

	/*----- �ċA�����p�ް� �ǂň͂� -----*/
	for ( idx=0 ; idx<FIELDX+2 ; idx++ ) {	// ����(���̂݁j
		aiRecurData[FIELDY][idx].co = WALL;
		aiRecurData[FIELDY][idx].st = WALL;
	}
	for ( idy=0 ; idy<FIELDY+1 ; idy++ ) {	// �c��(���E)
		aiRecurData[idy][0].co = WALL;
		aiRecurData[idy][0].st = WALL;
		aiRecurData[idy][FIELDX+1].co = WALL;
		aiRecurData[idy][FIELDX+1].st = WALL;
	}
}

//*** ٰČ�����p̨�����ް��̍Đݒ�i�R�s�[�j ************************
//	aifRecurCopy
//in:	AI_FIELD	aiFieldData	m̨����
//out:	AI_FIELD	aiRecurData	m̨����
//
void aifRecurCopy(void)
{
	int	idx,idy;

	/*----- field data copy -----*/
	for ( idx=0 ; idx<FIELDX ; idx++ ) {
	for ( idy=0 ; idy<FIELDY ; idy++ ) {
		aiRecurData[idy][idx+1].co = aiFieldData[idy][idx].co;	// (�ċA�����p)�F����߰
		aiRecurData[idy][idx+1].st = aiFieldData[idy][idx].st;	// (�ċA�����p)�ð������߰
	}}
}



//*** ��߾ق�u����ꏊ�̌��� ************************
//	aifPlaceSearch
//in:	AI_FIELD	aiRecurData	m̨����
//out:	AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			aiFlagCnt	��߾ق�u����ʒu�̐�
//
void aifPlaceSearch(void)
{
	int		idx,idy;
	int		cnt;
	AI_EDGE	aiEdge[100];

	cnt = 0;						// counter reset
	aiTEdgeCnt = aiYEdgeCnt = 0;	// edge counter reset

	/*----- �c�u����� -----*/
	for ( idy=1 ; idy<FIELDY ; idy++ ) {
	for ( idx=1 ; idx<FIELDX+1 ; idx++ ) {
		if ( aiRecurData[idy][idx].co == COL_NOTHING ) {			// �Ȃɂ��Ȃ��H
			if ( aiRecurData[idy+1][idx].co != COL_NOTHING ) {		// ���Ȃɂ�����H
				if ( aiRecurData[idy-1][idx].co == COL_NOTHING ) {	// ��Ȃɂ��Ȃ��H
					aiEdge[cnt].tory = tate;
					aiEdge[cnt].x	 = idx;
					aiEdge[cnt].y	 = idy;
					cnt++;
					aiTEdgeCnt++;		// �c�u���\���ސ�
				}
			}
		}
	}}

	/*----- ���u����� -----*/
	for ( idy=1 ; idy<FIELDY ; idy++ ) {
	for ( idx=1 ; idx<FIELDX ; idx++ ) {
		if ( aiRecurData[idy][idx].co == COL_NOTHING ) {			// �Ȃɂ��Ȃ��H
			if ( aiRecurData[idy+1][idx].co != COL_NOTHING ||		// ��or�E���Ȃɂ�����H
				 aiRecurData[idy+1][idx+1].co != COL_NOTHING ) {
				if ( aiRecurData[idy][idx+1].co == COL_NOTHING ) {		// �E�Ȃɂ��Ȃ��H
					aiEdge[cnt].tory = yoko;
					aiEdge[cnt].x	 = idx;
					aiEdge[cnt].y	 = idy;
					cnt++;
					aiYEdgeCnt++;		// ���u���\���ސ�
				}
			}
		}
	}}

	/*----- �׸��ް� ��� -----*/
	cnt = 0;
	for ( idx=0 ; idx<aiTEdgeCnt ; idx++ ) {			// �c�u���@���]����
		aiFlag[cnt].tory = tate;
		aiFlag[cnt].x	 = aiEdge[idx].x;
		aiFlag[cnt].y	 = aiEdge[idx].y;
		aiFlag[cnt].rev	 = 0;
		cnt++;
	}
	if( aiNext.ca != aiNext.cb ) {
		for ( idx=0 ; idx<aiTEdgeCnt ; idx++ ) {			// �c�u���@���]�L��
			aiFlag[cnt].tory = tate;
			aiFlag[cnt].x	 = aiEdge[idx].x;
			aiFlag[cnt].y	 = aiEdge[idx].y;
			aiFlag[cnt].rev	 = 1;
			cnt++;
		}
	}
	for ( idx=0 ; idx<aiYEdgeCnt ; idx++ ) {			// ���u���@���]����
		aiFlag[cnt].tory = yoko;
		aiFlag[cnt].x	 = aiEdge[aiTEdgeCnt+idx].x;
		aiFlag[cnt].y	 = aiEdge[aiTEdgeCnt+idx].y;
		aiFlag[cnt].rev	 = 0;
		cnt++;
	}
	if( aiNext.ca != aiNext.cb ) {
		for ( idx=0 ; idx<aiYEdgeCnt ; idx++ ) {			// ���u���@���]�L��
			aiFlag[cnt].tory = yoko;
			aiFlag[cnt].x	 = aiEdge[aiTEdgeCnt+idx].x;
			aiFlag[cnt].y	 = aiEdge[aiTEdgeCnt+idx].y;
			aiFlag[cnt].rev	 = 1;
			cnt++;
		}
	}
	aiFlagCnt = cnt;		// �׸��ް���

}



//*** ��߾ق�u���鏊����ړ��J�n�ʒu�܂ňړ��\���� ************************
//	aifMoveCheck
//in:	AI_FIELD	aiRecurData	m̨����
//		AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			aiFlagCnt	��߾ق�u����ʒu�̐�
//out:	AI_FLAG		aiFlag[].ok	�ړ��\���ǂ����t���O(1:�����0:���s)
//		float		aiRootP		�S�ړ��ʂ̕��ϒl�i�ړ��͈͂���������̂��������邽�߂�ܰ��j
//
void aifMoveCheck(void)
{
	int	cnt,okctr,okrootctr;
	int	idx,idy;
	int	i;

	for ( cnt=okctr=okrootctr=0 ; cnt<aiFlagCnt ; cnt++ ) {
		for(i = 0;i < FIELDX+2;i++) {
			aiRecurData[0][i].co = WALK;
		}
		aifRecurCopy();
		aiRootCnt = 0;
		aiRollFinal = 0;
		success = 0;
		if ( aiFlag[cnt].tory == tate )	aifTRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
		else							aifYRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt);
		aiFlag[cnt].ok = success;
		if ( (!success) && (aiSelCom > 1) ) {
			aiRollFinal = 1;
			if ( aiFlag[cnt].tory == tate )	{
				if ( aiFlag[cnt].x < 7 && aiRecurData[aiFlag[cnt].y][aiFlag[cnt].x+1].st == MB_NOTHING ) {
					aifYRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
				}
			} else {
				if ( aiRecurData[aiFlag[cnt].y-1][aiFlag[cnt].x].st == MB_NOTHING ) {
					aifTRecur(aiFlag[cnt].x , aiFlag[cnt].y, cnt );
				} else if ( aiRecurData[aiFlag[cnt].y-1][aiFlag[cnt].x+1].st == MB_NOTHING && (aiRecurData[aiFlag[cnt].y][aiFlag[cnt].x+2].st != MB_NOTHING || aiFlag[cnt].x == 6) ) {
					aifTRecur(aiFlag[cnt].x+1 , aiFlag[cnt].y, cnt );
				}
			}
			aiFlag[cnt].ok = success;
		}
		if ( success ) {
			okctr++;
			okrootctr += aiRootCnt;
		}
	}

	if ( okctr ) aiRootP = (float)okrootctr / (float)okctr;
	else aiRootP = 0.0;
}

//*** �c�u����߾� �ړ��\��� ************************
//	aifTRecur
//in:	u8			x,y			̨���ވړ��������̌��ݒn
//		u8			cnt			���݌������Ă����߾ق�u����ʒu��ٰď��̔ԍ�
//		AI_FIELD	aiRecurData	m̨����
//out:	u8			success		1:�o�H����������������
//		AI_ROOT		aiRoot		�������ړ��o�H�ۑ�
//		u8			aiRootCnt	�ړ��o�H�ۑ�����
//
void aifTRecur(u8 x,u8 y,u8 cnt)
{
	aiRecurData[y][x].co = WALK;

	if ( x==GOALX && y==GOALY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-2][x].st==MB_NOTHING )
		aifTRecur(x,y-1,cnt);
	if ( success!=1 && aiRecurData[y][x+1].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifTRecur(x+1,y,cnt);
		else aifTRecurUP(x+1,y,cnt);
	}
	if ( success!=1 && aiRecurData[y][x-1].co==COL_NOTHING && aiRecurData[y-1][x-1].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifTRecur(x-1,y,cnt);
		else aifTRecurUP(x-1,y,cnt);
	}

	//---- ٰċL�^ -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;

		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");

	}
}

//*** �c�u����߾� �ړ��\���(��ړ���p) ************************
//	aifTRecurUP
//in:	u8			x,y			̨���ވړ��������̌��ݒn
//		u8			cnt			���݌������Ă����߾ق�u����ʒu��ٰď��̔ԍ�
//		AI_FIELD	aiRecurData	m̨����
//out:	u8			success		1:�o�H����������������
//		AI_ROOT		aiRoot		�������ړ��o�H�ۑ�
//		u8			aiRootCnt	�ړ��o�H�ۑ�����
//
void aifTRecurUP(u8 x,u8 y,u8 cnt)
{
//	aiRecurData[y][x].co = WALK;

	if ( x==GOALX && y==GOALY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-2][x].st==MB_NOTHING )
		aifTRecur(x,y-1,cnt);

	//---- ٰċL�^ -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;

		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");

	}
}

//*** ���u����߾� �ړ��\��� ************************
//	aifYRecur
//in:	u8			x,y			̨���ވړ��������̌��ݒn
//		u8			cnt			���݌������Ă����߾ق�u����ʒu��ٰď��̔ԍ�
//		AI_FIELD	aiRecurData	m̨����
//out:	u8			success		1:�o�H����������������
//		AI_ROOT		aiRoot		�������ړ��o�H�ۑ�
//		u8			aiRootCnt	�ړ��o�H�ۑ�����
//
void aifYRecur(u8 x,u8 y,u8 cnt)
{
	aiRecurData[y][x].co = WALK;

	if ( x==GOALX && y==GOALY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING )
		aifYRecur(x,y-1,cnt);
	if ( success!=1 && aiRecurData[y][x+1].co==COL_NOTHING && aiRecurData[y][x+2].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifYRecur(x+1,y,cnt);
		else aifYRecurUP(x+1,y,cnt);
	}
	if ( success!=1 && aiRecurData[y][x-1].co==COL_NOTHING && aiRecurData[y][x].st==MB_NOTHING ) {
		if ( aiMoveSF ) aifYRecur(x-1,y,cnt);
		else aifYRecurUP(x-1,y,cnt);
	}


	//---- ٰċL�^ -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;

		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");

	}
}

//*** ���u����߾� �ړ��\���(��ړ���p) ************************
//	aifYRecurUP
//in:	u8			x,y			̨���ވړ��������̌��ݒn
//		u8			cnt			���݌������Ă����߾ق�u����ʒu��ٰď��̔ԍ�
//		AI_FIELD	aiRecurData	m̨����
//out:	u8			success		1:�o�H����������������
//		AI_ROOT		aiRoot		�������ړ��o�H�ۑ�
//		u8			aiRootCnt	�ړ��o�H�ۑ�����
//
void aifYRecurUP(u8 x,u8 y,u8 cnt)
{
//	aiRecurData[y][x].co = WALK;

	if ( x==GOALX && y==GOALY ) success = 1;

	if ( success!=1 && aiRecurData[y-1][x].co==COL_NOTHING && aiRecurData[y-1][x+1].st==MB_NOTHING )
		aifYRecur(x,y-1,cnt);

	//---- ٰċL�^ -----
	if ( success==1 ) {
//		osSyncPrintf("(%d,%d)",x,y );
		aiRoot[aiRootCnt].x = x;
		aiRoot[aiRootCnt].y = y;
		aiRootCnt++;

		if ( aiRootCnt >= ROOTCNT ) osSyncPrintf("*");

	}
}

//*** �����ʒu����������xٰČ��� ************************
//	aifReMoveCheck
//in:	AI_FIELD	aiRecurData	m̨����
//		AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			aiFlagCnt	��߾ق�u����ʒu�̐�
//		u8			decide		�ŏI�I���肷���׸��ް��ԍ�
//out:	AI_FLAG		aiFlag[].ok	�ړ��\���ǂ����t���O(1:�����0:���s)
//		float		aiRootP		�S�ړ��ʂ̕��ϒl�i�ړ��͈͂���������̂��������邽�߂�ܰ��j
//
void aifReMoveCheck(void)
{
	int	idx;

	for ( idx=0 ; idx<ROOTCNT ; idx++ ) {
		aiRoot[idx].x=0;
		aiRoot[idx].y=0;
	}
	aiRootCnt = 0;
	aiRollFinal = 0;
	aiMoveSF = 1;			// �Č������͈ړ��ʂ��ő�ɂ������̂�
	success = 0;
	aifRecurCopy();
	if ( aiFlag[decide].tory == tate )	aifTRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
	else							aifYRecur(aiFlag[decide].x , aiFlag[decide].y, decide);
	if ( !success ) {
		aiRollFinal = 1;
		aifRecurCopy();
		if ( aiFlag[decide].tory == tate )	{
			if ( aiRecurData[aiFlag[decide].y][aiFlag[decide].x+1].st == MB_NOTHING ) {
				aifYRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
			}
		} else {
			if ( aiRecurData[aiFlag[decide].y-1][aiFlag[decide].x].st == MB_NOTHING ) {
				aifTRecur(aiFlag[decide].x , aiFlag[decide].y, decide );
			} else if ( aiRecurData[aiFlag[decide].y-1][aiFlag[decide].x+1].st == MB_NOTHING && (aiRecurData[aiFlag[decide].y][aiFlag[decide].x+2].st != MB_NOTHING || aiFlag[decide].x == 6) ) {
				aifTRecur(aiFlag[decide].x+1 , aiFlag[decide].y, decide );
			}
		}
	}
	aiRoot[aiRootCnt].x = aiRoot[aiRootCnt-1].x;
	aiRoot[aiRootCnt].y = aiRoot[aiRootCnt-1].y+1;
}


/************************************************
	�L�[���쐬
************************************************/

//*** �ړ��ʒu�����L�[���쐬 ************************
//	aifKeyMake
//in:	game_state	*uupw		��ڲ԰�\�����߲��
//		AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			decide		�ŏI�I���肷���׸��ް��ԍ�
//		s8			aiSelSpeedRensa		�b�n�l�̑���X�s�[�h���x�ԍ��i�A���o���鎞������߰�ނ�ς������Ƃ��g�p�j
//out:	AIWORK		uupw->ai	�e��������
//		s8			aiSelSpeed	�b�n�l�̑���X�s�[�h���x�ԍ�
//
void aifKeyMake(game_state* uupw)
{
	int	cnt;
	int	roll[2][2] = {{ 1,3 },{ 0,2 }};
	u8	lev;

	//----- �v�l�׸ނ̕ۑ� -----
	uupw->ai.aiFlagDecide = aiFlag[decide];

//if (aipn == 0) osSyncPrintf("pri = %d\n",aiFlag[decide].pri);

	for ( cnt=0 ; cnt<ROOTCNT ; cnt++ ) {
		uupw->ai.aiRootDecide[cnt].x=aiRoot[cnt].x;
		uupw->ai.aiRootDecide[cnt].y=aiRoot[cnt].y;
	}
	uupw->ai.aiEX = aiRoot[aiRootCnt-1].x;
	uupw->ai.aiEY = aiRoot[aiRootCnt-1].y;

if (aipn == 0) {
//	osSyncPrintf("%d,%d,%d\n",uupw->ai.aiEY,aiRootCnt,decide);
//	osSyncPrintf("ai0:%d %d,%d\n",aiFlag[decide].pri,aiNext.ca,aiNext.cb);
}

	//----- �A���`�F�b�N -----
	if ( uupw->ai.aiFlagDecide.rensa = aifRensaCheck(uupw,&(uupw->ai.aiFlagDecide)) && aiSelSpeedRensa != -1 )	{
		aiSelSpeed = aiSelSpeedRensa;
	}

	//----- ��]���쐬 -----
	uupw->ai.aiRollCnt = (40 + roll[uupw->ai.aiFlagDecide.tory][uupw->ai.aiFlagDecide.rev] - uupw->ai.aiOldRollCnt + uupw->ai.aiRollCnt - aiRollFinal + uupw->ai.aiRollFinal ) % 4;
	uupw->ai.aiOldRollCnt = uupw->ai.aiRollCnt;
	uupw->ai.aiRollFinal = aiRollFinal;

	//----- �ړ���߰���׸ސݒ� -----
	uupw->ai.aiSelSpeed = aiSelSpeed;
	uupw->ai.aiSpeedCnt = 2;
	uupw->ai.aiKRFlag = 1;
	uupw->ai.aiRootP = (aiRootP > 100)?100:(u8)aiRootP;

	lev = genrand(aiVirusLevel[aiSelSpeed][uupw->ai.aivl]);

//if ( aipn == 0 ) osSyncPrintf(" %d %d %d \n",aiSelCom,aiSelSpeed,uupw->ai.aivl);

	if ( lev>6) {
		uupw->ai.aiSpUpFlag = FALSE;
	} else {
		uupw->ai.aiSpUpFlag = TRUE;
		uupw->ai.aiSpUpStart = RAND(5);
	}
}

/***********************************************
	�L�[���o��
***********************************************/

//*** �b�n��������o�� ************************
//	aifKeyOut
//in:	game_state	*uupw		��ڲ԰�\�����߲��
//		AIWORK		uupw->ai	�e��������
//out:	u16			joygam		���۰װ�d�l�̷����o��
//
void aifKeyOut(game_state* uupw)
{
	s8	idx,idy;

	aifMakeWork(uupw);

	joygam[uupw->pn] = 0;
	if ( uupw->ai.aiok == FALSE ) return;
	if ( uupw->cap.my == 0 ) return;

	if ( uupw->cap.mx==uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].x-1 && uupw->cap.my==uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y ) uupw->ai.aiKeyCount++;
	uupw->ai.aiSpeedCnt--;

	idx = (uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].x-1) - uupw->cap.mx;
	idy = uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y - uupw->cap.my;

	if ( uupw->ai.aiState & AIF_AGAPE ) {
		if ( uupw->cap.my >= uupw->ai.aiTimer ) {
			uupw->ai.aiState &= ~AIF_AGAPE;
			uupw->ai.aiSpeedCnt = 0;
		}
	}


	if ( (uupw->ai.aiSpeedCnt == 0) && ((uupw->ai.aiState & AIF_AGAPE) == 0) ) {
		if ( uupw->ai.aiRollCnt == 0 && uupw->ai.aiRollHabit & 0x02 && ((genrand(20) == 0 && uupw->cap.my < uupw->ai.aiEY - 3) || (uupw->ai.aiRollHabit & 0x01 != 0)) ) {
			uupw->ai.aiRollCnt += 2;
			uupw->ai.aiRollHabit ^= 1;
		}
		if ( uupw->ai.aiRollCnt!=0 ) {						// ��߾ى�]
			if ( uupw->ai.aiRollCnt==3 ) {
				joygam[uupw->pn] = B_BUTTON;
				uupw->ai.aiRollCnt = 0;
			} else {
				joygam[uupw->pn] = A_BUTTON;
				uupw->ai.aiRollCnt--;
			}
			uupw->ai.aiSpeedCnt = ROLL_SPEED;
		}
		if ( idy<0 ) {										// ���������Ă���
			uupw->ai.aiKeyCount++;
//			uupw->ai.aiSpeedCnt = aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
			uupw->ai.aiSpeedCnt = 1;
//			uupw->ai.aiKRFlag = 0;
			if ( uupw->ai.aiEY < uupw->cap.my ) {
				uupw->ai.aiok = FALSE;
			}
		}
		if ( idx > 0 ) {										// �E�ړ�
			joygam[uupw->pn] |= R_JPAD;
			uupw->ai.aiSpeedCnt = (!uupw->ai.aiKRFlag)?aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl]:aiSlideSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
			uupw->ai.aiKRFlag++;								// ����߰�ON
		}
		if ( idx < 0 ) {										// ���ړ�
			joygam[uupw->pn] |= L_JPAD;
			uupw->ai.aiSpeedCnt = (!uupw->ai.aiKRFlag)?aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl]:aiSlideSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
			uupw->ai.aiKRFlag++;
		}
		if ( idy>0 ) {										// ����������
			if (!uupw->ai.aiKRFlag) {
				if ( (uupw->ai.aiSelSpeed == SPD_FLASH) || (uupw->ai.aiRootDecide[uupw->ai.aiKeyCount].y != uupw->ai.aiRootDecide[uupw->ai.aiKeyCount + 1].y) ) {
					if ( (uupw->ai.aiSelSpeed == SPD_MAX) || (uupw->ai.aiSelSpeed == SPD_FLASH) || (uupw->cap.my >= 3) || ((uupw->ai.aiSelSpeed >= SPD_SLOW) && (uupw->cap.my == 2) && (uupw->cap.cn > FallSpeed[uupw->cap.sp]/2 )) ) {
						if( evs_gamesel ==	GSL_VSCPU ) {
							if ( !uupw->ai.aiFlagDecide.rensa || (game_state_data[1-aipn].cap_attack_work[0] == 0) ) {
								joygam[uupw->pn] |= D_JPAD;
							}
						} else {
							joygam[uupw->pn] |= D_JPAD;
						}
					}
					if ( uupw->ai.aiSpUpFlag == TRUE ) {
						if ( uupw->ai.aiKeyCount > uupw->ai.aiSpUpStart ) {
							uupw->ai.aiSpeedCnt += VIRUSSPEEDMAX;
						} else {
							uupw->ai.aiSpeedCnt += aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
						}
					} else {
						uupw->ai.aiSpeedCnt += aiDownSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
					}
				} else {
					uupw->ai.aiSpeedCnt += VIRUSSPEEDMAX;
				}
			} else {
				uupw->ai.aiSpeedCnt += aiSlideFSpeed[uupw->ai.aiSelSpeed][uupw->ai.aivl];
				uupw->ai.aiKRFlag = 0;
			}
		}
	}
	if ( uupw->cap.mx==uupw->ai.aiEX-1 && uupw->cap.my==uupw->ai.aiEY && uupw->ai.aiRollFinal && joygam[uupw->pn] != A_BUTTON && joygam[uupw->pn] != B_BUTTON ) {
		joygam[uupw->pn] = A_BUTTON;
		uupw->ai.aiRollFinal = 0;
	}

	if ( uupw->ai.aiSelSpeed == SPD_FLASH ) uupw->ai.aiSpeedCnt = 1;

}


