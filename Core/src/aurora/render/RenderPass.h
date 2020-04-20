#pragma once

#include "aurora/Material.h"
#include "aurora/render/RenderPriority.h"
#include "aurora/render/RenderState.h"
#include "aurora/render/RenderTag.h"
//#include <map>
#include <set>

namespace aurora::render {
	class AE_DLL RenderPass : public Ref {
	public:
		RenderPriority priority;
		RefPtr<RenderState> state;
		RefPtr<Material> material;

		std::set<RenderTag, RenderTag::std_compare> tags;
		std::vector<RefPtr<RenderPass>> subPasses;
		//std::map<RenderTag, RefPtr<RenderPass>, RenderTag::std_compare> subPasses;
	};
}