/*
* character class takes input
*/


#pragma once

#include "Actor.h"
#include "../Animation/header/AnimationGraph.h"

namespace Game
{
	class ICharacter : public CActor
	{
	public:
												~ICharacter();

		virtual void							Update(double pDeltaTime) override;

	protected:
		
												ICharacter(const char* pPath);
												ICharacter(shared_ptr<Renderer::CMesh> pMesh);
												ICharacter() = delete;

		virtual void							Input() = 0;

		shared_ptr<Renderer::CAnimationGraph>	mAnimationGraph;
		bool									mHasAnimationGraph;
	};
}
