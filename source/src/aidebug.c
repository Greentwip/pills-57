// dims.or.jp/blender/blender_jp.html
#include <ultra64.h>
#include "dbg_font.h"
#include "nnsched.h"
#include "joy.h"
#include "evsworks.h"
#include "aiset.h"
#include "aidata.h"
#include "aidebug.h"
#include "util.h"

// 10進数の桁毎の単位テーブル
static int s_KetaNumTbl[] = { 1, 10, 100, 1000, };

// 各種文字列テーブル
static char *s_LogicStrTbl[] = {
	"FastErase", "SmallRensa", "Rensa&Erase",
	"Rensa&Attack", "WidErase", "MajorityRensa",
};
static char *s_StateStrTbl[] = {
	"Dead", "Narrow", "Last", "LastVsnh",
	"LastVsn", "Badline2", "Badline1", "Normal",
};
static char *s_OffOnStrTbl[] = {
	"Off", "On",
};
static char *s_CharaStrTbl[] = {
	"Mario", "NokoNoko", "Bomhei", "PukuPuku",
	"Chiropoo", "Heihoo", "Hanachan", "Telesa",
	"PakkunFlower", "Kamek", "Koopa", "Peach",
	"Plain1", "Plain2", "Plain3", "Plain4",
};
static char *s_SpeedStrTbl[] = {
	"SpSlow", "Slower", "Slow", "Fast",
	"Faster", "Fastest", "NoWait", "FasterNW",
};
static char *s_ConditionStrTbl[] = {
	"--------------",
	"Unconditional", "Random", "Damage", "Many Vir",
	"Few Vir", "Pile", "Easy", "Normal",
	"Hard", "Last", "Rensa", "PreemPtive",
};
static char *s_EffectStrTbl[] = {
	"----------",
	"Rotate", "Waver", "Not Move", "beFast",
	"beSlow", "beNoWait", "Rapid", "be Attack",
	"be Erase", "Lose",
};

// パラメータを編集する為のワーク
static s16 s_DbgParamTbl[40];

// カーソル点滅カウンタ
#define CURSOR_BLINK_MASK (1 << 4)
static int s_CurBlinkCnt;

//////////////////////////////////////////////////////////////////////////////
// スピード調整画面

extern u8 aiVirusLevel[][3], aiDownSpeed[][3],
	aiSlideSpeed[][3], aiSlideFSpeed[][3];

static u8 (*s_AiSpeedTbl[4])[3] = {
	aiVirusLevel, aiDownSpeed, aiSlideSpeed, aiSlideFSpeed,
};

static int s_DbgSpeedCur;

static void _DebugSpeedCallback(Gfx **gpp, int x, int y, int marker)
{
	static int column;
	int trg = joyupd[0];
	int rep = joycur[0];
	int i;
	u8 *ptr;

	if(marker != s_DbgSpeedCur) {
		return;
	}

	if(gpp) {
		if(++s_CurBlinkCnt & CURSOR_BLINK_MASK) {
			DbgFont_DrawAscii(gpp,
				x + DBG_FONT_W * (2 - column),
				y + DBG_FONT_H, '^');
		}
	}
	else {
		i = 0;
		if(trg & CONT_LEFT) i++;
		if(trg & CONT_RIGHT) i--;
		column = CLAMP(0, 2, column + i);

		i = 0;
		if(rep & CONT_UP) i++;
		if(rep & CONT_DOWN) i--;
		ptr = &s_AiSpeedTbl[marker / 21][marker % 7 + 1][marker / 7 % 3];
		*ptr = CLAMP(0, 30, *ptr + i * s_KetaNumTbl[column]);
	}
}

void DebugSpeed_Format(Gfx **gpp, int x, int y)
{
	static char *kind_tbl[4] = { "VirusLv", "DownS", "SlideS", "SlideFS", };
	static char *diff_tbl[3] = { "Easy", "Normal", "Hard", };
	EnumMarkerCallback *func = _DebugSpeedCallback;
	int i, j, k;

	// スピード調整項目
	DbgCon_Open(gpp, func, x, y + DBG_FONT_H);
	DbgCon_Format("@c2");
	for(i = 0; i < 4; i++) {
		DbgCon_Format("%s\n\n\n\n\n\n", kind_tbl[i]);
	}
	DbgCon_Close();

	// 難易度
	DbgCon_Open(gpp, func, x + DBG_FONT_W * 7, y + DBG_FONT_H);
	DbgCon_Format("@c2");
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 3; j++) {
			DbgCon_Format("%6s\n\n", diff_tbl[j]);
		}
	}
	DbgCon_Close();

	// ウィルスのキー操作モード
	DbgCon_Open(gpp, func, x + DBG_FONT_W * 14, y);
	DbgCon_Format("@c2S_er Slow Fast F_er Fest N_Wt F_NW");
	DbgCon_Close();

	// 各数値
	DbgCon_Open(gpp, func, x + DBG_FONT_W * 15, y + DBG_FONT_H);
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 3; j++) {
			for(k = 1; k < 8; k++) {
				DbgCon_Format("@m%c%03d  ", i*21 + j*7 + k - 1, s_AiSpeedTbl[i][k][j]);
			}
			DbgCon_Format("\n\n");
		}
	}
	DbgCon_Close();

	// ガイド
	DbgCon_Open(gpp, func, x, y + DBG_FONT_H * 25);
	DbgCon_Format("@c3<- CharacterMenu\t\t\t\t\tLogicMenu ->");
	DbgCon_Close();
}

//////////////////////////////////////////////////////////////////////////////
// ロジック調整画面

static int s_DbgLogicCur;
static int s_DbgLogicNo;
static int s_DbgLogicState;

