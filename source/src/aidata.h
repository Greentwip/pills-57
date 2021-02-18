/***********************************************************
	ai data header file		Tadashi Ogura

	Modified by Yasunori Kobayashi.
***********************************************************/

// virus ������Ӱ��
enum {
	AI_SPEED_SUPERSLOW,			// �ƂĂ��x���i���ݖ��g�p�j
	AI_SPEED_VERYSLOW,			// �ƂĂ��x��
	AI_SPEED_SLOW,				// �x��
	AI_SPEED_FAST,				// ����
	AI_SPEED_VERYFAST,			// �ƂĂ�����
	AI_SPEED_MAX,				// �ō���
	AI_SPEED_FLASH,				// ɰ���Ĉړ�
	AI_SPEED_VERYFASTNW,			// �ƂĂ������i�������S�O�����j
	_AI_SPEED_SUM,
} AI_SPEED;

// �v�l���W�b�N�̎��
typedef enum {
	AI_LOGIC_FastErase,
	AI_LOGIC_SmallRensa,
	AI_LOGIC_RensaAndErase,
	AI_LOGIC_RensaAndAttack,
	AI_LOGIC_WidErase,
	AI_LOGIC_MajorityRensa,
	_AI_LOGIC_SUM,
} AI_LOGIC;

// �L�����̃v���C��
enum {
	AI_STATE_DEAD,			// ���񂾌���K���[�h�p
	AI_STATE_NARROW,		// �ړ��\�͈͂������Ȃ��Ă�����
	AI_STATE_LASTVS,		// �I�Ղų�ٽ�����鎞
	AI_STATE_LASTVSNH,		// �I�Ղų�ٽ���Ȃ����ō����ς񂾶�߾ق�������
	AI_STATE_LASTVSN,		// �I�Ղų�ٽ���Ȃ���
	AI_STATE_BADLINE2,		// �����鍂�ς�ײ݂������Qײ݂ɂ��鎞
	AI_STATE_BADLINE1,		// �����Ȃ����ς�ײ݂������Uײ݂ɂ��鎞
	AI_STATE_NORMAL,		//
	_AI_STATE_SUM,
} AI_STATE;

// �v�l���W�b�N�f�[�^
typedef struct {
	s16		dt_LnOnLinVrs;
	s16		dt_LnEraseVrs;
	s16		dt_EraseLinP1;
	s16		dt_EraseLinP3;
	s16		dt_HeiEraseLinRate;
	s16		dt_WidEraseLinRate;

	s16		dt_P_ALN_HeightP;
	s16		dt_P_ALN_FallCap;
	s16		dt_P_ALN_FallVrs;
	s16		dt_P_ALN_Capsule;
	s16		dt_P_ALN_Virus;

	s16		dt_WP_ALN_Bottom;
	s16		dt_WP_ALN_FallCap;
	s16		dt_WP_ALN_FallVrs;
	s16		dt_WP_ALN_Capsule;
	s16		dt_WP_ALN_Virus;

	s16		dt_OnVirusP;
	s16		dt_RensaP;
	s16		dt_RensaMP;
	s16		dt_LnHighCaps;
	s16		dt_LnHighVrs;
	s16		dt_aiWall_F;

	s16		dt_HeiLinesAllp2;
	s16		dt_HeiLinesAllp3;
	s16		dt_HeiLinesAllp4;

	s16		dt_WidLinesAllp2;
	s16		dt_WidLinesAllp3;
	s16		dt_WidLinesAllp4;
} AI_DATA;

// �L�����̍s���p�^�[���f�[�^
typedef struct {

	// ����ɂ�����܃J�v�Z���̃X�g�b�N������Ƃ��A
	// �U����҂��������s���邩���Ȃ����ݒ肷��t���O�B �i0:OFF,1:ON�j
	s8 wait_attack;

	// ���쑬�x(AI_SPEED_????)
	s8 speed;

	// �����ւ̃_���[�W�J�v�Z�����s���̂悢���ɗ����闦
	s16 luck;

	// ��(AI_STATE_????)���Ŏg�p����v�l���W�b�N(AI_LOGIC_????)
	s8 logic[_AI_STATE_SUM];

	// ���ꏈ����������(AI_CONDITION_????)
	s8 condition[NUM_AI_EFFECT];
	s16 condition_param[NUM_AI_EFFECT];

	// ���s������ꏈ��(AI_EFFECT_????)
	s8 effect[NUM_AI_EFFECT];
	s16 effect_param[NUM_AI_EFFECT];

} AI_CHAR;

