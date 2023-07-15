include(functions/devilutionx_library)
add_devilutionx_object_library(libdevilutionx_android android.cpp)

target_link_libraries(libdevilutionx_android PUBLIC
  DevilutionX::SDL
  SDL_audiolib::SDL_audiolib
)
