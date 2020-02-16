#pragma once

#include "../BaseTester.h"

class RenderPipelineTester : public BaseTester {
public:
	virtual int32_t AE_CALL run() override {
		auto monitors = Monitor::getMonitors();
		auto vms = monitors[0].getVideoModes();

		RefPtr app = new Application(u8"TestApp", 1000. / 60.);

		Application::Style wndStype;
		wndStype.thickFrame = true;
		if (app->createWindow(wndStype, u8"", Box2i32(Vec2i32({ 100, 100 }), Vec2i32({ 800, 600 })), false)) {
			RefPtr gml = new GraphicsModuleLoader();

			//if (gml->load(getDLLName("ae-win-gl"))) {
			if (gml->load(getDLLName("ae-win-d3d11"))) {
				auto gpstml = new ModuleLoader<IProgramSourceTranslator>();
				gpstml->load(getDLLName("ae-program-source-translator"));
				auto gpst = gpstml->create(&Args().add("dxc", getDLLName("dxcompiler")));
				if (gpst) gpst->ref();

				RefPtr graphics = gml->create(&Args().add("app", &*app).add("sampleCount", SampleCount(4)).add("trans", gpst));

				if (graphics) {
					println("Graphics Version : ", graphics->getVersion());

					graphics->getEventDispatcher().addEventListener(GraphicsEvent::ERR, new EventListener(Recognitor<GraphicsEvent>(),[](Event<GraphicsEvent>& e) {
						println(*(std::string_view*)e.getData());
						int a = 1;
					}));

					struct {
						RefPtr<IGraphicsModule> g;
						RefPtr<Node> wrold;
						RefPtr<Material> material;
						RefPtr<StandardRenderPipeline> renderPipeline;
					} renderData;
					renderData.g = graphics;

					{
						RefPtr worldNode = new Node();
						auto modelNode = worldNode->addChild(new Node());
						auto cameraNode = worldNode->addChild(new Node());
						auto camera = new Camera();
						cameraNode->addComponent(camera);
						auto renderableMesh = new RenderableMesh();
						auto pass = new RenderPass();
						auto mat = new Material();
						renderData.material = mat;
						pass->material = mat;
						renderableMesh->renderPasses.emplace_back(pass);
						renderableMesh->renderPasses.emplace_back(pass);
						auto mesh = new Mesh();
						renderableMesh->setMesh(mesh);
						modelNode->addComponent(renderableMesh);

						{
							//auto vertexBuffer = graphics->createVertexBuffer();
							auto vertexBuffer = new MultipleVertexBuffer(*graphics, 3);
							if (vertexBuffer) {
								/*
								f32 vertices[] = {
									0.0f, 0.0f,
									0.0f, 0.5f,
									0.8f, 0.0f };
								vb->stroage(sizeof(vertices), vertices);
								vb->setFormat(VertexSize::TWO, VertexType::F32);
								*/
								///*
								f32 vertices[] = {
									-0.5f, -0.5f, .0f,
									-0.5f, 0.5f, .0f,
									0.45f, 0.45f, 0.2f,
									0.45f, -0.5f, .0f,

									0.0f, 0.0f, 0.5f,
									0.0f, 1.0f, 0.5f,
									1.0f, 1.0f, 0.5f,
									1.0f, 0.0f, 0.5f };
								vertexBuffer->create(sizeof(vertices), Usage::NONE, vertices, sizeof(vertices));
								vertexBuffer->setFormat(VertexSize::THREE, VertexType::F32);
							}

							auto uvBuffer = graphics->createVertexBuffer();
							if (uvBuffer) {
								f32 uvs[] = {
									0.f, 1.f,
									0.f, 0.f,
									1.f, 0.f,
									1.f, 1.f,

									0.f, 1.f,
									0.f, 0.f,
									1.f, 0.f,
									1.f, 1.f };
								uvBuffer->create(sizeof(uvs), Usage::NONE, uvs, sizeof(uvs));
								uvBuffer->setFormat(VertexSize::TWO, VertexType::F32);
							}

							mesh->getVertexBuffers().add("POSITION0", vertexBuffer);
							mesh->getVertexBuffers().add("TEXCOORD0", uvBuffer);
						}

						{
							auto ib = graphics->createIndexBuffer();
							uint16_t indices[] = {
									0, 1, 3,
									3, 1, 2,

									4, 5, 7,
									7, 5, 6 };
							ib->create(sizeof(indices), Usage::NONE, indices, sizeof(indices));
							ib->setFormat(IndexType::UI16);

							mesh->setIndexBuffer(ib);
						}

						{
							RefPtr s = new Shader();
							mat->setShader(s);
							std::string shaderResourcesFolder = app->getAppPath() + u8"Resources/shaders/";
							//s->upload(std::filesystem::path(app->getAppPath() + u8"Resources/shaders/test.shader"));
							s->upload(graphics,
								new ProgramSource(readProgramSource(shaderResourcesFolder + "vert.hlsl", ProgramStage::VS)),
								new ProgramSource(readProgramSource(shaderResourcesFolder + "frag.hlsl", ProgramStage::PS)),
								nullptr, 0, nullptr, 0,
								[&shaderResourcesFolder](const Shader& shader, ProgramStage stage, const std::string_view& name) {
								return readFile(shaderResourcesFolder + name.data());
							});

							{
								mat->getParameters().add("red", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
								mat->getParameters().add("green", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
								//cf->add("blue", new ShaderParameter())->set(Vec4f32::ONE).setUpdated();

								auto aabbccStruct = new ShaderParameterCollection();
								aabbccStruct->add("val1", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set(Vec4f32::ONE).setUpdated();
								f32 val2[] = { 1.0f, 1.0f };
								aabbccStruct->add("val2", new ShaderParameter(ShaderParameterUsage::EXCLUSIVE))->set<f32>(val2, sizeof(val2), sizeof(f32), true).setUpdated();
								aabbccStruct->add("val3", new ShaderParameter())->set(Vec4f32::ONE).setUpdated();
								mat->getParameters().add("blue", new ShaderParameter())->set(aabbccStruct);
							}
						}

						auto renderer = new ForwardRenderer(*graphics);
						renderableMesh->setRenderer(renderer);

						renderData.wrold = worldNode;
						renderData.renderPipeline = new StandardRenderPipeline();
					}

					{
						RefPtr<IRasterizerState> rs = graphics->createRasterizerState();
						rs->setFillMode(FillMode::SOLID);
						rs->setFrontFace(FrontFace::CW);
						rs->setCullMode(CullMode::NONE);
						graphics->setRasterizerState(&*rs);
					}

					{
						auto texRes = graphics->createTexture2DResource();
						if (texRes) {
							auto img0 = extensions::file::PNGConverter::parse(readFile(app->getAppPath() + u8"Resources/c4.png"));
							auto mipLevels = Image::calcMipLevels(img0->size);
							ByteArray mipsData0;
							std::vector<void*> mipsData0Ptr;
							img0->generateMips(img0->format, mipLevels, mipsData0, mipsData0Ptr);

							auto img1 = extensions::file::PNGConverter::parse(readFile(app->getAppPath() + u8"Resources/red.png"));
							ByteArray mipsData1;
							std::vector<void*> mipsData1Ptr;
							img1->generateMips(img1->format, mipLevels, mipsData1, mipsData1Ptr);

							mipsData0Ptr.insert(mipsData0Ptr.end(), mipsData1Ptr.begin(), mipsData1Ptr.end());

							auto hr = texRes->create(img0->size, 0, 1, 1, img0->format, Usage::IGNORE_UNSUPPORTED | Usage::MAP_WRITE, mipsData0Ptr.data());

							auto texView = graphics->createTextureView();
							texView->create(texRes, 0, -1, 0, -1);

							auto pb = graphics->createPixelBuffer();
							if (pb) {
								//pb->create(img0->size.getMultiplies() * 4, Usage::MAP_WRITE | Usage::PERSISTENT_MAP, mipsData0Ptr.data()[0], img0->size.getMultiplies() * 4);
								//texRes->copyFrom(0, 0, Box2ui32(Vec2ui32(0, 0), Vec2ui32(4, 4)), pb);
							}

							uint8_t texData[] = { 255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 0, 0, 0, 255 };
							//auto mapped = texRes->map(0, 0, Usage::MAP_WRITE);
							//texRes->write(0, 0, 4, texData, sizeof(texData));
							//texRes->unmap(0, 0);
							//texRes->update(0, 0, Box2ui32(Vec2ui32(1, 1), Vec2ui32(2, 2)), texData);

							renderData.material->getParameters().add("texDiffuse", new ShaderParameter(ShaderParameterUsage::AUTO))->set(texView).setUpdated();
						}

						auto sam = graphics->createSampler();
						if (sam) {
							//sam->setMipLOD(0, 0);
							//sam->setAddress(SamplerAddressMode::WRAP, SamplerAddressMode::WRAP, SamplerAddressMode::WRAP);
							sam->setFilter(SamplerFilterOperation::NORMAL, SamplerFilterMode::POINT, SamplerFilterMode::POINT, SamplerFilterMode::POINT);
							renderData.material->getParameters().add("samLiner", new ShaderParameter(ShaderParameterUsage::AUTO))->set(sam).setUpdated();
						}
					}

					RefPtr<IEventListener<ApplicationEvent>> appClosingListener = new EventListener(std::function([](Event<ApplicationEvent>& e) {
						//*e.getData<bool>() = true;
					}));

					auto& evtDispatcher = app->getEventDispatcher();

					evtDispatcher.addEventListener(ApplicationEvent::TICKING, new EventListener(std::function([renderData](Event<ApplicationEvent>& e) {
						auto app = e.getTarget<Application>();

						auto dt = f64(*e.getData<int64_t>());

						renderData.g->beginRender();
						renderData.g->clear(ClearFlag::COLOR | ClearFlag::DEPTH | ClearFlag::STENCIL, Vec4f32(0.0f, 0.0f, 0.25f, 1.0f), 1.0f, 0);

						renderData.renderPipeline->render(renderData.wrold);

						renderData.g->endRender();
						renderData.g->present();
					})));

					evtDispatcher.addEventListener(ApplicationEvent::CLOSING, *appClosingListener);

					(new Stats())->run(app);
					app->setVisible(true);
					app->run(true);
				}
			}
		}

		return 0;
	}

private:
	RefPtr<Node> _worldRoot;
};