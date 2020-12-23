#pragma once

#include "../BaseTester.h"

class OffscreenTester : public BaseTester {
public:
	virtual int32_t AE_CALL run() override {
		auto monitors = Monitor::getMonitors();
		auto vms = monitors[0].getVideoModes();

		RefPtr app = new Application("TestApp");

		ApplicationStyle wndStype;
		wndStype.thickFrame = true;
		if (app->createWindow(wndStype, "", Vec2ui32(800, 600), false)) {
			RefPtr gml = new GraphicsModuleLoader();

			//if (gml->load(getDLLName("ae-graphics-gl"))) {
			if (gml->load("libs/" + getDLLName("ae-graphics-d3d11"))) {
				SerializableObject args;

				RefPtr gpstml = new ModuleLoader<IProgramSourceTranslator>();
				gpstml->load("libs/" + getDLLName("ae-program-source-translator"));

				args.insert("dxc", "libs/" + getDLLName("dxcompiler"));
				auto gpst = gpstml->create(&args);

				std::function<void(const std::string_view&)> createProcessInfoHandler = [](const std::string_view& msg) {
					printaln(msg);
				};

				args.insert("app", (uintptr_t)&*app);
				args.insert("sampleCount", 1);
				args.insert("trans", (uintptr_t)&*gpst);
				args.insert("offscreen", true);
				args.insert("driverType", "software");
				args.insert("createProcessInfoHandler", (uintptr_t)&createProcessInfoHandler);
#ifdef AE_DEBUG
				args.insert("debug", true);
#endif
				auto graphics = gml->create(&args);

				if (graphics) {
					printaln("Graphics Version : ", graphics->getVersion());

					graphics->getEventDispatcher().addEventListener(GraphicsEvent::ERR, createEventListener<GraphicsEvent>([](Event<GraphicsEvent>& e) {
						printaln(*(std::string_view*)e.getData());
						int a = 1;
					}));

					{
						{
							auto rs = graphics->createRasterizerState();
							rs->setFillMode(FillMode::SOLID);
							rs->setFrontFace(FrontFace::CW);
							rs->setCullMode(CullMode::NONE);
							graphics->setRasterizerState(rs);
						}

						auto tr = graphics->createTexture2DResource();
						auto rv = graphics->createRenderView();
						auto rt = graphics->createRenderTarget();
						tr->create(Vec2ui32(800, 600), 0, 1, 1, modules::graphics::TextureFormat::R8G8B8A8, modules::graphics::Usage::RENDERABLE);
						rv->create(tr, 0, 0, 0);
						rt->setRenderView(0, rv);

						auto program = graphics->createProgram();
						{
							const std::string _vs = R"(
struct VS_INPUT {
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.position = float4(input.position.x, input.position.y, 0.5f, 1.0f);
    output.uv = float2(input.uv.x, 1.0f - input.uv.y);
    return output;
}
)";
							const std::string _ps = R"(
struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET {
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

							ProgramSource vs;
							vs.language = ProgramLanguage::HLSL;
							vs.stage = ProgramStage::VS;
							vs.data = ByteArray((void*)_vs.data(), _vs.size(), ByteArray::Usage::SHARED);
							ProgramSource ps;
							ps.language = ProgramLanguage::HLSL;
							ps.stage = ProgramStage::VS;
							ps.data = ByteArray((void*)_ps.data(), _ps.size(), ByteArray::Usage::SHARED);
							program->create(vs, ps, nullptr, 0, nullptr);
						}

						RefPtr vertexBuffers = new VertexBufferCollection();
						{
							modules::graphics::VertexFormat vf;
							vf.set<2, float32_t>();

							float32_t vd[] = {
									-1.f, 1.f,
									1.f, 1.f,
									1.f, -1.f,
									-1.f, -1.f };
							auto vb = graphics->createVertexBuffer();
							vb->create(32, modules::graphics::Usage::NONE, vd, 32);
							vb->setFormat(vf);
							vertexBuffers->set("POSITION0", vb);

							float32_t uvd[] = {
									0.f, 0.f,
									1.f, 0.f,
									1.f, 1.f,
									0.f, 1.f };
							auto uvb = graphics->createVertexBuffer();
							uvb->create(32, modules::graphics::Usage::NONE, uvd, 32);
							uvb->setFormat(vf);
							vertexBuffers->set("TEXCOORD0", uvb);
						}

						RefPtr shaderParameters = new ShaderParameterCollection();
						{
							//shaderParameters->set("tex", new ShaderParameter(ShaderParameterUsage::AUTO))->set(tv, ShaderParameterUpdateBehavior::FORCE);
							//shaderParameters->set("texSampler", new ShaderParameter(ShaderParameterUsage::AUTO))->set(_pointTexSampler, ShaderParameterUpdateBehavior::FORCE);
						}

						auto indices = graphics->createIndexBuffer();
						{
							uint16_t id[] = {
									0, 1, 2,
									0, 2, 3 };
							indices->create(12, modules::graphics::Usage::NONE, id, 12);
							indices->setFormat<uint16_t>();
						}

						graphics->setRenderTarget(rt);
						graphics->setViewport(Box2i32ui32(Vec2i32::ZERO, tr->getSize().cast<2>()));
						graphics->beginRender();
						graphics->clear(ClearFlag::COLOR | ClearFlag::DEPTH | ClearFlag::STENCIL, Vec4f32(0.0f, 0.0f, 0.25f, 1.0f), 1.0f, 0);
						graphics->setBlendState(nullptr, Vec4f32::ZERO);
						graphics->setDepthStencilState(nullptr, 0);
						graphics->draw(vertexBuffers, program, shaderParameters, indices);
						graphics->setRenderTarget(nullptr);

						{
							auto tex = graphics->createTexture2DResource();
							if (tex->create(tr->getSize(), 0, 1, 1, tr->getFormat(), modules::graphics::Usage::MAP_READ)) {
								if (tex->copyFrom(Vec3ui32::ZERO, 0, 0, tr, 0, 0, Box3ui32(Vec3ui32::ZERO, tr->getSize()))) {
									if (tex->map(0, 0, modules::graphics::Usage::MAP_READ) != modules::graphics::Usage::NONE) {
										auto pixelsSize = tr->getSize().getMultiplies() * 4;
										ByteArray pixels(pixelsSize, pixelsSize);
										tex->read(0, 0, 0, pixels.getSource(), pixels.getLength());
										tex->unmap(0, 0);

										Image img;
										img.format = modules::graphics::TextureFormat::R8G8B8A8;
										img.size = tr->getSize().slice<2>();
										img.source = std::move(pixels);

										writeFile(getAppPath().parent_path().string() + "/offscreen.png", extensions::PNGConverter::encode(img));
									}
								}
							}
						}
					}

					app->getEventDispatcher().addEventListener(ApplicationEvent::CLOSING, new EventListener(std::function([](Event<ApplicationEvent>& e) {
						//*e.getData<bool>() = true;
					})));

					app->getEventDispatcher().addEventListener(ApplicationEvent::CLOSED, new EventListener(std::function([](Event<ApplicationEvent>& e) {
					})));

					RefPtr looper = new Looper(1000.0 / 60.0);

					looper->getEventDispatcher().addEventListener(LooperEvent::TICKING, new EventListener(std::function([](Event<LooperEvent>& e) {
						auto dt = float64_t(*e.getData<int64_t>());
					})));

					(new Stats())->run(looper);
					//app->setVisible(true);
					looper->run(true);
				}
			}
		}

		return 0;
	}
};