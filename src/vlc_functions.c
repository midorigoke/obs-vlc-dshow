#include <windows.h>
#include <obs-module.h>
#include <util/platform.h>
#include "./vlc_functions.h"

LIBVLC_NEW libvlc_new_ = NULL;
LIBVLC_RELEASE libvlc_release_ = NULL;

LIBVLC_MEDIA_PLAYER_NEW libvlc_media_player_new_ = NULL;
LIBVLC_MEDIA_PLAYER_RELEASE libvlc_media_player_release_ = NULL;
LIBVLC_MEDIA_PLAYER_SET_MEDIA libvlc_media_player_set_media_ = NULL;
LIBVLC_MEDIA_PLAYER_PLAY libvlc_media_player_play_ = NULL;
LIBVLC_MEDIA_PLAYER_STOP libvlc_media_player_stop_ = NULL;

LIBVLC_VIDEO_SET_CALLBACKS libvlc_video_set_callbacks_ = NULL;
LIBVLC_VIDEO_SET_FORMAT_CALLBACKS libvlc_video_set_format_callbacks_ = NULL;

LIBVLC_MEDIA_NEW_LOCATION libvlc_media_new_location_ = NULL;
LIBVLC_MEDIA_RELEASE libvlc_media_release_ = NULL;
LIBVLC_MEDIA_ADD_OPTION libvlc_media_add_option_ = NULL;

LIBVLC_CLOCK libvlc_clock_ = NULL;

void *module = NULL;

bool load_module(void)
{
  HKEY key;
  LSTATUS status = RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\VideoLAN\\VLC", &key);
  if (status != ERROR_SUCCESS) return false;

  wchar_t path[1024] = {0};
  DWORD size = 1024;

  status = RegQueryValueExW(key, L"InstallDir", NULL, NULL, (LPBYTE)path, &size);
  if (status == ERROR_SUCCESS)
  {
    char *path_utf8 = NULL;
    wcscat(path, L"\\libvlc.dll");
    os_wcs_to_utf8_ptr(path, 0, &path_utf8);
    module = os_dlopen(path_utf8);
    bfree(path_utf8);
  }

  RegCloseKey(key);

  return module != NULL;
}

void unload_module(void)
{
  if (module != NULL) os_dlclose(module);
  module = NULL;
}

bool load_functions(void)
{
  if (module == NULL) return false;

  #define LOAD(func) func##_ = os_dlsym(module, #func); if (func##_ == NULL) return false;

  LOAD(libvlc_new);
  LOAD(libvlc_release);
  LOAD(libvlc_media_player_new);
  LOAD(libvlc_media_player_release);
  LOAD(libvlc_media_player_set_media);
  LOAD(libvlc_media_player_play);
  LOAD(libvlc_media_player_stop);
  LOAD(libvlc_video_set_callbacks);
  LOAD(libvlc_video_set_format_callbacks);
  LOAD(libvlc_media_new_location);
  LOAD(libvlc_media_release);
  LOAD(libvlc_media_add_option);
  LOAD(libvlc_clock);

  #undef LOAD

  return true;
}
