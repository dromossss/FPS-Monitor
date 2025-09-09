// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "StyleCompiler.h"
#include "StyleResolver.h"
#include "../Element.h"

namespace p2c::gfx::lay::sty
{
	// ties compiler and resolver together
	class StyleProcessor
	{
	public:
		// types
		// this token ensures stack push/pops for compiler and resolver are matched properly
		class StackToken
		{
		public:
			StackToken(StyleProcessor& processor_) : processor{ processor_ } {}
			StackToken(const StackToken&) = delete;
			StackToken& operator=(const StackToken&) = delete;
			~StackToken()
			{
				Resolve();
			}
			void Resolve()
			{
				if (!resolved)
				{
					processor.Pop();
					resolved = true;
				}
			}
		private:
			StyleProcessor& processor;
			bool resolved = false;
		};
		// functions
		StyleProcessor(std::vector<std::shared_ptr<Stylesheet>> rules, std::shared_ptr<Stylesheet> pBase)
			:
			compiler{ std::move(rules) },
			resolver{ std::move(pBase) }
		{}
		StackToken Push(const Element* pElement)
		{
			resolver.PushAppliedSheet(compiler.Compile(pElement->GetClasses()));
			compiler.PushParent(pElement->GetClasses());
			return { *this };
		}
		template<class A>
		auto Resolve() const
		{
			return resolver.Resolve<A>();
		}
	private:
		// functions
		void Pop()
		{
			compiler.PopParent();
			resolver.PopAppliedSheet();
		}
		// data
		StyleCompiler compiler;
		StyleResolver resolver;
	};
}