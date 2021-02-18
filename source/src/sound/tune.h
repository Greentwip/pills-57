
#define VOL_CHAR_DEF	0x6c
#define VOL_GAME1_DEF	0x74
#define VOL_GAME2_DEF	0x6a
#define VOL_GAME3_DEF	0x60
#define VOL_MENU_DEF	0x78

typedef struct {
	u8 no;     // ���ԍ�
	s8 coarse; // ����?(T's ���璸�����l)
	s8 fine;   //     ?(T's ���璸�����l)
	u8 pri;    // �v���C�I���e�B
	u8 time;   // �Đ�����(1/10�b�P��, 0�Ŗ���)
	u8 vol;    // ����
} FxTune;

static const FxTune _fxTuneTbl[] = {
	// No         Coarse Fine  Pri Time
	{ FX_01_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �C�J�^�R�e���O
	{ FX_01_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_01_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_02_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �L�O����
	{ FX_02_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_02_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_03_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �n���}�[���{
	{ FX_03_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_03_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_04_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �t�E�Z���܂���
	{ FX_04_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_04_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_05_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �ӂ����炰
	{ FX_05_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_05_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_06_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �܂�J�G��
	{ FX_06_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_06_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_07_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �}�b�h���V�^�C��
	{ FX_07_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_07_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_08_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �}���s�[
	{ FX_08_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_08_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_09_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �����}��
	{ FX_09_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_09_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_10_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // ��񂲂낤
	{ FX_10_AT2,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_DAMAGE,   -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_10_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_11_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �Ȃ��̂���
	{ FX_11_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_11_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_SE01,       -12,   0,  10,   0, VOL_GAME1_DEF }, // �J�v�Z���������̔ԍ�
	{ FX_SE02,         0,   0,  10,   0, VOL_GAME1_DEF }, // �E�C���X�������̔ԍ�
	{ FX_SE03,        -6, -45,  10,   0, VOL_GAME1_DEF }, // ����E�C���X���Ō�
	{ FX_SE04,       -12,   0,  10,   0, VOL_MENU_DEF }, // �������x�A�b�v���̔ԍ�
	{ FX_SE05,        -6, -45,  10,   0, VOL_MENU_DEF }, // �P�o�A�����̉�
	{ FX_SE06,         0,   0,  10,   0, VOL_MENU_DEF }, // �|�[�Y���ʉ�
	{ FX_SE07,       -12,   0,  10,   0, VOL_GAME1_DEF }, // �J�v�Z���������̔ԍ�
	{ FX_SE08,         0,   0,  10,   0, VOL_MENU_DEF }, // ����
	{ FX_SE09,        -6, -45,  10,   0, VOL_MENU_DEF }, // ���j���[���̃J�[�\���ړ�
	{ FX_SE10,        -6, -45,  10,   0, VOL_MENU_DEF }, // ���j���[�؂�ւ�
	{ FX_SE11,       -12,   0,  10,   0, VOL_GAME2_DEF }, // ���E�ړ����̔ԍ�
	{ FX_SE12,       -12,   0,  10,   0, VOL_GAME1_DEF }, // ���n���̔ԍ�
	{ FX_SE13,        -6, -45,  10,   0, VOL_GAME3_DEF }, // ��]���̔ԍ�
	{ FX_SE14,         0,   0,  10,   0, VOL_MENU_DEF }, // �L�����Z��
	{ FX_SE15,         5,  55,  10,   0, VOL_MENU_DEF }, // ��ʂ���ς�
	{ FX_SE16,       -12,   0,  10,   0, VOL_MENU_DEF }, // �o�b�N�A�b�v�f�[�^�폜
	{ FX_SE17,       -12,   0,  10,   0, VOL_MENU_DEF }, // �G���[
	{ FX_SE18,        -6, -45,  10,   0, VOL_MENU_DEF }, // �J�E���g�_�E��,
	{ FX_SE19,        -6, -45,  10,   0, VOL_MENU_DEF }, // �J�E���g�_�E���I��
	{ FX_SE20,         0,   0,  10,  20, VOL_GAME1_DEF }, // ����E�C���X�_���[�W
	{ FX_SE21,        -6, -45,  40,   0, VOL_MENU_DEF }, // �Q�o�A�����̉�
	{ FX_SE22,        -6, -45,  40,   0, VOL_MENU_DEF }, // �R�o�A�����̉�
	{ FX_SE23,        -6, -45,  40,   0, VOL_MENU_DEF }, // �S�o�A�����̉�
	{ FX_SE24,        -6, -45,  40,   0, VOL_GAME1_DEF }, // �R�C���l��
	{ FX_SE25,        -6, -45,  40,   0, VOL_MENU_DEF }, // �J�v�Z���ςݏグ�x����
	{ FX_SE26,        -6, -45,  40,   0, VOL_MENU_DEF }, // �E�C���X���c��R�̂Ƃ��̉�

	{ FX_12_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �����I
	{ FX_12_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_12_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_13_AT1,     -12,   0,  40,   0, VOL_CHAR_DEF }, // �o���p�C�A�����I
	{ FX_13_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_AT3,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_13_WIN,     -12,   0,  40,   0, VOL_CHAR_DEF },

	{ FX_14_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // �}���I
	{ FX_14_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_14_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_14_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_14_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_15_AT1,      -6, -45,  40,   0, VOL_CHAR_DEF }, // ���^���}���I
	{ FX_15_AT2,     -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_15_AT3,      -6, -45,  40,   0, VOL_CHAR_DEF },
	{ FX_15_DAMAGE,  -12,   0,  40,   0, VOL_CHAR_DEF },
	{ FX_15_WIN,      -6, -45,  40,   0, VOL_CHAR_DEF },

	{ FX_SE27,         0,   0,  10,   0, VOL_MENU_DEF }, // 
	{ FX_SE28,         0,   0,  10,   0, VOL_MENU_DEF }, // 
	{ FX_SE29,         0,   0,  10,   0, VOL_MENU_DEF }, // 
};
