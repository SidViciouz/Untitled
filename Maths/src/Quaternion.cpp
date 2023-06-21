#include "Quaternion.h"

namespace Math
{
	SQuaternion::SQuaternion()
	{
		mX = 0;
		mY = 0;
		mZ = 0;
		mW = 1;
	}

	SQuaternion::SQuaternion(const float& pX, const float& pY, const float& pZ, const float& pW)
	{
		mX = pX;
		mY = pY;
		mZ = pZ;
		mW = pW;
	}

	float SQuaternion::Dot(const SQuaternion& pA, const SQuaternion& pB)
	{
		float lResult = 0.0f;

		for (int i = 0; i < 4; ++i)
			lResult += pA.mElement[i] * pB.mElement[i];

		return lResult;
	}

	SQuaternion SQuaternion::operator-() const
	{
		SQuaternion lResult;

		for (int i = 0; i < 4; ++i)
			lResult.mElement[i] = -mElement[i];

		return lResult;
	}

	template<typename T>
	SQuaternion	SQuaternion::operator*(const T& pValue) const
	{
		SQuaternion lResult;

		for (int i = 0; i < 4; ++i)
			lResult.mElement[i] = mElement[i] * pValue;

		return lResult;
	}

	SQuaternion	SQuaternion::operator+(const SQuaternion& pOther)
	{
		SQuaternion lResult;

		for (int i = 0; i < 4; ++i)
			lResult.mElement[i] = mElement[i] + pOther.mElement[i];

		return lResult;
	}

	void SQuaternion::Normalized()
	{
		DirectX::XMVECTOR lQuaternion = DirectX::XMLoadFloat4(&mXmElement);

		lQuaternion = DirectX::XMQuaternionNormalize(lQuaternion);

		DirectX::XMStoreFloat4(&mXmElement,lQuaternion);
	}

	SQuaternion	SQuaternion::Slerp(SQuaternion pOther, const double& pAlpha) const
	{
		SQuaternion lResult;
		/*
		double lCosTheta = (double)SQuaternion::Dot(*this, pOther);

		if (lCosTheta < 0.0f)
		{
			lCosTheta = -lCosTheta;
			pOther = -pOther;
		}

		double lTheta = acos(lCosTheta);

		double lEpsilon = 0.000001f;

		double lSinTheta = sin(lTheta) + lEpsilon;

		lResult = ((*this) * (sin((1.0f - pAlpha) * lTheta) / lSinTheta)) + (pOther * (sin(pAlpha * lTheta) / lSinTheta));

		lResult.Normalized();
		*/


		double lCosTheta = (double)SQuaternion::Dot(*this, pOther);

		if (lCosTheta < 0.0f)
		{
			lCosTheta = -lCosTheta;
			pOther = -pOther;
		}

		double lK0, lK1;

		if (lCosTheta > 0.9999f)
		{
			lK0 = 1.0 - pAlpha;
			lK1 = pAlpha;
		}
		else
		{
			double lSinTheta = sqrt(1.0 - lCosTheta * lCosTheta);
			double lTheta = atan2(lSinTheta, lCosTheta);
			double lOneOverSinTheta = 1.0 / lSinTheta;

			lK0 = sin((1.0f - pAlpha) * lTheta) * lOneOverSinTheta;
			lK1 = sin(pAlpha * lTheta) * lOneOverSinTheta;
		}

		for (int i = 0; i < 4; ++i)
			lResult.mElement[i] = this->mElement[i] * lK0 + pOther.mElement[i] * lK1;

		return lResult;
	}
}