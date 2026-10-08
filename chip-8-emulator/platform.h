#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>


class Platform{

    private:
        SDL_Window  * window{};
        SDL_Renderer * renderer{};
        SDL_Texture * texture{} ;

    public:
        Platform( char const * title  ,int windowwidth , int windowheight , int texturewidth , int textureheight );
        ~Platform();

        void Update( void const * buffer , int pitch  );
        bool ProcessInput(uint8_t * keys);


};


