#include "Program.h"
#include "Graphics.h"
#include "base/String.h"
#include <vector>

namespace aurora::modules::graphics::win_d3d11 {
	Program::InLayout::~InLayout() {
		delete[] formats;
		if (layout) layout->Release();
	}

	bool Program::InLayout::isEqual(const D3D11_INPUT_ELEMENT_DESC* inElements, ui32 num) const {
		for (ui32 i = 0, n = num; i < n; ++i) {
			if (formats[i] != inElements[i].Format) return false;
		}
		return true;
	}


	Program::Program(Graphics& graphics) : IProgram(graphics),
		_inElementsDirty(false),
		_vertBlob(nullptr),
		_vs(nullptr),
		_ps(nullptr),
		_inElements(nullptr),
		_numInElements(0),
		_curInLayout(nullptr),
		_numConstBuffers(0) {
		_cb = (ConstantBuffer*)_graphics->createConstantBuffer();
		f32 c[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		_cb->stroage(64, &c);
	}

	Program::~Program() {
		_release();
	}

	bool Program::upload(const ProgramSource& vert, const ProgramSource& frag) {
		_release();

		DXObjGuard objs;

		auto g = (Graphics*)_graphics;

		auto& sm = g->getSupportShaderModel();

		_vertBlob = _compileShader(vert, ProgramSource::toHLSLShaderModel(ProgramStage::VS, vert.version.empty() ? sm : vert.version).c_str());
		if (!_vertBlob) return false;

		auto pixelBlob = _compileShader(frag, ProgramSource::toHLSLShaderModel(ProgramStage::PS, frag.version.empty() ? sm : frag.version).c_str());
		if (!pixelBlob) {
			_release();
			return false;
		}
		objs.add(pixelBlob);

		auto device = g->getDevice();

		HRESULT hr = device->CreateVertexShader(_vertBlob->GetBufferPointer(), _vertBlob->GetBufferSize(), 0, &_vs);
		if (FAILED(hr)) {
			_release();
			return false;
		}

		ID3D11ShaderReflection* vsr = nullptr;
		if (FAILED(D3DReflect(_vertBlob->GetBufferPointer(), _vertBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vsr))) {
			_release();
			return false;
		}
		objs.add(vsr);

		D3D11_SHADER_DESC sDesc;
		vsr->GetDesc(&sDesc);
		_numInElements = sDesc.InputParameters;
		if (_numInElements > 0) {
			ui32 offset = 0;
			_inElements = new D3D11_INPUT_ELEMENT_DESC[sDesc.InputParameters];
			for (ui32 i = 0; i < _numInElements; ++i) {
				D3D11_SIGNATURE_PARAMETER_DESC spDesc;
				vsr->GetInputParameterDesc(i, &spDesc);

				auto& ieDesc = _inElements[i];
				ui32 len = strlen(spDesc.SemanticName);
				i8* name = new i8[len + 1];
				ieDesc.SemanticName = name;
				name[len] = 0;
				memcpy(name, spDesc.SemanticName, len);
				//ieDesc.SemanticName = spDesc.SemanticName;
				ieDesc.SemanticIndex = spDesc.SemanticIndex;
				ieDesc.InputSlot = i;
				ieDesc.AlignedByteOffset = 0;
				ieDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				ieDesc.InstanceDataStepRate = 0;

				auto& info = _inVerBufInfos.emplace_back();
				info.name = spDesc.SemanticName + String::toString(spDesc.SemanticIndex);
				info.slot = i;

				if (spDesc.Mask == 1) {
					if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
						ieDesc.Format = DXGI_FORMAT_R32_UINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
						ieDesc.Format = DXGI_FORMAT_R32_SINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
						ieDesc.Format = DXGI_FORMAT_R32_FLOAT;
					}
					offset += 4;
				} else if (spDesc.Mask <= 3) {
					if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32_UINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32_SINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
					}
					offset += 8;
				} else if (spDesc.Mask <= 7) {
					if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					}
					offset += 12;
				} else if (spDesc.Mask <= 15) {
					if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					} else if (spDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) {
						ieDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					}
					offset += 16;
				}

