//
// Created by sluggernot on 4/1/25.
//

// --- other includes ---
#include "./shared/vk_types.h"
#include "./shared/vk_initializers.h"

//bootstrap library
#include "VkBootstrap.h"

#ifndef VULKANENGINE_H
#define VULKANENGINE_H



class VulkanEngine {
public:

    // --- omitted ---

    VkInstance _instance;// Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
    VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
    VkDevice _device; // Vulkan device for commands
    VkSurfaceKHR _surface;// Vulkan window surface

    int _frameNumber {0};
    bool stop_rendering{ false };
    VkExtent2D _windowExtent{ 1700 , 900 };
    struct SDL_Window* _window{ nullptr };

    bool _isInitialized = false;

private:

    static VulkanEngine& Get();

    //initializes everything in the engine
    void init();

    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();

};



#endif //VULKANENGINE_H
