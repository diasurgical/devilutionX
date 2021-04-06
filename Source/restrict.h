/**
 * @file restrict.h
 *
 * Interface of functionality for checking if the game will be able run on the system.
 */
#ifndef __RESTRICT_H__
#define __RESTRICT_H__

namespace dvl {

#ifdef __cplusplus
extern "C" {
#endif

void ReadOnlyTest();

#ifdef __cplusplus
}
#endif

}

#endif /* __RESTRICT_H__ */
