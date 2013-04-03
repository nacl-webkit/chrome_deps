include(NaclBuildNexe)

set(untrusted_common_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
    ${CHROME_DEPS_DIR}/third_party/khronos
    ${CHROME_DEPS_DIR}/gpu
)

# command_buffer_common_untrusted
set(libcommand_buffer_common_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/common/cmd_buffer_common.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/common/debug_marker_manager.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/common/gles2_cmd_format.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/common/id_allocator.cc
)

nacl_build_nlib(libcommand_buffer_common_untrusted_64.a "" 64 "" "${untrusted_common_include_dirs}" "" "libcommand_buffer_common_untrusted.a" "${libcommand_buffer_common_untrusted_sources}")
nacl_build_nlib(libcommand_buffer_common_untrusted_32.a "" 32 "" "${untrusted_common_include_dirs}" "" "libcommand_buffer_common_untrusted.a" "${libcommand_buffer_common_untrusted_sources}")

add_custom_target(command_buffer_common_untrusted ALL
    DEPENDS gles2_utils_untrusted ${NACL_TC_NEWLIB64_DIR}/libcommand_buffer_common_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libcommand_buffer_common_untrusted.a
)

# command_buffer_client_untrusted
set(libcommand_buffer_client_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/atomicops.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/cmd_buffer_helper.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/fenced_allocator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/mapped_memory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/ring_buffer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/transfer_buffer.cc
)

nacl_build_nlib(libcommand_buffer_client_untrusted_64.a "" 64 "" "${untrusted_common_include_dirs}" "" "libcommand_buffer_client_untrusted.a" "${libcommand_buffer_client_untrusted_sources}")
nacl_build_nlib(libcommand_buffer_client_untrusted_32.a "" 32 "" "${untrusted_common_include_dirs}" "" "libcommand_buffer_client_untrusted.a" "${libcommand_buffer_client_untrusted_sources}")

add_custom_target(command_buffer_client_untrusted ALL
    DEPENDS command_buffer_common_untrusted ${NACL_TC_NEWLIB64_DIR}/libcommand_buffer_client_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libcommand_buffer_client_untrusted.a
)

# gpu_ipc_untrusted
set(libgpu_ipc_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc/command_buffer_proxy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc/gpu_command_buffer_traits.cc
)

set(libgpu_ipc_untrusted_include_dirs
    ${untrusted_common_include_dirs}
    ${WEBKIT2_DIR}/Shared/Plugins/ppapi/gfx
)

nacl_build_nlib(libgpu_ipc_untrusted_64.a "" 64 "" "${libgpu_ipc_untrusted_include_dirs}" "" "libgpu_ipc_untrusted.a" "${libgpu_ipc_untrusted_sources}")
nacl_build_nlib(libgpu_ipc_untrusted_32.a "" 32 "" "${libgpu_ipc_untrusted_include_dirs}" "" "libgpu_ipc_untrusted.a" "${libgpu_ipc_untrusted_sources}")

add_custom_target(gpu_ipc_untrusted ALL
    DEPENDS command_buffer_common_untrusted ${NACL_TC_NEWLIB64_DIR}/libgpu_ipc_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libgpu_ipc_untrusted.a
)

# gles2_cmd_helper_untrusted
set(libgles2_cmd_helper_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/gles2_cmd_helper.cc
)

nacl_build_nlib(libgles2_cmd_helper_untrusted_64.a "" 64 "" "${untrusted_common_include_dirs}" "" "libgles2_cmd_helper_untrusted.a" "${libgles2_cmd_helper_untrusted_sources}")
nacl_build_nlib(libgles2_cmd_helper_untrusted_32.a "" 32 "" "${untrusted_common_include_dirs}" "" "libgles2_cmd_helper_untrusted.a" "${libgles2_cmd_helper_untrusted_sources}")

add_custom_target(gles2_cmd_helper_untrusted ALL
    DEPENDS command_buffer_client_untrusted ${NACL_TC_NEWLIB64_DIR}/libgles2_cmd_helper_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libgles2_cmd_helper_untrusted.a
)

# gles2_implementation_untrusted
set(libgles2_implementation_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/buffer_tracker.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/client_context_state.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/gles2_implementation.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/gles2_interface.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/program_info_manager.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/query_tracker.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/share_group.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/command_buffer/client/vertex_array_object_manager.cc
)

nacl_build_nlib(libgles2_implementation_untrusted_64.a "" 64 "GLES2_IMPL_IMPLEMENTATION" "${untrusted_common_include_dirs}" "" "libgles2_implementation_untrusted.a" "${libgles2_implementation_untrusted_sources}")
nacl_build_nlib(libgles2_implementation_untrusted_32.a "" 32 "GLES2_IMPL_IMPLEMENTATION" "${untrusted_common_include_dirs}" "" "libgles2_implementation_untrusted.a" "${libgles2_implementation_untrusted_sources}")

add_custom_target(gles2_implementation_untrusted ALL
    DEPENDS gles2_cmd_helper_untrusted ${NACL_TC_NEWLIB64_DIR}/libgles2_implementation_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libgles2_implementation_untrusted.a
)
