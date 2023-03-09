#pragma once

class Layer
{
public:
	virtual void OnAttach(){};
	virtual void OnUpdate(){};
	virtual void OnDetach(){};
};