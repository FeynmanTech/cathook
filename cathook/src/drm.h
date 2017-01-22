/*
 * drm.h
 *
 *  Created on: Jan 13, 2017
 *      Author: nullifiedcat
 */

#ifndef DRM_H_
#define DRM_H_

#include <ctime>

#ifndef __DRM_ENABLED
#define __DRM_ENABLED true
#endif
#define __DRM_NOTES "Unstable build, for testing only!"

#define __QUIT_SEGV (*((int*)0) = 0)

#if __DRM_ENABLED == false
#define _DEVELOPER true
#else
#define _DEVELOPER false
#endif

#ifndef __DRM_HWID_0
#define __DRM_HWID_0 0x7fa8d247bb8389e7
#define __DRM_HWID_1 0x08ebdb1cdb642f0e
#define __DRM_HWID_2 0xf571eeb0c17a0aed
#define __DRM_HWID_3 0x20cea01f36f359ac
#endif

#ifndef __DRM_EXPIRES
#define __DRM_EXPIRES 0
#endif
#ifndef __DRM_BUILDNUMBER
#define __DRM_BUILDNUMBER 0
#endif
#ifndef __DRM_NAME
#define __DRM_NAME "UNDEFINED"
#endif

#if __DRM_ENABLED != false
#define DRM_CHECK_HWID __DRM_HWID
#define DRM_CHECK_EXPIRE __DRM_EXPIRE
#define DRM_CHECK_ENGINE __DRM_ENGINE
#define DRM_ENFORCE __DRM_ENFORCE
#else
#define DRM_CHECK_HWID
#define DRM_CHECK_EXPIRE
#define DRM_CHECK_ENGINE
#define DRM_ENFORCE
#endif

#if __DRM_HWID_0 == 0
#define __DRM_HWID false
#else
#define __DRM_HWID !(hwid::check_hwid(__DRM_HWID_0, __DRM_HWID_1, __DRM_HWID_2, __DRM_HWID_3))
#endif
#if __DRM_EXPIRES == 0
#define __DRM_EXPIRE false
#else
#define __DRM_EXPIRE ((time(0) - __DRM_EXPIRES) > 0)
#endif
#if __DRM_BUILDNUMBER == 0
#define __DRM_ENGINE false
#else
#define __DRM_ENGINE true
#endif

#define __DRM_ENFORCE if (DRM_CHECK_EXPIRE || DRM_CHECK_ENGINE || DRM_CHECK_HWID) __QUIT_SEGV

#endif /* DRM_H_ */
