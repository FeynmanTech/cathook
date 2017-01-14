var fs = require('fs');

var SteamID = process.argv[2];
var expires = 0;
var build = 0;
var buildmax = 0;
var name = 'd4rkc4t';

fs.writeFileSync('./src/autogen/autogen.h', `

#ifndef AUTOGEN_H_
#define AUTOGEN_H_

#define __DRM_ENABLED true
#define __DRM_STEAMID ${SteamID}
#define __DRM_EXPIRES ${expires}
#define __DRM_BUILDNUMBER ${build}
#define __DRM_BUILDNUMBER_MAX ${buildmax}
#define __DRM_NAME "${name}"

#endif

`);