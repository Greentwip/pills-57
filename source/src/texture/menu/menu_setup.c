#include <ultra64.h>
#include "tex_func.h"
#include "local.h"

static STexInfo  texTbl[];
static STexInfo *texTblPtr = texTbl;
static int       texTblSize[];
static int      *texTblSizePtr = texTblSize;

#define USE_TEX_SIZE
#define USE_TEX_ADDR

#define NON_PALETTE
#include "setup/alpha/answer_no_alpha.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/alpha/gamelv_answer_alpha.txt"
#include "../../i10n/assets/zh/menu/setup/alpha/gamelv_answer_b_alpha.txt"
#include "../../i10n/assets/zh/menu/setup/alpha/speed_answer_a_alpha.txt"
#include "../../i10n/assets/zh/menu/setup/alpha/speed_answer_b_alpha.txt"
#include "../../i10n/assets/zh/menu/setup/alpha/finger_sub_alpha.txt"
#else
#include "setup/alpha/gamelv_answer_alpha.txt"
#include "setup/alpha/gamelv_answer_b_alpha.txt"
#include "setup/alpha/speed_answer_a_alpha.txt"
#include "setup/alpha/speed_answer_b_alpha.txt"
#include "setup/alpha/finger_sub_alpha.txt"
#endif
#include "setup/alpha/ok_alpha.txt"

#include "setup/color/answer_no.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/color/finger_sub.txt"
#include "../../i10n/assets/zh/menu/setup/color/gamelv_answer.txt"
#include "../../i10n/assets/zh/menu/setup/color/gamelv_answer_b.txt"
#include "../../i10n/assets/zh/menu/setup/color/speed_answer_a.txt"
#include "../../i10n/assets/zh/menu/setup/color/speed_answer_b.txt"
#else
#include "setup/color/finger_sub.txt"
#include "setup/color/gamelv_answer.txt"
#include "setup/color/gamelv_answer_b.txt"
#include "setup/color/speed_answer_a.txt"
#include "setup/color/speed_answer_b.txt"
#endif
#include "setup/color/ok.txt"
#undef NON_PALETTE

#include "setup/normal/all_bg.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/normal/ohanashi_panel_shard.txt"
#else
#include "setup/normal/ohanashi_panel_shard.txt"
#endif

#include "setup/normal2/bg_cursor.txt"
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/normal2/player_panel.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/setup_sub_panel.txt"
#else
#include "setup/normal2/player_panel.txt"
#include "setup/normal2/setup_sub_panel.txt"
#endif

#if LOCAL==JAPAN
#include "setup/normal2/r_command.txt"
#elif LOCAL==AMERICA
#include "setup/normal/r_command_am.txt"
#elif LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/normal/r_command_am.txt"
#endif

#define NON_PALETTE
#if LOCAL==CHINA
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_gamelv_easy.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_gamelv_normal.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_gamelv_hard.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_gamelv_shard.txt"

#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_speed_low.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_speed_med.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_speed_hi.txt"

#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_music_fever.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_music_chill.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_music_cube.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_music_queque.txt"
#include "../../i10n/assets/zh/menu/setup/normal2/new_cursor_music_off.txt"
#else
#include "setup/normal2/new_cursor_gamelv_easy.txt"
#include "setup/normal2/new_cursor_gamelv_normal.txt"
#include "setup/normal2/new_cursor_gamelv_hard.txt"
#include "setup/normal2/new_cursor_gamelv_shard.txt"

#include "setup/normal2/new_cursor_speed_low.txt"
#include "setup/normal2/new_cursor_speed_med.txt"
#include "setup/normal2/new_cursor_speed_hi.txt"

#include "setup/normal2/new_cursor_music_fever.txt"
#include "setup/normal2/new_cursor_music_chill.txt"
#include "setup/normal2/new_cursor_music_cube.txt"
#include "setup/normal2/new_cursor_music_queque.txt"
#include "setup/normal2/new_cursor_music_off.txt"
#endif
#undef NON_PALETTE

static STexInfo texTbl[] = {
	{ answer_no_alpha_tex_addr, answer_no_alpha_tex_size },
	{ finger_sub_alpha_tex_addr, finger_sub_alpha_tex_size },
	{ gamelv_answer_alpha_tex_addr, gamelv_answer_alpha_tex_size },
	{ gamelv_answer_b_alpha_tex_addr, gamelv_answer_b_alpha_tex_size },
	{ ok_alpha_tex_addr, ok_alpha_tex_size },
	{ speed_answer_a_alpha_tex_addr, speed_answer_a_alpha_tex_size },
	{ speed_answer_b_alpha_tex_addr, speed_answer_b_alpha_tex_size },

	{ answer_no_tex_addr, answer_no_tex_size },
	{ finger_sub_tex_addr, finger_sub_tex_size },
	{ gamelv_answer_tex_addr, gamelv_answer_tex_size },
	{ gamelv_answer_b_tex_addr, gamelv_answer_b_tex_size },
	{ ok_tex_addr, ok_tex_size },
	{ speed_answer_a_tex_addr, speed_answer_a_tex_size },
	{ speed_answer_b_tex_addr, speed_answer_b_tex_size },

	{ all_bg_tex_addr, all_bg_tex_size },
	{ ohanashi_panel_shard_tex_addr, ohanashi_panel_shard_tex_size },

	{ bg_cursor_tex_addr, bg_cursor_tex_size },
	{ player_panel_tex_addr, player_panel_tex_size },
	{ setup_sub_panel_tex_addr, setup_sub_panel_tex_size },

#if LOCAL==JAPAN
	{ r_command_tex_addr, r_command_tex_size },
#elif LOCAL==AMERICA
	{ r_command_am_tex_addr, r_command_am_tex_size },
#elif LOCAL==CHINA
	{ r_command_am_tex_addr, r_command_am_tex_size },
#endif

	{ new_cursor_gamelv_easy_tex_addr, new_cursor_gamelv_easy_tex_size },
	{ new_cursor_gamelv_normal_tex_addr, new_cursor_gamelv_normal_tex_size },
	{ new_cursor_gamelv_hard_tex_addr, new_cursor_gamelv_hard_tex_size },
	{ new_cursor_gamelv_shard_tex_addr, new_cursor_gamelv_shard_tex_size },

	{ new_cursor_speed_low_tex_addr, new_cursor_speed_low_tex_size },
	{ new_cursor_speed_med_tex_addr, new_cursor_speed_med_tex_size },
	{ new_cursor_speed_hi_tex_addr, new_cursor_speed_hi_tex_size },

	{ new_cursor_music_fever_tex_addr, new_cursor_music_fever_tex_size },
	{ new_cursor_music_chill_tex_addr, new_cursor_music_chill_tex_size },
	{ new_cursor_music_cube_tex_addr, new_cursor_music_cube_tex_size },
	{ new_cursor_music_queque_tex_addr, new_cursor_music_queque_tex_size },
	{ new_cursor_music_off_tex_addr, new_cursor_music_off_tex_size },
};
static int texTblSize[] = { sizeof(texTbl) / sizeof(*texTbl) };
