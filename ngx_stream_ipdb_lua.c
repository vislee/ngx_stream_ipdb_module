/*
 * Copyright (C) vislee
 */

#include <lualib.h>
#include <lauxlib.h>

#include "ngx_stream_ipdb_module.h"
#include "ngx_stream_ipdb_lua.h"

// #include "ngx_stream_lua_api.h"
#include "ngx_stream_lua_request.h"
#include "ngx_stream_lua_util.h"

static int
ngx_stream_ipdb_get_raw(lua_State *L)
{
    char                          body[512];
    ngx_int_t                     err;
    ngx_str_t                     ip, v;
    ngx_addr_t                    addr;
    ngx_stream_lua_request_t     *r;
    ngx_stream_ipdb_srv_conf_t   *iscf;
    ngx_stream_ipdb_main_conf_t  *imcf;


    r = ngx_stream_lua_get_req(L);
    if (r == NULL) {
        return luaL_error(L, "no request object found");
    }

    iscf = ngx_stream_lua_get_module_srv_conf(r, ngx_stream_ipdb_module);
    imcf = ngx_stream_lua_get_module_main_conf(r, ngx_stream_ipdb_module);

    if (lua_type(L, -1) != LUA_TSTRING) {
        return luaL_error(L, "bad addr");
    }

    ip.data = (u_char *) luaL_checklstring(L, 1, &ip.len);

    if (ngx_parse_addr(r->connection->pool, &addr, ip.data, ip.len) != NGX_OK) {
        return luaL_error(L, "bad addr format");
    }

    err = ngx_stream_ipdb_item_by_addr(imcf->ipdb, &addr,
            (const char *)iscf->lang.data, body);
    if (err) {
        return luaL_error(L, "bad addr info");
    }

    v.len = ngx_strlen(body);
    v.data = ngx_palloc(r->connection->pool, v.len);
    if (v.data == NULL) {
        return luaL_error(L, "no memory");
    }
    ngx_memcpy(v.data, body, v.len);

    lua_pushlstring(L, (const char *) v.data, (size_t) v.len);
    return 1;
}


static int
ngx_stream_ipdb_lua_register(lua_State *L)
{
    lua_createtable(L, 0, 1);

    lua_pushcfunction(L, ngx_stream_ipdb_get_raw);
    lua_setfield(L, -2, "get_raw");

    return 1;
}


ngx_int_t
ngx_stream_ipdb_lua_preload(ngx_conf_t *cf)
{
    if (ngx_stream_lua_add_package_preload(cf, "ngx.stream.ipdb",
                                         ngx_stream_ipdb_lua_register)
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    return NGX_OK;
}
