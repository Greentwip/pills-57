#include <ultra64.h>

typedef struct {
	Gfx		*dl;
	int		parent, key_count, key_index;
	s16		px, py, pz;
} SCENE_OBJ;
typedef struct {
	int		key_no, next;
	s16		lx, ly, lz;
	s16		rx, ry, rz;
	s16		sx, sy, sz;
} SCENE_KEY;
typedef struct {
	int			first_frame, last_frame, frame_rate;
	int			obj_count;
	SCENE_OBJ	*obj;
	SCENE_KEY	*key;
} SCENE_DATA;


extern	SCENE_DATA	retire_scene;
extern	SCENE_DATA	pause_scene;
extern	SCENE_DATA	gameover_scene;
extern	SCENE_DATA	timeover_scene;
extern	SCENE_DATA	win_2P_scene;
extern	SCENE_DATA	win_4P_scene;
extern	SCENE_DATA	lose_2P_scene;
extern	SCENE_DATA	lose_4P_scene;
extern	SCENE_DATA	draw_2P_scene;
extern	SCENE_DATA	draw_4P_scene;
extern	SCENE_DATA	ready_3sec_256_scene;
extern	SCENE_DATA	stageclear_scene;
extern	SCENE_DATA	trynext_scene;
extern	SCENE_DATA	allclear_scene;

SCENE_DATA	*scene_dat[] = {
	&win_2P_scene,
	&win_4P_scene,
	&lose_2P_scene,
	&lose_4P_scene,
	&draw_2P_scene,
	&draw_4P_scene,
	&retire_scene,
	&pause_scene,
	&gameover_scene,
	&ready_3sec_256_scene,
	&stageclear_scene,
	&trynext_scene,
	&timeover_scene,
	&allclear_scene,
};

#include	"lose_4P.ani"
#include	"lose_2P.ani"
#include	"draw_4P.ani"
#include	"draw_2P.ani"
#include	"win_2P.ani"
#include	"win_4P.ani"
#include	"retire.ani"
#include	"pause.ani"
#include	"ready_3sec_256.ani"
#include	"stageclear.ani"
#include	"trynext.ani"
#include	"gameover.ani"
#include	"timeover.ani"
#include	"allclear.ani"
