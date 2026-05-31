#pragma once

#include "Game/Entity.hpp"
#include "Engine/Core/Vertex.hpp"

#include <vector>

class Texture;

class Prop : public Entity
{
public:
	explicit Prop(Game* owner);
	virtual ~Prop() override;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

public:
	std::vector<Vertex_PCU> m_vertexes;
	Texture* m_texture = nullptr;
};
