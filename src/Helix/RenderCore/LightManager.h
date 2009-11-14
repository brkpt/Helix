#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

namespace Helix {

class LightManager
{
	static void Create()
	{
		GetInstance();
	}

	LightManager & GetInstance()
	{
		static LightManager s_instance;

		return instance;
	}

	struct Directional
	{
		Directional(std::string &name)
		{
			this->name = name;
			direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

		}
		Directional(std::string &name, D3DXVECTOR3 dir)
		{
			this->name = name;
			direction = dir;
		}

		std::string name;
		D3DXVECTOR3 direction;
	};

	struct Point
	{
	}
private:
	LightManager();
	LightManager(const LightManager &other);
	LightManager & operator=(const LightManager &other);
};

} // namespace Helix

#endif // LIGHTMANAGER_H