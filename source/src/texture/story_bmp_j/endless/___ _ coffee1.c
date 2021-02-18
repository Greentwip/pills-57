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

extern	SCENE_DATA	endless_cb_easy_scene;
extern	SCENE_DATA	endless_cb_med_scene;
extern	SCENE_DATA	endless_cb_hard_scene;

extern	SCENE_DATA	endless_cb_easy_scene0;
extern	SCENE_DATA	endless_cb_med_scene0;
extern	SCENE_DATA	endless_cb_hard_scene0;

extern	SCENE_DATA	endless_cb_easy_scene1;
extern	SCENE_DATA	endless_cb_med_scene1;
extern	SCENE_DATA	endless_cb_hard_scene1;
extern	SCENE_DATA	endless_cb_shard_scene1;


SCENE_DATA	*scene_dat[] = {
	&endless_cb_easy_scene,
	&endless_cb_easy_scene0,
	&endless_cb_med_scene,
	&endless_cb_med_scene0,
	&endless_cb_hard_scene,
	&endless_cb_hard_scene0,

	&endless_cb_hard_scene1,
	&endless_cb_shard_scene1,
};

Gfx all_texture_dummy_dl[] = {
	gsSPEndDisplayList(),
};
#include	"all_texture.h"
#include	"all_model.h"

#include	"endless_cb_easy.ani"
#include	"endless_cb_med.ani"
#include	"endless_cb_hard.ani"
#include	"endless_cb.ani"
