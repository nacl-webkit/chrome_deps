include(NaclBuildNexe)

set(libtracing_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tracing/child_trace_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tracing/tracing_messages.cc
)

set(libtracing_untrusted_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
    ${NATIVE_CLIENT_DIR}/src/third_party
)

nacl_build_nlib(libtracing_untrusted_64.a "" 64 "" "${libtracing_untrusted_include_dirs}" "" "build_nacl_lib_newlib_64.a" "libtracing_untrusted.a" "${libtracing_untrusted_sources}")
nacl_build_nlib(libtracing_untrusted_32.a "" 32 "" "${libtracing_untrusted_include_dirs}" "" "build_nacl_lib_newlib_32.a" "libtracing_untrusted.a" "${libtracing_untrusted_sources}")

add_custom_target(tracing_untrusted ALL
    DEPENDS build_libtracing_untrusted_64.a build_libtracing_untrusted_32.a
)
