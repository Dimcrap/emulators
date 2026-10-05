#include "chip8.h" 



Chip8::Chip8():
randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	pc=START_ADDRESS;
	randbyte = std::uniform_int_distribution<uint8_t>(0,255U);

	for( unsigned int i = 0 ; i < FONTSET_SIZE ; ++i){
		memory[START_ADDRESS+i] = fontest[i] ;
	};

	//auto val = randbyte.operator();
}


void Chip8::loadROM(char const * filepath  ){
	std::ifstream file(filepath , std::ios::binary | std::ios::ate );

	if(file.is_open()){
		std::streampos size = file.tellg();
		char * buffer = new char[size];


		file.seekg(0,std::ios::beg);
		file.read(buffer, size);
		file.close();

		for( long i = 0 ; i < size ; ++i )
		{
			memory[START_ADDRESS + i ] = buffer[i];
		}		

		delete[] buffer;

	} 

}

/*
inline unsigned int Chip8::getrandnum(){
	return randbyte(randGen);
};*/
