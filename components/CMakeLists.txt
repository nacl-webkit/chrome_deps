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

nacl_build_nlib(libtracing_untrusted_64.a "" 64 "" "${libtracing_untrusted_include_dirs}" "" "libtracing_untrusted.a" "${libtracing_untrusted_sources}")
nacl_build_nlib(libtracing_untrusted_32.a "" 32 "" "${libtracing_untrusted_include_dirs}" "" "libtracing_untrusted.a" "${libtracing_untrusted_sources}")

add_custom_target(tracing_untrusted ALL
    DEPENDS ${NACL_TC_NEWLIB64_DIR}/libtracing_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libtracing_untrusted.a
)
