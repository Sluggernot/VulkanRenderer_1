cmake_minimum_required(VERSION 3.29)
project(VulkanRender1)

set(CMAKE_CXX_STANDARD 20)

add_executable(VulkanRender1
        main.cpp
        VulkanEngine.h
        VulkanEngine.cpp
)

find_package(Vulkan REQUIRED)

add_subdirectory(thirdParty)
add_subdirectory(shared)

target_include_directories(VulkanRender1 PRIVATE ${CMAKE_SOURCE_DIR}/shared)

target_include_directories(VulkanRender1 PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(VulkanRender1
        PUBLIC
        vkbootstrap
        PRIVATE
        SDL2
        gmp
        ${Vulkan_LIBRARIES}
#        Vulkan
        volk
)
