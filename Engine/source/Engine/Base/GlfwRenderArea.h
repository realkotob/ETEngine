#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GraphicsHelper/RenderArea.h"


//---------------------------------
// GlfwRenderArea
//
// Render area using GLFW windows as a subsystem, managing an openGL context
//
class GlfwRenderArea : public I_RenderArea
{
	// construct destruct
	//-------------------
public:
	GlfwRenderArea() : I_RenderArea() {}
	virtual ~GlfwRenderArea() = default;

	void Initialize();
	void Uninitialize();

	// functionality
	//---------------
	void Update();
	void SetVSyncEnabled(bool const val);

private:
	void OnResize();

	// accessors
	//-----------
public:
	GLFWwindow* GetWindow() const { return m_Window; }

	// Render Area Interface
	//-----------------------
protected:
	void SetOnInit(std::function<void(I_GraphicsApiContext* const)>& callback) override { m_OnInit = callback; }
	void SetOnDeinit(std::function<void()>& callback) override { m_OnDeinit = callback; }
	void SetOnResize(std::function<void(vec2 const)>& callback) override { m_OnResize = callback; }
	void SetOnRender(std::function<void()>& callback) override { m_OnRender = callback; }

	void QueueDraw() override;
	bool MakeCurrent() override;

	// Data
	///////
private:
	std::function<void(I_GraphicsApiContext* const)> m_OnInit;
	std::function<void()> m_OnDeinit;
	std::function<void(vec2 const)> m_OnResize;
	std::function<void()> m_OnRender;

	GLFWwindow* m_Window = nullptr; // also serves as context

	bool m_ShouldDraw = false;
};

