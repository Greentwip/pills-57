
let cap_flg_on  =	1;

let cap_flg_off =   0;


let cap_size_10	=   10;	

let cap_size_8	=   8;


let cap_size_big    =   0;

let cap_size_small  =   1;



let cap_disp_flg	=   0;

let cap_down_flg	=   1;

let cap_condition_flg	=   2;

let cap_col_flg			=   3;

let cap_virus_flg		=   4;


let cap_map_flg     =   2;

let cap_throw_flg	=   2;


let dm_retire_flg		=   0;

let dm_game_over_flg	=   1;

let dm_training_flg		=   2;

let dm_static_cnd		=   3;


let cap_turn_r			=   1;

let cap_turn_l			=   -1;


let dm_mode_now			=   0;

let dm_mode_old			=   1;


enum DM_GAME_CONDITION{

	case dm_cnd_null = 0,

	dm_cnd_wait,			

	dm_cnd_init,			

	dm_cnd_stage_clear,		

	dm_cnd_game_over,		

//	dm_cnd_game_over_retry,	

	dm_cnd_win,				

	dm_cnd_win_retry,		

	dm_cnd_win_retry_sc,	

//	dm_cnd_win_any_key,		

//	dm_cnd_win_any_key_sc,	

	dm_cnd_lose,			

	dm_cnd_lose_retry,		

	dm_cnd_lose_retry_sc,	

//	dm_cnd_lose_sc,			

//	dm_cnd_lose_any_key,	

	dm_cnd_draw,			

	dm_cnd_draw_retry,		

//	dm_cnd_draw_any_key,	

	dm_cnd_pause,			

	dm_cnd_pause_re,		

	dm_cnd_pause_re_sc,		

//	dm_cnd_pause_tar,		

//	dm_cnd_pause_tar_re,	

//	dm_cnd_pause_tar_re_sc,	

	dm_cnd_retire,			

	dm_cnd_tr_chack,		

	dm_cnd_training,		

//	dm_cnd_manual,			

	dm_cnd_clear_wait,

	dm_cnd_clear_result,

	dm_cnd_gover_wait,

	dm_cnd_gover_result,

	dm_cnd_retire_wait,

	dm_cnd_retire_result,

	dm_cnd_debug_config,	

	dm_cnd_debug			

}


enum DM_GAME_MODE{

	case dm_mode_null,

	dm_mode_init,			

//	dm_mode_4p_story,		

	dm_mode_wait,			

	dm_mode_throw,			

	dm_mode_down,			

	dm_mode_down_wait,		

	dm_mode_erase_chack,	

	dm_mode_erase_anime,	

	dm_mode_ball_down,		

	dm_mode_cap_set,		

//	dm_mode_get_coin,		

	dm_mode_stage_clear,	

	dm_mode_game_over,		

//	dm_mode_game_over_retry,

//	dm_mode_use_coin,		

//	dm_mode_black_up,		

	dm_mode_bottom_up,		

	dm_mode_win,			

	dm_mode_win_retry,		

	dm_mode_lose,			

	dm_mode_lose_retry,		

	dm_mode_draw,			

	dm_mode_draw_retry,		

	dm_mode_tr_chaeck,		

	dm_mode_training,		

	dm_mode_tr_erase_chack,	

	dm_mode_tr_cap_set,		

	dm_mode_pause,			

	dm_mode_pause_retry,	

	dm_mode_no_action,		

	dm_mode_clear_wait,

	dm_mode_clear_result,

	dm_mode_gover_wait,

	dm_mode_gover_result,

	dm_mode_retire_wait,

	dm_mode_retire_result,

	dm_mode_debug_config,	

	dm_mode_debug			

}



enum DM_GAME_RET: Int{

	case dm_ret_game_end   =  -2,

	dm_ret_game_over  =  -1,

	dm_ret_null       =   0,

	dm_ret_next_stage =   1,

	dm_ret_retry      =   2,

	dm_ret_virus_wait =   3,

	dm_ret_pause      =   4,

	dm_ret_black_up   =   5,

	dm_ret_clear      =   6,

	dm_ret_tr_a       =   7,

	dm_ret_tr_b       =   8,

	dm_ret_replay     =   9,

	dm_ret_end        = 100

}


/*
#if 0 // �폜



#define dm_1_VS_3				0x0e	// ����p 1 �� 1 �̂Ƃ�

#define dm_2_VS_2				0x0c	// ����p 2 �� 2 �̂Ƃ�

#define dm_3_VS_1				0x08	// ����p 3 �� 1�̂Ƃ�



#endif
*/




let v_anime_def_speed		=   8;

let v_anime_def_speed_4p	=   12;		

let v_anime_speed			=   1;

let v_anime_speed_4p		=   4;



let dm_bound_wait			=   18;		

let dm_erace_speed_1		=   10;

let dm_erace_speed_2		=   27;

let dm_down_speed			=   14;

let dm_black_up_speed		=   6;



let dm_wold_x				=   118;

let dm_wold_y				=   46;



/*
#if 0 // �폜


#define dm_next_y				36		// �l�N�X�g�J�v�Z���̂x���W(1P�E2P)



#endif
*/



let dm_wold_x_vs_1p			= 28;

let dm_wold_x_vs_2p			= 212;



let dm_wold_x_4p			= 20;

let dm_wold_y_4p			= 46;


/*
#if 0 // �폜



#define dm_next_y_4p			44		// 4P�l�N�X�g�J�v�Z���̂x���W


#endif

*/


