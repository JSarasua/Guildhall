#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"

class RenderContext;
class GPUMesh;	

class UIManager
{
public:
	UIManager( Vec2 screenDimensions, RenderContext* contextToUse );
	~UIManager();


	void Startup();
	void Render();
	void Update( float deltaSeconds );
	Widget* GetRootWidget() { return m_rootWidget; }
	GPUMesh* GetUIMesh() { return m_UIMesh; }
	AABB2 GetScreenBounds() const { return m_screenBounds; }

private:
	RenderContext* m_context = nullptr;
	Camera m_camera;
	AABB2 m_screenBounds;


	Widget* m_rootWidget = nullptr;
	GPUMesh* m_UIMesh = nullptr;
};