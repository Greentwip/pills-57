// ///////////////////////////////////////////////////////////////////////////
//
//	Story-mode Program File
//
//	Created by Shizuoka?
//	Modifed by Katsuyuki Ohkura
//
//	Modifed by Hiroyuki Watanabe
//
//	Last Update: 1999/12/11
//
// ///////////////////////////////////////////////////////////////////////////
#include <ultra64.h>
#include <PR/gs2dex.h>
#include "def.h"
#include "vram.h"
#include "segment.h"
#include "message.h"
#include "nnsched.h"
#include "main.h"
#include "audio.h"
#include "graphic.h"
#include "joy.h"
#include "static.h"
#include "evsworks.h"
#include "dm_code_data.h"

#include "o_segment.h"
#include "o_static.h"
#include "bg_dat_extern.h"
#include "story_data.h"
#include "st_message.h"


extern u8 face_number;			// ��ԍ�

enum {				//	��������̔ԍ�
	TSTAT_OPEN,
	TSTAT_ANIMA,
	TSTAT_SET,
	TSTAT_LOOP,
	TSTAT_GAME,
	TSTAT_DEMO,
	TSTAT_CLOSE,
	TSTAT_LOOPEND,
};
typedef struct {
	u8 *st_seg;		// �Z�O�����g�n��
	u8 *ed_seg;		// �Z�O�����g�I��
	PIC tile;		// �^�C���p�^�[��
	u8 bg_num;		// �w�i��
} ST_STAGE_DAT;

static uObjBg st_bg[12];			// �w�i
static uObjBg st_ch[12];			// �����
static uObjBg st_cld;				// �_
static u8 st_bg_all_num = 0;		// ���̖ʂ̊G�͔z��̉��ԖځH
static u8 now_story = 0;			// �X�g�[���[�ԍ�
static u8 st_mode_flow = 0;			// �X�g�[���[���[�h���ꐧ��
static u8 tile_adj_y = 0;			// �^�C���x���W�����p
static u8 pic_num = 0;				// �w�i����
static u8 *addr_pat[12];			// �w�i�e�N�X�`���A�h���X
static u16 *addr_pal[12];			// �w�i�p���b�g�A�h���X
static s16 comple_y = 0;			// �S�̒����p�x���W
static s16 p1_x = 200,p1_y = 50;	// �L�����P���W
static s16 bg_x = 0,  bg_y = 0;		// �w�i���W
static s16 cl_x = 29, cl_y = 163;	// �_���W
static u8 cannon_on = 0;			// ��C���ˁI�I
static s8 pino_mouth_flg;			// �s�m�L�I�N�`�p�N�t���O
static s8 pino_mouth_count;			// �s�m�L�I�N�`�p�N�J�E���^
static ST_STAGE_DAT *st_p;			// �X�e�[�W�f�[�^�|�C���^

// ���b�Z�[�W( ���͂̐��䕶���� st_message.c ��)

static	u8	st_mes_01[] = "���ۂ��ۂ��@���Ă񂫉����Ђ�ˁ@�}���I�B�~������ǂ��@���C�[�W�@��������āB�~���Ȃɂ�����ȂɁ@����ĂĂ���́B�~����13��04���C�[�W���ɂ�����I�@�ɂ�����I���s�[�`�Ђ߂������ւ񂾁I�~\
�Ŋ�01�������₷��˂ނ�s�[�`�Ђ߁B�~���L�m�s�I�����́@����ς������B���~�����ց@�}���I���@����Ă����B�~����00��04�L�m�s�I�������}���I����I���݂Ă��������~���Ђ߂��������ւ�Ȃ��ƂɁE�E�B�~\
�Ŋ�01���s�[�`�Ђ߂���������ۂ��イ�~�����˂ނ����́@�͂Ȃ����������������ł��B�~�����ꂩ�炸���Ɓ@�Ђ߂͉��˂ނ���ςȂ��B�~\
�Ŋ�01���ł��@���͂悤�����̉��͂Ȃ�����΁@�������傤�Ԏ~�Ђ߂����������Ƃ����ł���͂��ł��B�~���Ƃ��낪�E�E�E�B�~\
�Ŋ�01�����͂悤�����́@���ꂩ�ɉ����������Ă��܂����B�~�����̂����Ɂ@���̂Ă��݂����̂�����Ă����̂ł��B�~\
�Ŋ�01����11��03�N�b�p���K�n�n�I�I������˂񂾂����ȁ@�}���I�I�~���͂悤�����́@�킪�͂��������΂��Ă��������ˁI�~�����܂��݂����ȃ��u������ɉ��悤�͂Ȃ��I�@�K�n�n�I�~\
�Ŋ�01�����܂����@�N�b�p�̂�邾���݁B�~\
�Ŋ�01���h�N�^�[�}���I�̉��ł΂񂾂��I�I�~\
�Ŋ�01��������Ăā@�Ƃт������h�N�^�[�}���I�B�~���߂����͉��܂����N�b�p�́@������B�~�����͂悤�������@�Ƃ���ǂ��I�~\
�Ŋ�01���������Ɓ@�������@�}���I�̉��܂��Ɂ@�Ђ������@�m�R�m�R�������ꂽ�I�~����02��04�m�R�m�R���}���I���񂶂���Ȃ��b�X���I�~���ЂƂI�C���Ɖ����傤�Ԃ��܂��񂩁H�~�n��01";

