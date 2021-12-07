target_compile_definitions(asio INTERFACE
  ASIO_DISABLE_THREADS=ON
  ASIO_HAS_UNISTD_H=ON)

# Missing headers and declarations provided by DevilutionX
target_include_directories(asio BEFORE INTERFACE ${CMAKE_CURRENT_LIST_DIR}/asio/include)
