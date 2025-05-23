cmake_minimum_required(VERSION 3.29)
project(VulkanRender1)

set(CMAKE_CXX_STANDARD 20)

add_executable(VulkanRender1
        main.cpp
        VulkanEngine.h
        VulkanEngine.cpp
)

find_package(Vulkan REQUIRED)

add_subdirectory(shared)
add_subdirectory(thirdParty)

target_include_directories(VulkanRender1 PRIVATE ${Vulkan_INCLUDE_DIRS})
target_include_directories(VulkanRender1 PRIVATE ${CMAKE_SOURCE_DIR}/shared)

target_link_libraries(VulkanRender1
        PUBLIC
        vkbootstrap
        fmt::fmt
        volk
        PRIVATE
        SDL2
        gmp
        vkguide_shared
        ${Vulkan_LIBRARIES}
        vulkan
)
