#include "decoder.h"

extern uint8_t ITER_MAX;
extern uint8_t BP_MS_NMS;
#define ALPHA 0.6
uint32_t global_col;
LDPC_Decoder::LDPC_Decoder(
	uint32_t vnu_num_in,
	uint32_t cnu_num_in,
	uint8_t *dc_in, 
	uint8_t dc_num, 
	uint8_t *dv_in, 
	uint8_t dv_num,
	uint16_t lift_degree_in,
	float init_snr_db,
	bool isLayered
) {
	vnu_num = vnu_num_in;
	cnu_num = cnu_num_in;
	row_degree_vec = new uint8_t[dc_num];
	col_degree_vec = new uint8_t[dv_num];
	memcpy(row_degree_vec, dc_in, dc_num);
	memcpy(col_degree_vec, dv_in, dv_num);
	code_rate = (float) (vnu_num-cnu_num) / (float) vnu_num;
	snr_db = init_snr_db; // default value

	if(isLayered == true) {
		base_row_num = cnu_num;
		base_col_num = vnu_num;
		layer_num = base_row_num;
		lift_degree = lift_degree_in;
	
		base_matrix.matrix = new uint8_t* [base_row_num];
		base_matrix.circulantShift = new int32_t* [base_row_num];
		//std::cout << "base_row_num: " << base_row_num << ", base_col_num: " << base_col_num << std::endl;
		for(uint32_t i=0; i<base_row_num; i++) {
			base_matrix.matrix[i] = new uint8_t[base_col_num];
			base_matrix.circulantShift[i] = new int32_t[base_col_num];

			// For the regular QC code structure
			for(uint32_t j=0; j< base_col_num; j++)
				base_matrix.matrix[i][j]=1;
		}

		shift_patterns = new uint16_t** [base_col_num];
		layeredMEMs = new APP_MEM[base_col_num];
		channel_block = new Channel_Wrapper[base_col_num];
		for(uint32_t col=0; col<base_col_num; col++) {
			shift_patterns[col] = new uint16_t* [layer_num];
			layeredMEMs[col].app_cache = new _LAYER_MEM_TYPE[lift_degree];
			layeredMEMs[col].c2v_cache = new _LAYER_MEM_TYPE* [layer_num];
			for(uint32_t l=0; l<layer_num; l++) {
				shift_patterns[col][l] = new uint16_t[lift_degree];
				layeredMEMs[col].c2v_cache[l] = new _LAYER_MEM_TYPE[lift_degree](); // initial c2v=0
			}

			channel_block[col].init((unsigned int) AWGN_CH, (unsigned int) lift_degree);
			channel_block[col].snr_config(snr_db, code_rate);
		}
	}
	else {
		lift_degree = 1;
	}
}

