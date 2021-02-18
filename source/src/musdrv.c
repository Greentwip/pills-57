
/*************************************************************

  test_music.c : Nintendo 64 Music Tools Library Sample
  (c) Copyright 1998, Software Creations (Holdings) Ltd.

  Version 3.11

  NNSCHED demo main source file. This demo illustrates how to
  use a scheduler other than the default OS scheduler.

**************************************************************/

/* include system header files */

#include <ultra64.h>
#include <libmus.h>
#include <libmus_data.h>

#include "nnsched.h"
#include "dmacopy.h"
#include "debug.h"

/* internal function prototypes */
static void DmaRomToRam(void *rom, void *ram, int len);

/* music library callback function prototypes these prototypes must match the
   typedefs of the callback functions specified in "libmus.h":

      typedef void    (*LIBMUScb_install)     (void);
      typedef void    (*LIBMUScb_waitframe)   (void);
      typedef void    (*LIBMUScb_dotask)      (musTask *);
*/
static void NnSchedInstall(void);
static void NnSchedWaitFrame(void);
static void NnSchedDoTask(musTask *task);

/* music library scheduler callback structure */
static musSched nn_mus_sched = {
   NnSchedInstall, NnSchedWaitFrame, NnSchedDoTask
};

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [GLOBAL FUNCTION]
  InitMusicDriver()

  [Explantion]
  Download ROM files and initialise the music player.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#define ALIGN_16(p) (((unsigned)(p) + 15) & ~15)

// �V�[�P���X�E�X�e�[�^�X�\����
typedef struct {
	u8       *seqBuf;
	int       seqBufSize;
	musHandle handle;
} MuSeqState;

typedef struct {
	musHandle handle;
} MuSndState;

// �T�E���h�E�h���C�o�[�\����
typedef struct {
	NNSched *nnSched;

	u8 *heap;
	int heapSize;

	u8 *pbkBuf;
	int pbkBufSize;

	MuSeqState *seqState;
	int         seqCount;

	u8 *fbkBuf;
	int fbkBufSize;

	MuSndState *sndState;
	int         sndCount;

	OSMesgQueue dmaMsgQ;
	OSMesg      dmaMsgBuf[8];

	NNScClient  nnClient;
	#define     QUEUE_SIZE 4

	OSMesgQueue nnFrameMsgQ;
	OSMesg      nnFrameMsgBuf[QUEUE_SIZE];

	OSMesgQueue nnTaskMsgQ;
	OSMesg      nnTaskMsgBuf[QUEUE_SIZE];
} MuDriver;

// �T�E���h�E�h���C�o�[�ł�
MuDriver *muDriver;

