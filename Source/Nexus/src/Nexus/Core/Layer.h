#pragma once

class Layer
{
public:
	virtual void OnAttach(){};
	virtual void OnUpdate(){};
	virtual void OnRender(){};
	virtual void OnDetach(){};
	virtual void OnImGuiRender(){};
	virtual void OnWindowResize(int width, int height) {};
};