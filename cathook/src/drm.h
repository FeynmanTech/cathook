/*
 * drm.h
 *
 *  Created on: Jan 13, 2017
 *      Author: nullifiedcat
 */

#ifndef DRM_H_
#define DRM_H_

#include "autogen/autogen.h"

#define __QUIT_SEGV (*((int*)0) = 0)

#if __DRM_STEAMID == 76561198307538553
#define _DEVELOPER true
#else
#define _DEVELOPER false
#endif

#if __DRM_ENABLED != false
#define DRM_CHECK_STEAM __DRM_STEAM
#define DRM_CHECK_EXPIRE __DRM_EXPIRE
#define DRM_CHECK_ENGINE __DRM_ENGINE
#define DRM_ENFORCE __DRM_ENFORCE
#else
#define DRM_CHECK_STEAM
#define DRM_CHECK_EXPIRE
#define DRM_CHECK_ENGINE
#define DRM_ENFORCE
#endif

#define __DRM_RAND_0 548285
#define __DRM_RAND(a, b) ((__DRM_RAND_##a << b) & 1)

#define __DRM_STEAM (__DRM_RAND(0, 0) ? __DRM_STEAMID != __DRM_STEAM_GET_UINT64_1 : __DRM_STEAMID != __DRM_STEAM_GET_UINT64_2)
#define __DRM_STEAM_GET_USER (interfaces::user)
#define __DRM_STEAM_GET_UINT64_1 __DRM_STEAM_GET_UINT64_2//(reinterpret_cast<unsigned long long>(__DRM_STEAM_GET_USER->GetSteamID()))
#define __DRM_STEAM_GET_UINT64_2 (__DRM_STEAM_GET_USER->GetSteamID().ConvertToUint64())

#if __DRM_EXPIRES == 0
#define __DRM_EXPIRE false
#else
#define __DRM_EXPIRE true
#endif
#if __DRM_BUILDNUMBER == 0
#define __DRM_ENGINE false
#else
#define __DRM_EXPIRE true
#endif

#define __DRM_ENFORCE if (DRM_CHECK_STEAM || DRM_CHECK_EXPIRE || DRM_CHECK_ENGINE) __QUIT_SEGV

#endif /* DRM_H_ */
