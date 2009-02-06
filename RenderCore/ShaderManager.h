#ifndef SHADERMANGER_H
#define SHADERMANAGER_H

#include <map>
#include <string>

class Shader;

class ShaderManager
{
public:
	static ShaderManager &GetInstance()
	{
		static ShaderManager	instance;

		return instance;
	}

	~ShaderManager();

	Shader *	Load(const std::string &shaderName);
	Shader *	GetShader(const std::string &shaderName);

private:
	ShaderManager();
	ShaderManager(const ShaderManager &other);
	ShaderManager &	operator=(const ShaderManager &other);

	typedef std::map<const std::string, Shader *>	ShaderMap;
	ShaderMap	m_shaderMap;
};

#endif // SHADERMANAGER_H