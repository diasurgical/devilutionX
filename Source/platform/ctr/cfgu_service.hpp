#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <3ds/result.h>
#include <3ds/services/cfgu.h>

#ifdef __cplusplus
}
#endif

namespace devilution {
namespace n3ds {

class CFGUService {
public:
	CFGUService()
	{
		Result res = cfguInit();
		isInitialized = R_SUCCEEDED(res);
	}

	~CFGUService()
	{
		cfguExit();
	}

	bool IsInitialized()
	{
		return isInitialized;
	}

private:
	bool isInitialized;
};

} // namespace n3ds
} // namespace devilution
