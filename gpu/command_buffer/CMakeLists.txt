include(NaclBuildNexe)

set(libgles2_utils_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/common/gles2_cmd_format.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/gles2_cmd_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/gles2_cmd_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/gles2_utils_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/logging.h
)

set(libgles2_utils_untrusted_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
    ${CHROME_DEPS_DIR}/third_party/khronos
    ${CHROME_DEPS_DIR}/gpu
)

nacl_build_nlib(libgles2_utils_untrusted_64.a "" 64 "" "${libgles2_utils_untrusted_include_dirs}" "" "libgles2_utils_untrusted.a" "${libgles2_utils_untrusted_sources}")
nacl_build_nlib(libgles2_utils_untrusted_32.a "" 32 "" "${libgles2_utils_untrusted_include_dirs}" "" "libgles2_utils_untrusted.a" "${libgles2_utils_untrusted_sources}")

add_custom_target(gles2_utils_untrusted ALL
    DEPENDS ${NACL_TC_NEWLIB64_DIR}/libgles2_utils_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libgles2_utils_untrusted.a
)