// �T�E���h�E�h���C�o�[������������
int muInitDriver(
	NNSched *sched,			// �m�m�X�P�W���[���̃A�h���X
	void    *heap,			// �I�[�f�B�I�q�[�v�̃A�h���X
	int      heapSize,		// �I�[�f�B�I�q�[�v�̃T�C�Y
	int      pbkBufSize,	// �T���v���|�C���^�o���N�̃T�C�Y
	int      seqBufSize,	// �V�[�P���X�o�b�t�@�̃T�C�Y
	int      seqBufCount,	// �V�[�P���X�o�b�t�@�̐�
	int      fbkBufSize,	// �T�E���h�G�t�F�N�g�o���N�̃T�C�Y
	int      sndBufCount,	// �T�E���h�o�b�t�@�̐�
	int      audioPri		// �I�[�f�B�I�X���b�h�̃v���C�I���e�B
	)
{
	MuDriver *st = muDriver = (MuDriver *)ALIGN_16(heap);
	musConfig init;
	int i;

	// �q�[�v�������[�A�h���X��ۑ�
	st->heap     = (u8 *)heap;
	st->heapSize = heapSize;
	heap = st + 1;

	// �X�P�W���[���A�h���X��ۑ�
	st->nnSched = sched;

	// �T���v���|�C���^�o���N�����蓖�Ă�
	st->pbkBuf     = (u8 *)ALIGN_16(heap);
	st->pbkBufSize = pbkBufSize;
	heap = st->pbkBuf + st->pbkBufSize;

	// �V�[�P���X�f�[�^�o�b�t�@�����蓖�Ă�
	st->seqState = (MuSeqState *)ALIGN_16(heap);
	st->seqCount = seqBufCount;
	heap = st->seqState + st->seqCount;

	for(i = 0; i < st->seqCount; i++) {
		st->seqState[i].seqBuf     = (u8 *)ALIGN_16(heap);
		st->seqState[i].seqBufSize = seqBufSize;
		st->seqState[i].handle     = 0;
		heap = st->seqState[i].seqBuf + seqBufSize;
	}

	// �T�E���h�G�t�F�N�g�o���N�����蓖�Ă�
	st->fbkBuf     = (u8 *)ALIGN_16(heap);
	st->fbkBufSize = pbkBufSize;
	heap = st->fbkBuf + st->fbkBufSize;

	// �T�E���h�o�b�t�@�����蓖�Ă�
	st->sndState = (MuSndState *)ALIGN_16(heap);
	st->sndCount = sndBufCount;
	heap = st->sndState + st->sndCount;

	for(i = 0; i < st->sndCount; i++) {
		st->sndState[i].handle = 0;
	}

	// DMA�f�[�^�]�����s���ׂ̃��b�Z�[�W�p��������
	osCreateMesgQueue(&st->dmaMsgQ, st->dmaMsgBuf,
		sizeof(st->dmaMsgBuf)/sizeof(st->dmaMsgBuf[0]));

	// Mus�Ɋ��蓖�Ă�q�[�v�̃A�h���X�ƃT�C�Y�����߂�
	heap      = (void *)ALIGN_16(heap);
	heapSize -= (u32)heap - (u32)st->heap;
	DBG2(heapSize <= 0, "�q�[�v������܂���(%d/%d)\n", st->heapSize - heapSize, st->heapSize);

	// setup configuration structure
	init.control_flag       = 0;
	init.channels           = 24;
	init.sched              = sched;
	init.thread_priority    = audioPri;
	init.heap               = (u8 *)heap;
	init.heap_length        = heapSize;
	init.fifo_length        = 64;
	init.ptr                = NULL;
	init.wbk                = NULL;
	init.default_fxbank     = NULL;
	init.syn_output_rate    = 32000;
	init.syn_updates        = 256;
	init.syn_rsp_cmds       = 4096;
	init.syn_retraceCount   = 1;
	init.syn_num_dma_bufs   = 36;
	init.syn_dma_buf_size   = 0x1000;

	init.syn_updates        = 128;
	init.syn_rsp_cmds       = 2048;
	init.syn_num_dma_bufs   = 32;
	init.syn_dma_buf_size   = 640;

	// set for NN scheduler this function must be called before 'musInitialize'
	// otherwise default scheduler will be initialised and used
	MusSetScheduler(&nn_mus_sched);

	return MusInitialize(&init) + ((u8 *)heap - (u8 *)st->heap);
}

// �T���v���|�C���^�o���N��ǂݍ���
int muSetPbkData(
	void *pbkRomAddr,	// �T���v���|�C���^�o���N��ROM�A�h���X
	int   pbkSize,		// �T���v���|�C���^�o���N�̃T�C�Y
	void *wbkRomAddr	// �T���v���E�F�[�u�o���N��ROM�A�h���X
	)
// �ߒl:
//   false: �V�[�P���X���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �T���v���|�C���^�o���N��ǂݍ��񂾁B
{
	MuDriver *st = muDriver;

	DBG2(pbkSize > st->pbkBufSize,
		"�T���v���|�C���^�o���N�o�b�t�@������܂���(%d/%d)\n",
		pbkSize, st->pbkBufSize);

	if(MusAsk(MUSFLAG_SONGS)) {
		return 0;
	}

	DmaRomToRam(pbkRomAddr, st->pbkBuf, pbkSize);
	MusPtrBankInitialize(st->pbkBuf, wbkRomAddr);

	return 1;
}

// �V�[�P���X�f�[�^��ǂݍ���
int muSeqSetData(
	int   seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	void *seqRomAddr,	// �V�[�P���X�f�[�^�̂q�n�l�A�h���X
	int   seqSize		// �V�[�P���X�f�[�^�̃T�C�Y
	)
