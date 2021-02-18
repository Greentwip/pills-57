/***********************************************************
	ai data header file		Tadashi Ogura
***********************************************************/

enum {
	AIDT_DEAD,			// ���񂾌���K���[�h�p
	AIDT_NARROW,		// �ړ��\�͈͂������Ȃ��Ă�����
	AIDT_LASTVS,		// �I�Ղų�ٽ�����鎞
	AIDT_LASTVSNH,		// �I�Ղų�ٽ���Ȃ����ō����ς񂾶�߾ق�������
	AIDT_LASTVSN,		// �I�Ղų�ٽ���Ȃ���
	AIDT_BADLINE2,		// �����鍂�ς�ײ݂������Qײ݂ɂ��鎞
	AIDT_BADLINE1,		// �����Ȃ����ς�ײ݂������Uײ݂ɂ��鎞
	AIDT_NORMAL,		//
};

typedef struct {
	s16		dt_LnOnLinVrs;
	s16		dt_LnEraseVrs;
	s16		dt_EraseLinP1;
	s16		dt_HeiEraseLinRate;
	s16		dt_WidEraseLinRate;
	s16		dt_P_ALN_FallCap;
	s16		dt_P_ALN_FallVrs;
	s16		dt_P_ALN_Capsule;
	s16		dt_P_ALN_Virus;
	s16		dt_WP_ALN_Bottom;
	s16		dt_WP_ALN_FallCap;
	s16		dt_WP_ALN_FallVrs;
	s16		dt_WP_ALN_Capsule;
	s16		dt_WP_ALN_Virus;
	s16		dt_P_ALN_HeightP;
	s16		dt_OnVirusP;
	s16		dt_RensaP;
	s16		dt_RensaMP;
	s16		dt_HeiLinesAllp2;
	s16		dt_HeiLinesAllp3;
	s16		dt_HeiLinesAllp4;
	s16		dt_HeiLinesAllp5;
	s16		dt_HeiLinesAllp6;
	s16		dt_HeiLinesAllp7;
	s16		dt_HeiLinesAllp8;
	s16		dt_WidLinesAllp2;
	s16		dt_WidLinesAllp3;
	s16		dt_WidLinesAllp4;
	s16		dt_WidLinesAllp5;
	s16		dt_WidLinesAllp6;
	s16		dt_WidLinesAllp7;
	s16		dt_WidLinesAllp8;

	s16		dt_LnHighCaps;
	s16		dt_LnHighVrs;

	s16		dt_aiWall_F;
} AI_DATA;
