#ifndef		__PassWD_H__
#define		__PassWD_H__

//---------------------------------------------------------------------------
// u16 *passwd				�p�X���[�h�̕����f�[�^�i�S�p�Ȃ̂�u16�j�P�W����
// u8  game_mode= 2bit		0:�I���W�i���@1:�X�R�A�A�^�b�N�@2:�������イ
// u8  level	= 8bit		�N���A�X�e�[�W Lv.0�`255  �܂���  �Q�[�����x�� 0:Easy  1:Normal  2:Hard
// u8  speed	= 2bit		�J�v�Z���������x 0:Low  1:Med  2:Hi
// u32 score	= 20bit		�X�R�A�̕\�����e��1/10�����l ( 0 �` 999999 )
// u16 time		= 16bit		���Ԃ�1/10�b�P�ʂɂ����l�i 0'00.0 �` 99'59.9 = 0 �` 59999 �j
// u8  name[4]	= 32bit		���O�i�S�����j
// ---------------------
//				�v80bit + 8bit + 2bit = 90bit
//---------------------------------------------------------------------------
extern	char *make_passwd(u16 *passwd, u8 game_mode, u8 level, u8 speed, u32 score, u16 time, u8 *name);

#endif
