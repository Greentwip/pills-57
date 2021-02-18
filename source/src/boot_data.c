
#include <ultra64.h>
#include "boot_data.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////////////
// main_story.cÇ≈égópÇ∑ÇÈÇqÇnÇlÉAÉhÉåÉX

extern	u8	_coffee01SegmentRomStart[], _coffee01SegmentRomEnd[];
extern	u8	_menu_bgSegmentRomStart[], _menu_bgSegmentRomEnd[];
extern	u8	_menu_bg2SegmentRomStart[], _menu_bg2SegmentRomEnd[];
extern	u8	_wakuSegmentRomStart[], _wakuSegmentRomEnd[];
extern	u8	_waku2SegmentRomStart[], _waku2SegmentRomEnd[];
extern	u8	_title_allSegmentRomStart[], _title_allSegmentRomEnd[];
extern	u8	_title_bmpSegmentRomStart[], _title_bmpSegmentRomEnd[];
u8	*storyRomData[][2] = {
	{ _coffee01SegmentRomStart, _coffee01SegmentRomEnd },
	{ _menu_bgSegmentRomStart, _menu_bgSegmentRomEnd },
	{ _menu_bg2SegmentRomStart, _menu_bg2SegmentRomEnd },
	{ _wakuSegmentRomStart, _wakuSegmentRomEnd },
	{ _waku2SegmentRomStart, _waku2SegmentRomEnd },
	{ _title_allSegmentRomStart, _title_allSegmentRomEnd },
	{ _title_bmpSegmentRomStart, _title_bmpSegmentRomEnd },
};
extern	u8	_story_bg01SegmentRomStart[], _story_bg01SegmentRomEnd[];	// ï™Ç©ÇÍìπ
extern	u8	_story_bg02SegmentRomStart[], _story_bg02SegmentRomEnd[];	// ëêÇﬁÇÁ
extern	u8	_story_bg03SegmentRomStart[], _story_bg03SegmentRomEnd[];	// ë∫
extern	u8	_story_bg04SegmentRomStart[], _story_bg04SegmentRomEnd[];	// ä‚éR
extern	u8	_story_bg05SegmentRomStart[], _story_bg05SegmentRomEnd[];	// â_
extern	u8	_story_bg07SegmentRomStart[], _story_bg07SegmentRomEnd[];	// äCä›
extern	u8	_story_bg08SegmentRomStart[], _story_bg08SegmentRomEnd[];	// äCíÜ
extern	u8	_story_bg09SegmentRomStart[], _story_bg09SegmentRomEnd[];	// êX
extern	u8	_story_bg10SegmentRomStart[], _story_bg10SegmentRomEnd[];	// ì¸ÇËå˚
extern	u8	_story_bg11SegmentRomStart[], _story_bg11SegmentRomEnd[];	// ê_ìa
u8	*bgRomData[][2] = {
	{ NULL, NULL },
	{ _story_bg03SegmentRomStart, _story_bg03SegmentRomEnd },
	{ _story_bg01SegmentRomStart, _story_bg01SegmentRomEnd },
	{ _story_bg09SegmentRomStart, _story_bg09SegmentRomEnd },
	{ _story_bg02SegmentRomStart, _story_bg02SegmentRomEnd },
	{ _story_bg07SegmentRomStart, _story_bg07SegmentRomEnd },
	{ _story_bg05SegmentRomStart, _story_bg05SegmentRomEnd },
	{ _story_bg10SegmentRomStart, _story_bg10SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ NULL, NULL },
	{ NULL, NULL },
	{ _story_bg03SegmentRomStart, _story_bg03SegmentRomEnd },
	{ _story_bg01SegmentRomStart, _story_bg01SegmentRomEnd },
	{ _story_bg09SegmentRomStart, _story_bg09SegmentRomEnd },
	{ _story_bg02SegmentRomStart, _story_bg02SegmentRomEnd },
	{ _story_bg08SegmentRomStart, _story_bg08SegmentRomEnd },
	{ _story_bg04SegmentRomStart, _story_bg04SegmentRomEnd },
	{ _story_bg10SegmentRomStart, _story_bg10SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ _story_bg11SegmentRomStart, _story_bg11SegmentRomEnd },
	{ NULL, NULL },
};

