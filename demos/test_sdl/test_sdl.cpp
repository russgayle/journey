#include <iostream>
#include <string>

#include <SDL.h>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Very Simple SDL", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
  if (window == NULL) {
  	std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
  	SDL_Quit();
  	return 1;
  }

  SDL_Surface *surface = SDL_GetWindowSurface(window);

  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

  SDL_UpdateWindowSurface(window);

  SDL_Delay(2000);

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
