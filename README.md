
*A construction of QC-LDPC codes using Integer Linear Programming. It is an exercise mainly attempting to reproduce the work in [1] and [2]*

## Work to be done (latest update: 30 Nov., 2021) -> Cycle Candidates Searching, and MILP based decision of all circulant shift values over nonzero elements of the considered base graph)  
- [x] A BFS-like searching to generate Cycle Generating Tree for each base graph column [1].
- [x] To summarise the Cycle Generating Trees to a) 3-tuple formed tables and b) cycle cnadidate-pairs matrices.
- [x] To create the weighting coefficents matrices, i.e., A_{l} for all l <= g/2 and even. 

## Work to be done (latest update: 1 Dec., 2021) -> Implementation of QC-LDPC decoder based on layered decoding strategy using C++ with openMP+openMPI
- [x] Implementation of node process units, e.g., min, sum and soft-information product.
- [x] Implementation of layered decoding scheduling schmeme.

## Plan of QC-LDPC codes evaluation
- [ ] Evluation of base graph based on the C1 in [3], only Sum-Product decoding is taken into account -> to compare the BERs of girth-4, girth-6, girth-8 and girth-10
- [ ] Evaluation of base graph based on the MacKay[4] and [6]. The (N=504,R=0.5, P=84) and (N=1008, R=0.5, P=84) are targeted.
- [ ] Evaluation of IEEE-802.16e (WiMax) standard code based on the [5]. The code structure is: (N=2304, R=0.5, Z=96) of G=V_a union V_b where |V_a|=12 and |V_b|=24.

## References
[1] Bocharova, Irina E., Boris D. Kudryashov, and Rolf Johannesson. "Searching for binary and nonbinary block and convolutional LDPC codes." IEEE Transactions on Information Theory 62.1 (2015): 163-183.\
[2] Sułek, Wojciech. "Protograph based low-density parity-check codes design with mixed integer linear programming." IEEE Access 7 (2018): 1424-1438. \
[3] Mohr, Philipp, et al. "Coarsely Quantized Layered Decoding Using the Information Bottleneck Method." ICC 2021-IEEE International Conference on Communications. IEEE, 2021.\
[4] D. MacKay, “Online database of low-density parity-check codes,” Available from wol.ra.phy.cam.ac.uk/mackay/codes/data.html\
[5] Zhang, Kai, Xinming Huang, and Zhongfeng Wang. "High-throughput layered decoder implementation for quasi-cyclic LDPC codes." IEEE Journal on Selected Areas in Communications 27.6 (2009): 985-994.\
[6] Bonello, Nicholas, Sheng Chen, and Lajos Hanzo. "Construction of regular quasi-cyclic protograph LDPC codes based on Vandermonde matrices." IEEE transactions on vehicular technology 57.4 (2008): 2583-2588.\

## Misc.
* Folder: trapping-sets-enumeration
	1) Enumerate trapping sets
	2) Convert .alist file into .sparse file
	3) Convert .qc file into sparse file
	4) Convert .sparse file into graph file

	Misc.
		a) .sparse file simply enumerate all nonzero element over the given H matrix in a (x, y) presentation
		b) .qc file is the file we need to analyse the girth and cycle counts using the toolset in the Folder "EMD-Spectrum-LDPC"
