include(functions/devilutionx_library)
add_devilutionx_object_library(libdevilutionx_android android.cpp)
target_link_dependencies(libdevilutionx_android PUBLIC
  DevilutionX::SDL
  libdevilutionx_init
  libdevilutionx_mpq
)