static u8 st_mes_02[] = "���N�b�p�́@������͉��܂��܂��@�Ƃ����B�~���܂��́@���������@�������炯�B�~���Ƃ���@�����̉��ق������@�h�J�[�[���I�I��08�~\
�Ŋ�01���Ȃɂ����@������ց@�Ƃ�ł���B�~���Ƃ�Ł@�����͉̂��{���ւ��@�݂����I�H�~����03��02�{���ւ�����������@�����ւ͉��Ƃ����܂���I�I�~�n��01";

static u8 st_mes_03[] = "���Ƃ��Ƃ��@���݂܂Ł@����Ă����B�~�����݂̂ނ����́@�N�b�p�Ƃ��B�~���}���I�́@���݂Ɂ@�Ƃт��񂾁I�I�~\
�Ŋ�01���ӂ����@�ӂ����@���݂̂����B�~�����ꂢ���Ȃ�ā@�������Ă�������������ȁ@�������ȁ@����Ă����B�~���������Ȃӂ��́@�v�N�v�N�͉��Ȃ�ł��@���ׂ��Ⴄ����������ڂ��B�~\
����04��04�v�N�v�N�����Ȃ����@�������ȁ[�����ׂ��́@�����Ăȁ[���H�~�������ĂȂ��Ȃ�����܂����@���ׂ��Ⴄ���B�~�n��01";

static u8 st_mes_04[] = "������Ƃ́@�������Ł@���݂��@�������N�b�p�̂��܂Ɂ@���ǂ���B�~���z�b�ƂЂƂ����@������}���I�B�~��������͂܂����Ɓ@�߂����炷�B�~���������Ȃ��߂߂ɂ���͉̂������܂łƂǂ��@��������܁B�~�������Ȃ���I�I�~\
�Ŋ�01������܂Ł@�Â��@�����[���@��܉���������@����������̂ڂ��Ă����Ǝ~���Ƃ���@���߂񂪉������������~���Ȃ�������`�����v�[�@�Ƃт������I�~\
����05��05�`�����v�[���r�b�N�������H�I�~���I�����܂Ɓ@���傤�Ԃ��I�~�n��01";

static u8 st_mes_05[] = "������ȁ@��܂݂����ւ������}���I�@���񂫂����ς��������̂ڂ�I�~�������������܂��ā@�����̂����B�~\
�Ŋ�01�����������Ȃ��悤�@���イ�����ĉ��X�C�X�C�@���������@�����݂̂��B�~���������ā@����̂́@�Ȃ�̂��ƁH�~������́@�����܂Ł@������܂ނ��B�~\
����06��08�v���y���w�C�z�[���E�E�E�E�E�E�E�B�~�n��01";

static u8 st_mes_06[] = "�������܂Ł@�Ƃǂ��@�Ȃ��[���c�^�B�~���������@�������@����Ă݂�B�~�������́@�����߂�@�ӂ�������B�~\
�Ŋ�01�������܂ł���΁@�������傤�ԁB�~���}���I�́@�c�^����@�Ƃт��肽�I�~���Ƃт��肽�܂ł͂悩��������������ۃn�i�����@�ӂ�Â����B�~\
����07��05�n�i���������[�[����ӂ�Â���ꂿ�Ⴂ�܂����B�~�n�i�����@�Ƃ��Ă������������ł��I�~�n��01";

static u8 st_mes_07[] = "���ӂ����@�ӂ����@����̂Ȃ����ӂ邢�₵�����݂�����B�~\
�Ŋ�01��������́@�܂�������Ȃ�ɂ��@�݂���B�~���������Ȃт���������񂯂�}���I�B�~���Ȃ񂾂��@�����������ނ��Ȃ��H�~���Ƃ��Ă��Ƃ��Ă��@���Ȃ��񂶁B�~\
����08��03�e���T���P�P�P�P�P�b�I�~�n��01";

static u8 st_mes_08[] = "���₵���Ł@�݂������Ђ݂́@����B�~���������@�����߂Ή������݂��@�����ˁB�~\
�Ŋ�01������[���@����[���@��������������ɂ́@�ǂ��񂪁@����܂����B�~���C���ȁ@�悩��͉��X�o���@�Ă����イ�I�������ꂽ�̂́@�p�b�N���t�����[�~\
����09��08�p�b�N���t�����[���p�b�N���@�p�b�N�����y���[���@�y�����~�n��01";

static u8 st_mes_09[] = "���ǂ�����@�ʂ���Ɖ��A�b�`�b�`�b�I�I�������͂��������@�ǂ����낤�H�~\
�Ŋ�01���Ȃ�Ɓ@�r�b�N�����N�b�p�́@������B�~�������܂Ł@����Ή������@�炭���傤�H�~�������́@�Ƃ�₪�@���낵�܂���I�~\
����10��04�J���b�N���L�[�b�I�I�~���܂����@����ȂƂ��܂ŉ�����Ă���Ȃ�āB�~���N�b�p���ڂ�����܂̉��Ƃ���ւ́@�������܂���I�~�n��01";

static u8 st_mes_10[] = "��������́@�Ȃ��́@���i���炯���h�N�^�[�}���I�@���������ςI�I�~\
�Ŋ�01�������Ƃ���́@���͂悤�����������΂����@���傤�ق�ɂ�~���}���I�́@���キ�Ă����N�b�p�́@���ւ�B�~���������́@���������̉��͂��܂�ł��I�~\
����11��03�N�b�p���������E�E�E�@�}���I����������Ă����̂��B�~�������������܂��Ȃ񂩂��~���킪�͂��Ɂ@���Ȃ��킯�����Ȃ���I�@�K�n�n�b�I�I�~\
�����͂悤�������@���[���������킽�������ˁI�~���@�@�@�@�@�@�@�������I�I�~�n��01";

