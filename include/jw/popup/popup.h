
#ifndef JWPOPUP_JWPOPUP_H
#define JWPOPUP_JWPOPUP_H

#include <jw/popup/window_backend.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t u32;

struct JWPopup
{
    /* These are all used to animate the popup, either the initial animation or the one that
    happens when another popup appears and the ones already on screen need to make room for it. */
    size_t final_x;
    size_t final_y;
    size_t start_x;
    size_t start_y;
    float animation_t_sec;
    size_t animation_duration_ms;
    bool animation_initial_done;

    /* Including inner padding */
    size_t w;
    /* Including inner padding */
    size_t h;
    size_t inner_padding;

    bool has_icon;

    size_t show_ellapsed_ms;
    size_t show_duration_ms;

    void *ctx;
};

struct JWPopupContext
{
    JWPopup *popups;
    size_t popup_count;

    /* There are all going to be filled out by the backend */
    size_t display_width;
    size_t display_height;

    void *window_backend_ctx;
    jwpopup_window_create_t window_create;
    jwpopup_window_destroy_t window_destroy;
    jwpopup_window_move_t window_move;
    jwpopup_window_draw_primary_text_t window_draw_primary_text;
    jwpopup_window_draw_secondary_text_t window_draw_secondary_text;
    jwpopup_window_draw_icon_t window_draw_icon;
};

int jwpopup_ctx_init(
    const char *primary_font,
    const char *secondary_font,
    const char *color_bg,
    const char *color_primary,
    const char *color_secondary,
    JWPopupWindowBackend backend,
    JWPopupContext *ctx);

void jwpopup_ctx_destroy(JWPopupContext *ctx);

int jwpopup_info(
    const char *text,
    const char *description,
    const char *icon,
    bool play_sound,
    size_t duration_ms,
    JWPopupContext *ctx);

int jwpopup_spin_ctx(float dt, JWPopupContext *ctx);

#endif // !JWPOPUP_JWPOPUP_H
