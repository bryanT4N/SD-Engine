#pragma once

class ChessObject
{
public:
	virtual ~ChessObject() = default;
	virtual void Render() const = 0;
};
