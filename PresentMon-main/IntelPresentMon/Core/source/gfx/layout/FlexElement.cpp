// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "FlexElement.h"
#include "AxisMapping.h"
#include <Core/source/infra/Logging.h>
#include <CommonUtilities/Exception.h>
#include <numeric>
#include <algorithm>
#include "style/StyleProcessor.h"

namespace rn = std::ranges;
using namespace pmon::util;

namespace p2c::gfx::lay
{
	// distributes space for inline justification based on mode enum
	class SpaceDistributor
	{
	public:
		float Pre() { return Pre_(); }
		float Post() { const auto v = Post_(); iNext++; return v; };
		static std::unique_ptr<SpaceDistributor> Make(FlexJustification justi, float space, size_t count);
		SpaceDistributor(float space, size_t count) : totalSpace{ space }, count{ count } {}
		virtual ~SpaceDistributor() = default;
	protected:
		virtual float Pre_() = 0;
		virtual float Post_() = 0;
		size_t iNext = 0;
		size_t count;
		float totalSpace;
	};


	FlexElement::FlexElement(std::vector<std::shared_ptr<Element>> children, std::vector<std::string> classes_)
		:
		Element{ std::move(classes_) }
	{
		for (auto& p : children)
		{
			AddChild(std::move(p));
		}
	}

	// Called with nullopt to get baseline width, call with actual width to get height
	LayoutConstraints FlexElement::QueryLayoutConstraints_(std::optional<float> width, sty::StyleProcessor& sp, Graphics& gfx) const
	{
		const auto queryDirection = width ? FlexDirection::Column : FlexDirection::Row;

		const auto flexGrow = sp.Resolve<sty::at::flexGrow>();
		const auto flexDirection = sp.Resolve<sty::at::flexDirection>();

		// calculate constraints based on constraints of all children in container
		// TODO: grow behavior when one flex >child< (or more) has grow?
		LayoutConstraints constraints{.flexGrow = flexGrow};
		for (auto& pChild : GetChildren())
		{
			const auto childConstraints = pChild->QueryLayoutConstraints(queryDirection, sp, gfx);
			if (flexDirection == queryDirection)
			{
				constraints.basis += childConstraints.basis;
				constraints.min += childConstraints.min;
			}
			else
			{
				constraints.basis = std::max(childConstraints.basis, constraints.basis);
				constraints.min = std::max(childConstraints.min, constraints.min);
			}
		}
		return constraints;
	}

	// called to finalize the dimensions of each child in flex container 
	// column direction can only be set if row direction has already been set
	void FlexElement::SetDimension_(float dimension, FlexDirection setdir, sty::StyleProcessor& sp, Graphics& gfx)
	{
		const auto flexDirection = sp.Resolve<sty::at::flexDirection>();
		const auto flexAlignment = sp.Resolve<sty::at::flexAlignment>();

		if (flexDirection == setdir) // container inline with setting direction
		{
			// cache contraints with ptr to child (useful for sorting)
			struct LayoutConstraintRecord
			{
				Element* pChild;
				LayoutConstraints constraints;
				float distToBottom = -1.0f; // this is the "distance" to bottoming out (baseline shrinks to min)
			};
			std::vector<LayoutConstraintRecord> records;
			for (auto& pChild : GetChildren())
			{
				records.push_back({ pChild.get(), pChild->QueryLayoutConstraints(flexDirection, sp, gfx) });
			}
			// get total baseline constraint
			const auto totalBaseline = std::accumulate(records.begin(), records.end(), 0.0f,
				[](float acc, const LayoutConstraintRecord& rec) {return acc + rec.constraints.basis; }
			);
			// if less than width, size baseline all non-grow, redistribute remaining to growers
			if (totalBaseline <= dimension)
			{
				decltype(records) growers;
				float growTotal = 0.f;
				for (const auto& record : records)
				{
					if (record.constraints.flexGrow == 0.f)
					{
						record.pChild->SetDimension(record.constraints.basis, setdir, sp, gfx);
					}
					else
					{
						growers.push_back(record);
						growTotal += record.constraints.flexGrow;
					}
				}
				const float surplus = dimension - totalBaseline;
				for (const auto& grower : growers)
				{
					grower.pChild->SetDimension(grower.constraints.basis + surplus * (grower.constraints.flexGrow / growTotal), setdir, sp, gfx);
				}
			}
			else // if greater than available width, try shrinking auto
			{
				// calculate "distance" to bottoming out for each child
				for (auto& record : records)
				{
					// "distance" is ratio of available shrink space over shrink rate
					record.distToBottom = (record.constraints.basis - record.constraints.min) /
						(record.constraints.basis / totalBaseline);
				}
				// sort by distance to bottoming out
				rn::sort(records, {}, &LayoutConstraintRecord::distToBottom);
				// loop and shrink by ratio, size, keeping track of remaining
				auto shortfallRemaining = totalBaseline - dimension;
				auto totalBaselineRemaining = totalBaseline;
				for (const auto& record : records)
				{
					const auto maxShrink = (shortfallRemaining / totalBaselineRemaining) * record.constraints.basis;
					const auto actualShrink = std::min(maxShrink, record.constraints.basis - record.constraints.min);
					record.pChild->SetDimension(record.constraints.basis - actualShrink, setdir, sp, gfx);
					totalBaselineRemaining -= record.constraints.basis;
					shortfallRemaining -= actualShrink;
				}
			}
		}
		else // container crosses setting direction
		{
			for (auto& pChild : GetChildren())
			{
				const auto crossConstraints = pChild->QueryLayoutConstraints(setdir, sp, gfx);
				// TODO: respect min?
				const auto fitSized = std::min(crossConstraints.basis, dimension);
				const auto stretchSized = std::min(crossConstraints.max, dimension);
				pChild->SetDimension(flexAlignment == FlexAlignment::Stretch ? stretchSized : fitSized, setdir, sp, gfx);
			}
		}
	}

