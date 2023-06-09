#include "../Resource/header/ResourceManager.h"
#include "Util.h"
#include "../Resource/header/DDSTextureLoader12.h"
#include "../Common/header/d3dx12.h"

namespace Renderer
{
	
	CResourceManager::CResourceManager(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12GraphicsCommandList* pCopyCommandList)
		:mDevice{ pDevice }, mCommandList{pCommandList}, mCopyCommandList{pCopyCommandList}
	{
		D3D12_DESCRIPTOR_HEAP_DESC lHeapDesc = {};

		lHeapDesc.NodeMask = 0;
		lHeapDesc.NumDescriptors = MAX_RTV_NUM;
		lHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		lHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (!SUCCEEDED(mDevice->CreateDescriptorHeap(&lHeapDesc, IID_PPV_ARGS(mDescriptorHeaps[0].GetAddressOf()))))
			throw string("creating descriptor heap fails.");

		lHeapDesc.NodeMask = 0;
		lHeapDesc.NumDescriptors = MAX_DSV_NUM;
		lHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		lHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (!SUCCEEDED(mDevice->CreateDescriptorHeap(&lHeapDesc, IID_PPV_ARGS(mDescriptorHeaps[1].GetAddressOf()))))
			throw string("creating descriptor heap fails.");

		lHeapDesc.NodeMask = 0;
		lHeapDesc.NumDescriptors = MAX_DESCRIPTOR_NUM;
		lHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		lHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		if (!SUCCEEDED(mDevice->CreateDescriptorHeap(&lHeapDesc, IID_PPV_ARGS(mDescriptorHeaps[2].GetAddressOf()))))
			throw string("creating descriptor heap fails.");


		mRtvIncSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDsvIncSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		mCbvSrvUavIncSize =  mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		mRtvCpuHandleStart = mDescriptorHeaps[0]->GetCPUDescriptorHandleForHeapStart();
		mRtvGpuHandleStart = mDescriptorHeaps[0]->GetGPUDescriptorHandleForHeapStart();
		mDsvCpuHandleStart = mDescriptorHeaps[1]->GetCPUDescriptorHandleForHeapStart();
		mDsvGpuHandleStart = mDescriptorHeaps[1]->GetGPUDescriptorHandleForHeapStart();
		mCbvSrvUavCpuHandleStart = mDescriptorHeaps[2]->GetCPUDescriptorHandleForHeapStart();
		mCbvSrvUavGpuHandleStart = mDescriptorHeaps[2]->GetGPUDescriptorHandleForHeapStart();
	}



	CResourceManager::~CResourceManager()
	{

	}
	


	int CResourceManager::CreateBuffer(int pByteSize, EResourceHeapType pType)
	{
		SResourceDescription lResourceDescription = {};
		lResourceDescription.mDimension = EResourceDimension::eBuffer;
		lResourceDescription.mAlignment = 0;
		lResourceDescription.mWidth = pByteSize;
		lResourceDescription.mHeight = 1;
		lResourceDescription.mDepth = 1;
		lResourceDescription.mFormat = DXGI_FORMAT_UNKNOWN;
		lResourceDescription.mHeapType = pType;
		lResourceDescription.mLayout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		lResourceDescription.mMipLevel = 1;
		lResourceDescription.mFlags = D3D12_RESOURCE_FLAG_NONE;

		return CreateResource(lResourceDescription);
	}



	int CResourceManager::CreateTexture1D(int pWidth, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags)
	{
		SResourceDescription lResourceDescription = {};
		lResourceDescription.mDimension = EResourceDimension::eTexture1D;
		lResourceDescription.mAlignment = 0;
		lResourceDescription.mWidth = pWidth;
		lResourceDescription.mHeight = 1;
		lResourceDescription.mDepth = 1;
		lResourceDescription.mFormat = pFormat;
		lResourceDescription.mHeapType = pType;
		lResourceDescription.mLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		lResourceDescription.mMipLevel = 1;
		lResourceDescription.mFlags = pFlags;

		return CreateResource(lResourceDescription);
	}



