//HEADER_GOES_HERE
#ifndef __PATH_H__
#define __PATH_H__

extern PATHNODE path_nodes[MAXPATHNODES];
extern int gdwCurPathStep;
extern int gdwCurNodes;
extern int pnode_vals[MAX_PATH_LENGTH];
extern PATHNODE *pnode_ptr;
extern PATHNODE *pnode_tblptr[MAXPATHNODES];
extern PATHNODE *path_2_nodes;
extern PATHNODE path_unusednodes[MAXPATHNODES];

int FindPath(BOOL (*PosOk)(int, int, int), int PosOkArg, int sx, int sy, int dx, int dy, char *path);
int path_get_h_cost(int sx, int sy, int dx, int dy);
int path_check_equal(PATHNODE *pPath, int dx, int dy);
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

/* data */
extern char path_directions[9];

#endif /* __PATH_H__ */
