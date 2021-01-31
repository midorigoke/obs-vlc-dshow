#include <stdio.h>
#include <obs-module.h>

#include "./vlc_functions.h"
#include "./obs-vlc-dshow-plugin.h"
#include "./obs-vlc-dshow-source.h"

#define SETTINGS_VDEV "vdev"
#define SETTINGS_SIZE "size"
#define SETTINGS_FRAME_RATE "frame-rate"
#define SETTINGS_CACHING "caching"

struct source_data
{
  obs_source_t *source_instance;
  libvlc_media_player_t *media_player;
  struct obs_source_frame frame;
};

static const char *get_name(void *type_data)
{
  UNUSED_PARAMETER(type_data);
  return obs_module_text("SourceName");
}

static void *video_lock(void *data, void **planes)
{
  struct source_data *sd = data;

  for (size_t i = 0; i < MAX_AV_PLANES && sd->frame.data[i] != NULL; i++) {
    planes[i] = sd->frame.data[i];
  }

  return NULL;
}

static void video_display(void *data, void *picture)
{
  UNUSED_PARAMETER(picture);

  struct source_data *sd = data;
  sd->frame.timestamp = (uint64_t)libvlc_clock_() * 1000ULL;

  obs_source_output_video(sd->source_instance, &sd->frame);
}

static bool is_chroma_same(const char *a, const char *b) {
  return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
}

static void set_chroma(char *target, const char *source) {
  target[0] = source[0];
  target[1] = source[1];
  target[2] = source[2];
  target[3] = source[3];
}

static enum video_format convert_video_format(char *chroma, bool *is_full_range)
{
  *is_full_range = false;
  #define CHROMA_TEST(val, ret)	if (is_chroma_same(chroma, val)) return ret;
  #define CHROMA_CONV(val, new_val, ret) if (is_chroma_same(chroma, val)) { set_chroma(chroma, new_val); return ret; }
  #define CHROMA_CONV_FULL(val, new_val, ret) *is_full_range = true; CHROMA_CONV(val, new_val, ret);

