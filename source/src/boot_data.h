
#if !defined(_BOOT_DATA_H_)
#define _BOOT_DATA_H_

extern	u8	*bgRomData[][2];
extern	u8	*storyRomData[][2];

extern void *_romDataTbl[][2];

enum {
	// sound
	_wbk_drmario,
	_pbk_drmario,
	_fbk_drmario,
	_seq_chill,
	_seq_fever,
	_seq_game_c,
	_seq_game_d,
	_seq_game_e,
	_seq_chillF,
	_seq_feverF,
	_seq_game_cF,
	_seq_game_dF,
	_seq_game_eF,
	_seq_opening,
	_seq_title,
	_seq_menu,
	_seq_ending,
	_seq_staff,
	_seq_end_a,
	_seq_end_b,
	_seq_end_b_s,
	_seq_end_c,
	_seq_end_c_s,
	_seq_story_a,
	_seq_story_b,
	_seq_story_c,
	_seq_coffee,

	// character
	_anime_a,
	_anime_b,
	_anime_c,
	_anime_d,
	_anime_e,
	_anime_f,
	_anime_g,
	_anime_h,
	_anime_i,
	_anime_j,
	_anime_k,
	_anime_l,
	_anime_m,
	_anime_n,
	_anime_o,
	_anime_mario,
	_anime_virus_b,
	_anime_virus_r,
	_anime_virus_y,
	_anime_smog,

	// game
	_game_al,
	_game_p1,
	_game_p2,
	_game_p4,
	_game_ls,
	_game_item,
	_game_etc,

	// menu
	_menu_char,
	_menu_common,
	_menu_level,
	_menu_main,
	_menu_name,
	_menu_p2,
	_menu_p4,
	_menu_rank,
	_menu_setup,
	_menu_story,
	_menu_cont,
	_menu_kasa,

	// tutorial
	_tutorial_data,
};

#endif // _BOOT_DATA_H_
