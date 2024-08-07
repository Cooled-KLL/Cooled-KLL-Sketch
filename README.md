# Cooled-KLL: EnhancingQuantile Estimation by Filtering Hot Item

## Compilation and Execution

### Compilation

To compile the code, use the following command:

```sh
g++ main.cpp -o main -O2 -std=c++11 -g
```

### Execution

To run the compiled program, use:

```sh
./main
```

## Output

The results are stored in various CSV files:

- `aqe.csv` and `are.csv`: These files store the Average Quantile Error (AQE) and Average Rank Error (ARE) for different algorithms under varying memory constraints.
- `insert_speed.csv`: This file logs the insertion speed of each algorithm.
- `query_aqe_time.csv` and `query_are_time.csv`: These files document the speed of quantile and rank queries, respectively.
- `aqe_distribution.csv` and `are_distribution.csv`: These files contain the distribution of errors for quantile and rank queries concerning their respective quantiles.

The current dataset's distribution information is found in `distribution.txt`. To visualize the data distribution, run:

```sh
python draw_distribution.py
```

## Configuration

### Switching Datasets

To switch to a different dataset, modify the `define` directive in `common/config.h`. For instance, to switch to the 'ecom' dataset, use:

```cpp
#define USE_KAGGLE_ECOM
```

### Disabling *Binary Split Insertion* Optimization

To test the algorithm's performance without *Binary Split Insertion* optimization, comment out the following line in `common/config.h`:

```cpp
// #define USE_MULTI_INSERT
```

### Adjusting Experimental Parameters

To obtain more stable experimental results, you can modify the `QUERYTESTNUMBER` parameter in `main.cpp` to change the number of repeated queries. The `TEST_NUMBER` parameter can be altered to change the number of experimental rounds. The final results are averaged over multiple rounds and repeated queries to enhance accuracy.

```cpp
#define QUERYTESTNUMBER <desired_value>
#define TEST_NUMBER <desired_value>
```