static u8 st_mes_ed[] = "�����͂悤�������@�Ăɂ��ꂽ���}���I�́@�����������s�[�`�̂���Ɂ@�ނ����A�~���Ԃ��@�s�[�`�����߂��߂����邱�Ƃ����ł��܂����B�~�n��01";



// �X�e�[�W�f�[�^
static ST_STAGE_DAT st_dat[] = {
//	�����J�n�A�h���X�E�����I���A�h���X�E�^�C���O���t�B�b�N�f�[�^�A�h���X�E�^�C���p���b�g�f�[�^�A�h���X�E�g�p�w�i��
	_bg_dat_01SegmentRomStart, _bg_dat_01SegmentRomEnd, tile_01_bm0_0, tile_01_bm0_0tlut, 10,
	_bg_dat_02SegmentRomStart, _bg_dat_02SegmentRomEnd, tile_02_bm0_0, tile_02_bm0_0tlut, 2,
	_bg_dat_03SegmentRomStart, _bg_dat_03SegmentRomEnd, tile_03_bm0_0, tile_03_bm0_0tlut, 2,
	_bg_dat_04SegmentRomStart, _bg_dat_04SegmentRomEnd, tile_04_bm0_0, tile_04_bm0_0tlut, 2,
	_bg_dat_05SegmentRomStart, _bg_dat_05SegmentRomEnd, tile_05_bm0_0, tile_05_bm0_0tlut, 2,
	_bg_dat_06SegmentRomStart, _bg_dat_06SegmentRomEnd, tile_05_bm0_0, tile_05_bm0_0tlut, 2,
	_bg_dat_07SegmentRomStart, _bg_dat_07SegmentRomEnd, tile_07_bm0_0, tile_07_bm0_0tlut, 2,
	_bg_dat_08SegmentRomStart, _bg_dat_08SegmentRomEnd, tile_08_bm0_0, tile_08_bm0_0tlut, 2,
	_bg_dat_09SegmentRomStart, _bg_dat_09SegmentRomEnd, tile_08_bm0_0, tile_08_bm0_0tlut, 2,
	_bg_dat_10SegmentRomStart, _bg_dat_10SegmentRomEnd, tile_10_bm0_0, tile_10_bm0_0tlut, 3,
	_bg_dat_edSegmentRomStart, _bg_dat_edSegmentRomEnd, tile_10_bm0_0, tile_10_bm0_0tlut, 1,
};


// ��z��
static PIC faces[] = {
///	{ st_face_ff_bm0_0, st_face_ff_bm0_0tlut },		// �~
	{ st_face_13_bm0_0, st_face_13_bm0_0tlut },		// �L�m�s�I
	{ st_face_01_bm0_0, st_face_01_bm0_0tlut },		// �}���I
	{ st_face_02_bm0_0, st_face_02_bm0_0tlut },		// �m�R�m�R
	{ st_face_03_bm0_0, st_face_03_bm0_0tlut },		// �{����
	{ st_face_04_bm0_0, st_face_04_bm0_0tlut },		// �v�N�v�N
	{ st_face_05_bm0_0, st_face_05_bm0_0tlut },		// �`�����u�[
	{ st_face_06_bm0_0, st_face_06_bm0_0tlut },		// �v���y���w�C�z�[
	{ st_face_07_bm0_0, st_face_07_bm0_0tlut },		// �n�i�����
	{ st_face_08_bm0_0, st_face_08_bm0_0tlut },		// �e���T
	{ st_face_09_bm0_0, st_face_09_bm0_0tlut },		// �p�b�N���t�����[
	{ st_face_10_bm0_0, st_face_10_bm0_0tlut },		// �J���b�N
	{ st_face_11_bm0_0, st_face_11_bm0_0tlut },		// �N�b�p
	{ st_face_12_bm0_0, st_face_12_bm0_0tlut },		// �s�[�`
	{ st_face_14_bm0_0, st_face_14_bm0_0tlut },		// ���C�[�W
};


