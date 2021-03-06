#pragma once

#include "aurora/render/commands/IRenderCommand.h"

namespace aurora::render::commands {
	template<typename T>
	class BaseRenderCommand : public IRenderCommand {
	protected:
		BaseRenderCommand() {
			_fn = &T::execute;
		}

		virtual ~BaseRenderCommand() {};
	};
}