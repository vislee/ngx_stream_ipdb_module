/*
 * Copyright (C) vislee
 */

#ifndef _NGX_STREAM_IPDB_MODULE
#define _NGX_STREAM_IPDB_MODULE

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>

#include "ipdb/ipdb.h"


typedef struct {
    ipdb_reader    *ipdb;
} ngx_stream_ipdb_main_conf_t;


typedef struct {
    ngx_str_t       lang;
} ngx_stream_ipdb_srv_conf_t;



ngx_int_t ngx_stream_ipdb_item_by_addr(ipdb_reader *reader, ngx_addr_t *addr,
    const char *lang, char *body);

#endif
