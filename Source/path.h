/**
 * @file path.h
 *
 * Interface of the path finding algorithms.
 */
#ifndef __PATH_H__
#define __PATH_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PATHNODE {
	Uint8 f;
	Uint8 h;
	Uint8 g;
	int x;
	int y;
	struct PATHNODE *Parent;
	struct PATHNODE *Child[8];
	struct PATHNODE *NextNode;
} PATHNODE;

int FindPath(BOOL (*PosOk)(int, int, int), int PosOkArg, int sx, int sy, int dx, int dy, Sint8 path[MAX_PATH_LENGTH]);
int path_get_h_cost(int sx, int sy, int dx, int dy);
PATHNODE *GetNextPath();
BOOL path_solid_pieces(PATHNODE *pPath, int dx, int dy);
BOOL path_get_path(BOOL (*PosOk)(int, int, int), int PosOkArg, PATHNODE *pPath, int x, int y);
BOOL path_parent_path(PATHNODE *pPath, int dx, int dy, int sx, int sy);
PATHNODE *path_get_node1(int dx, int dy);
PATHNODE *path_get_node2(int dx, int dy);
void path_next_node(PATHNODE *pPath);
void path_set_coords(PATHNODE *pPath);
void path_push_active_step(PATHNODE *pPath);
PATHNODE *path_pop_active_step();
PATHNODE *path_new_step();

/* rdata */

extern const char pathxdir[8];
extern const char pathydir[8];
#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __PATH_H__ */
