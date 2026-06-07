#pragma once

#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"

class Player : public Entity
{
public:
	bool m_isInputEnabled = true;

	explicit Player(Game* owner);
	virtual ~Player() override;

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	Camera const& GetCamera() const;
	void ResetPose();
	void SnapToPose(Vec3 const& position, EulerAngles const& orientation);

private:
	void UpdateCameraFromTransform();

private:
	Camera m_camera;
};
