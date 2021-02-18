#include <ultra64.h>
#include "passwd.h"

#define	STRNUM	32		// Žg—p•¶Žš”
#define	STRBIT	5		// ‚»‚Ìƒrƒbƒg”
#define	STRMSK	0x1f	// ‚»‚Ìƒ}ƒXƒN’l

extern	u32	framecont;

static	char	strlist_full[] =
	"‚`‚a‚b‚c‚d‚e‚f‚g‚i‚j"
	"‚k‚l‚m‚o‚p‚q‚r‚s‚u‚v"
	"‚w‚x‚P‚Q‚R‚S‚T‚U‚V‚W"
	"‚X‚O";

static	char	strlist[] = {
	'A','B','C','D','E','F','G','H','J','K',
	'L','M','N','P','Q','R','S','T','V','W',
	'X','Y','1','2','3','4','5','6','7','8',
	'9','0',
};
static	char	passwd_str[] = "01234567890123456789";

static	u32 mask1[] = { 0x0b626835, 0x0763337a, 0x0f4d6f49, 0x06343069 };
static	u32 mask2[] = { 0x03df4b61, 0x040e7254, 0x0a456b4f, 0x092e476f };
static	u32 mask3[] = { 0x08c26b8a, 0x0d73b9a1, 0x053ad652, 0x024df62e };

static	u32 mask_01[] = {
0x81021794, 0xF4967A99, 0xB403DEC6, 0x7FD52F56, 0x5086D67F, 0xD10A4924, 0x939AEA49, 0x24925292,
0x492494A4, 0x92492529, 0x79775FFF, 0x00942FFE, 0xB95EA2BC, 0xC3EB108E, 0xA377F5BF, 0xEFAC4D93,
0x6307CC7F, 0xBAE8741C, 0x46B6AF5C, 0x8F73890D, 0xFEA85A79, 0x5536EA9C, 0xC7F04155, 0x3A6E4D2D,
0xC567B808, 0x1AEE23E9, 0x7F8454BA, 0xA7586398, 0x69A0CCE8, 0xE7FF0027, 0xF90A7044, 0x62C46329,
};
static	u32 mask_02[] = {
0xE435FBDF, 0xE2384C76, 0xD702AE2A, 0x9E8D9B77, 0xED3B7F7A, 0x3DA8B55B, 0xD8A8F1CA, 0xB42CF9E0,
0x4FA87E8F, 0xCC992492, 0x53B51492, 0x4924A4B8, 0xFF00CE0F, 0x8ADD0B06, 0x8FA63E2B, 0x78205490,
0x22350C22, 0x3505266A, 0x33505A8C, 0xD4D5240B, 0x0FEB4FF4, 0x767F5FFE, 0xFAF5B816, 0x27D68FE8,
0xCCFEB8FF, 0x00A9B536, 0x5B32E2F9, 0xC35FEAC7, 0xD0B7E2D5, 0xD0AE7BEA, 0xCE95D9FD, 0x61F91740,
};

