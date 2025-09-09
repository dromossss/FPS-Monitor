// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include <Core/source/win/WinAPI.h>

#include <CppUnitTest.h>

#include <Core/source/gfx/layout/style/Stylesheet.h>
#include <Core/source/gfx/layout/style/StyleCompiler.h>
#include <Core/source/gfx/layout/style/StyleResolver.h>
#include <Core/source/gfx/layout/style/StyleProcessor.h>
#include <Core/source/gfx/layout/style/Attributes.h>
#include <Core/source/gfx/layout/style/ReadOnlyAttributes.h>
#include <Core/source/gfx/layout/FlexElement.h>

#include <Core/source/gfx/layout/style/RawAttributeHelpers.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

p2c::gfx::lay::sty::at::RawAttributeObjectValue MakeColorRaw(int r, int g, int b, float a)
{
	using namespace p2c;
	using namespace gfx;
	using namespace lay;

	return sty::at::RawAttributeObjectValue{
		{"r", (double)r},
		{"g", (double)g},
		{"b", (double)b},
		{"a", a},
	};
}

namespace LayoutTests
{
	using namespace std::string_literals;

	TEST_CLASS(TestStyle)
	{
	public:
		TEST_METHOD(Auto)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::height::key, std::wstring(at::Special::Auto::key));
			Assert::IsTrue(std::holds_alternative<at::Special::Auto>(pSheet->Resolve<at::height>()));
		}
		TEST_METHOD(Absent)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::height::key, std::wstring(at::Special::Auto::key));
			Assert::IsTrue(std::holds_alternative<std::monostate>(pSheet->Resolve<at::width>()));
		}
		TEST_METHOD(Value)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::height::key, 420.69);
			Assert::IsTrue(std::get<float>(pSheet->Resolve<at::height>()) == 420.69f);
		}
		TEST_METHOD(Enum)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::flexAlignment::key, at::make::Enum(p2c::gfx::lay::FlexAlignment::Center));
			Assert::IsTrue(std::get<p2c::gfx::lay::FlexAlignment>(pSheet->Resolve<at::flexAlignment>()) == p2c::gfx::lay::FlexAlignment::Center);
		}
		TEST_METHOD(ReadOnly)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::borderLeft::key, 12.1);
			pSheet->InsertRaw(at::borderTop::key, 13.2);
			pSheet->InsertRaw(at::borderRight::key, 14.3);
			pSheet->InsertRaw(at::borderBottom::key, 15.4);
			StyleResolver resolver{ std::move(pSheet) };
			resolver.PushAppliedSheet(Stylesheet::Make());

			auto border = at::border::Resolve(resolver);
			Assert::AreEqual(12.1f, border.left);
			Assert::AreEqual(13.2f, border.top);
			Assert::AreEqual(14.3f, border.right);
			Assert::AreEqual(15.4f, border.bottom);
		}
		TEST_METHOD(CompileBaseSelectNoInherit)
		{
			using namespace p2c::gfx::lay::sty;
			auto pActiveSheet = Stylesheet::Make({ {}, {"a", "c"} });
			pActiveSheet->InsertRaw(at::width::key, 69.);
			auto pInactiveSheet = Stylesheet::Make({ {}, {"b", "d"} });
			pInactiveSheet->InsertRaw(at::width::key, 420.);
			auto pBaseSheet = Stylesheet::Make();
			pBaseSheet->InsertRaw(at::width::key, std::wstring(at::Special::Auto::key));
			pBaseSheet->InsertRaw(at::height::key, std::wstring(at::Special::Auto::key));

			StyleCompiler compiler;
			compiler.SetSheets({ pInactiveSheet, pActiveSheet });
			auto pCompiled = compiler.Compile({ "a", "b", "c" });

			StyleResolver resolver;
			resolver.PushAppliedSheet(pCompiled);
			resolver.SetBaseSheet(pBaseSheet);
			auto width = resolver.Resolve<at::width>();
			auto height = resolver.Resolve<at::height>();
			
			Assert::IsTrue(*width == 69.f);
			Assert::IsFalse(bool(height));
		}
		TEST_METHOD(CompileBaseSelectWithInherit)
		{
			using namespace p2c::gfx::lay::sty;
			auto pActiveSheet = Stylesheet::Make({ {}, {"a", "c"} });
			pActiveSheet->InsertRaw(at::backgroundColor::key, std::wstring(at::Special::Inherit::key));
			pActiveSheet->InsertRaw(at::borderColorLeft::key, std::wstring(at::Special::Inherit::key));
			auto pInactiveSheet = Stylesheet::Make({ {}, {"b", "d"} });
			pInactiveSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(4, 20, 0, 1.f));
			auto pBaseSheet = Stylesheet::Make();
			pBaseSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(10, 0, 0, 1.f));
			pBaseSheet->InsertRaw(at::borderColorLeft::key, MakeColorRaw(22, 0, 0, 1.f));

			auto pParentSheet = Stylesheet::Make();
			pParentSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(1337, 20, 0, 1.f));

			StyleCompiler compiler;
			compiler.SetSheets({ pInactiveSheet, pActiveSheet });
			auto pCompiled = compiler.Compile({ "a", "b", "c" });

			StyleResolver resolver;
			resolver.PushAppliedSheet(pParentSheet);
			resolver.PushAppliedSheet(pCompiled);
			resolver.SetBaseSheet(pBaseSheet);
			auto bgc = resolver.Resolve<at::backgroundColor>();
			auto bdc = resolver.Resolve<at::borderColorLeft>();

			Assert::AreEqual(float(1337. / 255.), bgc.r);
			Assert::AreEqual(float(22. / 255.), bdc.r);
		}
		TEST_METHOD(InheritDefaultTwoParent)
		{
			using namespace p2c::gfx::lay::sty;
			auto pActiveSheet = Stylesheet::Make({ {}, {"a", "c"} });
			pActiveSheet->InsertRaw(at::backgroundColor::key, std::wstring(at::Special::Inherit::key));
			pActiveSheet->InsertRaw(at::borderColorLeft::key, std::wstring(at::Special::Inherit::key));
			auto pInactiveSheet = Stylesheet::Make({ {}, {"b", "d"} });
			pInactiveSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(4, 20, 0, 1.f));
			auto pBaseSheet = Stylesheet::Make();
			pBaseSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(1, 0, 0, 1.f));
			pBaseSheet->InsertRaw(at::borderColorLeft::key, MakeColorRaw(2, 0, 0, 1.f));

			auto pParentSheetNear = Stylesheet::Make();
			auto pParentSheetFar = Stylesheet::Make();
			pParentSheetFar->InsertRaw(at::backgroundColor::key, MakeColorRaw(11, 0, 0, 1.f));
			pParentSheetFar->InsertRaw(at::borderColorLeft::key, MakeColorRaw(22, 0, 0, 1.f));

			StyleCompiler compiler;
			compiler.SetSheets({ pInactiveSheet, pActiveSheet });
			auto pCompiled = compiler.Compile({ "a", "b", "c" });

			StyleResolver resolver;
			resolver.PushAppliedSheet(pParentSheetFar);
			resolver.PushAppliedSheet(pParentSheetNear);
			resolver.PushAppliedSheet(pCompiled);
			resolver.SetBaseSheet(pBaseSheet);
			auto bgc = resolver.Resolve<at::backgroundColor>();
			auto bdc = resolver.Resolve<at::borderColorLeft>();

			Assert::AreEqual(float(1. / 255.), bgc.r);
			Assert::AreEqual(float(2. / 255.), bdc.r);
		}
		TEST_METHOD(InheritSecondTwoParent)
		{
			using namespace p2c::gfx::lay::sty;
			auto pActiveSheet = Stylesheet::Make({ {}, {"a", "c"} });
			pActiveSheet->InsertRaw(at::backgroundColor::key, std::wstring(at::Special::Inherit::key));
			pActiveSheet->InsertRaw(at::borderColorLeft::key, std::wstring(at::Special::Inherit::key));
			auto pInactiveSheet = Stylesheet::Make({ {}, {"b", "d"} });
			pInactiveSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(4, 20, 0, 1.f));
			auto pBaseSheet = Stylesheet::Make();
			pBaseSheet->InsertRaw(at::backgroundColor::key, MakeColorRaw(1, 0, 0, 1.f));
			pBaseSheet->InsertRaw(at::borderColorLeft::key, MakeColorRaw(2, 0, 0, 1.f));

			auto pParentSheetNear = Stylesheet::Make();
			auto pParentSheetFar = Stylesheet::Make();
			pParentSheetNear->InsertRaw(at::backgroundColor::key, std::wstring(at::Special::Inherit::key));
			pParentSheetNear->InsertRaw(at::borderColorLeft::key, std::wstring(at::Special::Inherit::key));
			pParentSheetFar->InsertRaw(at::backgroundColor::key, MakeColorRaw(11, 0, 0, 1.f));
			pParentSheetFar->InsertRaw(at::borderColorLeft::key, MakeColorRaw(22, 0, 0, 1.f));

			StyleCompiler compiler;
			compiler.SetSheets({ pInactiveSheet, pActiveSheet });
			auto pCompiled = compiler.Compile({ "a", "b", "c" });

			StyleResolver resolver;
			resolver.PushAppliedSheet(pParentSheetFar);
			resolver.PushAppliedSheet(pParentSheetNear);
			resolver.PushAppliedSheet(pCompiled);
			resolver.SetBaseSheet(pBaseSheet);
			auto bgc = resolver.Resolve<at::backgroundColor>();
			auto bdc = resolver.Resolve<at::borderColorLeft>();

			Assert::AreEqual(float(11. / 255.), bgc.r);
			Assert::AreEqual(float(22. / 255.), bdc.r);
		}
		TEST_METHOD(ReadOnlyProcessed)
		{
			using namespace p2c::gfx::lay::sty;
			auto pSheet = Stylesheet::Make();
			pSheet->InsertRaw(at::borderLeft::key, 12.1);
			pSheet->InsertRaw(at::borderTop::key, 13.2);
			pSheet->InsertRaw(at::borderRight::key, 14.3);
			pSheet->InsertRaw(at::borderBottom::key, 15.4);
			StyleProcessor processor{ {}, std::move(pSheet)};
			
			p2c::gfx::lay::FlexElement el{ {}, {} };
			auto tok = processor.Push(&el);

			auto border = processor.Resolve<at::border>();
			Assert::AreEqual(12.1f, border.left);
			Assert::AreEqual(13.2f, border.top);
			Assert::AreEqual(14.3f, border.right);
			Assert::AreEqual(15.4f, border.bottom);
		}


		// test precedence of target element matching sheets
		// test enum resolution
		// test successive parent pushing/popping sequences
		// problem with conflicting styles / closer parent match should win (not decided yet)
	};
}
