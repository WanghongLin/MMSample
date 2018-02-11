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
#include "SDL2/SDL_image.h"
#include "gif_lib.h"

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    SDL_Window* sdlWindow;
    SDL_Renderer* sdlRenderer;
    SDL_CreateWindowAndRenderer(400, 400, 0, &sdlWindow, &sdlRenderer);

    // SDL_Texture* sdlTexture = IMG_LoadTexture(sdlRenderer, "/home/mutter/Downloads/Rotating_earth_(large).gif");

	int error;
	GifFileType* gifFileType = DGifOpenFileName("/home/mutter/Downloads/Rotating_earth_(large).gif", &error);
    if (gifFileType != nullptr) {
		if ((error = DGifSlurp(gifFileType)) == GIF_OK) {
			std::cout << "gif image count " << gifFileType->ImageCount << std::endl;
		} else {
			std::cout << "slurp gif " << GifErrorString(error) << std::endl;
		}
	} else {
        std::cerr << "open gif error " << GifErrorString(error) << std::endl;
	}

	SDL_Texture* sdlTexture;
	if (gifFileType != nullptr) {
		sdlTexture = SDL_CreateTexture(sdlRenderer, 0, SDL_TEXTUREACCESS_STREAMING,
									   gifFileType->SWidth, gifFileType->SHeight);
	} else {
		exit(EXIT_FAILURE);
	}

    Uint32 format;
	int access, w, h;
	if (SDL_QueryTexture(sdlTexture, &format, &access, &w, &h) == 0) {
		SDL_SetWindowSize(sdlWindow, w, h);
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		SDL_RenderSetLogicalSize(sdlRenderer, w, h);
	}

    int i = 0;
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

		void* pixel;
        int pitch;
        SDL_LockTexture(sdlTexture, nullptr, &pixel, &pitch);
		memcpy(pixel, gifFileType->SavedImages[i].RasterBits,
			   (size_t) (gifFileType->SavedImages[i].ImageDesc.Width * gifFileType->SavedImages[i].ImageDesc.Height) * 4);
        SDL_UnlockTexture(sdlTexture);

    	SDL_RenderClear(sdlRenderer);
    	SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);
    	SDL_RenderPresent(sdlRenderer);
    	SDL_Delay(100);
		if (++i >= gifFileType->ImageCount) {
			i = 0;
		}
    }

	DGifCloseFile(gifFileType, &error);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    return 0;
}