#pragma once

#include "../../Maths/header/Vector3.h"
#include "../../Maths/header/Matrix4.h"

namespace Renderer
{
	class CCamera
	{
	public:
												CCamera() = delete;
												CCamera(const int& pWidth,const int& pHeight);
		void									Resize(const int& pWidth, const int& pHeight);
		Math::SMatrix4							GetViewMatrix();
		Math::SMatrix4							GetProjectionMatrix();
		Math::SVector3							GetPosition() const;

	protected:

		Math::SVector3							mRight;
		Math::SVector3							mUp;
		Math::SVector3							mFront;
		Math::SVector3							mPosition;
		int										mWidth;
		int										mHeight;
		float									mFov;
		float									mMinPlane;
		float									mMaxPlane;
	};
}
