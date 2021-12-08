############################################################################
# Various macros for 3DS homebrews tools
#
# add_3dsx_target
# ^^^^^^^^^^^^^^^
#
# This macro has two signatures :
#
# ## add_3dsx_target(target [NO_SMDH])
#
# Adds a target that generates a .3dsx file from `target`. If NO_SMDH is specified, no .smdh file will be generated.
#
# You can set the following variables to change the SMDH file :
#
# * APP_TITLE is the name of the app stored in the SMDH file (Optional)
# * APP_DESCRIPTION is the description of the app stored in the SMDH file (Optional)
# * APP_AUTHOR is the author of the app stored in the SMDH file (Optional)
# * APP_ICON is the filename of the icon (.png), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - $(target).png
#     - icon.png
#     - $(libctru folder)/default_icon.png
#
# ## add_3dsx_target(target APP_TITLE APP_DESCRIPTION APP_AUTHOR [APP_ICON])
#
# This version will produce the SMDH with tha values passed as arguments. Tha APP_ICON is optional and follows the same rule as the other version of `add_3dsx_target`.
#
# add_cia_target(target RSF IMAGE SOUND [APP_TITLE APP_DESCRIPTION APP_AUTHOR [APP_ICON]])
# ^^^^^^^^^^^^^^
#
# Same as add_3dsx_target but for CIA files.
#
# RSF is the .rsf file to be given to makerom.
# IMAGE is either a .png or a cgfximage file.
# SOUND is either a .wav or a cwavaudio file.
#
# add_netload_target(name target_or_file)
# ^^^^^^^^^^^^^^^^^^
#
# Adds a target `name` that sends a .3dsx using the homebrew launcher netload system (3dslink).
# target_or_file is either the name of a target or of file.
#
# add_binary_library(target input1 [input2 ...])
# ^^^^^^^^^^^^^^^^^^
#
#    /!\ Requires ASM to be enabled ( `enable_language(ASM)` or `project(yourprojectname C CXX ASM)`)
#
# Converts the files given as input to arrays of their binary data. This is useful to embed resources into your project.
# For example, logo.bmp will generate the array `u8 logo_bmp[]` and its size `logo_bmp_size`. By linking this library, you
# will also have access to a generated header file called `logo_bmp.h` which contains the declarations you need to use it.
#
#   Note : All dots in the filename are converted to `_`, and if it starts with a number, `_` will be prepended.
#   For example 8x8.gas.tex would give the name _8x8_gas_tex.
#
# target_embed_file(target input1 [input2 ...])
# ^^^^^^^^^^^^^^^^^
#
# Same as add_binary_library(tempbinlib input1 [input2 ...]) + target_link_libraries(target tempbinlib)
#
# add_shbin(output input [entrypoint] [shader_type])
# ^^^^^^^^^^^^^^^^^^^^^^^
#
# Assembles the shader given as `input` into the file `output`. No file extension is added.
# You can choose the shader assembler by setting SHADER_AS to `picasso` or `nihstro`.
#
# If `nihstro` is set as the assembler, entrypoint and shader_type will be used.
# entrypoint is set to `main` by default
# shader_type can be either VSHADER or GSHADER. By default it is VSHADER.
#
# generate_shbins(input1 [input2 ...])
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Assemble all the shader files given as input into .shbin files. Those will be located in the folder `shaders` of the build directory.
# The names of the output files will be <name of input without longest extension>.shbin. vshader.pica will output shader.shbin but shader.vertex.pica will output shader.shbin too.
#
# add_shbin_library(target input1 [input2 ...])
# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
#    /!\ Requires ASM to be enabled ( `enable_language(ASM)` or `project(yourprojectname C CXX ASM)`)
#
# This is the same as calling generate_shbins and add_binary_library. This is the function to be used to reproduce devkitArm makefiles behaviour.
# For example, add_shbin_library(shaders data/my1stshader.vsh.pica) will generate the target library `shaders` and you
# will be able to use the shbin in your program by linking it, including `my1stshader_pica.h` and using `my1stshader_pica[]` and `my1stshader_pica_size`.
#
# target_embed_shader(target input1 [input2 ...])
# ^^^^^^^^^^^^^^^^^
#
# Same as add_shbin_library(tempbinlib input1 [input2 ...]) + target_link_libraries(target tempbinlib)
#
############################################################################

