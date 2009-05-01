#ifndef THREADLOAD_H
#define THREADLOAD_H

#include <string>

namespace Helix { 
	void	InitializeThreadLoader();
	bool	GetLoadThreadShutdown();
	void	ShutdownLoadThread();

	bool	LoadFileAsync(const std::string &file);
}
#endif // THREADLOAD_H