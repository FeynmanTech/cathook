/*
 * common.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "beforecheaders.h"
#include <vector>

#include "aftercheaders.h"
#include "drawing.h"
#include "entitycache.h"
#include "enums.h"
#include "globals.h"
#include "helpers.h"
#include "interfaces.h"
#include "localplayer.h"
#include "logging.h"
#include "playerresource.h"
#include "relations.h"
#include "usercmd.h"
#include "trace.h"
#include "cvwrapper.h"
#include "netvars.h"
#include "vfunc.h"
#include "hooks.h"
#include "prediction.h"
#include "conditions.h"
#include "ipc/ipcctl.h"
#include "chatstack.h"
#include "textfile.h"
#include "gui/GUI.h"

#include "hacks/hacklist.h"
#include "glowobjects.h"

#include "drm.h"

#include "sdk.h"

#define SQR(x) x * x

#ifndef CATHOOK_BUILD_NUMBER
#define CATHOOK_BUILD_NUMBER "1"
#endif
#define CATHOOK_BUILD_NAME "Sweet Roll"

#define CON_NAME "cat"
#define CON_PREFIX CON_NAME "_"

#define SUPER_VERBOSE_DEBUG false
#if SUPER_VERBOSE_DEBUG == true
#define SVDBG(...) logging::Info(__VA_ARGS__)
#else
#define SVDBG(...)
#endif

#ifndef DEG2RAD
#define DEG2RAD(x) (float)(x) * (PI / 180.0f)
#endif

#define NO_DEVIGNORE false
#if _DEVELOPER == true || __DRM_ENABLED == false
#define DEBUG_SEGV true
#else
#define DEBUG_SEGV false
#endif
#define NO_IPC true
#define STR(c) #c

#if DEBUG_SEGV == true

#define SEGV_BEGIN \
	try {

#define SEGV_END \
	} catch (...) { \
		logging::Info("SEGV/FPE occured! %s in %s:%d", __func__, __FILE__, __LINE__); \
	}

#define SEGV_END_INFO(x) \
	} catch (...) { \
		logging::Info("SEGV/FPE occured! (%s)", x); \
	}

#define SAFE_CALL(x) \
	SEGV_BEGIN \
	x; \
	SEGV_END_INFO(#x)

#else

#define SEGV_BEGIN
#define SEGV_END
#define SEGV_END_INFO(x)
#define SAFE_CALL(x) x

#endif

/*#define ADD_HACK(x) \
	hack::AddHack(g_ph##x = new x());

#define CREATE_MOVE(x) \
	g_ph##x->CreateMove(thisptr, inputSample, cmd)*/

#endif /* COMMON_H_ */
