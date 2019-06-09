/*
 * Copyright (C) vislee
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>

#include "ipdb/ipdb.h"

#define NGX_IPDB_country_name    0
#define NGX_IPDB_region_name     1
#define NGX_IPDB_city_name       2
#define NGX_IPDB_owner_domain    3
#define NGX_IPDB_isp_domain      4
#define NGX_IPDB_latitude        5
#define NGX_IPDB_longitude       6
// all item
#define NGX_IPDB_raw             20


typedef struct {
    ipdb_reader    *ipdb;
} ngx_stream_ipdb_main_conf_t;


typedef struct {
    ngx_str_t       lang;
} ngx_stream_ipdb_srv_conf_t;


static ngx_int_t ngx_stream_ipdb_variable(ngx_stream_session_t *s,
    ngx_stream_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_stream_ipdb_add_variables(ngx_conf_t *cf);
static char *ngx_stream_ipdb_language(ngx_conf_t *cf, void *post, void *data);
static void *ngx_stream_ipdb_create_main_conf(ngx_conf_t *cf);
static void *ngx_stream_ipdb_create_srv_conf(ngx_conf_t *cf);
static char *ngx_stream_ipdb_merge_srv_conf(ngx_conf_t *cf, void *parent,
    void *child);
static void ngx_stream_ipdb_cleanup(void *data);
static char *ngx_stream_ipdb_open(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_conf_post_handler_pt  ngx_stream_ipdb_language_p =
    ngx_stream_ipdb_language;


static ngx_command_t  ngx_stream_ipdb_commands[] = {

    { ngx_string("ipdb"),
      NGX_STREAM_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_stream_ipdb_open,
      NGX_STREAM_MAIN_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("ipdb_language"),
      NGX_STREAM_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_STREAM_SRV_CONF_OFFSET,
      offsetof(ngx_stream_ipdb_srv_conf_t, lang),
      &ngx_stream_ipdb_language_p },

      ngx_null_command
};


static ngx_stream_module_t  ngx_stream_ipdb_module_ctx = {
    ngx_stream_ipdb_add_variables,        /* preconfiguration */
    NULL,                                 /* postconfiguration */

    ngx_stream_ipdb_create_main_conf,     /* create main configuration */
    NULL,                                 /* init main configuration */

    ngx_stream_ipdb_create_srv_conf,      /* create server configuration */
    ngx_stream_ipdb_merge_srv_conf        /* merge server configuration */
};


ngx_module_t  ngx_stream_ipdb_module = {
    NGX_MODULE_V1,
    &ngx_stream_ipdb_module_ctx,          /* module context */
    ngx_stream_ipdb_commands,             /* module directives */
    NGX_STREAM_MODULE,                    /* module type */
    NULL,                                 /* init master */
    NULL,                                 /* init module */
    NULL,                                 /* init process */
    NULL,                                 /* init thread */
    NULL,                                 /* exit thread */
    NULL,                                 /* exit process */
    NULL,                                 /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_stream_variable_t  ngx_stream_ipdb_vars[] = {
    { ngx_string("ipdb_country_name"), NULL,
      ngx_stream_ipdb_variable,
      NGX_IPDB_country_name, 0, 0 },

    { ngx_string("ipdb_region_name"), NULL,
      ngx_stream_ipdb_variable,
      NGX_IPDB_region_name, 0, 0 },

    { ngx_string("ipdb_city_name"), NULL,
      ngx_stream_ipdb_variable,
      NGX_IPDB_city_name, 0, 0 },

    { ngx_string("ipdb_isp_domain"), NULL,
      ngx_stream_ipdb_variable,
      NGX_IPDB_isp_domain, 0, 0 },

    { ngx_string("ipdb_raw"), NULL,
      ngx_stream_ipdb_variable,
      NGX_IPDB_raw, 0, 0 },

    ngx_stream_null_variable
};


static ngx_int_t
ngx_stream_ipdb_add_variables(ngx_conf_t *cf)
{
    ngx_stream_variable_t  *var, *v;

    for (v = ngx_stream_ipdb_vars; v->name.len; v++) {
        var = ngx_stream_add_variable(cf, &v->name, v->flags);
        if (var == NULL) {
            return NGX_ERROR;
        }

        var->get_handler = v->get_handler;
        var->data = v->data;
    }

    return NGX_OK;
}

static void *
ngx_stream_ipdb_create_main_conf(ngx_conf_t *cf)
{
    ngx_pool_cleanup_t           *cln;
    ngx_stream_ipdb_main_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_ipdb_main_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    cln = ngx_pool_cleanup_add(cf->pool, 0);
    if (cln == NULL) {
        return NULL;
    }

    cln->handler = ngx_stream_ipdb_cleanup;
    cln->data = conf;

    return conf;
}


static void *
ngx_stream_ipdb_create_srv_conf(ngx_conf_t *cf)
{
    ngx_stream_ipdb_srv_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_stream_ipdb_srv_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}


static char *
ngx_stream_ipdb_merge_srv_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_stream_ipdb_srv_conf_t *prev = parent;
    ngx_stream_ipdb_srv_conf_t *conf = child;

    ngx_conf_merge_str_value(conf->lang, prev->lang, "EN");

    return NGX_CONF_OK;
}


