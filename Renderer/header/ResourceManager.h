#pragma once

#include "Common.h"

namespace Renderer
{
	enum class EResourceHeapType
	{
		eDefault,
		eUpload,
		eReadBack,
		eCustom
	};

	enum class EResourceDimension
	{
		eBuffer,
		eTexture1D,
		eTexture2D,
		eTexture3D,
		eUnknown
	};

	enum class EDescriptorType
	{
		eCBV,
		eSRV,
		eUAV,
		eRTV,
		eDSV
	};

	struct SResourceDescription
	{
		EResourceHeapType					mHeapType;
		EResourceDimension					mDimension;
		int									mWidth;
		int									mHeight;
		int									mDepth;
		DXGI_FORMAT							mFormat;
		D3D12_TEXTURE_LAYOUT				mLayout;
		D3D12_RESOURCE_FLAGS				mFlags;
		int									mAlignment;
		int									mMipLevel;
	};

	struct SResourceInfo
	{
		D3D12_RESOURCE_STATES				mState;
		ComPtr<ID3D12Resource>				mResource;
	};

	class CResourceManager
	{
	public:
											CResourceManager(ID3D12Device* pDevice);
											~CResourceManager();

		/*
		* create buffer and return resource handle.
		*/
		int									CreateBuffer(int pByteSize, EResourceHeapType pType);
		int									CreateTexture1D(int pWidth, DXGI_FORMAT pFormat, EResourceHeapType pType,D3D12_RESOURCE_FLAGS pFlags);
		int									CreateTexture2D(int pWidth, int pHeight, int pArraySize, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags);
		int									CreateTexture3D(int pWidth, int pHeight, int pDepth, DXGI_FORMAT pFormat, EResourceHeapType pType, D3D12_RESOURCE_FLAGS pFlags);

		/*
		* create descriptor and return descriptor handle.
		*/
		int									CreateDescriptor(int pResourceHandle, EDescriptorType pType); //buffer, texture2d array and textureCube must be dealt with in other method.
		D3D12_CPU_DESCRIPTOR_HANDLE			GetCpuHandle(EDescriptorType pType, int pHandle);
		D3D12_GPU_DESCRIPTOR_HANDLE			GetGpuHandle(EDescriptorType pType, int pHandle);

	private:
		/*
		* create buffer and return decriptor handle.
		*/
		int									CreateResource(const SResourceDescription& pDescrption);

		array<SResourceInfo, MAX_RESOURCE_NUM> mResources;
		int									mResourceCount = 0;

		array<ComPtr<ID3D12DescriptorHeap>, 3> mDescriptorHeaps;
		array<pair<int,int>, MAX_DESCRIPTOR_NUM> mDescriptorHandleToHeap; // descriptor Handle -> corresponding <desciptor heap, index>
		int									mDescriptorCount = 0;
		int									mRtvCount = 0;
		int									mDsvCount = 0;
		int									mCbvSrvUavCount = 0;

		UINT								mRtvIncSize = 0;
		UINT								mDsvIncSize = 0;
		UINT								mCbvSrvUavIncSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE			mRtvCpuHandleStart;
		D3D12_GPU_DESCRIPTOR_HANDLE			mRtvGpuHandleStart;
		D3D12_CPU_DESCRIPTOR_HANDLE			mDsvCpuHandleStart;
		D3D12_GPU_DESCRIPTOR_HANDLE			mDsvGpuHandleStart;
		D3D12_CPU_DESCRIPTOR_HANDLE			mCbvSrvUavCpuHandleStart;
		D3D12_GPU_DESCRIPTOR_HANDLE			mCbvSrvUavGpuHandleStart;

		ID3D12Device*						mDevice;
	};
}