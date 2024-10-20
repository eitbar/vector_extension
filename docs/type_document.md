# User Defined vector type

## Basic structure

```
typedef struct Vector
{
	int32 vl_len_;
	int	dim;
	float x[FLEXIBLE_ARRAY_MEMBER];
} Vector;

```

usage example:

```
CREATE TABLE vectors (
    id serial PRIMARY KEY, 
    vec vector(5));
```

## Input Format

The input format is a string representing a vector, enclosed in square brackets, with the elements of the vector (float type number) separated by commas. 

usage exmaple:

```
INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');
```

or

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector;
```

## Basic Operator

### Comparison Operators

Equal Operator: `=`

usage:

```
SELECT * FROM vectors WHERE vec = '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector;
```

Not Equal Operator: `<>`

usage:

```
SELECT * FROM vectors WHERE vec <> '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector;
```

### Calculation Operators

Add Operator: `+`

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector + '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;
```

Subtraction Operator: `-`

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector - '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;
```

Dot product Operator: `*`

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector * '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;

SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector * 5;
```

Division Operator: `/`

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector / 5;
```


### Vector Operators

Dimension Operator: `dim`

usage:

```
SELECT id, dim(vec) from vectors;
```

Norm Operator: `norm`

usage:

```
SELECT id, vec, norm(vec) from vectors;
```

### Distance Operators

L2distance Operator: `<->`

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector <-> '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;

SELECT id, vec FROM vectors ORDER BY vec <-> '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector LIMIT 3;
```

Cosin Distance Operator: `<#>` 

usage:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector <#> '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;

SELECT id, vec FROM vectors ORDER BY vec <#> '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector DESC LIMIT 3;
```

we can also implement the cosin distance by the basic operators:

```
SELECT id, vec FROM vectors ORDER BY vec * '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector / norm(vec) / norm('[6.0, 7.0, 8.0, 9.0, 10.0]'::vector) DESC LIMIT 3;
```

## Index Support

Todo ...