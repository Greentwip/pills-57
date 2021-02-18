
#ifndef __dm_game_def_h__
#define	__dm_game_def_h__

#include "aidef.h"
#include "char_anime.h"

//////////////////////////////////////////////////////////////////////////////
// �錾

#define cap_flg_on				1
#define cap_flg_off				0

#define cap_size_10				10		// �J�v�Z���ʏ�T�C�Y
#define cap_size_8				8		// �J�v�Z���S�o�T�C�Y

#define cap_size_big			0		// �ʏ�T�C�Y
#define cap_size_small			1		// �S�o�T�C�Y

#define cap_disp_flg			0		// �\���t���O
#define cap_down_flg			1		// �����t���O
#define cap_condition_flg		2		// ��ԃt���O
#define cap_col_flg				3		// �F�t���O
#define cap_virus_flg			4		// �E�C���X�̔ԍ�

#define cap_map_flg				2		// �}�b�v�ԍ��t���O
#define cap_throw_flg			2		// ������ԃt���O

#define dm_retire_flg			0		// ���^�C�A�t���O
#define dm_game_over_flg		1		// ���オ��I���m�F�t���O
#define dm_training_flg			2		// �g���[�j���O�t���O
#define dm_static_cnd			3		// ��ΓI�R���f�B�V����


#define cap_turn_r				1		// ���v���(�E�ړ�)
#define cap_turn_l				-1		// �����v���(���ړ�)

#define dm_mode_now				0		// ���݂̔z��ԍ����w��
#define dm_mode_old				1		// �ۑ��p�̔z��ԍ����w��

typedef enum {
	dm_cnd_null,
	dm_cnd_wait,			// �������Ȃ��҂����
	dm_cnd_init,			// ������
	dm_cnd_stage_clear,		// �X�e�[�W�N���A
	dm_cnd_game_over,		// �Q�[���I�[�o�[
//	dm_cnd_game_over_retry,	// �Q�[���I�[�o�[�Ń��g���C�҂�
	dm_cnd_win,				// WIN�̂�
	dm_cnd_win_retry,		// WIN�Ń��g���C�҂�
	dm_cnd_win_retry_sc,	// WIN & ���g���C & ���_
//	dm_cnd_win_any_key,		// WIN�ŃL�[�҂�
//	dm_cnd_win_any_key_sc,	// WIN & ANY KEY & ���_
	dm_cnd_lose,			// LOSE�̂�
	dm_cnd_lose_retry,		// LOSE�Ń��g���C�҂�
	dm_cnd_lose_retry_sc,	// LOSE & ���g���C & ���_
//	dm_cnd_lose_sc,			// LOSE & ���_
//	dm_cnd_lose_any_key,	// LOSE�ŃL�[�҂�
	dm_cnd_draw,			// DRAW�̂�
	dm_cnd_draw_retry,		// DRAW�̂� & ���g���C
//	dm_cnd_draw_any_key,	// DRAW�ŃL�[�҂�
	dm_cnd_pause,			// �|�[�Y
	dm_cnd_pause_re,		// �|�[�Y & ���g���C
	dm_cnd_pause_re_sc,		// �|�[�Y & ���g���C & ���_
//	dm_cnd_pause_tar,		// �|�[�Y & �U������
//	dm_cnd_pause_tar_re,	// �|�[�Y & �U������ & ���g���C
//	dm_cnd_pause_tar_re_sc,	// �|�[�Y & �U������ & ���g���C & ���_
	dm_cnd_retire,			// ���^�C�A
	dm_cnd_tr_chack,		// ���K�m�F
	dm_cnd_training,		// ���K
//	dm_cnd_manual,			// �������
	dm_cnd_clear_wait,
	dm_cnd_clear_result,
	dm_cnd_gover_wait,
	dm_cnd_gover_result,
	dm_cnd_retire_wait,
	dm_cnd_retire_result,
	dm_cnd_debug_config,	// �R���t�B�O
	dm_cnd_debug,			// �f�o�b�N
} DM_GAME_CONDITION;