	int CResourceManager::CreateTexture2D(int pWidth, int pHeight, int pArraySize, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags)
	{
		SResourceDescription lResourceDescription = {};
		lResourceDescription.mDimension = EResourceDimension::eTexture2D;
		lResourceDescription.mAlignment = 0;
		lResourceDescription.mWidth = pWidth;
		lResourceDescription.mHeight = pHeight;
		lResourceDescription.mDepth = pArraySize;
		lResourceDescription.mFormat = pFormat;
		lResourceDescription.mHeapType = pType;
		lResourceDescription.mLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		lResourceDescription.mMipLevel = 1;
		lResourceDescription.mFlags = pFlags;

		return CreateResource(lResourceDescription);
	}



	int CResourceManager::CreateTexture3D(int pWidth, int pHeight, int pDepth, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags)
	{
		SResourceDescription lResourceDescription = {};
		lResourceDescription.mDimension = EResourceDimension::eTexture3D;
		lResourceDescription.mAlignment = 0;
		lResourceDescription.mWidth = pWidth;
		lResourceDescription.mHeight = pHeight;
		lResourceDescription.mDepth = pDepth;
		lResourceDescription.mFormat = pFormat;
		lResourceDescription.mHeapType = pType;
		lResourceDescription.mLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		lResourceDescription.mMipLevel = 1;
		lResourceDescription.mFlags = pFlags;
		
		return CreateResource(lResourceDescription);
	}

	int	CResourceManager::StoreSwapchainBuffer(IDXGISwapChain* pSwapchain, int pBufferIndex, int pResourceHandle)
	{
		if (pResourceHandle == -1)
		{
			SResourceInfo& lResourceInfo = mResources[mResourceCount];

			if (!SUCCEEDED(pSwapchain->GetBuffer(pBufferIndex, IID_PPV_ARGS(lResourceInfo.mResource.GetAddressOf()))))
				throw string("getting swapchain buffer fails.");

			lResourceInfo.mState = D3D12_RESOURCE_STATE_PRESENT;

			return mResourceCount++;
		}
		else
		{
			SResourceInfo& lResourceInfo = mResources[pResourceHandle];

			if (!SUCCEEDED(pSwapchain->GetBuffer(pBufferIndex, IID_PPV_ARGS(lResourceInfo.mResource.GetAddressOf()))))
				throw string("getting swapchain buffer fails.");

			lResourceInfo.mState = D3D12_RESOURCE_STATE_PRESENT;

			return pResourceHandle;
		}
	}

	int CResourceManager::CreateDepthTexture(int pWidth, int pHeight, int pArraySize, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags)
	{
		SResourceDescription lResourceDescription = {};
		lResourceDescription.mDimension = EResourceDimension::eTexture2D;
		lResourceDescription.mAlignment = 0;
		lResourceDescription.mWidth = pWidth;
		lResourceDescription.mHeight = pHeight;
		lResourceDescription.mDepth = pArraySize;
		lResourceDescription.mFormat = pFormat;
		lResourceDescription.mHeapType = pType;
		lResourceDescription.mLayout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		lResourceDescription.mMipLevel = 1;
		lResourceDescription.mFlags = pFlags;
		lResourceDescription.mUseClearValue = true;
		lResourceDescription.mUseDepthClearValue = true;
		lResourceDescription.mDepthClearValue = 1.0f;

		return CreateResource(lResourceDescription);
	}

	int	CResourceManager::CreateNullResourcePointer()
	{
		return mResourceCount++;
	}



	void CResourceManager::ChangeState(int pHandle, D3D12_RESOURCE_STATES pAfterState)
	{
		D3D12_RESOURCE_BARRIER lBarrier;
		lBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		lBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		lBarrier.Transition.pResource = mResources[pHandle].mResource.Get();
		lBarrier.Transition.StateBefore = mResources[pHandle].mState;
		lBarrier.Transition.StateAfter = pAfterState;
		lBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		mCommandList->ResourceBarrier(1, &lBarrier);

		mResources[pHandle].mState = pAfterState;
	}

