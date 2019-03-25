#pragma once

#include "modules/graphics/GObject.h"

namespace aurora::modules::graphics {
	class GraphicsModule;

	class AE_DLL Program : public GObject {
	public:
		virtual ~Program();

	protected:
		Program(GraphicsModule& graphics);
	};
}