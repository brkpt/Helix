#ifndef TRIANGLE_H
#define TRIANGLE_H

namespace Helix {
	class Instance;
}

class Triangle
{
public:
	struct TriangleVertex
	{
		float       x,y,z;
		float		u,v;
	};

	Triangle(void);
	~Triangle(void);

	void	Update(float frameTime);
	void	Render(void);

private:

	void	OrthoNormalization(D3DXMATRIX &mat);

	Helix::Instance *	m_instance;
	INT					m_iCurrentEffect;
	INT					m_iNumberOfTechniques;
	INT					m_iCurrentTechnique;
	WCHAR				m_strTechnique[512];
};

#endif // TRIANGLE_H