#include "postgres.h"

#include <math.h>

#include "catalog/pg_type.h"
#include "common/shortest_dec.h"
#include "fmgr.h"
#include "executor/spi.h"
#include "lib/stringinfo.h"
#include "libpq/pqformat.h"
#include "port.h"				/* for strtof() */
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/float.h"
#include "utils/lsyscache.h"
#include "utils/numeric.h"
#include "vector.h"
#include "commands/defrem.h"
#include "access/htup_details.h"
#include "utils/elog.h"
#include "funcapi.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif


static inline bool
vector_isspace(char ch)
{
	if (ch == ' ' ||
		ch == '\t' ||
		ch == '\n' ||
		ch == '\r' ||
		ch == '\v' ||
		ch == '\f')
		return true;
	return false;
}

Vector * 
InitVector(int dim)
{
	Vector *result;
	int	size;

	size = VECTOR_SIZE(dim);
	result = (Vector *) palloc0(size);
	SET_VARSIZE(result, size);
	result->dim = dim;

	return result;
}


PG_FUNCTION_INFO_V1(vector_in);
Datum
vector_in(PG_FUNCTION_ARGS)
{
	char *lit = PG_GETARG_CSTRING(0);
    int32	typmod = PG_GETARG_INT32(2);
	float x[1024];
	char *pt = lit;
    int dim = 0;
	Vector *result;

	while (vector_isspace(*pt))
		pt++;

	if (*pt != '[')
        ereport(ERROR, (errmsg("Vector contents must start with \"[\".")));

	pt++;

	while (vector_isspace(*pt))
		pt++;

	if (*pt == ']')
		ereport(ERROR, (errmsg("vector must have at least 1 dimension")));

	for (;;)
	{
		float val = 0.0;
		char *stringEnd;

		while (vector_isspace(*pt))
			pt++;
        // ereport(WARNING, (errmsg("%c", *pt)));
		val = strtof(pt, &stringEnd);
        // ereport(WARNING, (errmsg("%f", val)));
		x[dim++] = val;

		pt = stringEnd;

		while (vector_isspace(*pt))
			pt++;

		if (*pt == ',') {
            pt++;
        } else if (*pt == ']') {
			pt++;
			break;
		} else {
            ereport(ERROR, (errmsg("invalid input syntax for type vector: \"%s\"", lit)));
        }
	}
    if (typmod != -1 && dim != typmod) {
        ereport(ERROR, (errmsg("invalid input dimension, %d and %d", typmod, dim)));
    }
	result = InitVector(dim);
	for (int i = 0; i < dim; i++) {
		result->x[i] = x[i];
        // ereport(WARNING, (errmsg("%f", result->x[i])));
    }
    // ereport(WARNING, (errmsg("%d", result->dim)));
	PG_RETURN_POINTER(result);
}

#define DatumGetVector(x)		((Vector *) PG_DETOAST_DATUM(x))
#define PG_GETARG_VECTOR_P(x)	DatumGetVector(PG_GETARG_DATUM(x))

PG_FUNCTION_INFO_V1(vector_out);
Datum
vector_out(PG_FUNCTION_ARGS)
{
    Vector *vector = PG_GETARG_VECTOR_P(0);
    StringInfoData buf;

    initStringInfo(&buf);
    appendStringInfoChar(&buf, '[');
    for (int i = 0; i < vector->dim; i++) {
        if (i > 0) appendStringInfoChar(&buf, ',');
        appendStringInfo(&buf, "%f", vector->x[i]);
    }
    appendStringInfoChar(&buf, ']');

    PG_RETURN_CSTRING(buf.data);
}


PG_FUNCTION_INFO_V1(vector_l2_distance);
Datum
vector_l2_distance(PG_FUNCTION_ARGS)
{
    Vector *v1 = (Vector *) PG_GETARG_VECTOR_P(0);
    Vector *v2 = (Vector *) PG_GETARG_VECTOR_P(1);
    float sum = 0.0;

    if (v1->dim != v2->dim) {
        ereport(ERROR, (errmsg("vectors must be the same dimension")));
    }
    for (int i = 0; i < v1->dim; i++) {
        float diff = v1->x[i] - v2->x[i];
        sum += diff * diff;
    }
    PG_RETURN_FLOAT4(sqrt(sum));
}


