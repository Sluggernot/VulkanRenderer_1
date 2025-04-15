#include <iostream>

#include "VulkanEngine.h"

int main()
{
    VulkanEngine engine;
    engine.init();
    engine.run();


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
