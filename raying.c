#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

#define WIDTH 1000
#define HEIGHT 800
#define NUM_RAYS 720

typedef struct Circle
{
    double cx;
    double cy;
    double r;
} Circle;

void Draw_Circle(const Circle *C, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.0f, 0.0f, 1.0f);

    int minX = (int)fmax(0, C->cx - C->r);
    int maxX = (int)fmin(WIDTH, C->cx + C->r);
    int minY = (int)fmax(0, C->cy - C->r);
    int maxY = (int)fmin(HEIGHT, C->cy + C->r);

    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            double dx = x - C->cx;
            double dy = y - C->cy;
            if (dx * dx + dy * dy <= C->r * C->r)
            {
                SDL_RenderPoint(renderer, (float)x, (float)y);
            }
        }
    }
}

double Ray_Circle_Intersect(Circle *C1, double dx, double dy, const Circle *C2)
{
    double ox = C1->cx;
    double oy = C1->cy;

    double dx0 = ox - C2->cx;
    double dy0 = oy - C2->cy;

    double B = 2.0 * (dx * dx0 + dy * dy0);
    double C_val = (dx0 * dx0 + dy0 * dy0) - (C2->r * C2->r);

    double discriminant = B * B - 4.0 * C_val;

    if (discriminant < 0)
    {
        return -1.0;
    }

    double t = (-B - sqrt(discriminant)) / 2.0;
    return (t > 0) ? t : -1.0;
}

int main(int argc, char *argv[])
{
    Circle C1;
    C1.cx = 300;
    C1.cy = 300;
    C1.r = 50;

    Circle C2;
    C2.cx = 600;
    C2.cy = 600;
    C2.r = 100;

    double c2_speed = 5;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL 初始化失败: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!SDL_CreateWindowAndRenderer("SDL3 Window", WIDTH, HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("窗口创建失败: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    bool is_clicking = false;
    bool is_running = true;
    SDL_Event event;

    while (is_running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                is_running = false;
            }

            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                {
                    double dx = event.button.x - C1.cx;
                    double dy = event.button.y - C1.cy;

                    if (dx * dx + dy * dy <= C1.r * C1.r)
                    {
                        is_clicking = true;
                    }
                }

                if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
                {
                    is_clicking = false;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION && is_clicking)
            {
                C1.cx = event.motion.x;
                C1.cy = event.motion.y;
            }
        }

        C2.cy += c2_speed;

        if (C2.cy - C2.r <= 0)
        {
            C2.cy = C2.r;
            c2_speed = -c2_speed;
        }
        else if (C2.cy + C2.r >= HEIGHT)
        {
            C2.cy = HEIGHT - C2.r;
            c2_speed = -c2_speed;
        }

        SDL_SetRenderDrawColorFloat(renderer, 0.1f, 0.1f, 0.1f, 1.0f);
        SDL_RenderClear(renderer);

        for (int i = 0; i < NUM_RAYS; i++)
        {
            double angle = i * (2.0 * M_PI / NUM_RAYS);
            double dx = cos(angle);
            double dy = sin(angle);

            double t = Ray_Circle_Intersect(&C1, dx, dy, &C2);

            double max_dist = (t > 0) ? t : 1500.0;

            float end_x = (float)(C1.cx + dx * max_dist);
            float end_y = (float)(C1.cy + dy * max_dist);

            SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.9f, 0.2f, 1.0f);
            SDL_RenderLine(renderer, (float)C1.cx, (float)C1.cy, end_x, end_y);
        }

        Draw_Circle(&C1, renderer);
        Draw_Circle(&C2, renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}