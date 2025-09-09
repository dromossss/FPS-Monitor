// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: MIT

#include <CppUnitTest.h>

#include <Core/source/gfx/layout/GraphData.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace AlgorithmTests
{
	using namespace p2c::gfx::lay;

	TEST_CLASS(TestExtremeQueue)
	{
	public:
		TEST_METHOD(MaxAddEmpty)
		{
			MaxQueue q;
			q.Push(10.f);
			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxAddToBack)
		{
			MaxQueue q;
			q.Push(10.f);
			q.Push(9.f);
			q.Push(8.f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(9.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::AreEqual(8.f, *q.GetCurrent());
			q.Pop(8.f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxResetPushGreaterThanMax)
		{
			MaxQueue q;
			q.Push(8.f);
			q.Push(10.f);
			q.Push(9.f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(9.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxStackingPushEqualToMax)
		{
			MaxQueue q;
			q.Push(10.f);
			q.Push(10.f);
			q.Push(9.f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(9.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxAddMiddleTruncates)
		{
			MaxQueue q;
			q.Push(10.f);
			q.Push(9.f);
			q.Push(8.f);
			q.Push(7.f);

			q.Push(8.5f);
			q.Push(7.5f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(9.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::AreEqual(8.5f, *q.GetCurrent());
			q.Pop(8.5f);
			Assert::AreEqual(7.5f, *q.GetCurrent());
			q.Pop(7.5f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxAddMiddleEqualTruncates)
		{
			MaxQueue q;
			q.Push(10.f);
			q.Push(9.f);
			q.Push(8.f);
			q.Push(7.f);

			q.Push(8.f);
			q.Push(7.5f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::AreEqual(9.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::AreEqual(8.0f, *q.GetCurrent());
			q.Pop(8.0f);
			Assert::AreEqual(8.0f, *q.GetCurrent());
			q.Pop(8.0f);
			Assert::AreEqual(7.5f, *q.GetCurrent());
			q.Pop(7.5f);
			Assert::IsTrue(q.Empty());
		}
		TEST_METHOD(MaxNonCandidatePopDoesntRemove)
		{
			MaxQueue q;
			q.Push(9.f);
			q.Push(10.f);

			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(9.f);
			Assert::AreEqual(10.f, *q.GetCurrent());
			q.Pop(10.f);
			Assert::IsTrue(q.Empty());
		}
	};
}