//////////////////////////////////////////////////////////////////////////////

#define SEG_IMPL(seg) _##seg##SegmentRomStart, _##seg##SegmentRomEnd,

// sound
#include "sound/sound_data.h"

// character
#include "texture/character/char_data.h"

// game
MSegDecl(game_al)
MSegDecl(game_p1)
MSegDecl(game_p2)
MSegDecl(game_p4)
MSegDecl(game_ls)
MSegDecl(game_item)
MSegDecl(game_etc)

// menu
MSegDecl(menu_char)
MSegDecl(menu_common)
MSegDecl(menu_level)
MSegDecl(menu_main)
MSegDecl(menu_name)
MSegDecl(menu_p2)
MSegDecl(menu_p4)
MSegDecl(menu_rank)
MSegDecl(menu_setup)
MSegDecl(menu_story)
MSegDecl(menu_cont)
MSegDecl(menu_kasa)

// tutorial
MSegDecl(tutorial_data)

void *_romDataTbl[][2] = {
	// sound
	SEG_IMPL(wbk_drmario)
	SEG_IMPL(pbk_drmario)
	SEG_IMPL(fbk_drmario)
	SEG_IMPL(seq_chill)
	SEG_IMPL(seq_fever)
	SEG_IMPL(seq_game_c)
	SEG_IMPL(seq_game_d)
	SEG_IMPL(seq_game_e)
	SEG_IMPL(seq_chillF)
	SEG_IMPL(seq_feverF)
	SEG_IMPL(seq_game_cF)
	SEG_IMPL(seq_game_dF)
	SEG_IMPL(seq_game_eF)
	SEG_IMPL(seq_opening)
	SEG_IMPL(seq_title)
	SEG_IMPL(seq_menu)
	SEG_IMPL(seq_ending)
	SEG_IMPL(seq_staff)
	SEG_IMPL(seq_end_a)
	SEG_IMPL(seq_end_b)
	SEG_IMPL(seq_end_b_s)
	SEG_IMPL(seq_end_c)
	SEG_IMPL(seq_end_c_s)
	SEG_IMPL(seq_story_a)
	SEG_IMPL(seq_story_b)
	SEG_IMPL(seq_story_c)
	SEG_IMPL(seq_coffee)

	// character
	SEG_IMPL(anime_a)
	SEG_IMPL(anime_b)
	SEG_IMPL(anime_c)
	SEG_IMPL(anime_d)
	SEG_IMPL(anime_e)
	SEG_IMPL(anime_f)
	SEG_IMPL(anime_g)
	SEG_IMPL(anime_h)
	SEG_IMPL(anime_i)
	SEG_IMPL(anime_j)
	SEG_IMPL(anime_k)
	SEG_IMPL(anime_l)
	SEG_IMPL(anime_m)
	SEG_IMPL(anime_n)
	SEG_IMPL(anime_o)
	SEG_IMPL(anime_mario)
	SEG_IMPL(anime_virus_b)
	SEG_IMPL(anime_virus_r)
	SEG_IMPL(anime_virus_y)
	SEG_IMPL(anime_smog)

	// game
	SEG_IMPL(game_al)
	SEG_IMPL(game_p1)
	SEG_IMPL(game_p2)
	SEG_IMPL(game_p4)
	SEG_IMPL(game_ls)
	SEG_IMPL(game_item)
	SEG_IMPL(game_etc)

	// menu
	SEG_IMPL(menu_char)
	SEG_IMPL(menu_common)
	SEG_IMPL(menu_level)
	SEG_IMPL(menu_main)
	SEG_IMPL(menu_name)
	SEG_IMPL(menu_p2)
	SEG_IMPL(menu_p4)
	SEG_IMPL(menu_rank)
	SEG_IMPL(menu_setup)
	SEG_IMPL(menu_story)
	SEG_IMPL(menu_cont)
	SEG_IMPL(menu_kasa)

	// tutorial
	SEG_IMPL(tutorial_data)
};

//////////////////////////////////////////////////////////////////////////////
