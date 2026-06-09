#include "Game/Prop.hpp"

#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Renderer.hpp"

Prop::Prop(Game* owner)
	: Entity(owner)
{
}

Prop::~Prop()
{
}

void Prop::Update(float deltaSeconds)
{
	EulerAngles deltaOrientation(
		m_angularVelocity.m_yawDegrees * deltaSeconds,
		m_angularVelocity.m_pitchDegrees * deltaSeconds,
		m_angularVelocity.m_rollDegrees * deltaSeconds);
	m_orientation += deltaOrientation;
}

void Prop::Render() const
{
	if (g_engine == nullptr || g_engine->m_render == nullptr) {
		return;
	}

	g_engine->m_render->SetBlendMode(BlendMode::OPAQUE);
	g_engine->m_render->BindTexture(m_texture);
	g_engine->m_render->DrawVertexArray(m_vertexes);
}