if(NOT NINTENDO_3DS)
    message(WARNING "Those tools can only be used if you are using the 3DS toolchain file. Please erase this build directory or create another one, and then use -DCMAKE_TOOLCHAIN_FILE=DevkitArm3DS.cmake when calling cmake for the 1st time. For more information, see the Readme.md for more information.")
endif()

get_filename_component(__tools3dsdir ${CMAKE_CURRENT_LIST_FILE} PATH) # Used to locate files to be used with configure_file

message(STATUS "Looking for 3ds tools...")

##############
## 3DSXTOOL ##
##############
if(NOT _3DSXTOOL)
    # message(STATUS "Looking for 3dsxtool...")
    find_program(_3DSXTOOL 3dsxtool ${DEVKITPRO}/tools/bin)
    if(_3DSXTOOL)
        message(STATUS "3dsxtool: ${_3DSXTOOL} - found")
    else()
        message(WARNING "3dsxtool - not found")
    endif()
endif()


##############
## SMDHTOOL ##
##############
if(NOT SMDHTOOL)
    # message(STATUS "Looking for smdhtool...")
    find_program(SMDHTOOL smdhtool ${DEVKITPRO}/tools/bin)
    if(SMDHTOOL)
        message(STATUS "smdhtool: ${SMDHTOOL} - found")
    else()
        message(WARNING "smdhtool - not found")
    endif()
endif()

################
## BANNERTOOL ##
################
if(NOT BANNERTOOL)
    # message(STATUS "Looking for bannertool...")
    find_program(BANNERTOOL bannertool ${DEVKITPRO}/tools/bin /usr/local/bin)
    if(BANNERTOOL)
        message(STATUS "bannertool: ${BANNERTOOL} - found")
    else()
        message(WARNING "bannertool - not found")
    endif()
endif()

set(FORCE_SMDHTOOL FALSE CACHE BOOL "Force the use of smdhtool instead of bannertool")

#############
## MAKEROM ##
#############
if(NOT MAKEROM)
    # message(STATUS "Looking for makerom...")
    find_program(MAKEROM makerom ${DEVKITPRO}/tools/bin /usr/local/bin)
    if(MAKEROM)
        message(STATUS "makerom: ${MAKEROM} - found")
    else()
        message(WARNING "makerom - not found")
    endif()
endif()



#############
##  STRIP  ##
#############
if(NOT STRIP)
    # message(STATUS "Looking for strip...")
    find_program(STRIP arm-none-eabi-strip ${DEVKITARM}/bin)
    if(STRIP)
        message(STATUS "strip: ${STRIP} - found")
    else()
        message(WARNING "strip - not found")
    endif()
endif()



#############
##  BIN2S  ##
#############
if(NOT BIN2S)
    # message(STATUS "Looking for bin2s...")
    find_program(BIN2S bin2s ${DEVKITPRO}/tools/bin)
    if(BIN2S)
        message(STATUS "bin2s: ${BIN2S} - found")
    else()
        message(WARNING "bin2s - not found")
    endif()
endif()

###############
##  3DSLINK  ##
###############
if(NOT _3DSLINK)
    # message(STATUS "Looking for 3dslink...")
    find_program(_3DSLINK 3dslink ${DEVKITPRO}/tools/bin)
    if(_3DSLINK)
        message(STATUS "3dslink: ${_3DSLINK} - found")
    else()
        message(WARNING "3dslink - not found")
    endif()
endif()

#############
## PICASSO ##
#############
if(NOT PICASSO_EXE)
    # message(STATUS "Looking for Picasso...")
    find_program(PICASSO_EXE picasso ${DEVKITPRO}/tools/bin)
    if(PICASSO_EXE)
        message(STATUS "Picasso: ${PICASSO_EXE} - found")
        set(SHADER_AS picasso CACHE STRING "The shader assembler to be used. Allowed values are 'none', 'picasso' or 'nihstro'")
    else()
        message(STATUS "Picasso - not found")
    endif()
endif()


#############
## NIHSTRO ##
#############

if(NOT NIHSTRO_AS)
    # message(STATUS "Looking for nihstro...")
    find_program(NIHSTRO_AS nihstro ${DEVKITPRO}/tools/bin)
    if(NIHSTRO_AS)
        message(STATUS "nihstro: ${NIHSTRO_AS} - found")
        set(SHADER_AS nihstro CACHE STRING "The shader assembler to be used. Allowed values are 'none', 'picasso' or 'nihstro'")
    else()
        message(STATUS "nihstro - not found")
    endif()
