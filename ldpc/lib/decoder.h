#ifndef __LDPC_DECODER_H
#define __LDPC_DECODER_H
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

typedef float _V2C_Msg;
typedef float _C2V_Msg;

class LDPC_Decoder {
	private:
		uint32_t vnu_num; // the number of variable node units
		uint32_t cnu_num; // the number of check node units
		uint8_t *row_degree_type;
		uint8_t *col_degree_type;

	public:
		LDPC_Decoder (
			uint8_t *dc_in, 
			uint8_t dc_num, 
			uint8_t *dv_in, 
			uint8_t dv_num)
		;
		
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
};

#endif // __LDPC_DECODER_H