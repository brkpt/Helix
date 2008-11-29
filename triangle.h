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

	IDirect3DVertexBuffer9			*m_pVB;           // Buffer to hold vertices
	IDirect3DIndexBuffer9			*m_pIB;			// Index buffer
	IDirect3DVertexDeclaration9		*m_pVertexDecl;   // Vertex format decl
	IDirect3DVertexShader9			*m_pVertexShader; // Vertex Shader
	IDirect3DPixelShader9			*m_pPixelShader;  // Pixel Shader
	IDirect3DTexture9				*m_texture;			// Texture

	ID3DXEffect	*m_pEffect;
	INT         m_iCurrentEffect;
	INT         m_iNumberOfTechniques;
	INT         m_iCurrentTechnique;
	WCHAR       m_strTechnique[512];
};
