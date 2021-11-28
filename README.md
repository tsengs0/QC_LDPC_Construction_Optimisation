
*A construction of QC-LDPC codes using Integer Linear Programming.*
## Work to be done (latest update: 27 Nov., 2021)
- [x] A BFS-like searching to generate Cycle Generating Tree for each base graph column [1].


## References
[1] Bocharova, Irina E., Boris D. Kudryashov, and Rolf Johannesson. "Searching for binary and nonbinary block and convolutional LDPC codes." IEEE Transactions on Information Theory 62.1 (2015): 163-183.

## Misc.
* Folder: trapping-sets-enumeration
	1) Enumerate trapping sets
	2) Convert .alist file into .sparse file
	3) Convert .qc file into sparse file
	4) Convert .sparse file into graph file

	Misc.
		a) .sparse file simply enumerate all nonzero element over the given H matrix in a (x, y) presentation
		b) .qc file is the file we need to analyse the girth and cycle counts using the toolset in the Folder "EMD-Spectrum-LDPC"
