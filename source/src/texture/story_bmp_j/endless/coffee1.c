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

extern	SCENE_DATA	endless_back_scene1;
extern	SCENE_DATA	endless_back_scene2;
extern	SCENE_DATA	endless_a_scene;
extern	SCENE_DATA	endless_b_scene;
extern	SCENE_DATA	endless_c_scene;


extern	SCENE_DATA	endless_cb_hard_scene1;
extern	SCENE_DATA	endless_cb_shard_scene1;


SCENE_DATA	*scene_dat[] = {
	&endless_back_scene1,
	&endless_back_scene2,
	&endless_a_scene,
	&endless_b_scene,
	&endless_c_scene,
	&endless_cb_hard_scene1,
	&endless_cb_shard_scene1,
};

Gfx all_texture_dummy_dl[] = {
	gsSPEndDisplayList(),
};
#include	"all_texture.h"
#include	"all_model.h"

#include	"endless_back.ani"
#include	"endless_a.ani"
#include	"endless_b.ani"
#include	"endless_c.ani"
#include	"endless_cb.ani"
