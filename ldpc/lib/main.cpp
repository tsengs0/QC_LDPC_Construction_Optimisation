#include "decoder.h"
#include <random>
#include <ctime>
#ifdef _OPENMP
	#include <omp.h>
#endif // _OPENMP

using namespace std; 

int main(int argc, char **argv)
{
	srand(time(NULL));

	uint8_t *dc = new uint8_t[1]; dc[0] = 10;
	uint8_t *dv = new uint8_t[1]; dv[0] = 3;
	LDPC_Decoder decoder(
		10,  // vnu_num
		3, // cnu_num
		dc,
		1,
		dv,
		1,
		765,
		true // the underlying decoder is layered decoding system
	);

	uint16_t shiftVal[30] = { 
		0, 123, 764, 618, 50, 0, 519, 40, 0, 430,
		30, 0, 351, 20, 0, 282, 10, 0, 213, 0,
		0, 144, 0, 10, 65, 0, 20, 0, 24, 54
	};
	decoder.liftDegree_preload(shiftVal);
	decoder.layered_scheduler();

	return 0;
}