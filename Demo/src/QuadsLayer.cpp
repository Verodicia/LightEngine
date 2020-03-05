#include "QuadsLayer.h"

QuadsLayer::QuadsLayer()
	: m_Camera(glm::vec2(500.0f, 500.0f), Light::GraphicsContext::GetAspectRatio(), 1000.0f), m_CameraSpeed(500),
	  m_SelectedSprite(nullptr)
{
	m_LayeDebugrName = "QuadsLayer";

	srand(time(NULL));

	for (int i = 0; i < 125; i++)
	{
		Sprite sprite;

		sprite.size = glm::vec2(100.0f, 100.0f);
		sprite.position.x = std::rand() % 1000;
		sprite.position.y = std::rand() % 1000;

		m_Sprites.push_back(sprite);
	}
}

void QuadsLayer::OnAttach()
{
	LT_TRACE("Attached QuadLayer");

	m_Camera.SetProjection(Light::GraphicsContext::GetAspectRatio(), m_Camera.GetZoomLevel());

	Light::TextureCoordinates* awesomeCoords, * boxCoords;
	m_TextureAtlas = Light::TextureAtlas::Create("res/atlas.txt");

	awesomeCoords = m_TextureAtlas->GetCoordinates("awesomeface");
	boxCoords = m_TextureAtlas->GetCoordinates("box");

	for (int i = 0; i < m_Sprites.size(); i++)
	{
		m_Sprites[i].shape = (Shape)(i % 2);
		m_Sprites[i].coordinates = m_Sprites[i].shape == Rect ? boxCoords : awesomeCoords;
	}
}

void QuadsLayer::OnDetatch()
{
	LT_TRACE("Detatched QuadLayer");
}

void QuadsLayer::OnUpdate(float DeltaTime)
{
	if (m_SelectedSprite)
		m_SelectedSprite->position = Light::Input::MousePosToCameraView(m_Camera) - m_SelectedSprite->size / 2.0f;

	for (auto& sprite : m_Sprites)
	{
		for (auto& target : m_Sprites)
		{
			if (&sprite == &target)
				continue;

			DoCollision(sprite, target);
		}
	}

	if (Light::Input::GetKey(KEY_A))
		m_Camera.MoveX(-m_CameraSpeed * DeltaTime);
	if (Light::Input::GetKey(KEY_D))
		m_Camera.MoveX(m_CameraSpeed * DeltaTime);

	if (Light::Input::GetKey(KEY_W))
		m_Camera.MoveY(-m_CameraSpeed * DeltaTime);
	if (Light::Input::GetKey(KEY_S))
		m_Camera.MoveY(m_CameraSpeed * DeltaTime);
}

void QuadsLayer::OnRender()
{
	Light::Renderer::Start(m_Camera);

	for (auto& sprite : m_Sprites)
		Light::Renderer::DrawQuad(sprite.position, sprite.size, sprite.coordinates,
			&sprite == m_SelectedSprite ? glm::vec4(1.0f, 0.6f, 0.6f, 1.0f) :
			glm::vec4(1.0f));

	Light::Renderer::End();
}

void QuadsLayer::ShowDebugWindow()
{
	if (ImGui::TreeNode("Camera"))
	{
		m_Camera.ShowDebugLayer();
		ImGui::BulletText("speed: %f", m_CameraSpeed);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Sprites"))
	{
		ImGui::BulletText("you can drag sprites with mouse!");
		ImGui::TreePop();
	}
}

void QuadsLayer::OnEvent(Light::Event& event)
{
	Light::Dispatcher dispatcher(event);

	dispatcher.Dispatch<Light::MouseButtonPressedEvent>(LT_EVENT_FN(QuadsLayer::OnButtonPress));
	dispatcher.Dispatch<Light::MouseButtonReleasedEvent>(LT_EVENT_FN(QuadsLayer::OnButtonRelease));

	dispatcher.Dispatch<Light::MouseScrolledEvent>(LT_EVENT_FN(QuadsLayer::OnMouseScroll));

	dispatcher.Dispatch<Light::WindowResizedEvent>(LT_EVENT_FN(QuadsLayer::OnWindowResize));
}

bool QuadsLayer::OnButtonPress(Light::MouseButtonPressedEvent& event)
{
	glm::vec2 mouse = Light::Input::MousePosToCameraView(m_Camera);

	for (auto& sprite : m_Sprites)
	{
		if (mouse.x > sprite.position.x&& mouse.x < sprite.position.x + sprite.size.x &&
			mouse.y > sprite.position.y&& mouse.y < sprite.position.y + sprite.size.y)
		{
			m_SelectedSprite = &sprite;
			return true;
		}
	}
	return true;
}

bool QuadsLayer::OnButtonRelease(Light::MouseButtonReleasedEvent& event)
{
	m_SelectedSprite = nullptr;
	return true;
}

bool QuadsLayer::OnMouseScroll(Light::MouseScrolledEvent& event)
{
	m_Camera.Zoom(event.GetOffset() * 25);
	return true;
}

bool QuadsLayer::OnWindowResize(Light::WindowResizedEvent& event)
{
	m_Camera.SetProjection(Light::GraphicsContext::GetAspectRatio(), m_Camera.GetZoomLevel());
	return true;
}

void QuadsLayer::DoCollision(Sprite& sprite, Sprite& target)
{
	glm::vec2 diff(0.0f);

	if (sprite.shape == Circle)
	{
		if (target.shape == Circle)
			Light::CheckCollision(sprite.position, sprite.size.x / 2.0f,
				target.position, target.size.x / 2.0f,
				&diff);

		else
			Light::CheckCollision(sprite.position, sprite.size.x / 2.0f,
				target.position, target.size,
				&diff);
	}
	else
	{
		if (target.shape == Circle)
			Light::CheckCollision(sprite.position, sprite.size,
				target.position, target.size.x / 2.0f,
				&diff);

		else
			Light::CheckCollision(sprite.position, sprite.size,
				target.position, target.size,
				&diff);
	}

	sprite.position -= diff / 2.0f;
	target.position += diff / 2.0f;
}
