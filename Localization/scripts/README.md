# Image retrieval

This is a description of the use of the image retrieval script.

The purpose of image retrieval is to find the most similar set of database images with the query image to reduce the matching time. The steps are as follows (you need to configure the Hloc algorithm library in advance, address: [Hloc](https://github.com/cvg/Hierarchical-Localization).

* 1. extract the global descriptors of query image and database image respectively:
```
eg：
python -m hloc.extract_features --image_dir .../(Query Image Path) --export_dir .../(output path) --conf netvlad
python -m hloc.extract_features --image_dir .../(Database Image Path) --export_dir .../(output path)  --conf netvlad
```

* 2. Retrieval:
```
eg:
Cambridge - shopFacade
python3 -m hloc.pairs_from_retrieval --descriptors .../query-global-feats-netvlad.h5 --output .../top-5.txt --num_matched 5 
                                    --query_list .../query_imgfile.txt --db_list .../train_imgfile.txt --db_descriptors .../db-global-feats-netvlad.h5
```
