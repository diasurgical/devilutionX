/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#ifndef __HELP_H__
#define __HELP_H__

namespace dvl {

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL helpflag;

void InitHelp();
void DrawHelp(CelOutputBuffer out);
void DisplayHelp();
void HelpScrollUp();
void HelpScrollDown();

#ifdef __cplusplus
}
#endif

}

#endif /* __HELP_H__ */
