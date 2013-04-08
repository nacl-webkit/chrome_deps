include(NaclBuildNexe)

set(libshared_memory_support_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/audio_parameters.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/audio_parameters.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/shared_memory_util.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/shared_memory_util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base/audio_bus.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/base/audio_bus.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base/channel_layout.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/base/channel_layout.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base/limits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base/media_export.h
)

set(libshared_memory_support_untrusted_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
)

set(libshared_memory_support_untrusted_defines
    MEDIA_IMPLEMENTATION
)

nacl_build_nlib(libshared_memory_support_untrusted_64.a "" 64 "${libshared_memory_support_untrusted_defines}" "${libshared_memory_support_untrusted_include_dirs}" "" "libshared_memory_support_untrusted.a" "${libshared_memory_support_untrusted_sources}")
nacl_build_nlib(libshared_memory_support_untrusted_32.a "" 32 "${libshared_memory_support_untrusted_defines}" "${libshared_memory_support_untrusted_include_dirs}" "" "libshared_memory_support_untrusted.a" "${libshared_memory_support_untrusted_sources}")

add_custom_target(shared_memory_support_untrusted ALL
    DEPENDS ${NACL_TC_NEWLIB64_DIR}/libshared_memory_support_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libshared_memory_support_untrusted.a
)
