#ifndef __LDPC_DECODER_H
#define __LDPC_DECODER_H
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include "channel.h"

typedef float _V2C_Msg;
typedef float _C2V_Msg;
typedef float _EXT_Msg;
typedef float _LAYER_MEM_TYPE;

typedef struct BaseMatrix_t{
	uint8_t **matrix;
	int32_t **circulantShift;
} BaseMatrix;

typedef struct APP_MEM_t {
	_LAYER_MEM_TYPE *layered_app_vec;
	//_LAYER_MEM_TYPE *layered_c2v_vec;
} APP_MEM;

class LDPC_Decoder {
	private:
		uint32_t vnu_num; // the number of variable node units
		uint32_t cnu_num; // the number of check node units
		uint8_t *row_degree_vec;
		uint8_t *col_degree_vec;
		Channel_Wrapper *channel_block;

		// Only used in QC-LDPC decoding system
		BaseMatrix base_matrix;
		uint16_t lift_degree; // Z-value
		uint32_t base_row_num;
		uint32_t base_col_num;
		uint32_t layer_num;
		uint16_t ***shift_patterns;
		APP_MEM *layeredMEMs;

	public:
		LDPC_Decoder (
			uint32_t vnu_num_in,
			uint32_t cnu_num_in,
			uint8_t *dc_in, 
			uint8_t dc_num, 
			uint8_t *dv_in, 
			uint8_t dv_num,
			uint16_t lift_degree_in,
			bool isLayered
		);

		void min (
			_C2V_Msg *c2v_out, 

			_V2C_Msg *v2c_in, 
			uint8_t dc_in
		);

		void soft_product(
			_C2V_Msg *c2v_out,
		
			_V2C_Msg *v2c_in, 
			uint8_t dc_in
		);

		uint8_t sum(
			_V2C_Msg *v2c_out,
			
			_C2V_Msg *c2v_in,
			_V2C_Msg channel_msg, // such as initial LLR
			uint8_t dv_in
		);
		/*=====================================================*/
		// Layered Decoding Mechanism
		/*=====================================================*/
		void liftDegree_preload(uint16_t *shiftVal_in);
		void precompute_circulant_shift();
		void check_MemAddr();
		void permuation_run(
			uint8_t layer_id,
			uint32_t base_col_id,
			_EXT_Msg *extrinsic_vec_in
		);
		void layered_scheduler(); // top module
};
#endif // __LDPC_DECODER_H