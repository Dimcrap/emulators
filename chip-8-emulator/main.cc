#include <cstdlib>
#include <iostream>
#include <chrono>
#include "platform.h"
#include "chip8.h"

bool debugmode {false};



int main(int argc , char * argv[] )
{
    if( strcmp(argv[0],"gdb")==0 )
        debugmode = true ;



    if (  argc < 4  || argc > 5 )
    {
            std::cerr<< "Usage" << argv[0] << " <Scale> <Delay> <ROM> \n";
            std::exit( EXIT_FAILURE );
    }



    int videoScale = std::stoi( argv[  debugmode ? 2 : 1  ] ) ;
    int cycledelay = std::stoi( argv[  debugmode ? 3 : 2  ] ) ;
    char const * romFilename = argv[  debugmode ? 4 : 3  ] ;

    Platform platform( "CHIP-8 Emulator" , VIDEO_WIDTH * videoScale , VIDEO_HEIGHT * videoScale , VIDEO_WIDTH ,
                       VIDEO_HEIGHT ) ;

    Chip8 chip8 ;
    chip8.loadROM(romFilename);

    int videoPitch { sizeof( chip8.video[0]) * VIDEO_WIDTH };

    auto lastcyleTime { std::chrono::high_resolution_clock::now()} ;
    bool quit {false};

    while(!quit)
    {
        quit = platform.ProcessInput( chip8.keypad ) ;

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float , std::chrono::milliseconds::period >
        ( currentTime - lastcyleTime).count();

        if( dt > cycledelay)
        {
            lastcyleTime = currentTime ;

            chip8.Cycle() ;

            platform.Update(chip8.video , videoPitch );
        }

    }

    return 0 ;

}

