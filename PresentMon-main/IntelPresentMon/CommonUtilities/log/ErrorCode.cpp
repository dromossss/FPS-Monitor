#include "ErrorCode.h"
#include <limits>
#include <format>

namespace pmon::util::log
{
	ErrorCode::ErrorCode(const ErrorCode& other)
		:
		pTypeInfo_{ other.pTypeInfo_ },
		pStrings_{ other.pStrings_ ? std::make_unique<IErrorCodeResolver::Strings>(*other.pStrings_) :
			std::unique_ptr<IErrorCodeResolver::Strings>{} },
		integral_{ other.integral_ }
	{}
	ErrorCode::ErrorCode(ErrorCode&&) = default;
	ErrorCode& ErrorCode::operator=(ErrorCode&& rhs) = default;
	bool ErrorCode::HasUnsigned() const { return std::holds_alternative<uint64_t>(integral_); }
	bool ErrorCode::HasSigned() const { return std::holds_alternative<int64_t>(integral_); }
	bool ErrorCode::HasIntegral() const { return !std::holds_alternative<std::monostate>(integral_); }
	bool ErrorCode::Fits32() const
	{
		if (auto u = AsUnsigned()) {
			return *u <= std::numeric_limits<uint32_t>::max();
		}
		else if (auto u = AsSigned()) {
			using i32 = std::numeric_limits<int32_t>;
			return *u >= i32::min() && *u <= i32::max();
		}
		// doesn't really apply, but return true anyways
		return true;
	}
	std::optional<uint64_t> ErrorCode::AsUnsigned() const {
		return HasUnsigned() ?
			std::optional{ std::get<uint64_t>(integral_) } : std::nullopt;
	}
	std::optional<int64_t> ErrorCode::AsSigned() const {
		return HasSigned() ?
			std::optional{ std::get<int64_t>(integral_) } : std::nullopt;
	}
	bool ErrorCode::Empty() const
	{
		return std::holds_alternative<std::monostate>(integral_);
	}
	bool ErrorCode::Resolve(const IErrorCodeResolver& resolver)
	{
		if (!Empty() && HasTypeInfo()) {
			pStrings_ = std::make_unique<IErrorCodeResolver::Strings>(
				resolver.Resolve(std::type_index{ *pTypeInfo_ }, *this));
			return true;
		}
		pStrings_ = std::make_unique<IErrorCodeResolver::Strings>();
		return false;
	}
	bool ErrorCode::HasTypeInfo() const
	{
		return bool(pTypeInfo_);
	}
	bool ErrorCode::IsResolved() const
	{
		return bool(pStrings_);
	}
	bool ErrorCode::IsResolvedNontrivial() const
	{
		return IsResolved() && !pStrings_->type.empty();
	}
	std::string ErrorCode::AsHex() const
	{
		if (Empty()) {
			return "---";
		}
		if (auto o = AsSigned()) {
			if (Fits32()) {
				return std::format("0x{:08X}", (uint32_t)*o);
			}
			else {
				return std::format("0x{:016X}", (uint64_t)*o);
			}
		}
		if (auto o = AsUnsigned()) {
			if (Fits32()) {
				return std::format("0x{:08X}", (uint32_t)*o);
			}
			else {
				return std::format("0x{:016X}", (uint64_t)*o);
			}
		}
		return "";
	}
	const IErrorCodeResolver::Strings* ErrorCode::GetStrings() const
	{
		return pStrings_.get();
	}
	ErrorCode::operator bool() const
	{
		return !Empty();
	}
}