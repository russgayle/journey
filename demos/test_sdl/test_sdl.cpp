#include <iostream>
#include <string>

#include <SDL.h>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Hi from SDL", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
  if (window == NULL) {
  	std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
  	SDL_Quit();
  	return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
  	std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
  	SDL_DestroyWindow(window);
  	SDL_Quit();
  	return 1;
  }

  SDL_Quit();
  return 0;
}
