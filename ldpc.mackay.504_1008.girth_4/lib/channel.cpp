#include <iostream>
#include <cmath>
#include "channel.h"

Channel_Wrapper::Channel_Wrapper()
{

}
/*=====================================================*/
// Initial Configuration
/*=====================================================*/
void Channel_Wrapper::init(
	unsigned int channTypeIn, 
	unsigned int block_length_in
) {
	block_length = block_length_in;
	tran_msg     = new float[block_length];
	received_msg = new float[block_length];
	channel_msg  = new float[block_length];
	app_msg = new float[block_length];
	tran_msg = new float[block_length](); // to simulate the all-zero codeword
	sigma = 0.1; //default value
}
/*=====================================================*/
// Conversion of standard deviation of noise variance
// according to the given SNR (Eb/N0 in dB) and code rate
/*=====================================================*/
void Channel_Wrapper::snr_config(float snr_in, float code_rate)
{
	float EbN0, variance; 
	EbN0=pow(10, snr_in/10);
	variance=1 / (2*code_rate*EbN0);
	update_sigma(sqrt(variance));
}
/*=====================================================*/
// BPSK modulation
/*=====================================================*/
void Channel_Wrapper::bpsk_modulation()
{
	unsigned int i ;
	for (i = 0; i<block_length; i++)
		tran_msg[i] = (tran_msg[i] == 0) ? 1.0 : -1.0;
}
/*=====================================================*/
// AWGN modulation
// y = x + N*sigma where N~(mu=0, variance=sigma^2)
/*=====================================================*/
void Channel_Wrapper::awgn_sim()
{
	float u1,u2,s,noise,randmum;
	unsigned int i;

	for (i=0; i<block_length; i++)
	{
		do {
			randmum = drand48();
			u1 = randmum*2.0f - 1.0f;
			randmum = drand48() ;
			u2 = randmum*2.0f - 1.0f;
			s = u1*u1 + u2*u2;
		} while( s >= 1) ;
		noise = u1 * sqrt( (-2.0f*log(s))/s );
		received_msg[i] = tran_msg[i]+noise*sigma;
	}
}
/*=====================================================*/
// To update the simulated standard deviation on-the-fly
/*=====================================================*/
void Channel_Wrapper::update_sigma(float sigma_in)
{
	sigma = sigma_in;
}
/*=====================================================*/
// calculation of LLRs over the underlying codeword bit
/*=====================================================*/
void Channel_Wrapper::llr_cal()
{
	unsigned int i;
	for (i = 0; i < block_length; i++)
		channel_msg[i] = (received_msg[i]*2)/(sigma*sigma) ; // 2y/sigma^2 ;
}
/*=====================================================*/
// Printing out all channel messages
/*=====================================================*/
void Channel_Wrapper::show_chMsg()
{
	for(unsigned int i=0; i<block_length-1; i++)
		std::cout << channel_msg[i] << ", ";
	std::cout << channel_msg[block_length-1] << std::endl;
}
/*=====================================================*/
// To check if the underlying codeword contain error bit(s)
/*=====================================================*/
unsigned int Channel_Wrapper::err_check()
{
	errBits=0;
	for(bitLoc=0; bitLoc<block_length; bitLoc++) {
		if(app_msg[bitLoc] <= 0) {
			//std::cout << "bitLoc[ " << bitLoc << "]: " << app_msg[bitLoc] << std::endl;
			errBits += 1;
		}
	}

	return errBits;
}