#include "decoder.h"

LDPC_Decoder::LDPC_Decoder(
	uint32_t vnu_num_in,
	uint32_t cnu_num_in,
	uint8_t *dc_in, 
	uint8_t dc_num, 
	uint8_t *dv_in, 
	uint8_t dv_num,
	bool isLayered
) {
	row_degree_vec = new uint8_t[dc_num];
	col_degree_vec = new uint8_t[dv_num];
	memcpy(row_degree_vec, dc_in, dc_num);
	memcpy(col_degree_vec, dv_in, dv_num);

	if(isLayered == true) {
		base_row_num = cnu_num;
		base_col_num = vnu_num;
		layer_num = base_row_num;
	
		base_matrix.matrix = new uint8_t* [base_row_num];
		base_matrix.circulantShift = new int32_t* [base_row_num];
		for(uint32_t i=0; i<base_row_num; i++) {
			base_matrix.matrix[i] = new uint8_t[base_col_num];
			base_matrix.circulantShift[i] = new int32_t[base_col_num];
		}

		shift_patterns = uint16_t** [base_col_num];
		layeredMEMs = new APP_MEM[base_col_num];
		for(uint32_t col=0; col<base_col_num; col++) {
			shift_patterns[col] = new uint16_t* [layer_num];
			layeredMEMs[col].layered_app_vec = new _LAYER_MEM_TYPE[lift_degree];
			for(uint32_t l=0; l<layer_num; l++) 
				shift_patterns[col][l] = new uint16_t[lift_degree];	
		}
	}
}

void LDPC_Decoder::min(
	_C2V_Msg *c2v_out,

	_V2C_Msg *v2c_in, 
	uint8_t dc_in
) {
	_V2C_Msg min_0, min_1;
	uint8_t *min_index = new uint8_t[2];
	uint8_t i;

	// Take the absolute values
	_V2C_Msg *abs_v2c_in = new _V2C_Msg[dc_in];
	int8_t common_sign; common_sign = 0;
	int8_t *v2c_sign; v2c_sign = new int8_t[dc_in];
	for(i=0; i<dc_in; i++) {
		abs_v2c_in[i] = fabs(v2c_in[i]);
		v2c_sign[i] = (v2c_in[i] >= 0) ? 0 : 1;
		common_sign = common_sign^v2c_sign[i];
	}

	// to initialise the minimum and second minimum by comparison between first two V2Cs
	if(abs_v2c_in[0] <= abs_v2c_in[1]) {
		min_0 = abs_v2c_in[0]; min_index[0] = 0;
		min_1 = abs_v2c_in[1]; min_index[1] = 1;
	}
	else {
		min_0 = abs_v2c_in[1]; min_index[0] = 1;
		min_1 = abs_v2c_in[0]; min_index[1] = 0;		
	}

	for(i=2; i<dc_in; i++) {
		if(abs_v2c_in[i]<min_0) {
			min_1 = min_0; min_index[1] = min_index[0];
			min_0 = abs_v2c_in[i]; min_index[0] = i;
		}
		else if(abs_v2c_in[i]<min_1 && abs_v2c_in[i] != min_0) {
			min_1 = abs_v2c_in[i];
			min_index[1] = i;
		}
	}

	// Allocate the values to each corresponding check-to-variable messages' memory region
	for(i=0; i<dc_in; i++) {
		// 0 -> 1
		// 1 -> -1
		// (common_sign^v2c_sign[i])*(-1)+1
		c2v_out[i] = (i == min_index[0]) ? min_1*((common_sign^v2c_sign[i])*(-1)+1) : min_0*((common_sign^v2c_sign[i])*(-1)+1); 
	}
}

// tanh(17.5) = 0.999999999999999 (%.15lf) under "double" whose precision is about 15~16 decimal digits.
void LDPC_Decoder::soft_product(
	_C2V_Msg *c2v_out,

	_V2C_Msg *v2c_in, 
	uint8_t dc_in
) {
	uint8_t i, j;
	// Extrinsic message = ln( (1 + product( tanh(v2c/2) )) / (1 - product( tanh(v2c/2) )))
	//                   = 2 * atanh( product( tanh(v2c/2) ) )
	_C2V_Msg val_0, val_1;

	// to perform the product of tanh(.)
	_C2V_Msg temp;
	uint8_t last_index = dc_in-1;
	for(i=0; i<dc_in; i++) {
		val_0 = 1.0;

		for(j=0; j<dc_in; j++) {
			if(j == i) {
				if(i == last_index) break;
				else j += 1;
			} 
			temp = v2c_in[j] / 2;

			if(fabs(temp) < 17.5)
				val_0 = tanh(temp)*val_0;
			else if(temp < -17.5)
				val_0 = -1*val_0;
		}
		val_1 = 1-val_0;
		val_0 += 1;
		c2v_out[i] = log(val_0 / val_1);
	}
}

uint8_t LDPC_Decoder::sum(
	_V2C_Msg *v2c_out,
	
	_C2V_Msg *c2v_in,
	_V2C_Msg channel_msg, // such as initial LLR
	uint8_t dv_in
) {
	uint8_t i, hard_decision; // the sign bit of APP
	_V2C_Msg common_sum;

	// APP calculation
	for(i=0; i<dv_in; i++)
		common_sum += c2v_in[i];
	common_sum += channel_msg;
	hard_decision = (common_sum >= 0) ? 0 : 1;

	// Variable-to-Check messages calculation
	for(i=0; i<dv_in; i++)
		v2c_out[i] = common_sum-c2v_in[i];

	return hard_decision;
}
/*=====================================================*/
// Layered Decoding Mechanism
/*=====================================================*/
void LDPC_Decoder::liftDegree_preload(uint16_t *shiftVal_in)
{
	uint32_t cnt; cnt=0;
	for(uint32_t row_id=0; row_id<base_row_num; row_id++) {
		for(uint32_t col_id=0; col_id<base_col_num; col_id++) {
			if(base_matrix.matrix[row_id][col_id] == 1) {
				base_matrix.circulantShift[row_id][col_id] = shiftVal_in[cnt];
				cnt += 1;	
			}
			else base_matrix.circulantShift[row_id][col_id] = -1;
		}
	}
}

void LDPC_Decoder::precompute_circulant_shift(uint8_t layer_id)
{
	// Pre-compute the shifting patterns in a base-matrix wise
	for(uint32_t col=0; col<base_col_num; col++) 
		for(uint32_t l=0; l<layer_num; l++) 
			for(uint32_t submatrix_col=0; submatrix_col<lift_degree; submatrix_col++)
				shift_patterns[col][l][submatrix_col] = (uint16_t) ((submatrix_col+base_matrix.circulantShift) % lift_degree)
}

void LDPC_Decoder::permuation_run(
	uint8_t layer_id, ,
	uint32_t base_col_id,
	_EXT_Msg *extrinsic_vec_in
) {
	uint16_t submatrix_col;
	uint32_t col_temp; col_temp=base_col_id;
	uint16_t mem_addr;
	for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
		mem_addr = shift_patterns[col_temp][layer_id][submatrix_col];
		layeredMEMs[col_temp].layered_app_vec[mem_addr] = extrinsic_vec_in[submatrix_col];
	}
}