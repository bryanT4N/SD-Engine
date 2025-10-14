#pragma once


struct Rgba8 {
public:
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	void ScaleAlpha( float alphaMultiplier);

	// Construction/Destruction
	~Rgba8() {}												// destructor (do nothing)
	Rgba8() {}												// default constructor (do nothing)
	Rgba8(Rgba8 const& copyFrom);							// copy constructor (from another Rgba8)
	explicit Rgba8(unsigned char initialR, unsigned char initialG, unsigned char initialB, unsigned char initialA);		

	// Operators (const)
	bool		operator==(Rgba8 const& compare) const;		// Rgba8 == Rgba8
	bool		operator!=(Rgba8 const& compare) const;		// Rgba8 != Rgba8
};

