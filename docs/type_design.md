# Type Desgin

Currently, it support one type of vector, which is `Vector`.

## Vector

### Structure

Source code `src/vector.h`.

The structure for `Vector` is:

```
typedef struct Vector
{
	int32 vl_len_;
	int	dim;
	float x[FLEXIBLE_ARRAY_MEMBER];
} Vector;

```

About `vl_len_`: 

The first `int32` is a 4-byte header, which is used by PostgreSQL to indicate the size of the header of a variable-length data type. 
Its is mainly used to store the length information of the variable-length data type.

About `dim`:

It is used to represents the dimension of the vector data.

About `x`:

It is a variable-length array specified by postgres. It is use to store the float type element in the vector.

### Construction Format

Source code: `PG_FUNCTION_INFO_V1(vector_in);` in `src/vector.c`.

It take a string type input parameter, and construct a vector data as result.

The input format is a string representing a vector, enclosed in square brackets, with the elements of the vector (float type number) separated by commas. 

For example:

```
'[1.0, 2.0, 3.0, 4.0]'
```