// �w�i�f�[�^
static PIC2 st_bg_data[] = {
	{ st_back_op_1_bm0_0, st_back_op_1_bm0_0tlut, 240, 160,40, 0 },		//	1-1
	{ st_back_op_2_bm0_0, st_back_op_2_bm0_0tlut, 240, 160,40, 0 },		//	1-2
	{ st_back_op_3_bm0_0, st_back_op_3_bm0_0tlut, 160, 160,80, 0 },		//	1-3
	{ st_back_op_4_bm0_0, st_back_op_4_bm0_0tlut, 240, 160,40, 0 },		//	1-4
	{ st_back_op_5_bm0_0, st_back_op_5_bm0_0tlut, 240, 160,40, 0 },		//	1-5
	{ st_back_op_6_bm0_0, st_back_op_6_bm0_0tlut, 240, 160,40, 0 },		//	1-6
	{ st_back_op_7_bm0_0, st_back_op_7_bm0_0tlut, 160, 160,80, 0 },		//	1-7
	{ st_back_op_8_bm0_0, st_back_op_8_bm0_0tlut, 240, 160,40, 0 },		//	1-8
	{ st_back_01_1_bm0_0, st_back_01_1_bm0_0tlut, 240, 160,40, 0 },		//	1-9
	{ st_back_01_2_bm0_0, st_back_01_2_bm0_0tlut, 320, 160, 0, 0 },		//	1-10
	{ st_back_02_1_bm0_0, st_back_02_1_bm0_0tlut, 160, 160,80, 0 },		//	2-1
	{ st_back_02_2_bm0_0, st_back_02_2_bm0_0tlut, 320, 160, 0, 0 },		//	2-2
	{ st_back_03_1_bm0_0, st_back_03_1_bm0_0tlut, 240, 160,40, 0 },		//	3-1
	{ st_back_03_2_bm0_0, st_back_03_2_bm0_0tlut, 320, 160, 0, 0 },		//	3-2
	{ st_back_04_1_bm0_0, st_back_04_1_bm0_0tlut, 240, 160,40, 0 },		//	4-1
	{ st_back_04_2_bm0_0, st_back_04_2_bm0_0tlut, 320, 160, 0, 0 },		//	4-2
	{ st_back_05_1_bm0_0, st_back_05_1_bm0_0tlut, 240, 160,40, 0 },		//	5-1
	{ st_back_05_2_bm0_0, st_back_05_2_bm0_0tlut, 320, 160, 0, 0 },		//	5-2
	{ st_back_06_1_bm0_0, st_back_06_1_bm0_0tlut, 160, 160,80, 0 },		//	6-1
	{ st_back_06_2_bm0_0, st_back_06_2_bm0_0tlut, 320, 160, 0, 0 },		//	6-2
	{ st_back_07_1_bm0_0, st_back_07_1_bm0_0tlut, 240, 160,40, 0 },		//	7-1
	{ st_back_07_2_bm0_0, st_back_07_2_bm0_0tlut, 320, 160, 0, 0 },		//	7-2
	{ st_back_08_1_bm0_0, st_back_08_1_bm0_0tlut, 160, 160,80, 0 },		//	8-1
	{ st_back_08_2_bm0_0, st_back_08_2_bm0_0tlut, 320, 160, 0, 0 },		//	8-2
	{ st_back_09_1_bm0_0, st_back_09_1_bm0_0tlut, 160, 160,80, 0 },		//	9-1
	{ st_back_09_2_bm0_0, st_back_09_2_bm0_0tlut, 320, 160, 0, 0 },		//	9-2
	{ st_back_10_1_bm0_0, st_back_10_1_bm0_0tlut, 240, 160,40, 0 },		//	10-1
	{ st_back_10_2_bm0_0, st_back_10_2_bm0_0tlut, 320, 160, 0, 0 },		//	10-2
	{ st_back_03_2_bm0_0, st_back_03_2_bm0_0tlut, 320, 160, 0, 0 },		//	10-3(�_�~�[)
	{ st_back_ed_1_bm0_0, st_back_ed_1_bm0_0tlut, 240, 160,40, 0 },		//	ED
};


// �L�����f�[�^
static PIC2 st_ch_data[] = {
	{ st_char_01_21_bm0_0, st_char_01_21_bm0_0tlut, 96,140, 181, 18 },		// 1-2 �}���I
	{ st_char_01_22_bm0_0, st_char_01_22_bm0_0tlut, 72, 94,	 56, 63 },		// 1-2 �m�R�m�R
	{ st_char_02_21_bm0_0, st_char_02_21_bm0_0tlut,104,130, 182, 25 },		// 2-2 �}���I
	{ st_char_02_22_bm0_0, st_char_02_22_bm0_0tlut, 88,117,	 61, 44 },		// 2-2 �{����
	{ st_char_03_21_bm0_0, st_char_03_21_bm0_0tlut,128, 84,	 24, 38 },		// 3-2 �}���I
	{ st_char_03_22_bm0_0, st_char_03_22_bm0_0tlut, 96, 93, 203, 23 },		// 3-2 �v�N�v�N
	{ st_char_04_21_bm0_0, st_char_04_21_bm0_0tlut,128,130, 168, 28 },		// 4-2 �}���I
	{ st_char_04_22_bm0_0, st_char_04_22_bm0_0tlut, 88, 69,	 42, 50 },		// 4-2 �`�����u�[
	{ st_char_05_21_bm0_0, st_char_05_21_bm0_0tlut,120,138, 165, 14 },		// 5-2 �}���I
	{ st_char_05_22b_bm0_0, st_char_05_22b_bm0_0tlut,48,29,	 60,120 },		// 5-2 �w�C�z�[�e
	{ st_char_05_22_bm0_0, st_char_05_22_bm0_0tlut, 64, 83,	 56, 32 },		// 5-2 �w�C�z�[
	{ st_char_06_21_bm0_0, st_char_06_21_bm0_0tlut,120,116, 144,  2 },		// 6-2 �}���I
	{ st_char_06_22_bm0_0, st_char_06_22_bm0_0tlut,224,149,	 58, 12 },		// 6-2 �͂Ȃ����
	{ st_char_07_21_bm0_0, st_char_07_21_bm0_0tlut,176,158, 142,  3 },		// 7-2 �}���I
	{ st_char_07_22_bm0_0, st_char_07_22_bm0_0tlut,120,113,	 61, 41 },		// 7-2 �e���T
	{ st_char_08_21_bm0_0, st_char_08_21_bm0_0tlut,128,160, 142,  1 },		// 8-2 �}���I
	{ st_char_08_22_bm0_0, st_char_08_22_bm0_0tlut, 88, 72,	 52,  2 },		// 8-2 �p�b�N���t�����[
	{ st_char_09_21_bm0_0, st_char_09_21_bm0_0tlut,112,131, 195, 30 },		// 9-2 �}���I
	{ st_char_09_22_bm0_0, st_char_09_22_bm0_0tlut,128, 99,	 33, 54 },		// 9-2 �J���b�N
	{ st_char_10_31_bm0_0, st_char_10_31_bm0_0tlut,128,144, 195, 17 },		//10-3 �}���I
	{ st_char_10_32_bm0_0, st_char_10_32_bm0_0tlut,152,160,	 20,  1 },		//10-3 �N�b�p
};


