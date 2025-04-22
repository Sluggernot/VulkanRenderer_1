//
// Created by sluggernot on 4/1/25.
//
#pragma once
// --- other includes ---
#include "./shared/vk_types.h"
#include "./shared/vk_initializers.h"

//bootstrap library
#include "VkBootstrap.h"

struct FrameData {
    VkCommandPool _commandPool;
    VkCommandBuffer _mainCommandBuffer;
};

constexpr unsigned int FRAME_OVERLAP = 2;


class VulkanEngine {
public:

    FrameData _frames[FRAME_OVERLAP];

    FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

    VkQueue _graphicsQueue;
    uint32_t _graphicsQueueFamily;

    VkInstance _instance{};// Vulkan library handle
    VkDebugUtilsMessengerEXT _debug_messenger{};// Vulkan debug output handle
    VkPhysicalDevice _chosenGPU{};// GPU chosen as the default device
    VkDevice _device{}; // Vulkan device for commands
    VkSurfaceKHR _surface{};// Vulkan window surface

    VkSwapchainKHR _swapchain;
    VkFormat _swapchainImageFormat;

    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkExtent2D _swapchainExtent;


    int _frameNumber {0};
    bool stop_rendering{ false };
    VkExtent2D _windowExtent{ 1700 , 900 };
    struct SDL_Window* _window{ nullptr };

    bool _isInitialized = false;

    //initializes everything in the engine
    void init();

    void cleanup();

    void run();

    void draw();

private:

    static VulkanEngine& Get();

    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();

    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain();
};
