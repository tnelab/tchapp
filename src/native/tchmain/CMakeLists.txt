# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

SET_CEF_TARGET_OUT_DIR()
set (LIBRARY_OUTPUT_PATH ${CEF_TARGET_OUT_DIR})
if(OS_LINUX)
add_definitions(-DOS_LINUX)
endif()

# Append platform specific sources to a list of sources.

set(TCHMAIN_SRC
  TchMainExport.h
  TchMainExportGtk.cc  
  TchClientApi.cc
)
source_group(. FILES ${TCHMAIN_SRC})

add_library(tchmain SHARED ${TCHMAIN_SRC})
SET_LIBRARY_TARGET_PROPERTIES(tchmain)
# Remove the default "lib" prefix from the resulting library.
set_target_properties(tchmain PROPERTIES PREFIX "")

#if(OS_LINUX)
#target_link_libraries(${CEF_TARGET})
#endif()
