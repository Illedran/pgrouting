/*PGR-GNU*****************************************************************
File: withPoints_ksp.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer: 
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: 

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

********************************************************************PGR-GNU*/

#include "postgres.h"
#include "executor/spi.h"
#include "funcapi.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#if PGSQL_VERSION > 92
#include "access/htup_details.h"
#endif


#include "fmgr.h"
#include "./../../common/src/pgr_types.h"
#include "./../../common/src/postgres_connection.h"
#include "./../../common/src/edges_input.h"
#include "./../../common/src/points_input.h"
#include "./../../withPoints/src/get_new_queries.h"
#include "./withPoints_ksp_driver.h"
#include "./../../common/src/debug_macro.h"

PG_FUNCTION_INFO_V1(withPoints_ksp);
#ifndef _MSC_VER
Datum
#else  // _MSC_VER
PGDLLEXPORT Datum
#endif
withPoints_ksp(PG_FUNCTION_ARGS);


/*******************************************************************************/
/*                          MODIFY AS NEEDED                                   */
static
void
process(
        char* edges_sql,
        char* points_sql,
        int64_t start_pid,
        int64_t end_pid,
        int64_t k,

        bool directed,
        bool heap_paths,
        char *driving_side,
        bool details,

        General_path_element_t **result_tuples,
        size_t *result_count) {

    driving_side[0] = tolower(driving_side[0]);
    PGR_DBG("driving side:%c",driving_side[0]);
    if (! ((driving_side[0] == 'r')
                || (driving_side[0] == 'l'))) {
        driving_side[0] = 'b'; 
    }

    pgr_SPI_connect();

    Point_on_edge_t *points = NULL;
    int64_t total_points = 0;
    pgr_get_points(points_sql, &points, &total_points);

    char *edges_of_points_query = NULL;
    char *edges_no_points_query = NULL;
    get_new_queries(
            edges_sql, points_sql,
            &edges_of_points_query,
            &edges_no_points_query);


    pgr_edge_t *edges_of_points = NULL;
    int64_t total_edges_of_points = 0;
    pgr_get_data_5_columns(edges_of_points_query, &edges_of_points, &total_edges_of_points);

    pgr_edge_t *edges = NULL;
    int64_t total_edges = 0;
    pgr_get_data_5_columns(edges_no_points_query, &edges, &total_edges);

    PGR_DBG("freeing allocated memory not used anymore");
    free(edges_of_points_query);
    free(edges_no_points_query);

    if ((total_edges + total_edges_of_points) == 0) {
        PGR_DBG("No edges found");
        (*result_count) = 0;
        (*result_tuples) = NULL;
        pgr_SPI_finish();
        return;
    }

    PGR_DBG("Starting processing");
    char *err_msg = NULL;
    int errcode = do_pgr_withPointsKsp(
            edges,
            total_edges,
            points,
            total_points,
            edges_of_points,
            total_edges_of_points,
            start_pid,
            end_pid,
            k,

            directed,
            heap_paths,
            driving_side[0],
            details,

            result_tuples,
            result_count,
            &err_msg);
    PGR_DBG("Returned message = %s\n", err_msg);

    if (!err_msg) free(err_msg);

    pfree(edges);
    pfree(edges_of_points);
    pfree(points);

    pgr_SPI_finish();

    if (errcode) {
        pgr_send_error(errcode);
    }

}

/*                                                                             */
/*******************************************************************************/



#ifndef _MSC_VER
Datum
#else  // _MSC_VER
PGDLLEXPORT Datum
#endif
withPoints_ksp(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    size_t              call_cntr;
    size_t               max_calls;
    TupleDesc            tuple_desc;

    /*******************************************************************************/
    /*                          MODIFY AS NEEDED                                   */
    /*                                                                             */
    General_path_element_t  *result_tuples = 0;
    size_t result_count = 0;
    /*                                                                             */
    /*******************************************************************************/

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);


        /*******************************************************************************/
        /*                          MODIFY AS NEEDED                                   */
        // CREATE OR REPLACE FUNCTION pgr_withPoint(
        // edges_sql TEXT,
        // points_sql TEXT,
        // start_pid BIGINT,
        // end_pid BIGINT,
        // k BIGINT,
        //
        // directed BOOLEAN -- DEFAULT true,
        // heap_paths BOOLEAN -- DEFAULT false,
        // driving_side CHAR -- DEFAULT 'b',
        // details BOOLEAN -- DEFAULT false,

        PGR_DBG("Calling process");
        PGR_DBG("initial driving side:%s", pgr_text2char(PG_GETARG_TEXT_P(7)));
        process(
                pgr_text2char(PG_GETARG_TEXT_P(0)),
                pgr_text2char(PG_GETARG_TEXT_P(1)),
                PG_GETARG_INT64(2),
                PG_GETARG_INT64(3),
                PG_GETARG_INT64(4),
                PG_GETARG_BOOL(5),
                PG_GETARG_BOOL(6),
                pgr_text2char(PG_GETARG_TEXT_P(7)),
                PG_GETARG_BOOL(8),
                &result_tuples,
                &result_count);

        /*                                                                             */
        /*******************************************************************************/

        funcctx->max_calls = result_count;
        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (General_path_element_t*) funcctx->user_fctx;

    if (call_cntr < max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        char*        nulls;

        /*******************************************************************************/
        /*                          MODIFY AS NEEDED                                   */
        // OUT seq BIGINT,
        // OUT path_seq,
        // OUT node BIGINT,
        // OUT edge BIGINT,
        // OUT cost FLOAT,
        // OUT agg_cost FLOAT)


        values = palloc(6 * sizeof(Datum));
        nulls = palloc(6 * sizeof(char));

        size_t i;
        for(i = 0; i < 6; ++i) {
            nulls[i] = ' ';
        }

        /*
           OUT seq INTEGER, OUT path_id INTEGER, OUT path_seq INTEGER,
           OUT node BIGINT, OUT edge BIGINT,
           OUT cost FLOAT, OUT agg_cost FLOAT)
        */


        // postgres starts counting from 1
        values[0] = Int32GetDatum(call_cntr + 1);
        values[1] = Int32GetDatum(result_tuples[call_cntr].start_id + 1);
        values[2] = Int32GetDatum(result_tuples[call_cntr].seq);
        values[3] = Int64GetDatum(result_tuples[call_cntr].node);
        values[4] = Int64GetDatum(result_tuples[call_cntr].edge);
        values[5] = Float8GetDatum(result_tuples[call_cntr].cost);
        values[6] = Float8GetDatum(result_tuples[call_cntr].agg_cost);
        /*******************************************************************************/

        tuple =heap_formtuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        // cleanup
        if (result_tuples) free(result_tuples);

        SRF_RETURN_DONE(funcctx);
    }
}

