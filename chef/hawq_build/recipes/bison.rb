bison_home = "#{Chef::Config[:file_cache_path]}/bison"

package 'm4' do
  action :install
end

if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::bison_debian'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::bison_rhel'
end

directory bison_home do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

remote_file "#{Chef::Config[:file_cache_path]}/bison.tgz" do
  source node[:hawq_build][:bison][:url]
  checksum node[:hawq_build][:bison][:checksum]
  action :create
  user node[:hawq_build][:build_user]
end

bash 'expand bison' do
  cwd bison_home
  code "tar -xzf #{Chef::Config[:file_cache_path]}/bison.tgz --strip-components=1"
  user node[:hawq_build][:build_user]
end

bash 'configure bison' do
  cwd bison_home
  environment 'PATH' => '/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin'
  code "./configure --prefix=#{node[:hawq_build][:bison][:prefix]} CXXFLAGS='-g -O2' CFLAGS='-g -O2' --with-cpp"
  creates "#{bison_home}/Makefile"
  user node[:hawq_build][:build_user]
end

bash 'make bison' do
  cwd bison_home
  code "make -j #{node[:cpu][:total]}"
  user node[:hawq_build][:build_user]
end

bash 'make install bison' do
  cwd bison_home
  code 'make install'
  creates "#{node[:hawq_build][:bison][:prefix]}/bin/bison"
end
