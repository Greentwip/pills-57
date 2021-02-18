#ifndef		__PassWD_H__
#define		__PassWD_H__

//---------------------------------------------------------------------------
// u16 *passwd				パスワードの文字データ（全角なのでu16）１８文字
// u8  game_mode= 2bit		0:オリジナル　1:スコアアタック　2:たいきゅう
// u8  level	= 8bit		クリアステージ Lv.0～255  または  ゲームレベル 0:Easy  1:Normal  2:Hard
// u8  speed	= 2bit		カプセル落下速度 0:Low  1:Med  2:Hi
// u32 score	= 20bit		スコアの表示内容を1/10した値 ( 0 ～ 999999 )
// u16 time		= 16bit		時間を1/10秒単位にした値（ 0'00.0 ～ 99'59.9 = 0 ～ 59999 ）
// u8  name[4]	= 32bit		名前（４文字）
// ---------------------
//				計80bit + 8bit + 2bit = 90bit
//---------------------------------------------------------------------------
extern	char *make_passwd(u16 *passwd, u8 game_mode, u8 level, u8 speed, u32 score, u16 time, u8 *name);

#endif