// �ߒl:
//   false: �w�肵���V�[�P���X�o�b�t�@�́A�V�[�P���X���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �V�[�P���X�f�[�^��ǂݍ��񂾁B
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	DBG2(seqSize > st->seqState[seqBufNo].seqBufSize,
		"�V�[�P���X�o�b�t�@������܂���(%d/%d)\n",
		seqSize, st->seqState[seqBufNo].seqBufSize);

	if(muSeqAsk(seqBufNo)) {
		return 0;
	}

	DmaRomToRam(seqRomAddr, st->seqState[seqBufNo].seqBuf, seqSize);

	return 1;
}

// �V�[�P���X�f�[�^���Đ�����
void muSeqPlay(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	)
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	st->seqState[seqBufNo].handle = MusStartSong(st->seqState[seqBufNo].seqBuf);
}

// �V�[�P���X�n���h�����擾
musHandle muSeqGetHandle(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	)
// �ߒl: �V�[�P���X�n���h��
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return st->seqState[seqBufNo].handle;
}

// �����`�����l������������Ă��邩���ׂ�
int muSeqAsk(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	)
// �ߒl: ��������Ă���`�����l����
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleAsk(st->seqState[seqBufNo].handle);
}

// �V�[�P���X�̍Đ����~����
int muSeqStop(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int speed			// ��~����܂ł̃t���[����
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleStop(st->seqState[seqBufNo].handle, speed);
}

// �V�[�P���X�̃{�����[����ݒ肷��
int muSeqSetVol(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int volume			// �ݒ肷��{�����[���̒l(0~256, 128��100%�̃{�����[��)
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleSetVolume(st->seqState[seqBufNo].handle, volume);
}

// �V�[�P���X�̃p����ݒ肷��
int muSeqSetPan(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int pan				// �X�e���I�p���X�P�[���l(0~256, 128�Ńf�t�H���g�̒l)
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleSetPan(st->seqState[seqBufNo].handle, pan);
}

// �V�[�P���X�̃e���|��ύX����
int muSeqSetTempo(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int tempo			// �e���|(0~256, 128�Ńf�t�H���g�̒l)
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleSetTempo(st->seqState[seqBufNo].handle, tempo);
}

// �V�[�P���X�̈ꎞ��~���s�Ȃ�
int muSeqPause(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	)
// �ߒl: �����R�}���h������������0, �����łȂ����0�ȊO
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandlePause(st->seqState[seqBufNo].handle);
}

// �ꎞ��~�����V�[�P���X���ĊJ
int muSeqUnPause(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	)
// �ߒl: �����R�}���h������������0, �����łȂ����0�ȊO
{
	MuDriver *st = muDriver;

	DBG1(seqBufNo < 0 || seqBufNo >= st->seqCount,
		"�s���ȃV�[�P���X�o�b�t�@�ԍ�(%d)\n", seqBufNo);

	return MusHandleUnPause(st->seqState[seqBufNo].handle);
}

// �T�E���h�G�t�F�N�g�o���N��ǂݍ���
int muSetFbkData(
	void *fbkRomAddr,	// �T�E���h�G�t�F�N�g�o���N��ROM�A�h���X
	int   fbkSize		// �T�E���h�G�t�F�N�g�o���N�̃T�C�Y
	)
// �ߒl:
//   false: �T�E���h���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �T�E���h�G�t�F�N�g�o���N��ǂݍ��񂾁B
{
	MuDriver *st = muDriver;

	DBG2(fbkSize > st->fbkBufSize,
		"�T�E���h�G�t�F�N�g�o���N�o�b�t�@������܂���(%d/%d)\n",
		fbkSize, st->fbkBufSize);

	if(MusAsk(MUSFLAG_EFFECTS)) {
		return 0;
	}

	DmaRomToRam(fbkRomAddr, st->fbkBuf, fbkSize);
	MusFxBankInitialize(st->fbkBuf);

	return 1;
}

// �T�E���h�G�t�F�N�g�o���N�̃A�h���X��Ԃ�
void *muGetFbkAddr()
// �ߒl:
//   �T�E���h�G�t�F�N�g�o���N�̃A�h���X
{
	MuDriver *st = muDriver;

	return st->fbkBuf;
}

