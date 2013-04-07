include(NaclBuildNexe)

set(nacl_irt_extra_args "--strip-debug")
set(nacl_irt_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
    ${CHROME_DEPS_DIR}/ppapi/native_client/lib/gl/include
    ${CHROME_DEPS_DIR}/third_party/khronos
    ${CHROME_DEPS_DIR}/gpu
)

set(nacl_irt_libs
    irt_browser
    ppapi_proxy_untrusted
    ppapi_ipc_untrusted
    ppapi_shared_untrusted
    gles2_implementation_untrusted
    gles2_cmd_helper_untrusted
    gles2_utils_untrusted
    command_buffer_client_untrusted
    command_buffer_common_untrusted
    tracing_untrusted
    gpu_ipc_untrusted
    ipc_untrusted
    base_untrusted
    shared_memory_support_untrusted
    srpc
    imc_syscalls
    platform
    gio
)

set(nacl_irt_common_deps
    prep_toolchain
    irt_browser_lib
    ppapi_proxy_untrusted
    ppapi_ipc_untrusted
    ppapi_shared_untrusted
    gles2_implementation_untrusted
    gles2_cmd_helper_untrusted
    gles2_utils_untrusted
    command_buffer_client_untrusted
    command_buffer_common_untrusted
    tracing_untrusted
    gpu_ipc_untrusted
    ipc_untrusted
    base_untrusted
    shared_memory_support_untrusted
    srpc_lib
    imc_syscalls_lib
    platform_lib
    gio_lib
)

set(nacl_irt_link_flags
    -Wl,--start-group
)

set(nacl_irt_32_extra_deps ${nacl_irt_common_deps})
set(nacl_irt_64_extra_deps ${nacl_irt_common_deps})

foreach (i_lib ${nacl_irt_libs})
    list(APPEND nacl_irt_link_flags -l${i_lib})
    list(APPEND nacl_irt_64_extra_deps ${NACL_TC_NEWLIB64_DIR}/lib${i_lib}.a)
    list(APPEND nacl_irt_32_extra_deps ${NACL_TC_NEWLIB32_DIR}/lib${i_lib}.a)
endforeach ()

list(APPEND nacl_irt_link_flags
    -Wl,--end-group
    -lm
)

nacl_build_nexe(nacl_irt_x86_64.nexe "${nacl_irt_extra_args}" 64 "${nacl_irt_include_dirs}" "${nacl_irt_link_flags}" "${nacl_irt_64_extra_deps}" "")
nacl_build_nexe(nacl_irt_x86_32.nexe "${nacl_irt_extra_args}" 32 "${nacl_irt_include_dirs}" "${nacl_irt_link_flags}" "${nacl_irt_32_extra_deps}" "")

add_custom_target(nacl_irt ALL
    DEPENDS  ${CMAKE_BINARY_DIR}/bin/nacl_irt_x86_64.nexe  ${CMAKE_BINARY_DIR}/bin/nacl_irt_x86_32.nexe
)
