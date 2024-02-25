
#include <jw/popup/x11.h>
#include <jw/popup/popup.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <jw/popup/drw.h>

typedef struct
{
    Display *dpy;
    Drw *drw;
    Fnt *font;
    Clr *scheme_bg;
    Clr *scheme_fg;
} JWPopupX11WindowBackend;

typedef struct
{
    Window win;
} JWPopupX11;

static int min_i(int x, int y)
{
    return x < y ? x : y;
}

/* blah blah interger overflow on multiplication, I have not once used something other than 1 for the first argument of calloc.
I always use it as a convenient "allocate one struct and zero it out". */
__THROW __attribute_malloc__ __attribute_alloc_size__((1)) static void *normal_fucking_calloc(size_t size)
{
    return calloc(1, size);
}

static int jwpopup_x11_window_create(ssize_t x, ssize_t y, JWPopup *popup, const JWPopupContext *ctx)
{
    popup->ctx = normal_fucking_calloc(sizeof(JWPopupX11));
    if (!popup->ctx)
        return -1;

    JWPopupX11 *x11_popup_ctx = popup->ctx;
    const JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;
    Drw *drw = x11_ctx->drw;

    XSetWindowAttributes swa;
    swa.override_redirect = 1;
    swa.border_pixel = 0;
    swa.colormap = drw->cmap;

    x11_popup_ctx->win = XCreateWindow(
        drw->dpy,
        drw->root,
        x,
        y,
        popup->w,
        popup->h,
        0,
        drw->depth,
        CopyFromParent,
        drw->visual,
        CWOverrideRedirect | CWBorderPixel | CWColormap,
        &swa //
    );
    XMapRaised(drw->dpy, x11_popup_ctx->win);

    // Draw background
    drw_setscheme(drw, x11_ctx->scheme_bg);
    drw_rect(drw, 0, 0, popup->w, popup->h, 1, 0);
    drw_map(drw, x11_popup_ctx->win, 0, 0, popup->w, popup->h);
    return 0;
}

static void jwpopup_x11_window_destroy(JWPopup *popup, const JWPopupContext *ctx)
{
    JWPopupX11 *x11_popup_ctx = popup->ctx;
    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;

    XDestroyWindow(x11_ctx->dpy, x11_popup_ctx->win);
    XSync(x11_ctx->dpy, 0);

    free(popup->ctx);
    popup->ctx = NULL;
}

static int jwpopup_x11_window_move(ssize_t x, ssize_t y, struct JWPopup *popup, const struct JWPopupContext *ctx)
{
    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;
    JWPopupX11 *x11_popup_ctx = popup->ctx;
    XMoveWindow(x11_ctx->dpy, x11_popup_ctx->win, x, y);
    XSync(x11_ctx->dpy, x11_popup_ctx->win);
    return 0;
}

static int jwpopup_x11_window_draw_primary_text(const char *text, JWPopup *popup, const JWPopupContext *ctx)
{
    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;
    const JWPopupX11 *x11_popup_ctx = popup->ctx;
    Drw *drw = x11_ctx->drw;

    drw_setscheme(drw, x11_ctx->scheme_fg);
    drw_setfontset(drw, x11_ctx->font);

    const size_t icon_size = popup->has_icon ? (popup->h - popup->inner_padding) : 0;
    const int text_max_width = min_i(drw_fontset_getwidth(drw, text), popup->w - popup->inner_padding);

    drw_text_no_bg(
        drw,
        popup->inner_padding + icon_size,
        popup->inner_padding,
        text_max_width,
        x11_ctx->font->h,
        0,
        text,
        0 //
    );
    drw_map(drw, x11_popup_ctx->win, 0, 0, popup->w, popup->h);
    return 0;
}

static int jwpopup_x11_window_draw_secondary_text(const char *text, JWPopup *popup, const JWPopupContext *ctx)
{
    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;
    const JWPopupX11 *x11_popup_ctx = popup->ctx;
    Drw *drw = x11_ctx->drw;

    drw_setscheme(drw, x11_ctx->scheme_fg);
    drw_setfontset(drw, x11_ctx->font->next);

    const size_t icon_size = popup->has_icon ? (popup->h - popup->inner_padding) : 0;
    drw_text_no_bg(
        drw,
        popup->inner_padding + icon_size,
        popup->inner_padding + x11_ctx->font->h,
        drw_fontset_getwidth(drw, text),
        x11_ctx->font->next->h,
        0,
        text,
        1 //
    );
    drw_map(drw, x11_popup_ctx->win, 0, 0, popup->w, popup->h);
    return 0;
}