// �T�E���h�f�[�^���Đ�����
void muSndPlay(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int number			// �T�E���h�G�t�F�N�g�̔ԍ�
	)
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@(%d)\n", sndBufNo);

	st->sndState[sndBufNo].handle = MusStartEffect(number);
}

// �T�E���h�f�[�^���Đ�����2
void muSndPlay2(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int number,			// �T�E���h�G�t�F�N�g�̔ԍ�
	int volume,			// �{�����[���X�P�[��
	int pan,			// �p���X�P�[��
	int overwrite,		// �����ԍ��̃I�[�o�[���C�g�i�㏑���p�j�T�E���h�G�t�F�N�g
	int priority		// �v���C�I���e�B���x��
	)
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@(%d)\n", sndBufNo);

	st->sndState[sndBufNo].handle = MusStartEffect2(
		number, volume, pan, overwrite, priority);
}

// �T�E���h�n���h�����擾
musHandle muSndGetHandle(
	int sndBufNo		// �T�E���h�o�b�t�@�̔ԍ�
	)
// �ߒl: �T�E���h�n���h��
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@(%d)\n", sndBufNo);

	return st->sndState[sndBufNo].handle;
}

// �����`�����l������������Ă��邩���ׂ�
extern int muSndAsk(
	int sndBufNo		// �T�E���h�o�b�t�@�̔ԍ�
	)
// �ߒl: ��������Ă���`�����l����
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@(%d)\n", sndBufNo);

	return MusHandleAsk(st->sndState[sndBufNo].handle);
}

// �T�E���h�̍Đ����~����
int muSndStop(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int speed			// ��~����܂ł̃t���[����
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@(%d)\n", sndBufNo);

	return MusHandleStop(st->sndState[sndBufNo].handle, speed);
}

// �T�E���h�̃{�����[����ݒ肷��
int muSndSetVol(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int volume			// �ݒ肷��{�����[���̒l(0~256, 128��100%�̃{�����[��)
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@�ԍ�(%d)\n", sndBufNo);

	return MusHandleSetVolume(st->sndState[sndBufNo].handle, volume);
}

// �T�E���h�̃p����ݒ肷��
int muSndSetPan(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int pan				// �X�e���I�p���X�P�[���l(0~256, 128�Ńf�t�H���g�̒l)
	)
// �ߒl: �R�}���h�����s���ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@�ԍ�(%d)\n", sndBufNo);

	return MusHandleSetPan(st->sndState[sndBufNo].handle, pan);
}

// �T�E���h�̎��g����ݒ�
int muSndSetFreq(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	float offset		// ���g���̃I�t�Z�b�g
	)
