//////////////////////////////////////////////////////////////////////////////
// ����   : �Z�[�u�f�[�^�֘A�̐錾
// ̧�ٖ� : record.h
// ���   : ���� �N�K
// �쐬�� : 1999/03/10(��)
// �X�V�� : 1999/10/28(��)
// ���e   : �Z�[�u�f�[�^�֘A�̐錾�ł�
//////////////////////////////////////////////////////////////////////////////

#if !defined(_RECORD_H_)
#define _RECORD_H_

#include <ultra64.h>
#include "msgwnd.h"

//////////////////////////////////////////////////////////////////////////////
//{### �}�N��

//## EEPRom �̃G���[�R�[�h
typedef enum EepRomErr {
	EepRomErr_NoError,    // OK
	EepRomErr_NotFound,   // EEPRom ������
	EepRomErr_NotInit,    // ����������Ă��Ȃ�
	EepRomErr_ReadError,  // �ǂݍ��ݒ��ɃG���[
	EepRomErr_WriteError, // �������ݒ��ɃG���[
	EepRomErr_BadSum,     // �`�F�b�N�T���s��
} EepRomErr;

//////////////////////////////////////////////////////////////////////////////
//{### �J�[�\���ʒu

// �V�Q�[���ݒ�ۑ��p�\����
typedef struct {
	u8 st_lv;    // �Q�[�����x��
	u8 st_sh;    // S-HARD MODE
	u8 st_st;    // �X�e�[�W�ԍ�
	u8 st_no;    // �g�p�L�����N�^�[�ԍ� 0:�}���I 1:�����I

	u8 p1_lv;    // LEVEL SELECT �E�C���X���x��
	u8 p1_sp;    // LEVEL SELECT �J�v�Z�����x
	u8 p1_m;     // LEVEL SELECT ���y

	u8 p1_ta_lv; // TIME ATTACK LEVEL
	u8 p1_tq_lv; // �ϋv        LEVEL

	u8 vc_fl_lv[2]; // VSCOM �t���b�V�����[�h�̓�Փx (0: MAN 1:CPU )
	u8 vc_lv[2]; // VSCOM �E�C���X���x�� (0: MAN 1:CPU )
	u8 vc_sp[2]; // VSCOM �J�v�Z�����x (0: MAN 1:CPU )
	u8 vc_no[2]; // VSCOM �g�p�L�����N�^�[�ԍ� (0: MAN 1:CPU )
	u8 vc_st;    // �w�i�ԍ�
	u8 vc_m;     // VSCOM ���y

	u8 vm_fl_lv; // VSMAN �t���b�V�����[�h�̓�Փx
	u8 vm_ta_lv; // VSMAN �^�C���A�^�b�N�̓�Փx
	u8 vm_lv;    // VSMAN �E�C���X���x��
	u8 vm_sp;    // VSMAN �J�v�Z�����x
	u8 vm_no;    // VSMAN �g�p�L�����N�^�[�ԍ�
	u8 vm_st;    // �w�i�ԍ�
	u8 vm_m;     // VSMAN ���y
} game_config;

typedef struct {
	u8 p4_team[4]; // 4P �`�[���ԍ�
	u8 p4_lv[4];   // 4P �E�C���X���x��
	u8 p4_fl_lv[4];// 4P �t���b�V�����[�h�̃Q�[�����x��
	u8 p4_no[4];   // 4P �g�p�L�����N�^�[�ԍ�
	u8 p4_sp[4];   // 4P�J�v�Z�����x
	u8 p4_st;      // 4P �w�i�ԍ�
	u8 p4_m;       // 4P ���y�ԍ�
} game_cfg_4p;

//////////////////////////////////////////////////////////////////////////////
//{### �Z�[�u�f�[�^

// �Z�[�u�p�X�g���[���[�h�̃f�[�^
typedef struct {
	u32 score;   // ���_
	u32 time;    // �N���A�^�C��
	u8  c_stage; // �N���A�X�e�[�W
} mem_story;

// �Z�[�u�p���x���Z���N�g�̃f�[�^
typedef struct {
	u32 score;   // ���_
	u8  c_level; // �N���A���x��
} mem_level;

// �Z�[�u�p�ϋv���[�h�̃f�[�^
typedef struct {
	u32 score;   // ���_
	u32 time;    // �^�C��
} mem_taiQ;

