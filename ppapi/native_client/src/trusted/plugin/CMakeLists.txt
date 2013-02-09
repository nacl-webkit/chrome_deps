SET(ppapi_DIR "${CMAKE_SOURCE_DIR}/Source/chrome_deps/ppapi")
SET(nacl_plugin_INCLUDE_DIRECTORIES
    "${ppapi_DIR}"
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}/Source"
    "${CMAKE_SOURCE_DIR}/Source/chrome_deps"
)

SET(nacl_plugin_SOURCES
    ${ppapi_DIR}/native_client/src/trusted/plugin/arch_x86/sandbox_isa.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/file_downloader.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/json_manifest.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/local_temp_file.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/module_ppapi.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/nacl_subprocess.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/plugin.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/scriptable_plugin.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/sel_ldr_launcher_chrome.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/service_runtime.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/srpc_client.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/srpc_params.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/temporary_file.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/utility.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/pnacl_coordinator.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/pnacl_resources.cc
    ${ppapi_DIR}/native_client/src/trusted/plugin/pnacl_translate_thread.cc
)

SET(nacl_plugin_LIBRARIES
    ${nacl_plugin_LIBRARY_NAME}
    ${nacl_LIBRARY_NAME}
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
INCLUDE_DIRECTORIES(
    ${nacl_plugin_INCLUDE_DIRECTORIES} 
    ${jsoncpp_INCLUDE_DIRECTORIES} 
    ${nacl_INCLUDE_DIRECTORIES}
)
ADD_LIBRARY(
    ${nacl_plugin_LIBRARY_NAME} 
    ${nacl_plugin_LIBRARY_TYPE} 
    ${nacl_plugin_SOURCES}
)

TARGET_LINK_LIBRARIES(${nacl_plugin_LIBRARY_NAME} 
    ${nacl_plugin_LIBRARIES} 
    rt 
    ${jsoncpp_LIBRARY_NAME} 
    ${nacl_LIBRARY_NAME}
)
SET_TARGET_PROPERTIES(${nacl_plugin_LIBRARY_NAME} PROPERTIES FOLDER "WebKit")
SET_TARGET_PROPERTIES(${nacl_plugin_LIBRARY_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

SET_TARGET_PROPERTIES(${nacl_plugin_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

INSTALL(TARGETS ${nacl_plugin_LIBRARY_NAME} DESTINATION "${LIB_INSTALL_DIR}")

