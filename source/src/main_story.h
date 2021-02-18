#ifndef		__main_story__
#define		__main_story__


// �X�^�b�t���[���e�L�X�g
extern	char st_staffroll_txt[];


// �J�[�e���\������
extern	void curtain_proc(Gfx **glp, int count);
// curtain_proc �̈��� count ��
#define		CURTAIN_UP		0		// �J�[�e�����オ�������
#define		CURTAIN_DOWN	30		// �J�[�e���������������
#define		ZOOM_EFF_0		0		// �J�[�e���������������
#define		ZOOM_EFF_MAX	90		// �J�[�e���������������


// �Q�[�����w�i    ���� stage �ɂ� STORY_?_ST? ���w��
extern	void *story_bg_init(int stage, u32 buffAddr);
extern	void story_bg_proc(Gfx **glp);


// �G���f�B���O���b�Z�[�W
extern	char	*EndingLastMessage;

// �X�g�[���[�\���̏����ԍ��i�X�e�[�W�j
extern int	story_proc_no;
// story_proc_no�ɐݒ肷��l
#define	STORY_M_OPEN	0
#define	STORY_M_ST1		1
#define	STORY_M_ST2		2
#define	STORY_M_ST3		3
#define	STORY_M_ST4		4
#define	STORY_M_ST5		5
#define	STORY_M_ST6		6
#define	STORY_M_ST7		7
#define	STORY_M_ST8		8
#define	STORY_M_ST9		9
#define	STORY_M_END		10
#define	STORY_M_END2	11
#define	STORY_W_OPEN	12
#define	STORY_W_ST1		13
#define	STORY_W_ST2		14
#define	STORY_W_ST3		15
#define	STORY_W_ST4		16
#define	STORY_W_ST5		17
#define	STORY_W_ST6		18
#define	STORY_W_ST7		19
#define	STORY_W_ST8		20
#define	STORY_W_ST9		21
#define	STORY_W_END		22
#define	STORY_W_END2	23


// GBI�f�[�^
typedef struct {
	int	type;
	int	x, y;
	u8	*lut_addr;
	u8	*tex_addr;
} GBI_DATA;



extern	void	get_gbi_stat(GBI_DATA *gbi, u32 gbi_addr);



// �^�C�g����ʂ̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int flg			TRUE=���߂���\��	 FALSE=�����Ȃ�\���i���C�����j���[����߂����ꍇ�j
extern	u32	init_title(u32 buffer, int flg);

// �^�C�g����ʂ̕\��
//		int flg			���A�[�t���O  ��:0�A�[:1
extern	int demo_title(Gfx **glp, int flg21);

// ���j���[�a�f�̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int flg			�w�i�̒��A�[�t���O  ��:0�A�[:1
u32	init_menu_bg(u32 buffer, int flg);

// ���j���[�a�f�̕\��
//		int	ofsx,ofsy	�\���ʒu�I�t�Z�b�g
void draw_menu_bg(Gfx **glp, int ofsx, int ofsy);


// �R�[�q�[�u���C�N�̏�����
//		u32 buffer		�O���t�B�b�N�f�[�^�̓ǂݍ��݃A�h���X
//		int type		�f���̎��	0:EASY 1:MED 2:HARD 3:SHARD
extern	u32		init_coffee_break(u32 buffer, int type);

// �R�[�q�[�u���C�N�̓����J�E���^�[������
extern	void	init_coffee_break_cnt(void);

// �R�[�q�[�u���C�N�̕\��
// tpye      0:Low, 1:Med, 2:Hi
// mode      FALSE=Lv21  TRUE=Lv24
// disp_flg  FALSE=���b�Z�[�W�n����  TRUE=���b�Z�[�W�n��
void draw_coffee_break(Gfx **glp, int type, int mode, int disp_flg);

//-------------------------------------------------------------------------
// �m�����e������
// input:
//	int	count		ZOOM_EFF_0 --> ZOOM_EFF_MAX
extern	void	story_zoomfade(Gfx **glp, int count);


//-------------------------------------------------------------------------
// ���^�X�|�b�g���C�g����
// input:
//		x     : �X�|�b�g���S�@�w
//		y     : �X�|�b�g���S�@�x
//		count : 0 �^���� �` 255
extern	void	star_spot(Gfx **glp, int x, int y, int count);


//-------------------------------------------------------------------------
// �X�^�b�t���[����̃��b�Z�[�W���̔w�i
extern	void	draw_ending_mess_bg(Gfx **glp);


#endif
