# Function Desgin

Currently, it support one type of distance functions, `L2 distance` and one type of k nearest neighbors search.

## Distance Function

### L2 distance

Source code position: `PG_FUNCTION_INFO_V1(vector_l2_distance);` in `src/vector.c`.

It take two vector type input parameters, and return a float type number that represents the distance between the input vector pair.

$$

d_{l2}(\mathbf{A}, \mathbf{B}) = \sqrt {\sum_{i=1}^{n} (\mathbf{A}_i - \mathbf{B}_i)^2}

$$

## Search Funcion

### K nearest neighbors search

Source code position: `PG_FUNCTION_INFO_V1(vector_nearest_neighbor);` in `src/vector.c`.