// �Z�[�u�p�^�C���A�^�b�N�̃f�[�^
typedef struct {
	u32 score; // ���_
	u32 time;  // �^�C��
	u8  erase; // �������E�B���X
} mem_timeAt;

// �Z�[�u�p�e�l�̃f�[�^
typedef struct {

	// ���܂��܂ȃr�b�g�t���O ���L�� DM_MEM_? �̃r�b�g������
	u8 mem_use_flg;
		#define DM_MEM_USE 0x01 // �g�p��
		#define DM_MEM_CAP 0x02 // �J�v�Z�������ʒu�\���̂n�m�^�n�e�e

	// ���O
	#define MEM_NAME_SIZE 4
	u8 mem_name[MEM_NAME_SIZE];

	// �X�g�[���[���[�h�̃N���A�[�����X�e�[�W[0:easy, 1:normal, 2:hard, 3:s-hard][0:mario, 1:wario]
	int clear_stage[4][2];

	// �X�g�[���[���[�h�̃f�[�^(�Q�[�����x����)
	mem_story story_data[3];

	// ���x���Z���N�g�̃f�[�^(�Q�[���X�s�[�h��)
	mem_level level_data[3];

	// �ϋv�̃f�[�^(�Q�[�����x����)
	mem_taiQ taiQ_data[3];

	// �^�C���A�^�b�N�̃f�[�^(�Q�[�����x����)
	mem_timeAt timeAt_data[3];

	// ���s���̍ő�l
	#define MEM_VS_RESULT_MAX 99

	// VSCOM�̏��s[0:���� 1:�s�k]
	u16 vscom_data[2];

	// VSCOM(FLASH)�̏��s[0:���� 1:�s�k]
	u16 vc_fl_data[2];

	// 2PLAY�̏��s[0:���� 1:�s�k]
	u16 vsman_data[2];

	// 2PLAY(FLASH)�̏��s[0:���� 1:�s�k]
	u16 vm_fl_data[2];

	// 2PLAY(TIME ATTACK)�̏��s[0:���� 1:�s�k]
	u16 vm_ta_data[2];

	// �e�Q�[���̐ݒ�
	game_config config;

} mem_char;

// �e�l�̃f�[�^�ϐ�
#define DM_MEM_GUEST 0x08 // �Q�X�g�p�z��ԍ�
#define REC_CHAR_SIZE 8   // �Z�[�u����z��
#define MEM_CHAR_SIZE 9   // �Q�X�g���܂ޔz��
extern mem_char evs_mem_data[MEM_CHAR_SIZE];

// 4P�e�ݒ�̕ۑ��p�ϐ�
extern game_cfg_4p evs_cfg_4p;

//////////////////////////////////////////////////////////////////////////////
//{### �e��f�[�^����

extern void dm_story_sort_set(int player_no, int char_no, int g_level, int score, int time, int c_stage, int ignoreFlag);
extern void dm_level_sort_set(int player_no, int g_speed, int score, int c_level);

extern void dm_taiQ_sort_set(int player_no, int dif, int score, int time);
extern void dm_timeAt_sort_set(int player_no, int dif, int score, int time, int erase);

extern void dm_vscom_set(int player_no, int win, int lose);
extern void dm_vc_fl_set(int player_no, int win, int lose);

extern void dm_vsman_set(int player_no, int win, int lose);
extern void dm_vm_fl_set(int player_no, int win, int lose);
extern void dm_vm_ta_set(int player_no, int win, int lose);

//////////////////////////////////////////////////////////////////////////////
//{### �Z�[�u�f�[�^�������֐��Q

extern void dm_init_config_save(game_config *config);
extern void dm_init_config_4p_save(game_cfg_4p *config);
extern void dm_init_story_save(mem_story *st);
extern void dm_init_level_save(mem_level *st);
extern void dm_init_save_mem(mem_char *mem);
extern void dm_init_system_mem();

//////////////////////////////////////////////////////////////////////////////
//{### �e��f�[�^���\�[�g