// �w�i�ƃL�����̑Ή�
static u8 st_scene_data[][3] = {
	 0,255,255,		// op-1
	 0,255,255,		// op-2
	 0,255,255,		// op-3
	 0,255,255,		// op-4
	 0,255,255,		// op-5
	 0,255,255,		// op-6
	 0,255,255,		// op-7
	 0,255,255,		// op-8
	 1,255,255,		//	1-1
	 2,0,1,			//	1-2
	 3,255,255,		//	2-1
	 4,2,3,			//	2-2
	 5,255,255,		//	3-1
	 6,4,5,			//	3-2
	 7,255,255,		//	4-1
	 8,6,7,			//	4-2
	 9,255,255,		//	5-1
	10,8,9,			//	5-2
	11,255,255,		//	6-1
	12,11,12,		//	6-2
	13,255,255,		//	7-1
	14,13,14,		//	7-2
	15,255,255,		//	8-1
	16,15,16,		//	8-2
	17,255,255,		//	9-1
	18,17,18,		//	9-2
	19,255,255,		// 10-1
	20,19,20,		// 10-2
	21,255,255,		// 10-3
	22,255,255,		// ed-1
};


// ���b�Z�[�W�e�[�u��
static u8 *st_meses[11] = {
	st_mes_01,
	st_mes_02,
	st_mes_03,
	st_mes_04,
	st_mes_05,
	st_mes_06,
	st_mes_07,
	st_mes_08,
	st_mes_09,
	st_mes_10,
	st_mes_ed,
};




// �a�f�p�����[�^�Zq�b�g
void set_bg_param( uObjBg *bg, u8 *tex, u16 load, u8 size, u16 width, u16 height, s16 posx, s16 posy, u16 scx, u16 scy )
{
	bg->s.imageX		= 0		<< 5;		// BG image �̍���ʒu X ���W  (u10.5)
	bg->s.imageW		= width << 2;		// BG image �̕�			   (u10.2)
	bg->s.frameX		= posx	<< 2;		// �]���t���[���̍���ʒu	   (s10.2)
	bg->s.frameW		= width << 2;		// �]���t���[���̕�			   (u10.2)
	bg->s.imageY		= 0		 << 5;		// BG image �̍���ʒu Y ���W  (u10.5)
	bg->s.imageH		= height << 2;		// BG image �̍���			   (u10.2)
	bg->s.frameY		= posy	 << 2;		// �]���t���[���̍���ʒu	   (s10.2)
	bg->s.frameH		= height << 2;		// �]���t���[���̍���		   (u10.2)
	bg->s.imagePtr		= (u64 *)tex;		// BG image ����p�̃e�N�X�`���A�h���X
	bg->s.imageLoad		= load;				// LoadBlock, LoadTile �̂ǂ�����g�p���邩
	bg->s.imageFmt		= G_IM_FMT_CI;		// BG image �̃t�H�[�}�b�g	   G_IM_FMT_*
	bg->s.imageSiz		= size;				// BG image �̃T�C�Y		   G_IM_SIZ_*
	bg->s.imagePal		= 0;				// �p���b�g�ԍ�
	bg->s.imageFlip		= 0;				// �C���[�W�̍��E���] G_BG_FLAG_FLIPS �Ŕ��]
	bg->s.scaleW		= scx;				// X �����X�P�[���l			   (u5.10)
	bg->s.scaleH		= scy;				// Y �����X�P�[���l			   (u5.10)
	bg->s.imageYorig	= 0 << 5;			// image �ɂ�����`��n�_	   (s20.5)
}

// �g�k�����W�␳
static void correct_pos( uObjBg *bg, s16 pos, u16 size, u8 vh)
{
	s16 correct;
	f32 f_scale;

	if( !vh ){	// ��
		f_scale = 1024.0 / (f32)bg->s.scaleW;
		correct = (s16)(size - size * f_scale) / 2;
		bg->s.frameX = ( pos + correct ) << 2;
		bg->s.frameW = (u16)( size * f_scale ) << 2;
	} else {	// �c
		f_scale = 1024.0 / (f32)bg->s.scaleH;
		correct = (s16)(size - size * f_scale) / 2;
		bg->s.frameY = ( pos + correct ) << 2;
		bg->s.frameH = (u16)( size * f_scale ) << 2;
	}
}

// ��ʗh��
void make_pos( void )
{
	s16 data[] = {6,5,-3,-3,-2,-2,-1,0,3,2,-2,-1,-1,-1,0};
	static u8 cnt = 0;

	if( cannon_on ){
		bg_y += data[cnt++];
		if( cnt >= 15 ){
			cnt = 0;
			cannon_on = 0;
		}
	}
}

