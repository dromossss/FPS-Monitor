#include "MessageBox.h"
#include "WinAPI.h"

using namespace std::literals;

namespace pmon::util::win
{
	MsgBox::MsgBox() = default;
	MsgBox::MsgBox(std::string message) : message_{ std::move(message) } {}
	MsgBox::~MsgBox()
	{
		UINT typeFlags = 0;
		if (type_ == Type_::Error) {
			typeFlags |= MB_ICONERROR;
		}
		if (modal_) {
			typeFlags |= MB_TASKMODAL;
		}

		if (!title_) {
			switch (type_) {
			case Type_::Error: title_ = "Error"; break;
			case Type_::Notification: title_ = "Notification"; break;
			default: title_ = "Message"; break;
			}
		}

		MessageBoxA((HWND)hWndModal_, message_.c_str(), title_->c_str(), typeFlags);
	}
	MsgBox& MsgBox::WithMessage(std::string message)
	{
		message_ = std::move(message);
		return *this;
	}
	MsgBox& MsgBox::WithTitle(std::string title)
	{
		title_ = std::move(title);
		return *this;
	}
	MsgBox& MsgBox::AsError()
	{
		type_ = Type_::Error;
		return *this;
	}
	MsgBox& MsgBox::AsModal(const void* hWnd)
	{
		modal_ = true;
		hWndModal_ = hWnd;
		return *this;
	}
}