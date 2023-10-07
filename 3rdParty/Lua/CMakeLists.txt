include(functions/FetchContent_MakeAvailableExcludeFromAll)

set(LUA_ENABLE_TESTING OFF)
set(LUA_BUILD_COMPILER OFF)
if(DEVILUTIONX_STATIC_LUA)
  set(LUA_ENABLE_SHARED OFF)
else()
  set(LUA_ENABLE_SHARED ON)
endif()

include(FetchContent)
FetchContent_Declare(Lua
    URL https://github.com/walterschell/Lua/archive/88246d621abf7b6fba9332f49229d507f020e450.tar.gz
    URL_HASH MD5=03b76927cb5341ffc53bea12c37ddcca
)
FetchContent_MakeAvailableExcludeFromAll(Lua)

if(ANDROID AND ("${ANDROID_ABI}" STREQUAL "armeabi-v7a" OR "${ANDROID_ABI}" STREQUAL "x86"))
  target_compile_definitions(lua_internal INTERFACE -DLUA_USE_C89)
elseif(NINTENDO_3DS OR VITA OR NINTENDO_SWITCH OR NXDK)
  target_compile_definitions(lua_static PUBLIC -DLUA_USE_C89)
elseif(IOS)
  target_compile_definitions(lua_static PUBLIC -DLUA_USE_IOS)
endif()
