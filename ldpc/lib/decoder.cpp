#include "decoder.h"

LDPC_Decoder::LDPC_Decoder(uint8_t *dc_in, uint8_t dc_num, uint8_t *dv_in, uint8_t dv_num)
{
	row_degree_type = new uint8_t[dc_num];
	col_degree_type = new uint8_t[dv_num];
	memcpy(row_degree_type, dc_in, dc_num);
	memcpy(col_degree_type, dv_in, dv_num);
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

/*
long double rand_normal(long double mean, long double stddev)
{//Box muller method
    static long double n2 = 0.0;
    static int n2_cached = 0;
    if (!n2_cached)
    {
        long double x, y, r;
        do
        {
            x = 2.0*rand()/RAND_MAX - 1;
            y = 2.0*rand()/RAND_MAX - 1;

            r = x*x + y*y;
        }
        while (r == 0.0 || r > 1.0);
        {
            long double d = sqrt(-2.0*log(r)/r);
            long double n1 = x*d;
            n2 = y*d;
            long double result = n1*stddev + mean;
            n2_cached = 1;
            return result;
        }
    }
    else
    {
        n2_cached = 0;
        return n2*stddev + mean;
    }
}
*/