#ifndef VERTEXDECL_H
#define VERTEXDECL_H

#include <string>
#include <map>

#include "LuaPlus.h"
#include "Kernel/RefCount.h"

namespace Helix {

class Shader;

class VertexDecl : public ReferenceCountable
{
public:
	VertexDecl();
	~VertexDecl();

	bool	Load(const std::string &path);
	bool	Load(LuaObject &object);

	ID3D10InputLayout *			BuildLayout(Shader *shader);
	ID3D10InputLayout *			GetLayout() { return m_layout; }
	D3D10_INPUT_ELEMENT_DESC *	GetDecl() { return m_desc; }
	int							VertexSize() { return m_vertexSize; }
	bool						HasSemantic(const char *semanticName, int &offset);

private:

	bool	GetFormat(DXGI_FORMAT &format, const std::string &str) const;
	bool	GetClassification(D3D10_INPUT_CLASSIFICATION &classification, const std::string &str) const;

	typedef std::map<const std::string, DXGI_FORMAT>	FormatMap;
	typedef std::map<const std::string, int>			ClassificationMap;

	FormatMap			m_inputLayoutFormatMap;
	ClassificationMap	m_inputLayoutClassificationMap;

	int							m_numElements;
	int							m_vertexSize;
	D3D10_INPUT_ELEMENT_DESC *	m_desc;
	ID3D10InputLayout *			m_layout;
};

} // namespace Helix
#endif // VERTEXDECL_H