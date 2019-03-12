#pragma once

#include "modules/GraphicsModule.h"
#include "math/Rect.h"

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

AE_NS_BEGIN

class AE_MODULE_DLL GraphicsWinD3D9 : public GraphicsModule {
public:
	GraphicsWinD3D9();

	virtual void AE_CALL createView(void* style, const i8* windowTitle, const Rect<i32>& rect, bool fullscreen, f32 fps);
	virtual void AE_CALL setFPS(f32 fps);
	virtual bool AE_CALL isWindowed() const;
	virtual void AE_CALL toggleFullscreen();
	virtual void AE_CALL getViewRect(Rect<i32>& dst) const;
	virtual void AE_CALL setViewRect(const Rect<i32>& rect);
	virtual void AE_CALL shutdown();

private:
	bool AE_CALL _init(HWND hWnd);
	void AE_CALL _toggleFullscreen();

	PDIRECT3D9 _d3d; // D3D对象
	PDIRECT3DDEVICE9 _d3dDevice; // D3D设备对象
	D3DPRESENT_PARAMETERS _d3dpp;
	f32 _tpf; //timePreFrame
	Rect<i32> _rect;
};

AE_NS_END

#ifdef AE_MODULE_EXPORTS
extern "C" AE_MODULE_DLL_EXPORT void* createModule() {
	return new AE_NS::GraphicsWinD3D9();
}
#endif