# TimePixAna

VERY detailed instructions to RE-RUN the analysis. The user instructions are below to take directly the calibration

The inputs for all of the below functions can be retrieved from this eos directory:
/eos/atlas/atlascerngroupdisk/penn-ww/TestBeam/TRT

First starting with the data files. The raw text files need to be time sorted in order to select cluster size 1 hits. This is done in fitSPIDR.
> source run.sh # runs the sorting and selecting of size 1 clusters. It then dumps the data to a root file histograms for the energy spectra

Now each energy spectra needs to be fit with a gaussian around the correct peak. Many of the spectra have multiple peaks. For example, the chalk has calcium and copper. The parameters are set by hand.
When running each sample, make sure to change in the input file name, the MAXMEAN, and the CompName to the appropriate sample.
> root -l fitSpectra.C++ # will fit one of the spectra with a gaussian. saves a TTree with the mean and width for each pixel

Now fitting the ToT to convert to energy in keV. Each of the outputs from the last step is loaded, and a fit function of:
[0]*x + [1] - [2]/(x-3.0)

> root -l fitSpectra.C++ # outputs a TTree with fit parameters for each pixel.


START HERE: if you directly want the calibration 

Once the analysis has finished, the output from the last step can be loaded by convertTot to retrieve the conversion from ToT to energy. The energy resolution is also calculated.
The function GetEnergyFromToT takes (x pixel, y pixel, ToT, returns energy, returns resolution)

> root -l convertTot.C++

> convertTot a; // you can spectify the input file name in the first option. so a("/eos/atlas/atlascerngroupdisk/penn-ww/TestBeam/TRT/fout.root")

> double en, reso;

> a.GetEnergyFromToT(10,10, 15.0, en, reso)

The energy and resolution are loaded in en and reso, respectively. fout.root is the input to this last function

The inputs for the calibration function is:
 /eos/atlas/atlascerngroupdisk/penn-ww/TestBeam/TRT/fout.root
