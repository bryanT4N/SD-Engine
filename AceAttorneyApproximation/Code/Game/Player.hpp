#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"

class Player : public Entity
{
public:
	explicit Player(Game* owner);
	virtual ~Player() override;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	Camera const& GetCamera() const;
	void ResetPose();

private:
	void UpdateCameraFromTransform();

private:
	Camera m_camera;
};
