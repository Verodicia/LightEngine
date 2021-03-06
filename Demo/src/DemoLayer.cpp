#include "DemoLayer.h"

#include "AudioLayer.h"
#include "QuadsLayer.h"
#include "PostProcessLayer.h"
#include "TextLayer.h"

DemoLayer::DemoLayer()
	: m_CameraSpeed(525.0f), m_CameraZoomSpeed(25.0f)
{
	LT_PROFILE_FUNC();
	LT_TRACE("DemoLayer::DemoLayer");
	m_LayeDebugrName = "DemoLayer"; // can be retrived with GetName().

	// Camera class is used to get view/projection/bounds of the camera.
	// CameraController is used to change position/zoomLevel/aspectRatio. you probably want to use it only in 1 layer.
	m_Camera = std::make_shared<Light::Camera>(glm::vec2(0.0f, 0.0f), Light::GraphicsContext::GetAspectRatio(), 1000.0f);
	m_CameraController = m_Camera->GetController();

	// construct layers
	m_AudioLayer = new AudioLayer;
	m_QuadsLayer = new QuadsLayer(m_Camera);
	m_PostProcessLayer = new PostProcessLayer;
	m_TextLayer = new TextLayer(m_Camera);
}

void DemoLayer::OnAttach()
{
	LT_PROFILE_FUNC();
	LT_TRACE("DemoLayer::OnAttach");

	// set mouse cursor's icon
	Light::Window::Get()->SetMouseCursor("res/cursor.png", 1u, 1u);

	// attach layers
	Light::Application::AttachLayer(m_AudioLayer);
	Light::Application::AttachLayer(m_QuadsLayer);
	Light::Application::AttachLayer(m_PostProcessLayer);
	Light::Application::AttachLayer(m_TextLayer);
}

void DemoLayer::OnDetatch()
{
	LT_TRACE("DemoLayer::OnDetatch");
}

void DemoLayer::OnUpdate(float DeltaTime)
{
	// camera control

	if (Light::Input::GetKey(KEY_A))
		m_CameraController->MoveX(-m_CameraSpeed * DeltaTime);
	if (Light::Input::GetKey(KEY_D))
		m_CameraController->MoveX(m_CameraSpeed * DeltaTime);
	
	if (Light::Input::GetKey(KEY_W))
		m_CameraController->MoveY(-m_CameraSpeed * DeltaTime);
	if (Light::Input::GetKey(KEY_S))
		m_CameraController->MoveY(m_CameraSpeed * DeltaTime);
}

