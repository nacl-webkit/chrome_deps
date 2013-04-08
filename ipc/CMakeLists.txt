set(CHROME_IPC_LIBRARY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/file_descriptor_set_posix.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/file_descriptor_set_posix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_posix.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_posix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_descriptors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_listener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_start.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_param_traits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sender.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_log_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_read_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_write_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_constructor_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_destructor_macros.h
)

set(CHROME_IPC_LIBRARY_LIBRARIES
    ${CHROME_BASE_LIBRARY_LIBRARIES}
)

include_directories(${CHROME_DEPS_DIR})
add_library(${CHROME_IPC_LIBRARY_NAME} SHARED ${CHROME_IPC_LIBRARY_SOURCES})
target_link_libraries(${CHROME_IPC_LIBRARY_NAME} ${CHROME_IPC_LIBRARY_LIBRARIES})

# untrusted part
include(NaclBuildNexe)

set(libipc_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/file_descriptor_set_posix.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/file_descriptor_set_posix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_nacl.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_nacl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_posix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_descriptors.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_listener.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_start.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_param_traits.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sender.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_log_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_read_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/param_traits_write_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_constructor_macros.h
    ${CMAKE_CURRENT_SOURCE_DIR}/struct_destructor_macros.h
)

set(libipc_untrusted_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
)

nacl_build_nlib(libipc_untrusted_64.a "" 64 "" "${libipc_untrusted_include_dirs}" "" "libipc_untrusted.a" "${libipc_untrusted_sources}")
nacl_build_nlib(libipc_untrusted_32.a "" 32 "" "${libipc_untrusted_include_dirs}" "" "libipc_untrusted.a" "${libipc_untrusted_sources}")

add_custom_target(ipc_untrusted ALL
    DEPENDS ${NACL_TC_NEWLIB64_DIR}/libipc_untrusted.a ${NACL_TC_NEWLIB32_DIR}/libipc_untrusted.a
)
