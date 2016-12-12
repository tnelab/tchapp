# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.



#
# Source files.
#

# cefclient common sources.
set(CEFCLIENT_COMMON_COMMON_SRCS
  common/client_app.cc
  common/client_app.h
  common/client_app_delegates_common.cc
  common/client_app_other.cc
  common/client_app_other.h
  common/client_switches.cc
  common/client_switches.h
  common/scheme_test_common.cc
  common/scheme_test_common.h
  )
  APPEND_PLATFORM_SOURCES(CEFCLIENT_COMMON_COMMON_SRCS)
source_group(cefclient\\\\common FILES ${CEFCLIENT_COMMON_COMMON_SRCS})

set(CEFCLIENT_COMMON_SRCS
  ${CEFCLIENT_COMMON_COMMON_SRCS}
  )

# cefclient renderer sources.
set(CEFCLIENT_RENDERER_RENDERER_SRCS
  renderer/client_app_delegates_renderer.cc
  renderer/client_app_renderer.cc
  renderer/client_app_renderer.h
  renderer/client_renderer.cc
  renderer/client_renderer.h
  renderer/performance_test.cc
  renderer/performance_test.h
  renderer/performance_test_setup.h
  renderer/performance_test_tests.cc
  )
APPEND_PLATFORM_SOURCES(CEFCLIENT_RENDERER_RENDERER_SRCS)
source_group(cefclient\\\\renderer FILES ${CEFCLIENT_RENDERER_RENDERER_SRCS})

set(CEFCLIENT_RENDERER_SRCS
  ${CEFCLIENT_RENDERER_RENDERER_SRCS}
  )

#cefclient Linux sources
set(CEFCLIENT_LINUX_SRCS 
 cefclient_gtk.cc
  )
APPEND_PLATFORM_SOURCES(CEFCLIENT_LINUX_SRCS)
source_group(cefclient FILES ${CEFCLIENT_LINUX_SRCS})

set(CEFCLIENT_SRCS
    ${CEFCLIENT_COMMON_SRCS}
    ${CEFCLIENT_RENDERER_SRCS}
    ${CEFCLIENT_LINUX_SRCS}
    )


set(CEF_TARGET "tchsubprocess")
# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")


SET_CEF_TARGET_OUT_DIR()
set (EXECUTABLE_OUTPUT_PATH ${CEF_TARGET_OUT_DIR})

# Executable target.
add_executable(${CEF_TARGET} ${CEFCLIENT_SRCS})
SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})