# Function Desgin

Currently, it support one type of distance functions, `L2 distance` and one type of k nearest neighbors search.

## Distance Function

### L2 distance

Source code position: `PG_FUNCTION_INFO_V1(vector_l2_distance);` in `src/vector.c`.

It take two vector type input parameters, and return a float type number that represents the distance between the input vector pair.

$$

d_{l2}(\mathbf{A}, \mathbf{B}) = \sqrt {\sum_{i=1}^{n} (\mathbf{A}_i - \mathbf{B}_i)^2}

$$

The calculation operator is represent as `<->`, for example:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector <-> '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;
```

The expected result is:

```
 11.18034
```

## Search Funcion

### K nearest neighbors search

Source code position: `PG_FUNCTION_INFO_V1(vector_nearest_neighbor);` in `src/vector.c`.

It take four input parameters, including `vector` with data type Vector, `table_name` with data type text, `column_name` with data type text and `k` with data type int.

It will search the `k` nearest neighbors with `l2 distance` on `table_name.column_name`. 

The results will be shown as a table with two column `id` and `vector`, which represent the original id and vector in the original table.

For example:

```
SELECT * FROM vector_k_nearest_neighbor('[6.0, 7.0, 7.0, 9.0, 10.0]', 'vectors', 'vec', 2);
```

The expected result is:

```
 id |                     vector                      
----+-------------------------------------------------
  2 | [6.000000,7.000000,8.000000,9.000000,10.000000]
  1 | [1.000000,2.000000,3.000000,4.000000,5.000000]
```