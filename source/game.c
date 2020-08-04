#include "platform.h"

context_t* ctx;

extern void game_init(context_t* ctx_ptr, rf_gfx_backend_init_data* gfx_data)
{
    ctx = ctx_ptr;

    if (ctx->reload_count == 0)
    {
        rf_init(&ctx->ctx, ctx->screen_width, ctx->screen_height, gfx_data, RF_DEFAULT_LOGGER, RF_LOG_TYPE_ALL);

        ctx->batch = rf_create_default_render_batch(RF_DEFAULT_ALLOCATOR);
        rf_set_active_render_batch(&ctx->batch);
    }

    ctx->reload_count++;

    rf_set_global_context_pointer(&ctx->ctx);
}

extern void game_update(const input_t* input)
{
    rf_begin();
    {
        rf_clear(RF_RAYWHITE);

        char text[1024] = {0};
        snprintf(text, sizeof(text), "Hi Daniel!!! You hot-reloaded this game: %d times", ctx->reload_count);
        rf_sizef size = rf_measure_text(rf_get_default_font(), text, 20, 2);
        rf_vec2  pos  = rf_center_to_screen(size.width, size.height);
        rf_draw_text(text, pos.x, pos.y, 20, RF_BLACK);
    }
    rf_end();
}

extern void game_window_resize(int width, int height)
{
    ctx->screen_width  = width;
    ctx->screen_height = height;

    rf_set_viewport(ctx->screen_width, ctx->screen_height);
}