	void CResourceManager::Upload(int pResourceHandle, const void* pData, int pWidth, int pHeight, int pOffsetX, int pOffsetY)
	{
		BYTE* lMemory;

		mResources[pResourceHandle].mResource->Map(0, nullptr, reinterpret_cast<void**>(&lMemory));

		if (pHeight == 1) // buffer or texture1D
		{
			memcpy(&lMemory[pOffsetX], pData, pWidth);
		}

		else //texture2D
		{
			int lRowPitch = Math::Alignment(pWidth, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
			for (int i = 0; i < pHeight; ++i)
			{
				memcpy(&lMemory[pOffsetY * lRowPitch + pOffsetX + i* lRowPitch], (reinterpret_cast<const BYTE*>(pData) + i* pWidth), pWidth);
			}
		}

		mResources[pResourceHandle].mResource->Unmap(0, nullptr);
	}

	void CResourceManager::LoadDDS(int pResoureceHandle, int pUploadResourceHandle, const wchar_t* pFilePath)
	{
		unique_ptr<uint8_t[]> lDDSData;
		vector<D3D12_SUBRESOURCE_DATA> lSubresources;

		if (!SUCCEEDED(DirectX::LoadDDSTextureFromFile(mDevice,  pFilePath, mResources[pResoureceHandle].mResource.ReleaseAndGetAddressOf(), lDDSData, lSubresources)))
			throw string("load dds texture fails.");
		mResources[pResoureceHandle].mState = D3D12_RESOURCE_STATE_COPY_DEST;

		const UINT64 lUploadBufferSize = GetRequiredIntermediateSize(mResources[pResoureceHandle].mResource.Get(), 0,
			static_cast<UINT>(lSubresources.size()));

		// Create the GPU upload buffer.
		CD3DX12_HEAP_PROPERTIES lHeapProps(D3D12_HEAP_TYPE_UPLOAD);

		auto lDesc = CD3DX12_RESOURCE_DESC::Buffer(lUploadBufferSize);

		if (!SUCCEEDED(mDevice->CreateCommittedResource(
			&lHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&lDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(mResources[pUploadResourceHandle].mResource.ReleaseAndGetAddressOf()))))
			throw string("create uploadRes fails.");

		UpdateSubresources(mCopyCommandList, mResources[pResoureceHandle].mResource.Get(), mResources[pUploadResourceHandle].mResource.Get(),
			0, 0, static_cast<UINT>(lSubresources.size()), lSubresources.data());
	

		D3D12_RESOURCE_BARRIER lBarrier;
		lBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		lBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		lBarrier.Transition.pResource = mResources[pResoureceHandle].mResource.Get();
		lBarrier.Transition.StateBefore = mResources[pResoureceHandle].mState;
		lBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		lBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		mCopyCommandList->ResourceBarrier(1, &lBarrier);

		mResources[pResoureceHandle].mState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	}

	int	CResourceManager::CreateResource(const SResourceDescription& pDescrption)
	{
		//heap description
		D3D12_HEAP_PROPERTIES lHeapProperties = {};
		lHeapProperties.VisibleNodeMask = 0;
		lHeapProperties.CreationNodeMask = 0;
		lHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		lHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		switch (pDescrption.mHeapType)
		{
		case EResourceHeapType::eDefault :
			lHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; break;

		case EResourceHeapType::eUpload:
			lHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; break;

		case EResourceHeapType::eReadBack:
			lHeapProperties.Type = D3D12_HEAP_TYPE_READBACK; break;

		// this case is not supported yet.
		case EResourceHeapType::eCustom:
			//lHeapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; break;
			throw std::string("creating buffer with custom type is not supported yet."); break;
		}


		//resrouce description
		D3D12_RESOURCE_DESC lResourceDesc = {};

		switch (pDescrption.mDimension)
		{
		case EResourceDimension::eBuffer :
			lResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; break;

		case EResourceDimension::eTexture1D:
			lResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D; break;

		case EResourceDimension::eTexture2D:
			lResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; break;

		case EResourceDimension::eTexture3D:
			lResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D; break;

		case EResourceDimension::eUnknown:
			lResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_UNKNOWN; break;
		}
		
		lResourceDesc.Alignment = pDescrption.mAlignment;
		lResourceDesc.Width = pDescrption.mWidth;
		lResourceDesc.Height = pDescrption.mHeight;
		lResourceDesc.DepthOrArraySize = pDescrption.mDepth;
		lResourceDesc.MipLevels = pDescrption.mMipLevel;
		lResourceDesc.Format = pDescrption.mFormat;
		lResourceDesc.SampleDesc.Count = 1;
		lResourceDesc.SampleDesc.Quality = 0;
		lResourceDesc.Layout = pDescrption.mLayout;
		lResourceDesc.Flags = pDescrption.mFlags;


		SResourceInfo& lResourceInfo = mResources[mResourceCount];

		if (pDescrption.mHeapType == EResourceHeapType::eUpload)
			lResourceInfo.mState = D3D12_RESOURCE_STATE_GENERIC_READ;

		else if (pDescrption.mHeapType == EResourceHeapType::eReadBack)
			lResourceInfo.mState = D3D12_RESOURCE_STATE_COPY_DEST;

		else
			lResourceInfo.mState = D3D12_RESOURCE_STATE_GENERIC_READ;

		unique_ptr<D3D12_CLEAR_VALUE> lClearValue;

		if (pDescrption.mUseClearValue)
		{
			lClearValue = make_unique<D3D12_CLEAR_VALUE>();
			lClearValue->Format = pDescrption.mFormat;

			if (pDescrption.mUseDepthClearValue)
			{
				lClearValue->DepthStencil.Depth = pDescrption.mDepthClearValue;
				lClearValue->DepthStencil.Stencil = 0;
			}
			else
			{
				for (int i = 0; i < 4; ++i)
					lClearValue->Color[i] = pDescrption.mColor[i];
			}
		}

		if (!SUCCEEDED(mDevice->CreateCommittedResource(&lHeapProperties, D3D12_HEAP_FLAG_NONE, &lResourceDesc,
			lResourceInfo.mState, lClearValue.get(), IID_PPV_ARGS(lResourceInfo.mResource.GetAddressOf()))))
			return -1;
		

		return mResourceCount++;
	}



	void CResourceManager::ReleaseResource(int pResourceHandle)
	{
		mResources[pResourceHandle].mResource.Reset();
	}




	int	CResourceManager::CreateDescriptor(int pResourceHandle, EDescriptorType pType)
	{
		D3D12_RESOURCE_DESC lResourceDesc = mResources[pResourceHandle].mResource->GetDesc();

		if (pType == EDescriptorType::eRTV)
		{
			mDescriptorHandleToHeap[mDescriptorCount] = { 0, mRtvCount};
			mDevice->CreateRenderTargetView(mResources[pResourceHandle].mResource.Get(), nullptr, GetCpuHandle(mDescriptorCount));

			++mRtvCount;

			return mDescriptorCount++;
		}

		else if(pType == EDescriptorType::eDSV)
		{
			mDescriptorHandleToHeap[mDescriptorCount] = { 1, mDsvCount};
			mDevice->CreateDepthStencilView(mResources[pResourceHandle].mResource.Get(), nullptr, GetCpuHandle(mDescriptorCount));

			++mDsvCount;

			return mDescriptorCount++;
		}

		else // CBV, SRV, UAV
		{
			mDescriptorHandleToHeap[mDescriptorCount] = { 2, mCbvSrvUavCount};
			if (pType == EDescriptorType::eCBV)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC lCbvDesc = { };
				lCbvDesc.BufferLocation = mResources[pResourceHandle].mResource->GetGPUVirtualAddress();
				lCbvDesc.SizeInBytes = lResourceDesc.Width;
				mDevice->CreateConstantBufferView(&lCbvDesc, GetCpuHandle(mDescriptorCount));
			}

			else if (pType == EDescriptorType::eSRV)
			{
				D3D12_SRV_DIMENSION lDimension;
				switch(lResourceDesc.Dimension)
				{
				case D3D12_RESOURCE_DIMENSION_BUFFER:
					throw string("buffer dimension resource must be dealt with in other method."); break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE1D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE2D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE3D; break;

				case D3D12_RESOURCE_DIMENSION_UNKNOWN:
					lDimension = D3D12_SRV_DIMENSION_UNKNOWN; break;
				}

				D3D12_SHADER_RESOURCE_VIEW_DESC lSrvDesc = {};
				lSrvDesc.Format = lResourceDesc.Format;
				lSrvDesc.ViewDimension = lDimension;
				lSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				if (lDimension == D3D12_SRV_DIMENSION_TEXTURE1D)
				{
					lSrvDesc.Texture1D.MipLevels = -1;
					lSrvDesc.Texture1D.MostDetailedMip = 0;
					lSrvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
				}
				else if (lDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
				{
					lSrvDesc.Texture2D.MipLevels = -1;
					lSrvDesc.Texture2D.MostDetailedMip = 0;
					lSrvDesc.Texture2D.PlaneSlice = 0;
					lSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
				else if (lDimension == D3D12_SRV_DIMENSION_TEXTURE3D)
				{
					lSrvDesc.Texture3D.MipLevels = -1;
					lSrvDesc.Texture3D.MostDetailedMip = 0;
					lSrvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
				}
				else
				{
					throw string("creating SRV fails.");
				}

				mDevice->CreateShaderResourceView(mResources[pResourceHandle].mResource.Get(), &lSrvDesc, GetCpuHandle(mDescriptorCount));
			}

			else //UAV
			{
				D3D12_UAV_DIMENSION lDimension;
				switch (lResourceDesc.Dimension)
				{
				case D3D12_RESOURCE_DIMENSION_BUFFER:
					throw string("buffer dimension resource must be dealt with in other method."); break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE1D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE2D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE3D; break;

				case D3D12_RESOURCE_DIMENSION_UNKNOWN:
					lDimension = D3D12_UAV_DIMENSION_UNKNOWN; break;
				}

				D3D12_UNORDERED_ACCESS_VIEW_DESC lUavDesc = {};
				lUavDesc.Format = lResourceDesc.Format;
				lUavDesc.ViewDimension = lDimension;
				
				if (lDimension == D3D12_UAV_DIMENSION_TEXTURE1D)
				{
					lUavDesc.Texture1D.MipSlice = 0;
				}
				else if (lDimension == D3D12_UAV_DIMENSION_TEXTURE2D)
				{
					lUavDesc.Texture2D.MipSlice = 0;
					lUavDesc.Texture2D.PlaneSlice = 0;

				}
				else if (lDimension == D3D12_UAV_DIMENSION_TEXTURE3D)
				{
					lUavDesc.Texture3D.FirstWSlice = 0;
					lUavDesc.Texture3D.MipSlice = 0;
					lUavDesc.Texture3D.WSize = -1;
				}
				else
				{
					throw string("creating UAV fails.");
				}

				mDevice->CreateUnorderedAccessView(mResources[pResourceHandle].mResource.Get(), nullptr, &lUavDesc, GetCpuHandle(mDescriptorCount));
			}

			++mCbvSrvUavCount;

			return mDescriptorCount++;
		}
	}



	int	CResourceManager::RecreateDescriptor(int pResourceHandle, EDescriptorType pType, int pDescriptorHandle)
	{
		D3D12_RESOURCE_DESC lResourceDesc = mResources[pResourceHandle].mResource->GetDesc();

		if (pType == EDescriptorType::eRTV)
		{
			mDevice->CreateRenderTargetView(mResources[pResourceHandle].mResource.Get(), nullptr, GetCpuHandle(pDescriptorHandle));

			return pDescriptorHandle;
		}

		else if (pType == EDescriptorType::eDSV)
		{
			mDevice->CreateDepthStencilView(mResources[pResourceHandle].mResource.Get(), nullptr, GetCpuHandle(pDescriptorHandle));


			return pDescriptorHandle;
		}

		else // CBV, SRV, UAV
		{
			if (pType == EDescriptorType::eCBV)
			{
				D3D12_CONSTANT_BUFFER_VIEW_DESC lCbvDesc = { };
				lCbvDesc.BufferLocation = mResources[pResourceHandle].mResource->GetGPUVirtualAddress();
				lCbvDesc.SizeInBytes = lResourceDesc.Width;
				mDevice->CreateConstantBufferView(&lCbvDesc, GetCpuHandle(pDescriptorHandle));
			}

			else if (pType == EDescriptorType::eSRV)
			{
				D3D12_SRV_DIMENSION lDimension;
				switch (lResourceDesc.Dimension)
				{
				case D3D12_RESOURCE_DIMENSION_BUFFER:
					throw string("buffer dimension resource must be dealt with in other method."); break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE1D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE2D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					lDimension = D3D12_SRV_DIMENSION_TEXTURE3D; break;

				case D3D12_RESOURCE_DIMENSION_UNKNOWN:
					lDimension = D3D12_SRV_DIMENSION_UNKNOWN; break;
				}

				D3D12_SHADER_RESOURCE_VIEW_DESC lSrvDesc = {};
				lSrvDesc.Format = lResourceDesc.Format;
				lSrvDesc.ViewDimension = lDimension;
				lSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

				if (lDimension == D3D12_SRV_DIMENSION_TEXTURE1D)
				{
					lSrvDesc.Texture1D.MipLevels = -1;
					lSrvDesc.Texture1D.MostDetailedMip = 0;
					lSrvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
				}
				else if (lDimension == D3D12_SRV_DIMENSION_TEXTURE2D)
				{
					lSrvDesc.Texture2D.MipLevels = -1;
					lSrvDesc.Texture2D.MostDetailedMip = 0;
					lSrvDesc.Texture2D.PlaneSlice = 0;
					lSrvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
				else if (lDimension == D3D12_SRV_DIMENSION_TEXTURE3D)
				{
					lSrvDesc.Texture3D.MipLevels = -1;
					lSrvDesc.Texture3D.MostDetailedMip = 0;
					lSrvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
				}
				else
				{
					throw string("creating SRV fails.");
				}

				mDevice->CreateShaderResourceView(mResources[pResourceHandle].mResource.Get(), &lSrvDesc, GetCpuHandle(pDescriptorHandle));
			}

			else //UAV
			{
				D3D12_UAV_DIMENSION lDimension;
				switch (lResourceDesc.Dimension)
				{
				case D3D12_RESOURCE_DIMENSION_BUFFER:
					throw string("buffer dimension resource must be dealt with in other method."); break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE1D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE2D; break;

				case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
					lDimension = D3D12_UAV_DIMENSION_TEXTURE3D; break;

				case D3D12_RESOURCE_DIMENSION_UNKNOWN:
					lDimension = D3D12_UAV_DIMENSION_UNKNOWN; break;
				}

				D3D12_UNORDERED_ACCESS_VIEW_DESC lUavDesc = {};
				lUavDesc.Format = lResourceDesc.Format;
				lUavDesc.ViewDimension = lDimension;

				if (lDimension == D3D12_UAV_DIMENSION_TEXTURE1D)
				{
					lUavDesc.Texture1D.MipSlice = 0;
				}
				else if (lDimension == D3D12_UAV_DIMENSION_TEXTURE2D)
				{
					lUavDesc.Texture2D.MipSlice = 0;
					lUavDesc.Texture2D.PlaneSlice = 0;

				}
				else if (lDimension == D3D12_UAV_DIMENSION_TEXTURE3D)
				{
					lUavDesc.Texture3D.FirstWSlice = 0;
					lUavDesc.Texture3D.MipSlice = 0;
					lUavDesc.Texture3D.WSize = -1;
				}
				else
				{
					throw string("creating UAV fails.");
				}

				mDevice->CreateUnorderedAccessView(mResources[pResourceHandle].mResource.Get(), nullptr, &lUavDesc, GetCpuHandle(pDescriptorHandle));
			}

			return pDescriptorHandle;
		}
	}



	int CResourceManager::CreateBufferSrv(int pResourceHandle, int pElementNum, int pByteStride)
	{
		D3D12_RESOURCE_DESC lResourceDesc = mResources[pResourceHandle].mResource->GetDesc();

		mDescriptorHandleToHeap[mDescriptorCount] = { 2, mCbvSrvUavCount };

		D3D12_SHADER_RESOURCE_VIEW_DESC lSrvDesc = {};

		lSrvDesc.Format = lResourceDesc.Format;
		lSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		lSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		lSrvDesc.Buffer.FirstElement = 0;
		lSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		lSrvDesc.Buffer.NumElements = pElementNum;
		lSrvDesc.Buffer.StructureByteStride = pByteStride;

		mDevice->CreateShaderResourceView(mResources[pResourceHandle].mResource.Get(), &lSrvDesc, GetCpuHandle(mDescriptorCount));

		++mCbvSrvUavCount;

		return mDescriptorCount++;

	}




	D3D12_CPU_DESCRIPTOR_HANDLE CResourceManager::GetCpuHandle(int pHandle)
	{
		int lDescriptorType = mDescriptorHandleToHeap[pHandle].first;
		int lHandle = mDescriptorHandleToHeap[pHandle].second; //handle to specific descriptor type

		if (lDescriptorType == 0) // RTV
		{
			D3D12_CPU_DESCRIPTOR_HANDLE lCpuHandle = mRtvCpuHandleStart;
			lCpuHandle.ptr += lHandle * mRtvIncSize;

			return lCpuHandle;
		}
		else if (lDescriptorType == 1) // DSV
		{
			D3D12_CPU_DESCRIPTOR_HANDLE lCpuHandle = mDsvCpuHandleStart;
			lCpuHandle.ptr += lHandle * mDsvIncSize;

			return lCpuHandle;
		}
		else // CBV, SRV, UAV
		{
			D3D12_CPU_DESCRIPTOR_HANDLE lCpuHandle = mCbvSrvUavCpuHandleStart;
			lCpuHandle.ptr += lHandle * mCbvSrvUavIncSize;

			return lCpuHandle;
		}
	}



	D3D12_GPU_DESCRIPTOR_HANDLE	CResourceManager::GetGpuHandle(int pHandle)
	{
		int lDescriptorType = mDescriptorHandleToHeap[pHandle].first;
		int lHandle = mDescriptorHandleToHeap[pHandle].second; //handle to specific descriptor type

		if (lDescriptorType == 0) // RTV
		{
			D3D12_GPU_DESCRIPTOR_HANDLE lGpuHandle = mRtvGpuHandleStart;
			lGpuHandle.ptr += lHandle * mRtvIncSize;

			return lGpuHandle;
		}
		else if (lDescriptorType == 1) // DSV
		{
			D3D12_GPU_DESCRIPTOR_HANDLE lGpuHandle = mDsvGpuHandleStart;
			lGpuHandle.ptr += lHandle * mDsvIncSize;

			return lGpuHandle;
		}
		else // CBV, SRV, UAV
		{
			D3D12_GPU_DESCRIPTOR_HANDLE lGpuHandle = mCbvSrvUavGpuHandleStart;
			lGpuHandle.ptr += lHandle * mCbvSrvUavIncSize;

			return lGpuHandle;
		}
	}



	D3D12_GPU_VIRTUAL_ADDRESS CResourceManager::GetGpuVirtualAddress(int pResourceHandle) const
	{
		return mResources[pResourceHandle].mResource->GetGPUVirtualAddress();
	}



	ID3D12DescriptorHeap* CResourceManager::GetDescriptorHeap(EDescriptorType pType) const
	{
		if (pType == EDescriptorType::eRTV)
		{
			return mDescriptorHeaps[0].Get();
		}
		else if (pType == EDescriptorType::eDSV)
		{
			return mDescriptorHeaps[1].Get();
		}
		else
		{
			return mDescriptorHeaps[2].Get();
		}

	}
}