#pragma once
#include "graphics/interface/Imgui.h"
#include "graphics/d3d12/D3D12Device.h"

namespace INF::GFX
{
	class D3D12Imgui : public IImgui
	{
	public:
		D3D12Imgui();
		~D3D12Imgui();

		void CreateContext(INF::IWindow* window, IDeviceManager* deviceManager) override;
		void Shutdown() override;
		void NewFrame() override;
		void Render(ICommandList* commandList) override;
	private:
		D3D12Device* m_device;
		DescriptorIndex m_descriptorIndex;
	};
}