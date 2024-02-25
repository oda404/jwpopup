
#include <jw/popup/popup.h>
#include <jw/popup/x11.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static size_t g_popup_width = 370;
static size_t g_popup_height = 0;
#define POPUP_INNER_PADDING 15
#define POPUP_FULL_W (g_popup_width + POPUP_INNER_PADDING * 2)
#define POPUP_FULL_H (g_popup_height + POPUP_INNER_PADDING * 2)

#define POPUP_PADDING 20

static JWPopup *popup_create(JWPopupContext *ctx)
{
    JWPopup *tmp = realloc(ctx->popups, (ctx->popup_count + 1) * sizeof(JWPopup));
    if (!tmp)
        return NULL;

    ctx->popups = tmp;
    ++ctx->popup_count;

    memset(&ctx->popups[ctx->popup_count - 1], 0, sizeof(JWPopup));
    return &ctx->popups[ctx->popup_count - 1];
}

static void popup_delete(size_t idx, JWPopupContext *ctx)
{
    ctx->window_destroy(&ctx->popups[idx], ctx);

    for (size_t i = idx; i < ctx->popup_count - 1; ++i)
        ctx->popups[i] = ctx->popups[i + 1];

    --ctx->popup_count;
    if (ctx->popup_count == 0)
        free(ctx->popups);
    else
        ctx->popups = realloc(ctx->popups, ctx->popup_count * sizeof(JWPopup));
}

static void jwpopup_shift_all_up(JWPopupContext *ctx)
{
    for (size_t i = 0; i < ctx->popup_count - 1; ++i)
    {
        JWPopup *popup = &ctx->popups[i];

        popup->start_y = popup->final_y;
        popup->final_y -= POPUP_FULL_H + POPUP_PADDING;
        popup->animation_t_sec = 0;
    }
}

static void jwpopup_shift_all_down(JWPopupContext *ctx)
{
    for (size_t i = 0; i < ctx->popup_count - 1; ++i)
    {
        JWPopup *popup = &ctx->popups[i];

        popup->start_y = popup->final_y;
        popup->final_y += POPUP_FULL_H + POPUP_PADDING;
        popup->animation_t_sec = 0;
    }
}

int jwpopup_ctx_init(
    const char *primary_font,
    const char *secondary_font,
    const char *color_bg,
    const char *color_primary,
    const char *color_secondary,
    JWPopupWindowBackend backend,
    JWPopupContext *ctx)
{
    memset(ctx, 0, sizeof(JWPopupContext));

    ssize_t rc;
    switch (backend)
    {
    case JWPOPUP_WINDOW_BACKEND_X11:
        rc = jwpopup_x11_window_backend_init(
            primary_font,
            secondary_font,
            color_bg,
            color_primary,
            color_secondary,
            ctx //
        );
        break;

    default:
        return -1;
    }

    if (rc <= 0)
        return -1;

    g_popup_height = (size_t)rc;
    return 0;
}

void jwpopup_ctx_destroy(JWPopupContext *ctx)
{
    for (size_t i = 0; i < ctx->popup_count; ++i)
        popup_delete(i--, ctx); // NOTE: This only works because we use size_t, if 'i' was signed this would be UB

    jwpopup_x11_window_backend_destroy(ctx);
}

int jwpopup_info(
    const char *text,
    const char *description,
    const char *icon,
    bool play_sound,
    size_t duration_ms,
    JWPopupContext *ctx)
{
    (void)play_sound;

    JWPopup *popup = popup_create(ctx);
    if (!popup)
        return -1;

    popup->final_x = ctx->display_width - POPUP_FULL_W - POPUP_PADDING;
    popup->final_y = ctx->display_height - POPUP_FULL_H - POPUP_PADDING;

    popup->start_y = ctx->display_height;
    popup->start_x = popup->final_x;

    popup->has_icon = (bool)icon;
    popup->animation_duration_ms = 150;
    popup->show_duration_ms = duration_ms;

    popup->w = POPUP_FULL_W;
    popup->h = POPUP_FULL_H;
    popup->inner_padding = POPUP_INNER_PADDING;

    ctx->window_create(popup->start_x, popup->start_y, popup, ctx);
    ctx->window_draw_primary_text(text, popup, ctx);
    ctx->window_draw_secondary_text(description, popup, ctx);
    if (icon)
        ctx->window_draw_icon(icon, popup, ctx);

    if (ctx->popup_count > 1)
        jwpopup_shift_all_up(ctx);

    return 0;
}

int jwpopup_spin_ctx(float dt, JWPopupContext *ctx)
{
    for (size_t i = 0; i < ctx->popup_count; ++i)
    {
        JWPopup *popup = &ctx->popups[i];

        if (popup->animation_t_sec < 1)
        {
            popup->animation_t_sec += dt / (popup->animation_duration_ms / 1000.0);
            if (popup->animation_t_sec >= 1)
            {
                popup->animation_initial_done = true; // Will trigger for all animations after, but we don't care
                popup->animation_t_sec = 1;
            }

            ssize_t y = popup->start_y + popup->animation_t_sec * ((float)popup->final_y - (float)popup->start_y);
            ctx->window_move(popup->start_x, y, popup, ctx);
        }

        if (popup->animation_initial_done)
        {
            popup->show_ellapsed_ms += dt * 1000;
            if (popup->show_ellapsed_ms > popup->show_duration_ms)
            {
                jwpopup_shift_all_down(ctx);
                popup_delete(i--, ctx); // NOTE: This only works because we use size_t, if 'i' was signed this would be UB
                continue;
            }
        }
    }

    return ctx->popup_count;
}
