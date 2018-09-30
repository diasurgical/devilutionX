//HEADER_GOES_HERE

#include "../types.h"

#ifndef NO_GLOBALS
// preallocated nodes, search is terminated after 300 nodes are visited
PATHNODE path_nodes[300];
// size of the pnode_tblptr stack
int gdwCurPathStep;
// the number of in-use nodes in path_nodes
int gdwCurNodes;
/* for reconstructing the path after the A* search is done. The longest
 * possible path is actually 24 steps, even though we can fit 25
 */
int pnode_vals[25];
// a linked list of all visited nodes
PATHNODE *pnode_ptr;
// a stack for recursively searching nodes
PATHNODE *pnode_tblptr[300];
// a linked list of the A* frontier, sorted by distance
PATHNODE *path_2_nodes;
#endif

// for iterating over the 8 possible movement directions
const char pathxdir[8] = { -1, -1, 1, 1, -1, 0, 1, 0 };
const char pathydir[8] = { -1, 1, -1, 1, 0, -1, 0, 1 };

/* data */

/* each step direction is assigned a number like this:
 *       dx
 *     -1 0 1
 *     +-----
 *   -1|5 1 6
 * dy 0|2 0 3
 *    1|8 4 7
 */
char path_directions[9] = { 5, 1, 6, 2, 0, 3, 8, 4, 7 };

/* find the shortest path from (sx,sy) to (dx,dy), using PosOk(PosOkArg,x,y) to
 * check that each step is a valid position. Store the step directions (see
 * path_directions) in path, which must have room for 24 steps
 */
int __fastcall FindPath(BOOL (__fastcall *PosOk)(int, int, int), int PosOkArg, int sx, int sy, int dx, int dy, char *path)
{
	PATHNODE *path_start; // esi
	char initial_h; // al
	PATHNODE *next_node; // eax
	int result; // eax
	PATHNODE *current; // edx
	PATHNODE **previous; // eax
	int path_length; // edi
	bool path_is_full; // zf
	int *step_ptr; // ecx
	char step; // dl

	// clear all nodes, create root nodes for the visited/frontier linked lists
	gdwCurNodes = 0;
	path_2_nodes = path_new_step();
	gdwCurPathStep = 0;
	pnode_ptr = path_new_step();
	path_start = path_new_step();
	path_start->g = 0;
	initial_h = path_get_h_cost(sx, sy, dx, dy);
	path_start->h = initial_h;
	path_start->x = sx;
	path_start->f = initial_h + path_start->g;
	path_start->y = sy;
	path_2_nodes->NextNode = path_start;
	// A* search until we find (dx,dy) or fail
	while ( 1 )
	{
		next_node = GetNextPath();
		// frontier is empty, no path!
		if ( !next_node )
			return 0;
		// reached the end, success!
		if ( next_node->x == dx && next_node->y == dy )
			break;
		// ran out of nodes, abort!
		if ( !path_get_path(PosOk, PosOkArg, next_node, dx, dy) )
			return 0;
	}
	current = next_node;
	previous = &next_node->Parent;
	path_length = 0;
	if ( *previous )
	{
		while ( 1 )
		{
			path_is_full = path_length == 25;
			if ( path_length >= 25 )
				break;
			pnode_vals[++path_length-1] = path_directions[3 * (current->y - (*previous)->y) - (*previous)->x + 4 + current->x];
			current = *previous;
			previous = &(*previous)->Parent;
			if ( !*previous )
			{
				path_is_full = path_length == 25;
				break;
			}
		}
		if ( path_is_full )
			return 0;
	}
	result = 0;
	if ( path_length > 0 )
	{
		step_ptr = &pnode_vals[path_length-1];
		do
		{
			step = *(_BYTE *)step_ptr;
			--step_ptr;
			path[result++] = step;
		}
		while ( result < path_length );
	}
	return result;
}

/* heuristic, estimated cost from (sx,sy) to (dx,dy) */
int __fastcall path_get_h_cost(int sx, int sy, int dx, int dy)
{
	int y; // esi
	int delta_x; // edi
	int delta_y; // eax
	int min_delta; // ecx

	y = sy;
	delta_x = abs(sx - dx);
	delta_y = abs(y - dy);
	// this is a pointless swap, it's just 2(delta_x+delta_y)
	min_delta = delta_x;
	if ( delta_x >= delta_y )
	{
		min_delta = delta_y;
		if ( delta_x > delta_y )
			delta_y = delta_x;
	}
	// see path_check_equal for why this is times 2
	return 2 * (min_delta + delta_y);
}

/* return 2 if pPath is horizontally/vertically aligned with (dx,dy), else 3
 *
 * This approximates that diagonal movement on a square grid should have a cost
 * of sqrt(2). That's approximately 1.5, so they multiply all step costs by 2,
 * except diagonal steps which are times 3
 */
int __fastcall path_check_equal(PATHNODE *pPath, int dx, int dy)
{
	int result; // [esp-4h] [ebp-4h]

	if ( pPath->x == dx || pPath->y == dy )
		result = 2;
	else
		result = 3;
	return result;
}

