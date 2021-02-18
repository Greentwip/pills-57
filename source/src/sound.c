#include <ultra64.h>
#include <math.h>

#include "def.h"
#include "nnsched.h"
#include "sound.h"
#include "musdrv.h"
#include "util.h"
#include "evsworks.h"
#include "boot_data.h"
#include "debug.h"

//////////////////////////////////////////////////////////////////////////////
// �}�N��

#define AUDIO_HEAP_SIZE 0x00031000
#define AUDIO_THREAD_PRI 50
//#define AUDIO_THREAD_PRI (MAIN_THREAD_PRI - 1)

#define AUDIO_SEQ_PBK_START (_romDataTbl[_pbk_drmario][0])
#define AUDIO_SEQ_PBK_END   (_romDataTbl[_pbk_drmario][1])
#define AUDIO_SEQ_PBK_SIZE ((u32)AUDIO_SEQ_PBK_END - (u32)AUDIO_SEQ_PBK_START)
#define AUDIO_SEQ_BUF_SIZE _getMaxSeqSize()
#define AUDIO_SEQ_BUF_COUNT 2

#define AUDIO_SND_FBK_START (_romDataTbl[_fbk_drmario][0])
#define AUDIO_SND_FBK_END   (_romDataTbl[_fbk_drmario][1])
#define AUDIO_SND_FBK_SIZE ((u32)AUDIO_SND_FBK_END - (u32)AUDIO_SND_FBK_START)
#define AUDIO_SND_BUF_COUNT 4

#define AUDIO_WBK_START (_romDataTbl[_wbk_drmario][0])

//////////////////////////////////////////////////////////////////////////////
// �T�E���h���x���̃e�[�u��

const unsigned char _charSE_tbl[] = {
	FX_14_AT1, // �}���I
	FX_12_AT1, // �����I
	FX_08_AT1, // �}���s�[
	FX_09_AT1, // �����}��
	FX_10_AT1, // ��񂲂낤
	FX_04_AT1, // �t�E�Z���܂���
	FX_06_AT1, // �܂�J�G��
	FX_05_AT1, // �ӂ����炰
	FX_01_AT1, // �C�J�^�R�e���O
	FX_02_AT1, // �L�O����
	FX_03_AT1, // �n���}�[���{
	FX_07_AT1, // �}�b�h���V�^�C��
	FX_11_AT1, // �Ȃ��̂���
	FX_13_AT1, // �o���p�C�A�����I
	FX_15_AT1, // ���^���}���I
};

//////////////////////////////////////////////////////////////////////////////
// �T�E���h�f�[�^

// �V�[�P���X�f�[�^�e�[�u��
static const int _seqDataTbl[] = {
	_seq_fever,   // fever
	_seq_feverF,  // fever
	_seq_chill,   // chill
	_seq_chillF,  // chill
	_seq_game_c,  // �Q�[����ʁA��X�e�[�W
	_seq_game_cF, // �Q�[����ʁA��X�e�[�W
	_seq_game_d,  // �Q�[����ʁA�ŏI�X�e�[�W�����������X�e�[�W
	_seq_game_dF, // �Q�[����ʁA�ŏI�X�e�[�W�����������X�e�[�W
	_seq_game_e,  // �Q�[����ʁA�ŏI�X�e�[�W
	_seq_game_eF, // �Q�[����ʁA�ŏI�X�e�[�W
	_seq_opening, // �I�[�v�j���O
	_seq_title,   // �^�C�g��
	_seq_menu,    // ���j���[
	_seq_staff,   // �X�^�b�t���[��
	_seq_end_a,   // �N���A�^�E�B���^�h���[�^���[�Y�i�u�r�@�b�n�l�E�Q�l�ŗV�ԁE�݂�ȂŗV�ԃ��[�h�j
	_seq_end_b,   // �R�{��悵����
	_seq_end_b_s, // �R�{��悵����(short ver)
	_seq_end_c,   // �Q�[���I�[�o�[�^���[�Y�i�X�g�[���[���[�h�j
	_seq_end_c_s, // �Q�[���I�[�o�[�^���[�Y�i�X�g�[���[���[�h�j(short ver)
	_seq_story_a, // �X�g�[���[��ʁA��X�e�[�W
	_seq_story_b, // �X�g�[���[��ʁA�u�Ȃ��̂����̂���ł�v�X�e�[�W���������X�e�[�W
	_seq_story_c, // �X�g�[���[��ʁA�u�Ȃ��̂����̂���ł�v
	_seq_ending,  // �X�g�[���[��ʁA�G���f�B���O
	_seq_coffee,  // �R�[�q�[�u���[�N
};

