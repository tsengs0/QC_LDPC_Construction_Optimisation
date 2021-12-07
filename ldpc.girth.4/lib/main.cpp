#include "decoder.h"
#include <random>
#include <ctime>
#ifdef _OPENMP
	#include <omp.h>
#endif // _OPENMP

using namespace std; 

uint8_t ITER_MAX;
uint8_t BP_MS_NMS;
int main(int argc, char **argv)
{
	int rand_seed;
	time_t tloc;
	time(&tloc);
	rand_seed=tloc;
	srand48(rand_seed);

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
		((float) atoi(argv[2]))/10.0,
		true // the underlying decoder is layered decoding system
	);

	uint16_t shiftVal[30] = { 
	/*==========================================
		girth =  8	122400 cycles
		EMD	Number
		32	122400
		girth =  10	412335 cycles
		EMD	Number
		40	412335
	*/
	//	0, 123, 764, 618, 50, 0, 519, 40, 0, 430,
	//	30, 0, 351, 20, 0, 282, 10, 0, 213, 0,
	//	0, 144, 0, 10, 65, 0, 20, 0, 24, 54

	/*==========================================
	girth =  10	133110 cycles
	EMD	Number
	5	133110
	*/
	//	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	//	0, 656, 76, 132, 184, 233, 216, 490, 714, 715,
	//	0, 22, 650, 359, 587, 65, 463, 635, 91, 100

	/*==========================================
		girth =  4	103275 cycles
		EMD	Number
		0	103275
		girth =  6	550800 cycles
		EMD	Number
		0	550800
		girth =  8	11308995 cycles
		EMD	Number
		0	11308995
		girth =  10	135496800 cycles
		EMD	Number
		0	135496800
	*/	
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	decoder.liftDegree_preload(shiftVal);

	ITER_MAX = atoi(argv[1]);
	BP_MS_NMS = atoi(argv[3]);
	decoder.layered_scheduler();

	return 0;
}