PG_FUNCTION_INFO_V1(vector_typmod_in);
Datum
vector_typmod_in(PG_FUNCTION_ARGS)
{
	ArrayType  *ta = PG_GETARG_ARRAYTYPE_P(0);
	int32	   *tl;
	int			n;

	tl = ArrayGetIntegerTypmods(ta, &n);
    PG_RETURN_INT32(*tl);
}



PG_FUNCTION_INFO_V1(vector_nearest_neighbor);
Datum
vector_nearest_neighbor(PG_FUNCTION_ARGS)
{
    Vector *input_vec = (Vector *) PG_GETARG_VECTOR_P(0);
    text *table_name_text = PG_GETARG_TEXT_PP(1);
    text *column_name_text = PG_GETARG_TEXT_PP(2);
    int limit = PG_GETARG_INT32(3);
	// ereport(WARNING, (errmsg("enter success")));
    Datum values[2];  // 用于存储 id 和 vector 的返回值
    bool nulls[2] = {false, false};  // 标记 id 和 vector 的空值状态
    TupleDesc tupdesc;  // 用于返回的复合类型
    // Oid *argtypes;
    // Datum *values_input;
    SPIPlanPtr plan;
    int ret;
    HeapTuple rettuple;

    // 初始化并连接 SPI
    StringInfoData query;
    initStringInfo(&query);
    appendStringInfo(&query,
                     "SELECT id, %s FROM %s ORDER BY %s <-> $1 LIMIT 1",
                     text_to_cstring(column_name_text),
                     text_to_cstring(table_name_text),
                     text_to_cstring(column_name_text));
	// ereport(WARNING, (errmsg("construct query success")));
    // 执行 SPI 查询
    if (SPI_connect() != SPI_OK_CONNECT)
        ereport(ERROR, (errmsg("SPI_connect failed")));

    Oid argtypes[1] = {get_fn_expr_argtype(fcinfo->flinfo, 0)};
    Datum values_input[1] = {PointerGetDatum(input_vec)};
    plan = SPI_prepare(query.data, 1, argtypes);

    if (plan == NULL)
        ereport(ERROR, (errmsg("SPI_prepare failed")));

    ret = SPI_execute_plan(plan, values_input, NULL, true, 1);  // 只返回 1 行
    if (ret != SPI_OK_SELECT)
        ereport(ERROR, (errmsg("SPI_execute_plan failed: %s", SPI_result_code_string(SPI_result))));
	// ereport(WARNING, (errmsg("excute query success")));
    // 检查是否有结果
    if (SPI_processed > 0) {
        SPITupleTable *tuptable = SPI_tuptable;
        TupleDesc spi_tupdesc = tuptable->tupdesc;
        HeapTuple tuple = tuptable->vals[0];
        bool isnull;

        // 获取 id 和 vector 列的值
        values[0] = SPI_getbinval(tuple, spi_tupdesc, 1, &isnull);  // 获取 id
        if (isnull) nulls[0] = true;  // 检查 id 是否为 NULL
		// ereport(WARNING, (errmsg("excute query success %d", nulls[0])));
        values[1] = SPI_getbinval(tuple, spi_tupdesc, 2, &isnull);  // 获取 vector
        if (isnull) nulls[1] = true;  // 检查 vector 是否为 NULL
		// ereport(WARNING, (errmsg("excute query success %d", nulls[1])));
    } else {
        SPI_finish();
        PG_RETURN_NULL();  // 如果没有找到最近邻，返回 NULL
    }

    // // 获取返回类型的 TupleDesc
    if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
        ereport(ERROR, (errmsg("Failed to retrieve tuple descriptor")));
	// ereport(WARNING, (errmsg("excute query success")));
    // 创建返回的复合类型元组
    rettuple = heap_form_tuple(tupdesc, values, nulls);
	// ereport(WARNING, (errmsg("excute HeapTuple success")));
    // 结束 SPI 会话
    SPI_finish();
	// ereport(WARNING, (errmsg("excute finish success")));
    // 返回复合类型元组
    PG_RETURN_DATUM(HeapTupleGetDatum(rettuple));
}



