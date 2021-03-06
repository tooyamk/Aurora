#pragma once

#include "aurora/Shader.h"
#include "aurora/ShaderDefine.h"
#include "aurora/ShaderParameter.h"

namespace aurora {
	class AE_FW_DLL Material : public Ref {
	public:
		Material();

		inline const IntrusivePtr<Shader> AE_CALL getShader() const {
			return _shader;
		}

		inline IntrusivePtr<Shader> AE_CALL getShader() {
			return _shader;
		}

		inline void AE_CALL setShader(Shader* shader) {
			_shader = shader;
		}

		inline ShaderDefineCollection* AE_CALL getDefines() const {
			return _defines;
		}
		inline void AE_CALL setDefines(ShaderDefineCollection* defines) {
			_defines = defines;
		}

		inline ShaderParameterCollection* AE_CALL getParameters() const {
			return _parameters;
		}
		inline void AE_CALL setParameters(ShaderParameterCollection* parameters) {
			_parameters = parameters;
		}

	protected:
		IntrusivePtr<Shader> _shader;
		IntrusivePtr<ShaderDefineCollection> _defines;
		IntrusivePtr<ShaderParameterCollection> _parameters;
	};
}