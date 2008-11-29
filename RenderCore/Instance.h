#ifndef INSTANCE_H
#define INSTANCE_H

#include <d3d9.h>
#include <d3dx9.h>

class Instance 
{
public:
	Instance();
	~Instance();

private:
	IDirect3DVertexBuffer9 *		m_pVB;           // Buffer to hold vertices
	IDirect3DIndexBuffer9 *			m_pIB;			// Index buffer
	IDirect3DVertexDeclaration9 *	m_pVertexDecl;   // Vertex format decl
	ID3DXEffect	*					m_pEffect;

};

#endif INSTANCE_H
