#include <cstring>
#include <stdlib.h>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <random>


constexpr  unsigned int START_ADDRESS = 0x200 ;
constexpr unsigned int FONTSET_SIZE = 80 ;
constexpr unsigned int FONTSET_START_ADDRESS = 0x50 ;

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32


class Chip8{

	uint8_t fontest[FONTSET_SIZE]  {
	0xF0,0x90,0x90,0x90,0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randbyte;

	public:
		uint8_t registers[16]{};
		uint8_t memory[ (0xFFF)+1 ];
		uint16_t index{};
		uint16_t pc{};
		uint16_t stack[16]{};
		uint8_t sp{};
		uint8_t delayTimer{};
		uint8_t soundTimer{};
		uint8_t keypad[16]{};
		uint32_t video[ 64 * 32 ]{};
		uint16_t opcode;


		Chip8();
		void loadROM(char const * filename);	
		inline unsigned int getrandnum(){
			return randbyte(randGen);
		};
		inline void OP_00E0(){
			memset(video,0,sizeof(video));
		};

		inline void OP_00EE(){ 	//return subroutine
			--sp;
			pc = stack[sp] ;
		};

		inline void OP_1nnn(){	// jump
			uint16_t address = opcode & 0x0FFFu ;
			pc = address;
		};

		inline void OP_2nnn(){						//call subroutine
			uint16_t address = opcode & 0x0FFFu ;

			stack[sp] = pc;
			++sp;
			pc = address ;

		};

		inline void OP_3xkk(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t byte = opcode & 0x00FFu ;

			if(registers[Vx]==byte){
				pc+=2;
			}

		}

		inline void OP_4xkk(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t byte = opcode & 0x00FFu ;

			if (registers[Vx] != byte )
			{
				pc+= 2 ;
			}
		}

		inline void OP_5xy0(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = opcode & 0x00FFu ;

			if (‌ registers[Vx] == registers[Vy]  )
				pc+=2 ;
		}


		inline void OP_6xkk(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t byte = opcode & 0x00FFu ;

			registers[Vx] = byte ;
		}


		inline void OP_7xkk(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t byte = opcode & 0x00FFu ;

			registers[Vx] += byte ;
		}

		inline void OP_8xy0(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			registers[Vx] = registers[Vy] ;
		}

		inline void OP_8xy1(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			registers[Vx] |= registers[Vy] ;
		}

		inline void OP_8xy2(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			registers[Vx] &= registers[Vy] ;
		}


		inline void OP_8xy3(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			registers[Vx] ^= registers[Vy] ;
		}


		inline void OP_8xy4(){
			uint8_t Vx =  ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			uint16_t sum = registers[Vx] + registers[Vy] ;
			(sum >‌ 255U) ? registers[0xF]=1 : registers[0xF]=0 ;

			registers[Vx] = sum & 0xFFu ;
		}


		inline void OP_8xy5(){
			uint8_t Vx =  ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			( registers[Vx] > registers[Vy] ) ? registers[0xF] = 1 :  registers[0xF] = 1 ;

			registers[Vx] -= registers[Vy] ;
		}


		inline void OP_8xy6(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;

			registers[0xF] = ( registers[Vx] & 0x1u );
			registers[Vx] >>= 1 ;
		}


		inline void OP_8xy7(){
			uint8_t Vx =  ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			( registers[Vy] > registers[Vx] )? registers[0xF]=1 : registers[0xF]=0 ;

			registers[Vx] = registers[Vy] - registers[Vx] ;
		}


		inline void OP_8xyE(){
			uint8_t Vx = ( opcode & 0x0f00u ) >> 8u ;

			registers[0xF] = ( registers[Vx] & 0x80u ) >> 7u ;

			registers[Vx] <<= 1 ;
		}

		inline void OP_9xy0(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;

			if ( registers[Vx] != registers[Vy] )
				pc += 2 ;
		}

		inline void OP_Annn(){
			uint16_t address = opcode & 0x0FFFu;
			index = address ;
		}

		inline void OP_Bnnn(){
			uint16_t address = opcode & 0x0FFFu;
			pc = registers[0] + address ;
		}

		inline void OP_Cxkk(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t byte = opcode & 0x00FFu ;

			registers[Vx] =  randbyte(randGen) & byte ;
		}

		inline void OP_Dyxn(){ //Draw
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t Vy = ( opcode & 0x00F0u ) >> 4u ;
			uint8_t height = opcode & 0x000Fu ;

			uint8_t xPos = registers[Vx] % VIDEO_WIDTH ;
			uint8_t yPos = registers[Vy] % VIDEO_HEIGHT ;

			registers[0xF] = 0 ;

			for ( unsigned int row {0} ; row < height ; ++ row)
			{
					uint8_t spriteByte = memory[index + row ] ;

					for ( unsigned int col = 0 ; row <height ; ++col ){
						uint8_t spritepixel = spriteByte & ( 0x80u >> col );
						uint32_t * screenPixel = &video[ (yPos+ row ) * VIDEO_HEIGHT + (xPos + col ) ] ;

						if(spritepixel){

							if( * screenPixel == 0xFFFFFFFF )
								registers[0xF] = 1 ;

							*screenPixel ^= 0xFFFFFFFF ;
						}

					}
			}
		}


		inline void OP_ExA1()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;

			uint8_t key = registers[Vx];

			if(!keypad[key])
				pc+=2 ;
		}