// �V�[�P���X�f�[�^�̍ő�l���擾
static int _getMaxSeqSize()
{
	void **seq;
	int i, size, maxSize = 0;

	for(i = 0; i < ARRAY_SIZE(_seqDataTbl); i++) {
		seq = _romDataTbl[ _seqDataTbl[i] ];
		size = (u32)seq[1] - (u32)seq[0];
		maxSize = MAX(size, maxSize);
	}

	return maxSize;
}

// SE �̃p�����[�^
#include "sound/tune.h"

//////////////////////////////////////////////////////////////////////////////

// �I�[�f�B�I�q�[�v
static u8 heaparea[AUDIO_HEAP_SIZE];

//////////////////////////////////////////////////////////////////////////////
// �T�E���h�Ǘ�

// �T�E���h�X�e�[�^�X�\����
typedef struct {
	const FxTune *tune;
	int sndBuf;
	int frmCnt;
} SndState;

// �T�E���h�X�e�[�^�X��������
static void sndState_init(SndState *st, int seqBuf)
{
	st->tune = 0;
	st->sndBuf = seqBuf;
	st->frmCnt = 0;
}

// FxTune ��ݒ�
static void sndState_set(SndState *st, const FxTune *tune)
{
	muSndStop(st->sndBuf, 0);
	st->tune = tune;
	st->frmCnt = 0;
}

// �T�E���h���Đ�����
static bool sndState_play(SndState *st)
{
	if(!st->tune || muSndAsk(st->sndBuf) != 0) {
		return false;
	}

	muSndPlay(st->sndBuf, st->tune->no);
	muSndSetVol(st->sndBuf, st->tune->vol);
	muSndSetFreq(st->sndBuf, st->tune->coarse * (1.0 / 8.0));

	return true;
}

// �T�E���h���Đ�����2
static bool sndState_play2(SndState *st)
{
	if(!st->tune || muSndAsk(st->sndBuf) != 0) {
		return false;
	}

	muSndPlay2(st->sndBuf, st->tune->no, st->tune->vol, 0x80, TRUE, st->tune->pri);
	muSndSetFreq(st->sndBuf, st->tune->coarse * (1.0 / 8.0));

	return true;
}