void LDPC_Decoder::min(
	_C2V_Msg *c2v_out,

	_V2C_Msg *v2c_in
) {
	_V2C_Msg min_0, min_1;
	uint8_t *min_index = new uint8_t[2];
	uint8_t i;

	// Take the absolute values
	_V2C_Msg *abs_v2c_in = new _V2C_Msg[row_degree_vec[0]];
	int8_t common_sign; common_sign = 0;
	int8_t *v2c_sign; v2c_sign = new int8_t[row_degree_vec[0]];
	for(i=0; i<row_degree_vec[0]; i++) {
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

	for(i=2; i<row_degree_vec[0]; i++) {
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
	for(i=0; i<row_degree_vec[0]; i++) {
		// 0 -> 1
		// 1 -> -1
		// (common_sign^v2c_sign[i])*(-2)+1
		c2v_out[i] = (i == min_index[0]) ? min_1*((common_sign^v2c_sign[i])*(-2.0)+1.0) : min_0*((common_sign^v2c_sign[i])*(-2.0)+1.0); 
	}
}

// tanh(17.5) = 0.999999999999999 (%.15lf) under "double" whose precision is about 15~16 decimal digits.
void LDPC_Decoder::soft_product(
	_C2V_Msg *c2v_out,

	_V2C_Msg *v2c_in
) {
	uint8_t i, j;
	// Extrinsic message = ln( (1 + product( tanh(v2c/2) )) / (1 - product( tanh(v2c/2) )))
	//                   = 2 * atanh( product( tanh(v2c/2) ) )
	_C2V_Msg val_0, val_1;

	// to perform the product of tanh(.)
	_C2V_Msg temp;
	uint8_t last_index = row_degree_vec[0]-1;
	for(i=0; i<row_degree_vec[0]; i++) {
		val_0 = 1.0;

		for(j=0; j<row_degree_vec[0]; j++) {
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
	_V2C_Msg channel_msg // such as initial LLR
) {
	uint8_t i, hard_decision; // the sign bit of APP
	_V2C_Msg common_sum;

	// APP calculation
	for(i=0; i<col_degree_vec[0]; i++)
		common_sum += c2v_in[i];
	common_sum += channel_msg;
	hard_decision = (common_sum >= 0) ? 0 : 1;

	// Variable-to-Check messages calculation
	for(i=0; i<col_degree_vec[0]; i++)
		v2c_out[i] = common_sum-c2v_in[i];

	return hard_decision;
}

void LDPC_Decoder::eval_config(uint8_t iter_max_in)
{
	sim_results.iter_max=iter_max_in;
	sim_results.iter_acc=0;
	sim_results.errBits=0;
	sim_results.errFrames=0;
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

void LDPC_Decoder::precompute_circulant_shift()
{
	// Pre-compute the shifting patterns in a base-matrix wise
	for(uint32_t col=0; col<base_col_num; col++) 
		for(uint32_t l=0; l<layer_num; l++) 
			for(uint32_t submatrix_col=0; submatrix_col<lift_degree; submatrix_col++)
				shift_patterns[col][l][submatrix_col] = (uint16_t) ((submatrix_col+base_matrix.circulantShift[l][col]) % (int32_t)lift_degree);
}

void LDPC_Decoder::check_MemAddr()
{
	for(uint32_t col=0; col<base_col_num; col++) {
		std::cout << "==========================================================" << std::endl; 
		std::cout << "Base-Matrix Column_" << col << std::endl;
		for(uint32_t l=0; l<layer_num; l++) {
			std::cout << "Layer_" << l << std::endl;
			for(uint32_t submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
				std::cout << shift_patterns[col][l][submatrix_col] << ", ";
			}
			std::cout << std::endl;
		}
	}
}

// Preloading channel messages onto each associated APP
// as its initial soft information, i.e., APP=ch_msg @ (Iter=0, layer=0)
void LDPC_Decoder::initAPP_preload()
{
	for(uint32_t col_id=0; col_id<base_col_num; col_id++)
		for(uint16_t submatrix_col=0; submatrix_col<lift_degree; submatrix_col++)
			channel_block[col_id].app_msg[submatrix_col] = channel_block[col_id].channel_msg[submatrix_col];
}

void LDPC_Decoder::layer_mem_reset()
{
	uint32_t col_id, layer, submatrix_col;
	for(col_id=0; col_id<base_col_num; col_id++)
		for(layer=0; layer<layer_num; layer++)
			for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++)
				layeredMEMs[col_id].c2v_cache[layer][submatrix_col] = 0.0;
}

void LDPC_Decoder::app_permuation(
	uint8_t layer_id,
	uint32_t base_col_id,
	_EXT_Msg *extrinsic_vec_in
) {
	uint16_t submatrix_col;
	uint32_t col_temp; col_temp=base_col_id;
	uint16_t mem_addr;
	for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
		mem_addr = shift_patterns[col_temp][layer_id][submatrix_col];
		layeredMEMs[col_temp].app_cache[mem_addr] = extrinsic_vec_in[submatrix_col];
	}
}

void LDPC_Decoder::permuation_run(
	_EXT_Msg *shifted_vec_out,
	uint8_t layer_id,
	uint32_t base_col_id,
	_EXT_Msg *extrinsic_vec_in
)
{
	uint16_t submatrix_col;
	uint32_t col_temp; col_temp=base_col_id;
	uint16_t mem_addr;
	for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
		mem_addr = shift_patterns[col_temp][layer_id][submatrix_col];
		shifted_vec_out[mem_addr] = extrinsic_vec_in[submatrix_col];
	}
}

void LDPC_Decoder::layered_scheduler()
{
	sim_results.errBits = 0;
	sim_results.iter_acc=0;
	sim_results.errFrames=0;
	sim_results.ber=0.0;
	sim_results.frame_acc=0;

	// Pre-calculating all memory-write addresses for all layers
	precompute_circulant_shift();

while(sim_results.errFrames < 1000) {
	// a) Initially received the soft information from the underlying channel
	// b) To simulate the encoding
	// c) To simulate the transmission of encoded codeword over the configured Channel
	// b) To calculate all channel LLRs
	for(uint32_t col=0; col<base_col_num; col++) {
		channel_block[col].bpsk_modulation();
		channel_block[col].awgn_sim();
		channel_block[col].llr_cal();
	}

	// Iterative decoding operation
	//eval_config(ITER_MAX);
	uint8_t iter, layer, cnu_layer_cur, cnu_layer_next;
	uint32_t col_id, submatrix_col;

	uint16_t write_addr_cache;
	uint32_t errBits;


	initAPP_preload();
	layer_mem_reset();
	for(iter=0; iter<ITER_MAX; iter++) {
		for(layer=0; layer<layer_num; layer++) {
			// Variable Node Units
			cnu_layer_cur = (uint8_t) ((layer+0) % layer_num);
			for(col_id=0; col_id<base_col_num; col_id++) {
				for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
					// To cache the write addresses being used by both VNUs and CNUs
					write_addr_cache = shift_patterns[col_id][cnu_layer_cur][submatrix_col];
					/////////////////////////////////////////////////////////////////////
					//if(col_id==0 && write_addr_cache == 15) {
					//	std::cout << "Layer_" << (uint16_t) cnu_layer_cur << " ==> ";
					//	std::cout << channel_block[col_id].app_msg[write_addr_cache] << " - ";
					//	global_col = submatrix_col;
					//}
					/////////////////////////////////////////////////////////////////////
					channel_block[col_id].app_msg[write_addr_cache] -=
					layeredMEMs[col_id].c2v_cache[cnu_layer_cur][submatrix_col];
					/////////////////////////////////////////////////////////////////////
					//if(col_id==0 && write_addr_cache==15)
					//	std::cout << layeredMEMs[col_id].c2v_cache[cnu_layer_cur][submatrix_col]
					//		      << " = "
					//			  << channel_block[col_id].app_msg[write_addr_cache]
					//			  << std::endl;
					/////////////////////////////////////////////////////////////////////
				}
			}

			// Check Node Units
			cnu_layer_next = (uint8_t) ((layer+0) % layer_num);
			for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
				if(BP_MS_NMS == 0) layered_softProduct(cnu_layer_next, submatrix_col);
				else if(BP_MS_NMS == 1) layered_min(cnu_layer_next, submatrix_col);
				else if(BP_MS_NMS == 2) layered_normalised_min(cnu_layer_next, submatrix_col);
				else layered_offset_min(cnu_layer_next, submatrix_col);
			}

			// Updating the latest APPs at current layer
			for(col_id=0; col_id<base_col_num; col_id++) {
				for(submatrix_col=0; submatrix_col<lift_degree; submatrix_col++) {
	/////////////////////////////////////////////////////////////////////
	//if(col_id==0 && shift_patterns[col_id][cnu_layer_cur][submatrix_col]==15) {
	//	std::cout << "-------------------" << std::endl;
	//	std::cout << "Layer_" << (uint16_t) cnu_layer_next << " ==> ";
	//	std::cout << channel_block[col_id].app_msg[15] << " + ";
	//}
	/////////////////////////////////////////////////////////////////////

					channel_block[col_id].app_msg[ shift_patterns[col_id][cnu_layer_cur][submatrix_col] ] += 
					layeredMEMs[col_id].c2v_cache[cnu_layer_next][ shift_patterns[col_id][cnu_layer_next][submatrix_col] ];
					
	/////////////////////////////////////////////////////////////////////
	//if(col_id==0 && shift_patterns[col_id][cnu_layer_cur][submatrix_col]==15) {
	//	std::cout << layeredMEMs[col_id].c2v_cache[cnu_layer_next][15]
	//		      << " = "
	//			  << channel_block[col_id].app_msg[15]
	//			  << std::endl << std::endl; 
	//}
	/////////////////////////////////////////////////////////////////////					
					//channel_block[col_id].app_msg[submatrix_col] = layeredMEMs[col_id].app_cache[ shift_patterns[col_id][(uint8_t) ((layer+1) % layer_num)][submatrix_col] ];
				}
			}

		}

		// Syndrome Calculation
		errBits = 0;
		for(col_id=0; col_id<base_col_num; col_id++) {
			//std::cout << "\t\t\t\t";
			errBits += channel_block[col_id].err_check();
			//std::cout << "\t\t\t\tBaseMatrix.col_" << col_id << std::endl;
		}
		//std::cout << "Error count: " << errBits << " @ Iter_" << (uint32_t)iter << "\n======================\n\n";
		if(errBits==0 || iter==ITER_MAX-1) {
			sim_results.errBits += errBits;
			sim_results.errFrames = (errBits == 0) ? sim_results.errFrames : sim_results.errFrames+1;
			sim_results.frame_acc += 1;
			sim_results.ber = sim_results.errBits / (sim_results.frame_acc*7650.0);
			std::cout << snr_db << ","
					  << sim_results.errBits << ","
					  << sim_results.frame_acc << ","
					  << sim_results.errFrames << ","
					  << /*log10(sim_results.ber)*/sim_results.ber << std::endl;
			break;
		}
		else if(errBits != 0 && iter==ITER_MAX-1) {
			sim_results.errBits += errBits;
			sim_results.errFrames = (errBits == 0) ? sim_results.errFrames : sim_results.errFrames+1;
			sim_results.frame_acc += 1;
			sim_results.ber = (double) sim_results.errBits / (double) (sim_results.frame_acc*7650.0);
			std::cout << snr_db << ","
					  << sim_results.errBits << ","
					  << sim_results.frame_acc << ","
					  << sim_results.errFrames << ","
					  << /*og10(sim_results.ber)*/sim_results.ber << std::endl;
		}
	}
}
}

