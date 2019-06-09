Name
====

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


[Back to TOC](#table-of-contents)

Example Configuration
====================

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
