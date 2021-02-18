#include <ultra64.h>
#include "local.h"

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

extern	SCENE_DATA	title_all_scene;
extern	SCENE_DATA	stage_house_a_scene;
extern	SCENE_DATA	stage_house_b_scene;
extern	SCENE_DATA	stage_mountain_a1_scene;
extern	SCENE_DATA	stage_mountain_a2_scene;
extern	SCENE_DATA	stage_forest_a1_scene;
extern	SCENE_DATA	stage_forest_a2_scene;
extern	SCENE_DATA	stage_forest_b1_scene;
extern	SCENE_DATA	stage_forest_b2_scene;
extern	SCENE_DATA	stage_beach_a1_scene;
extern	SCENE_DATA	stage_beach_a2_scene;
extern	SCENE_DATA	stage_mountain_b_scene;
extern	SCENE_DATA	stage_sky_scene;
extern	SCENE_DATA	stage_mountain_c_scene;
extern	SCENE_DATA	stage_mountain_d1_scene;
extern	SCENE_DATA	stage_mountain_d2_scene;
extern	SCENE_DATA	stage_castle_a1_scene;
extern	SCENE_DATA	stage_castle_a2_scene;
extern	SCENE_DATA	stage_castle_a3_scene;
extern	SCENE_DATA	stage_castle_b1_scene;
extern	SCENE_DATA	stage_castle_b2_scene;
extern	SCENE_DATA	stage_castle_c1_scene;
extern	SCENE_DATA	stage_castle_c2_scene;
extern	SCENE_DATA	END_mario_a_scene;
extern	SCENE_DATA	END_mario_b_scene;
extern	SCENE_DATA	END_mario_c_scene;
extern	SCENE_DATA	END_wario_a_scene;
extern	SCENE_DATA	END_wario_b_scene;
extern	SCENE_DATA	END_wario_c_scene;
extern	SCENE_DATA	stage_prologue_a1_scene;
extern	SCENE_DATA	stage_prologue_a2_scene;
extern	SCENE_DATA	stage_prologue_b_scene;
extern	SCENE_DATA	stage_prologue_c_scene;
extern	SCENE_DATA	title_all_eve_scene;
extern	SCENE_DATA	stage_episode_scene;

SCENE_DATA	*scene_dat[] = {
	&title_all_scene,
	&stage_house_a_scene,
	&stage_house_b_scene,
	&stage_mountain_a1_scene,
	&stage_mountain_a2_scene,
	&stage_forest_a1_scene,
	&stage_forest_a2_scene,
	&stage_forest_b1_scene,
	&stage_forest_b2_scene,
	&stage_beach_a1_scene,
	&stage_beach_a2_scene,
	&stage_mountain_b_scene,
	&stage_sky_scene,
	&stage_mountain_c_scene,
	&stage_castle_a1_scene,
	&stage_castle_a2_scene,
	&stage_castle_b1_scene,
	&stage_castle_b2_scene,
	&stage_castle_c1_scene,
	&stage_castle_c2_scene,
	&END_mario_a_scene,
	&END_mario_b_scene,
	&END_mario_c_scene,
	&END_wario_a_scene,
	&END_wario_b_scene,
	&END_wario_c_scene,
	&stage_prologue_a1_scene,
	&stage_prologue_a2_scene,
	&stage_prologue_b_scene,
	&stage_prologue_c_scene,
	&stage_mountain_d1_scene,
	&stage_mountain_d2_scene,
	&stage_castle_a3_scene,
	&title_all_eve_scene,
	&stage_episode_scene,
};

Gfx all_texture_dummy_dl[] = {
	gsSPEndDisplayList(),
};
#include	"all_texture.h"
#include	"all_model.h"

#include	"title_all.ani"
#include	"stage_house_a.ani"
#include	"stage_house_b.ani"
#include	"stage_mountain_a1.ani"
#include	"stage_mountain_a2.ani"
#include	"stage_forest_a1.ani"
#include	"stage_forest_a2.ani"
#include	"stage_forest_b1.ani"
#include	"stage_forest_b2.ani"
#include	"stage_beach_a1.ani"
#include	"stage_beach_a2.ani"
#include	"stage_mountain_b.ani"
#include	"stage_sky.ani"
#include	"stage_mountain_c.ani"
#include	"stage_mountain_d1.ani"
#include	"stage_mountain_d2.ani"
#include	"stage_castle_a1.ani"
#include	"stage_castle_a2.ani"
#include	"stage_castle_a3.ani"
#include	"stage_castle_b1.ani"
#include	"stage_castle_b2.ani"
#include	"stage_castle_c1.ani"
#include	"stage_castle_c2.ani"
#include	"END_mario_a.ani"
#include	"END_mario_b.ani"
#include	"END_mario_c.ani"
#include	"END_wario_a.ani"
#include	"END_wario_b.ani"
#include	"END_wario_c.ani"
#include	"stage_prologue_a1.ani"
#include	"stage_prologue_a2.ani"
#include	"stage_prologue_b.ani"
#include	"stage_prologue_c.ani"
#include	"title_all_eve.ani"
#include	"stage_episode.ani"
