#define SOKOL_IMPL
#define SOKOL_GLCORE33
#define SOKOL_WIN32_NO_GL_LOADER
#define SOKOL_WIN32_FORCE_MAIN
#include "sokol/sokol_app.h"
#include "glad/glad.h"
#include "platform.h"

static input_t input_state;
static context_t ctx;
static HMODULE game_code;

void (*game_init)(context_t* ctx_ptr, rf_gfx_backend_init_data* gfx_data);
void (*game_update)(const input_t* input);
void (*game_window_resize)(int, int);

static void sokol_on_init(void)
{
    gladLoadGL();

    CopyFile("libgame.dll", "game.dll", FALSE);
    game_code = LoadLibraryA("game.dll");

    game_init          = GetProcAddress(game_code, "game_init");
    game_update        = GetProcAddress(game_code, "game_update");
    game_window_resize = GetProcAddress(game_code, "game_window_resize");

    ctx.memory_size = 1024*1024*30;
    ctx.memory = malloc(ctx.memory_size);

    ctx.screen_width  = 800;
    ctx.screen_height = 450;
    ctx.window_title  = "rayfork template";

    game_init(&ctx, RF_DEFAULT_GFX_BACKEND_INIT_DATA);
}

static void sokol_on_frame(void)
{
    game_update(&input_state);

    for (int i = 0; i < sizeof(input_state.keys) / sizeof(key_t); i++)
    {
        if (input_state.keys[i] == KEY_RELEASE)
        {
            input_state.keys[i] = KEY_DEFAULT_STATE;
        }

        if (input_state.keys[i] == KEY_PRESSED_DOWN)
        {
            input_state.keys[i] = KEY_HOLD_DOWN;
        }
    }
}

static void sokol_on_event(const sapp_event* event)
{
    switch (event->type)
    {
        case SAPP_EVENTTYPE_RESIZED:
            game_window_resize(event->window_width, event->window_height);
            break;

        case SAPP_EVENTTYPE_MOUSE_MOVE:
            input_state.mouse_x = (int) event->mouse_x;
            input_state.mouse_y = (int) event->mouse_y;
            break;

        case SAPP_EVENTTYPE_KEY_DOWN:
            if (input_state.keys[event->key_code] != KEY_PRESSED_DOWN) {
                input_state.keys[event->key_code] = KEY_PRESSED_DOWN;
            }
            break;

        case SAPP_EVENTTYPE_KEY_UP:
            if (event->key_code == SAPP_KEYCODE_R)
            {
                FreeLibrary(game_code);

                CopyFile("libgame.dll", "game.dll", FALSE);
                game_code = LoadLibraryA("game.dll");

                game_init          = GetProcAddress(game_code, "game_init");
                game_update        = GetProcAddress(game_code, "game_update");
                game_window_resize = GetProcAddress(game_code, "game_window_resize");

                game_init(&ctx, NULL);
            }

            input_state.keys[event->key_code] = KEY_RELEASE;
            break;

        default:
            break;
    }
}

sapp_desc sokol_main(int argc, char** argv)
{
    return (sapp_desc)
    {
        .width    = 800,
        .height   = 450,
        .init_cb  = sokol_on_init,
        .frame_cb = sokol_on_frame,
        .event_cb = sokol_on_event,
    };
}