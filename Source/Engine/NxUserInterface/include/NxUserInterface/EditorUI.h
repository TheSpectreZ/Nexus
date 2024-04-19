#pragma once

#ifdef NEXUS_USER_INTERFACE_SHARED_BUILD
#define NEXUS_UI_API __declspec(dllexport)
#else 
#define NEXUS_UI_API __declspec(dllimport)
#endif // NEXUS_USER_INTERFACE_SHARED_BUILD

namespace Nexus
{
	class NEXUS_UI_API EditorUIDrawer
	{
	public:
		EditorUIDrawer(GraphicsDevice* pDevice);
		~EditorUIDrawer();

		void Draw();
		Ref<Framebuffer> getPresentableFramebuffer() override;
	private:
		Ref<UserInterface> mUserInterface;
		Ref<Framebuffer> mFramebuffer;
	};
}
