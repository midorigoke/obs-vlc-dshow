#include <obs-module.h>

#include "./vlc_functions.h"
#include "./obs-vlc-dshow-plugin.h"
#include "./obs-vlc-dshow-source.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-vlc-dshow", "en-US")

libvlc_instance_t *vlc_instance = NULL;

bool obs_module_load(void)
{
  bool result = load_module();
  if (!result) return false;
  result = load_functions();
  if (!result) return false;

  vlc_instance = libvlc_new_(0, NULL);
  if (vlc_instance == NULL) return false;

  obs_register_source(&vlc_dshow_source_info);

  return true;
}

void obs_module_unload(void)
{
  libvlc_release_(vlc_instance);
  unload_module();
}