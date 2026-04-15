#pragma once

#include "Engine/Math/Vec2.hpp"
#include <vector>

class CubicHermiteCurve2D;

//-----------------------------------------------------------------------------------------------
class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);
	explicit CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite);

	Vec2	EvaluateAtParametric(float parametricZeroToOne) const;
	float	GetApproximateLength(int numSubdivisions = 64) const;
	Vec2	EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

	Vec2	GetStartPos() const;
	Vec2	GetGuidePos1() const;
	Vec2	GetGuidePos2() const;
	Vec2	GetEndPos() const;

	Vec2	GetStartVelocity() const;
	Vec2	GetEndVelocity() const;
	void	SetStartVelocity(Vec2 const& startVelocity);
	void	SetEndVelocity(Vec2 const& endVelocity);

private:
	Vec2	m_startPos;
	Vec2	m_guidePos1;
	Vec2	m_guidePos2;
	Vec2	m_endPos;
};


//-----------------------------------------------------------------------------------------------
class CubicHermiteCurve2D
{
public:
	CubicHermiteCurve2D(Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity);
	explicit CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier);

	Vec2	EvaluateAtParametric(float parametricZeroToOne) const;
	float	GetApproximateLength(int numSubdivisions = 64) const;
	Vec2	EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

	Vec2	GetStartPos() const;
	Vec2	GetEndPos() const;
	Vec2	GetStartVelocity() const;
	Vec2	GetEndVelocity() const;
	void	SetStartPos(Vec2 const& startPos);
	void	SetEndPos(Vec2 const& endPos);
	void	SetStartVelocity(Vec2 const& startVelocity);
	void	SetEndVelocity(Vec2 const& endVelocity);

private:
	Vec2	m_startPos;
	Vec2	m_startVelocity;
	Vec2	m_endPos;
	Vec2	m_endVelocity;
};


//-----------------------------------------------------------------------------------------------
class CubicHermiteSpline2D
{
public:
	explicit CubicHermiteSpline2D(std::vector<Vec2> const& positions);
	CubicHermiteSpline2D(std::vector<Vec2> const& positions, std::vector<Vec2> const& velocities);

	int		GetNumControlPoints() const;
	int		GetNumCurves() const;
	Vec2	GetPositionAtIndex(int index) const;
	Vec2	GetVelocityAtIndex(int index) const;
	CubicHermiteCurve2D GetHermiteCurveForSegment(int segmentIndex) const;

	Vec2	EvaluateAtParametric(float parametricZeroToNumCurves) const;
	float	GetApproximateLength(int numSubdivisionsPerCurve = 64) const;
	Vec2	EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCurve = 64) const;

	void	SetControlPointsUsingCatmullRom(std::vector<Vec2> const& positions);

private:
	std::vector<Vec2> m_positions;
	std::vector<Vec2> m_velocities;
};
