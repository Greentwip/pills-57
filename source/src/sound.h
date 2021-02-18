#ifndef _SOUND_H_
#define _SOUND_H_

#include "sound/se.h"

//////////////////////////////////////////////////////////////////////////////
// �T�E���h�̃��x��

enum {
	SEQ_Fever,		// fever
	SEQ_FeverF,		// fever
	SEQ_Chill,		// chill
	SEQ_ChillF,		// chill
	SEQ_Game_C,		// �Q�[����ʁA��X�e�[�W
	SEQ_Game_CF,	// �Q�[����ʁA��X�e�[�W
	SEQ_Game_D,		// �Q�[����ʁA�ŏI�X�e�[�W�����������X�e�[�W
	SEQ_Game_DF,	// �Q�[����ʁA�ŏI�X�e�[�W�����������X�e�[�W
	SEQ_Game_E,		// �Q�[����ʁA�ŏI�X�e�[�W
	SEQ_Game_EF,	// �Q�[����ʁA�ŏI�X�e�[�W
	SEQ_Opening,	// �I�[�v�j���O
	SEQ_Title,		// �^�C�g��
	SEQ_Menu,		// ���j���[
	SEQ_Staff,		// �X�^�b�t���[��
	SEQ_End_A,		// �N���A�^�E�B���^�h���[�^���[�Y�i�u�r�@�b�n�l�E�Q�l�ŗV�ԁE�݂�ȂŗV�ԃ��[�h�j
	SEQ_End_B,		// �R�{��悵����
	SEQ_End_BS,		// �R�{��悵����(short ver)
	SEQ_End_C,		// �Q�[���I�[�o�[�^���[�Y�i�X�g�[���[���[�h�j
	SEQ_End_CS,		// �Q�[���I�[�o�[�^���[�Y�i�X�g�[���[���[�h�j(short ver)
	SEQ_Story_A,	// �X�g�[���[��ʁA��X�e�[�W
	SEQ_Story_B,	// �X�g�[���[��ʁA�u�Ȃ��̂����̂���ł�v�X�e�[�W���������X�e�[�W
	SEQ_Story_C,	// �X�g�[���[��ʁA�u�Ȃ��̂����̂���ł�v
	SEQ_Ending,		// �X�g�[���[��ʁA�G���f�B���O
	SEQ_Coffee,		// �R�[�q�[�u���[�N
	_SEQ_SUM,
};

extern const unsigned char _charSE_tbl[];

// �U����
#define SE_getAttack(charNo, attackLv) (_charSE_tbl[(charNo)] + (attackLv))

// �_���[�W��
#define SE_getDamage(charNo)           (_charSE_tbl[(charNo)] + 3)

// ����
#define SE_getWin(charNo)              (_charSE_tbl[(charNo)] + 4)

enum {
	SE_gFreeFall      = FX_SE01, // �J�v�Z���������̔ԍ�
	SE_gVrsErase      = FX_SE02, // �E�C���X�������̔ԍ�
	SE_gVrsErsAl      = FX_SE03, // ����E�C���X���Ō�
	SE_gSpeedUp       = FX_SE04, // �������x�A�b�v���̔ԍ�
	SE_gCombo1P       = FX_SE05, // �P�o�A�����̉�
	SE_gPause         = FX_SE06, // �|�[�Y���ʉ�
	SE_gCapErase      = FX_SE07, // �J�v�Z���������̔ԍ�
	SE_mDecide        = FX_SE08, // ����
	SE_mLeftRight     = FX_SE09, // ���j���[���̃J�[�\���ړ�
	SE_mUpDown        = FX_SE10, // ���j���[�؂�ւ�

	SE_gLeftRight     = FX_SE11, // ���E�ړ����̔ԍ�
	SE_gTouchDown     = FX_SE12, // ���n���̔ԍ�
	SE_gCapRoll       = FX_SE13, // ��]���̔ԍ�
	SE_mCANCEL        = FX_SE14, // �L�����Z��
	SE_mDispChg       = FX_SE15, // ��ʂ���ς�
	SE_mDataErase     = FX_SE16, // �o�b�N�A�b�v�f�[�^�폜
	SE_mError         = FX_SE17, // �G���[
	SE_gCount1        = FX_SE18, // �J�E���g�_�E��,
	SE_gCountEnd      = FX_SE19, // �J�E���g�_�E���I��
	SE_gVirusStruggle = FX_SE20, // ����E�C���X�_���[�W

	SE_gCombo2P       = FX_SE21, // �Q�o�A�����̉�
	SE_gCombo3P       = FX_SE22, // �R�o�A�����̉�
	SE_gCombo4P       = FX_SE23, // �S�o�A�����̉�
	SE_gCoin          = FX_SE24, // �R�C���l��
	SE_gWarning       = FX_SE25, // �J�v�Z���ςݏグ�x����
	SE_gReach         = FX_SE26, // �E�C���X���c��R�̂Ƃ��̉�

	SE_gBottomUp      = FX_SE27, // 
	SE_gEraseFlash    = FX_SE28, // 
	SE_mGameStart     = FX_SE29, // 

//	SE_gREraseP11,      // 1P�A�����N�_
//	SE_gREraseP12,      // 1P�Q�A����
//	SE_gREraseP13,      // 1P�R�A���ȏ㉹
//	SE_gDamageP1,       // 1P���炢��

//	SE_gREraseP21,      // 2P�A�����N�_
//	SE_gREraseP22,      // 2P�Q�A����
//	SE_gREraseP23,      // 2P�R�A���ȏ㉹
//	SE_gDamageP2,       // 2P���炢��
//	SE_gDamageP22,      // 2P���炢��

//	SE_xNoRequest,
};

//////////////////////////////////////////////////////////////////////////////

extern void dm_audio_set_stereo(int stereo);

extern void dm_audio_init_driver(NNSched *sched);
extern void dm_audio_update();

extern void dm_audio_stop();
extern int  dm_audio_is_stopped();

extern void  dm_seq_play(int seqNo);
extern void _dm_seq_play(int bufNo, int seqNo);

extern void  dm_seq_play_fade(int seqNo, int fade);
extern void _dm_seq_play_fade(int bufNo, int seqNo, int fade);

extern void  dm_seq_play_in_game(int seqNo);
extern void _dm_seq_play_in_game(int bufNo, int seqNo);

extern void  dm_seq_stop();
extern void _dm_seq_stop();

extern void  dm_seq_set_volume(int vol);
extern void _dm_seq_set_volume(int bufNo, int vol);

extern int  dm_seq_is_stopped();
extern int _dm_seq_is_stopped(int bufNo);

extern void dm_snd_play(int sndNo);
extern void dm_snd_play_in_game(int sndNo);

extern void dm_snd_play_strange_sound();

//////////////////////////////////////////////////////////////////////////////

#endif
