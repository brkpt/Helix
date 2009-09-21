#ifndef RENDERMGR_H
#define RENDERMGR_H

namespace Helix {

class RenderMgr
{
public:
	static RenderMgr & GetInstance()
	{
		static RenderMgr	instance;

		return instance;
	}

	void SetDXDevice(ID3D10Device *device, IDXGISwapChain *swapChain)
	{
		_ASSERT(device != NULL);
		_ASSERT(m_device == NULL);
		m_device = device; 
		m_swapChain = swapChain;
	}

	ID3D10Device * GetDevice()
	{
		return m_device;
	}

	IDXGISwapChain *GetSwapChain()
	{
		return m_swapChain;
	}

private:
	RenderMgr()
	: m_device(NULL)
	{}

	RenderMgr(const RenderMgr &other)
	: m_device(NULL)
	, m_swapChain(NULL)
	{}

	~RenderMgr() {}
	RenderMgr & operator =(const RenderMgr &other) {}

	ID3D10Device *		m_device;
	IDXGISwapChain *	m_swapChain;
};

} // namespace Helix
#endif // RENDERMGR_H