set(BASE_STATIC_LIBRARY_NAME chrome_base_static)
set(ALLOCATOR_LIBRARY_NAME allocator)
set(XDG_MIME_LIBRARY_NAME xdg_mime)
set(MODP_B64_LIBRARY_NAME modp_b64)
set(DYNAMIC_ANNOTATIONS_LIBRARY_NAME dynamic_annotations)

set(CHROME_BASE_LIBRARY_SOURCES
    ${CHROME_DEPS_DIR}/base/third_party/xdg_user_dirs/xdg_user_dir_lookup.cc
    ${CHROME_DEPS_DIR}/base/event_recorder_stubs.cc
    ${CHROME_DEPS_DIR}/base/linux_util.cc
    ${CHROME_DEPS_DIR}/base/md5.cc
    ${CHROME_DEPS_DIR}/base/message_pump_glib.cc
    #${CHROME_DEPS_DIR}/base/message_pump_gtk.cc
    ${CHROME_DEPS_DIR}/base/message_pump_webkit.cc
    ${CHROME_DEPS_DIR}/base/message_pump_libevent.cc
    ${CHROME_DEPS_DIR}/base/metrics/field_trial.cc
    ${CHROME_DEPS_DIR}/base/posix/file_descriptor_shuffle.cc
    ${CHROME_DEPS_DIR}/base/sync_socket_posix.cc
    ${CHROME_DEPS_DIR}/base/third_party/dmg_fp/g_fmt.cc
    ${CHROME_DEPS_DIR}/base/third_party/dmg_fp/dtoa_wrapper.cc
    ${CHROME_DEPS_DIR}/base/third_party/icu/icu_utf.cc
    ${CHROME_DEPS_DIR}/base/third_party/nspr/prtime.cc
    ${CHROME_DEPS_DIR}/base/allocator/allocator_extension.cc
    ${CHROME_DEPS_DIR}/base/allocator/type_profiler_control.cc
    ${CHROME_DEPS_DIR}/base/at_exit.cc
    ${CHROME_DEPS_DIR}/base/atomicops_internals_x86_gcc.cc
    ${CHROME_DEPS_DIR}/base/base_paths.cc
    ${CHROME_DEPS_DIR}/base/base_paths_posix.cc
    ${CHROME_DEPS_DIR}/base/base64.cc
    ${CHROME_DEPS_DIR}/base/bind_helpers.cc
    ${CHROME_DEPS_DIR}/base/build_time.cc
    ${CHROME_DEPS_DIR}/base/callback_internal.cc
    ${CHROME_DEPS_DIR}/base/command_line.cc
    ${CHROME_DEPS_DIR}/base/cpu.cc
    ${CHROME_DEPS_DIR}/base/debug/alias.cc
    ${CHROME_DEPS_DIR}/base/debug/crash_logging.cc
    ${CHROME_DEPS_DIR}/base/debug/debugger.cc
    ${CHROME_DEPS_DIR}/base/debug/debugger_posix.cc
    ${CHROME_DEPS_DIR}/base/debug/profiler.cc
    ${CHROME_DEPS_DIR}/base/debug/stack_trace.cc
    ${CHROME_DEPS_DIR}/base/debug/stack_trace_posix.cc
    ${CHROME_DEPS_DIR}/base/debug/trace_event_impl.cc
    ${CHROME_DEPS_DIR}/base/environment.cc
    ${CHROME_DEPS_DIR}/base/file_path.cc
    ${CHROME_DEPS_DIR}/base/file_util.cc
    ${CHROME_DEPS_DIR}/base/file_util_linux.cc
    ${CHROME_DEPS_DIR}/base/file_util_posix.cc
    ${CHROME_DEPS_DIR}/base/files/file_path_watcher.cc
    ${CHROME_DEPS_DIR}/base/files/file_path_watcher_linux.cc
    ${CHROME_DEPS_DIR}/base/files/file_util_proxy.cc
    ${CHROME_DEPS_DIR}/base/files/important_file_writer.cc
    ${CHROME_DEPS_DIR}/base/files/scoped_temp_dir.cc
    ${CHROME_DEPS_DIR}/base/guid.cc
    ${CHROME_DEPS_DIR}/base/guid_posix.cc
    ${CHROME_DEPS_DIR}/base/hash.cc
    ${CHROME_DEPS_DIR}/base/hi_res_timer_manager_posix.cc
    ${CHROME_DEPS_DIR}/base/json/json_file_value_serializer.cc
    ${CHROME_DEPS_DIR}/base/json/json_parser.cc
    ${CHROME_DEPS_DIR}/base/json/json_reader.cc
    ${CHROME_DEPS_DIR}/base/json/json_string_value_serializer.cc
    ${CHROME_DEPS_DIR}/base/json/json_writer.cc
    ${CHROME_DEPS_DIR}/base/json/string_escape.cc
    ${CHROME_DEPS_DIR}/base/lazy_instance.cc
    ${CHROME_DEPS_DIR}/base/location.cc
    ${CHROME_DEPS_DIR}/base/logging.cc
    ${CHROME_DEPS_DIR}/base/memory/aligned_memory.cc
    ${CHROME_DEPS_DIR}/base/memory/discardable_memory.cc
    ${CHROME_DEPS_DIR}/base/memory/ref_counted.cc
    ${CHROME_DEPS_DIR}/base/memory/ref_counted_memory.cc
    ${CHROME_DEPS_DIR}/base/memory/singleton.cc
    ${CHROME_DEPS_DIR}/base/memory/weak_ptr.cc
    ${CHROME_DEPS_DIR}/base/message_loop.cc
    ${CHROME_DEPS_DIR}/base/message_loop_proxy.cc
    ${CHROME_DEPS_DIR}/base/message_loop_proxy_impl.cc
    ${CHROME_DEPS_DIR}/base/message_pump.cc
    ${CHROME_DEPS_DIR}/base/message_pump_default.cc
    ${CHROME_DEPS_DIR}/base/metrics/sample_map.cc
    ${CHROME_DEPS_DIR}/base/metrics/sample_vector.cc
    ${CHROME_DEPS_DIR}/base/metrics/bucket_ranges.cc
    ${CHROME_DEPS_DIR}/base/metrics/histogram.cc
    ${CHROME_DEPS_DIR}/base/metrics/histogram_base.cc
    ${CHROME_DEPS_DIR}/base/metrics/histogram_samples.cc
    ${CHROME_DEPS_DIR}/base/metrics/histogram_snapshot_manager.cc
    ${CHROME_DEPS_DIR}/base/metrics/sparse_histogram.cc
    ${CHROME_DEPS_DIR}/base/metrics/statistics_recorder.cc
    ${CHROME_DEPS_DIR}/base/metrics/stats_counters.cc
    ${CHROME_DEPS_DIR}/base/metrics/stats_table.cc
    ${CHROME_DEPS_DIR}/base/native_library_posix.cc
    ${CHROME_DEPS_DIR}/base/path_service.cc
    ${CHROME_DEPS_DIR}/base/pending_task.cc
    ${CHROME_DEPS_DIR}/base/pickle.cc
    ${CHROME_DEPS_DIR}/base/platform_file.cc
    ${CHROME_DEPS_DIR}/base/platform_file_posix.cc
    ${CHROME_DEPS_DIR}/base/posix/global_descriptors.cc
    ${CHROME_DEPS_DIR}/base/posix/unix_domain_socket_linux.cc
    ${CHROME_DEPS_DIR}/base/process_linux.cc
    ${CHROME_DEPS_DIR}/base/process_posix.cc
    ${CHROME_DEPS_DIR}/base/process_util.cc
    ${CHROME_DEPS_DIR}/base/process_util_linux.cc
    ${CHROME_DEPS_DIR}/base/process_util_posix.cc
    ${CHROME_DEPS_DIR}/base/profiler/scoped_profile.cc
    ${CHROME_DEPS_DIR}/base/profiler/alternate_timer.cc
    ${CHROME_DEPS_DIR}/base/profiler/tracked_time.cc
    ${CHROME_DEPS_DIR}/base/rand_util.cc
    ${CHROME_DEPS_DIR}/base/rand_util_posix.cc
    ${CHROME_DEPS_DIR}/base/run_loop.cc
    ${CHROME_DEPS_DIR}/base/safe_strerror_posix.cc
    ${CHROME_DEPS_DIR}/base/scoped_native_library.cc
    ${CHROME_DEPS_DIR}/base/sequence_checker_impl.cc
    ${CHROME_DEPS_DIR}/base/sequenced_task_runner.cc
    ${CHROME_DEPS_DIR}/base/sha1_portable.cc
    ${CHROME_DEPS_DIR}/base/shared_memory_posix.cc
    ${CHROME_DEPS_DIR}/base/string_number_conversions.cc
    ${CHROME_DEPS_DIR}/base/string_piece.cc
    ${CHROME_DEPS_DIR}/base/string_split.cc
    ${CHROME_DEPS_DIR}/base/string_util.cc
    ${CHROME_DEPS_DIR}/base/string16.cc
    ${CHROME_DEPS_DIR}/base/stringprintf.cc
    ${CHROME_DEPS_DIR}/base/supports_user_data.cc
    ${CHROME_DEPS_DIR}/base/synchronization/cancellation_flag.cc
    ${CHROME_DEPS_DIR}/base/synchronization/condition_variable_posix.cc
    ${CHROME_DEPS_DIR}/base/synchronization/lock.cc
    ${CHROME_DEPS_DIR}/base/synchronization/lock_impl_posix.cc
    ${CHROME_DEPS_DIR}/base/synchronization/waitable_event_posix.cc
    ${CHROME_DEPS_DIR}/base/synchronization/waitable_event_watcher_posix.cc
    ${CHROME_DEPS_DIR}/base/system_monitor/system_monitor.cc
    ${CHROME_DEPS_DIR}/base/system_monitor/system_monitor_posix.cc
    ${CHROME_DEPS_DIR}/base/sys_info.cc
    ${CHROME_DEPS_DIR}/base/sys_info_linux.cc
    ${CHROME_DEPS_DIR}/base/sys_info_posix.cc
    ${CHROME_DEPS_DIR}/base/sys_string_conversions_posix.cc
    ${CHROME_DEPS_DIR}/base/task_runner.cc
    ${CHROME_DEPS_DIR}/base/thread_task_runner_handle.cc
    ${CHROME_DEPS_DIR}/base/threading/non_thread_safe_impl.cc
    ${CHROME_DEPS_DIR}/base/threading/platform_thread_posix.cc
    ${CHROME_DEPS_DIR}/base/threading/post_task_and_reply_impl.cc
    ${CHROME_DEPS_DIR}/base/threading/sequenced_worker_pool.cc
    ${CHROME_DEPS_DIR}/base/threading/simple_thread.cc
    ${CHROME_DEPS_DIR}/base/threading/thread.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_checker_impl.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_collision_warner.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_id_name_manager.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_local_posix.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_local_storage_posix.cc
    ${CHROME_DEPS_DIR}/base/threading/thread_restrictions.cc
    ${CHROME_DEPS_DIR}/base/threading/watchdog.cc
    ${CHROME_DEPS_DIR}/base/threading/worker_pool.cc
    ${CHROME_DEPS_DIR}/base/threading/worker_pool_posix.cc
    ${CHROME_DEPS_DIR}/base/time.cc
    ${CHROME_DEPS_DIR}/base/time_posix.cc
    ${CHROME_DEPS_DIR}/base/timer.cc
    ${CHROME_DEPS_DIR}/base/tracked_objects.cc
    ${CHROME_DEPS_DIR}/base/tracking_info.cc
    ${CHROME_DEPS_DIR}/base/utf_offset_string_conversions.cc
    ${CHROME_DEPS_DIR}/base/utf_string_conversion_utils.cc
    ${CHROME_DEPS_DIR}/base/utf_string_conversions.cc
    ${CHROME_DEPS_DIR}/base/values.cc
    ${CHROME_DEPS_DIR}/base/value_conversions.cc
    ${CHROME_DEPS_DIR}/base/version.cc
    ${CHROME_DEPS_DIR}/base/vlog.cc
    ${CHROME_DEPS_DIR}/base/nix/mime_util_xdg.cc
    ${CHROME_DEPS_DIR}/base/nix/xdg_util.cc
)

