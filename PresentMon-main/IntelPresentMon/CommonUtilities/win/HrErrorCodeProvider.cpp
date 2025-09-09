#include "HrErrorCodeProvider.h"
#include "../log/ErrorCode.h"
#include "Utilities.h"

namespace pmon::util::win
{
    std::type_index pmon::util::win::HrErrorCodeProvider::GetTargetType() const
    {
        return typeid(hr_wrap);
    }

    log::IErrorCodeResolver::Strings pmon::util::win::HrErrorCodeProvider::Resolve(const log::ErrorCode& c) const
    {
        if (const auto s = c.AsSigned()) {
            return {
                .type = "HRESULT",
                .description = GetErrorDescription((HRESULT)*s),
            };
        }
        return {};
    }
}
