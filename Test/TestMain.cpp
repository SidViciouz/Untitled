#include "Renderer.h"
#include <iostream>

using namespace Renderer;

int main()
{
	try {

		CRenderer r;
		r.Initialize();
		r.Run();
	}
	catch (std::string errorMessage)
	{
		std::cout << errorMessage << std::endl;
	}

	while (1);

	return 0;
}