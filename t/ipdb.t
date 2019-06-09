#!/usr/bin/perl

# (C) vislee

# Tests for stream ipdb module.

###############################################################################

use warnings;
use strict;

use Test::More;

use Socket qw/ $CRLF /;

BEGIN { use FindBin; chdir($FindBin::Bin); }

use lib 'lib';
use Test::Nginx;
use Test::Nginx::Stream qw/ stream /;

###############################################################################

select STDERR; $| = 1;
select STDOUT; $| = 1;

my $t = Test::Nginx->new();

$t->write_file_expand('nginx.conf', <<'EOF');

%%TEST_GLOBALS%%

daemon off;

# load_module /tmp/nginx/modules/ngx_stream_ipdb_module.so;

events {
}

stream {
    set_real_ip_from  127.0.0.1/32;

    ipdb ./ipiptest.ipdb;
    ipdb_language CN;

    server {
        listen  127.0.0.1:8091 proxy_protocol;

        return  "country_name:$ipdb_country_name
                 region_name:$ipdb_region_name
                 city_name:$ipdb_city_name
                 isp_domain:$ipdb_isp_domain
                 ipdb_raw:$ipdb_raw";
    }
}

EOF

$t->try_run('no ipdb')->plan(5);

###############################################################################

my %data = stream_pp('36.102.4.81') =~ /(\w+):(.*)/g;
is($data{country_name}, '中国', 'ipdb country name');
is($data{region_name}, '内蒙古', 'ipdb region name');
is($data{city_name}, '呼和浩特', 'ipdb city name');
is($data{isp_domain}, '', 'ipdb isp domain');
is($data{ipdb_raw}, "中国\t内蒙古\t呼和浩特", 'ipdb raw');

###############################################################################

sub stream_pp {
    my ($ip) = @_;
    my $type = ($ip =~ ':' ? 'TCP6' : 'TCP4');
    return stream('127.0.0.1:' . port(8091))
        ->io("PROXY $type $ip 127.0.0.1 8091 8091${CRLF}");
}

###############################################################################