typedef enum {
	dm_mode_null,
	dm_mode_init,			// ������
//	dm_mode_4p_story,		// 4P �X�g�[���[���̊�\��
	dm_mode_wait,			// �҂�
	dm_mode_throw,			// ����
	dm_mode_down,			// �J�v�Z������
	dm_mode_down_wait,		// ���n�E�F�C�g
	dm_mode_erase_chack,	// ���Ŕ���
	dm_mode_erase_anime,	// ���ŃA�j��
	dm_mode_ball_down,		// ���Ō�̗���
	dm_mode_cap_set,		// �V�J�v�Z���Z�b�g
//	dm_mode_get_coin,		// �N���A�f�� & �R�C���f��
	dm_mode_stage_clear,	// �N���A�f��
	dm_mode_game_over,		// �Q�[���I�[�o�[�f��
//	dm_mode_game_over_retry,// �Q�[���I�[�o�[�f�� & ���g���C
//	dm_mode_use_coin,		// �Q�[���I�[�o�[�f�� & ���g���C & �R�C���f��
//	dm_mode_black_up,		// ������Â��Ȃ鏈��
	dm_mode_bottom_up,		// �r�̒��g����i�オ�鏈��
	dm_mode_win,			// WIN�f��
	dm_mode_win_retry,		// WIN�f�� �����g���C
	dm_mode_lose,			// LOSE�f��
	dm_mode_lose_retry,		// LOSE�f�������g���C
	dm_mode_draw,			// DRAW�f��
	dm_mode_draw_retry,		// DRAW�f�������g���C
	dm_mode_tr_chaeck,		// ���K���f
	dm_mode_training,		// ���K�����ݒ�
	dm_mode_tr_erase_chack,	// ���K�p���Ŕ���
	dm_mode_tr_cap_set,		// ���K�p�V�J�v�Z���Z�b�g
	dm_mode_pause,			// �|�[�Y
	dm_mode_pause_retry,	// �|�[�Y&���g���C
	dm_mode_no_action,		// �|�[�Y&���g���C
	dm_mode_clear_wait,
	dm_mode_clear_result,
	dm_mode_gover_wait,
	dm_mode_gover_result,
	dm_mode_retire_wait,
	dm_mode_retire_result,
	dm_mode_debug_config,	// �R���t�B�O
	dm_mode_debug,			// �f�o�b�N
} DM_GAME_MODE;

typedef enum {
	dm_ret_game_end   =  -2, // ���S�I��
	dm_ret_game_over  =  -1, // �Q�[���I�[�o�[
	dm_ret_null       =   0,
	dm_ret_next_stage =   1, // ���̃X�e�[�W��
	dm_ret_retry      =   2, // ���̃X�e�[�W��
	dm_ret_virus_wait =   3, // �E�C���X�z�u�I��
	dm_ret_pause      =   4, // �|�[�Y
	dm_ret_black_up   =   5, // ������Â��Ȃ鏈���I��
	dm_ret_clear      =   6, // �N���A
	dm_ret_tr_a       =   7, // ���K�U������
	dm_ret_tr_b       =   8, // ���K�U������
	dm_ret_replay     =   9, // ���v���C
	dm_ret_end        = 100, // ���[�v�E�o
} DM_GAME_RET;

#if 0 // �폜
/*
#define dm_1_VS_3				0x0e	// ����p 1 �� 1 �̂Ƃ�
#define dm_2_VS_2				0x0c	// ����p 2 �� 2 �̂Ƃ�
#define dm_3_VS_1				0x08	// ����p 3 �� 1�̂Ƃ�
*/
#endif

#define v_anime_def_speed		8		// �E�C���X�A�j���[�V�������x
#define v_anime_def_speed_4p	12		// �E�C���X�A�j���[�V�������x(4P)
#define v_anime_speed			1		// �E�C���X�A�j���[�V�������x(�Q�[���I�[�o�[��)
#define v_anime_speed_4p		4		// �E�C���X�A�j���[�V�������x(4P)(�Q�[���I�[�o�[��)

#define dm_bound_wait			18		// ���n�E�F�C�g
#define dm_erace_speed_1		10		// �E�C���X�̏��ŃA�j���[�V�����̂P�i�K�ڂ܂ł̃J�E���g
#define dm_erace_speed_2		27		// �E�C���X�̏��ŃA�j���[�V�����̂Q�i�K�ڂ܂ł̃J�E���g
#define dm_down_speed			14		// �U���J�v�Z���Ɨ���J�v�Z���̗������x
#define dm_black_up_speed		6		// �����獕���Ȃ鏈���̑��x

#define dm_wold_x				118		// 1P�p��w���W
#define dm_wold_y				46		// 1P�p��x���W

#if 0 // �폜
/*
#define dm_next_y				36		// �l�N�X�g�J�v�Z���̂x���W(1P�E2P)
*/
#endif

#define dm_wold_x_vs_1p			28		// 2P�p 1P ��w���W
#define dm_wold_x_vs_2p			212		// 2P�p 2P ��w���W

#define dm_wold_x_4p			20		// 4P�p��w���W
#define dm_wold_y_4p			46		// 4P�p��x���W

