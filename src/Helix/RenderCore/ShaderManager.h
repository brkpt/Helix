#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <string>

namespace Helix {
class Shader;

class ShaderManager
{
public:
	static void Create()
	{
		GetInstance();
	}

	static ShaderManager &GetInstance()
	{
		static ShaderManager	instance;

		return instance;
	}

	~ShaderManager();

	Shader *	Load(const std::string &shaderName);
	Shader *	GetShader(const std::string &shaderName);
	void		LoadShared();
	void		SetSharedParameter(const std::string &paramName, D3DXMATRIX &matrix);

private:
	ShaderManager();
	ShaderManager(const ShaderManager &other);
	ShaderManager &	operator=(const ShaderManager &other);

	typedef std::map<const std::string, Shader *>	ShaderMap;
	ShaderMap	m_shaderMap;

	ID3DXEffectPool *	m_effectPool;
};

} // namespace Helix
#endif // SHADERMANAGER_H