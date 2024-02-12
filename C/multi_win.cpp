#include <SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int num_windows = 0;
const int max_windows = 5;
SDL_Window* windows[max_windows];
SDL_Renderer *renderers[max_windows];

void make_new_window()
{
	if (num_windows >= max_windows) return;
	char title[20];
	snprintf(title, 20 ,"Window %d", num_windows+1);
	windows[num_windows] = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
	                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderers[num_windows] = SDL_CreateRenderer(windows[num_windows], -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderers[num_windows], 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RaiseWindow(windows[num_windows]);
	num_windows++;
}

void init()
{
	SDL_Init(SDL_INIT_VIDEO);
	make_new_window();
}

void close()
{
	for (int i=0;i<num_windows;i++) {
		SDL_DestroyRenderer(renderers[i]);
		SDL_DestroyWindow(windows[i]);
	}
	SDL_Quit();
}

int main(int argc, char* args[])
{
	init();
	bool quit = false;
	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				quit = true; // Set quit flag to true if the exit button is pressed
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_n) {
				make_new_window();
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);

			}
		}
		for (int i=0;i<num_windows;i++) {
			SDL_RenderClear(renderers[i]);
			SDL_SetRenderDrawColor(renderers[i], 0xFF, 0x00, 0x00, 0xFF);
			SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
			SDL_RenderFillRect(renderers[i], &fillRect);
			SDL_RenderPresent(renderers[i]);
		}
	}

	close();

	return 0;
}
