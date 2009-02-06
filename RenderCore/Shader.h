#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader
{
public:
	Shader();
	~Shader();

	bool	LoadShader(const std::string &path);

private:
}
#endif // SHADER_H