// �^�C���\��
void disp_tile( void )
{
	int i,j;

	gDPLoadTLUT_pal256( gp++,st_p->tile.pal );
	gDPLoadTextureTile_4b( gp++, st_p->tile.pat, G_IM_FMT_CI,80,40,0,0,80-1,40-1,
					0, G_TX_WRAP, G_TX_CLAMP, G_TX_NOMASK,G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

	st_scr_stop = 1;	//	������̐i�s��~
	switch( (st_flag & 0x7) ){
		case 0:			// ���ɂ�����
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			comple_y = 0;
			tile_adj_y = ( tile_adj_y + 5 >160 )?160:tile_adj_y + 5;
			if( tile_adj_y == 160 ){	//	��ʒu�܂ŉ���������
				st_scr_stop = 0;		//	������̐i�s�J�n
			}
			break;
		case 1:			// ��ɂ�����
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			if( tile_adj_y == 0 ){		//	��ԏ�܂ł���������
				if( st_page != 255 ){	//	�I����ԂłȂ��ꍇ
					pic_num = st_page;	//	�w�i�ԍ������̂��̂Ɠ���ւ���
					st_flag = 0;		//	�t���O�̃N���A
				} else {				//	�I����ԏꍇ
					st_flag = 4;		//	���̂܂܃A�E�g
				}
			}
			comple_y = 0;
			tile_adj_y = ( tile_adj_y - 5 < 0 )?0:tile_adj_y - 5;
			break;
		case 3:
		case 4:
			for( j = 0; j < 6; j ++ ){
				for( i = 0; i < 4; i ++ ){
					gSPScisTextureRectangle( gp++, (i*80)<<2, (j*40+tile_adj_y)<<2, (i*80+80)<<2, (j*40+40+tile_adj_y)<<2,
									G_TX_RENDERTILE, 0, 0, 1<<10, 1<<10);
				}
			}
			break;
	}

};

// ���C��
void main50(NNSched* sched)
{
	OSMesgQueue msgQ;
	OSMesg		msgbuf[MAX_MESGS];
	NNScClient	client;
	PIC2 *picp;
	int i,j;
	s16	w_size,h_size,sin_f;
	u8	loop_flg = TRUE;

	osCreateMesgQueue(&msgQ, msgbuf, MAX_MESGS);	// create message queue for VI retrace
	nnScAddClient(sched, &client, &msgQ);			// add client to shceduler


FIRST_POS:

	st_mode_flow = 0;
	tile_adj_y = 0;
	pic_num = 0;
	comple_y = 0;
	p1_x = 200;
	p1_y = 50;
	bg_x = 0;
	bg_y = 0;
	cl_x = 29;
	cl_y = 167;
	sin_f = 0;
	pino_mouth_flg = 0;
	pino_mouth_count = 0;

	init_font();								//	������\���Ɋւ���ϐ��̏�����
	set_intencity_rgb( 0,0,0,255,255,255 );		//	������\���Ɋւ���ϐ��̏�����

	loop_flg =TRUE;
	face_number = 0;

	now_story = evs_story_no;	//	���݂̃X�g�[���[�ԍ�������ϐ��ɃZ�b�g����
	st_p = &st_dat[now_story];	//	�X�g�[���[�̕����������ϐ��ɃZ�b�g����

	// �f�[�^�ǂݍ���
	auRomDataRead( (u32)_story_dataSegmentRomStart, gfx_freebuf[GFX_SPRITE_PAGE], (u32)(_story_dataSegmentRomEnd - _story_dataSegmentRomStart));	// �������[�h
	auRomDataRead( (u32)st_p->st_seg,gfx_freebuf[GFX_SPRITE_PAGE + 2], (u32)(st_p->ed_seg - st_p->st_seg ));		// �w�i���[�h

	//	���݂̃X�e�[�W�܂ł̔w�i�̖������v�Z���Ă���B
	for( i = 0,st_bg_all_num = 0; i < now_story; i ++ ){
		st_bg_all_num += st_dat[i].bg_num;
	}
	// �G�p�����[�^�Z�b�g
	for( i = 0; i < st_p->bg_num; i ++ ){
		j = st_bg_all_num + i;
		picp = &st_bg_data[st_bg_all_num + i];
		// �w�i�Z�b�g
		set_bg_param( &st_bg[i],picp->pat,G_BGLT_LOADTILE,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1<<10,1<<10 );

		// �L�����Z�b�g
		if( st_scene_data[j][1] != 255 ){
			picp = &st_ch_data[st_scene_data[j][1]];
			set_bg_param( &st_ch[i*2],picp->pat,G_BGLT_LOADBLOCK,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1024,1024 );
		}
		if( st_scene_data[j][2] != 255 ){
			picp = &st_ch_data[st_scene_data[j][2]];
			set_bg_param( &st_ch[i*2+1],picp->pat,G_BGLT_LOADBLOCK,G_IM_SIZ_8b,picp->width,picp->height,picp->pos_x,picp->pos_y,1024,1024 );
		}
	}
	// �_�Z�b�g
	set_bg_param( &st_cld,st_mes_cloud_bm0_0,G_BGLT_LOADTILE,G_IM_SIZ_4b,272,64,cl_x,cl_y,1024-100,1024-100 );

	i = ( cl_y+comple_y >= 240 )?239:cl_y+comple_y;		//	�_��\������x���W�̌v�Z
	h_size = (s16)( sinf( DEGREE( sin_f ) ) * 5 );		//	���������l�̌v�Z
	w_size = (s16)( sinf( DEGREE( sin_f ) ) * 15 );		//	�c�������l�̌v�Z
	st_cld.s.scaleW = (272 << 10) / (300 + w_size);		//	������ S2DEX �̕ϐ��ɐݒ�
	st_cld.s.scaleH = (64 << 10) / (70 + h_size);		//	�c���� S2DEX �̕ϐ��ɐݒ�
	correct_pos( &st_cld, cl_x, 272, 0 );				//	�w���W�̒���
	correct_pos( &st_cld, i, 64, 1 );					//	�x���W�̒���

	// ���b�Z�[�W�Z�b�g
	font_make( st_meses[now_story] );					//	������f�[�^�݂̂��O���t�B�b�N�ɒu�������鏈��

	// ���������낢��
	for( i=0;i<MAXCONTROLLERS;i++ ) joyflg[i] =( DM_KEY_UP | DM_KEY_DOWN | DM_KEY_LEFT | DM_KEY_RIGHT );
	joycur1 = 24;
	joycur2 = 6;
	joystp = FALSE;			// start button pause ... off.
	#ifdef STICK_3D_ON
	joystk = TRUE;			// joy stick key.
	#endif

	evs_prgstat = TSTAT_OPEN;
	evs_maincnt = 0;
	evs_grphcnt = 0;
	evs_fadecol = 0;
	auSeqPlayerStop(0);
	//	�O�̂���
	(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.
	graphic_no = GFX_50;

	// ���C�����[�v
	while( loop_flg ) {

		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);	// VI retrace wait.


		evs_maincnt ++;
		evs_grphcnt ++;
		sin_f += 6;
		if( sin_f >= 360 ){
			sin_f -= 360;
		}
		switch( evs_prgstat ) {
			case TSTAT_OPEN:
				if( evs_maincnt == 60 -50 ) {
					evs_prgstat = TSTAT_SET;
					evs_maincnt = 0;
				}
				break;
			case TSTAT_SET:
				auSeqPlayerPlay(0,SEQ_Clear);					//	�a�f�l�Đ��J�n
				evs_prgstat = TSTAT_LOOP;						//	�����𕶏͂�i�߂鏈���ɐ؂�ւ���
				break;
			case TSTAT_LOOP:
				if( ((st_flag & 0x4 ) == 4) || joyupd[main_joy[0]] & DM_KEY_START ){	//	�I���r�b�g�������Ă��邩�A�X�^�[�g�{�^�����������ꍇ
					evs_prgstat = TSTAT_CLOSE;					//	�����������t�F�[�h�A�E�g�����ɐݒ�
					evs_maincnt = 0;							//	���C���J�E���^�̃N���A
					auSeqPlayerStop(0);							//	���y��~
					auSndPlay( SE_gCombo1P );					//	SE�ݒ�
				}
#ifdef	DM_DEBUG_FLG
				if( joyupd[main_joy[0]] & DM_KEY_L ){
					if( !st_scr_stop ){
						calc_skip_message();
					}
				}
#endif
				make_pos();											//	��ʗh��̌v�Z���s���Ă���( �{�����X�e�[�W�ő�C���˂̂Ƃ��� )
				if( calc_message() > 0 ){							//	���͂�i�߂鏈��( calc_message() ) ���͂̓r���̏ꍇ �P �I���Ȃ� -1���Ԃ��Ă���
					pino_mouth_count ++;							//	�L�m�s�I�̃N�`�p�N�������邽�߂̏���
					if( pino_mouth_count >= 5 ){
						pino_mouth_count = 0;
						pino_mouth_flg ^= 1;
					}
				}else{												//	���͒�~(�L�[�҂���)�̂��߃N�`�p�N�����Ȃ�
					pino_mouth_flg = 0;
				}

				// �w�i�ʒu
				st_bg[pic_num].s.frameY = bg_y << 2;

				// �_���X�V
				i = ( cl_y+comple_y >= 240 )?239:cl_y+comple_y;		//	�_��\������x���W�̌v�Z
				h_size = (s16)( sinf( DEGREE( sin_f ) ) * 5 );		//	���������l�̌v�Z
				w_size = (s16)( sinf( DEGREE( sin_f ) ) * 15 );		//	�c�������l�̌v�Z
				st_cld.s.scaleW = (272 << 10) / (300 + w_size);		//	������ S2DEX �̕ϐ��ɐݒ�
				st_cld.s.scaleH = (64 << 10) / (70 + h_size);		//	�c���� S2DEX �̕ϐ��ɐݒ�
				correct_pos( &st_cld, cl_x, 272, 0 );				//	�w���W�̒���
				correct_pos( &st_cld, i, 64, 1 );					//	�x���W�̒���
				if( now_story == 4 ){								//	�T�ʂ̏ꍇ�A�v���y���w�C�z�[���㉺�����邽�߁A���\�v�Z���s���Ă���
					p1_y = (s16)(sinf( ((evs_maincnt*4) % 360) * 3.14159265 /180.0 ) * 10.0) + st_ch_data[10].pos_y;
				}

				if( st_flag & 0x8 ){								//	���̃r�b�g�������Ă�����
					cannon_on = 1;									//	��ʗh��t���O�𗧂Ă�( �{�����X�e�[�W�ő�C���˂̂Ƃ��� )
					st_flag &= 0xf7;								//	�r�b�g���N���A����
				}
				break;
			case TSTAT_CLOSE:	//	�t�F�[�h�A�E�g����
				if( evs_fadecol < 100 ) {							//	evs_fadecol �� 99�ȉ��̏ꍇ
					evs_fadecol+=10;								//	����������
					if( evs_fadecol > 100 ) evs_fadecol = 100;
				} else {
					evs_prgstat = TSTAT_LOOPEND;					//	�����I����
				}
				break;
			case TSTAT_LOOPEND:
				graphic_no = GFX_NULL;								// graphics finish.
				loop_flg = FALSE;									//	���[�v�E�o����
				break;
		}
		#ifndef THREAD_JOY
		joyProcCore();
		#endif
	}
	auSeqPlayerStop(0);													// stop the audio.
	graphic_no = GFX_NULL;												// graphics finish.
	while((auSeqPlayerState(0) != AL_STOPPED) || (pendingGFX != 0)) {	// task finish wait.
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	for(i = 0;i < 10;i++ ){
		(void) osRecvMesg(&msgQ, NULL, OS_MESG_BLOCK);
	}

	nnScRemoveClient(sched, &client);									// remove client to shceduler

	evs_story_no = (evs_story_no+1)%12;							//	�X�g�[���[�ԍ���i�߂鏈��

	evs_seqnumb = evs_story_no % 3;								//	�Q�[���̋Ȃ�ݒ肷��

	return;
}

// �O���t�B�b�N
void graphic50(void)
{
	NNScTask *t;
	int i;

	// graphic display list buffer. task buffer.
	gp = &gfx_glist[gfx_gtask_no][0];
	t  = &gfx_task[gfx_gtask_no];

	// RSP initialize.
	gSPSegment(gp++, 0, 0x0);
	gSPSegment(gp++, ST_SEGMENT,	 osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE]));
	gSPSegment(gp++, BG_SEGMENT,	 osVirtualToPhysical((void *)gfx_freebuf[GFX_SPRITE_PAGE + 2]));

	S2RDPinitRtn(TRUE);
	S2ClearCFBRtn(TRUE);
	gSPDisplayList(gp++, S2Spriteinit_dl);

	gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
	i = 255 * (100 - evs_fadecol) / 100;
	gDPSetPrimColor( gp++, 0,0, i,i,i,255 );

	// BG�̕`��
	gDPSetTextureLUT(gp++, G_TT_RGBA16);
	gDPSetCycleType(gp++, G_CYC_1CYCLE);
	gDPSetTextureFilter(gp++, G_TF_POINT );
	gDPLoadTLUT_pal256(gp++, st_bg_data[st_bg_all_num+pic_num].pal );
	gSPBgRect1Cyc(gp++, &st_bg[pic_num]);

	// �L�����\��
	if( (st_flag & 0x7) < 3 ){
		i = st_scene_data[st_bg_all_num+pic_num][2];
		if( i != 255 ){
			gDPLoadTLUT_pal256(gp++, st_ch_data[i].pal );
			gSPBgRect1Cyc(gp++, &st_ch[pic_num*2+1]);
		}
		i = st_scene_data[st_bg_all_num+pic_num][1];
		if( i != 255 ){
			gDPLoadTLUT_pal256(gp++, st_ch_data[i].pal );
			gSPBgRect1Cyc(gp++, &st_ch[pic_num*2]);
		}
	}

	// �v���y���{�����\��
	if( now_story == 4 && pic_num == 1 ){
		if( (st_flag & 0x7) < 3 ){
			gDPSetTextureFilter(gp++, G_TF_BILERP );
			disp_tex_8bt( st_ch_data[10].pat,st_ch_data[10].pal,st_ch_data[10].width,st_ch_data[10].height,st_ch_data[10].pos_x,p1_y,SET_PTD );
		}
	}

	// �^�C���\��
	disp_tile();

	// �_�\��
	gDPSetTextureLUT(gp++, G_TT_RGBA16);
	gDPSetRenderMode(gp++, G_RM_SPRITE, G_RM_SPRITE2);
	gDPSetCycleType(gp++, G_CYC_1CYCLE);
	gDPSetTextureFilter(gp++, G_TF_POINT );
	gDPLoadTLUT_pal256(gp++, st_mes_cloud_bm0_0tlut );	//	�p���b�g���[�h
	gSPBgRect1Cyc(gp++, &st_cld);						//	�_�`��


	// ��\��
	if( disp_face_flg ){
		gSPDisplayList( gp++, Normal_TNZ_Texture_dl);
		if( face_number != 0 ){	//	�L�m�s�I�ȊO�̏ꍇ
			disp_tex_4bt( faces[face_number].pat,faces[face_number].pal,48,48,32,176+comple_y,SET_PTD );
		}else{					//	�L�m�s�I�̏ꍇ
			disp_tex_4bt( faces[face_number].pat,faces[face_number].pal,48,48,32,176+comple_y,SET_PTD );
			//	�N�`�p�N
			if( pino_mouth_flg ){
				load_TexTile_4b( st_face_13b_bm0_0,8,5,0,0,7,4 );
				draw_Tex( 59,210 + comple_y,8,5,0,0 );
			}
		}
	}

	// ���b�Z�[�W�\��
	disp_message();

	// end of display list & go the graphic task.
	gDPFullSync(gp++);
	gSPEndDisplayList(gp++);
	osWritebackDCacheAll();
	gfxTaskStart(t, gfx_glist[gfx_gtask_no], (s32)(gp - gfx_glist[gfx_gtask_no]) * sizeof(Gfx), GFX_GSPCODE_S2DEX, NN_SC_SWAPBUFFER);
}

