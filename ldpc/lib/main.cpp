#include "decoder.h"
#include <random>
#include <ctime>
#ifdef _OPENMP
	#include <omp.h>
#endif // _OPENMP

using namespace std; 

int main(int argc, char **argv)
{
	uint32_t V2C_SET_NUM;
	srand(time(NULL));

	V2C_SET_NUM = atoi(argv[1]);
	uint8_t *dc = new uint8_t[1]; dc[0] = 10;
	uint8_t *dv = new uint8_t[1]; dv[0] = 3;
	LDPC_Decoder decoder(
		10,  // vnu_num
		3, // cnu_num
		dc,
		1,
		dv,
		1,
		true // the underlying decoder is layered decoding system
	);

	_V2C_Msg **v2c_in; 
	v2c_in = new _V2C_Msg* [V2C_SET_NUM]; 
	for(uint32_t i=0; i<V2C_SET_NUM; i++){
		v2c_in[i] = new _V2C_Msg[10];
		for(uint32_t j=0; j<10; j++)
			v2c_in[i][j] = rand() % 16;
	}


	_C2V_Msg **c2v_out = new _C2V_Msg* [V2C_SET_NUM];
	for(uint32_t i=0; i<V2C_SET_NUM; i++) c2v_out[i] = new _C2V_Msg[10];

	double t;
#ifdef _OPENMP
	t=omp_get_wtime();
	
	//#pragma omp parallel for
	#pragma omp simd
#endif // _OPENMP 
	for(uint32_t i=0; i<V2C_SET_NUM; i++) {
		decoder.min(
			c2v_out[i],
			v2c_in[i],
			10
		);
		for(uint8_t j=0; j<10; j++) cout << c2v_out[i][j] << "\t";
		cout << endl;

		decoder.soft_product(
			c2v_out[i],
			v2c_in[i],
			10
		);
		for(uint8_t j=0; j<10; j++) cout << c2v_out[i][j] << "\t";
		cout << endl << "=====================================" << endl;

		#ifdef _OPENMP
			//cout << "Number of Threads: " << omp_get_num_threads() << endl;
			//cout << "Thread_" << omp_get_thread_num() << endl;
		#endif // _OPENMP
	}
#ifdef _OPENMP
	t=omp_get_wtime()-t;
	printf("t = %f sec.\r\n", t);
#endif // _OPENMP

	return 0;
}