if(NOT DEFINED DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY)
  set(DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/assets")
endif()

set(devilutionx_langs bg cs da de es fr hr it ja ko pl pt_BR ro ru uk sv zh_CN zh_TW)
if(USE_GETTEXT_FROM_VCPKG)
  # vcpkg doesn't add its own tools directory to the search path
  list(APPEND Gettext_ROOT ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/tools/gettext/bin)
endif()
find_package(Gettext)
if (Gettext_FOUND)
  file(MAKE_DIRECTORY "${DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY}")
  foreach(lang ${devilutionx_langs})
    set(_po_file "${CMAKE_CURRENT_SOURCE_DIR}/Translations/${lang}.po")
    set(_gmo_file "${DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY}/${lang}.gmo")
    set(_lang_target devilutionx_lang_${lang})
    add_custom_command(
      COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" -o "${_gmo_file}" "${_po_file}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT "${_gmo_file}"
      MAIN_DEPENDENCY "${_po_file}"
      VERBATIM
    )
    add_custom_target("${_lang_target}" DEPENDS "${_gmo_file}")
    list(APPEND devilutionx_lang_targets "${_lang_target}")
    list(APPEND devilutionx_lang_files "${_gmo_file}")

    if(APPLE)
      set_source_files_properties("${_gmo_file}" PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
        XCODE_EXPLICIT_FILE_TYPE compiled)
      add_dependencies(libdevilutionx "${_lang_target}")
      add_dependencies(${BIN_TARGET} "${_lang_target}")
      target_sources(${BIN_TARGET} PRIVATE "${_gmo_file}")
    endif()

    if(VITA)
      list(APPEND VITA_TRANSLATIONS_LIST "FILE" "${_gmo_file}" "assets/${lang}.gmo")
    endif()
  endforeach()
endif()

set(devilutionx_assets
  data/boxleftend.pcx
  data/boxmiddle.pcx
  data/boxrightend.pcx
  data/charbg.pcx
  data/dirtybuc.pcx
  data/dirtybucp.pcx
  data/healthbox.pcx
  data/health.pcx
  data/panel8buc.pcx
  data/panel8bucp.pcx
  data/monstertags.pcx
  data/resistance.pcx
  data/talkbutton.pcx
  data/xpbar.pcx
  fonts/12-00.bin
  fonts/12-00.pcx
  fonts/12-01.bin
  fonts/12-01.pcx
  fonts/12-02.bin
  fonts/12-02.pcx
  fonts/12-03.bin
  fonts/12-03.pcx
  fonts/12-04.bin
  fonts/12-04.pcx
  fonts/12-1f4.pcx
  fonts/12-1f6.pcx
  fonts/12-1f9.pcx
  fonts/12-26.pcx
  fonts/22-00.bin
  fonts/22-00.pcx
  fonts/22-01.bin
  fonts/22-01.pcx
  fonts/22-02.bin
  fonts/22-02.pcx
  fonts/22-03.bin
  fonts/22-03.pcx
  fonts/22-04.bin
  fonts/22-04.pcx
  fonts/22-05.bin
  fonts/22-05.pcx
  fonts/24-00.bin
  fonts/24-00.pcx
  fonts/24-01.bin
  fonts/24-01.pcx
  fonts/24-02.bin
  fonts/24-02.pcx
  fonts/24-03.bin
  fonts/24-03.pcx
  fonts/24-04.bin
  fonts/24-04.pcx
  fonts/24-1f4.pcx
  fonts/24-1f6.pcx
  fonts/24-1f9.pcx
  fonts/24-26.pcx
  fonts/30-00.bin
  fonts/30-00.pcx
  fonts/30-01.bin
  fonts/30-01.pcx
  fonts/30-02.bin
  fonts/30-02.pcx
  fonts/30-03.bin
  fonts/30-03.pcx
  fonts/30-04.bin
  fonts/30-04.pcx
  fonts/42-00.bin
  fonts/42-00.pcx
  fonts/42-01.bin
  fonts/42-01.pcx
  fonts/42-02.bin
  fonts/42-02.pcx
  fonts/42-03.bin
  fonts/42-03.pcx
  fonts/42-04.bin
  fonts/42-04.pcx
  fonts/46-00.bin
  fonts/46-00.pcx
  fonts/46-01.bin
  fonts/46-01.pcx
  fonts/46-02.bin
  fonts/46-02.pcx
  fonts/46-03.bin
  fonts/46-03.pcx
  fonts/46-04.bin
  fonts/46-04.pcx
  fonts/black.trn
  fonts/blue.trn
  fonts/buttonface.trn
  fonts/buttonpushed.trn
  fonts/golduis.trn
  fonts/goldui.trn
  fonts/grayuis.trn
  fonts/grayui.trn
  fonts/red.trn
  fonts/whitegold.trn
  fonts/white.trn
  fonts/yellowdialog.trn
  gendata/cutportlw.pcx
  gendata/cutportrw.pcx
  gendata/cutstartw.pcx
  ui_art/creditsw.pcx
  ui_art/hf_titlew.pcx
  ui_art/mainmenuw.pcx
  ui_art/supportw.pcx)

if(NOT USE_SDL1 AND NOT VITA)
  list(APPEND devilutionx_assets
    ui_art/button.png
    ui_art/directions2.png
    ui_art/directions.png
    ui_art/menu-levelup.png
    ui_art/menu.png)
endif()

if(APPLE)
  foreach(asset_file ${devilutionx_assets})
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/Packaging/resources/assets/${asset_file}")
    get_filename_component(_asset_dir "${asset_file}" DIRECTORY)
    set_source_files_properties("${src}" PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources/${_asset_dir}"
      XCODE_EXPLICIT_FILE_TYPE compiled)
    target_sources(${BIN_TARGET} PRIVATE "${src}")
  endforeach()
else()
  # Copy assets to the build assets subdirectory. This serves two purposes:
  # - If smpq is installed, devilutionx.mpq is built from these files.
  # - If smpq is not installed, the game will load the assets directly from this directoy.
  foreach(asset_file ${devilutionx_assets})
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/Packaging/resources/assets/${asset_file}")
    set(dst "${DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY}/${asset_file}")
    list(APPEND DEVILUTIONX_MPQ_FILES "${asset_file}")
    list(APPEND DEVILUTIONX_OUTPUT_ASSETS_FILES "${dst}")
    add_custom_command(
      COMMENT "Copying ${asset_file}"
      OUTPUT "${dst}"
      DEPENDS "${src}"
      COMMAND ${CMAKE_COMMAND} -E copy "${src}" "${dst}"
      VERBATIM)
  endforeach()
  if (Gettext_FOUND)
    foreach(lang ${devilutionx_langs})
      list(APPEND DEVILUTIONX_MPQ_FILES "${lang}.gmo")
    endforeach()
  endif()

  if(BUILD_ASSETS_MPQ)
    set(DEVILUTIONX_MPQ "${CMAKE_CURRENT_BINARY_DIR}/devilutionx.mpq")
    add_custom_command(
      COMMENT "Building devilutionx.mpq"
      OUTPUT "${DEVILUTIONX_MPQ}"
      COMMAND ${CMAKE_COMMAND} -E remove -f "${DEVILUTIONX_MPQ}"
      COMMAND ${SMPQ} -M 1 -C PKWARE -c "${DEVILUTIONX_MPQ}" ${DEVILUTIONX_MPQ_FILES}
      WORKING_DIRECTORY "${DEVILUTIONX_ASSETS_OUTPUT_DIRECTORY}"
      DEPENDS ${DEVILUTIONX_OUTPUT_ASSETS_FILES} ${devilutionx_lang_targets} ${devilutionx_lang_files}
      VERBATIM)
    add_custom_target(devilutionx_mpq DEPENDS "${DEVILUTIONX_MPQ}")
    add_dependencies(libdevilutionx devilutionx_mpq)
  else()
    add_custom_target(devilutionx_copied_assets DEPENDS ${DEVILUTIONX_OUTPUT_ASSETS_FILES} ${devilutionx_lang_targets})
    add_dependencies(libdevilutionx devilutionx_copied_assets)
  endif()
endif()
