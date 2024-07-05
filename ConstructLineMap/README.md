# To build your line map

There are three ways to establish a line map, but the difference is that an adaptive line map data reading function needs to be written during localization.

* 1.[Limap](https://github.com/cvg/limap)

```
git clone https://github.com/cvg/limap.git
git submodule update --init --recursive
# Refer to https://pytorch.org/get-started/previous-versions/ to install pytorch compatible with your CUDA
python -m pip install torch==1.12.0 torchvision==0.13.0 
python -m pip install -r requirements.txt
# To install the LIMAP Python package:
python -m pip install -Ive . 
# To double check if the package is successfully installed:
python -c "import limap"

cd limap/runners/
python visualsfm_triangulation.py [-a <image_folder>] [--nvm_file <full_path_to_nvm_file>]

# Visual mapping results:
python visualize_3d_lines.py [-i <full_path_to_obj_file>] --use_robust_ranges
```

* 2.[Line3Dpp](https://github.com/manhofer/Line3Dpp)

```
git clone https://github.com/manhofer/Line3Dpp.git
mkdir build
cd build
cmake ..
make
./runLine3Dpp_vsfm -m <full_path_to_nvm_file> [-i <image_folder>] 
```

* 3.Optimized Line3Dpp
```
At that time, the optimized Line3Dpp code will be released.
```
