# Vector_extension

This is a postgres extension project for supporting vector search in postgres database.

Currently, it support a basic `vector` data type with different dimention and float element type. 
Base on this vector data type, we can support `l2_distance` distance function to calculate l2 distance between two vector data point.
And we also support a very basic knn search method that can get k nearest neighbors of a given vector in a specific column. 

## Directory

```
vector_extension
|-- src                             # source code dir
|   |-- vector.h                    # defination of vector data type
|   |-- vector.c                    # defination of related function
|   |-- vector_extension--1.0.sql   # register data type and function into postgres
|   |-- vector_extension.control    # description file for extension installation
|   |-- Makefile              
|-- README.md      
```

## Environment

This project is runing on LINUX system.

To install this extension, we first need install several packages to support postgres extension development (Assume that postgres-sql-16 is already installed).

```
sudo apt-get install postgresql-server-dev-16
```

## Installation

use `make install` to automatically install the extension and register related function into the portgres.

```
cd src
make clean
make
sudo make install
```

## Run 

### Create Extension

After connecting to the postgres database, we should create extension in postgres:

```
CREATE EXTENSION vector_extension;
```

### Creata Table and Insert Data

We can create table with our defined data type `vector`, the dimension can be changed:

```
CREATE TABLE vectors (id serial PRIMARY KEY, vec vector(5));
```

We can insert data points into the created table:

```
INSERT INTO vectors (vec) VALUES
    ('[1.0, 2.0, 3.0, 4.0, 5.0]'),
    ('[6.0, 7.0, 8.0, 9.0, 10.0]'),
    ('[11.0, 12.0, 13.0, 14.0, 15.0]');
```

We can check the results by:

```
SELECT * FROM vectors;
```

The expected result is:

```
 id |                         vec                         
----+-----------------------------------------------------
  1 | [1.000000,2.000000,3.000000,4.000000,5.000000]
  2 | [6.000000,7.000000,8.000000,9.000000,10.000000]
  3 | [11.000000,12.000000,13.000000,14.000000,15.000000]
```

### L2 distance calculation and Knn search

We can use defined operator `<->` to calculate l2 distance between two vectors:

```
SELECT '[6.0, 7.0, 8.0, 9.0, 10.0]'::vector <-> '[1.0, 2.0, 3.0, 4.0, 5.0]'::vector;
```

The expected result is:

```
 ?column? 
----------
 11.18034
```

We can use `vector_k_nearest_neighbor` function to find top-k nearest vectors:

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

### Uninstall

```
DROP TABLE vectors;
DROP EXTENSION IF EXISTS vector_extension;
```