let dm_wold_x_4p_1p			= 20;		

let dm_wold_x_4p_2p			= 92;

let dm_wold_x_4p_3p			= 164;

let dm_wold_x_4p_4p			= 236;


enum CapselVirus {

	case capsel_u,		

	capsel_d,				

	capsel_l,				

	capsel_r,				

	capsel_b,				

	erase_cap_a,			

	erase_cap_b,			

	virus_a1,				//      	a1

	virus_a2,				// 			a2

	virus_b1,				// 			b1

	virus_b2,				// 			b2

	virus_c1,				// 			c1

	virus_c2,				// 			c2

	erase_virus_a,			

	erase_virus_b,			

	no_item,				

	virus_a3,				// 			a3

	virus_a4,				// 			a4

	virus_b3,				// 			b3

	virus_b4,				// 			b4

	virus_c3,				// 			c3

	virus_c4				// 			c4

}



enum Capsel{

	case capsel_red,	

	capsel_yellow,			

	capsel_blue,			

	capsel_b_red,			

	capsel_b_yellow,		

	capsel_b_blue

};


let GAME_MAP_W = 8;

let GAME_MAP_H = 16;



struct game_map{
	var pos_m_x: Int8?;
	var pos_m_y: Int8?;
	var capsel_m_g: Int8?;
	var capsel_m_p: Int8
	var capsel_m_flg: [Int8]?;	// [6]
}

struct game_cap{
	var pos_x: [Int8]?; // [2]
    var pos_y: [Int8]?;	// [2]
	var capsel_g: [Int8]?; // [2]
	var capsel_p: [Int8]?; // [2]
	var capsel_flg: [Int8]?; // [4]
}


struct game_a_cap{
	var pos_a_x: Int8?;
    var pos_a_y: Int8?;
	var capsel_a_p: Int8?;
	var capsel_a_flg: [Int8]?;	// [3]
}


let SPEED_LOW = 0;

let SPEED_MID = 1;

let SPEED_HIGH = 2;

let SPEED_MAX  = 3;



let PUF_PlayerMAN = 0;

let PUF_PlayerCPU = 1;


let PMD_CPU0 = 0;

let PMD_CPU1 = 1;

let PMD_CPU2 = 2;

let TEAM_MARIO  = 0;

let TEAM_ENEMY  = 1;

let TEAM_ENEMY2 = 2;

let TEAM_ENEMY3 = 3;

let DAMAGE_MAX = 0x10;

let FLASH_VIRUS_MAX = 9;

struct game_state{

	var game_score: UInt32?;

	var game_retry: UInt16?; 

	var map_x: Int16?;
    var map_y: Int16?;

	var map_item_size: Int8; 

	var mode_now: DM_GAME_MODE?; 

	var mode_old: DM_GAME_MODE?; 

	var cnd_now: DM_GAME_CONDITION?;      

	var cnd_old: DM_GAME_CONDITION?;      

	var cnd_training: DM_GAME_CONDITION?; 

	var cnd_static: DM_GAME_CONDITION?;   


	var virus_order_number: UInt8?;

	var virus_number: UInt8?;

	var virus_level: UInt8?; 

	var virus_anime: UInt8?; 

	var virus_anime_vec: Int8?;

	var virus_anime_count: UInt8?;

	var virus_anime_max: UInt8?;

	var virus_anime_spead: UInt8?;



	var cap_def_speed: UInt8;

	var cap_speed: UInt8?;

	var cap_count: UInt8?;

	var cap_speed_count: UInt8?;

	var cap_speed_vec: UInt8?;

	var cap_speed_max: UInt8?;

	var cap_magazine_cnt: UInt8?;

	var cap_magazine_save: UInt8?;

	var cap_move_se_flg: Int8?;



	var erase_anime: UInt8?;

	var erase_anime_count: UInt8?;

	var erase_virus_count: UInt8?;

	var erase_virus_count_old: UInt8?;



	var chain_count: UInt8?;

	var chain_line: UInt8?;

	var chain_line_max: UInt8?;

	var chain_color: [UInt8]?; //[4]

		// 0: 

		// 1: 

		// 2: 

		// 3: 


	var black_up_count: UInt8?;

	var warning_se_flag: Int?;


	var flg_retire: UInt8?;

	var flg_game_over: UInt8?;

	var flg_training: UInt8?;



	var player_no: UInt8?;

	var player_type: UInt8?;

	var think_type: UInt8?;

	var think_level: UInt8?;

	var team_no: UInt8?;

	var cap_attack_work: [[UInt16]]?; // [DAMAGE_MAX][2];


	var charNo: Int?;

	var anime: SAnimeState?;


	var flash_virus_pos: [[Int]]?; //[FLASH_VIRUS_MAX][3];

	var flash_virus_bak: [Int]?; //[FLASH_VIRUS_MAX];

	var flash_virus_count: Int?;


	var bottom_up_scroll: Int?;


	var game_level: Int?;



	var total_erase_count: Int?;

	var total_chain_count: Int?;



	var now_cap: game_cap?;

	var next_cap: game_cap?;



	var ai: AIWORK?;

	var pn: UInt8?;

	var gs: UInt8?;

	var lv: UInt8?;

	var vs: UInt8?;


	var blk: [[AIBLK]]?; // [MFieldY+1][MFieldX]; 

	var cap: AICAPS?;

}



struct virus_map{

	var virus_type: Int8?;  

	var x_pos: UInt8?;
    var y_pos: UInt8?;

}

