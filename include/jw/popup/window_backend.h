
#ifndef JWPOPUP_WINDOW_BACKEND_H
#define JWPOPUP_WINDOW_BACKEND_H

#include <stddef.h>
#include <sys/types.h>

typedef enum
{
    JWPOPUP_WINDOW_BACKEND_X11
} JWPopupWindowBackend;

typedef struct JWPopup JWPopup;
typedef struct JWPopupContext JWPopupContext;

typedef int (*jwpopup_window_create_t)(ssize_t x, ssize_t y, struct JWPopup *popup, const struct JWPopupContext *ctx);
typedef void (*jwpopup_window_destroy_t)(struct JWPopup *popup, const struct JWPopupContext *ctx);
typedef int (*jwpopup_window_move_t)(ssize_t x, ssize_t y, struct JWPopup *popup, const struct JWPopupContext *ctx);
typedef int (*jwpopup_window_draw_primary_text_t)(const char *text, struct JWPopup *popup, const struct JWPopupContext *ctx);
typedef int (*jwpopup_window_draw_secondary_text_t)(const char *text, struct JWPopup *popup, const struct JWPopupContext *ctx);
typedef int (*jwpopup_window_draw_icon_t)(const char *path, struct JWPopup *popup, const struct JWPopupContext *ctx);

#endif // !JWPOPUP_WINDOW_BACKEND_H
