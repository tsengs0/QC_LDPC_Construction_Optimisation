#include "decoder.h"
#include <random>
#include <ctime>
#ifdef _OPENMP
	#include <omp.h>
#endif // _OPENMP

using namespace std; 

#define MACKAY_REGULAR_504_1008
//#define IEEE_802_16E_N2304_R50_Z96

#ifdef MACKAY_REGULAR_504_1008
	#define base_graph_row 6
	#define base_graph_col 12
	#define dc_max 6
	#define dv_max 3
	#define LIFT_DEGREE 84
#endif // MACKAY_REGULAR_504_1008

#ifdef IEEE_802_16E_N2304_R50_Z96
	#define base_graph_row 12
	#define base_graph_col 24
	#define dc_max 7
	#define dv_max 6
	#define LIFT_DEGREE 96
#endif // IEEE_802_16E_N2304_R50_Z96

uint8_t ITER_MAX;
uint8_t BP_MS_NMS;
int main(int argc, char **argv)
{
	int rand_seed;
	time_t tloc;
	time(&tloc);
	rand_seed=tloc;
	srand48(rand_seed);

	uint8_t rowDegree_distribution[base_graph_row] = {6,6,6,6,6,6};
	uint8_t colDegree_distribution[base_graph_col] = {3,3,3,3,3,3,3,3,3,3,3,3};
	uint32_t nonzero_element[] = {
		1, 3, 5, 7, 9, 11,
		2, 3, 6, 7, 10, 11,
		1, 4, 6, 8, 10, 12,
		2, 4, 5, 7, 9, 11,
		2, 3, 6, 8, 9, 12,
		1, 4, 5, 8, 10, 12
	};

	uint16_t shiftVal[36] = { 
		0,  0,  0,  0,  0,  20, 
		 0, 19,   0, 0,   0, 0, 
		0,   0,  0,  0,  0,  10,
		 0,  0, 0,  0,  0,  10, 
		 19, 0,   0,  10, 0,   0,
		0,   10, 38,   0,  0,  0	
 	};

	/*deprecated*/ uint8_t *dv = new uint8_t[1]; dv[0] = 3;
	LDPC_Decoder decoder(
		base_graph_col,  // vnu_num
		base_graph_row, // cnu_num
		rowDegree_distribution,
		base_graph_row,
		colDegree_distribution,
		base_graph_col,
		nonzero_element,
		LIFT_DEGREE,
		shiftVal,
		((float) atoi(argv[2]))/10.0,
		true // the underlying decoder is layered decoding system
	);
	decoder.liftDegree_preload(shiftVal);
	decoder.precompute_layer_sched();

	ITER_MAX = atoi(argv[1]);
	BP_MS_NMS = atoi(argv[3]);

	std::cout << "SNR," << "errBits," << "frame_acc," << "errFrame," << "BER" << std::endl;
	decoder.layered_scheduler();

	return 0;
}