void DemoLayer::OnUserInterfaceUpdate()
{
	// ImGui stuff, if you don't understand these, call Light::UserInterface::ShowImGuiDemoWindow(),
	//     and see imgui_demo.cpp in ImGui project.

	// ** all ImGui calls should be inside OnUserInterfaceUpdate, because it's wrapped with UserInterface::Begin/End
	//       calling ImGui functions in a function other than OnUserInterfaceUpdate results in runtime error.

	ImGui::SetNextWindowSize(ImVec2(Light::GraphicsContext::GetResolution().width / 3.0f, Light::GraphicsContext::GetResolution().height));
	ImGui::Begin("DemoLayer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	if (ImGui::TreeNode("Keyboard map"))
	{
		ImGui::Text(" __________________________________");
		ImGui::Text("|    KEY    |        ACTION        |");
		ImGui::Text("|-----------|----------------------|");
		ImGui::Text("|    ESC    |         Exit         |");
		ImGui::Text("|     1     |  BorderlessWindowed  |");
		ImGui::Text("|     2     |       Windowed       |");
		ImGui::Text("|     3     |      Fullscreen      |");
		ImGui::Text("|-----------|----------------------|");
		ImGui::Text("|     W     |     Camera: Up       |");
		ImGui::Text("|     A     |     Camera: Left     |");
		ImGui::Text("|     S     |     Camera: Down     |");
		ImGui::Text("|     D     |     Camera: Right    |");
		ImGui::Text("|-----------|----------------------|");
		ImGui::Text("|     E     |    GraphicsAPI:GL    |");
		ImGui::Text("|     R     |    GraphicsAPI:DX    |");
		ImGui::Text("|-----------|----------------------|");
		ImGui::Text("|     Z     |  Resolution (4:3)    |");
		ImGui::Text("|     X     |  Resolution (16:9)   |");
		ImGui::Text("|     C     |  Resolution (16:10)  |");
		ImGui::Text("|___________|______________________|");

		ImGui::TreePop();
	} 
	ImGui::Separator();

	if (ImGui::TreeNode("Mouse map"))
	{
		ImGui::Text(" _______________________________________");
		ImGui::Text("|     BUTTON    |        ACTION        |");
		ImGui::Text("|---------------|----------------------|");
		ImGui::Text("|  LEFT_BUTTON  |      Drag Sprite     |");
		ImGui::Text("|---------------|----------------------|");
		ImGui::Text("|  CTRL + WHEEL |      Zoom In/Out     |");
		ImGui::Text("|_______________|______________________|");

		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Monitors"))
	{
		Light::Monitor::ShowDebugWindowAll();
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Layers"))
	{
		Light::Application::ShowDebugWindow();
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Camera"))
	{
		m_Camera->ShowDebugWindow();
		ImGui::SliderFloat("CameraMoveSpeed", &m_CameraSpeed, 25.0f, 2000.0f, "%.1f");
		ImGui::SliderFloat("CameraZoomSpeed", &m_CameraZoomSpeed, 5.0f, 100.0f, "%.1f");

		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("MSAA"))
	{
		// toggle
		if (ImGui::Button(Light::GraphicsContext::IsMSAAEnabled() ? "disable" : "enable"))
			Light::GraphicsContext::Get()->SetMSAA(!Light::GraphicsContext::IsMSAAEnabled());

		// samples count should be a power of 2, '2, 4, 8, or 16'.
		static int samples = 1;
		if (ImGui::DragInt("^ 2 = SampleCount", &samples, 1.0f, 1, 4))
			Light::GraphicsContext::Get()->SetMSAASampleCount(BIT(samples));

		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("GraphicsContext"))
	{
		Light::GraphicsContext::Get()->ShowDebugWindow();
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Input"))
	{
		Light::Input::ShowDebugWindow();
		ImGui::TreePop();
	}
	ImGui::Separator();

	ImGui::End();
}

void DemoLayer::OnEvent(Light::Event& event)
{
	// uncomment to log every event.
	// LT_TRACE(event.GetLogInfo());

	Light::Dispatcher dispatcher(event); 

	// if one of these functions return true, next layers won't receive 'event'.
	dispatcher.Dispatch<Light::KeyboardKeyPressedEvent>(LT_EVENT_FN(DemoLayer::OnKeyPress));
	dispatcher.Dispatch<Light::MouseScrolledEvent>(LT_EVENT_FN(DemoLayer::OnMouseScroll));

	dispatcher.Dispatch<Light::WindowResizedEvent>(LT_EVENT_FN(DemoLayer::OnWindowResize));
}

bool DemoLayer::OnKeyPress(Light::KeyboardKeyPressedEvent& event)
{
	// exit program
	if (event.GetKey() == KEY_ESCAPE)
		Light::Window::Get()->Close();

	// change graphics context
	if (event.GetKey() == KEY_E || event.GetKey() == KEY_R)
	{
		// detach layers before deleting
		Light::Application::DetachLayer(m_QuadsLayer);
		Light::Application::DetachLayer(m_PostProcessLayer);
		Light::Application::DetachLayer(m_TextLayer);

		// delete layers
		delete m_QuadsLayer;
		delete m_PostProcessLayer;
		delete m_TextLayer;

		// change context
		Light::GraphicsContext::CreateContext(event.GetKey() == KEY_E ? Light::GraphicsAPI::Opengl : Light::GraphicsAPI::Directx,
		                                      Light::GraphicsContext::GetConfigurations());

		// construct layers
		m_QuadsLayer = new QuadsLayer(m_Camera);
		m_PostProcessLayer = new PostProcessLayer;
		m_TextLayer = new TextLayer(m_Camera);

		// attach layers
		Light::Application::AttachLayer(m_QuadsLayer);
		Light::Application::AttachLayer(m_PostProcessLayer);
		Light::Application::AttachLayer(m_TextLayer);
	}

	// change resolution 4:3
	if (event.GetKey() == KEY_Z)
	{
		static int index = 0;
		static const Light::Resolution res[] =
		{
			{ 640 , 480 , Light::AspectRatio::AR_4_3 },
			{ 800 , 600 , Light::AspectRatio::AR_4_3 },
			{ 960 , 720 , Light::AspectRatio::AR_4_3 },
			{ 1400, 1050, Light::AspectRatio::AR_4_3 },
		};

		Light::GraphicsContext::Get()->SetResolution(res[index++ % std::size(res)]);
	}

	// change resolution 16:9
	if (event.GetKey() == KEY_X)
	{
		static int index = 0;
		static const Light::Resolution res[] =
		{
			{ 1280, 720  , Light::AspectRatio::AR_16_9 },
			{ 1600, 900  , Light::AspectRatio::AR_16_9 },
			{ 1920, 1080 , Light::AspectRatio::AR_16_9 },
		};

		Light::GraphicsContext::Get()->SetResolution(res[++index % std::size(res)]);
	}

	// change resolution 16:10
	if (event.GetKey() == KEY_C)
	{
		static int index = 0;
		static const Light::Resolution res[] =
		{
			{ 1280, 800  ,Light::AspectRatio::AR_16_10 },
			{ 1440, 900  ,Light::AspectRatio::AR_16_10 },
			{ 1920, 1200 ,Light::AspectRatio::AR_16_10 },
			{ 1680, 1050 ,Light::AspectRatio::AR_16_10 },
		};

		Light::GraphicsContext::Get()->SetResolution(res[index++ % std::size(res)]);
	}

	// set display mode
	if (event.GetKey() == KEY_1)
		Light::Window::Get()->SetDisplayMode(Light::DisplayMode::BorderlessWindowed);

	if (event.GetKey() == KEY_2)
		Light::Window::Get()->SetDisplayMode(Light::DisplayMode::Windowed);

	if (event.GetKey() == KEY_3)
		Light::Window::Get()->SetDisplayMode(Light::DisplayMode::Fullscreen);

	return false;
}

bool DemoLayer::OnMouseScroll(Light::MouseScrolledEvent& event)
{
	if (Light::Input::GetKey(KEY_LEFT_CONTROL))
		m_CameraController->Zoom(event.GetOffset() * m_CameraZoomSpeed);

	return false;
}

bool DemoLayer::OnWindowResize(Light::WindowResizedEvent& event)
{
	// change aspect ratio of camera
	m_CameraController->SetProjection(Light::GraphicsContext::GetAspectRatio(), m_CameraController->GetZoomLevel());
	
	return false;
}