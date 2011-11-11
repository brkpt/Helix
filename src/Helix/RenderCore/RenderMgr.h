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

	void SetDXDevice(ID3D11Device *device, ID3D11DeviceContext *context, IDXGISwapChain *swapChain)
	{
		_ASSERT(device != NULL);
		_ASSERT(m_device == NULL);
		m_device = device; 
		m_context = context;
		m_swapChain = swapChain;
	}

	ID3D11Device * GetDevice()
	{
		return m_device;
	}

	ID3D11DeviceContext * GetContext()
	{
		return m_context;
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

	ID3D11Device *			m_device;
	ID3D11DeviceContext *	m_context;
	IDXGISwapChain *		m_swapChain;
};

} // namespace Helix
#endif // RENDERMGR_H