		inline void OP_Fx01(){
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			registers[Vx] = delayTimer;
		}


		inline void OP_Fx0A()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;

			if( keypad[0] )
				registers[Vx] = 0 ;
			else if( keypad[1] )
				registers[Vx] = 1 ;
			else if ( keypad[2] )
				registers[Vx] = 2 ;
			else if ( keypad[3] )
				registers[Vx] = 3 ;
			else if ( keypad[4] )
				registers[Vx] = 4 ;
			else if ( keypad[5] )
				registers[Vx] = 5 ;
			else if ( keypad[6] )
				registers[Vx] = 6 ;
			else if ( keypad[7] )
				registers[Vx] = 8 ;
			else if ( keypad[9] )
				registers[Vx] = 9 ;
			else if ( keypad[10] )
				registers[Vx] = 10 ;
			else if ( keypad[11] )
				registers[Vx] = 11 ;
			else if ( keypad[12] )
				registers[Vx] = 12 ;
			else if ( keypad[13] )
				registers[Vx] = 13 ;
			else if ( keypad[14] )
				registers[Vx] = 14 ;
			else if ( keypad[15] )
				registers[Vx] = 15 ;
			else
				pc -= 2 ;

		}


		inline void OP_Fx15()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			delayTimer = registers[Vx] ;
		}


		inline void OP_Fx18()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			soundTimer = registers[Vx] ;
		}


		inline void OP_Fx1E()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			index += registers[Vx] ;
		}


		inline void OP_Fx29() //LD F,Fx
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t digit = registers[Vx] ;

			index = FONTSET_START_ADDRESS + ( 5 * digit);
		}


		inline void OP_Fx33()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;
			uint8_t value = registers[Vx] ;

			memory [index + 2 ] = value % 10 ;
			value /= 10 ;

			memory [ index + 1 ] = value % 10 ;
			value /= 10 ;

			memory[index] = value  % 10 ;

		}


		inline void OP_Fx55()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;

			for ( uint8_t i = 0 ; i <= Vx ; ++i )
			{
				memory[ index + 1 ] = registers[i];
			}

		}

		inline void OP_Fx65()
		{
			uint8_t Vx = ( opcode & 0x0F00u ) >> 8u ;

			for( uint8_t i = 0 ; i <= Vx ; ++i)
			{
				registers[i] = memory[ index+ i ] ;
			}
		}

};
