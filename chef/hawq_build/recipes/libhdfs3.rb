libhdfs3_home = node[:hawq_build][:libhdfs3][:home]
libhdfs3_build = node[:hawq_build][:libhdfs3][:build]

if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::libhdfs3_debian'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::libhdfs3_rhel'
end

directory libhdfs3_home do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

git libhdfs3_home do
  repository node[:hawq_build][:libhdfs3][:git_repository]
  revision node[:hawq_build][:libhdfs3][:git_revision]
  action :sync
  user node[:hawq_build][:build_user]
end

directory libhdfs3_build do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

bash 'bootstrap libhdfs' do
  cwd libhdfs3_build
  code '../bootstrap'
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  creates "#{libhdfs3_build}/configure"
  user node[:hawq_build][:build_user]
end

bash 'configure libhdfs3' do
  cwd libhdfs3_build
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  code './configure CXXFLAGS="-g -O2" CFLAGS="-g -O2" --with-cpp'
  creates "#{libhdfs3_build}/Makefile"
  user node[:hawq_build][:build_user]
end

bash 'make libhdfs3' do
  cwd libhdfs3_build
  code "make -j #{node[:cpu][:total]}"
  user node[:hawq_build][:build_user]
end

bash 'test libhdfs3' do
  cwd libhdfs3_build
  code "make -j #{node[:cpu][:total]} unittest"
  user node[:hawq_build][:build_user]
end

bash 'make install libhdfs3' do
  cwd libhdfs3_build
  code "make -j #{node[:cpu][:total]} install"
  user node[:hawq_build][:build_user]
  creates "#{libhdfs3_build}/dist"
end
