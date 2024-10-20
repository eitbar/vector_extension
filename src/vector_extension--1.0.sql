CREATE FUNCTION vector_in(cstring) RETURNS vector 
AS 'vector_extension', 'vector_in' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_out(vector) RETURNS cstring 
AS 'vector_extension', 'vector_out' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_typmod_in(cstring[]) RETURNS integer 
AS 'vector_extension', 'vector_typmod_in' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_nearest_neighbor(vector, text, text) RETURNS neighbor_result 
AS 'vector_extension', 'vector_nearest_neighbor' 
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_k_nearest_neighbor(vector, text, text, int) RETURNS SETOF neighbor_result 
AS 'vector_extension', 'vector_k_nearest_neighbor' 
LANGUAGE C IMMUTABLE STRICT;



CREATE TYPE vector (
    INTERNALLENGTH = VARIABLE,
    INPUT = vector_in,
    TYPMOD_IN = vector_typmod_in,
    OUTPUT = vector_out,
	STORAGE = external
);

CREATE TYPE neighbor_result AS (
    id int,
    vector vector
);



CREATE FUNCTION vector_eq(vector, vector) RETURNS boolean
AS 'vector_extension', 'vector_eq'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_ne(vector, vector) RETURNS boolean
AS 'vector_extension', 'vector_ne'
LANGUAGE C IMMUTABLE STRICT;


CREATE OPERATOR = (
    LEFTARG = vector,       
    RIGHTARG = vector,        
    PROCEDURE = vector_eq,
    COMMUTATOR = '='     
);

CREATE OPERATOR <> (
    LEFTARG = vector,       
    RIGHTARG = vector,        
    PROCEDURE = vector_ne,
    COMMUTATOR = '<>'     
);

CREATE FUNCTION vector_add_vector(vector, vector) RETURNS vector
AS 'vector_extension', 'vector_add_vector'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_add_number(vector, float4) RETURNS vector
AS 'vector_extension', 'vector_add_number'
LANGUAGE C IMMUTABLE STRICT;


CREATE FUNCTION vector_sub_vector(vector, vector) RETURNS vector
AS 'vector_extension', 'vector_sub_vector'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_sub_number(vector, float4) RETURNS vector
AS 'vector_extension', 'vector_sub_number'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR + (
    LEFTARG = vector,         -- The left-hand operand is a vector
    RIGHTARG = float4,        -- The right-hand operand is a scalar (float4)
    PROCEDURE = vector_add_number, -- Calls the 'vector_add_scalar' function to add scalar to a vector
    COMMUTATOR = '+'         -- The commutator is '+', meaning the order of operands can be reversed
);


CREATE OPERATOR + (
    LEFTARG = vector,       
    RIGHTARG = vector,        
    PROCEDURE = vector_add_vector,
    COMMUTATOR = '+'     
);

CREATE OPERATOR - (
    LEFTARG = vector, 
    RIGHTARG = float4,        
    PROCEDURE = vector_sub_number
);


CREATE OPERATOR - (
    LEFTARG = vector,       
    RIGHTARG = vector,        
    PROCEDURE = vector_sub_vector   
);


CREATE FUNCTION vector_dotproduct_vector(vector, vector) RETURNS float4
AS 'vector_extension', 'vector_dotproduct_vector'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_dot_number(vector, float4) RETURNS vector
AS 'vector_extension', 'vector_dot_number'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION vector_div_number(vector, float4) RETURNS vector
AS 'vector_extension', 'vector_div_number'
LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR * (
    LEFTARG = vector,         -- The left-hand operand is a vector
    RIGHTARG = float4,        -- The right-hand operand is a scalar (float4)
    PROCEDURE = vector_dot_number, -- Calls the 'vector_add_scalar' function to add scalar to a vector
    COMMUTATOR = '*'         -- The commutator is '+', meaning the order of operands can be reversed
);

CREATE OPERATOR * (
    LEFTARG = vector,       
    RIGHTARG = vector,        
    PROCEDURE = vector_dotproduct_vector,
    COMMUTATOR = '*'     
);

CREATE OPERATOR / (
    LEFTARG = vector,         
    RIGHTARG = float4,       
    PROCEDURE = vector_div_number
);


CREATE FUNCTION dim(vector) RETURNS int 
AS 'vector_extension', 'dim' 
LANGUAGE C STRICT;

CREATE FUNCTION norm(vector) RETURNS float4 
AS 'vector_extension', 'norm' 
LANGUAGE C STRICT;



CREATE FUNCTION l2_distance(vector, vector) RETURNS float4 
AS 'vector_extension', 'vector_l2_distance' 
LANGUAGE C STRICT;

CREATE FUNCTION cos_distance(vector, vector) RETURNS float4 
AS 'vector_extension', 'cos_distance' 
LANGUAGE C STRICT;

CREATE OPERATOR <-> (
    LEFTARG = vector,
    RIGHTARG = vector,
    PROCEDURE = l2_distance,
    COMMUTATOR = '<->'
);

CREATE OPERATOR <#> (
    LEFTARG = vector,
    RIGHTARG = vector,
    PROCEDURE = cos_distance,
    COMMUTATOR = '<#>'
);
