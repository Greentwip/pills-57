//*** �n��ܰ����v�l�pܰ��ɕϊ� ************************
//	aifMakeWork
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	uupw			��ڲ԰�\����
//		AICAPS		uupw->cap		AI�p�����߾ُ����󂯎��ܰ�
//
void aifMakeWork(game_state *uupw)

//*** �n��̨����ܰ����v�l�p̨����ܰ��ɕϊ� ************************
//	aifMakeBlkWork
//in:	game_map	*game_map_data	��ڲ԰ϯ��̨����
//out:	AIBLK		uupw->blk		m̨���ޏ�̶�߾�&��ٽ�̏��ܰ�
//
void aifMakeBlkWork(game_state *uupw)

//*** �v�l�J�n�t���O�ݒ�i�����J�n���v�l�������Ăяo���j************************
//	aifMakeFlagSet
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifMakeFlagSet( game_state *uupw )

//*** �Q�[���J�n������������ ************************
//	aifGameInit
//in:	none
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifGameInit(void)

//*** �v���O�����J�n������������ ************************
//	aifFirstInit
//in:	none
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifFirstInit(void)

//*** �b�o�t�`�h�������C���v���O���� ************************
//	aifMake
//in:	game_state	*uupw			��ڲ԰�\����
//out:	game_state	*uupw			��ڲ԰�\����
//
void aifMake(game_state* uupw)

//*** �V�䂪�߂����Ķ�߾ق�ς�ł������Ȃ���ٽ�𓃂����Ăď����ɍs������ ************************
//	aiHiruSideLineEraser
//in:	game_state	xpw				��ڲ԰�\����
//out:	game_state	xpw				��ڲ԰�\����
//
void aiHiruSideLineEraser
(
	game_state*	xpw
)

//*** �������ꂽ��߾ق̔��Α����ʂɕϊ����� ************************
//	aif_MiniChangeBall
//in:	u8			x,y			�������ꂽ��߾ق̍��W�ʒu
//		AI_FIELD	aif_field	m̨����
//out:	AI_FIELD	aif_field	m̨����
//
aif_MiniChangeBall( u8 y, u8 x )

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
)

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

//*** ���݂̑���J�v�Z����ݒ肵�ĘA���`�F�b�N�i�����Ɋ댯�]�[���̃J�v�Z���������邩�ǂ����̌v�Z���܂ށj ************************
//	aifRensaCheck
//in:	game_state	uupw		��ڲ԰�\����
//		AI_FLAG		af			�ړ���߾ق̏��
//out:	int						�����łP�A���������łQ�A�����Ȃ����͂O��Ԃ�
//		s16			aiHiEraseCtr	��̕������ꂽ������
//
int aifRensaCheck(game_state* uupw ,AI_FLAG* af)

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
)

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
)

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
)

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
)

//*** �u���ꏊ���� ************************
//	aiHiruAllPriSet
//in:	game_state	xpw			��ڲ԰�\����
//		AI_FIELD	aiFieldData	m̨����
//out:	u8			decide		�ŏI�I���肷���׸��ް��ԍ�
//
void aiHiruAllPriSet(game_state *xpw)

//*** �L�����N�^�[�̐��i�ݒ� ************************
//	aiSetCharacter
//in:	game_state	xpw			��ڲ԰�\����
//		AI_FIELD	aiFieldData	m̨����
//		AI_DATA		ai_param	�v�l���Ұ�ð���
//out:	game_state	xpw			��ڲ԰�\����
//
void aiSetCharacter(game_state *xpw)

//*** ̨�����ް��̍Đݒ�i�R�s�[�j ************************
//	aifFieldCopy
//in:	game_state	xpw			��ڲ԰�\����
//out:	AI_FIELD	aiFieldData	m̨����
//		AI_FIELD	aiRecurData	m̨����
//
void aifFieldCopy(game_state* uupw)

//*** ٰČ�����p̨�����ް��̍Đݒ�i�R�s�[�j ************************
//	aifRecurCopy
//in:	AI_FIELD	aiFieldData	m̨����
//out:	AI_FIELD	aiRecurData	m̨����
//
void aifRecurCopy(void)

//*** ��߾ق�u����ꏊ�̌��� ************************
//	aifPlaceSearch
//in:	AI_FIELD	aiRecurData	m̨����
//out:	AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			aiFlagCnt	��߾ق�u����ʒu�̐�
//
void aifPlaceSearch(void)

//*** ��߾ق�u���鏊����ړ��J�n�ʒu�܂ňړ��\���� ************************
//	aifMoveCheck
//in:	AI_FIELD	aiRecurData	m̨����
//		AI_FLAG		aiFlag[]	��߾ق�u����ʒu��ٰď��
//		u8			aiFlagCnt	��߾ق�u����ʒu�̐�
//out:	AI_FLAG		aiFlag[].ok	�ړ��\���ǂ����t���O(1:�����0:���s)
//		float		aiRootP		�S�ړ��ʂ̕��ϒl�i�ړ��͈͂���������̂��������邽�߂�ܰ��j
//
void aifMoveCheck(void)

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

//*** �b�n��������o�� ************************
//	aifKeyOut
//in:	game_state	*uupw		��ڲ԰�\�����߲��
//		AIWORK		uupw->ai	�e��������
//out:	u16			joygam		���۰װ�d�l�̷����o��
//
void aifKeyOut(game_state* uupw)


