# Enables a number of header file definitions required by ASIO
target_compile_definitions(asio INTERFACE _DEFAULT_SOURCE=ON)

# Missing headers and declarations provided by DevilutionX
target_include_directories(asio BEFORE INTERFACE ${DevilutionX_SOURCE_DIR}/Source/platform/switch/asio/include)
