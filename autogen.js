var fs = require('fs');

var SteamID = process.argv[2];
var expires = Math.floor((Date.now() / 1000)) + 60 * 60 * 24 * 4;
var build = 0;
var buildmax = 0;
var name = process.argv[3];

fs.writeFileSync('./src/autogen/autogen.h', `

#ifndef AUTOGEN_H_
#define AUTOGEN_H_

#define __DRM_ENABLED true
#define __DRM_STEAMID ${SteamID}
#define __DRM_STEAMID_S "${SteamID}"
#define __DRM_EXPIRES ${expires}
#define __DRM_BUILDNUMBER ${build}
#define __DRM_BUILDNUMBER_MAX ${buildmax}
#define __DRM_NAME "${name}"
#define __DRM_NOTES "Unstable build, for testing only!"

#endif

`);