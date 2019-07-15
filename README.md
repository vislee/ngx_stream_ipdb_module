Name
====


[![Build Status](https://travis-ci.org/vislee/ngx_stream_ipdb_module.svg?branch=master)](https://travis-ci.org/vislee/ngx_stream_ipdb_module)
[![Coverage Status](https://coveralls.io/repos/github/vislee/ngx_stream_ipdb_module/badge.svg?branch=master)](https://coveralls.io/github/vislee/ngx_stream_ipdb_module?branch=master)

ngx_stream_ipdb_module - creates variables with values depending on the client IP address, using the precompiled [ipip.net](https://www.ipip.net) [ipdb](https://www.ipip.net/ipdb/test).


Table of Contents
=================
* [Name](#name)
* [Status](#status)
* [Install](#install)
* [Example Configuration](#example-configuration)
* [Directives](#directives)
    * [ipdb](#ipdb)
    * [ipdb_language](#ipdb_language)
* [Variable](#variable)
    * [$ipdb_country_name](#ipdb_country_name)
    * [$ipdb_region_name](#ipdb_region_name)
    * [$ipdb_city_name](#ipdb_city_name)
    * [$ipdb_isp_domain](#ipdb_isp_domain)
    * [$ipdb_raw](#ipdb_raw)
* [Lua API](#lua_api)
    * [get_raw](#get_raw)
* [TODO](#todo)
* [Author](#author)
* [Copyright and License](#copyright-and-license)
* [See Also](#see-also)


Status
======
The module is currently in active development.

[Back to TOC](#table-of-contents)

Install
=======

```sh
# install json-c lib
# centos
yum install json-c-devel -y
#or mac OSX
brew install json-c

configure --prefix=/usr/local/nginx --with-stream --add-module=./github.com/vislee/ngx_stream_ipdb_module
# or dynamic compile
configure --prefix=/usr/local/nginx --with-stream --add-dynamic-module=./github.com/vislee/ngx_stream_ipdb_module --with-compat

# or stream_lua
configure --prefix=/usr/local/nginx --with-stream  --add-module=./github.com/openresty/stream-lua-nginx-module/ --add-module=./github.com/vislee/ngx_stream_ipdb_module --with-cc-opt='-I ./github.com/openresty/stream-lua-nginx-module/src'
```

The following information is success:

 >> checking for json-c library ... found
 >>  + ngx_stream_ipdb_module was configured


[Back to TOC](#table-of-contents)

Example Configuration
====================

```nginx

# load_module modules/ngx_stream_ipdb_module.so;

stream {
    ipdb conf/ipiptest.ipdb;
    ipdb_language "CN";

    server {
        listen 8091;
        # ipdb_language EN;

        return "country_name:$ipdb_country_name, raw_info:$ipdb_raw";
    }
}

stream {
    ipdb conf/ipiptest.ipdb;
    ipdb_language "CN";

    server {
        listen 8092;
        # ipdb_language EN;

        content_by_lua_block {
            local sipdb = require "ngx.stream.ipdb";
            ngx.say(sipdb.get_raw("127.0.0.1"));
        }
    }
}


```

[Back to TOC](#table-of-contents)

TODO
==========

[Back to TOC](#table-of-contents)

Directives
==========

ipdb
----
**syntax:** *ipdb file;*

**default:** *-*

**context:** *stream*

Specifies a database.

ipdb_language
-------------
**syntax:** *ipdb_language EN|CN;*

**default:** *EN*

**context:** *stream,server*

set variable language.


[Back to TOC](#table-of-contents)


Variable
========

ipdb_country_name
----------------

$ipdb_country_name - country name, for example, "中国", "China"

ipdb_region_name
----------------

$ipdb_region_name - country region name, for example, "内蒙古","Nei Mongol", "北京", "Beijing"

ipdb_city_name
--------------

$ipdb_city_name - city name, for example, "呼和浩特", "Hohhot", "北京", "Beijing"

ipdb_isp_domain
---------------

$ipdb_isp_domain - ISP name, for example, "电信", "ChinaTelecom"

ipdb_raw
--------

$ipdb_raw - raw info, for example, "中国\t内蒙古\t呼和浩特"，"China\tNei Mongol\tHohhot"


[Back to TOC](#table-of-contents)


Lua API
========

get_raw
-------

**syntax:** ipdb_raw = ngx.stream.ipdb.get_raw(addr)

**context:** content_by_lua*, log_by_lua*


[Back to TOC](#table-of-contents)

Author
======

wenqiang li(vislee)

[Back to TOC](#table-of-contents)

Copyright and License
=====================

This module is licensed under the BSD 3-Clause License.

Copyright (C) 2019, by vislee.

All rights reserved.

[Back to TOC](#table-of-contents)


See Also
========

+ [ngx_stream_geoip_module](http://nginx.org/en/docs/stream/ngx_stream_geoip_module.html)
+ [github.com/ipipdotnet/ipdb-c](https://github.com/ipipdotnet/ipdb-c)
+ [ngx_http_ipdb_module](https://github.com/vislee/ngx_http_ipdb_module)
