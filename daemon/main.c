
#include <jw/popup/popup.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    JWPopupContext ctx;
    int st = jwpopup_ctx_init(
        "Ubuntu Mono Nerd Font:size=14",
        "Ubuntu Mono Nerd Font:size=12",
        "#F00D1821",
        "#FFFFFFFF",
        "#FF999999",
        JWPOPUP_WINDOW_BACKEND_X11,
        &ctx //
    );

    if (st < 0)
        return -1;

    jwpopup_info(
        "Warning",
        "The penis is mare this is a ",
        "/home/oda/Downloads/9db.png",
        false,
        5000,
        &ctx //
    );

    struct timeval tv_start;
    struct timeval tv_end;
    float dt = 0;
    const float min_target_dt = 1.f / 60.f;

    while (true)
    {
        gettimeofday(&tv_start, NULL);

        if (jwpopup_spin_ctx(dt, &ctx) <= 0)
            break;

        gettimeofday(&tv_end, NULL);

        const float start_sec = tv_start.tv_sec + (float)tv_start.tv_usec / 1000000.f;
        const float end_sec = tv_end.tv_sec + (float)tv_end.tv_usec / 1000000.f;

        dt = end_sec - start_sec;
        if (dt < min_target_dt)
        {
            usleep((min_target_dt - dt) * 1000000);
            dt = min_target_dt;
        }
    }

    jwpopup_ctx_destroy(&ctx);
    return 0;
}
