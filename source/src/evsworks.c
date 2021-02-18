
#include <ultra64.h>
#include <PR/ramrom.h>
#include <PR/gs2dex.h>
#include "aiset.h"
#include "evsworks.h"
#include "segment.h"
#include "record.h"

//////////////////////////////////////////////////////////////////////////////
// EVSܰ� ( prototype in evsworks.h )

u8 evs_stereo = TRUE;         // true:��ڵ or false:����
u8 evs_seqence = TRUE;        // ���ݽ on/off
u8 evs_seqnumb = 0;           // ���ݽ�ԍ�
u8 evs_playmax;               // ��ڲ԰��max( ���۰א� )
u8 evs_playcnt = 1;           // ��ڲ԰
u8 evs_keyrept[2] = { 12,6 }; // ����߰�
u8 evs_gamespeed = 1;         // �Q�[���S�̂̑��x(�ʏ�͂P)
u8 evs_score_flag = 1;        // �X�R�A�\���t���O

GAME_SELECT evs_gamesel;      // ���قł̹ްѾڸ� 0-3 == 1P,2P,4P,vsCPU
GAME_MODE   evs_gamemode;     // �Q�[�����[�h

s8  evs_story_flg = 0;            // �X�g�[���[���[�h���ʃt���O
s8  evs_story_no = 0;             // �X�g�[���[���[�h�X�e�[�W�ԍ�
s8  evs_story_level = 1;          // �X�g�[���[���[�h�̓�Փx
u8  evs_secret_flg[2] = { 0, 0 }; // �B��L�����g�p�t���O [0]:vwario, [1]:mmario
u8  evs_one_game_flg = 0;         // �X�g�[���[�X�e�[�W�Z���N�g����ON
u8  evs_level_21 = 0;             // "�G���h���X�Q�P���x�����N���A����" �t���O
s8  evs_manual_no = 0;            // ��������ԍ�
u8  evs_select_name_no[2];        // �I�����ꂽ���O�̔z��ԍ�
u32 evs_high_score = 0;           // �ō����_
int evs_vs_count = 3;             // �ΐ��
u32 evs_game_time;                // �X�g�[���[�p����
int evs_default_name[MEM_NAME_SIZE];// ���O�iNEW�j�̃f�[�^

//////////////////////////////////////////////////////////////////////////////
// �e���{�����ް�

// ��߾ٓ���( default ={ 39,18,19 } )
u8 FlyingCnt[] = { 40, 20, 20 };

// ���ٶ�߾� y=1~3��, �����]�ͳ���
u8 BonusWait[][3] = {
	// { L M H },
	{  2,  2,  3, },
	// { L M H },
	{  1,  1,  1, },
	// { L M H }
	{  1,  0,  1, },
};

// ��߾ى�����ۯ����L�鎞�̒ǉ�����( != 0 )	 1P 2P 3P 4P
u8 TouchDownWait[] = { 1, 2, 2, 2 };

// ��߰�ޏ������޸� by FallSpeed[]		Low Mid Hi Max
u8 GameSpeed[] = { 0, 10, 15, 55, };

// ������߰��
u8 FallSpeed[] = {
	39,37,35,33,31,
	29,27,25,23,21,
	19,18,17,16,15,
	14,13,12,11,10,
	 9, 9, 8, 8, 7,
	 7, 6, 6, 5, 5,
	 5, 5, 5, 5, 5,
	 5, 5, 5, 5, 5,
	 4, 4, 4, 4, 4,
	 3, 3, 3, 3, 3,
	 2, 2, 2, 2, 2,
	 1,
};

// ����v�Z
u8 Score1p[][SCORE_MAX] = {
	{  1, 2, 4, 8,16,32, },
	{  2, 4, 8,16,32,64, },
	{  3, 6,12,24,48,96, },
};

// ��߾�϶޼��
u8 CapsMagazine[MAGA_MAX];

//////////////////////////////////////////////////////////////////////////////