				if (i == 0) {
					ieDesc.Format = DXGI_FORMAT_R8G8_UINT;
				}
			}
		}

		_curInLayout = _getOrCreateInputLayout();
		_inElementsDirty = false;

		for (ui32 i = 0; i < sDesc.BoundResources; ++i) {
			D3D11_SHADER_INPUT_BIND_DESC desc;
			vsr->GetResourceBindingDesc(i, &desc);
			int a = 1;
		}

		_numConstBuffers = sDesc.ConstantBuffers;
		if (_numConstBuffers > 0) {
			for (ui32 i = 0; i < _numInElements; ++i) {
				auto cb = vsr->GetConstantBufferByIndex(i);
				D3D11_SHADER_BUFFER_DESC desc;
				cb->GetDesc(&desc);
				for (ui32 j = 0; j < desc.Variables; ++j) {
					auto var = cb->GetVariableByIndex(j);
					
					D3D11_SHADER_VARIABLE_DESC vDesc;
					var->GetDesc(&vDesc);
					auto type = var->GetType();
					int a = 1;
				}
			}
		}

		ID3D11PixelShader* fs = nullptr;
		if (FAILED(device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), 0, &_ps))) {
			_release();
			return false;
		}

		ID3D11ShaderReflection* psr = nullptr;
		if (FAILED(D3DReflect(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&psr))) {
			_release();
			return false;
		}
		objs.add(psr);

		psr->GetDesc(&sDesc);

		for (ui32 i = 0; i < sDesc.BoundResources; ++i) {
			D3D11_SHADER_INPUT_BIND_DESC desc;
			psr->GetResourceBindingDesc(i, &desc);
			int a = 1;
		}

		if (sDesc.ConstantBuffers > 0) {
			for (ui32 i = 0; i < sDesc.ConstantBuffers; ++i) {
				auto cb = psr->GetConstantBufferByIndex(i);
				D3D11_SHADER_BUFFER_DESC desc;
				cb->GetDesc(&desc);
				for (ui32 j = 0; j < desc.Variables; ++j) {
					auto var = cb->GetVariableByIndex(j);

					D3D11_SHADER_VARIABLE_DESC vDesc;
					var->GetDesc(&vDesc);
					auto type = var->GetType();
					int a = 1;
				}
			}
		}

		return true;
	}

	bool Program::use() {
		if (_vs) {
			auto context = ((Graphics*)_graphics)->getContext();

			context->VSSetShader(_vs, 0, 0);
			context->PSSetShader(_ps, 0, 0);

			return true;
		}
		return false;
	}

	void Program::useVertexBuffers(const VertexBufferFactory& factory) {
		for (ui32 i = 0, n = _inVerBufInfos.size(); i < n; ++i) {
			auto& info = _inVerBufInfos[i];
			auto vb = (VertexBuffer*)factory.get(info.name);
			if (vb) {
				auto& ie = _inElements[i];
				DXGI_FORMAT fmt;
				if (vb->use(info.slot, fmt)) {
					if (ie.Format != fmt) {
						_inElementsDirty = true;
						ie.Format = fmt;
					}
				}
			}
		}
	}

	void Program::draw(const IIndexBuffer& indexBuffer, ui32 count, ui32 offset) {
		if (_vs) {
			auto g = (Graphics*)_graphics;
			auto context = g->getContext();

			if (_inElementsDirty) {
				_inElementsDirty = false;

				_curInLayout = _getOrCreateInputLayout();
			}

			if (_curInLayout) {
				_cb->use(0);

				context->IASetInputLayout(_curInLayout);
				((IndexBuffer&)indexBuffer).draw(count, offset);
			}
		}
	}

	void Program::_release() {
		_curInLayout = nullptr;
		_inVerBufInfos.clear();
		_inLayouts.clear();

		if (_numInElements > 0) {
			for (ui32 i = 0; i < _numInElements; ++i) delete[] _inElements[i].SemanticName;
			delete[] _inElements;
			_numInElements = 0;
		}

		if (_curInLayout) {
			_curInLayout->Release();
			_curInLayout = nullptr;
		}

		_inElementsDirty = false;

		if (_vs) {
			_vs->Release();
			_vs = nullptr;
		}

		if (_ps) {
			_ps->Release();
			_ps = nullptr;
		}

		if (_vertBlob) {
			_vertBlob->Release();
			_vertBlob = nullptr;
		}
	}

	ID3DBlob* Program::_compileShader(const ProgramSource& source, const i8* target) {
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef AE_DEBUG
		shaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

		ID3DBlob* buffer = nullptr, *errorBuffer = nullptr;
		HRESULT hr = D3DCompile(source.data.getBytes(), source.data.getLength(), nullptr, nullptr, nullptr, 
			ProgramSource::getEntryPoint(source).c_str(), target, shaderFlags, 0, &buffer, &errorBuffer);

		if (FAILED(hr)) {
			if (errorBuffer) {
				println("d3d11 compile shader err : %s", errorBuffer->GetBufferPointer());
				errorBuffer->Release();
			}

			return nullptr;
		} else if (errorBuffer) {
			errorBuffer->Release();
		}

		return buffer;
	}

	ID3D11InputLayout* Program::_getOrCreateInputLayout() {
		for (ui32 i = 0, n = _inLayouts.size(); i < n; ++i) {
			auto& il = _inLayouts[i];
			if (il.isEqual(_inElements, _numInElements)) return il.layout;
		}

		auto& il = _inLayouts.emplace_back();
		il.formats = new ui32[_numInElements];
		for (ui32 i = 0; i < _numInElements; ++i) il.formats[i] = _inElements[i].Format;
		auto hr = ((Graphics*)_graphics)->getDevice()->CreateInputLayout(_inElements, _numInElements, _vertBlob->GetBufferPointer(), _vertBlob->GetBufferSize(), &il.layout);
		return il.layout;
	}
}