#if 0 // �폜
/*
#define dm_next_y_4p			44		// 4P�l�N�X�g�J�v�Z���̂x���W
*/
#endif

#define dm_wold_x_4p_1p			20		// 4P�p 1P ��w���W
#define dm_wold_x_4p_2p			92		// 4P�p 2P ��w���W
#define dm_wold_x_4p_3p			164		// 4P�p 3P ��w���W
#define dm_wold_x_4p_4p			236		// 4P�p 4P ��w���W

enum {
	capsel_u,				// �J�v�Z��	��
	capsel_d,				// 			��
	capsel_l,				// 			��
	capsel_r,				// 			�E
	capsel_b,				// 			�{�[��
	erase_cap_a,			// 			���łP
	erase_cap_b,			// 			���łQ
	virus_a1,				// �E�C���X	a1
	virus_a2,				// 			a2
	virus_b1,				// 			b1
	virus_b2,				// 			b2
	virus_c1,				// 			c1
	virus_c2,				// 			c2
	erase_virus_a,			// 			���łP
	erase_virus_b,			// 			���łQ
	no_item,				// ��������
	virus_a3,				// 			a3
	virus_a4,				// 			a4
	virus_b3,				// 			b3
	virus_b4,				// 			b4
	virus_c3,				// 			c3
	virus_c4				// 			c4
};

enum {
	capsel_red,				// �J�v�Z��	��
	capsel_yellow,			// 			��
	capsel_blue,			// 			��
	capsel_b_red,			// 		�Â���
	capsel_b_yellow,		// 		�Â���
	capsel_b_blue,			// 		�Â���
};

//////////////////////////////////////////////////////////////////////////////
// �Q�[�����p�\����

// �r�̒��̏��T�C�Y
#define GAME_MAP_W 8
#define GAME_MAP_H 16

typedef struct {
	s8	pos_m_x,pos_m_y;	// �\�����W(8 x 16)
	s8	capsel_m_g;			// �J�v�Z���̃O���t�B�b�N�ԍ�
	s8	capsel_m_p;			// �J�v�Z���̃p���b�g�ԍ�
	s8	capsel_m_flg[6];	// �J�v�Z���̕\���t���O�Ɨ�������t���O
} game_map;

// �����J�v�Z���p�\����
typedef struct {
	s8	pos_x[2],pos_y[2];	// �\�����W(8 x 16)
	s8	capsel_g[2];		// �J�v�Z���̃O���t�B�b�N�ԍ�
	s8	capsel_p[2];		// �J�v�Z���̃p���b�g�ԍ�
	s8	capsel_flg[4];		// �J�v�Z���̕\���t���O�Ɨ�������t���O
} game_cap;

// �U���J�v�Z���p�\����
typedef struct {
	s8	pos_a_x,pos_a_y;	// �\�����W(8 x 16)
	s8	capsel_a_p;			// �J�v�Z���̃p���b�g�ԍ�
	s8	capsel_a_flg[3];	// �J�v�Z���̕\���t���O�Ɨ�������t���O
} game_a_cap;