void LDPC_Decoder::layered_min(uint8_t layer_id, uint16_t submatrix_col)
{	
	_V2C_Msg min_0, min_1;
	uint8_t *min_index = new uint8_t[2];
	uint8_t i;

	// Take the absolute values
	_V2C_Msg *abs_v2c_in = new _V2C_Msg[row_degree_vec[0]];
	int8_t common_sign; common_sign = 0;
	int8_t *v2c_sign; v2c_sign = new int8_t[row_degree_vec[0]];
	for(i=0; i<row_degree_vec[0]; i++) {
		abs_v2c_in[i] = fabs(channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ]);
		v2c_sign[i] = (channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] >= 0) ? 0 : 1;
		common_sign = common_sign^v2c_sign[i];

		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ").cnuIn -> " << channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
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

	for(i=2; i<row_degree_vec[0]; i++) {
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
	for(i=0; i<row_degree_vec[0]; i++) {
		// 0 -> 1
		// 1 -> -1
		// (common_sign^v2c_sign[i])*(-2)+1
		layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] = (i == min_index[0]) ? min_1*((common_sign^v2c_sign[i])*(-2.0)+1.0) : min_0*((common_sign^v2c_sign[i])*(-2.0)+1.0); 
		
		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ") " << min_0 << ", " << min_1 << "\tcnuOut -> " << layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
	}	
}