PG_FUNCTION_INFO_V1(vector_k_nearest_neighbor);
Datum
vector_k_nearest_neighbor(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;
    MemoryContext oldcontext;

    if (SRF_IS_FIRSTCALL())
    {
        // 初始化 FuncCallContext
        funcctx = SRF_FIRSTCALL_INIT();

        // 创建多行返回的内存上下文
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // 初始化 SPI
        if (SPI_connect() != SPI_OK_CONNECT)
            ereport(ERROR, (errmsg("SPI_connect failed")));

        // 构建查询
        Vector *input_vec = (Vector *) PG_GETARG_POINTER(0);  // 输入向量
        text *table_name_text = PG_GETARG_TEXT_PP(1);         // 表名
        text *column_name_text = PG_GETARG_TEXT_PP(2);        // 列名
        int limit = PG_GETARG_INT32(3);                       // 返回的 K 近邻数量

        StringInfoData query;
        initStringInfo(&query);
        appendStringInfo(&query,
                         "SELECT id, %s FROM %s ORDER BY %s <-> $1 LIMIT %d",
                         text_to_cstring(column_name_text),
                         text_to_cstring(table_name_text),
                         text_to_cstring(column_name_text),
                         limit);

        // 准备 SPI 查询
        Oid argtypes[1] = {get_fn_expr_argtype(fcinfo->flinfo, 0)};
        Datum values_input[1] = {PointerGetDatum(input_vec)};
        SPIPlanPtr plan = SPI_prepare(query.data, 1, argtypes);
        if (plan == NULL)
            ereport(ERROR, (errmsg("SPI_prepare failed")));

        // 执行 SPI 查询
        int ret = SPI_execute_plan(plan, values_input, NULL, true, limit);
        if (ret != SPI_OK_SELECT)
            ereport(ERROR, (errmsg("SPI_execute_plan failed: %s", SPI_result_code_string(SPI_result))));

        // 保存查询结果
        funcctx->max_calls = SPI_processed;
        funcctx->user_fctx = SPI_tuptable;

        MemoryContextSwitchTo(oldcontext);
    }

    // 从 FuncCallContext 中提取上下文
    funcctx = SRF_PERCALL_SETUP();

    // 检查是否还有结果
    if (funcctx->call_cntr < funcctx->max_calls)
    {
        SPITupleTable *tuptable = (SPITupleTable *) funcctx->user_fctx;
        TupleDesc tupdesc = tuptable->tupdesc;
        HeapTuple tuple = tuptable->vals[funcctx->call_cntr];

        Datum values[2];  // 用于存储 id 和 vector
        bool nulls[2] = {false, false};
        bool isnull;

        // 获取 id 和 vector 的值
        values[0] = SPI_getbinval(tuple, tupdesc, 1, &isnull);  // id 列
        if (isnull) nulls[0] = true;
        values[1] = SPI_getbinval(tuple, tupdesc, 2, &isnull);  // vector 列
        if (isnull) nulls[1] = true;

        // 获取返回类型的 TupleDesc
        TupleDesc rettupdesc;
        if (get_call_result_type(fcinfo, NULL, &rettupdesc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR, (errmsg("Failed to retrieve tuple descriptor")));

        // 创建返回的复合类型元组
        HeapTuple rettuple = heap_form_tuple(rettupdesc, values, nulls);

        // 返回单行
        SRF_RETURN_NEXT(funcctx, HeapTupleGetDatum(rettuple));
    }
    else
    {
        // 完成
        SPI_finish();
        SRF_RETURN_DONE(funcctx);
    }
}