static void _DebugLogic_GetParam()
{
	AI_DATA *param = &ai_param[s_DbgLogicNo][s_DbgLogicState];
	s16 *tbl = s_DbgParamTbl;

	// 0
	*tbl++ = s_DbgLogicNo;
	*tbl++ = s_DbgLogicState;

	// 2
	*tbl++ = param->dt_LnOnLinVrs;
	*tbl++ = param->dt_LnEraseVrs;
	*tbl++ = param->dt_EraseLinP1;
	*tbl++ = param->dt_HeiEraseLinRate;
	*tbl++ = param->dt_WidEraseLinRate;

	// 7
	*tbl++ = param->dt_P_ALN_HeightP;
	*tbl++ = param->dt_P_ALN_FallCap;
	*tbl++ = param->dt_P_ALN_FallVrs;
	*tbl++ = param->dt_P_ALN_Capsule;
	*tbl++ = param->dt_P_ALN_Virus;
	*tbl++ = param->dt_WP_ALN_Bottom;
	*tbl++ = param->dt_WP_ALN_FallCap;
	*tbl++ = param->dt_WP_ALN_FallVrs;
	*tbl++ = param->dt_WP_ALN_Capsule;
	*tbl++ = param->dt_WP_ALN_Virus;

	// 17
	*tbl++ = param->dt_OnVirusP;
	*tbl++ = param->dt_RensaP;
	*tbl++ = param->dt_RensaMP;
	*tbl++ = param->dt_LnHighCaps;
//	s_DbgParamTbl[20]	= param->dt_LnHighVrs;
	*tbl++ = param->dt_aiWall_F;
	*tbl++ = param->dt_EraseLinP3;

	// 23
	*tbl++ = param->dt_HeiLinesAllp2;
	*tbl++ = param->dt_HeiLinesAllp3;
	*tbl++ = param->dt_HeiLinesAllp4;

	// 26
	*tbl++ = param->dt_WidLinesAllp2;
	*tbl++ = param->dt_WidLinesAllp3;
	*tbl++ = param->dt_WidLinesAllp4;

	// 29
	*tbl++ = evs_gamespeed;
	*tbl++ = aiDebugP1 + 1;
}

static void _DebugLogic_PutParam()
{
	AI_DATA *param = &ai_param[s_DbgLogicNo][s_DbgLogicState];
	s16 *tbl = s_DbgParamTbl;

	// 0
	s_DbgLogicNo				= *tbl++;
	s_DbgLogicState				= *tbl++;

	// 2
	param->dt_LnOnLinVrs		= *tbl++;
	param->dt_LnEraseVrs		= *tbl++;
	param->dt_EraseLinP1		= *tbl++;
	param->dt_HeiEraseLinRate	= *tbl++;
	param->dt_WidEraseLinRate	= *tbl++;

	// 7
	param->dt_P_ALN_HeightP		= *tbl++;
	param->dt_P_ALN_FallCap		= *tbl++;
	param->dt_P_ALN_FallVrs		= *tbl++;
	param->dt_P_ALN_Capsule		= *tbl++;
	param->dt_P_ALN_Virus		= *tbl++;
	param->dt_WP_ALN_Bottom		= *tbl++;
	param->dt_WP_ALN_FallCap	= *tbl++;
	param->dt_WP_ALN_FallVrs	= *tbl++;
	param->dt_WP_ALN_Capsule	= *tbl++;
	param->dt_WP_ALN_Virus		= *tbl++;

	// 17
	param->dt_OnVirusP			= *tbl++;
	param->dt_RensaP			= *tbl++;
	param->dt_RensaMP			= *tbl++;
	param->dt_LnHighCaps		= *tbl++;
	param->dt_LnHighVrs			= param->dt_LnHighCaps;
	param->dt_aiWall_F			= *tbl++;
	param->dt_EraseLinP3		= *tbl++;

	// 23
	param->dt_HeiLinesAllp2		= *tbl++;
	param->dt_HeiLinesAllp3		= *tbl++;
	param->dt_HeiLinesAllp4		= *tbl++;

	// 26
	param->dt_WidLinesAllp2		= *tbl++;
	param->dt_WidLinesAllp3		= *tbl++;
	param->dt_WidLinesAllp4		= *tbl++;

	// 29
	evs_gamespeed	= *tbl++;
	aiDebugP1		= *tbl++ - 1;
}

static void _DebugLogicCallback(Gfx **gpp, int x, int y, int marker)
{
	static int column;
	int trg = joyupd[0];
	int rep = joycur[0];
	int min_val, max_val, keta_num, i;

	if(marker != s_DbgLogicCur) {
		return;
	}

	min_val = 0;
	keta_num = 2;
	if(marker == 0) {
		// 消去ロジック(AI_LOGIC_????)
		max_val = _AI_LOGIC_SUM - 1;
	} else if(marker == 1) {
		// 消去ロジックの状況(AI_STATE_????)
		max_val = _AI_STATE_SUM - 1;
	} else if(marker == 21) {
		// WallP
		max_val = 1;
	} else if(marker < 29) {
		// 消去ロジックの加算、乗算パラメータ
		min_val = -9999;
		max_val = 9999;
		keta_num = 5;
	} else if(marker == 29) {
		// デバッグ中のゲームスピード
		max_val = 20;
		keta_num = 3;
	} else if(marker == 30) {
		// プレイヤー１の操作を行うキャラ
		max_val = 16;
		keta_num = 3;
	}

	if(gpp) {
		if(++s_CurBlinkCnt & CURSOR_BLINK_MASK) {
			DbgFont_DrawAscii(gpp,
				x + DBG_FONT_W * (keta_num - column - 1),
				y + DBG_FONT_H, '^');
		}
	}
	else {
		i = 0;
		if(trg & CONT_LEFT) i++;
		if(trg & CONT_RIGHT) i--;
		column = CLAMP(0, keta_num - 1, column + i);

		i = 0;
		if(rep & CONT_UP) i++;
		if(rep & CONT_DOWN) i--;
		s_DbgParamTbl[marker] = CLAMP(min_val, max_val,
			s_DbgParamTbl[marker] + s_KetaNumTbl[column] * i);
	}
}

