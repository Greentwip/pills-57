#ifndef		__game_etc__
#define		__game_etc__

#define	PAUSE_RET_CONTINUE		0	// つづける
#define	PAUSE_RET_RETRY			1	// もういちど
#define	PAUSE_RET_EXIT			2	// おわる

#define	CONTINUE_RET_REPLAY		0	// リプレイ
#define	CONTINUE_RET_CONTINUE	1	// つづける
#define	CONTINUE_RET_RETRY		2	// もういちど
#define	CONTINUE_RET_EXIT		3	// おわる

extern	void	initEtcWork(u32 address, int bin_count);
extern	void	init_pause_disp(void);

extern	void	disp_logo_setup(Gfx **glp);
extern	void	disp_win_logo(Gfx **glp, int player);
extern	void	disp_lose_logo(Gfx **glp, int player);
extern	void	disp_draw_logo(Gfx **glp, int player);
extern	int		disp_pause_logo(Gfx **glp, int player, int score, int cursor, int type);
extern	int		disp_continue_logo(Gfx **glp, int player, int cursor, int type);
extern	int		disp_continue_logo_score(Gfx **glp, int player, int cursor, int type);
extern	void	disp_gameover_logo(Gfx **glp, int player);
extern	void	disp_timeover_logo(Gfx **glp, int player);
extern	void	disp_retire_logo(Gfx **glp, int player);
extern	int		disp_count_logo(Gfx **glp, int player, int count);
extern	void	disp_clear_logo(Gfx **glp, int player, int flg);
extern	void	disp_allclear_logo(Gfx **glp, int player, int flg);
extern	void	disp_timestop_logo(Gfx **glp, int player);

extern	void	add_attack_effect(int player, int sx, int sy, int ex, int ey);
extern	void	disp_attack_effect(Gfx **glp);

#endif
