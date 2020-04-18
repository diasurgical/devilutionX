//HEADER_GOES_HERE
#ifndef __TRACK_H__
#define __TRACK_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

void track_process();
void track_repeat_walk(BOOL rep);
BOOL track_isscrolling();

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __TRACK_H__ */