void DebugLogic_Format(Gfx **gpp, int x, int y)
{
	EnumMarkerCallback *func = _DebugLogicCallback;
	s16 *tbl = s_DbgParamTbl;
	int i, j = 0;

	DbgCon_Open(gpp, func, x, y);

	DbgCon_Format(
		"@c2Logic             State\n"
		"@c7 @m%c%02d:%-14s @m%c%02d:%-14s\n\n",
		j+0, tbl[j+0], s_LogicStrTbl[tbl[j+0]],
		j+1, tbl[j+1], s_StateStrTbl[tbl[j+1]]);
	j += 2;

	DbgCon_Format(
		"@c2OLVrs  ErVrs  ErL1   HeiEr   WidEr   HeightP\n"
		"@c7");
	for(i = 0; i < 6; i++) {
		char *fmt = (i==3 || i==4) ? " @m%c%05d%% " : " @m%c%05d ";
		DbgCon_Format(fmt, j, tbl[j]);
		j++;
	}
	DbgCon_Format("\n\n");

	DbgCon_Format(
		"@c2\tALNB   FC     FV     C      V\n"
		"@c2Hei@c7  -----");
	for(i = 1; i < 5; i++) {
		DbgCon_Format("  @m%c%05d", j, tbl[j]);
		j++;
	}
	DbgCon_Format(
		"\n\n"
		"@c2Wid@c7");
	for(i = 0; i < 5; i++) {
		DbgCon_Format("  @m%c%05d", j, tbl[j]);
		j++;
	}
	DbgCon_Format("\n\n");

	DbgCon_Format(
		"@c2OnVrs  Rensa  MRensa HiEr    WallP   ErOL3\n"
		"@c7 @m%c%05d  @m%c%05d  @m%c%05d  @m%c%05d%%  @m%c%02d:%-3s  @m%c%05d\n"
		"\n",
		j+0, tbl[j+0],
		j+1, tbl[j+1],
		j+2, tbl[j+2],
		j+3, tbl[j+3],
		j+4, tbl[j+4], s_OffOnStrTbl[tbl[j+4]],
		j+5, tbl[j+5]);
	j += 6;

	DbgCon_Format(
		"@c2\tLine2  Line3  Line4~8\n"
		"@c2Hei@c7");
	for(i = 0; i < 3; i++) {
		DbgCon_Format("  @m%c%05d", j, tbl[j]);
		j++;
	}
	DbgCon_Format(
		"\n\n"
		"@c2Wid@c7");
	for(i = 0; i < 3; i++) {
		DbgCon_Format("  @m%c%05d", j, tbl[j]);
		j++;
	}

	DbgCon_Format(
		"\n\n"
		"@c2DebugSpeed PlayerCom\n"
		"@c7 @m%c%03d        @m%c%03d:%-14s\n"
		"\n\n\n\n",
		j+0, tbl[j+0],
		j+1, tbl[j+1], tbl[j+1] ? s_CharaStrTbl[tbl[j+1]-1] : "Man");
	j += 2;

	DbgCon_Format("@c3<- SpeedMenu\t\t\t\t\tCharacterMenu ->");

	DbgCon_Close();
}

//////////////////////////////////////////////////////////////////////////////
// キャラクター調整画面

#define NUM_DISP_EFFECT 4
#define MID_EFFECT_TOP 12
#define MID_EFFECT_END (MID_EFFECT_TOP + NUM_DISP_EFFECT * 4 - 1)

static int s_DbgCharaCur;
static int s_DbgCharaNo;
static int s_DbgEffectTop;

static void _DebugChara_GetParam()
{
	AI_CHAR *chara = &ai_char_data[s_DbgCharaNo];
	s16 *param = s_DbgParamTbl;
	int i;

	*param++ = s_DbgCharaNo;
	*param++ = chara->wait_attack;
	*param++ = chara->speed;
	*param++ = chara->luck;

	for(i = 0; i < _AI_STATE_SUM; i++) {
		*param++ = chara->logic[i];
	}

	for(i = s_DbgEffectTop; i < s_DbgEffectTop + NUM_DISP_EFFECT; i++) {
		*param++ = chara->condition      [i];
		*param++ = chara->condition_param[i];
		*param++ = chara->effect         [i];
		*param++ = chara->effect_param   [i];
	}
}

static void _DebugChara_PutParam()
{
	AI_CHAR *chara = &ai_char_data[s_DbgCharaNo];
	s16 *param = s_DbgParamTbl;
	int i;

	s_DbgCharaNo       = *param++;
	chara->wait_attack = *param++;
	chara->speed       = *param++;
	chara->luck        = *param++;

	for(i = 0; i < _AI_STATE_SUM; i++) {
		chara->logic[i] = *param++;
	}

	for(i = s_DbgEffectTop; i < s_DbgEffectTop + NUM_DISP_EFFECT; i++) {
		chara->condition      [i] = *param++;
		chara->condition_param[i] = *param++;
		chara->effect         [i] = *param++;
		chara->effect_param   [i] = *param++;
	}
}

static void _DebugCharaCallback(Gfx **gpp, int x, int y, int marker)
{
	static int column;
	int trg = joyupd[0];
	int rep = joycur[0];
	int min_val, max_val, keta_num, i;
	int on_condition, on_effect;

	if(marker != s_DbgCharaCur) {
		return;
	}

	on_condition = FALSE;
	on_effect = FALSE;

	min_val = 0;
	keta_num = 2;

	if(marker == 0) {
		// キャラ番号
		max_val = 15;
	} else if(marker == 1) {
		// WaitAttack
		max_val = 1;
	} else if(marker == 2) {
		// 操作速度
		min_val = 1;
		max_val = _AI_SPEED_SUM - 1;
	} else if(marker == 3) {
		// 自分へのダメージカプセルが都合のよい所に落ちる率
		min_val = -100;
		max_val = 100;
		keta_num = 3;
	} else if(marker < MID_EFFECT_TOP) {
		// 消去ロジック
		max_val = _AI_LOGIC_SUM - 1;
	} else if(marker < MID_EFFECT_END) {
		switch((marker - MID_EFFECT_TOP) & 3) {
		// 特殊処理の条件
		case 0: max_val = _AI_CONDITION_SUM - 1; break;
		case 1: max_val = 999; on_condition = TRUE; break;
		// 特殊処理
		case 2: max_val = _AI_EFFECT_SUM - 1; break;
		case 3: max_val = 999; on_effect = TRUE; break;
		}
		keta_num = 3;
	}

	if(gpp) {
		if(++s_CurBlinkCnt & CURSOR_BLINK_MASK) {
			DbgFont_DrawAscii(gpp,
				x + DBG_FONT_W * (keta_num - column - 1),
				y + DBG_FONT_H, '^');
		}
	}
	else {
		int modifiable = TRUE;

		i = 0;
		if(trg & CONT_LEFT) i++;
		if(trg & CONT_RIGHT) i--;
		column = CLAMP(0, keta_num - 1, column + i);

		if(on_condition) {
			switch(s_DbgParamTbl[marker - 1]) {
			case AI_CONDITION_Ignore:
			case AI_CONDITION_Unconditional:
			case AI_CONDITION_Damage:
			case AI_CONDITION_Pile:
			case AI_CONDITION_Easy:
			case AI_CONDITION_Normal:
			case AI_CONDITION_Hard:
			case AI_CONDITION_Rensa:
				modifiable = FALSE;
				break;
			}
		} else if(on_effect) {
			switch(s_DbgParamTbl[marker - 1]) {
			case AI_EFFECT_Ignore:
			case AI_EFFECT_Rotate:
			case AI_EFFECT_Waver:
				modifiable = FALSE;
				break;
			}
		}

		if(modifiable) {
			i = 0;
			if(rep & CONT_UP) i++;
			if(rep & CONT_DOWN) i--;
			s_DbgParamTbl[marker] = CLAMP(min_val, max_val,
				s_DbgParamTbl[marker] + s_KetaNumTbl[column] * i);
		}
	}
}

