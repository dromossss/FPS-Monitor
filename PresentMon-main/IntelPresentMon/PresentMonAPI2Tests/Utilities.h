#pragma once
#include <crtdbg.h>

inline bool CrtDiffHasMemoryLeaks(const _CrtMemState& before, const _CrtMemState& after)
{
	_CrtMemState difference;
	if (_CrtMemDifference(&difference, &before, &after)) {
		if (difference.lCounts[_NORMAL_BLOCK] > 0) {
			return true;
		}
	}
	return false;
}