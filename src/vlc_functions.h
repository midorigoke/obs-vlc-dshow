#pragma once

#include <basetsd.h>
typedef SSIZE_T ssize_t;

#include <libvlc.h>
#include <libvlc_renderer_discoverer.h>
#include <libvlc_media.h>
#include <libvlc_media_player.h>
#include <libvlc_media_list.h>
#include <libvlc_media_list_player.h>
#include <libvlc_media_library.h>
#include <libvlc_media_discoverer.h>
#include <libvlc_events.h>
#include <libvlc_dialog.h>
#include <libvlc_vlm.h>
#include <deprecated.h>

extern bool load_module(void);
extern void unload_module(void);
extern bool load_functions(void);

typedef libvlc_instance_t *(*LIBVLC_NEW)(int argc, const char *const *argv);
typedef void (*LIBVLC_RELEASE)(libvlc_instance_t *p_instance);

typedef libvlc_media_player_t *(*LIBVLC_MEDIA_PLAYER_NEW)(libvlc_instance_t *p_libvlc_instance);
typedef void (*LIBVLC_MEDIA_PLAYER_RELEASE)(libvlc_media_player_t *p_mi);
typedef void (*LIBVLC_MEDIA_PLAYER_SET_MEDIA)(libvlc_media_player_t *p_mi, libvlc_media_t *p_md);
typedef int (*LIBVLC_MEDIA_PLAYER_PLAY)(libvlc_media_player_t *p_mi);
typedef void (*LIBVLC_MEDIA_PLAYER_STOP)(libvlc_media_player_t *p_mi);

typedef void (*LIBVLC_VIDEO_SET_CALLBACKS)(libvlc_media_player_t *mp, libvlc_video_lock_cb lock, libvlc_video_unlock_cb unlock, libvlc_video_display_cb display, void *opaque);
typedef void (*LIBVLC_VIDEO_SET_FORMAT_CALLBACKS)(libvlc_media_player_t *mp, libvlc_video_format_cb setup, libvlc_video_cleanup_cb cleanup);

typedef libvlc_media_t *(*LIBVLC_MEDIA_NEW_LOCATION)(libvlc_instance_t *p_instance, const char *psz_mrl);
typedef void (*LIBVLC_MEDIA_RELEASE)(libvlc_media_t *p_md);
typedef void (*LIBVLC_MEDIA_ADD_OPTION)(libvlc_media_t *p_md, const char *psz_options);

typedef int64_t (*LIBVLC_CLOCK)(void);

extern LIBVLC_NEW libvlc_new_;
extern LIBVLC_RELEASE libvlc_release_;

extern LIBVLC_MEDIA_PLAYER_NEW libvlc_media_player_new_;
extern LIBVLC_MEDIA_PLAYER_RELEASE libvlc_media_player_release_;
extern LIBVLC_MEDIA_PLAYER_SET_MEDIA libvlc_media_player_set_media_;
extern LIBVLC_MEDIA_PLAYER_PLAY libvlc_media_player_play_;
extern LIBVLC_MEDIA_PLAYER_STOP libvlc_media_player_stop_;

extern LIBVLC_VIDEO_SET_CALLBACKS libvlc_video_set_callbacks_;
extern LIBVLC_VIDEO_SET_FORMAT_CALLBACKS libvlc_video_set_format_callbacks_;

extern LIBVLC_MEDIA_NEW_LOCATION libvlc_media_new_location_;
extern LIBVLC_MEDIA_RELEASE libvlc_media_release_;
extern LIBVLC_MEDIA_ADD_OPTION libvlc_media_add_option_;

extern LIBVLC_CLOCK libvlc_clock_;
