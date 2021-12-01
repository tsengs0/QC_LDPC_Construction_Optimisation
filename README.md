
*A construction of QC-LDPC codes using Integer Linear Programming. It is an exercise mainly attempting to reproduce the work in [1] and [2]*

## Work to be done (latest update: 30 Nov., 2021)
- [x] A BFS-like searching to generate Cycle Generating Tree for each base graph column [1].
- [x] To summarise the Cycle Generating Trees to a) 3-tuple formed tables and b) cycle cnadidate-pairs matrices.
- [x] To create the weighting coefficents matrices, i.e., A_{l} for all l <= g/2 and even. 


## References
[1] Bocharova, Irina E., Boris D. Kudryashov, and Rolf Johannesson. "Searching for binary and nonbinary block and convolutional LDPC codes." IEEE Transactions on Information Theory 62.1 (2015): 163-183.\
[2] Sułek, Wojciech. "Protograph based low-density parity-check codes design with mixed integer linear programming." IEEE Access 7 (2018): 1424-1438.

## Misc.
* Folder: trapping-sets-enumeration
	1) Enumerate trapping sets
	2) Convert .alist file into .sparse file
	3) Convert .qc file into sparse file
	4) Convert .sparse file into graph file

	Misc.
		a) .sparse file simply enumerate all nonzero element over the given H matrix in a (x, y) presentation
		b) .qc file is the file we need to analyse the girth and cycle counts using the toolset in the Folder "EMD-Spectrum-LDPC"