  CHROMA_TEST("RGBA", VIDEO_FORMAT_RGBA);
  CHROMA_TEST("BGRA", VIDEO_FORMAT_BGRA);
  CHROMA_TEST("NV12", VIDEO_FORMAT_NV12);
  CHROMA_TEST("I420", VIDEO_FORMAT_I420);
  CHROMA_TEST("IYUV", VIDEO_FORMAT_I420);
  CHROMA_CONV("NV21", "NV12", VIDEO_FORMAT_NV12);
  CHROMA_CONV("I422", "NV12", VIDEO_FORMAT_NV12);
  CHROMA_CONV("Y42B", "NV12", VIDEO_FORMAT_NV12);
  CHROMA_CONV("YV12", "NV12", VIDEO_FORMAT_NV12);
  CHROMA_CONV("yv12", "NV12", VIDEO_FORMAT_NV12);
  CHROMA_CONV_FULL("J420", "J420", VIDEO_FORMAT_I420);
  CHROMA_TEST("UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("UYNV", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("UYNY", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("Y422", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("HDYC", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("AVUI", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("uyv1", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("2vuy", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("2Vuy", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("2Vu1", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("YUY2", VIDEO_FORMAT_YUY2);
  CHROMA_TEST("YUYV", VIDEO_FORMAT_YUY2);
  CHROMA_TEST("YUNV", VIDEO_FORMAT_YUY2);
  CHROMA_TEST("V422", VIDEO_FORMAT_YUY2);
  CHROMA_TEST("YVYU", VIDEO_FORMAT_YVYU);
  CHROMA_CONV("v210", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("cyuv", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("CYUV", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("VYUY", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("NV16", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("NV61", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("I410", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("I422", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("Y42B", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("J422", "UYVY", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("I444", VIDEO_FORMAT_I444);
  CHROMA_CONV_FULL("J444", "RGBA", VIDEO_FORMAT_RGBA);
  CHROMA_CONV("YUVA", "RGBA", VIDEO_FORMAT_RGBA);
  CHROMA_CONV("I440", "I444", VIDEO_FORMAT_I444);
  CHROMA_CONV("J440", "I444", VIDEO_FORMAT_I444);
  CHROMA_CONV("YVU9", "NV12", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("I410", "NV12", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("I411", "NV12", VIDEO_FORMAT_UYVY);
  CHROMA_CONV("Y41B", "NV12", VIDEO_FORMAT_UYVY);
  CHROMA_TEST("GREY", VIDEO_FORMAT_Y800);
  CHROMA_TEST("Y800", VIDEO_FORMAT_Y800);
  CHROMA_TEST("Y8  ", VIDEO_FORMAT_Y800);

  #undef CHROMA_CONV_FULL
  #undef CHROMA_CONV
  #undef CHROMA_TEST

  set_chroma(chroma, "BGRA");
  return VIDEO_FORMAT_BGRA;
}

static unsigned get_format_lines(enum video_format format, unsigned height, size_t plane) {
  switch (format)
  {
    case VIDEO_FORMAT_I420:
    case VIDEO_FORMAT_NV12:
      return (plane == 0) ? height : height / 2;
    case VIDEO_FORMAT_YVYU:
    case VIDEO_FORMAT_YUY2:
    case VIDEO_FORMAT_UYVY:
    case VIDEO_FORMAT_I444:
    case VIDEO_FORMAT_RGBA:
    case VIDEO_FORMAT_BGRA:
    case VIDEO_FORMAT_BGRX:
    case VIDEO_FORMAT_Y800:
      return height;
    case VIDEO_FORMAT_NONE:
    default:
      break;
  }

  return 0;
}

static unsigned video_format(void **data_ptr, char *chroma, unsigned *width, unsigned *height, unsigned *pitches, unsigned *lines)
{
  struct source_data *sd = *data_ptr;
  bool is_full_range;
  enum video_format format = convert_video_format(chroma, &is_full_range);

  if (sd->frame.format != format || sd->frame.width != *width || sd->frame.height != *height)
  {
    obs_source_frame_free(&sd->frame);
    obs_source_frame_init(&sd->frame, format, *width, *height);
    sd->frame.format = format;
    sd->frame.full_range = is_full_range;
    enum video_range_type range_type = is_full_range ? VIDEO_RANGE_FULL : VIDEO_RANGE_PARTIAL;
    video_format_get_parameters(VIDEO_CS_DEFAULT, range_type, sd->frame.color_matrix, sd->frame.color_range_min, sd->frame.color_range_max);
  }

  for (size_t i = 0; sd->frame.data[i] != NULL; i++)
  {
    pitches[i] = (unsigned)sd->frame.linesize[i];
    lines[i] = get_format_lines(sd->frame.format, *height, i);
  }

  return 1;
}

static void *create(obs_data_t *settings, obs_source_t *source)
{
  struct source_data *data = bzalloc(sizeof(*data));
  data->source_instance = source;

  data->media_player = libvlc_media_player_new_(vlc_instance);

  libvlc_video_set_callbacks_(data->media_player, video_lock, NULL, video_display, data);
  libvlc_video_set_format_callbacks_(data->media_player, video_format, NULL);

  obs_source_update(source, NULL);

  return data;
}

static void destroy(void *data)
{
  struct source_data *sd = data;
  if (sd->media_player != NULL)
  {
    libvlc_media_player_stop_(sd->media_player);
    libvlc_media_player_release_(sd->media_player);
  }

  obs_source_frame_free(&sd->frame);
  bfree(sd);
}

static void update(void *data, obs_data_t *settings)
{
  struct source_data *sd = data;
  libvlc_media_t *media = libvlc_media_new_location_(vlc_instance, "dshow://");

  const char *vdev = obs_data_get_string(settings, SETTINGS_VDEV);
  const char *size = obs_data_get_string(settings, SETTINGS_SIZE);
  double frame_rate = obs_data_get_double(settings, SETTINGS_FRAME_RATE);
  long long caching = obs_data_get_int(settings, SETTINGS_CACHING);

  char vdev_option[256] = {0};
  char size_option[256] = {0};
  char frame_rate_option[256] = {0};
  char caching_option[256] = {0};

  snprintf(vdev_option, 256, ":dshow-vdev=%s", vdev);
  snprintf(size_option, 256, ":dshow-size=%s", size);
  snprintf(frame_rate_option, 256, ":dshow-fps=%.2f", frame_rate);
  snprintf(caching_option, 256, ":live-caching=%lld", caching);

  libvlc_media_add_option_(media, vdev_option);
  libvlc_media_add_option_(media, ":dshow-adev=none");
  libvlc_media_add_option_(media, size_option);
  libvlc_media_add_option_(media, frame_rate_option);
  libvlc_media_add_option_(media, caching_option);

  libvlc_media_player_stop_(sd->media_player);

  libvlc_media_player_set_media_(sd->media_player, media);
  libvlc_media_release_(media);

  libvlc_media_player_play_(sd->media_player);
}

static void activate(void *data) {
  struct source_data *sd = data;
  libvlc_media_player_play_(sd->media_player);
}

static void deactivate(void *data) {
  struct source_data *sd = data;
  libvlc_media_player_stop_(sd->media_player);
}

static obs_properties_t *get_properties(void *data)
{
  obs_properties_t *properties = obs_properties_create();

  obs_properties_add_text(properties, SETTINGS_VDEV, obs_module_text("VDevLabel"), OBS_TEXT_DEFAULT);
  obs_properties_add_text(properties, SETTINGS_SIZE, obs_module_text("SizeLabel"), OBS_TEXT_DEFAULT);
  obs_properties_add_float(properties, SETTINGS_FRAME_RATE, obs_module_text("FrameRateLabel"), 0.0, 240.0, 0.01);
  obs_properties_add_int(properties, SETTINGS_CACHING, obs_module_text("CachingLabel"), 0, 10000, 1);

  return properties;
}

static void get_defaults(obs_data_t *settings)
{
  obs_data_set_default_string(settings, SETTINGS_VDEV, "none");
  obs_data_set_default_string(settings, SETTINGS_SIZE, "");
  obs_data_set_default_double(settings, SETTINGS_FRAME_RATE, 0.0);
  obs_data_set_default_int(settings, SETTINGS_CACHING, 50);
}

struct obs_source_info vlc_dshow_source_info =
{
  .id = "vlc-dshow-source",
  .type = OBS_SOURCE_TYPE_INPUT,
  .output_flags = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_DO_NOT_DUPLICATE,
  .get_name = get_name,
  .create = create,
  .destroy = destroy,
  .update = update,
  .activate = activate,
  .deactivate = deactivate,
  .get_properties = get_properties,
  .get_defaults = get_defaults,
};