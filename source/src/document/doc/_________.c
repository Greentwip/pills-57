
//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|
// �O�����珑�����܂��ܰ�

u8	aiUsedFlag;				// 1int���ɂP�񂵂��v�l�������g���Ȃ��悤�ɂ����׸�
							// 1int���ƂɃQ�[���v���O������FALSE�ɏ���������

//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|
// �`�h��p���[�N

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

// �װ (����) pw->blk[][].co, pw->cap(nex).co
enum {
	COL_RED,			// �����Œ�
	COL_YELLOW,			//	  ..
	COL_BLUE,			//	  ..
	COL_NOTHING,
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



//----------------------------------------------------------
//	�Q�[���̐���p�\����
//
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

//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|
// aiset.c �� ���[�N

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





