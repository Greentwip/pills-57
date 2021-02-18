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


#include	"menu_back_evening.ani"