// �L�����̓��ꏈ����������  Commented by Mr.Maeta.
typedef enum {

// �����������������
	AI_CONDITION_Ignore,

// �������Ō��ʂ���������B
	AI_CONDITION_Unconditional,

// �����_���Ō��ʂ���������B
// Sub�p�����[�^�����݂��A��������ݒ肷��B
	AI_CONDITION_Random,

// �ΐ푊�肩��U�����󂯂�ƌ��ʂ���������B
	AI_CONDITION_Damage,

// �ΐ푊����������̃E�B���X�������Ƃ��Ɍ��ʂ���������B
// Sub�p�����[�^�����݂��A��������E�B���X����ݒ肷��B
	AI_CONDITION_ManyVir,

// �ΐ푊����������̃E�B���X�����Ȃ��Ƃ��Ɍ��ʂ���������B
// Sub�p�����[�^�����݂��A��������E�B���X����ݒ肷��B
	AI_CONDITION_FewVir,

// �ΐ푊��̃J�v�Z�����ςݏオ��ƌ��ʂ���������B
	AI_CONDITION_Pile,

// �b�n�l���x�����d�`�r�x�̂Ƃ��Ɍ��ʂ���������B
	AI_CONDITION_Easy,

// �b�n�l���x�����m�n�q�l�`�k�̂Ƃ����ʂ���������B
	AI_CONDITION_Normal,

// �b�n�l���x�����g�`�q�c�̂Ƃ��Ɍ��ʂ���������B
	AI_CONDITION_Hard,

// �����̎c��E�B���X�����Ȃ��Ȃ�ƌ��ʂ���������B
// Sub�p�����[�^�����݂��A�E�B���X�����C�ȉ��ɂȂ�Ɣ������邩�ݒ肷��B
	AI_CONDITION_Last,

// �A����������ƌ��ʂ���������B
// Sub�p�����[�^�����݂��A��������A������ݒ肷��B
	AI_CONDITION_Rensa,

// �Q�[���J�n���̃E�B���X���w�肵�����ȏ�̎��ɔ�������
// Sub�p�����[�^�����݂��A�E�B���X����ݒ肷��B
	AI_CONDITION_PreemPtive,

	_AI_CONDITION_SUM,

// �� Sub�p�����[�^�Ƃ́AAI_CHAR �\���̂� condition_param �̂��ƁB

} AI_CONDITION;

// �L�����̓��ꏈ��  Commented by Mr.Maeta.
typedef enum {

// �G�t�F�N�g�������ȏ�ԁB
	AI_EFFECT_Ignore,

// �J�v�Z���𖳈Ӗ��ɉ�]������B
	AI_EFFECT_Rotate,

// �J�v�Z���𖳈Ӗ��ɉ��ړ������āA�����Ă���悤�Ɍ�����B
	AI_EFFECT_Waver,

// �J�v�Z���𑀍�ł��Ȃ��Ȃ�B
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
	AI_EFFECT_NotMove,

// �J�v�Z���̑��쑬�x�������Ȃ�B
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
	AI_EFFECT_BeFast,

// �J�v�Z���̑��쑬�x���x���Ȃ�B
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
	AI_EFFECT_BeSlow,

// �J�v�Z���̑��쑬�x���m�[�E�G�C�g�ɂȂ�B
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
	AI_EFFECT_BeNoWait,

// �J�v�Z���̑��쑬�x���ǂ�ǂ񑬂��Ȃ�B
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
	AI_EFFECT_Rapid,

// �������W�b�N���g�U���h�����ւP�i�K�i�ށB
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
// �������W�b�N�́@�u�������v�@FastErase-SmallRensa-Rensa&Erase-Rensa&Attack�@�u�U���v�@�ƁA���ԁB
	AI_EFFECT_BeAttack,

// �������W�b�N���g�������h�����ւP�i�K�i�ށB
// Sub�p�����[�^�����݂��A���ʂ��������鎞�Ԃ�ݒ肷��B�i0�́��B�j
// �������W�b�N�́@�u�������v�@FastErase-SmallRensa-Rensa&Erase-Rensa&Attack�@�u�U���v�@�ƁA���ԁB
	AI_EFFECT_BeErase,

// �������Ă�����ꏈ����S�Ė����ɂ���B
// Sub�p�����[�^�����݂��A�����ɂ�����ꏈ�����w�肷��B�i0�͑S�āB�j
	AI_EFFECT_Lose,

	_AI_EFFECT_SUM,

// �� Sub�p�����[�^�Ƃ́AAI_CHAR �\���̂� effect_param �̂��ƁB

} AI_EFFECT;

///////////////////////////////////////////////////////////

// �v�l�p�����[�^�f�[�^���[�N
//�i�p�����[�^�f�[�^���f�o�b�O���[�h�ŉςɂ��邽�ߌ��f�[�^����]�����Ďg���j
extern AI_DATA	ai_param[_AI_LOGIC_SUM][_AI_STATE_SUM];

// �v�l�p�����[�^�f�[�^
extern AI_DATA	ai_param_org[_AI_LOGIC_SUM][_AI_STATE_SUM];

// �L�����̎v�l�p�^�[��
// �i�f�o�b�O���[�h�ŉςɂ��邽�ߌ��f�[�^����]�����Ďg���j
extern AI_CHAR ai_char_data[16];

// �L�����̎v�l�p�^�[�� [ �L�����̔ԍ� (CHR_????) ]
extern AI_CHAR ai_char_data_org[16];
