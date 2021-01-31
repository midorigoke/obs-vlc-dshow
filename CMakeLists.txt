project (obs-vlc-dshow)

find_package(LibVLC REQUIRED)
include_directories(${LIBVLC_INCLUDE_DIRS})
add_definitions(${LIBVLC_DEFINITIONS})

set(obs-vlc-dshow_SOURCES
  src/obs-vlc-dshow-plugin.c
  src/obs-vlc-dshow-source.c
  src/vlc_functions.c
)

set(obs-vlc-dshow_HEADERS
  src/obs-vlc-dshow-plugin.h
  src/obs-vlc-dshow-source.h
  src/vlc_functions.h
)

add_library(obs-vlc-dshow MODULE ${obs-vlc-dshow_SOURCES} ${obs-vlc-dshow_HEADERS})

target_link_libraries(obs-vlc-dshow libobs)

set_target_properties(obs-vlc-dshow PROPERTIES FOLDER "plugins")

if (COMMAND install_obs_plugin_with_data)
  install_obs_plugin_with_data(obs-vlc-dshow data)
endif()