void LDPC_Decoder::layered_normalised_min(uint8_t layer_id, uint16_t submatrix_col)
{	
	_V2C_Msg min_0, min_1;
	uint8_t *min_index = new uint8_t[2];
	uint8_t i;

	// Take the absolute values
	_V2C_Msg *abs_v2c_in = new _V2C_Msg[row_degree_vec[0]];
	int8_t common_sign; common_sign = 0;
	int8_t *v2c_sign; v2c_sign = new int8_t[row_degree_vec[0]];
	for(i=0; i<row_degree_vec[0]; i++) {
		abs_v2c_in[i] = fabs(channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ]);
		v2c_sign[i] = (channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] >= 0) ? 0 : 1;
		common_sign = common_sign^v2c_sign[i];

		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ").cnuIn -> " << channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
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

	for(i=2; i<row_degree_vec[0]; i++) {
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
	for(i=0; i<row_degree_vec[0]; i++) {
		// 0 -> 1
		// 1 -> -1
		// (common_sign^v2c_sign[i])*(-2)+1
		layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] = (i == min_index[0]) ? ALPHA*min_1*((common_sign^v2c_sign[i])*(-2.0)+1.0) : ALPHA*min_0*((common_sign^v2c_sign[i])*(-2.0)+1.0); 
		
		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ") " << min_0 << ", " << min_1 << "\tcnuOut -> " << layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
	}	
}

