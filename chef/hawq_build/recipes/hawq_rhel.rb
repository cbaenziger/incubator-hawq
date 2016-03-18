%W{
   apr-devel
   bison-devel
   boost
   boost-devel
   bzip2-devel
   flex-devel
   gcc
   gcc-c++
   gperf
   json-c-devel
   krb5-devel
   libcurl-devel
   libesmtp-devel
   libevent-devel
   libgsasl-devel
   libuuid-devel
   libxml2-devel
   libyaml-devel
   make
   openssl-devel
   python-devel
   readline-devel
   snappy-devel
   zlib-devel
  }.each do |pkg|
  package pkg do
    action :install
  end
end

# runtime dependencies
%W{PyGreSQL python-psutil python-paramiko}.each do |pkg|
  package pkg do
    action :install
  end
end

