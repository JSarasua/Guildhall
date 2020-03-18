#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"


static RenderContext*	s_DebugRenderContext = nullptr;
static Camera*			s_DebugCamera = nullptr;

class DebugRenderSystem
{
	RenderContext* m_context;
	Camera* m_camera;
	//Other stuff
};

static DebugRenderSystem* s_DebugRenderSystem = nullptr;

struct DebugRenderObject
{
public:
	//array of points?
	//array of colors?

	bool m_isReadyToBeCulled = false;
};

/************************************************************************/
/* Methods                                                                     */
/************************************************************************/
