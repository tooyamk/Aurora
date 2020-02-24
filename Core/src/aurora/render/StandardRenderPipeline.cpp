#include "StandardRenderPipeline.h"
#include "aurora/Node.h"
#include "aurora/ShaderPredefine.h"
#include "aurora/components/Camera.h"
#include "aurora/components/IRenderable.h"
#include "aurora/render/IRenderer.h"
#include <algorithm>

namespace aurora::render {
	StandardRenderPipeline::RenderDataCollector::RenderDataCollector(StandardRenderPipeline& pipeline) :
		_pipeline(pipeline) {
	}

	void StandardRenderPipeline::RenderDataCollector::commit() {
		_pipeline._appendRenderData(*this);
	}


	StandardRenderPipeline::StandardRenderPipeline() :
		_renderDataPoolVernier(0),
		_m34_w2v(new ShaderParameter()),
		_m44_w2p(new ShaderParameter()),
		_shaderParameters(new ShaderParameterCollection()),
		_shaderDefineStack(new ShaderDefineGetterStack()),
		_shaderParameterStack(new ShaderParameterGetterStack()) {
		_shaderParameters->set(ShaderPredefine::MATRIX_WV, _m34_w2v);
		_shaderParameters->set(ShaderPredefine::MATRIX_WP, _m44_w2p);
	}

	void StandardRenderPipeline::render(modules::graphics::IGraphicsModule* graphics, components::Camera* camera, Node* node) {
		if (graphics && camera && node && camera->getNode() && camera->getNode()->getRoot() == node->getRoot()) {
			RenderDataCollector collector(*this);
			collector.matrix.w2v = camera->getNode()->getInverseWorldMatrix();
			collector.matrix.w2v.append(camera->getProjectionMatrix(), collector.matrix.w2p);

			_collectNode(node, collector);

			if (!_renderQueue.empty()) {
				_m34_w2v->set(collector.matrix.w2v);
				_m44_w2p->set(collector.matrix.w2p);

				std::stable_sort(_renderQueue.begin(), _renderQueue.end(), [](const RenderData* lhs, const RenderData* rhs) {
					if (lhs->priority.level1 == rhs->priority.level1) {
						using Lv2_t = std::underlying_type<RenderPriority::Level2>::type;
						auto val = (Lv2_t)lhs->priority.level2 - (Lv2_t)rhs->priority.level2;
						if (val == 0) {
							//todo
							return false;
						} else {
							return val < 0;
						}
					} else {
						return lhs->priority.level1 < rhs->priority.level1;
					}
				});

				camera->getProjectionMatrix();

				graphics->beginRender();
				graphics->clear(camera->clearFlag, camera->clearColor, camera->clearDepthValue, camera->clearStencilValue);

				_render();

				graphics->endRender();
			}
		}
	}

	void StandardRenderPipeline::_collectNode(Node* node, RenderDataCollector& collector) {
		auto& components = node->getComponents();
		for (auto& c : components) {
			if (c->isKindOf<components::IRenderable>()) {
				auto r = (const components::IRenderable*)c;
				if (r->getRenderer()) {
					collector.data.renderable = r;
					r->getRenderer()->collectRenderData(collector);
				}
			}
		}

		for (auto& child : *node) _collectNode(child, collector);
	}

	void StandardRenderPipeline::_appendRenderData(RenderDataCollector& collector) {
		if (collector.data.renderer->collectRenderDataConfirm(collector)) {
			RenderData* data;
			if (_renderDataPoolVernier) {
				data = _renderDataPool[--_renderDataPoolVernier];
				data->reset();
			} else {
				data = new RenderData();
				_renderDataPool.emplace_back(data);
			}

			_renderQueue.emplace_back(data);
			data->set(collector.data);

			data->matrix.l2w = data->renderable->getNode()->getWorldMatrix();
			data->matrix.l2w.append(collector.matrix.w2v, data->matrix.l2v);
			data->matrix.l2w.append(collector.matrix.w2p, data->matrix.l2p);
		}
	}

	void StandardRenderPipeline::_render() {
		if (auto size = _renderQueue.size(); size) {
			_shaderParameterStack->push(*_shaderParameters);

			auto renderer = _renderQueue[0]->renderer;
			size_t begin = 0;
			for (size_t i = 1; i < size; ++i) {
				auto& data = _renderQueue[i];
				if (data->renderer != renderer) {
					if (renderer) renderer->render(_renderQueue.data() + begin, i - begin, *_shaderDefineStack, *_shaderParameterStack);
					renderer = data->renderer;
					begin = i;
				}
			}

			if (renderer) renderer->render(_renderQueue.data() + begin, size - begin, *_shaderDefineStack, *_shaderParameterStack);

			_renderQueue.clear();
			_renderDataPoolVernier = _renderDataPool.size();

			_shaderParameterStack->pop();
		}
	}
}