//---------------------------------------------------------------------------
// u16 *passwd				ƒpƒXƒ[ƒh‚Ì•¶Žšƒf[ƒ^i‘SŠp‚È‚Ì‚Åu16j‚P‚W•¶Žš
// u8  game_mode= 2bit		0:ƒIƒŠƒWƒiƒ‹@1:ƒXƒRƒAƒAƒ^ƒbƒN@2:‚½‚¢‚«‚ã‚¤
// u8  level	= 8bit		ƒNƒŠƒAƒXƒe[ƒW Lv.0`255  ‚Ü‚½‚Í  ƒQ[ƒ€ƒŒƒxƒ‹ 0:Easy  1:Normal  2:Hard
// u8  speed	= 2bit		ƒJƒvƒZƒ‹—Ž‰º‘¬“x 0:Low  1:Med  2:Hi
// u32 score	= 20bit		ƒXƒRƒA‚Ì•\Ž¦“à—e‚ð1/10‚µ‚½’l ( 0 ` 999999 )
// u16 time		= 16bit		ŽžŠÔ‚ð1/10•b’PˆÊ‚É‚µ‚½’li 0'00.0 ` 99'59.9 = 0 ` 59999 j
// u8  name[4]	= 32bit		–¼‘Oi‚S•¶Žšj
// ---------------------
//				Œv80bit + 8bit + 2bit = 90bit
//---------------------------------------------------------------------------
char *make_passwd(u16 *passwd, u8 game_mode, u8 level, u8 speed, u32 score, u16 time, u8 *name)
{
	char	*tmp;
	u16		*code, *pass;
	int		i, n;
	u32 	hi, lo, med, sum;
	u8		mask01, mask02;

	mask01 = framecont & STRMSK;
	mask02 = ( framecont >> STRBIT ) & STRMSK;

	// ƒ`ƒFƒbƒNƒTƒ€
	sum = (u32)game_mode + (u32)level + (u32)speed;
	sum += (u32)( score & 0x03ff );
	sum += (u32)( ( score >> 10 ) & 0x3ff );
	sum += (u32)( time & 0x00ff );
	sum += (u32)( ( time >> 8 ) & 0x0ff );
	sum += (u32)name[0] + (u32)name[1] + (u32)name[2] + (u32)name[3];
	sum &= 0x003ff;

	// hi = 30bit = 2 + 20 + 8
	hi = ( sum & 0x03 );
	hi <<= 20;
	hi |= score & 0x000fffff;
	hi <<= 8;
	hi |= ((u32)name[0]) & 0x00ff;

	// med = 30bit = 8 + 8 + 8 + 6
	med = ((u32)name[3]) & 0x00ff;
	med <<= 8;
	med |= ((u32)name[2]) & 0x00ff;
	med <<= 8;
	med |= ((u32)name[1]) & 0x00ff;
	med <<= 6;
	med |= ((u32)level) & 0x003f;

	// lo = 30bit = 8 + 2 + 2 + 2 + 16
	lo = ( sum >> 2 ) & 0x00ff;
	lo <<= 2;
	lo |= ((u32)speed) & 0x0003;
	lo <<= 2;
	lo |= ((u32)game_mode) & 0x0003;
	lo <<= 2;
	lo |= ((u32)level >> 6 ) & 0x03;
	lo <<= 16;
	lo |= ((u32)time) & 0x0000ffff;

	// ƒrƒbƒg”½“]
	hi ^= mask1[sum&0x03];
	med ^= mask2[sum&0x03];
	lo ^= mask3[sum&0x03];

	// “¯ˆêðŒ‚Å‚àˆá‚¤•¶Žš—ñ‚É‚·‚é
	hi ^= mask_01[mask01];
	med ^= mask_01[mask01];
	lo ^= mask_01[mask01];
	hi ^= mask_02[mask02];
	med ^= mask_02[mask02];
	lo ^= mask_02[mask02];

	code = (u16 *)strlist_full;
	pass = passwd;
	tmp = passwd_str;
	// maks01
	n = (int)mask01;
	*pass = code[n];
	pass++;
	*tmp = strlist[n];
	tmp++;
	// •¶Žš—ñì¬
	for ( i = 0; i < (30/STRBIT); i++ ) {
		n = ( hi & STRMSK );
		hi >>= STRBIT;
		*pass = code[n];
		pass++;
		*tmp = strlist[n];
		tmp++;
	}
	for ( i = 0; i < (30/STRBIT); i++ ) {
		n = ( med & STRMSK );
		med >>= STRBIT;
		*pass = code[n];
		pass++;
		*tmp = strlist[n];
		tmp++;
	}
	for ( i = 0; i < (30/STRBIT); i++ ) {
		n = ( lo & STRMSK );
		lo >>= STRBIT;
		*pass = code[n];
		pass++;
		*tmp = strlist[n];
		tmp++;
	}
	// maks02
	n = (int)mask02;
	*pass = code[n];
	pass++;
	*tmp = strlist[n];
	tmp++;

	return(passwd_str);
}
