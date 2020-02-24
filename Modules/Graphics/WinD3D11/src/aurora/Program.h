#pragma once

#include "ConstantBuffer.h"
#include "Sampler.h"
#include "TextureView.h"
#include "aurora/modules/graphics/ConstantBufferManager.h"

namespace aurora::modules::graphics::win_d3d11 {
	class AE_MODULE_DLL Program : public IProgram {
	public:
		Program(Graphics& graphics);
		virtual ~Program();

		virtual const void* AE_CALL getNative() const override;
		virtual bool AE_CALL create(const ProgramSource& vert, const ProgramSource& frag, const ShaderDefine* defines, size_t numDefines, const IncludeHandler& handler) override;
		virtual void AE_CALL destroy() override;

		bool AE_CALL use(const IVertexBufferGetter* vertexBufferGetter, const IShaderParameterGetter* shaderParamGetter);
		void AE_CALL useEnd();

	protected:
		class MyIncludeHandler : public ID3DInclude {
		public:
			MyIncludeHandler(const IProgram& program, ProgramStage stage, const IncludeHandler& handler);

			HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
			HRESULT Close(LPCVOID pData);

		private:
			ProgramStage _stage;
			ByteArray _data;
			const IProgram& _program;
			const IncludeHandler& _handler;
		};


		class MyConstantBufferLayout : public ConstantBufferLayout {
		public:
			MyConstantBufferLayout() : sameId(0) {}

			uint32_t sameId;
		};


		struct InVertexBufferInfo {
			InVertexBufferInfo() :
				name(),
				slot(0) {
			}

			std::string name;
			uint32_t slot;
		};


		struct InLayout {
			InLayout (uint32_t numInElements);
			~InLayout();

			std::vector<uint32_t> formats;
			ID3D11InputLayout* layout;

			bool isEqual(const D3D11_INPUT_ELEMENT_DESC* inElements, uint32_t num) const;
		};


		ID3DBlob* _vertBlob;
		ID3D11VertexShader* _vs;
		ID3D11PixelShader* _ps;

		ID3D11InputLayout* _curInLayout;
		D3D11_INPUT_ELEMENT_DESC* _inElements;
		uint32_t _numInElements;

		std::vector<InVertexBufferInfo> _inVerBufInfos;
		std::vector<InLayout> _inLayouts;


		struct TextureLayout {
			std::string name;
			uint32_t bindPoint;
		};


		struct SamplerLayout {
			std::string name;
			uint32_t bindPoint;
		};


		struct ParameterLayout {
			std::vector<MyConstantBufferLayout> constantBuffers;
			std::vector<TextureLayout> textures;
			std::vector<SamplerLayout> samplers;

			void clear(Graphics& g);
		};


		ParameterLayout _vsParamLayout;
		ParameterLayout _psParamLayout;

		std::vector<ConstantBuffer*> _usingSameConstBuffers;
		std::vector<ShaderParameter*> _tempParams;
		std::vector<const ConstantBufferLayout::Variables*> _tempVars;

		ID3DBlob* AE_CALL _compileShader(const ProgramSource& source, ProgramStage stage, const std::string_view& target, const D3D_SHADER_MACRO* defines, const IncludeHandler& handler);
		ID3D11InputLayout* _getOrCreateInputLayout();
		void AE_CALL _parseInLayout(const D3D11_SHADER_DESC& desc, ID3D11ShaderReflection& ref);
		void AE_CALL _parseParameterLayout(const D3D11_SHADER_DESC& desc, ID3D11ShaderReflection& ref, ParameterLayout& dst);
		void AE_CALL _parseConstantVar(ConstantBufferLayout::Variables& var, ID3D11ShaderReflectionType* type);
		void AE_CALL _calcConstantLayoutSameBuffers(std::vector<std::vector<MyConstantBufferLayout>*>& constBufferLayouts);

		ConstantBuffer* _getConstantBuffer(const MyConstantBufferLayout& cbLayout, const IShaderParameterGetter& paramGetter);
		void _constantBufferUpdateAll(ConstantBuffer* cb, const std::vector<ConstantBufferLayout::Variables>& vars);

		template<ProgramStage Stage>
		void AE_CALL _useParameters(const ParameterLayout& layout, const IShaderParameterGetter& paramGetter) {
			auto g = _graphics.get<Graphics>();

			for (auto& info : layout.constantBuffers) {
				auto cb = _getConstantBuffer(info, paramGetter);
				if (cb && g == cb->getGraphics()) {
					if (auto native = (ConstantBuffer*)cb->getNative(); native) {
						if (auto buffer = native->getInternalBuffer(); buffer) g->useConstantBuffers<Stage>(info.bindPoint, 1, &buffer);
					}
				}
			}

			for (auto& info : layout.textures) {
				if (auto p = paramGetter.get(info.name, ShaderParameterType::TEXTURE); p) {
					if (auto data = p->getData(); data && g == ((ITextureView*)data)->getGraphics()) {
						if (auto native = (TextureView*)((ITextureView*)data)->getNative(); native) {
							auto iv = native->getInternalView();
							g->useShaderResources<Stage>(info.bindPoint, 1, &iv);
						}
					}
					//if (data) (BaseTexture*)(((ITexture*)data)->getNative())->use<stage>((Graphics*)_graphics, info.bindPoint);
				}
			}

			for (auto& info : layout.samplers) {
				if (auto p = paramGetter.get(info.name, ShaderParameterType::SAMPLER); p) {
					if (auto data = p->getData(); data && g == ((ISampler*)data)->getGraphics()) {
						if (auto native = (Sampler*)((ISampler*)data)->getNative(); native) {
							native->update();
							auto is = native->getInternalSampler();
							g->useSamplers<Stage>(info.bindPoint, 1, &is);
						}
					}
				}
			}
		}
	};
}