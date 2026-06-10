#include "Engine/UI/UIImage.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"

//-----------------------------------------------------------------------------------------------
Vec2 UIImage::ComputeDesiredSize() const
{
	if (m_texture == nullptr) {
		return Vec2(0.f, 0.f);
	}
	IntVec2 textureDimensions = m_texture->GetDimensions();
	return Vec2(static_cast<float>(textureDimensions.x), static_cast<float>(textureDimensions.y));
}

//-----------------------------------------------------------------------------------------------
void UIImage::Render(Renderer& renderer) const
{
	if (m_texture == nullptr) return;

	std::vector<Vertex> imageVerts;
	AddVertsForAABB2D(imageVerts, m_screenBounds, m_tint, m_uvRect);

	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetSamplerMode(SamplerMode::BILINEAR_WRAP);
	renderer.SetDepthMode(DepthMode::DISABLED);
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	renderer.BindTexture(m_texture);
	renderer.DrawVertexArray(imageVerts);
}