static int jwpopup_x11_window_draw_icon(const char *path, JWPopup *popup, const JWPopupContext *ctx)
{
    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;
    const JWPopupX11 *x11_popup_ctx = popup->ctx;
    Drw *drw = x11_ctx->drw;

    const size_t icon_wh = popup->h - popup->inner_padding * 2;

    Img *img = drw_img_load(drw, path, icon_wh, icon_wh);
    if (!img)
        return -1;

    drw_img(drw, img, popup->inner_padding, popup->inner_padding);
    drw_map(drw, x11_popup_ctx->win, 0, 0, popup->w, popup->h);
    return 0;
}

int jwpopup_x11_window_backend_init(
    const char *primary_font,
    const char *secondary_font,
    const char *color_bg,
    const char *color_primary,
    const char *color_secondary,
    JWPopupContext *ctx)
{
    ctx->window_backend_ctx = normal_fucking_calloc(sizeof(JWPopupX11WindowBackend));
    if (!ctx->window_backend_ctx)
        return -1;

    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;

    x11_ctx->dpy = XOpenDisplay(NULL);
    if (!x11_ctx->dpy)
    {
        jwpopup_x11_window_backend_destroy(ctx);
        return -1;
    }

    const int screen = DefaultScreen(x11_ctx->dpy);
    const int w = DisplayWidth(x11_ctx->dpy, screen);
    const int h = DisplayHeight(x11_ctx->dpy, screen);

    /* Create drw, yes the drw is bigger than needed. It would be a pain to make it perfectly fit. */
    x11_ctx->drw = drw_create(x11_ctx->dpy, screen, RootWindow(x11_ctx->dpy, screen), w, h);
    if (!x11_ctx->drw)
    {
        jwpopup_x11_window_backend_destroy(ctx);
        return -1;
    }

    /* Create fontsets */
    {
        const char *fonts[] = {primary_font, secondary_font};
        x11_ctx->font = drw_fontset_create(x11_ctx->drw, fonts, 2);
        if (!x11_ctx->font)
        {
            jwpopup_x11_window_backend_destroy(ctx);
            return -1;
        }
    }

    /* Create color schemes */
    {
        const char *colors_fg[2] = {color_primary, color_secondary};
        x11_ctx->scheme_fg = drw_scm_create(x11_ctx->drw, colors_fg, 2);
        if (!x11_ctx->scheme_fg)
        {
            jwpopup_x11_window_backend_destroy(ctx);
            return -1;
        }

        const char *colors_bg[] = {color_bg, color_bg};
        x11_ctx->scheme_bg = drw_scm_create(x11_ctx->drw, colors_bg, 2);
        if (!x11_ctx->scheme_bg)
        {
            jwpopup_x11_window_backend_destroy(ctx);
            return -1;
        }
    }

    ctx->window_create = jwpopup_x11_window_create;
    ctx->window_destroy = jwpopup_x11_window_destroy;
    ctx->window_move = jwpopup_x11_window_move;
    ctx->window_draw_primary_text = jwpopup_x11_window_draw_primary_text;
    ctx->window_draw_secondary_text = jwpopup_x11_window_draw_secondary_text;
    ctx->window_draw_icon = jwpopup_x11_window_draw_icon;
    ctx->display_width = w;
    ctx->display_height = h;
    return x11_ctx->font->h + x11_ctx->font->next->h * 2;
}

void jwpopup_x11_window_backend_destroy(JWPopupContext *ctx)
{
    if (!ctx->window_backend_ctx)
        return;

    JWPopupX11WindowBackend *x11_ctx = ctx->window_backend_ctx;

    if (x11_ctx->scheme_bg)
        free(x11_ctx->scheme_bg);

    if (x11_ctx->scheme_fg)
        free(x11_ctx->scheme_fg);

    if (x11_ctx->drw)
        drw_free(x11_ctx->drw);

    if (x11_ctx->dpy)
        XCloseDisplay(x11_ctx->dpy);

    free(ctx->window_backend_ctx);
    ctx->window_backend_ctx = NULL;
}