/* get the next node on the A* frontier to explore (estimated to be closest to
 * the goal), mark it as visited, and return it
 */
PATHNODE *__cdecl GetNextPath()
{
	PATHNODE *result; // eax

	result = path_2_nodes->NextNode;
	if ( result )
	{
		path_2_nodes->NextNode = result->NextNode;
		result->NextNode = pnode_ptr->NextNode;
		pnode_ptr->NextNode = result;
	}
	return result;
}

/* check if stepping from pPath to (dx,dy) cuts a corner. If you step from A to
 * B, both Xs need to be clear:
 *
 *  AX
 *  XB
 *
 *  return true if step is allowed
 */
bool __fastcall path_solid_pieces(PATHNODE *pPath, int dx, int dy)
{
	bool result; // eax
	int dir; // ecx
	int tile1; // ecx
	int tile2; // edx

	result = 1;
	// this maps the four corner directions to 0,1,2,3
	dir = path_directions[3 * (dy - pPath->y) - pPath->x + 4 + dx] - 5;
	// and this is basically a switch
	if ( !dir ) // (-1,-1)->0
	{
		result = 0;
		if ( nSolidTable[dPiece[dx][dy + 1]] )
			return result;
		tile1 = dPiece[dx + 1][dy];
		goto LABEL_13;
	}
	if ( !--dir ) // (1,-1)->1
	{
		tile2 = dPiece[dx][dy + 1];
		goto LABEL_9;
	}
	if ( !--dir ) // (1,1)->2
	{
		tile2 = dPiece[dx][dy-1]; /* check */
LABEL_9:
		result = 0;
		if ( nSolidTable[tile2] )
			return result;
		tile1 = dPiece[dx-1][dy]; /* check */
		goto LABEL_13;
	}
	if ( dir == 1 ) // (-1,1)->3
	{
		result = 0;
		if ( !nSolidTable[dPiece[dx + 1][dy]] )
		{
			tile1 = dPiece[dx][dy-1]; /* check */
LABEL_13:
			if ( !nSolidTable[tile1] )
				result = 1;
			return result;
		}
	}
	return result;
}

/* perform a single step of A* bread-first search by trying to step in every
 * possible direction from pPath with goal (x,y). Check each step with PosOk
 *
 * return 0 if we ran out of preallocated nodes to use, else 1
 */
int __fastcall path_get_path(BOOL (__fastcall *PosOk)(int, int, int), int PosOkArg, PATHNODE *pPath, int x, int y)
{
	int dir; // eax
	int dx; // esi
	int dy; // edi
	int i; // [esp+14h] [ebp-4h]

	dir = 0;
	for ( i = 0; ; dir = i )
	{
		dx = pPath->x + pathxdir[dir];
		dy = pPath->y + pathydir[dir];
		if ( !PosOk(PosOkArg, dx, dy) )
			break;
		if ( path_solid_pieces(pPath, dx, dy) )
			goto LABEL_8;
LABEL_9:
		if ( ++i >= 8 )
			return 1;
	}
	if ( dx != x || dy != y )
		goto LABEL_9;
LABEL_8:
	if ( path_parent_path(pPath, dx, dy, x, y) )
		goto LABEL_9;
	return 0;
}

/* add a step from pPath to (dx,dy), return 1 if successful, and update the
 * frontier/visited nodes accordingly
 *
 * return 1 if step successfully added, 0 if we ran out of nodes to use
 */
