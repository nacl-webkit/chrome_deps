set(LIBEVENT_SOURCES
    ${CHROME_DEPS_DIR}/third_party/libevent/buffer.c
    ${CHROME_DEPS_DIR}/third_party/libevent/evbuffer.c
    ${CHROME_DEPS_DIR}/third_party/libevent/evdns.c
    ${CHROME_DEPS_DIR}/third_party/libevent/event.c
    ${CHROME_DEPS_DIR}/third_party/libevent/event_tagging.c
    ${CHROME_DEPS_DIR}/third_party/libevent/evrpc.c
    ${CHROME_DEPS_DIR}/third_party/libevent/evutil.c
    ${CHROME_DEPS_DIR}/third_party/libevent/http.c
    ${CHROME_DEPS_DIR}/third_party/libevent/log.c
    ${CHROME_DEPS_DIR}/third_party/libevent/poll.c
    ${CHROME_DEPS_DIR}/third_party/libevent/select.c
    ${CHROME_DEPS_DIR}/third_party/libevent/signal.c
    ${CHROME_DEPS_DIR}/third_party/libevent/strlcpy.c
    ${CHROME_DEPS_DIR}/third_party/libevent/epoll.c
    ${CHROME_DEPS_DIR}/third_party/libevent/epoll_sub.c
)

include_directories(${CHROME_DEPS_DIR}/third_party/libevent/linux)
add_library(${LIBEVENT_LIBRARY_NAME} STATIC ${LIBEVENT_SOURCES})
set_target_properties(${LIBEVENT_LIBRARY_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
