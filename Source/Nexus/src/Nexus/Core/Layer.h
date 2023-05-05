#pragma once
#include "TimeStep.h"

namespace Nexus
{
  class Layer
  {
  public:
	  virtual void OnAttach(){};
  	virtual void OnUpdate(Timestep ts){};
	  virtual void OnRender(){};
	  virtual void OnDetach(){};
	  virtual void OnWindowResize(int width, int height) {};
	  virtual ~Layer() {};
  };
}
