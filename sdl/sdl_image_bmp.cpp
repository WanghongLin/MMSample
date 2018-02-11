//
//  Copyright (c) 2016 mutter
//
//  This file is part of MMSample.
//
//  SDLSamples is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  SDLSamples is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SDLSamples.  If not, see <http://www.gnu.org/licenses/>.
//  
//
// Created by mutter on 12/3/16.
//

#include "SDL2/SDL.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface* sdlSurface = SDL_LoadBMP("/home/mutter/Downloads/red_panda_2-wallpaper-800x600.bmp");
    SDL_Window* sdlWindow = SDL_CreateWindow("SDL2 Image", SDL_WINDOWPOS_UNDEFINED,
                                             SDL_WINDOWPOS_UNDEFINED, sdlSurface->w, sdlSurface->h, 0);
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlSurface);
    SDL_FreeSurface(sdlSurface);

    SDL_Event sdlEvent;
    while (true) {
        bool shouldQuit = false;
        while (SDL_PollEvent(&sdlEvent)) {
            if ((sdlEvent.type == SDL_WINDOWEVENT &&
                sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) ||
                    (sdlEvent.type == SDL_KEYDOWN &&
                    sdlEvent.key.keysym.sym == SDLK_ESCAPE)) {
                shouldQuit = true;
                break;
            }
        }
        if (shouldQuit) {
            break;
        }
        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(100);
    }

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    return 0;
}

