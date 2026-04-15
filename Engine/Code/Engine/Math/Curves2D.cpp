#include "Engine/Math/Curves2D.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <algorithm>

//-----------------------------------------------------------------------------------------------
static Vec2 InterpolateVec2(Vec2 const& start, Vec2 const& end, float fractionTowardEnd)
{
	return start + (end - start) * fractionTowardEnd;
}

//-----------------------------------------------------------------------------------------------
static int GetSafeSubdivisionCount(int requestedSubdivisions)
{
	return requestedSubdivisions < 1 ? 1 : requestedSubdivisions;
}


//-----------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
	: m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{
}

//-----------------------------------------------------------------------------------------------
CubicBezierCurve2D::CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite)
	: m_startPos(fromHermite.GetStartPos())
	, m_guidePos1(fromHermite.GetStartPos() + (fromHermite.GetStartVelocity() / 3.f))
	, m_guidePos2(fromHermite.GetEndPos() - (fromHermite.GetEndVelocity() / 3.f))
	, m_endPos(fromHermite.GetEndPos())
{
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float t = GetClampedZeroToOne(parametricZeroToOne);
	Vec2 ab = InterpolateVec2(m_startPos, m_guidePos1, t);
	Vec2 bc = InterpolateVec2(m_guidePos1, m_guidePos2, t);
	Vec2 cd = InterpolateVec2(m_guidePos2, m_endPos, t);

	Vec2 abc = InterpolateVec2(ab, bc, t);
	Vec2 bcd = InterpolateVec2(bc, cd, t);

	return InterpolateVec2(abc, bcd, t);
}

