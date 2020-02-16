#include "ForwardRenderer.h"
#include "aurora/Mesh.h"
#include "aurora/StackPopper.h"
#include "aurora/components/IRenderable.h"
#include "aurora/render/IRenderDataCollector.h"

namespace aurora::render {
	ForwardRenderer::ForwardRenderer(modules::graphics::IGraphicsModule& graphics) :
		_graphics(graphics),
		_defaultBlendState(graphics.createBlendState()),
		_defaultDepthStencilState(graphics.createDepthStencilState()),
		_defaultRasterizerState(graphics.createRasterizerState()) {
	}

	/*
	bool ForwardRenderer::checkValidity(const components::IRenderable& renderable) const {
		for (auto& m : renderable.materials) {
			if (m) return true;
		}
		return false;
	}
	*/

	void ForwardRenderer::collectRenderData(IRenderDataCollector& collector) const {
		collector.data.renderer = this;

		auto renderable = collector.data.renderable;
		for (auto& pass : renderable->renderPasses) {
			if (pass && pass->material) {
				collector.data.priority = pass->priority;
				collector.data.state = pass->state;
				collector.data.material = pass->material;

				renderable->collectRenderData(collector);

				collector.data.priority.reset();
				collector.data.state = nullptr;
				collector.data.material = nullptr;
			}
		}

		collector.data.renderer = nullptr;
	}

	bool ForwardRenderer::collectRenderDataConfirm(IRenderDataCollector& collector) const {
		auto mesh = collector.data.mesh;
		return mesh && !mesh->getVertexBuffers().isEmpty() && mesh->getIndexBuffer();
	}

	void ForwardRenderer::render(RenderData*const* data, size_t count, ShaderDefineGetterStack& shaderDefineStack, ShaderParameterGetterStack& shaderParameterStack) const {
		for (size_t i = 0; i < count; ++i) {
			auto rd = data[i];

			auto material = rd->material;
			if(!material) continue;

			auto shader = rd->material->getShader();
			if (!shader) continue;

			modules::graphics::IProgram* program;

			{
				StackPopper<ShaderDefineGetterStack, StackPopperFlag::CHECK_POP> popper(shaderDefineStack, shaderDefineStack.push(rd->material->getDefines()));

				program = shader->select(&shaderDefineStack);
				if (!program) continue;
			}

			if (rd->state) {
				{
					auto& bs = rd->state->blend;
					_graphics->setBlendState(bs.state ? bs.state : _defaultBlendState, bs.constantFactors);
				}

				{
					auto& ds = rd->state->depthStencil;
					_graphics->setDepthStencilState(ds.state ? ds.state : _defaultDepthStencilState, ds.stencilFrontRef, ds.stencilBackRef);
				}

				{
					auto& rs = rd->state->rasterizer;
					_graphics->setRasterizerState(rs.state ? rs.state : _defaultRasterizerState);
				}
			} else {
				_graphics->setBlendState(_defaultBlendState, Vec4f32::ZERO);
				_graphics->setDepthStencilState(_defaultDepthStencilState, 0);
				_graphics->setRasterizerState(_defaultRasterizerState);
			}

			StackPopper<ShaderParameterGetterStack, StackPopperFlag::CHECK_POP> popper(shaderParameterStack, shaderParameterStack.push(rd->material->getParameters()));

			_graphics->draw(&rd->mesh->getVertexBuffers(), program, &shaderParameterStack, rd->mesh->getIndexBuffer());
		}
	}
}