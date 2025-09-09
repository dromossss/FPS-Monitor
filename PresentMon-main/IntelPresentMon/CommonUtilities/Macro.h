#pragma once

#define ZSTRINGIFY_(x) #x
#define STRINGIFY(x) ZSTRINGIFY_(x)

#define ZCONCATENATE_(x, y) x##y
#define CONCATENATE(x, y) ZCONCATENATE_(x, y)