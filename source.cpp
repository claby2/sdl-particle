#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <math.h>

#include <iostream>

const int PARTICLE_AMOUNT = 5;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class LTexture {
	public:
		LTexture() {
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
		~LTexture() {
            free();
        }
		bool loadFromFile( std::string path ) {
            free();
            SDL_Texture* newTexture = NULL;
            SDL_Surface* loadedSurface = IMG_Load(path.c_str());
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
            newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
            SDL_FreeSurface(loadedSurface);
            mTexture = newTexture;
            return mTexture != NULL;
        }
		void free() {
            if(mTexture != NULL){
                SDL_DestroyTexture(mTexture);
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
            }
        }
		void setColor( Uint8 red, Uint8 green, Uint8 blue ) {
            SDL_SetTextureColorMod(mTexture, red, green, blue);
        }
		void setBlendMode( SDL_BlendMode blending ) {
            SDL_SetTextureBlendMode(mTexture, blending);
        }
		void setAlpha( Uint8 alpha ) {
            SDL_SetTextureAlphaMod(mTexture, alpha);
        }
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ){
            SDL_Rect renderQuad = {x, y, mWidth, mHeight};
            if(clip != NULL) {
                renderQuad.w = clip->w;
                renderQuad.h = clip->h;
            }
            SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
        }
		int getWidth(){
            return mWidth;
        }
		int getHeight(){
            return mHeight;
        }

	private:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

LTexture gParticleTexture;

class Particle {
    public:
        static const int PARTICLE_WIDTH = 20;
        static const int PARTICLE_HEIGHT = 20;
        static const int PARTICLE_VEL = 3;
        float mPosX, mPosY;

        Particle() {
            mPosX = rand() % SCREEN_WIDTH;
            mPosY = rand() % SCREEN_HEIGHT;

            mVelX = rand() % 2 == 0 ? - (rand() % PARTICLE_VEL + 1) : rand() % PARTICLE_VEL + 1;
            mVelY = rand() % 2 == 0 ? - (rand() % PARTICLE_VEL + 1) : rand() % PARTICLE_VEL + 1;
        }

        void intersect(Particle* b){
            float x1 = mPosX + PARTICLE_WIDTH/2;
            float y1 = mPosY + PARTICLE_HEIGHT/2;
            int r1 = PARTICLE_WIDTH/2;

            float x2 = b->mPosX + b->PARTICLE_WIDTH/2;
            float y2 = b->mPosY + b->PARTICLE_HEIGHT/2;
            int r2 = b->PARTICLE_WIDTH/2;
        

            if((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) <= (r1+r2)*(r1+r2)){
                float angle = atan2(y2 - y1, x2 - x1);

                b->mPosX += cos(angle)*(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
                b->mPosY += cos(angle)*(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
                mPosX += cos(angle)*(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
                mPosY += cos(angle)*(sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
            }
        }

        void move() {
            mPosX += mVelX;
            if((mPosX < 0) || (mPosX + PARTICLE_WIDTH > SCREEN_WIDTH)){
                mVelX = -mVelX;
            }

            mPosY += mVelY;
            if((mPosY < 0) || (mPosY + PARTICLE_HEIGHT > SCREEN_HEIGHT)){
                mVelY = -mVelY;
            }

            mPosX += mVelX;
        }

        void render() {
            gParticleTexture.render(mPosX, mPosY);
        }

        private:
            int mVelX, mVelY;
};

void close() {
    gParticleTexture.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    IMG_Quit();
    SDL_Quit();
}

bool init() {
	bool success = true;

	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else {
		if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf( "Warning: Linear texture filtering not enabled!" );
		}
		gWindow = SDL_CreateWindow( "Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if(gWindow == NULL) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if(gRenderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags)) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

int main(int argc, char* args[]){
    srand((unsigned)time(NULL));
    if(!init()){
        printf("Failed to initialize!\n");
    } else {
        gParticleTexture.loadFromFile("images/dot.bmp");
        Particle* particles[PARTICLE_AMOUNT];

        for(int i = 0; i < PARTICLE_AMOUNT; i++){
            particles[i] = new Particle();
        }

        bool quit = false;

        SDL_Event e;

        while(!quit){
            while(SDL_PollEvent(&e) != 0){
                if(e.type == SDL_QUIT){
                    quit = true;
                }
            }

            SDL_RenderClear(gRenderer);

            for(int i = 0; i < PARTICLE_AMOUNT; i++){
                particles[i]->move();
                particles[i]->render();
                for(int j = 0; j < PARTICLE_AMOUNT; j++){
                    particles[i]->intersect(particles[j]);
                }
            }
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            SDL_RenderPresent(gRenderer);
        }
    }
    close();

    return 0;
}