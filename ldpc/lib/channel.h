#ifndef __CHANNEL_H
#define __CHANNEL_H

#define RAND_MAX 32767

enum {
	AWGN = 0, // Additive White Gaussian Noise Channel
	BSC = 1, // Binary Symmetric Channel
	BAC = 2, // Binary Asymmetric Channel
	Z_CH = 3 // Z Channel, e.g., DRAMs
};

class Channel_Wrapper {
	private:
		unsigned int channel_type;
		float sigma;
		unsigned int block_length;
		float *tran_msg;
		float *received_msg;
		float *channel_msg;

	public:
		Channel_Wrapper(unsigned int channTypeIn, unsigned int block_length_in);
		void awgn_sim();
		void llr_cal(float *recv_vec_in);
};


#endif // __CHANNEL_H