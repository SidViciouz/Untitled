#pragma once


namespace Math
{
	// pAlingment must be power of 2
	unsigned int Alignment(unsigned int pByteSize, unsigned int pAlignment)
	{
		return (pByteSize + pAlignment - 1) & ~(pAlignment - 1);
	}
}