void LDPC_Decoder::layered_offset_min(uint8_t layer_id, uint16_t submatrix_col)
{	
	_V2C_Msg min_0, min_1;
	uint8_t *min_index = new uint8_t[2];
	uint8_t i;
	// Take the absolute values
	_V2C_Msg *abs_v2c_in = new _V2C_Msg[row_degree_vec[0]];
	int8_t common_sign; common_sign = 0;
	int8_t *v2c_sign; v2c_sign = new int8_t[row_degree_vec[0]];
	for(i=0; i<row_degree_vec[0]; i++) {
		abs_v2c_in[i] = fabs(channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ]);
		v2c_sign[i] = (channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] >= 0) ? 0 : 1;
		common_sign = common_sign^v2c_sign[i];

		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ").cnuIn -> " << channel_block[i].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
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

	for(i=2; i<row_degree_vec[0]; i++) {
		if(abs_v2c_in[i]<min_0) {
			min_1 = min_0; min_index[1] = min_index[0];
			min_0 = abs_v2c_in[i]; min_index[0] = i;
		}
		else if(abs_v2c_in[i]<min_1 && abs_v2c_in[i] != min_0) {
			min_1 = abs_v2c_in[i];
			min_index[1] = i;
		}
	}

	min_0 -= 1.0; min_1 -= 1.0;
	min_0 = (min_0 > 0.0) ? min_0 : 0.0;
	min_1 = (min_1 > 0.0) ? min_0 : 0.0;
	
	// Allocate the values to each corresponding check-to-variable messages' memory region
	for(i=0; i<row_degree_vec[0]; i++) {
		// 0 -> 1
		// 1 -> -1
		// (common_sign^v2c_sign[i])*(-2)+1
		layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] = (i == min_index[0]) ? min_1*((common_sign^v2c_sign[i])*(-2.0)+1.0) : min_0*((common_sign^v2c_sign[i])*(-2.0)+1.0); 
		
		//if(shift_patterns[i][layer_id][submatrix_col]==15)
		//std::cout << "\t\t(" << (int)i << ") " << min_0 << ", " << min_1 << "\tcnuOut -> " << layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] << std::endl;
	}	
}

// tanh(17.5) = 0.999999999999999 (%.15lf) under "double" whose precision is about 15~16 decimal digits.
void LDPC_Decoder::layered_softProduct(uint8_t layer_id, uint16_t submatrix_col)
{
	uint8_t i, j;
	// Extrinsic message = ln( (1 + product( tanh(v2c/2) )) / (1 - product( tanh(v2c/2) )))
	//                   = 2 * atanh( product( tanh(v2c/2) ) )
	_C2V_Msg val_0, val_1;

	// to perform the product of tanh(.)
	_C2V_Msg temp;
	uint8_t last_index = row_degree_vec[0]-1;
	for(i=0; i<row_degree_vec[0]; i++) {
		val_0 = 1.0;

		for(j=0; j<row_degree_vec[0]; j++) {
			if(j == i) {
				if(i == last_index) break;
				else j += 1;
			} 
			temp = channel_block[j].app_msg[ shift_patterns[i][layer_id][submatrix_col] ] / 2.0;

			if(fabs(temp) < 17.5)
				val_0 = tanh(temp)*val_0;
			else if(temp < -17.5)
				val_0 = -1*val_0;
		}
		val_1 = 1.0-val_0;
		val_0 += 1.0;
		layeredMEMs[i].c2v_cache[layer_id][ shift_patterns[i][layer_id][submatrix_col] ] = log(val_0 / val_1);
		//std::cout << "\t\t\tcnuOut -> " << log(val_0/val_1) << std::endl; 
	}
}