//-----------------------------------------------------------------------------------------------
float CubicBezierCurve2D::GetApproximateLength(int numSubdivisions) const
{
	int subdivisions = GetSafeSubdivisionCount(numSubdivisions);
	float approximateLength = 0.f;
	Vec2 previousPos = EvaluateAtParametric(0.f);

	for (int subdivisionIndex = 1; subdivisionIndex <= subdivisions; ++subdivisionIndex) {
		float t = static_cast<float>(subdivisionIndex) / static_cast<float>(subdivisions);
		Vec2 currentPos = EvaluateAtParametric(t);
		approximateLength += GetDistance2D(previousPos, currentPos);
		previousPos = currentPos;
	}

	return approximateLength;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	int subdivisions = GetSafeSubdivisionCount(numSubdivisions);
	float approximateLength = GetApproximateLength(subdivisions);
	float clampedDistance = GetClamped(distanceAlongCurve, 0.f, approximateLength);

	if (clampedDistance <= 0.f) {
		return m_startPos;
	}
	if (clampedDistance >= approximateLength) {
		return m_endPos;
	}

	float traversedDistance = 0.f;
	Vec2 previousPos = EvaluateAtParametric(0.f);
	for (int subdivisionIndex = 1; subdivisionIndex <= subdivisions; ++subdivisionIndex) {
		float t = static_cast<float>(subdivisionIndex) / static_cast<float>(subdivisions);
		Vec2 currentPos = EvaluateAtParametric(t);
		float segmentLength = GetDistance2D(previousPos, currentPos);

		if (traversedDistance + segmentLength >= clampedDistance) {
			if (segmentLength <= 0.f) {
				return currentPos;
			}
			float segmentFraction = (clampedDistance - traversedDistance) / segmentLength;
			return InterpolateVec2(previousPos, currentPos, segmentFraction);
		}

		traversedDistance += segmentLength;
		previousPos = currentPos;
	}

	return m_endPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetStartPos() const
{
	return m_startPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetGuidePos1() const
{
	return m_guidePos1;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetGuidePos2() const
{
	return m_guidePos2;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetEndPos() const
{
	return m_endPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetStartVelocity() const
{
	return 3.f * (m_guidePos1 - m_startPos);
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicBezierCurve2D::GetEndVelocity() const
{
	return 3.f * (m_endPos - m_guidePos2);
}

//-----------------------------------------------------------------------------------------------
void CubicBezierCurve2D::SetStartVelocity(Vec2 const& startVelocity)
{
	m_guidePos1 = m_startPos + (startVelocity / 3.f);
}

//-----------------------------------------------------------------------------------------------
void CubicBezierCurve2D::SetEndVelocity(Vec2 const& endVelocity)
{
	m_guidePos2 = m_endPos - (endVelocity / 3.f);
}


//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D(Vec2 startPos, Vec2 startVelocity, Vec2 endPos, Vec2 endVelocity)
	: m_startPos(startPos)
	, m_startVelocity(startVelocity)
	, m_endPos(endPos)
	, m_endVelocity(endVelocity)
{
}

//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D::CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier)
	: m_startPos(fromBezier.GetStartPos())
	, m_startVelocity(fromBezier.GetStartVelocity())
	, m_endPos(fromBezier.GetEndPos())
	, m_endVelocity(fromBezier.GetEndVelocity())
{
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float t = GetClampedZeroToOne(parametricZeroToOne);
	float t2 = t * t;
	float t3 = t2 * t;

	float h00 = (2.f * t3) - (3.f * t2) + 1.f;
	float h10 = t3 - (2.f * t2) + t;
	float h01 = (-2.f * t3) + (3.f * t2);
	float h11 = t3 - t2;

	return (m_startPos * h00) + (m_startVelocity * h10) + (m_endPos * h01) + (m_endVelocity * h11);
}

//-----------------------------------------------------------------------------------------------
float CubicHermiteCurve2D::GetApproximateLength(int numSubdivisions) const
{
	int subdivisions = GetSafeSubdivisionCount(numSubdivisions);
	float approximateLength = 0.f;
	Vec2 previousPos = EvaluateAtParametric(0.f);

	for (int subdivisionIndex = 1; subdivisionIndex <= subdivisions; ++subdivisionIndex) {
		float t = static_cast<float>(subdivisionIndex) / static_cast<float>(subdivisions);
		Vec2 currentPos = EvaluateAtParametric(t);
		approximateLength += GetDistance2D(previousPos, currentPos);
		previousPos = currentPos;
	}

	return approximateLength;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	int subdivisions = GetSafeSubdivisionCount(numSubdivisions);
	float approximateLength = GetApproximateLength(subdivisions);
	float clampedDistance = GetClamped(distanceAlongCurve, 0.f, approximateLength);

	if (clampedDistance <= 0.f) {
		return m_startPos;
	}
	if (clampedDistance >= approximateLength) {
		return m_endPos;
	}

	float traversedDistance = 0.f;
	Vec2 previousPos = EvaluateAtParametric(0.f);
	for (int subdivisionIndex = 1; subdivisionIndex <= subdivisions; ++subdivisionIndex) {
		float t = static_cast<float>(subdivisionIndex) / static_cast<float>(subdivisions);
		Vec2 currentPos = EvaluateAtParametric(t);
		float segmentLength = GetDistance2D(previousPos, currentPos);

		if (traversedDistance + segmentLength >= clampedDistance) {
			if (segmentLength <= 0.f) {
				return currentPos;
			}
			float segmentFraction = (clampedDistance - traversedDistance) / segmentLength;
			return InterpolateVec2(previousPos, currentPos, segmentFraction);
		}

		traversedDistance += segmentLength;
		previousPos = currentPos;
	}

	return m_endPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetStartPos() const
{
	return m_startPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetEndPos() const
{
	return m_endPos;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetStartVelocity() const
{
	return m_startVelocity;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteCurve2D::GetEndVelocity() const
{
	return m_endVelocity;
}

//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetStartPos(Vec2 const& startPos)
{
	m_startPos = startPos;
}

//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetEndPos(Vec2 const& endPos)
{
	m_endPos = endPos;
}

//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetStartVelocity(Vec2 const& startVelocity)
{
	m_startVelocity = startVelocity;
}

//-----------------------------------------------------------------------------------------------
void CubicHermiteCurve2D::SetEndVelocity(Vec2 const& endVelocity)
{
	m_endVelocity = endVelocity;
}


//-----------------------------------------------------------------------------------------------
CubicHermiteSpline2D::CubicHermiteSpline2D(std::vector<Vec2> const& positions)
{
	SetControlPointsUsingCatmullRom(positions);
}

//-----------------------------------------------------------------------------------------------
CubicHermiteSpline2D::CubicHermiteSpline2D(std::vector<Vec2> const& positions, std::vector<Vec2> const& velocities)
	: m_positions(positions)
{
	m_velocities.assign(m_positions.size(), Vec2::ZERO);
	int velocityCount = static_cast<int>(std::min(m_positions.size(), velocities.size()));
	for (int velocityIndex = 0; velocityIndex < velocityCount; ++velocityIndex) {
		m_velocities[velocityIndex] = velocities[velocityIndex];
	}
}

//-----------------------------------------------------------------------------------------------
int CubicHermiteSpline2D::GetNumControlPoints() const
{
	return static_cast<int>(m_positions.size());
}

//-----------------------------------------------------------------------------------------------
int CubicHermiteSpline2D::GetNumCurves() const
{
	int numControlPoints = GetNumControlPoints();
	return numControlPoints > 1 ? (numControlPoints - 1) : 0;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::GetPositionAtIndex(int index) const
{
	if (m_positions.empty()) {
		return Vec2::ZERO;
	}

	int clampedIndex = static_cast<int>(GetClamped(static_cast<float>(index), 0.f, static_cast<float>(m_positions.size() - 1)));
	return m_positions[clampedIndex];
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::GetVelocityAtIndex(int index) const
{
	if (m_velocities.empty()) {
		return Vec2::ZERO;
	}

	int clampedIndex = static_cast<int>(GetClamped(static_cast<float>(index), 0.f, static_cast<float>(m_velocities.size() - 1)));
	return m_velocities[clampedIndex];
}

//-----------------------------------------------------------------------------------------------
CubicHermiteCurve2D CubicHermiteSpline2D::GetHermiteCurveForSegment(int segmentIndex) const
{
	if (GetNumCurves() <= 0) {
		return CubicHermiteCurve2D(Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO);
	}

	int clampedSegmentIndex = static_cast<int>(GetClamped(static_cast<float>(segmentIndex), 0.f, static_cast<float>(GetNumCurves() - 1)));
	int nextIndex = clampedSegmentIndex + 1;
	return CubicHermiteCurve2D(
		m_positions[clampedSegmentIndex],
		m_velocities[clampedSegmentIndex],
		m_positions[nextIndex],
		m_velocities[nextIndex]);
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::EvaluateAtParametric(float parametricZeroToNumCurves) const
{
	int numCurves = GetNumCurves();
	if (numCurves <= 0) {
		return m_positions.empty() ? Vec2::ZERO : m_positions.front();
	}

	float clampedParametric = GetClamped(parametricZeroToNumCurves, 0.f, static_cast<float>(numCurves));
	int curveIndex = static_cast<int>(clampedParametric);
	if (curveIndex >= numCurves) {
		curveIndex = numCurves - 1;
	}

	float localT = clampedParametric - static_cast<float>(curveIndex);
	if (clampedParametric == static_cast<float>(numCurves)) {
		localT = 1.f;
	}

	return GetHermiteCurveForSegment(curveIndex).EvaluateAtParametric(localT);
}

//-----------------------------------------------------------------------------------------------
float CubicHermiteSpline2D::GetApproximateLength(int numSubdivisionsPerCurve) const
{
	int numCurves = GetNumCurves();
	float approximateLength = 0.f;
	for (int curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
		approximateLength += GetHermiteCurveForSegment(curveIndex).GetApproximateLength(numSubdivisionsPerCurve);
	}

	return approximateLength;
}

//-----------------------------------------------------------------------------------------------
Vec2 CubicHermiteSpline2D::EvaluateAtApproximateDistance(float distanceAlongSpline, int numSubdivisionsPerCurve) const
{
	int numCurves = GetNumCurves();
	if (numCurves <= 0) {
		return m_positions.empty() ? Vec2::ZERO : m_positions.front();
	}

	float approximateLength = GetApproximateLength(numSubdivisionsPerCurve);
	float clampedDistance = GetClamped(distanceAlongSpline, 0.f, approximateLength);
	if (clampedDistance <= 0.f) {
		return m_positions.front();
	}
	if (clampedDistance >= approximateLength) {
		return m_positions.back();
	}

	float traversedDistance = 0.f;
	for (int curveIndex = 0; curveIndex < numCurves; ++curveIndex) {
		CubicHermiteCurve2D curve = GetHermiteCurveForSegment(curveIndex);
		float curveLength = curve.GetApproximateLength(numSubdivisionsPerCurve);
		if (traversedDistance + curveLength >= clampedDistance) {
			float curveDistance = clampedDistance - traversedDistance;
			return curve.EvaluateAtApproximateDistance(curveDistance, numSubdivisionsPerCurve);
		}

		traversedDistance += curveLength;
	}

	return m_positions.back();
}

//-----------------------------------------------------------------------------------------------
void CubicHermiteSpline2D::SetControlPointsUsingCatmullRom(std::vector<Vec2> const& positions)
{
	m_positions = positions;
	m_velocities.assign(m_positions.size(), Vec2::ZERO);

	int numControlPoints = static_cast<int>(m_positions.size());
	for (int pointIndex = 1; pointIndex < numControlPoints - 1; ++pointIndex) {
		m_velocities[pointIndex] = (m_positions[pointIndex + 1] - m_positions[pointIndex - 1]) * 0.5f;
	}
}
