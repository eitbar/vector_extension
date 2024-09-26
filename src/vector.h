#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_SIZE(_dim)		(VARHDRSZ + sizeof(int) + sizeof(float)*(_dim))

typedef struct Vector
{
	int32		vl_len_;
	int	dim;
	float x[FLEXIBLE_ARRAY_MEMBER];
} Vector;


#endif