#ifndef		__main_story__
#define		__main_story__


// スタッフロールテキスト
extern	char st_staffroll_txt[];


// カーテン表示処理
extern	void curtain_proc(Gfx **glp, int count);
// curtain_proc の引数 count で
#define		CURTAIN_UP		0		// カーテンが上がった状態
#define		CURTAIN_DOWN	30		// カーテンが下がった状態
#define		ZOOM_EFF_0		0		// カーテンが下がった状態
#define		ZOOM_EFF_MAX	90		// カーテンが下がった状態


// ゲーム中背景    引数 stage には STORY_?_ST? を指定
extern	void *story_bg_init(int stage, u32 buffAddr);
extern	void story_bg_proc(Gfx **glp);


// エンディングメッセージ
extern	char	*EndingLastMessage;

// ストーリー表示の処理番号（ステージ）
extern int	story_proc_no;
// story_proc_noに設定する値
#define	STORY_M_OPEN	0
#define	STORY_M_ST1		1
#define	STORY_M_ST2		2
#define	STORY_M_ST3		3
#define	STORY_M_ST4		4
#define	STORY_M_ST5		5
#define	STORY_M_ST6		6
#define	STORY_M_ST7		7
#define	STORY_M_ST8		8
#define	STORY_M_ST9		9
#define	STORY_M_END		10
#define	STORY_M_END2	11
#define	STORY_W_OPEN	12
#define	STORY_W_ST1		13
#define	STORY_W_ST2		14
#define	STORY_W_ST3		15
#define	STORY_W_ST4		16
#define	STORY_W_ST5		17
#define	STORY_W_ST6		18
#define	STORY_W_ST7		19
#define	STORY_W_ST8		20
#define	STORY_W_ST9		21
#define	STORY_W_END		22
#define	STORY_W_END2	23


// GBIデータ
typedef struct {
	int	type;
	int	x, y;
	u8	*lut_addr;
	u8	*tex_addr;
} GBI_DATA;



extern	void	get_gbi_stat(GBI_DATA *gbi, u32 gbi_addr);



// タイトル画面の初期化
//		u32 buffer		グラフィックデータの読み込みアドレス
//		int flg			TRUE=初めから表示	 FALSE=いきなり表示（メインメニューから戻った場合）
extern	u32	init_title(u32 buffer, int flg);

// タイトル画面の表示
//		int flg			昼、夕フラグ  昼:0、夕:1
extern	int demo_title(Gfx **glp, int flg21);

// メニューＢＧの初期化
//		u32 buffer		グラフィックデータの読み込みアドレス
//		int flg			背景の昼、夕フラグ  昼:0、夕:1
u32	init_menu_bg(u32 buffer, int flg);

// メニューＢＧの表示
//		int	ofsx,ofsy	表示位置オフセット
void draw_menu_bg(Gfx **glp, int ofsx, int ofsy);


// コーヒーブレイクの初期化
//		u32 buffer		グラフィックデータの読み込みアドレス
//		int type		デモの種類	0:EASY 1:MED 2:HARD 3:SHARD
extern	u32		init_coffee_break(u32 buffer, int type);

// コーヒーブレイクの内部カウンター初期化
extern	void	init_coffee_break_cnt(void);

// コーヒーブレイクの表示
// tpye      0:Low, 1:Med, 2:Hi
// mode      FALSE=Lv21  TRUE=Lv24
// disp_flg  FALSE=メッセージＯｆｆ  TRUE=メッセージＯｎ
void draw_coffee_break(Gfx **glp, int type, int mode, int disp_flg);

//-------------------------------------------------------------------------
// ＮｅｗＦａｄｅ
// input:
//	int	count		ZOOM_EFF_0 --> ZOOM_EFF_MAX
extern	void	story_zoomfade(Gfx **glp, int count);


//-------------------------------------------------------------------------
// 星型スポットライト処理
// input:
//		x     : スポット中心　Ｘ
//		y     : スポット中心　Ｙ
//		count : 0 真っ黒 ～ 255
extern	void	star_spot(Gfx **glp, int x, int y, int count);


//-------------------------------------------------------------------------
// スタッフロール後のメッセージ時の背景
extern	void	draw_ending_mess_bg(Gfx **glp);


#endif
