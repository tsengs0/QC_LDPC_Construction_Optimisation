#ifndef __CHANNEL_H
#define __CHANNEL_H

//#define RAND_MAX 32767

enum {
	AWGN_CH = 0, // Additive White Gaussian Noise Channel
	BSC = 1, // Binary Symmetric Channel
	BAC = 2, // Binary Asymmetric Channel
	Z_CH = 3 // Z Channel, e.g., DRAMs
};

class Channel_Wrapper {
	public:
		unsigned int channel_type;
		float snr;
		float sigma;
		unsigned int block_length;
		float *tran_msg;
		float *received_msg;
		float *channel_msg;
		float *app_msg;

		unsigned int errBits;
		unsigned int bitLoc;


		Channel_Wrapper();
		void init(unsigned int channTypeIn, unsigned int block_length_in);		
		void snr_config(float snr_in, float code_rate);
		void bpsk_modulation(); // output the tran_msg[z-1:0]
		void awgn_sim(); // output the received_msg[z-1:0]
		void update_sigma(float sigma_in);
		void llr_cal(); // output the channel_msg[z-1:0]
		void show_chMsg();
		unsigned int err_check();
};
#endif // __CHANNEL_H