	// called to finalize the position of each child
	void FlexElement::SetPosition_(const Vec2& pos, const Dimensions& dimensions, sty::StyleProcessor& sp, Graphics& gfx)
	{
		// function to calculate alignment in the cross direction
		const auto Align = [](FlexAlignment align, float start, float end, float size) -> float
		{
			using enum FlexAlignment;
			switch (align)
			{
			case Stretch:
			case Start:
				return start;
			case End:
				return end - size;
			case Center:
				return CalculateCenteredLeadingEdge(start, end, size);
			default:
				pmlog_error("bad flex alignment");
				throw Except<Exception>();
			}
		};

		const auto flexDirection = sp.Resolve<sty::at::flexDirection>();
		const auto flexJustification = sp.Resolve<sty::at::flexJustification>();
		const auto flexAlignment = sp.Resolve<sty::at::flexAlignment>();

		const auto containerRect = Rect{ pos, dimensions };
		const auto crossDirection = map::CrossDir(flexDirection);

		auto inlinePos = map::Vec2Scalar(pos, flexDirection);

		auto childrenPtrs = GetChildren();

		// get the total inline space so we can decide how to distribute it
		// sum of total bases and sum of implicit/shrinkable
		const auto totalBasis = std::accumulate(childrenPtrs.begin(), childrenPtrs.end(), 0.0f,
			[dir = flexDirection](float acc, const auto& pChild) {return acc + map::DimsScalar(pChild->GetElementDims(), dir); }
		);
		// free space is container size - totalBasis
		const auto space = map::DimsScalar(dimensions, flexDirection) - totalBasis;

		auto pSpacer = SpaceDistributor::Make(flexJustification, space, std::size(childrenPtrs));

		for (auto& pChild : childrenPtrs)
		{
			// pre-element spacing
			inlinePos += pSpacer->Pre();
			// element positioning
			const auto dims = pChild->GetElementDims();
			const auto crossPos = Align(
				flexAlignment,
				map::RectNearScalar(containerRect, crossDirection),
				map::RectFarScalar(containerRect, crossDirection),
				map::DimsScalar(dims, crossDirection)
			);
			pChild->SetPosition(map::MakeVec2(inlinePos, crossPos, flexDirection), sp, gfx);
			inlinePos += map::DimsScalar(dims, flexDirection);
			// post-element spacing
			inlinePos += pSpacer->Post();
		}
	}

	void FlexElement::Draw_(Graphics& gfx) const
	{
		for (auto& p : GetChildren())
		{
			p->Draw(gfx);
		}
	}


	// Space distribution implementations
	class SpaceDistributorStart : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
	protected:
		float Pre_() override
		{
			return 0.f;
		}
		float Post_() override
		{
			return 0.f;
		}
	};

	class SpaceDistributorEnd : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
		float Pre_() override
		{
			return iNext == 0 ? totalSpace : 0.f;
		}
		float Post_() override
		{
			return 0.f;
		}
	};

	class SpaceDistributorCenter : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
		float Pre_() override
		{
			return iNext == 0 ? totalSpace / 2.0f : 0.f;
		}
		float Post_() override
		{
			return iNext == count - 1 ? totalSpace / 2.0f : 0.f;
		}
	};

	class SpaceDistributorBetween : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
		float Pre_() override
		{
			return iNext != 0 ? totalSpace / float(count - 1) : 0.f;
		}
		float Post_() override
		{
			return 0.f;
		}
	};

	class SpaceDistributorAround : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
		float Pre_() override
		{
			return totalSpace / (float(count) * 2.0f);
		}
		float Post_() override
		{
			return totalSpace / (float(count) * 2.0f);
		}
	};

	class SpaceDistributorEven : public SpaceDistributor
	{
	public:
		using SpaceDistributor::SpaceDistributor;
		float Pre_() override
		{
			return iNext == 0 ? totalSpace / float(count + 1) : 0.f;
		}
		float Post_() override
		{
			return totalSpace / float(count + 1);
		}
	};

	std::unique_ptr<SpaceDistributor> SpaceDistributor::Make(FlexJustification justi, float space, size_t count)
	{
		using enum FlexJustification;

		if (space <= 0.0f && justi != Start && justi != End)
		{
			return std::make_unique<SpaceDistributorStart>(space, count);
		}

		switch (justi)
		{
		case Start: return std::make_unique<SpaceDistributorStart>(space, count);
		case End: return std::make_unique<SpaceDistributorEnd>(space, count);
		case Center: return std::make_unique<SpaceDistributorCenter>(space, count);
		case Between: return std::make_unique<SpaceDistributorBetween>(space, count);
		case Around: return std::make_unique<SpaceDistributorAround>(space, count);
		case Even: return std::make_unique<SpaceDistributorEven>(space, count);
		default:
			pmlog_error("unimplemented space distro for justification");
			throw Except<Exception>();
		}
	}
}