endif()

set(SHADER_AS none CACHE STRING "The shader assembler to be used. Allowed values are 'none', 'picasso' or 'nihstro'")

###############################
###############################
########    MACROS    #########
###############################
###############################


###################
### EXECUTABLES ###
###################


function(__add_smdh target APP_TITLE APP_DESCRIPTION APP_AUTHOR APP_ICON)
    if(BANNERTOOL AND NOT FORCE_SMDHTOOL)
        set(__SMDH_COMMAND ${BANNERTOOL} makesmdh -s ${APP_TITLE} -l ${APP_DESCRIPTION}  -p ${APP_AUTHOR} -i ${APP_ICON} -o ${CMAKE_CURRENT_BINARY_DIR}/${target})
    else()
        set(__SMDH_COMMAND ${SMDHTOOL} --create ${APP_TITLE} ${APP_DESCRIPTION} ${APP_AUTHOR} ${APP_ICON} ${CMAKE_CURRENT_BINARY_DIR}/${target})
    endif()
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}
                        COMMAND ${__SMDH_COMMAND}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                        DEPENDS ${APP_ICON}
                        VERBATIM
    )
endfunction()

function(add_3dsx_target target)
    get_filename_component(target_we ${target} NAME_WE)
    if((NOT (${ARGC} GREATER 1 AND "${ARGV1}" STREQUAL "NO_SMDH") ) OR (${ARGC} GREATER 3) )
        if(${ARGC} GREATER 3)
            set(APP_TITLE ${ARGV1})
            set(APP_DESCRIPTION ${ARGV2})
            set(APP_AUTHOR ${ARGV3})
        endif()
        if(${ARGC} EQUAL 5)
            set(APP_ICON ${ARGV4})
        endif()
        if(NOT APP_TITLE)
            set(APP_TITLE ${target})
        endif()
        if(NOT APP_DESCRIPTION)
            set(APP_DESCRIPTION "Built with devkitARM & libctru")
        endif()
        if(NOT APP_AUTHOR)
            set(APP_AUTHOR "Unspecified Author")
        endif()
        if(NOT APP_ICON)
            if(EXISTS ${target}.png)
                set(APP_ICON ${target}.png)
            elseif(EXISTS icon.png)
                set(APP_ICON icon.png)
            elseif(CTRULIB)
                set(APP_ICON ${CTRULIB}/default_icon.png)
            else()
                message(FATAL_ERROR "No icon found ! Please use NO_SMDH or provide some icon.")
            endif()
        endif()
        if( NOT ${target_we}.smdh)
            __add_smdh(${target_we}.smdh ${APP_TITLE} ${APP_DESCRIPTION} ${APP_AUTHOR} ${APP_ICON})
        endif()
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.3dsx
                        COMMAND ${_3DSXTOOL} $<TARGET_FILE:${target}> ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.3dsx --smdh=${CMAKE_CURRENT_BINARY_DIR}/${target_we}.smdh --romfs=${CMAKE_CURRENT_BINARY_DIR}/romfs
                        DEPENDS ${target} ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.smdh
                        VERBATIM
        )
    else()
        message(STATUS "No smdh file will be generated")
        add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.3dsx
                        COMMAND ${_3DSXTOOL} $<TARGET_FILE:${target}> ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.3dsx --romfs=${CMAKE_CURRENT_BINARY_DIR}/romfs
                        DEPENDS ${target}
                        VERBATIM
        )
    endif()
    add_custom_target(${target_we}_3dsx ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.3dsx)
    #set_target_properties(${target} PROPERTIES LINK_FLAGS "-specs=3dsx.specs")
endfunction()

function(__add_ncch_banner target IMAGE SOUND)
    if(IMAGE MATCHES ".*\\.png$")
        set(IMG_PARAM -i ${IMAGE})
    else()
        set(IMG_PARAM -ci ${IMAGE})
    endif()
    if(SOUND MATCHES ".*\\.wav$")
        set(SND_PARAM -a ${SOUND})
    else()
        set(SND_PARAM -ca ${SOUND})
    endif()
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}
                        COMMAND ${BANNERTOOL} makebanner -o ${CMAKE_CURRENT_BINARY_DIR}/${target} ${IMG_PARAM} ${SND_PARAM}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                        DEPENDS ${IMAGE} ${SOUND}
                        VERBATIM
    )