void DebugChara_Format(Gfx **gpp, int x, int y)
{
	EnumMarkerCallback *func = _DebugCharaCallback;
	s16 *tbl = s_DbgParamTbl;

	int i, j = 0;

	DbgCon_Open(gpp, func, x, y);

	DbgCon_Format(
		"@c2Character         WaitAttack Speed       Luck\n"
		"@c7 @m%c%02d:%-14s @m%c%02d:%-3s     @m%c%02d:%-8s @m%c%03d%%\n\n",
		j+0, tbl[j+0], s_CharaStrTbl[tbl[j+0]],
		j+1, tbl[j+1], s_OffOnStrTbl[tbl[j+1]],
		j+2, tbl[j+2], s_SpeedStrTbl[tbl[j+2]],
		j+3, tbl[j+3]);
	j += 4;

	for(i = 0; i < 4; i++) {
		DbgCon_Format(
			"@c2%-18s %-18s\n"
			"@c7 @m%c%02d:%-14s  @m%c%02d:%-14s\n\n",
			s_StateStrTbl[i * 2 + 0],
			s_StateStrTbl[i * 2 + 1],
			j+0, tbl[j+0], s_LogicStrTbl[tbl[j+0]],
			j+1, tbl[j+1], s_LogicStrTbl[tbl[j+1]]);
		j += 2;
	}

	DbgCon_Format("@c2  ExCondition        Sub   ExEffect       Sub\n");
	for(i = 0; i < NUM_DISP_EFFECT; i++) {
		DbgCon_Format("@c2%02d @c7@m%c%03d:%-14s ",
			s_DbgEffectTop+i+1,
			j, tbl[j], s_ConditionStrTbl[tbl[j]]);
		j++;

		switch(tbl[j-1]) {
		case AI_CONDITION_Ignore:
		case AI_CONDITION_Unconditional:
		case AI_CONDITION_Damage:
		case AI_CONDITION_Pile:
		case AI_CONDITION_Easy:
		case AI_CONDITION_Normal:
		case AI_CONDITION_Hard:
		case AI_CONDITION_Rensa:
			DbgCon_Format("@m%c---   ", j);
			break;
		default:
			DbgCon_Format("@m%c%03d%c  ", j, tbl[j],
				tbl[j-1] == AI_CONDITION_Random ? '%' : ' ');
			break;
		}
		j++;

		DbgCon_Format("@m%c%03d:%-10s ",
			j, tbl[j], s_EffectStrTbl[tbl[j]]);
		j++;

		switch(tbl[j-1]) {
		case AI_EFFECT_Ignore:
		case AI_EFFECT_Rotate:
		case AI_EFFECT_Waver:
			DbgCon_Format("@m%c---", j);
			break;
		default:
			DbgCon_Format("@m%c%03d", j, tbl[j]);
			break;
		}
		j++;

		DbgCon_Format("\n\n");
	}

	DbgCon_Format("\n");
	DbgCon_Format("@c3<- LogicMenu\t\t\t\t\t\tSpeedMenu ->");

	DbgCon_Close();
}

//////////////////////////////////////////////////////////////////////////////
// デバッグ画面のメイン

static int s_DbgModePage;

void DebugMode_Update()
{
	int trg = joyupd[0];
	int rep = joycur[0];
	int i;

	i = 0;
	if(trg & CONT_C) i--;
	if(trg & CONT_F) i++;
	s_DbgModePage = WrapI(0, 3, s_DbgModePage + i);

	i = 0;
	if(rep & CONT_L) i--;
	if(rep & CONT_R) i++;

	switch(s_DbgModePage) {
	case 0:
		s_DbgLogicCur = WrapI(0, 31, s_DbgLogicCur + i);
		_DebugLogic_GetParam();
		DebugLogic_Format(NULL, 16, 16);
		_DebugLogic_PutParam();
		break;
	case 1:
		s_DbgCharaCur = WrapI(0, 28, s_DbgCharaCur + i);
		if(i > 0) {
			if(s_DbgCharaCur == 0) {
				s_DbgEffectTop = WrapI(0, NUM_AI_EFFECT, s_DbgEffectTop + NUM_DISP_EFFECT);
				if(s_DbgEffectTop != 0) {
					s_DbgCharaCur = MID_EFFECT_TOP;
				}
			} else if(s_DbgCharaCur == MID_EFFECT_TOP) {
				s_DbgEffectTop = 0;
			}
		} else if(i < 0) {
			if(s_DbgCharaCur == MID_EFFECT_TOP - 1) {
				if(s_DbgEffectTop != 0) {
					s_DbgCharaCur = MID_EFFECT_END;
					s_DbgEffectTop = WrapI(0, NUM_AI_EFFECT, s_DbgEffectTop - NUM_DISP_EFFECT);
				}
			} else if(s_DbgCharaCur == MID_EFFECT_END) {
				s_DbgEffectTop = NUM_AI_EFFECT - NUM_DISP_EFFECT;
			}
		}
		_DebugChara_GetParam();
		DebugChara_Format(NULL, 16, 16);
		_DebugChara_PutParam();
		break;
	case 2:
		s_DbgSpeedCur = WrapI(0, 84, s_DbgSpeedCur + i);
		DebugSpeed_Format(NULL, 16, 16);
		break;
	}
}

