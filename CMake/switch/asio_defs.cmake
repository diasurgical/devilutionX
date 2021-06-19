# Enables a number of header file definitions required by ASIO
target_compile_definitions(asio INTERFACE _DEFAULT_SOURCE=ON)

# Missing headers and declarations provided by DevilutionX
target_include_directories(asio BEFORE INTERFACE CMake/switch/asio/include)

# Defines the pause() function
target_link_libraries(asio INTERFACE rdimon)
