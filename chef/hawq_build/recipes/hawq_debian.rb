include_recipe 'maven::default'

# build dependencies
%W{
   libapr1-dev
   libbz2-dev
   libcurl4-gnutls-dev
   libevent-dev
   libjson0
   libjson0-dev
   libsnappy-dev
   libyaml-dev
   thrift-compiler
  }.each do |pkg|
  package pkg do
    action :install
  end
end

# test dependencies
%W{libesmtp-dev}.each do |pkg|
  package pkg do
    action :install
  end
end
