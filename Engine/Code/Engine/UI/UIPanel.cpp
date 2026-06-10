#include "Engine/UI/UIPanel.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

//-----------------------------------------------------------------------------------------------
void UIPanel::Render(Renderer& renderer) const
{
	if (m_visibility != EUIVisibility::VISIBLE) return;

	std::vector<Vertex> bgVerts;
	AddVertsForAABB2D(bgVerts, m_screenBounds, m_bgColor);

	if (m_borderThickness > 0.f) {
		float t = m_borderThickness;
		AABB2 topEdge(m_screenBounds.m_mins.x, m_screenBounds.m_maxs.y - t,
		              m_screenBounds.m_maxs.x, m_screenBounds.m_maxs.y);
		AABB2 bottomEdge(m_screenBounds.m_mins.x, m_screenBounds.m_mins.y,
		                 m_screenBounds.m_maxs.x, m_screenBounds.m_mins.y + t);
		AABB2 leftEdge(m_screenBounds.m_mins.x, m_screenBounds.m_mins.y + t,
		               m_screenBounds.m_mins.x + t, m_screenBounds.m_maxs.y - t);
		AABB2 rightEdge(m_screenBounds.m_maxs.x - t, m_screenBounds.m_mins.y + t,
		                m_screenBounds.m_maxs.x, m_screenBounds.m_maxs.y - t);
		AddVertsForAABB2D(bgVerts, topEdge, m_borderColor);
		AddVertsForAABB2D(bgVerts, bottomEdge, m_borderColor);
		AddVertsForAABB2D(bgVerts, leftEdge, m_borderColor);
		AddVertsForAABB2D(bgVerts, rightEdge, m_borderColor);
	}

	renderer.SetBlendMode(BlendMode::ALPHA);
	renderer.SetSamplerMode(SamplerMode::POINT_CLAMP);
	renderer.SetDepthMode(DepthMode::DISABLED);
	renderer.SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(bgVerts);

	UIAnchorContainer::Render(renderer);
}
