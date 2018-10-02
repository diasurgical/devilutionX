find_package(SDL2 REQUIRED)
# WORKAROUND: Arch Linux SDL2 cmake config not setting this variable
if(NOT SDL2_LIBRARIES)
    # Note: you will probably need to install multilib/lib32-dbus on Arch
    set(SDL2_LIBRARIES SDL2::SDL2)
endif()

# WORKAROUND: Issue with Ubuntu 16.04 having whitespace (CMP0004)
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
