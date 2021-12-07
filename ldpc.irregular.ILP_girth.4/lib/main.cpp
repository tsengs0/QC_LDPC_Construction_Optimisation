#include "decoder.h"
#include <random>
#include <ctime>
#ifdef _OPENMP
	#include <omp.h>
#endif // _OPENMP

using namespace std; 

//#define MACKAY_REGULAR_504_1008
#define IEEE_802_16E_N2304_R50_Z96

#ifdef MACKAY_REGULAR_504_1008
	#define base_graph_row 6
	#define base_graph_col 12
	#define dc 6
	#define dv 3
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

	uint8_t rowDegree_distribution[base_graph_row] = {6, 7, 7, 6, 6, 7, 6, 6, 7, 6, 6, 6};
	uint8_t colDegree_distribution[base_graph_col] = {3,3,6,3,3,6,3,6,3,6,3,6,3,2,2,2,2,2,2,2,2,2,2,2};
	uint32_t nonzero_element[76] = {
		2, 3, 9, 10, 13, 14,
		2, 6, 7, 8, 12, 14, 15,
		4, 5, 6, 8, 12, 15, 16,
		1, 3, 9, 10, 16, 17,
		3, 7, 10, 11, 17, 18,
		5, 6, 8, 12, 13, 18, 19,
		3, 4, 10, 11, 19, 20,
		2, 3, 7, 10, 20, 21,
		1, 5, 6, 8, 12, 21, 22,
		6, 8, 11, 12, 22, 23,
		3, 4, 9, 10, 23, 24,
		1, 6, 8, 12, 13, 24
	};

	uint16_t shiftVal[76] = { 
 		0,40,10,0,0,0,10,76,0,26,0,20,0,0,95,56,16,0,10,0,0,0,10,20,0,30,30,0,0,0,0,0,40,30,0,13,0,61,32,30,0,10,20,0,0,10,10,10,0,0,0,0,0,0,0,43,0,30,20,0,71,23,0,42,0,0,0,0,0,0,0,30,20,53,50,81
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
