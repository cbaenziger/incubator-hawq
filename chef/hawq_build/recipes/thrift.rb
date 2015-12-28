thrift_home = node[:hawq_build][:thrift][:home]

if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::thrift_debian'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::thrift_rhel'
end

directory thrift_home do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

git thrift_home do
  repository node[:hawq_build][:thrift][:git_repository]
  revision node[:hawq_build][:thrift][:git_revision]
  action :sync
  user node[:hawq_build][:build_user]
end

bash 'bootstrap thrift' do
  cwd thrift_home
  code './bootstrap.sh'
  creates "#{thrift_home}/configure"
  user node[:hawq_build][:build_user]
end

bash 'configure thrift' do
  cwd thrift_home
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  code "./configure CXXFLAGS='-g -O2' CFLAGS='-g -O2' --prefix=#{node[:hawq_build][:thrift][:prefix]} --with-cpp"
  creates "#{thrift_home}/Makefile"
  user node[:hawq_build][:build_user]
end

bash 'make thrift' do
  cwd thrift_home
  code "make -j #{node[:cpu][:total]}"
  user node[:hawq_build][:build_user]
end

bash 'make install thrift' do
  cwd thrift_home
  code 'make install'
  creates "#{node[:hawq_build][:thrift][:prefix]}/bin/thrift"
end
