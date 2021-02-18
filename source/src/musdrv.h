
#if !defined(_MUSDRV_H_)
#define _MUSDRV_H_

#include <libmus.h>
#include <libmus_data.h>

// �T�E���h�h���C�o�[������������
extern int muInitDriver(
	NNSched *sched,			// �m�m�X�P�W���[���̃A�h���X
	void    *heap,			// �I�[�f�B�I�q�[�v�̃A�h���X
	int      heapSize,		// �I�[�f�B�I�q�[�v�̃T�C�Y
	int      pbkBufSize,	// �T���v���|�C���^�o���N�̃T�C�Y
	int      seqBufSize,	// �V�[�P���X�o�b�t�@�̃T�C�Y
	int      seqBufCount,	// �V�[�P���X�o�b�t�@�̐�
	int      fbkBufSize,	// �T�E���h�G�t�F�N�g�o���N�̃T�C�Y
	int      sndBufCount,	// �T�E���h�o�b�t�@�̐�
	int      audioPri		// �I�[�f�B�I�X���b�h�̃v���C�I���e�B
	);
// �ߒl:
//   �~���[�W�b�N�v���C���[�ƃI�[�f�B�I�}�l�[�W���ɂ���Ďg�p�����A
//   �q�[�v����̃������̗ʁB



// �T���v���|�C���^�o���N��ǂݍ���
extern int muSetPbkData(
	void *pbkRomAddr,	// �T���v���|�C���^�o���N��ROM�A�h���X
	int   pbkSize,		// �T���v���|�C���^�o���N�̃T�C�Y
	void *wbkRomAddr	// �T���v���E�F�[�u�o���N��ROM�A�h���X
	);
// �ߒl:
//   false: �V�[�P���X���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �T���v���|�C���^�o���N��ǂݍ��񂾁B


// �V�[�P���X�f�[�^��ǂݍ���
extern int muSeqSetData(
	int   seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	void *seqRomAddr,	// �V�[�P���X�f�[�^�̂q�n�l�A�h���X
	int   seqSize		// �V�[�P���X�f�[�^�̃T�C�Y
	);
// �ߒl:
//   false: �w�肵���V�[�P���X�o�b�t�@�́A�V�[�P���X���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �V�[�P���X�f�[�^��ǂݍ��񂾁B


// �V�[�P���X�f�[�^���Đ�����
extern void muSeqPlay(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	);


// �V�[�P���X�n���h�����擾
extern musHandle muSeqGetHandle(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	);
// �ߒl: �V�[�P���X�n���h��


// �����`�����l������������Ă��邩���ׂ�
extern int muSeqAsk(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	);
// �ߒl: ��������Ă���`�����l����


// �V�[�P���X�̍Đ����~����
extern int muSeqStop(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int speed			// ��~����܂ł̃t���[����
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �V�[�P���X�̃{�����[����ݒ肷��
extern int muSeqSetVol(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int volume			// �ݒ肷��{�����[���̒l(0~256, 128��100%�̃{�����[��)
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �V�[�P���X�̃p����ݒ肷��
extern int muSeqSetPan(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int pan				// �X�e���I�p���X�P�[���l(0~256, 128�Ńf�t�H���g�̒l)
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �V�[�P���X�̃e���|��ύX����
extern int muSeqSetTempo(
	int seqBufNo,		// �V�[�P���X�o�b�t�@�̔ԍ�
	int tempo			// �e���|(0~256, 128�Ńf�t�H���g�̒l)
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �V�[�P���X�̈ꎞ��~���s�Ȃ�
extern int muSeqPause(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	);
// �ߒl: �����R�}���h������������0, �����łȂ����0�ȊO


// �ꎞ��~�����V�[�P���X���ĊJ
extern int muSeqUnPause(
	int seqBufNo		// �V�[�P���X�o�b�t�@�̔ԍ�
	);
// �ߒl: �����R�}���h������������0, �����łȂ����0�ȊO


// �T�E���h�G�t�F�N�g�o���N��ǂݍ���
extern int muSetFbkData(
	void *fbkRomAddr,	// �T�E���h�G�t�F�N�g�o���N��ROM�A�h���X
	int   fbkSize		// �T�E���h�G�t�F�N�g�o���N�̃T�C�Y
	);
// �ߒl:
//   false: �T�E���h���Đ����ł��������߁A�f�[�^��ǂݍ��܂Ȃ������B
//   true : �T�E���h�G�t�F�N�g�o���N��ǂݍ��񂾁B


// �T�E���h�G�t�F�N�g�o���N�̃A�h���X��Ԃ�
extern void *muGetFbkAddr();
// �ߒl:
//   �T�E���h�G�t�F�N�g�o���N�̃A�h���X


// �T�E���h�f�[�^���Đ�����
extern void muSndPlay(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int number			// �T�E���h�G�t�F�N�g�̔ԍ�
	);


// �T�E���h�f�[�^���Đ�����2
extern void muSndPlay2(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int number,			// �T�E���h�G�t�F�N�g�̔ԍ�
	int volume,			// �{�����[���X�P�[��
	int pan,			// �p���X�P�[��
	int overwrite,		// �����ԍ��̃I�[�o�[���C�g�i�㏑���p�j�T�E���h�G�t�F�N�g
	int priority		// �v���C�I���e�B���x��
	);


// �T�E���h�n���h�����擾
extern musHandle muSndGetHandle(
	int sndBufNo		// �T�E���h�o�b�t�@�̔ԍ�
	);
// �ߒl: �T�E���h�n���h��


// �����`�����l������������Ă��邩���ׂ�
extern int muSndAsk(
	int sndBufNo		// �T�E���h�o�b�t�@�̔ԍ�
	);
// �ߒl: ��������Ă���`�����l����


// �T�E���h�̍Đ����~����
extern int muSndStop(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int speed			// ��~����܂ł̃t���[����
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �T�E���h�̃{�����[����ݒ肷��
extern int muSndSetVol(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int volume			// �ݒ肷��{�����[���̒l(0~256, 128��100%�̃{�����[��)
	);
// �ߒl: �R�}���h�����s���ꂽ�`�����l����


// �T�E���h�̃p����ݒ肷��
extern int muSndSetPan(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	int pan				// �X�e���I�p���X�P�[���l(0~256, 128�Ńf�t�H���g�̒l)
	);


// �T�E���h�̎��g����ݒ�
extern int muSndSetFreq(
	int sndBufNo,		// �T�E���h�o�b�t�@�̔ԍ�
	float offset		// ���g���̃I�t�Z�b�g
	);
// �ߒl: ���g���I�t�Z�b�g���K�����ꂽ�`�����l����


#endif // _MUSDRV_H_

