
#ifndef JWPOPUP_X11_H
#define JWPOPUP_X11_H

#include <jw/popup/window_backend.h>

int jwpopup_x11_window_backend_init(
    const char *primary_font,
    const char *secondary_font,
    const char *color_bg,
    const char *color_primary,
    const char *color_secondary,
    JWPopupContext *ctx);

void jwpopup_x11_window_backend_destroy(JWPopupContext *ctx);

#endif // !JWPOPUP_X11_H
