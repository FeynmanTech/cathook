/*
 * common.h
 *
 *  Created on: Dec 5, 2016
 *      Author: nullifiedcat
 */

#ifndef COMMON_H_
#define COMMON_H_

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
#include "prediction.h"
#include "sdk.h"

#define SQR(x) x * x

#define CON_NAME "cat"
#define CON_PREFIX CON_NAME "_"

#define DEG2RAD(x) (float)(x) * (PI / 180.0f)

#define DEBUG_SEGV false

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

#endif /* COMMON_H_ */
