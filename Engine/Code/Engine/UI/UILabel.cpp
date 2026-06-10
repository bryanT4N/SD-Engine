#include "Engine/UI/UILabel.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"

//-----------------------------------------------------------------------------------------------
Vec2 UILabel::ComputeDesiredSize() const
{
	if (m_font == nullptr) {
		return Vec2(0.f, 0.f);
	}
	float textWidth = m_font->GetTextWidth(m_textHeight, m_text);
	return Vec2(textWidth, m_textHeight);
}

//-----------------------------------------------------------------------------------------------
void UILabel::Render(Renderer& renderer) const
{
	if (m_font == nullptr) return;
	if (m_text.empty()) return;

	std::vector<Vertex> textVerts;
	m_font->AddVertsForTextInBox2D(textVerts, m_text, m_screenBounds, m_textHeight, m_color);

	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetSamplerMode(SamplerMode::POINT_CLAMP);
	renderer.SetDepthMode(DepthMode::DISABLED);
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	renderer.BindTexture(&m_font->GetTexture());
	renderer.DrawVertexArray(textVerts);
}
