default[:hawq_build][:thrift][:git_repository] = 'https://git-wip-us.apache.org/repos/asf/thrift.git'
default[:hawq_build][:thrift][:git_revision] = '0.9.3'
default[:hawq_build][:thrift][:home] = "#{Chef::Config[:file_cache_path]}/thrift"
default[:hawq_build][:thrift][:prefix] = '/usr/local/'

default[:hawq_build][:bison][:url] = 'http://ftp.gnu.org/gnu/bison/bison-2.5.1.tar.gz'
default[:hawq_build][:bison][:home] = "#{Chef::Config[:file_cache_path]}/bison"
default[:hawq_build][:bison][:prefix] = '/usr/local/'

default[:hawq_build][:build_user] = 'vagrant'
default[:hawq_build][:hawq_user] = 'hawq'
default[:hawq_build][:hawq_home_dir] = "/var/lib/#{node[:hawq_build][:hawq_user]}"

default[:hawq_build][:libhdfs3][:git_repository] = 'https://github.com/PivotalRD/libhdfs3'
default[:hawq_build][:libhdfs3][:git_revision] = 'apache-rpc-9'
default[:hawq_build][:libhdfs3][:home] = "#{Chef::Config[:file_cache_path]}/libhdfs3"
# Note: at the moment this must be one level under libhdfs3's home dir per the "bash[bootstrap libhdfs3]" action
default[:hawq_build][:libhdfs3][:build] = "#{node[:hawq_build][:libhdfs3][:home]}/build"

default[:hawq_build][:libyarn][:git_repository] = 'https://github.com/apache/incubator-hawq'
default[:hawq_build][:libyarn][:git_revision] = 'master'
default[:hawq_build][:libyarn][:home] = "#{Chef::Config[:file_cache_path]}/libyarn"
# Note: at the moment this must be one level under libyarn's src dir per the "bash[bootstrap libyarn]" action
default[:hawq_build][:libyarn][:build] = "#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/build"

default[:hawq_build][:git_repository] = 'https://github.com/apache/incubator-hawq'
default[:hawq_build][:git_revision] = 'master'
default[:hawq_build][:home] = "#{Chef::Config[:file_cache_path]}/hawq"