// �Q�[���̐���p�\����
typedef struct {
	u32 game_score; // ���_
	u16 game_retry; // ���g���C

	s16 map_x, map_y;  // �}�b�v�̍��W
	s8  map_item_size; // �}�b�v�̃A�C�e���̑傫��

	DM_GAME_MODE mode_now; // ���������ԍ�
	DM_GAME_MODE mode_old; // ���������ԍ��ۑ��ϐ�

	DM_GAME_CONDITION cnd_now;      // ���̎��̏��
	DM_GAME_CONDITION cnd_old;      // ���̎��̏�ԕۑ��ϐ�
	DM_GAME_CONDITION cnd_training; // �ω����Ȃ��󋵕ϐ�(�g���[�j���O�����ƂO�ȊO)
	DM_GAME_CONDITION cnd_static;   // �s�ϓI��� win lose pause wait

	u8 virus_order_number;// �z�u���̃E�B���X�ԍ�
	u8 virus_number;      // �E�C���X��
	u8 virus_level;       // �E�C���X���x��
	u8 virus_anime;       // �E�C���X�A�j���[�V�����ԍ�
	s8 virus_anime_vec;   // �E�C���X�A�j���[�V�����i�s����
	u8 virus_anime_count; // �E�C���X�A�j���[�V�����J�E���^
	u8 virus_anime_max;   // �E�C���X�A�j���[�V�����̍ő�R�}��
	u8 virus_anime_spead; // �E�C���X�A�j���[�V�������x

	u8 cap_def_speed; // �J�v�Z�����x(SPEED_?(LOW/MID/HIGH/MAX))
		#define SPEED_LOW  0
		#define SPEED_MID  1
		#define SPEED_HIGH 2
		#define SPEED_MAX  3
	u8 cap_speed;         // �J�v�Z�����x
	u8 cap_count;         // �J�v�Z��������
	u8 cap_speed_count;   // �J�v�Z�������p�J�E���^
	u8 cap_speed_vec;     // �J�v�Z�������p�J�E���^�����l
	u8 cap_speed_max;     // �J�v�Z���������x(�J�E���^�����̐��l�ȏ�ɂȂ�ƂP�i����)
	u8 cap_magazine_cnt;  // �J�v�Z���}�K�W���Q�Ɨp�ϐ�
	u8 cap_magazine_save; // �J�v�Z���}�K�W���Q�Ɨp�ۑ��ϐ�
	s8 cap_move_se_flg;   // �J�v�Z�����E�ړ����̂r�d��炷���߂̃t���O

	u8 erase_anime;       // ���ŃA�j���[�V�����R�}��
	u8 erase_anime_count; // ���ŃA�j���[�V�����J�E���^, �������̃J�E���^�����˂�
	u8 erase_virus_count; // ���ŃE�C���X��
	u8 erase_virus_count_old;

	u8 chain_count;    // �A����
	u8 chain_line;     // ���ŗ�
	u8 chain_line_max; // �ő���ŗ�
	u8 chain_color[4];
		// 0: ��
		// 1: ��
		// 2: �� �������F���J�E���g����B
		// 3: �A���J�n���ɏ������F�̃r�b�g�𗧂Ă�
		//    0x01:��
		//    0x02:��
		//    0x04:��
		//    �E�C���X�܂ޏꍇ 0x80 �̃r�b�g�𗧂Ă�

	// ���オ�菈���J�E���^
	u8 black_up_count;

	// �x������炷���߂̃J�E���^
	int warning_se_flag;

	u8 flg_retire;    // ���^�C�A�t���O
	u8 flg_game_over; // �Q�[���I�[�o�[
	u8 flg_training;  // �g���[�j���O�t���O

	u8 player_no;   // �v���C���[�̔ԍ�
	u8 player_type; // �v���C���[�̎��
		#define PUF_PlayerMAN  0
		#define PUF_PlayerCPU  1
	u8 think_type;  // �G�v�l�L����
	u8 think_level; // �b�o�t���x��
		#define PMD_CPU0 0
		#define PMD_CPU1 1
		#define PMD_CPU2 2
	u8 team_no;     // �`�[���ԍ�
		#define TEAM_MARIO  0
		#define TEAM_ENEMY  1
		#define TEAM_ENEMY2 2
		#define TEAM_ENEMY3 3

	// 0:�����ɗ�������U���J�v�Z���̃��[�N(2bit�Ő���,01:��,10:��,11:��,00:����)
	// 1:�U�����Ă�������
	#define DAMAGE_MAX 0x10
	u16 cap_attack_work[DAMAGE_MAX][2];

	int         charNo;   // �L�����N�^�ԍ�
	SAnimeState anime;    // �A�j���[�V��������

	// �t���b�V���E�B���X�̈ʒu [0]:X, [1]:Y, [2]:COLOR
	#define FLASH_VIRUS_MAX 9
	int flash_virus_pos[FLASH_VIRUS_MAX][3];
	int flash_virus_bak[FLASH_VIRUS_MAX];
	int flash_virus_count;

	// �r�̒��g����i�グ�鎞�̃J�E���^
	int bottom_up_scroll;

	// "�t���b�V��", "�ϋv", "�^�C�A�^" ���̓�Փx
	int game_level;

	int total_erase_count; // ���݂̃E�B���X������
	int total_chain_count; // ���v�A����

	game_cap    now_cap;  // ���ݑ��삷��J�v�Z��
	game_cap    next_cap; // ���̃J�v�Z��

	AIWORK ai; // COM�v�l�pܰ�
	u8     pn; // ��ڲ԰ no.
	u8     gs; // �ްѽð��
	u8     lv; // �ݒ�����(0-29)
	u8     vs; // ��ٽ�c��(�����ł͂Ȃ��A����Ă��Ƃ�ϯ���ް��ォ��T��������������)

	AIBLK  blk[MFieldY+1][MFieldX]; //��ۯ�ܰ�
	AICAPS cap;
} game_state;

// �E�C���X�z�u�p�\����
typedef struct {
	s8 virus_type;  // �E�C���X�̎��(�F�����˂�)
	u8 x_pos,y_pos; // �w�E�x���W
} virus_map;

//////////////////////////////////////////////////////////////////////////////

#endif // __dm_game_def_h__