static char *
ngx_stream_ipdb_open(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_int_t                     err;
    ngx_stream_ipdb_main_conf_t  *imcf = conf;

    ngx_str_t  *value;

    if (imcf->ipdb) {
        return "is duplicate";
    }

    value = cf->args->elts;

    err = ipdb_reader_new((char *) value[1].data, &imcf->ipdb);

    if (err || imcf->ipdb == NULL) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "ipdb_reader_new(\"%V\") failed", &value[1]);

        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;
}


static char *
ngx_stream_ipdb_language(ngx_conf_t *cf, void *post, void *data)
{
    ngx_str_t  *lang = data;

    if (ngx_strcmp(lang->data, "EN") == 0
        || ngx_strcmp(lang->data, "CN") == 0) {

        return NGX_CONF_OK;
    }

    return NGX_CONF_ERROR;
}


static char *
ngx_stream_ipdb_get_index_item(char *v, ngx_int_t idx)
{
    char *p, *q;
    ngx_int_t n = 0;

    if (v == NULL) return NULL;

    if (idx >= NGX_IPDB_raw) return v;

    p = v;
    q = p;

    while (*p) {

        if (*p == '\t') {

            if (idx == n) {
                *p = 0;
                return q;
            }

            q = p + 1;
            ++n;
        }

        ++p;
    }

    if (*p == 0 && idx == n) {
        return q;
    }

    return NULL;
}


static ngx_int_t
ngx_stream_ipdb_item_by_addr(ipdb_reader *reader, ngx_addr_t *addr,
    const char *lang, char *body)
{
    int node = 0;
    ngx_int_t err, i;
    off_t off = -1;
    size_t p = 0, o = 0, s = 0, e = 0;
    size_t len = reader->meta->fields_length;
    const char *content;

    for (i = 0; i < reader->meta->language_length; ++i) {
        if (ngx_strcmp(lang, reader->meta->language[i].name) == 0) {
            off = reader->meta->language[i].offset;
            break;
        }
    }

    if (off == -1) {
        return ErrNoSupportLanguage;
    }

    if (addr->sockaddr->sa_family == AF_INET) {
        if (!ipdb_reader_is_ipv4_support(reader)) {
            return ErrNoSupportIPv4;
        }

        struct sockaddr_in  *sin;

        sin = (struct sockaddr_in *) addr->sockaddr;
        // sin->sin_addr.s_addr
        err = ipdb_search(reader, (const u_char *)&sin->sin_addr.s_addr, 32, &node);
        if (err != ErrNoErr) {
            return err;
        }
    }
#if (NGX_HAVE_INET6)
    else if (addr->sockaddr->sa_family == AF_INET6) {
        if (!ipdb_reader_is_ipv6_support(reader)) {
            return ErrNoSupportIPv6;
        }

        struct in6_addr  *inaddr6;
        inaddr6 = &((struct sockaddr_in6 *) addr->sockaddr)->sin6_addr;
        err = ipdb_search(reader, (const u_char *)&inaddr6->s6_addr, 128, &node);
        if (err != ErrNoErr) {
            return err;
        }
    }
#endif
    else {
        return ErrIPFormat;
    }

    err = ipdb_resolve(reader, node, &content);
    if (err) {
        return err;
    }

    while (*(content + p)) {
        if (*(content + p) == '\t') {
            ++o;
        }
        if ((!e) && o == off + len) {
            e = p;
        }
        ++p;
        if (off && (!s) && (off_t)o == off) {
            s = p;
        }
    }
    if (!e) e = p;
    if (off + len > o + 1) {
        err = ErrDatabaseError;
    } else {
        strncpy(body, content + s, e - s);
        body[e - s] = 0;
    }

    return err;
}


static ngx_int_t
ngx_stream_ipdb_variable(ngx_stream_session_t *s,
    ngx_stream_variable_value_t *v, uintptr_t data)
{
    char                         *p;
    char                          body[512];
    ngx_int_t                     err;
    ngx_addr_t                    addr;
    ngx_stream_ipdb_srv_conf_t   *iscf;
    ngx_stream_ipdb_main_conf_t  *imcf;

    imcf = ngx_stream_get_module_main_conf(s, ngx_stream_ipdb_module);

    if (imcf == NULL || imcf->ipdb == NULL) {
        goto not_found;
    }

    iscf = ngx_stream_get_module_srv_conf(s, ngx_stream_ipdb_module);

    addr.sockaddr = s->connection->sockaddr;
    addr.socklen = s->connection->socklen;

    err = ngx_stream_ipdb_item_by_addr(imcf->ipdb, &addr,
            (const char *)iscf->lang.data, body);
    if (err) {
        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
        "ngx_http_ipdb_variable, ipdb find error: %d", err);

        goto not_found;
    }

    p = ngx_stream_ipdb_get_index_item(body, data);
    if (p == NULL) {
        goto not_found;
    }

    v->len = ngx_strlen(p);
    v->data = ngx_pnalloc(s->connection->pool, v->len);
    if (v->data == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(v->data, p, v->len);

    v->valid = 1;
    v->no_cacheable = 0;
    v->not_found = 0;

    return NGX_OK;

not_found:

    v->not_found = 1;

    return NGX_OK;
}


static void
ngx_stream_ipdb_cleanup(void *data)
{
    ngx_stream_ipdb_main_conf_t  *imcf = data;

    if (imcf->ipdb) {
        ipdb_reader_free(&imcf->ipdb);
    }
}
