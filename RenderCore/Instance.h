#ifndef INSTANCE_H
#define INSTANCE_H

#include <d3d9.h>
#include <d3dx9.h>
#include <new>

class Instance 
{
public:
	Instance();
	~Instance();

	bool	LoadEffectFromFile(const wchar_t *filePath);
	bool	LoadTextureFromFile(const wchar_t *filePath);
	bool	CreateVertexDecl(const D3DVERTEXELEMENT9 * vertexDecl);
	bool	CreateVertexBuffer(const int numVertices, const int vertexSize);
	bool	CreateIndexBuffer(const int numIndices);

	IDirect3DVertexBuffer9 *	GetVertexBuffer()	{ return m_pVB; }
	IDirect3DIndexBuffer9 *		GetIndexBuffer()	{ return m_pIB; }
	ID3DXEffect *				GetEffect()			{ return m_pEffect; }

private:
	IDirect3DVertexBuffer9 *		m_pVB;           // Buffer to hold vertices
	IDirect3DIndexBuffer9 *			m_pIB;			// Index buffer
	IDirect3DVertexDeclaration9 *	m_pVertexDecl;   // Vertex format decl
	ID3DXEffect	*					m_pEffect;
	IDirect3DTexture9 *				m_texture;			// Texture

};

#endif INSTANCE_H
