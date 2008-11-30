#ifndef RENDERMGR_H
#define RENDERMGR_H

class RenderMgr
{
public:
	static RenderMgr & GetInstance()
	{
		static RenderMgr	instance;

		return instance;
	}

	void SetDXDevice(IDirect3DDevice9 *device)
	{
		_ASSERT(device != NULL);
		_ASSERT(m_device == NULL);
		m_device = device; 
	}

	IDirect3DDevice9 * GetDevice()
	{
		return m_device;
	}

private:
	RenderMgr()
	: m_device(NULL)
	{}

	RenderMgr(const RenderMgr &other) {}
	~RenderMgr() {}
	RenderMgr & operator =(const RenderMgr &other) {}

	IDirect3DDevice9 *	m_device;
};
#endif // RENDERMGR_H