endfunction()

function(add_cia_target target RSF IMAGE SOUND)
    get_filename_component(target_we ${target} NAME_WE)
    if(${ARGC} GREATER 6)
        set(APP_TITLE ${ARGV4})
        set(APP_DESCRIPTION ${ARGV5})
        set(APP_AUTHOR ${ARGV6})
    endif()
    if(${ARGC} EQUAL 8)
        set(APP_ICON ${ARGV7})
    endif()
    if(NOT APP_TITLE)
        set(APP_TITLE ${target})
    endif()
    if(NOT APP_DESCRIPTION)
        set(APP_DESCRIPTION "Built with devkitARM & libctru")
    endif()
    if(NOT APP_AUTHOR)
        set(APP_AUTHOR "Unspecified Author")
    endif()
    if(NOT APP_ICON)
        if(EXISTS ${target}.png)
            set(APP_ICON ${target}.png)
        elseif(EXISTS icon.png)
            set(APP_ICON icon.png)
        elseif(CTRULIB)
            set(APP_ICON ${CTRULIB}/default_icon.png)
        else()
            message(FATAL_ERROR "No icon found ! Please use NO_SMDH or provide some icon.")
        endif()
    endif()
    if( NOT ${target_we}.smdh)
        __add_smdh(${target_we}.smdh ${APP_TITLE} ${APP_DESCRIPTION} ${APP_AUTHOR} ${APP_ICON})
    endif()
    __add_ncch_banner(${target_we}.bnr ${IMAGE} ${SOUND})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.cia
                        COMMAND ${STRIP} -o $<TARGET_FILE:${target}>-stripped $<TARGET_FILE:${target}>
                        COMMAND ${MAKEROM}     -f cia
                                            -target t
                                            -exefslogo
                                            -o ${target_we}.cia
                                            -elf $<TARGET_FILE:${target}>-stripped
                                            -rsf ${RSF}
                                            -banner ${target_we}.bnr
                                            -icon ${target_we}.smdh
                        DEPENDS ${target} ${RSF} ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.bnr ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.smdh
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        VERBATIM
    )

    add_custom_target(${target_we}_cia ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/${target_we}.cia)
    #set_target_properties(${target} PROPERTIES LINK_FLAGS "-specs=3dsx.specs")
endfunction()

macro(add_netload_target name target)
    set(NETLOAD_IP "" CACHE STRING "The ip address of the 3ds when using netload.")
    if(NETLOAD_IP)
        set(__NETLOAD_IP_OPTION -a ${NETLOAD_IP})
    endif()
    if(NOT TARGET ${target})
        message("NOT ${target}")
        set(FILE ${target})
    else()
        set(FILE ${CMAKE_CURRENT_BINARY_DIR}/${target}.3dsx)
    endif()
    add_custom_target(${name}
                    COMMAND ${_3DSLINK} ${FILE} ${__NETLOAD_IP_OPTION}
                    DEPENDS  ${FILE}
    )
endmacro()

######################
### File embedding ###
######################

macro(add_binary_library libtarget)
    if(NOT ${ARGC} GREATER 1)
        message(FATAL_ERROR "add_binary_library : Argument error (no input files)")
    endif()
    get_cmake_property(ENABLED_LANGUAGES ENABLED_LANGUAGES)
    if(NOT ENABLED_LANGUAGES MATCHES ".*ASM.*")
        message(FATAL_ERROR "You have to enable ASM in order to use add_binary_library (or any target_embed_* which relies on it). Use enable_language(ASM) in your CMakeLists. Currently enabled languages are ${ENABLED_LANGUAGES}")
    endif()


    foreach(__file ${ARGN})
        get_filename_component(__file_wd ${__file} NAME)
        string(REGEX REPLACE "^([0-9])" "_\\1" __BIN_FILE_NAME ${__file_wd}) # add '_' if the file name starts by a number
        string(REGEX REPLACE "[-./]" "_" __BIN_FILE_NAME ${__BIN_FILE_NAME})

        #Generate the header file
        configure_file(${__tools3dsdir}/bin2s_header.h.in ${CMAKE_CURRENT_BINARY_DIR}/${libtarget}_include/${__BIN_FILE_NAME}.h)
    endforeach()

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/binaries_asm)
    # Generate the assembly file, and create the new target
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/binaries_asm/${libtarget}.s
                        COMMAND ${BIN2S} ${ARGN} > ${CMAKE_CURRENT_BINARY_DIR}/binaries_asm/${libtarget}.s
                        DEPENDS ${ARGN}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    add_library(${libtarget} ${CMAKE_CURRENT_BINARY_DIR}/binaries_asm/${libtarget}.s)
    target_include_directories(${libtarget} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/${libtarget}_include)
