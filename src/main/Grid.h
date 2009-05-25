#ifndef GRID_DOT_H

struct LineVertex
{
	float       x,y,z;
	DWORD       Color;
};

class Grid
{
public:
	Grid(void);
	~Grid(void);

	void	Update(float frameTime);
	void	Render(void);

private:

	//// Grid stuff
	//IDirect3DVertexBuffer9			*m_pLineVerts;
	//IDirect3DIndexBuffer9			*m_pLineIndices;
	//ID3DXEffect						*m_pLineEffect;
	//INT								m_iCurrentEffect;
	//INT								m_iNumberOfTechniques;
	//INT								m_iCurrentTechnique;
	//WCHAR							m_strTechnique[512];
	//IDirect3DVertexDeclaration9		*m_pVertexDecl;   // Vertex format decl
};

#endif

