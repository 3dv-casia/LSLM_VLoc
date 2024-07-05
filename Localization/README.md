# Localization:

This part is the source code of the localization after the completion of the construction of the map, the steps are as follows.

# 1. Query image retrieval.

Find the set of database images most similar to the query image, see *./scripts/README.md* for steps.

# 2. Configuration file path

*main.cpp* in all the paths need to be changed to the path of your own computer, the script to Cambridge-ShopFacade dataset as an example of configuration, can also be changed to other datasets, but only after the completion of the line map construction.

# 3. Start localization

```
cmake ...
make
./Line_localization
```

Note: The localization script is input in limap, you can also use the line map built by Line3Dpp, or you can use our own optimized line map built by Line3Dpp. We will update the optimized Line3Dpp later.

# 4. Evaluation of localization accuracy

In the main program, we have only written to evaluate the localization accuracy of a single image, if you need to evaluate all the images, you can write your own corresponding code, which is very simple.

