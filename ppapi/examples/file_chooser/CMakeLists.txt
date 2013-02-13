SET(ppapi_DIR ./../..)
SET(paint_manager_LIBRARY_NAME ppapi_example_file_chooser)
SET(paint_manager_LIBRARY_TYPE SHARED)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(paint_manager_INCLUDE_DIRECTORIES
    "${ppapi_DIR}"
    "${ppapi_DIR}/.."
    "${ppapi_DIR}/cpp"
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}"
)

SET(paint_manager_SOURCES
    ${ppapi_DIR}/cpp/array_output.cc
    ${ppapi_DIR}/cpp/audio.cc
    ${ppapi_DIR}/cpp/audio_config.cc
    ${ppapi_DIR}/cpp/core.cc
    ${ppapi_DIR}/cpp/file_io.cc
    ${ppapi_DIR}/cpp/file_ref.cc
    ${ppapi_DIR}/cpp/file_system.cc
    ${ppapi_DIR}/cpp/fullscreen.cc
    ${ppapi_DIR}/cpp/graphics_2d.cc
    ${ppapi_DIR}/cpp/graphics_3d.cc
    ${ppapi_DIR}/cpp/graphics_3d_client.cc
    ${ppapi_DIR}/cpp/image_data.cc
    ${ppapi_DIR}/cpp/input_event.cc
    ${ppapi_DIR}/cpp/instance.cc
    ${ppapi_DIR}/cpp/instance_handle.cc
    ${ppapi_DIR}/cpp/message_loop.cc
    ${ppapi_DIR}/cpp/module.cc
    ${ppapi_DIR}/cpp/mouse_cursor.cc
    ${ppapi_DIR}/cpp/mouse_lock.cc
    ${ppapi_DIR}/cpp/ppp_entrypoints.cc
    ${ppapi_DIR}/cpp/rect.cc
    ${ppapi_DIR}/cpp/resource.cc
    ${ppapi_DIR}/cpp/url_loader.cc
    ${ppapi_DIR}/cpp/url_request_info.cc
    ${ppapi_DIR}/cpp/url_response_info.cc
    ${ppapi_DIR}/cpp/var.cc
    ${ppapi_DIR}/cpp/var_array_buffer.cc
    ${ppapi_DIR}/cpp/view.cc
    ${ppapi_DIR}/cpp/websocket.cc
    
    ${ppapi_DIR}/cpp/dev/audio_input_dev.cc
    ${ppapi_DIR}/cpp/dev/buffer_dev.cc
    ${ppapi_DIR}/cpp/dev/crypto_dev.cc
    ${ppapi_DIR}/cpp/dev/cursor_control_dev.cc
    ${ppapi_DIR}/cpp/dev/device_ref_dev.cc
    ${ppapi_DIR}/cpp/dev/directory_entry_dev.cc
    ${ppapi_DIR}/cpp/dev/directory_reader_dev.cc
    ${ppapi_DIR}/cpp/dev/file_chooser_dev.cc
    ${ppapi_DIR}/cpp/dev/find_dev.cc
    ${ppapi_DIR}/cpp/dev/font_dev.cc
    ${ppapi_DIR}/cpp/dev/graphics_2d_dev.cc
    ${ppapi_DIR}/cpp/dev/ime_input_event_dev.cc
    ${ppapi_DIR}/cpp/dev/memory_dev.cc
    ${ppapi_DIR}/cpp/dev/printing_dev.cc
    ${ppapi_DIR}/cpp/dev/resource_array_dev.cc
    ${ppapi_DIR}/cpp/dev/scriptable_object_deprecated.cc
    ${ppapi_DIR}/cpp/dev/scrollbar_dev.cc
    ${ppapi_DIR}/cpp/dev/selection_dev.cc
    ${ppapi_DIR}/cpp/dev/text_input_dev.cc
    ${ppapi_DIR}/cpp/dev/url_util_dev.cc
    ${ppapi_DIR}/cpp/dev/video_capture_client_dev.cc
    ${ppapi_DIR}/cpp/dev/video_capture_dev.cc
    ${ppapi_DIR}/cpp/dev/video_decoder_client_dev.cc
    ${ppapi_DIR}/cpp/dev/video_decoder_dev.cc
    ${ppapi_DIR}/cpp/dev/view_dev.cc
    ${ppapi_DIR}/cpp/dev/widget_client_dev.cc
    ${ppapi_DIR}/cpp/dev/widget_dev.cc
    ${ppapi_DIR}/cpp/dev/zoom_dev.cc

    ${ppapi_DIR}/cpp/private/content_decryptor_private.cc
    ${ppapi_DIR}/cpp/private/flash.cc
    ${ppapi_DIR}/cpp/private/flash_clipboard.cc
    ${ppapi_DIR}/cpp/private/flash_device_id.cc
    ${ppapi_DIR}/cpp/private/flash_file.cc
    ${ppapi_DIR}/cpp/private/flash_font_file.cc
    ${ppapi_DIR}/cpp/private/flash_fullscreen.cc
    ${ppapi_DIR}/cpp/private/flash_menu.cc
    ${ppapi_DIR}/cpp/private/flash_message_loop.cc
    ${ppapi_DIR}/cpp/private/host_resolver_private.cc
    ${ppapi_DIR}/cpp/private/instance_private.cc
    ${ppapi_DIR}/cpp/private/net_address_private.cc
    ${ppapi_DIR}/cpp/private/network_list_private.cc
    ${ppapi_DIR}/cpp/private/network_monitor_private.cc
    ${ppapi_DIR}/cpp/private/tcp_server_socket_private.cc
    ${ppapi_DIR}/cpp/private/tcp_socket_private.cc
    ${ppapi_DIR}/cpp/private/udp_socket_private.cc
    ${ppapi_DIR}/cpp/private/var_private.cc
    ${ppapi_DIR}/cpp/private/x509_certificate_private.cc

    ${ppapi_DIR}/cpp/trusted/browser_font_trusted.cc
    ${ppapi_DIR}/cpp/trusted/file_chooser_trusted.cc
    ${ppapi_DIR}/cpp/trusted/file_io_trusted.cc
    
    ${ppapi_DIR}/utility/threading/lock.cc
    ${ppapi_DIR}/utility/private/network_list_observer_private.cc
    ${ppapi_DIR}/utility/graphics/paint_aggregator.cc
    ${ppapi_DIR}/utility/graphics/paint_manager.cc
    ${ppapi_DIR}/utility/threading/simple_thread.cc
    ${ppapi_DIR}/utility/websocket/websocket_api.cc

    file_chooser.cc
)

SET(paint_manager_LIBRARIES
    ${paint_manager_LIBRARY_NAME}
)

ADD_DEFINITIONS(-DNACL_LINUX=1)
SET(NACL_LINUX 1)
ADD_DEFINITIONS(-DNACL_OSX=0)
SET(NACL_OSX 0)
ADD_DEFINITIONS(-DNACL_WINDOWS=0)
SET(NACL_WINDOWS 0)

INCLUDE_DIRECTORIES(${paint_manager_INCLUDE_DIRECTORIES})
ADD_LIBRARY(${paint_manager_LIBRARY_NAME} ${paint_manager_LIBRARY_TYPE} ${paint_manager_SOURCES})

TARGET_LINK_LIBRARIES(${paint_manager_LIBRARY_NAME} ${paint_manager_LIBRARIES})
SET_TARGET_PROPERTIES(${paint_manager_LIBRARY_NAME} PROPERTIES FOLDER "WebKit")
SET_TARGET_PROPERTIES(${paint_manager_LIBRARY_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

SET_TARGET_PROPERTIES(${paint_manager_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