int __fastcall path_parent_path(PATHNODE *pPath, int dx, int dy, int sx, int sy)
{
	PATHNODE *pPath2; // edi, pointless copy
	int next_g; // ebx

	PATHNODE *dxdy_frontier; // esi
	signed int empty_slot1; // eax
	struct PATHNODE **pPath_child_ptr1; // ecx
	char dxdy_h; // al

	PATHNODE *dxdy_visited; // esi
	signed int empty_slot2; // eax
	struct PATHNODE **pPath_child_ptr2; // ecx
	char dxdy_f; // al

	PATHNODE *result; // eax
	PATHNODE *dxdy_new; // esi
	char h_new; // al
	signed int empty_slot3; // ecx
	struct PATHNODE **pPath_child_ptr3; // eax

	int dx2; // [esp+Ch] [ebp-4h], pointless copy

	dx2 = dx;
	pPath2 = pPath;
	next_g = pPath->g + path_check_equal(pPath, dx, dy);

	// 3 cases to consider
	// case 1: (dx,dy) is already on the frontier
	dxdy_frontier = path_get_node1(dx2, dy);
	if ( dxdy_frontier )
	{
		empty_slot1 = 0;
		pPath_child_ptr1 = pPath2->Child;
		do
		{
			if ( !*pPath_child_ptr1 )
				break;
			++empty_slot1;
			++pPath_child_ptr1;
		}
		while ( empty_slot1 < 8 );
		pPath2->Child[empty_slot1] = dxdy_frontier;
		if ( next_g < dxdy_frontier->g )
		{
			if ( path_solid_pieces(pPath2, dx2, dy) )
			{
				// we'll explore it later, just update
				dxdy_h = dxdy_frontier->h;
				dxdy_frontier->Parent = pPath2;
				dxdy_frontier->g = next_g;
				dxdy_frontier->f = next_g + dxdy_h;
			}
		}
	}
	else
	{
		// case 2: (dx,dy) was already visited
		dxdy_visited = path_get_node2(dx2, dy);
		if ( dxdy_visited )
		{
			empty_slot2 = 0;
			pPath_child_ptr2 = pPath2->Child;
			do
			{
				if ( !*pPath_child_ptr2 )
					break;
				++empty_slot2;
				++pPath_child_ptr2;
			}
			while ( empty_slot2 < 8 );
			pPath2->Child[empty_slot2] = dxdy_visited;
			if ( next_g < dxdy_visited->g && path_solid_pieces(pPath2, dx2, dy) )
			{
				// update the node
				dxdy_f = next_g + dxdy_visited->h;
				dxdy_visited->Parent = pPath2;
				dxdy_visited->g = next_g;
				dxdy_visited->f = dxdy_f;
				// already explored, so re-update others starting from that node
				path_set_coords(dxdy_visited);
			}
		}
		else
		{
			// case 3: (dx,dy) is totally new
			result = path_new_step();
			dxdy_new = result;
			if ( !result )
				return 0;
			result->Parent = pPath2;
			result->g = next_g;
			h_new = path_get_h_cost(dx2, dy, sx, sy);
			dxdy_new->h = h_new;
			dxdy_new->f = next_g + h_new;
			dxdy_new->x = dx2;
			dxdy_new->y = dy;
			// add it to the frontier
			path_next_node(dxdy_new);
			empty_slot3 = 0;
			pPath_child_ptr3 = pPath2->Child;
			do
			{
				if ( !*pPath_child_ptr3 )
					break;
				++empty_slot3;
				++pPath_child_ptr3;
			}
			while ( empty_slot3 < 8 );
			pPath2->Child[empty_slot3] = dxdy_new;
		}
	}
	return 1;
}

/* return a node for (dx,dy) on the frontier, or NULL if not found */
PATHNODE *__fastcall path_get_node1(int dx, int dy)
{
	PATHNODE *result; // eax

	result = path_2_nodes;
	do
		result = result->NextNode;
	while ( result && (result->x != dx || result->y != dy) );
	return result;
}

/* return a node for (dx,dy) if it was visited, or NULL if not found */
PATHNODE *__fastcall path_get_node2(int dx, int dy)
{
	PATHNODE *result; // eax

	result = pnode_ptr;
	do
		result = result->NextNode;
	while ( result && (result->x != dx || result->y != dy) );
	return result;
}

/* insert pPath into the frontier (keeping the frontier sorted by total
 * distance) */
void __fastcall path_next_node(PATHNODE *pPath)
{
	PATHNODE *current; // edx
	PATHNODE *next; // eax

	current = path_2_nodes;
	next = path_2_nodes->NextNode;
	if ( next )
	{
		do
		{
			if ( next->f >= pPath->f )
				break;
			current = next;
			next = next->NextNode;
		}
		while ( next );
		pPath->NextNode = next;
	}
	current->NextNode = pPath;
}

/* update all path costs using depth-first search starting at pPath */
void __fastcall path_set_coords(PATHNODE *pPath)
{
	PATHNODE *PathOld; // edi
	PATHNODE *PathAct; // esi
	char next_g; // al
	int i; // [esp+0h] [ebp-8h]
	PATHNODE **child_ptr; // [esp+4h] [ebp-4h]

	path_push_active_step(pPath);
	while ( gdwCurPathStep )
	{
		PathOld = path_pop_active_step();
		child_ptr = PathOld->Child;
		for(i = 0; i < 8; i++)
		{
			PathAct = *child_ptr;
			if ( !*child_ptr )
				break;

			if ( PathOld->g + path_check_equal(PathOld, PathAct->x, PathAct->y) < PathAct->g )
			{
				if ( path_solid_pieces(PathOld, PathAct->x, PathAct->y) )
				{
					PathAct->Parent = PathOld;
					next_g = PathOld->g + path_check_equal(PathOld, PathAct->x, PathAct->y);
					PathAct->g = next_g;
					PathAct->f = next_g + PathAct->h;
					path_push_active_step(PathAct);
				}
			}
			++child_ptr;
		}
	}
}

/* push pPath onto the pnode_tblptr stack */
void __fastcall path_push_active_step(PATHNODE *pPath)
{
	int stack_index; // eax

	stack_index = gdwCurPathStep++;
	pnode_tblptr[stack_index] = pPath;
}

/* pop and return a node from the pnode_tblptr stack */
PATHNODE *__cdecl path_pop_active_step()
{
	return pnode_tblptr[--gdwCurPathStep];
}

/* zero one of the preallocated nodes and return a pointer to it, or NULL if
 * none are available */
PATHNODE *__cdecl path_new_step()
{
	PATHNODE *new_node; // esi

	if ( gdwCurNodes == 300 )
		return 0;
	new_node = &path_nodes[gdwCurNodes++];
	memset(new_node, 0, 0x34u);
	return new_node;
}
