#include "Game/header/Game.h"
#include "../Renderer/Render/header/Renderer.h"
#include "Actor/header/Actor.h"
#include "Actor/header/Player.h"
#include "Actor/header/Character.h"
#include "Actor/header/ActorPool.h"
#include "../Input/header/InputManager.h"

namespace Game
{
	
	CGame::CGame()
	{

	}

	CGame::~CGame()
	{

	}
	
	void CGame::Initialize(HINSTANCE pHInstance)
	{
		// create world
		mWorld = CWorld::Create();

		// create renderer and initialze it
		mRenderer = Renderer::CRenderer::Singleton(pHInstance);
		mRenderer->Initialize();

		// set camera into renderer
		mRenderer->SetCamera(mWorld->GetCamera());

		// set particle manager into renderer
		mRenderer->SetParticleManager(mWorld->GetParticleManager());
	}


	void CGame::Begin()
	{
		shared_ptr<Renderer::CMesh>	lMesh = make_shared<Renderer::CMesh>("../Model/AnimMan2.FBX");

		shared_ptr<CActor> lACtor = CActorPool::Singleton()->NewActor<CActor>(lMesh);
		shared_ptr<Game::CPlayer> lPlayer = CActorPool::Singleton()->NewActor<CPlayer>(lMesh);

		RegisterActor(lPlayer);
		RegisterActor(lACtor);
	}

	void CGame::Loop()
	{
		shared_ptr<Input::CInputManager> lInputManager = Input::CInputManager::Singleton();

		while (mRenderer->Loop())
		{
			mWorld->Update(1 / 60.0f);


			mRenderer->DrawBegin();
			/*
			mRenderer->DrawLine(lData1);
			mRenderer->DrawLine(lData2);
			mRenderer->DrawLine(lData3);

			mRenderer->DrawMeshPBR(lMesh4Handle, lObject4Handle, -1, -1, lTileNormalHandle, -1, lTileAOHandle);
			mRenderer->DrawMeshPBR(lMesh3Handle, lObject3Handle, lTexture1Handle, lTexture2Handle, lTexture3Handle, lTexture4Handle, -1);
			mRenderer->DrawMeshPBR(lMesh1Handle, lObject1Handle, lTexture1Handle, lTexture2Handle, lTexture3Handle, lTexture4Handle, -1);
			mRenderer->DrawParticles(lParticleSpriteHandle);
			mRenderer->DrawMeshPBR(lMesh2Handle, lObject2Handle, -1, -1, -1, -1, -1);
			*/
			mRenderer->DrawEnd();
		}
	}

	void CGame::RegisterActor(shared_ptr<CActor> pActor)
	{
		//add actor to world
		mWorld->Add(pActor);

		//register actor to renderer
		mRenderer->SetMesh(pActor->GetMesh());
		mRenderer->SetObject(pActor->GetObject());

	}

	void CGame::UnregisterActor(shared_ptr<CActor> pActor)
	{

	}
}