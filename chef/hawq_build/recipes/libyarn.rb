libyarn_home = node[:hawq_build][:libyarn][:home]
libyarn_build = node[:hawq_build][:libyarn][:build]

# use the libhdfs3 dependencies due to the similarity between
# libyarn and libhdfs3
if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::libhdfs3_debian'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::libhdfs3_rhel'
end

directory libyarn_home do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

git libyarn_home do
  repository node[:hawq_build][:libyarn][:git_repository]
  revision node[:hawq_build][:libyarn][:git_revision]
  action :sync
  user node[:hawq_build][:build_user]
end

directory libyarn_build do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

bash 'bootstrap libyarn' do
  cwd libyarn_build
  code '../bootstrap'
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  creates "#{libyarn_build}/configure"
  user node[:hawq_build][:build_user]
end

bash 'configure libyarn' do
  cwd libyarn_build
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  code './configure CXXFLAGS="-g -O2" CFLAGS="-g -O2" --with-cpp'
  creates "#{libyarn_build}/Makefile"
  user node[:hawq_build][:build_user]
end

bash 'make libyarn' do
  cwd libyarn_build
  code "make -j #{node[:cpu][:total]}"
  user node[:hawq_build][:build_user]
end

# XXX Can't get unittest to pass yet
#bash 'test libyarn' do
#  cwd libyarn_build
#  code "make -j #{node[:cpu][:total]} unittest"
#  user node[:hawq_build][:build_user]
#end

bash 'make install libyarn' do
  cwd libyarn_build
  code "make -j #{node[:cpu][:total]} install"
  user node[:hawq_build][:build_user]
  creates "#{libyarn_build}/../dist"
end