void DebugMode_Draw(Gfx **gpp)
{
	switch(s_DbgModePage) {
	case 0:
		DebugLogic_Format(gpp, 16, 16);
		break;
	case 1:
		DebugChara_Format(gpp, 16, 16);
		break;
	case 2:
		DebugSpeed_Format(gpp, 16, 16);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////
// キャラクター設定画面

// キャラ項目毎の文字列テーブル
static char *s_DbgCfg_Item00_Tbl[] = {
	"ﾏﾘｵ", "ﾉｺﾉｺ", "ﾎﾞﾑﾍｲ", "ﾌﾟｸﾌﾟｸ",
	"ﾁｮﾛﾌﾟｰ", "ﾌﾟﾛﾍﾟﾗﾍｲﾎｰ", "ﾊﾅﾁｬﾝ", "ﾃﾚｻ",
	"ﾊﾟｯｸﾝﾌﾗﾜｰ", "ｶﾒｯｸ", "ｸｯﾊﾟ", "ﾌﾟﾘﾝｾｽﾋﾟｰﾁ",
};
// ロジック項目毎の文字列テーブル
static char *s_DbgCfg_Item01_Tbl[] = {
	"ｽﾋﾟｰﾄﾞ", "ﾊﾞﾗﾝｽ", "ｺｳｹﾞｷ", "ﾀﾞｲﾚﾝｻﾈﾗｲ", "ﾖｺｽﾞｷ",
};
// スピード項目毎の文字列テーブル
static char *s_DbgCfg_Item02_Tbl[] = {
	"ﾋｼﾞｮｳﾆｵｿｲ", "ｵｿｲ", "ﾌﾂｳ", "ﾊﾔｲ", "ﾋｼﾞｮｳﾆﾊﾔｲ", "ﾓｯﾄﾓﾊﾔｲ",
};
// 特殊攻撃項目毎の文字列テーブル
static char *s_DbgCfg_Item03_Tbl[] = {
	"ｾﾝｾｲｺｳｹﾞｷ", "ｶｳﾝﾀｰ", "ﾗｯｸ", "ﾏﾖｲ",
	"ｸﾙｸﾙﾏﾜｽ", "ﾁｮｳﾊﾂ", "ﾄﾞｳﾖｳ", "ｽﾋﾟｰﾄﾞｱｯﾌﾟ",
	"ｵｲｱｹﾞﾙ", "ﾃﾇｷ", "ｵｲｺﾐ", "ﾃﾞﾌｫﾙﾄ",
};

// ロジック項目毎のロジック値
static char s_DbgCfg_Val01_Tbl[] = {
	AI_LOGIC_FastErase,
	AI_LOGIC_RensaAndErase,
	AI_LOGIC_RensaAndAttack,
	AI_LOGIC_MajorityRensa,
	AI_LOGIC_WidErase,
};
// スピード項目毎のスピード値
static char s_DbgCfg_Val02_Tbl[] = {
	AI_SPEED_VERYSLOW,
	AI_SPEED_SLOW,
	AI_SPEED_FAST,
	AI_SPEED_VERYFAST,
	AI_SPEED_MAX,
	AI_SPEED_FLASH,
};

// 分類毎の文字列テーブル
static char **s_DbgCfg_Items_Tbl[4] = {
	s_DbgCfg_Item00_Tbl,
	s_DbgCfg_Item01_Tbl,
	s_DbgCfg_Item02_Tbl,
	s_DbgCfg_Item03_Tbl,
};
// 分類毎のテーブルサイズ
static int s_DbgCfg_ItemsSize_Tbl[4] = {
	ARRAY_SIZE(s_DbgCfg_Item00_Tbl),
	ARRAY_SIZE(s_DbgCfg_Item01_Tbl),
	ARRAY_SIZE(s_DbgCfg_Item02_Tbl),
	ARRAY_SIZE(s_DbgCfg_Item03_Tbl),
};

// 設定項目構造体
typedef struct {
	s8  logic;   // ロジック番号
	s8  speed;   // スピード番号
	u16 special; // 特殊処理フラグ
} SDbgCfgData;

// 設定変更中のキャラクタ番号
static int s_DbgCfg_CharNo;

// 全キャラクタ分の設定値
static SDbgCfgData s_DbgCfg_Data[ARRAY_SIZE(s_DbgCfg_Item00_Tbl)];

// 編集中のキャラクタのデフォルト値
static SDbgCfgData s_DbgCfg_Default;

// カーソルの位置
static int s_DbgCfg_RowPos;
static int s_DbgCfg_ItemPos[4];

// デバッグコンソールからのコールバック処理
static void _DebugConfigCallback(Gfx **gpp, int x, int y, int marker)
{
	SDbgCfgData *cfg = &s_DbgCfg_Data[s_DbgCfg_CharNo];
	int trg = joyupd[0];
	int rep = joycur[0];
	int row = marker >> 5;
	int item = marker & ((1 << 5) - 1);

	if(row != s_DbgCfg_RowPos || item != s_DbgCfg_ItemPos[s_DbgCfg_RowPos]) {
		return;
	}

	if(gpp) {
		if(++s_CurBlinkCnt & CURSOR_BLINK_MASK) {
			DbgFont_SetColor(gpp, 7);
			DbgFont_DrawAscii(gpp, x-DBG_FONT_W, y, '>');
		}
	}
	else if(trg & CONT_A) {
		switch(row) {
		case 0:
			s_DbgCfg_CharNo = s_DbgCfg_ItemPos[row];
			break;

		case 1:
			cfg->logic = s_DbgCfg_ItemPos[row];
			break;

		case 2:
			cfg->speed = s_DbgCfg_ItemPos[row];
			break;

		case 3:
			cfg->special ^= 1 << s_DbgCfg_ItemPos[row];
			break;
		}
	}
}

// 強さパラメータを計算
static int _DebugConfig_CalcGradeParam()
{
	// キャラ項目毎の強さパラメータ
	static signed char s_DbgCfg_Grade00_Tbl[] = {
		0, -14, 0, 0, 2, -2, 0, 2, -2, 4, 2, 0,
	};
	// ロジック項目毎の強さパラメータ
	static signed char s_DbgCfg_Grade01_Tbl[] = {
		33, 40, 25, 10, 34,
	};
	// スピード項目毎の強さパラメータ
	static signed char s_DbgCfg_Grade02_Tbl[] = {
		0, 8, 16, 24, 32, 40,
	};
	// 特殊攻撃項目毎の強さパラメータ
	static signed char s_DbgCfg_Grade03_Tbl[] = {
		 6,  4,  10, -2,
		-2, -2, -10,  4,
		 2, -6,   4,  0,
	};
	SDbgCfgData *cfg = &s_DbgCfg_Data[s_DbgCfg_CharNo];
	SDbgCfgData cfgVal = *cfg;
	int i, doMask, grade = 0;

	// 特定の項目が選択されることにより、特殊攻撃を無効にする。
	for(i = 0; i < s_DbgCfg_ItemsSize_Tbl[3]; i++) {

		switch(cfgVal.special & (1 << i)) {
		// 先制攻撃
		case (1 << 0):
			// 攻撃 and (非常に速い or 最も速い)
			doMask = (cfgVal.logic == 2 && cfgVal.speed >= 4);
			break;

		// カウンター
		case (1 << 1):
			// 攻撃
			doMask = (cfgVal.logic == 2);
			break;

		// ラック
		case (1 << 2): break;
		// 迷い
		case (1 << 3): break;
		// クルクル回す
		case (1 << 4): break;
		// 挑発
		case (1 << 5): break;
		// 動揺
		case (1 << 6): break;

		// スピードアップ
		case (1 << 7):
			// 非常に速い or 最も速い
			doMask = (cfgVal.speed >= 4);
			break;

		// 追い上げる
		case (1 << 8):
			// 非常に速い or 最も速い
			doMask = (cfgVal.speed >= 4);
			break;

		// 手抜
		case (1 << 9):
			// 非常に遅い or 遅い
			doMask = (cfgVal.speed <= 1);
			break;

		// 追い込み
		case (1 << 10):
			// 非常に速い or 最も速い を無効にする
			doMask = (cfgVal.speed >= 4);
			break;

		// デフォルト
		case (1 << 11):
			switch(s_DbgCfg_CharNo) {
			// マリオ
			case 0: break;

			// ノコノコ
			case 1:
				// 迷い or くるくる回す or 挑発
				doMask = (cfgVal.special & ((1 << 3) | (1 << 4) | (1 << 6)));
				break;

			// ボムヘイ
			case 2: break;
			// プクプク
			case 3: break;

			// チョロプー
			case 4:
				// 最も速い
				doMask = (cfgVal.speed == 5);
				break;

			// プロペラヘイホー
			case 5:
				// くるくる回す
				doMask = (cfgVal.special & (1 << 4));
				break;

			// ハナチャン
			case 6: break;

			// テレサ
			case 7:
				// スピードアップ
				doMask = (cfgVal.special & (1 << 7));
				break;

			// パックンフラワー
			case 8:
				// 迷い
				doMask = (cfgVal.special & (1 << 3));
				break;

			// カメック
			case 9:
				// 非常に速い or 最も速い
				doMask = (cfgVal.speed >= 4);
				break;

			// クッパ
			case 10:
				// 非常に速い or 最も速い or カウンター or 追い込み
				doMask = (cfgVal.speed >= 4) ||
					(cfgVal.special & ((1 << 1) | (1 << 10)));
				break;

			// ピーチ姫
			case 11: break;
			}

			break;
		}

		if(doMask) {
			cfgVal.special &= ~(1 << i);
		}
	}
/*
	// 選択中のキャラクタによって、特定の項目を無効にする
	switch(s_DbgCfg_CharNo) {
	// マリオ
	case 0: break;

	// ノコノコ
	case 1:
		// 迷い、くるくる回す、挑発 を無効にする
		cfgVal.special &= ~((1 << 3) | (1 << 4) | (1 << 6));
		break;

	// ボムヘイ
	case 2: break;
	// プクプク
	case 3: break;

	// チョロプー
	case 4:
		// 最も速い を無効にする
		if(cfgVal.speed == 5) {
			cfgVal.speed = -1;
		}
		break;

	// プロペラヘイホー
	case 5:
		// くるくる回す を無効にする
		cfgVal.special &= ~(1 << 4);
		break;

	// ハナチャン
	case 6: break;

	// テレサ
	case 7:
		// スピードアップ を無効にする
		cfgVal.special &= ~(1 << 7);
		break;

	// パックンフラワー
	case 8:
		// 迷い を無効にする
		cfgVal.special &= ~(1 << 3);
		break;

	// カメック
	case 9:
		// 非常に速い、最も速い を無効にする
		if(cfgVal.speed >= 4) {
			cfgVal.speed = -1;
		}
		break;

	// クッパ
	case 10:
		// 非常に速い、最も速い を無効にする
		if(cfgVal.speed >= 4) {
			cfgVal.speed = -1;
		}
		// カウンター、追い込み を無効にする
		cfgVal.special &= ~((1 << 1) | (1 << 10));
		break;

	// ピーチ姫
	case 11: break;
	}


	// 設定されている特殊処理によって、特定の項目を無効にする。
	for(i = 0; i < s_DbgCfg_ItemsSize_Tbl[3]; i++) {
		switch(cfgVal.special & (1 << i)) {
		// 先制攻撃
		case (1 << 0):
			// 攻撃 を無効にする
			if(cfgVal.logic == 2) {
				cfgVal.logic = -1;
			}
			// 非常に速い、最も速い を無効にする
			if(cfgVal.speed >= 4) {
				cfgVal.speed = -1;
			}
			break;

		// カウンター
		case (1 << 1):
			// 攻撃 を無効にする
			if(cfgVal.logic == 2) {
				cfgVal.logic = -1;
			}
			break;

		// ラック
		case (1 << 2): break;
		// 迷い
		case (1 << 3): break;
		// クルクル回す
		case (1 << 4): break;
		// 挑発
		case (1 << 5): break;
		// 動揺
		case (1 << 6): break;

		// スピードアップ
		case (1 << 7):
			// 非常に速い、最も速い を無効にする
			if(cfgVal.speed >= 4) {
				cfgVal.speed = -1;
			}
			break;

		// 追い上げる
		case (1 << 8):
			// 非常に速い、最も速い を無効にする
			if(cfgVal.speed >= 4) {
				cfgVal.speed = -1;
			}
			break;

		// 手抜
		case (1 << 9):
			// 非常に遅い、遅い を無効にする
			if(cfgVal.speed <= 1) {
				cfgVal.speed = -1;
			}
			break;

		// 追い込み
		case (1 << 10):
			// 非常に速い、最も速い を無効にする
			if(cfgVal.speed >= 4) {
				cfgVal.speed = -1;
			}
			break;
		}
	}
*/
	// キャラクタ項目の得点を加算
	if(cfgVal.special & (1 << 11)) {
		grade += s_DbgCfg_Grade00_Tbl[s_DbgCfg_CharNo];
	}

	// ロジック項目毎の得点を計算
	if(cfgVal.logic >= 0) {
		grade += s_DbgCfg_Grade01_Tbl[cfgVal.logic];
	}

	// スピード項目の得点を加算
	if(cfgVal.speed >= 0) {
		grade += s_DbgCfg_Grade02_Tbl[cfgVal.speed];
	}

	// エフェクト項目の得点を加算
	for(i = 0; i < ARRAY_SIZE(s_DbgCfg_Grade03_Tbl); i++) {
		if(cfgVal.special & (1 << i)) {
			grade += s_DbgCfg_Grade03_Tbl[i];
		}
	}

	return grade;
}

// AI_CHAR 構造体から SDbgCfgData 構造体へ値を移す
static void _DebugConfig_GetParam()
{
	static char logicToCfgTbl[_AI_LOGIC_SUM] = {
		0, 0, 1, 2, 4, 3,
	};
	static char speedToCfgTbl[_AI_SPEED_SUM] = {
		0, 0, 1, 2, 3, 4, 5, 5,
	};
	static int initialized;
	AI_CHAR *chara, *chara_org;
	SDbgCfgData *cfg;
	SDbgCfgData *defCfg = &s_DbgCfg_Default;
	int i, j;

	for(i = 0; i < s_DbgCfg_ItemsSize_Tbl[0]; i++) {
		chara_org = &ai_char_data_org[i];
		chara = &ai_char_data[i];
		cfg = &s_DbgCfg_Data[i];

		cfg->logic = logicToCfgTbl[chara->logic[AI_STATE_NORMAL]];
		cfg->speed = speedToCfgTbl[chara->speed];

		if(!initialized) {
			cfg->special = 1 << 11;
		}

		if(i == s_DbgCfg_CharNo) {
			defCfg->logic = logicToCfgTbl[chara_org->logic[AI_STATE_NORMAL]];
			defCfg->speed = speedToCfgTbl[chara_org->speed];
		}
	}

	initialized = 1;
}

// SDbgCfgData 構造体から AI_CHAR 構造体へ値を移す
static void _DebugConfig_PutParam()
{
	AI_CHAR *chara, *chara_org;
	SDbgCfgData *cfg;
	int i, j, effectCount;

	for(i = 0; i < s_DbgCfg_ItemsSize_Tbl[0]; i++) {
		chara_org = &ai_char_data_org[i];
		chara = &ai_char_data[i];
		cfg = &s_DbgCfg_Data[i];

		// ロジックを設定
		for(j = 0; j < _AI_STATE_SUM; j++) {
			chara->logic[j] = s_DbgCfg_Val01_Tbl[cfg->logic];
		}

		// スピードを設定
		chara->speed = s_DbgCfg_Val02_Tbl[cfg->speed];

		// デフォルトのエフェクトを設定
		effectCount = 0;
		if(cfg->special & (1 << 11)) {
			for(j = 0; j < 4; j++) {
				chara->condition      [effectCount] = chara_org->condition      [j];
				chara->condition_param[effectCount] = chara_org->condition_param[j];
				chara->effect         [effectCount] = chara_org->effect         [j];
				chara->effect_param   [effectCount] = chara_org->effect_param   [j];
				effectCount++;
			}
		}

		// エフェクトを設定
		for(j = 0; j < s_DbgCfg_ItemsSize_Tbl[3]; j++) {
			switch(cfg->special & (1 << j)) {
			// 先制攻撃
			case (1 << 0):
				chara->condition      [effectCount] = AI_CONDITION_PreemPtive;
				chara->condition_param[effectCount] = 5;
				chara->effect         [effectCount] = AI_EFFECT_BeFast;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				chara->condition      [effectCount] = AI_CONDITION_PreemPtive;
				chara->condition_param[effectCount] = 5;
				chara->effect         [effectCount] = AI_EFFECT_BeAttack;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				break;

			// カウンター
			case (1 << 1):
				chara->condition      [effectCount] = AI_CONDITION_Damage;
				chara->condition_param[effectCount] = 0;
				chara->effect         [effectCount] = AI_EFFECT_BeAttack;
				chara->effect_param   [effectCount] = 7;
				effectCount++;
				break;

			// ラック
			case (1 << 2):
				chara->luck = 50;
				break;

			// 迷い
			case (1 << 3):
				chara->condition      [effectCount] = AI_CONDITION_Random;
				chara->condition_param[effectCount] = 33;
				chara->effect         [effectCount] = AI_EFFECT_Waver;
				chara->effect_param   [effectCount] = 0;
				effectCount++;
				break;

			// クルクル回す
			case (1 << 4):
				chara->condition      [effectCount] = AI_CONDITION_Unconditional;
				chara->condition_param[effectCount] = 0;
				chara->effect         [effectCount] = AI_EFFECT_Rotate;
				chara->effect_param   [effectCount] = 0;
				effectCount++;
				break;

			// 挑発
			case (1 << 5):
				chara->condition      [effectCount] = AI_CONDITION_FewVir;
				chara->condition_param[effectCount] = 5;
				chara->effect         [effectCount] = AI_EFFECT_Rotate;
				chara->effect_param   [effectCount] = 0;
				effectCount++;
				break;

			// 動揺
			case (1 << 6):
				chara->condition      [effectCount] = AI_CONDITION_Damage;
				chara->condition_param[effectCount] = 0;
				chara->effect         [effectCount] = AI_EFFECT_NotMove;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				break;

			// スピードアップ
			case (1 << 7):
				chara->condition      [effectCount] = AI_CONDITION_Unconditional;
				chara->condition_param[effectCount] = 0;
				chara->effect         [effectCount] = AI_EFFECT_Rapid;
				chara->effect_param   [effectCount] = 0;
				effectCount++;
				break;

			// 追い上げる
			case (1 << 8):
				chara->condition      [effectCount] = AI_CONDITION_ManyVir;
				chara->condition_param[effectCount] = 5;
				chara->effect         [effectCount] = AI_EFFECT_BeNoWait;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				break;

			// 手抜
			case (1 << 9):
				chara->condition      [effectCount] = AI_CONDITION_FewVir;
				chara->condition_param[effectCount] = 5;
				chara->effect         [effectCount] = AI_EFFECT_BeSlow;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				break;

			// 追い込み
			case (1 << 10):
				chara->condition      [effectCount] = AI_CONDITION_Last;
				chara->condition_param[effectCount] = 10;
				chara->effect         [effectCount] = AI_EFFECT_BeFast;
				chara->effect_param   [effectCount] = 1;
				effectCount++;
				break;
			}
		}

		for(j = effectCount; j < NUM_AI_EFFECT; j++) {
			chara->condition      [j] = AI_CONDITION_Ignore;
			chara->condition_param[j] = 0;
			chara->effect         [j] = AI_EFFECT_Ignore;
			chara->effect_param   [j] = 0;
		}
	}
}

// キー入力
static void _DebugConfigInput()
{
	SDbgCfgData *cfg = &s_DbgCfg_Data[s_DbgCfg_CharNo];
	int trg = joyupd[0];
	int rep = joycur[0];
	int curX, curY;

	curX = curY = 0;
	if(trg & CONT_LEFT) curX--;
	if(trg & CONT_RIGHT) curX++;
	if(trg & CONT_UP) curY--;
	if(trg & CONT_DOWN) curY++;

	s_DbgCfg_RowPos = WrapI(0, 4, s_DbgCfg_RowPos + curX);

	s_DbgCfg_ItemPos[s_DbgCfg_RowPos] = WrapI(0,
		s_DbgCfg_ItemsSize_Tbl[s_DbgCfg_RowPos],
		s_DbgCfg_ItemPos[s_DbgCfg_RowPos] + curY);
}

// デバッグ出力へ文字列を出力
void DebugConfig_Format(Gfx **gpp, int x, int y)
{
	// 分類毎のヘッダー名
	static char *s_DbgCfg_Header_Tbl[4] = {
		"<ｷｬﾗｸﾀｰ>", "<ｾｲｶｸ>", "<ｽﾋﾟｰﾄﾞ>", "<ﾄｸｼｭ>",
	};
	static int defColor = '5';
	static int stepX_Tbl[] = { 11, 10, 10, 14 };

	EnumMarkerCallback *func = _DebugConfigCallback;
	SDbgCfgData *cfg = &s_DbgCfg_Data[s_DbgCfg_CharNo];
	SDbgCfgData *defCfg = &s_DbgCfg_Default;
	int i, j, marker, color, check, grade;;

	grade = CLAMP(0, 10, _DebugConfig_CalcGradeParam() / 10);
	DbgCon_Open(gpp, func, x, y);
	DbgCon_Format("@c2[ｷｬﾗｸﾀｰｴﾃﾞｨｯﾄ]\n\n");
	DbgCon_Format("@c2ﾂﾖｻ ");
	for(i = 0; i < grade; i++) {
		DbgCon_Format("%c", DBG_FONT_CHAR_STAR_1);
	}
	for(; i < 10; i++) {
		DbgCon_Format("%c", DBG_FONT_CHAR_STAR_0);
	}
	DbgCon_Close();

	x += DBG_FONT_W;
	y += DBG_FONT_H * 4;

	for(i = 0; i < 4; i++) {
		DbgCon_Open(gpp, func, x, y);
		DbgCon_Format("@c2%s\n", s_DbgCfg_Header_Tbl[i]);

		marker = i << 5;

		for(j = 0; j < s_DbgCfg_ItemsSize_Tbl[i]; j++) {
			color = '7';

			switch(i) {
			case 0:
				if(j == s_DbgCfg_CharNo) {
					color = '3';
				}
				break;

			case 1:
				if(j == cfg->logic) {
					color = '3';
				}
				else if(j == defCfg->logic) {
					color = defColor;
				}
				break;

			case 2:
				if(j == cfg->speed) {
					color = '3';
				}
				else if(j == defCfg->speed) {
					color = defColor;
				}
				break;

			case 3:
				check = DBG_FONT_CHAR_CHECK_0;
				if(cfg->special & (1 << j)) {
					color = '3';
					check = DBG_FONT_CHAR_CHECK_1;
				}
				else if(j == 11) {
					color = defColor;
				}
				break;
			}

			switch(i) {
			case 0: case 1: case 2:
				DbgCon_Format("@m%c@c%c%s\n", marker, color, s_DbgCfg_Items_Tbl[i][j]);
				marker++;
				break;

			case 3:
				DbgCon_Format("@m%c@c%c%c:%s\n", marker, color, check, s_DbgCfg_Items_Tbl[i][j]);
				marker++;
				break;
			}
		}

		DbgCon_Close();
		x += stepX_Tbl[i] * DBG_FONT_W;
	}
}

// キャラクタ設定画面のキー入力処理
void DebugConfig_Update()
{
	_DebugConfigInput();
	_DebugConfig_GetParam();
	DebugConfig_Format(NULL, 32, 32);
	_DebugConfig_PutParam();
}

// キャラクタ設定画面の描画処理
void DebugConfig_Draw(Gfx **gpp)
{
	DebugConfig_Format(gpp, 32, 32);
}
