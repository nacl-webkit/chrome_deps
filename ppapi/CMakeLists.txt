SET(ppapi_INCLUDE_DIRECTORIES
    "${ppapi_DIR}"
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}/Source"
    "${CMAKE_SOURCE_DIR}/Source/chrome_deps"
)

SET(ppapi_SOURCES
    cpp/array_output.cc
    cpp/audio.cc
    cpp/audio_config.cc
    cpp/core.cc
    cpp/file_io.cc
    cpp/file_ref.cc
    cpp/file_system.cc
    cpp/fullscreen.cc
    cpp/graphics_2d.cc
    cpp/graphics_3d.cc
    cpp/graphics_3d_client.cc
    cpp/image_data.cc
    cpp/input_event.cc
    cpp/instance.cc
    cpp/instance_handle.cc
    cpp/message_loop.cc
    cpp/module.cc
    cpp/mouse_cursor.cc
    cpp/mouse_lock.cc
    cpp/ppp_entrypoints.cc
    cpp/rect.cc
    cpp/resource.cc
    cpp/url_loader.cc
    cpp/url_request_info.cc
    cpp/url_response_info.cc
    cpp/var.cc
    cpp/var_array_buffer.cc
    cpp/view.cc
    cpp/websocket.cc
    
    cpp/dev/audio_input_dev.cc
    cpp/dev/buffer_dev.cc
    cpp/dev/crypto_dev.cc
    cpp/dev/cursor_control_dev.cc
    cpp/dev/device_ref_dev.cc
    cpp/dev/directory_entry_dev.cc
    cpp/dev/directory_reader_dev.cc
    cpp/dev/file_chooser_dev.cc
    cpp/dev/find_dev.cc
    cpp/dev/font_dev.cc
    cpp/dev/graphics_2d_dev.cc
    cpp/dev/ime_input_event_dev.cc
    cpp/dev/memory_dev.cc
    cpp/dev/printing_dev.cc
    cpp/dev/resource_array_dev.cc
    cpp/dev/scriptable_object_deprecated.cc
    cpp/dev/scrollbar_dev.cc
    cpp/dev/selection_dev.cc
    cpp/dev/text_input_dev.cc
    cpp/dev/url_util_dev.cc
    cpp/dev/video_capture_client_dev.cc
    cpp/dev/video_capture_dev.cc
    cpp/dev/video_decoder_client_dev.cc
    cpp/dev/video_decoder_dev.cc
    cpp/dev/view_dev.cc
    cpp/dev/widget_client_dev.cc
    cpp/dev/widget_dev.cc
    cpp/dev/zoom_dev.cc

    cpp/private/content_decryptor_private.cc
    cpp/private/flash.cc
    cpp/private/flash_clipboard.cc
    cpp/private/flash_device_id.cc
    cpp/private/flash_file.cc
    cpp/private/flash_font_file.cc
    cpp/private/flash_fullscreen.cc
    cpp/private/flash_menu.cc
    cpp/private/flash_message_loop.cc
    cpp/private/host_resolver_private.cc
    cpp/private/instance_private.cc
    cpp/private/net_address_private.cc
    cpp/private/network_list_private.cc
    cpp/private/network_monitor_private.cc
    cpp/private/tcp_server_socket_private.cc
    cpp/private/tcp_socket_private.cc
    cpp/private/udp_socket_private.cc
    cpp/private/var_private.cc
    cpp/private/x509_certificate_private.cc

    cpp/trusted/browser_font_trusted.cc
    cpp/trusted/file_chooser_trusted.cc
    cpp/trusted/file_io_trusted.cc
    
    utility/threading/lock.cc
    utility/private/network_list_observer_private.cc
    utility/graphics/paint_aggregator.cc
    utility/graphics/paint_manager.cc
    utility/threading/simple_thread.cc
    utility/websocket/websocket_api.cc
)

SET(ppapi_LIBRARIES
    ${ppapi_LIBRARY_NAME}
)

WEBKIT_INCLUDE_CONFIG_FILES_IF_EXISTS()

ADD_DEFINITIONS(-DNACL_LINUX=1)
SET(NACL_LINUX 1)
ADD_DEFINITIONS(-DNACL_OSX=0)
SET(NACL_OSX 0)
ADD_DEFINITIONS(-DNACL_WINDOWS=0)
SET(NACL_WINDOWS 0)

ADD_DEFINITIONS(-DBUILDING_WEBKIT)
INCLUDE_DIRECTORIES(${ppapi_INCLUDE_DIRECTORIES})
ADD_LIBRARY(${ppapi_LIBRARY_NAME} ${ppapi_LIBRARY_TYPE} ${ppapi_SOURCES})

TARGET_LINK_LIBRARIES(${ppapi_LIBRARY_NAME} ${ppapi_LIBRARIES} pthread)
SET_TARGET_PROPERTIES(${ppapi_LIBRARY_NAME} PROPERTIES FOLDER "WebKit")
SET_TARGET_PROPERTIES(${ppapi_LIBRARY_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

SET_TARGET_PROPERTIES(${ppapi_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

INSTALL(TARGETS ${ppapi_LIBRARY_NAME} DESTINATION "${LIB_INSTALL_DIR}")

include(ppapi_proxy_untrusted.txt)
include(ppapi_ipc_untrusted.txt)
