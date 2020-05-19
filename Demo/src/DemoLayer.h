#pragma once

#include <LightEngine.h>

class AudioLayer;
class QuadsLayer;
class PostProcessLayer;
class TextLayer;

class DemoLayer : public Light::Layer
{
private:	
	AudioLayer* m_AudioLayer;
	QuadsLayer* m_QuadsLayer;
	PostProcessLayer* m_PostProcessLayer;
	TextLayer* m_TextLayer;

	float m_DeltaTime = 0.0f;

	std::shared_ptr<Light::Camera> m_Camera;
	std::shared_ptr<Light::CameraController> m_CameraController;
	float m_CameraSpeed;
public:
	DemoLayer();

	void OnAttach() override;
	void OnDetatch() override;

	void OnUpdate(float DeltaTime) override;

	void OnUserInterfaceUpdate() override;

	void OnEvent(Light::Event& event) override;
private:
	bool OnKeyPress(Light::KeyboardKeyPressedEvent& event);
	bool OnMouseScroll(Light::MouseScrolledEvent& event);
	bool OnWindowResize(Light::WindowResizedEvent& event);
};