endmacro()

macro(target_embed_file _target)
    if(NOT ${ARGC} GREATER 1)
        message(FATAL_ERROR "target_embed_file : Argument error (no input files)")
    endif()
    get_filename_component(__1st_file_wd ${ARGV1} NAME)
    add_binary_library(__${_target}_embed_${__1st_file_wd} ${ARGN})
    target_link_libraries(${_target} __${_target}_embed_${__1st_file_wd})
endmacro()

###################
##### SHADERS #####
###################

macro(add_shbin OUTPUT INPUT )

    if(SHADER_AS STREQUAL "picasso")

        if(${ARGC} GREATER 2)
            message(WARNING "Picasso doesn't support changing the entrypoint or shader type")
        endif()
        add_custom_command(OUTPUT ${OUTPUT} COMMAND ${PICASSO_EXE} -o ${OUTPUT} ${INPUT} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    elseif(SHADER_AS STREQUAL "nihstro")
        if(NOT NIHSTRO_AS)
            message(SEND_ERROR "SHADER_AS is set to nihstro, but nihstro wasn't found. Please set NIHSTRO_AS.")
        endif()
        if(${ARGC} GREATER 2)
            if(${ARGV2} EQUAL GSHADER)
                set(SHADER_TYPE_FLAG "-g")
            elseif(NOT ${ARGV2} EQUAL VSHADER)
                set(_ENTRYPOINT ${ARGV2})
            endif()
        endif()
        if(${ARGC} GREATER 3)
            if(${ARGV2} EQUAL GSHADER)
                set(SHADER_TYPE_FLAG "-g")
            elseif(NOT ${ARGV3} EQUAL VSHADER)
                set(_ENTRYPOINT ${ARGV3})
            endif()
        endif()
        if(NOT _ENTRYPOINT)
            set(_ENTRYPOINT "main")
        endif()
        add_custom_command(OUTPUT ${OUTPUT} COMMAND ${NIHSTRO_AS} ${INPUT} -o ${OUTPUT} -e ${_ENTRYPOINT} ${SHADER_TYPE_FLAG} WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    else()
        message(FATAL_ERROR "Please set SHADER_AS to 'picasso' or 'nihstro' if you use the shbin feature.")
    endif()

endmacro()

function(generate_shbins)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    foreach(__shader_file ${ARGN})
        get_filename_component(__shader_file_we ${__shader_file} NAME_WE)
        #Generate the shbin file
        list(APPEND __SHADERS_BIN_FILES ${CMAKE_CURRENT_BINARY_DIR}/shaders/${__shader_file_we}.shbin)
        add_shbin(${CMAKE_CURRENT_BINARY_DIR}/shaders/${__shader_file_we}.shbin ${__shader_file})
    endforeach()
endfunction()

function(add_shbin_library libtarget)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders)
    foreach(__shader_file ${ARGN})
        get_filename_component(__shader_file_we ${__shader_file} NAME_WE)
        #Generate the shbin file
        list(APPEND __SHADERS_BIN_FILES ${CMAKE_CURRENT_BINARY_DIR}/shaders/${__shader_file_we}.shbin)
        add_shbin(${CMAKE_CURRENT_BINARY_DIR}/shaders/${__shader_file_we}.shbin ${__shader_file})
    endforeach()
    add_binary_library(${libtarget} ${__SHADERS_BIN_FILES})
endfunction()


macro(target_embed_shader _target)
    if(NOT ${ARGC} GREATER 1)
        message(FATAL_ERROR "target_embed_shader : Argument error (no input files)")
    endif()
    get_filename_component(__1st_file_wd ${ARGV1} NAME)
    add_shbin_library(__${_target}_embed_${__1st_file_wd} ${ARGN})
    target_link_libraries(${_target} __${_target}_embed_${__1st_file_wd})
endmacro()
