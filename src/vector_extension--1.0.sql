CREATE FUNCTION vector_in(cstring) RETURNS vector AS 'vector_extension', 'vector_in' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION vector_out(vector) RETURNS cstring AS 'vector_extension', 'vector_out' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION vector_typmod_in(cstring[]) RETURNS integer AS 'vector_extension', 'vector_typmod_in' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION vector_nearest_neighbor(vector, text, text) RETURNS neighbor_result AS 'vector_extension', 'vector_nearest_neighbor' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION vector_k_nearest_neighbor(vector, text, text, int) RETURNS SETOF neighbor_result AS 'vector_extension', 'vector_k_nearest_neighbor' LANGUAGE C IMMUTABLE STRICT;


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

CREATE FUNCTION l2_distance(vector, vector) RETURNS float4 AS 'vector_extension', 'vector_l2_distance' LANGUAGE C STRICT;

CREATE OPERATOR <-> (
    LEFTARG = vector,
    RIGHTARG = vector,
    PROCEDURE = l2_distance,
    COMMUTATOR = '<->'
);

-- CREATE FUNCTION nearest_l2_neighbor(vector, text, text) RETURNS vector
--     AS 'vector_extension', 'nearest_l2_neighbor' LANGUAGE C STABLE STRICT;