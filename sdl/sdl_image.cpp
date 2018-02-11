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

#include <iostream>
#include <SDL2/SDL_image.h>

int main(int argc, char *argv[])
{
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    SDL_CreateWindowAndRenderer(100, 100, 0, &sdlWindow, &sdlRenderer);

    IMG_Init(IMG_INIT_JPG);

    SDL_Texture* sdlTexture = IMG_LoadTexture(sdlRenderer, "/home/mutter/Downloads/red_panda_2-wallpaper-800x600.jpg");

    Uint32 format;
    int access, w, h;
    if (SDL_QueryTexture(sdlTexture, &format, &access, &w, &h) == 0) {
        SDL_SetWindowSize(sdlWindow, w, h);
    }

    SDL_Event sdlEvent;
    while (true) {
        bool shouldQuit = false;
        while (SDL_PollEvent(&sdlEvent)) {
            if (sdlEvent.type == SDL_WINDOWEVENT
                && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
                shouldQuit = true;
                break;
            }
            if (sdlEvent.type == SDL_KEYDOWN
                && sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                shouldQuit = true;
                break;
            }
        }

        if (shouldQuit) {
            break;
        }

        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
        SDL_Delay(100);
    }

    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    return 0;
}