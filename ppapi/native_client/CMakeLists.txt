include(NaclBuildNexe)

set(nacl_irt_extra_args "--strip-debug")
set(nacl_irt_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
    ${CHROME_DEPS_DIR}/ppapi/native_client/lib/gl/include
    ${CHROME_DEPS_DIR}/third_party/khronos
    ${CHROME_DEPS_DIR}/gpu
)

set(nacl_irt_link_flags
    -Wl,--start-group
    -lirt_browser
    -lppapi_proxy_untrusted
    -lppapi_ipc_untrusted
    -lppapi_shared_untrusted
#    -lgles2_implementation_untrusted
#    -lgles2_cmd_helper_untrusted
#    -lgles2_utils_untrusted
#    -lcommand_buffer_client_untrusted
#    -lcommand_buffer_common_untrusted
    -ltracing_untrusted
#    -lgpu_ipc_untrusted
    -lipc_untrusted
    -lbase_untrusted
    -lshared_memory_support_untrusted
    -lsrpc
    -limc_syscalls
    -lplatform
    -lgio
    -Wl,--end-group
    -lm
)

set(nacl_irt_extra_deps
    prep_toolchain
    irt_browser_lib
    ppapi_proxy_untrusted
    ppapi_ipc_untrusted
    ppapi_shared_untrusted
#   gles2_implementation_untrusted
#   gles2_cmd_helper_untrusted
#   gles2_utils_untrusted
#   command_buffer_client_untrusted
#   command_buffer_common_untrusted
    tracing_untrusted
#   gpu_ipc_untrusted
    ipc_untrusted
    base_untrusted
    shared_memory_support_untrusted
    srpc_lib
    imc_syscalls_lib
    platform_lib
    gio_lib
)

nacl_build_nexe(nacl_irt_x86_64.nexe "${nacl_irt_extra_args}" 64 "${nacl_irt_include_dirs}" "${nacl_irt_link_flags}" "${nacl_irt_extra_deps}" "")
nacl_build_nexe(nacl_irt_x86_32.nexe "${nacl_irt_extra_args}" 32 "${nacl_irt_include_dirs}" "${nacl_irt_link_flags}" "${nacl_irt_extra_deps}" "")

add_custom_target(nacl_irt ALL
    DEPENDS build_nacl_irt_x86_64.nexe build_nacl_irt_x86_32.nexe
)