// �ߒl: ���g���I�t�Z�b�g���K�����ꂽ�`�����l����
{
	MuDriver *st = muDriver;

	DBG1(sndBufNo < 0 || sndBufNo >= st->sndCount,
		"�s���ȃT�E���h�o�b�t�@�ԍ�(%d)\n", sndBufNo);

	return MusHandleSetFreqOffset(st->sndState[sndBufNo].handle, offset);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [INTERNAL FUNCTION]
  DmaRomToRam(rom, ram, len)

  [Parameters]
  rom          address of ROM source
  ram          address of RAM destination
  len          number of bytes to transfer

  [Explantion]
  Download an area of ROM to RAM. Note this function limits the size of any DMA
  generated to 16k so as to cause clashes with audio DMAs.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

/* avoid DMA clashes */
#define DMA_LEN 16384

static void DmaRomToRam(void *rom, void *ram, int len)
{
	MuDriver *st = muDriver;
	OSIoMesg io_mesg;
	OSMesg dummyMesg;
	int length;
	u32 src;
	unsigned char *dest;

	src = (u32)rom;
	dest = ram;

	while(len) {
		if(len > DMA_LEN)
			length = DMA_LEN;
		else
			length = len;

		osInvalDCache(dest, length);

		io_mesg.hdr.pri = OS_MESG_PRI_NORMAL;
		io_mesg.hdr.retQueue = &st->dmaMsgQ;
		io_mesg.dramAddr = dest;
		io_mesg.devAddr = src;
		io_mesg.size = length;

		osEPiStartDma(osCartRomHandle(), &io_mesg, OS_READ);

		src += length;
		dest += length;
		len -= length;

		osRecvMesg(&st->dmaMsgQ, &dummyMesg, OS_MESG_BLOCK);
	}
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [MUSIC LIBRARY CALLBACK FUNCTION]
  NnSchedInstall()

  [Explantion]
  Initialise the NN scheduler callback functions for the music library. This function
  is called once by the audio thread before entering an infinite loop.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

static void NnSchedInstall(void)
{
	MuDriver *st = muDriver;

	/* create message queues for WaitFrame and DoTask functions */
	osCreateMesgQueue(&st->nnFrameMsgQ, &st->nnFrameMsgBuf[0], QUEUE_SIZE);
	osCreateMesgQueue(&st->nnTaskMsgQ, &st->nnTaskMsgBuf[0], QUEUE_SIZE);

	/* add waitframe client */
	nnScAddClient(st->nnSched, &st->nnClient, &st->nnFrameMsgQ);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [MUSIC LIBRARY CALLBACK FUNCTION]
  NnSchedWaitFrame()

  [Explantion]
  Wait for a retrace message for the music library. This function is called by the
  audio threads inifinite loop to wait for a retrace message. The 'syn_retraceCount'
  parameter of the musConfig structure must contain the number of retraces per
  message received.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

static void NnSchedWaitFrame(void)
{
	MuDriver *st = muDriver;
	NNScMsg *message;

	do {
		osRecvMesg(&st->nnFrameMsgQ, (OSMesg *)&message, OS_MESG_BLOCK);
		osRecvMesg(&st->nnFrameMsgQ, NULL, OS_MESG_NOBLOCK);  /* bin any missed syncs! */
	} while (*message != NN_SC_RETRACE_MSG);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  [MUSIC LIBRARY CALLBACK FUNCTION]
  NnSchedDoTask()

  [Parameters]
  task      address of the music library task descriptor structure

  [Explantion]
  Process a audio task for the music library. This function is called by the audio
  threads inifinite loop to generate an audio task and wait for its completion.

  The structure passed is defined in "libmus.h" and is defined as:

         typedef struct
         {
            u64   *data;
            int   data_size;
            u64   *ucode;
            u64   *ucode_data;
         } musTask;

  The 'data' and 'data_size' components of the structure relate to the command list
  to be processed. 'data' is the address of the command list and 'data_size' is the
  number of commands it contains.

  The 'ucode' and 'ucode_data' components of the structure relate to the microcode
  to be used. 'ucode' is the address of the microcode Text section, 'ucode_data' is
  the address of the microcode Data section.

  [Return value]
  NONE
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

static void NnSchedDoTask(musTask *task)
{
	MuDriver *st = muDriver;
	NNScTask t;
	NNScMsg message;

	/* copy settings from musTask structure */
	t.list.t.data_ptr       = task->data;
	t.list.t.data_size      = task->data_size;
	t.list.t.ucode          = (u64 *) task->ucode;
	t.list.t.ucode_data     = (u64 *) task->ucode_data;

	/* fill in the rest of task data */
	t.next                      = 0;
	t.msgQ                      = &st->nnTaskMsgQ;
	t.msg                       = &message;
	t.list.t.type               = M_AUDTASK;
	t.list.t.ucode_boot         = (u64 *)rspbootTextStart;
	t.list.t.ucode_boot_size    = ((int) rspbootTextEnd - (int) rspbootTextStart);
	t.list.t.flags              = 0;
	t.list.t.ucode_size         = 4096;
	t.list.t.ucode_data_size    = SP_UCODE_DATA_SIZE;
	t.list.t.dram_stack         = (u64 *) NULL;
	t.list.t.dram_stack_size    = 0;
	t.list.t.output_buff        = (u64 *) NULL;
	t.list.t.output_buff_size   = 0;
	t.list.t.yield_data_ptr     = NULL;
	t.list.t.yield_data_size    = 0;

	/* start task */
	osSendMesg(nnScGetAudioMQ(st->nnSched), (OSMesg) &t, OS_MESG_BLOCK);

	/* wait for task to finish */
	osRecvMesg(&st->nnTaskMsgQ, NULL, OS_MESG_BLOCK);
}

/* end of file */