typedef struct SRankSortInfo {
	u8 story_sort[3][REC_CHAR_SIZE];
	u8 story_rank[3][REC_CHAR_SIZE];

	u8 level_sort[3][REC_CHAR_SIZE];
	u8 level_rank[3][REC_CHAR_SIZE];

	u8 taiQ_sort[3][REC_CHAR_SIZE];
	u8 taiQ_rank[3][REC_CHAR_SIZE];

	u8 timeAt_sort[3][REC_CHAR_SIZE];
	u8 timeAt_rank[3][REC_CHAR_SIZE];

	u8  vscom_sort[REC_CHAR_SIZE];
	u8  vscom_rank[REC_CHAR_SIZE];
	u16 vscom_ave[REC_CHAR_SIZE]; // VSCOM �̏���������

	u8  vc_fl_sort[REC_CHAR_SIZE];
	u8  vc_fl_rank[REC_CHAR_SIZE];
	u16 vc_fl_ave[REC_CHAR_SIZE]; // VSCOM(FLASH) �̏���������

	u8  vsman_sort[REC_CHAR_SIZE];
	u8  vsman_rank[REC_CHAR_SIZE];
	u16 vsman_ave[REC_CHAR_SIZE]; // VSMAN �̏���������

	u8  vm_fl_sort[REC_CHAR_SIZE];
	u8  vm_fl_rank[REC_CHAR_SIZE];
	u16 vm_fl_ave[REC_CHAR_SIZE]; // VSMAN(FLASH) �̏���������

	u8  vm_ta_sort[REC_CHAR_SIZE];
	u8  vm_ta_rank[REC_CHAR_SIZE];
	u16 vm_ta_ave[REC_CHAR_SIZE]; // VSMAN(TIME ATTACK) �̏���������
} SRankSortInfo;

extern void dm_data_mode_story_sort(SRankSortInfo *st);
extern void dm_data_mode_level_sort(SRankSortInfo *st);

extern void dm_data_mode_taiQ_sort(SRankSortInfo *st);
extern void dm_data_mode_timeAt_sort(SRankSortInfo *st);

extern void dm_data_vscom_sort(SRankSortInfo *st);
extern void dm_data_vc_fl_sort(SRankSortInfo *st);

extern void dm_data_vsman_sort(SRankSortInfo *st);
extern void dm_data_vm_fl_sort(SRankSortInfo *st);
extern void dm_data_vm_ta_sort(SRankSortInfo *st);

//////////////////////////////////////////////////////////////////////////////
//{### ���̑�

//## ���O�̃Z���^�����O���s���̂ɕK�v�ȏ����擾
extern void fontName_getRange(const unsigned char *name, int *offset, int *size);

//////////////////////////////////////////////////////////////////////////////
//{### EEPRom ������

//## osContInit ����ɌĂт܂�
extern EepRomErr EepRom_Init();

//## �o�׎��̏�����
extern EepRomErr EepRom_InitFirst(void (*proc)(void *), void *args);

//## �ϐ���������
extern void EepRom_InitVars();

//////////////////////////////////////////////////////////////////////////////
//{### �e�\���� <-> EEPRom

//## �S�ǂ�
extern EepRomErr EepRom_ReadAll();

//## �S����
extern EepRomErr EepRom_WriteAll(void (*proc)(void *), void *args);

//////////////////////////////////////////////////////////////////////////////
//{### �f�o�b�O�p

//## �G���[���b�Z�[�W���o��
extern void EepRom_DumpErrMes(EepRomErr err);

//## �e�f�[�^�̃T�C�Y���o��
extern void EepRom_DumpDataSize();

//////////////////////////////////////////////////////////////////////////////
//{### EEPROM�������ݒ��̌x�����b�Z�[�W

typedef struct {
	SMsgWnd msgWnd;
	int timeout;
	int count;
} RecWritingMsg;

extern void RecWritingMsg_init(RecWritingMsg *st, void **heap);
extern void RecWritingMsg_setStr(RecWritingMsg *st, const unsigned char *str);
extern void RecWritingMsg_calc(RecWritingMsg *st);
extern void RecWritingMsg_draw(RecWritingMsg *st, Gfx **gpp);
extern void RecWritingMsg_start(RecWritingMsg *st);
extern void RecWritingMsg_end(RecWritingMsg *st);
extern int  RecWritingMsg_isEnd(RecWritingMsg *st);
extern void RecWritingMsg_setPos(RecWritingMsg *st, int x, int y);

//////////////////////////////////////////////////////////////////////////////
//{### �X���[�v�^�C�}�[

extern void setSleepTimer(int msec);

//////////////////////////////////////////////////////////////////////////////
//{### EOF

#endif // _RECORD_H_