set(CHROME_BASE_LIBRARY_INCLUDE_DIRECTORIES
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/..
    ${GLIB_INCLUDE_DIRS}
)

set(CHROME_BASE_LIBRARY_LIBRARIES
    ${BASE_STATIC_LIBRARY_NAME}
    ${ALLOCATOR_LIBRARY_NAME}
    ${XDG_MIME_LIBRARY_NAME}
    ${MODP_B64_LIBRARY_NAME}
    ${DYNAMIC_ANNOTATIONS_LIBRARY_NAME}
    ${LIBEVENT_LIBRARY_NAME}
    ${GLIB_LIBRARIES}
    -lrt
    -ldl
    -lpthread
)

set(XDG_MIME_SOURCES
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmime.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimealias.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimecache.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimeglob.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimeicon.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimeint.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimemagic.c
    ${CHROME_DEPS_DIR}/base/third_party/xdg_mime/xdgmimeparent.c
)

include_directories(${CHROME_BASE_LIBRARY_INCLUDE_DIRECTORIES})
remove_definitions(-DHAVE_CONFIG_H=1)
add_library(${BASE_STATIC_LIBRARY_NAME} STATIC ${CHROME_DEPS_DIR}/base/base_switches.cc)
add_library(${ALLOCATOR_LIBRARY_NAME} STATIC ${CHROME_DEPS_DIR}/base/allocator/allocator_extension_thunks.cc)
add_library(${MODP_B64_LIBRARY_NAME} STATIC ${CHROME_DEPS_DIR}/third_party/modp_b64/modp_b64.cc)
add_library(${XDG_MIME_LIBRARY_NAME} STATIC ${XDG_MIME_SOURCES})
add_library(${DYNAMIC_ANNOTATIONS_LIBRARY_NAME} STATIC ${CHROME_DEPS_DIR}/base/third_party/dynamic_annotations/dynamic_annotations.c)
set_target_properties(${BASE_STATIC_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
set_target_properties(${ALLOCATOR_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
set_target_properties(${MODP_B64_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
set_target_properties(${DYNAMIC_ANNOTATIONS_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
set_target_properties(${XDG_MIME_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)

add_library(${CHROME_BASE_LIBRARY_NAME} SHARED ${CHROME_BASE_LIBRARY_SOURCES})
target_link_libraries(${CHROME_BASE_LIBRARY_NAME} ${CHROME_BASE_LIBRARY_LIBRARIES})

# untrusted part
include(NaclBuildNexe)

set(libbase_untrusted_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/string16.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sync_socket_nacl.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/time_posix.cc
)

set(libbase_untrusted_include_dirs
    ${CHROME_DEPS_DIR}
    ${CHROME_DEPS_DIR}/ppapi
)

nacl_build_nlib(libbase_untrusted_64.a "" 64 "" "${libbase_untrusted_include_dirs}" "" "" "libbase_untrusted.a" "${libbase_untrusted_sources}")
nacl_build_nlib(libbase_untrusted_32.a "" 32 "" "${libbase_untrusted_include_dirs}" "" "" "libbase_untrusted.a" "${libbase_untrusted_sources}")

add_custom_target(base_untrusted ALL
    DEPENDS build_libbase_untrusted_64.a build_libbase_untrusted_32.a
)