// �t���[�����̍X�V����
static void sndState_update(SndState *st)
{
	if(!st->tune) {
		return;
	}

	// �Đ����I�����Ă��邩�`�F�b�N
	if(muSndAsk(st->sndBuf) == 0) {
		st->tune = 0;
	}
	// �Đ����Ԃ��o�߂��Ă��邩�`�F�b�N
	else if(st->tune->time) {
		if(st->frmCnt < st->tune->time * FRAME_PAR_SEC / 10) {
			st->frmCnt++;
		}
		else {
			muSndStop(st->sndBuf, 0);
			st->tune = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void dm_audio_set_stereo(int stereo)
{
	extern Acmd *alEnvmixerPull(void *filter, s16 *outp, s32 outCount, s32 sampleOffset, Acmd *p);
	u32 *addr[2] = {
		(u32 *)((u8 *)alEnvmixerPull + 0x0000010c),
		(u32 *)((u8 *)alEnvmixerPull + 0x000002ec),
	};

/*XXXblythe - this seems like a bad idea */
#if defined(NDEBUG) && 0
	if(stereo) {
		*addr[0] = 0x92220012;
		*addr[1] = 0x8C82000C;
	}
	else {
		*addr[0] = 0x24020040;
		*addr[1] = 0x24020040;
	}
#endif

	// �S�Ẵf�[�^�L���b�V�����C�������C�g�o�b�N�����������܂�
	osWritebackDCacheAll();
}

//////////////////////////////////////////////////////////////////////////////
// �h�N�}���p�I�[�f�B�I�h���C�o�[

// �I�[�f�B�I�h���C�o�[�\����
typedef struct {
	int seqNo[AUDIO_SEQ_BUF_COUNT];
	SndState sndState[AUDIO_SND_BUF_COUNT];
	SndState *sndQueue[AUDIO_SND_BUF_COUNT];
} AudioDriver;

AudioDriver audDrv;

// �T�E���h�h���C�o�[��������
void dm_audio_init_driver(NNSched *sched)
{
	AudioDriver *st = &audDrv;
	int i;

	// �Đ����̃V�[�P���X�ԍ�
	for(i = 0; i < AUDIO_SEQ_BUF_COUNT; i++) {
		st->seqNo[i] = -1;
	}

	// �T�E���h�X�e�[�^�X��������
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		sndState_init(&st->sndState[i], i);
	}

	// MUS�h���C�o�[��������
	i = muInitDriver(
		sched,
		heaparea,
		AUDIO_HEAP_SIZE,
		AUDIO_SEQ_PBK_SIZE,
		AUDIO_SEQ_BUF_SIZE,
		AUDIO_SEQ_BUF_COUNT,
		AUDIO_SND_FBK_SIZE,
		AUDIO_SND_BUF_COUNT,
		AUDIO_THREAD_PRI);
	PRTFL();
	PRT1("use audio heap %08x + ��\n", i);

	// �T���v���|�C���^�o���N��ǂݍ���
	muSetPbkData(AUDIO_SEQ_PBK_START, AUDIO_SEQ_PBK_SIZE, AUDIO_WBK_START);

	// �T�E���h�G�t�F�N�g�o���N��ǂݍ���
	muSetFbkData(AUDIO_SND_FBK_START, AUDIO_SND_FBK_SIZE);
}

// �T�E���h�h���C�o�[�̏������X�V
void dm_audio_update()
{
	AudioDriver *st = &audDrv;
	int i;

	// �V�[�P���X��؂�ւ���
	for(i = 0; i < AUDIO_SEQ_BUF_COUNT; i++) {
		if(st->seqNo[i] >= 0 && _dm_seq_is_stopped(i)) {
			void **seq = _romDataTbl[ _seqDataTbl[st->seqNo[i]] ];
			muSeqSetData(i, seq[0], (u32)seq[1] - (u32)seq[0]);
			muSeqPlay(i);
			muSeqSetVol(i, 0x80);
			st->seqNo[i] = -1;
		}
	}

	// �T�E���h�Đ�����
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {

		// �T�E���h�L���[���`�F�b�N
		if(st->sndQueue[i]) {
			// �T�E���h���Đ�
			if(sndState_play(st->sndQueue[i])) {
				st->sndQueue[i] = 0;
			}
		}
		// �A�b�v�f�[�g
		else {
			sndState_update(&st->sndState[i]);
		}
	}
}

// �S�T�E���h���~
void dm_audio_stop()
{
	dm_seq_stop();
	MusStop(MUSFLAG_SONGS | MUSFLAG_EFFECTS, 0);
}

// �S�T�E���h����~���Ă��邩�m�F
int dm_audio_is_stopped()
{
	return MusAsk(MUSFLAG_SONGS | MUSFLAG_EFFECTS) == 0;
}

//////////////////////////////////////////////////////////////////////////////

// �V�[�P���X���Đ�����
void  dm_seq_play(int seqNo)            { _dm_seq_play(0, seqNo); }
void _dm_seq_play(int bufNo, int seqNo) { _dm_seq_play_fade(bufNo, seqNo, 0); }

// �V�[�P���X���Đ�����2
void  dm_seq_play_fade(int seqNo, int fade) { _dm_seq_play_fade(0, seqNo, fade); }
void _dm_seq_play_fade(int bufNo, int seqNo, int fade)
{
	AudioDriver *st = &audDrv;

	if(seqNo != st->seqNo[bufNo]) {
		muSeqStop(bufNo, fade);
		st->seqNo[bufNo] = seqNo;
	}
}

// �V�[�P���X���Đ�����(�Q�[����ʗp)
void  dm_seq_play_in_game(int seqNo) { _dm_seq_play_in_game(0, seqNo); }
void _dm_seq_play_in_game(int bufNo, int seqNo)
{
	if(evs_seqence) {
		_dm_seq_play(bufNo, seqNo);
	}
	else {
		switch(seqNo) {
		case SEQ_Fever:  case SEQ_FeverF:
		case SEQ_Chill:  case SEQ_ChillF:
		case SEQ_Game_C: case SEQ_Game_CF:
		case SEQ_Game_D: case SEQ_Game_DF:
		case SEQ_Game_E: case SEQ_Game_EF:
			_dm_seq_stop(bufNo);
			break;
		default:
			_dm_seq_play(bufNo, seqNo);
			break;
		}
	}
}

// �V�[�P���X���~����
void  dm_seq_stop() { _dm_seq_stop(0); }
void _dm_seq_stop(int bufNo)
{
	AudioDriver *st = &audDrv;

	muSeqStop(bufNo, 0);
	st->seqNo[bufNo] = -1;
}

// �V�[�P���X�̃{�����[����ݒ�
void  dm_seq_set_volume(int vol) { _dm_seq_set_volume(0, vol); }
void _dm_seq_set_volume(int bufNo, int vol)
{
	AudioDriver *st = &audDrv;
	muSeqSetVol(bufNo, vol);
}

// �V�[�P���X����~���Ă��邩���ׂ�
int  dm_seq_is_stopped() { _dm_seq_is_stopped(0); }
int _dm_seq_is_stopped(int bufNo)
{
	return muSeqAsk(bufNo) == 0;
}

//////////////////////////////////////////////////////////////////////////////

// �T�E���h���Đ�����
void dm_snd_play(int sndNo)
{
	AudioDriver *st = &audDrv;
	const FxTune *tune, *newTune = &_fxTuneTbl[sndNo];
	int i;

	// �L���[�ɓ�����������ꍇ�͒��f
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(!st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// ���ꉹ��?
		if(tune && tune->no == newTune->no) {
			return;
		}
	}

	// ��̃o�b�t�@���`�F�b�N
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// NULL?
		if(!tune) {
			sndState_set(&st->sndState[i], newTune);
			st->sndQueue[i] = &st->sndState[i];
			return;
		}
	}

	// �Đ����̃o�b�t�@���`�F�b�N
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// �v���C�I���e�B <= ?
		if(tune && tune->pri <= newTune->pri) {
			sndState_set(&st->sndState[i], newTune);
			st->sndQueue[i] = &st->sndState[i];
			return;
		}
	}

	// �L���[���`�F�b�N
	for(i = 0; i < AUDIO_SND_BUF_COUNT; i++) {
		if(!st->sndQueue[i]) {
			continue;
		}

		tune = st->sndState[i].tune;

		// �v���C�I���e�B < ?
		if(tune && tune->pri < newTune->pri) {
			sndState_set(&st->sndState[i], newTune);
			return;
		}
	}

	// �Đ���f�O
}

// �T�E���h���Đ�����(�Q�[����ʗp)
void dm_snd_play_in_game(int sndNo)
{
	if(evs_gamespeed < 6) {
		dm_snd_play(sndNo);
	}
}

// �T�E���h�G�t�F�N�g�̐���Ԃ��܂�
int dm_snd_get_effects()
{
	return MusFxBankNumberOfEffects(muGetFbkAddr());
}

// �ςȉ����Đ�
void dm_snd_play_strange_sound()
{
	int snd[4]; // <- �ςȉ��o�b�t�@�[
	int i, j;

	// �ςȉ����쐬
	for(i = 0; i < ARRAY_SIZE(snd); i++) {
		snd[i] = rand() % 15;
		for(j = 0; j < i; j++) {
			if(snd[i] == snd[j]) {
				i--;
				break;
			}
		}
	}

	// �ςȉ����Đ�
	for(i = 0; i < ARRAY_SIZE(snd); i++) {
		dm_snd_play(SE_getDamage(snd[i]));
	}
}

//////////////////////////////////////////////////////////////////////////////

