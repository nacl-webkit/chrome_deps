set(CHROME_IPC_LIBRARY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/file_descriptor_set_posix.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_posix.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.cc
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
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_nacl.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_proxy.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_channel_reader.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_forwarding_message_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_logging.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_message_utils.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_platform_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_switches.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_channel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_sync_message_filter.cc
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
