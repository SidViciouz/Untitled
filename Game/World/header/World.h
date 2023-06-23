/*
* world contains actors and camera
*/
#pragma once

#include <memory>
#include <vector>

using namespace std;

namespace Renderer
{
	class CCamera;
	class CParticleManager;
}

namespace Game
{
	class CEventManager;

	class CActor;

	class CWorld
	{
	public:
		static shared_ptr<CWorld>				Create();
												~CWorld();
		
		void									Update(double pDeltaTime);

		void									Add(shared_ptr<CActor> pActor);
		void									Remove(shared_ptr<CActor> pActor);

		shared_ptr<Renderer::CCamera>			GetCamera() const;

		shared_ptr<Renderer::CParticleManager>	GetParticleManager() const;

		template<class F, class... Args>
		void									ToAllActors(F pFunction,Args&&... pArgs)
		{
			for (auto& lActor : mActors)
			{
				pFunction(std::forward<Args>(pArgs)..., lActor);
			}
		}


	protected:
												CWorld();

		vector<shared_ptr<CActor>>				mActors;

		shared_ptr<Renderer::CCamera>			mCamera;

		shared_ptr<Renderer::CParticleManager>	mParticleManager;

		shared_ptr<CEventManager>				mEventManager;
	};
}
