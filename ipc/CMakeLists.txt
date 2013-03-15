set(CHROME_IPC_LIBRARY_SOURCES
    ${CHROME_DEPS_DIR}/ipc/file_descriptor_set_posix.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_channel.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_channel_posix.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_channel_proxy.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_channel_reader.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_forwarding_message_filter.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_logging.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_message.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_message_utils.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_platform_file.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_switches.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_sync_channel.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_sync_message.cc
    ${CHROME_DEPS_DIR}/ipc/ipc_sync_message_filter.cc
)

set(CHROME_IPC_LIBRARY_LIBRARIES
    ${CHROME_BASE_LIBRARY_LIBRARIES}
)

include_directories(${CHROME_DEPS_DIR})
add_library(${CHROME_IPC_LIBRARY_NAME} SHARED ${CHROME_IPC_LIBRARY_SOURCES})
target_link_libraries(${CHROME_IPC_LIBRARY_NAME} ${CHROME_IPC_LIBRARY_LIBRARIES})
