
#if !defined(_HARDCOPY_H_)
#define _HARDCOPY_H_

typedef enum {
	HC_NULL,
	HC_CURTAIN, // �J�[�e��
	HC_SPHERE,  // ��
//	HC_OVI,
} HC_EFFECT;

// ��ʐؑփG�t�F�N�g��������
// �ߒl
//     �m�ۂ������[�N�������[�̍Ō��
extern void *HCEffect_Init(
	void      *ptr,  // ���[�N�������[�̐擪
	HC_EFFECT effect // �G�t�F�N�g�ԍ�
	);

// ��ʐؑփ��C��
// �ߒl
//     0 : ���ɃG�t�F�N�g���I�����Ă���
//     1 : �G�t�F�N�g���s��
extern int HCEffect_Main();

// ��ʐؑ֕`��
extern void HCEffect_Grap(
	Gfx **gpp        // �\�z��
	);

#endif
