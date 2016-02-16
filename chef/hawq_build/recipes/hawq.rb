hawq_home = node[:hawq_build][:home]

if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::hawq_debian'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::hawq_rhel'
end

directory hawq_home do
  owner node[:hawq_build][:build_user]
  recursive true
  action :create
end

git hawq_home do
  repository node[:hawq_build][:git_repository]
  revision node[:hawq_build][:git_revision]
  action :sync
  user node[:hawq_build][:build_user]
end

bash 'configure hawq' do
  cwd hawq_home
  environment 'PATH' => '/usr/local/maven/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin',
              'LD_RUN_PATH' => "#{node[:hawq_build][:libhdfs3][:home]}/dist/lib:#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/dist/lib:#{node[:hawq_build][:thrift][:prefix]}/lib"
              
  code "./configure BISON=#{node[:hawq_build][:bison][:prefix]}/bin/bison --with-libs=#{node[:hawq_build][:libhdfs3][:home]}/dist/lib:#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/dist/lib --with-includes=#{node[:hawq_build][:libhdfs3][:home]}/dist/include/:#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/dist/include"
  creates "#{hawq_home}/GNUmakefile"
  user node[:hawq_build][:build_user]
end

bash 'make hawq' do
  cwd hawq_home
  environment 'PATH' => '/usr/local/maven/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin',
              'HOME' => "/tmp/#{node[:hawq_build][:build_user]}" # needed for maven
  code "make -j #{node[:cpu][:total]}"
  creates "#{hawq_home}/src/bin/pg_ctl/pg_ctl"
  user node[:hawq_build][:build_user]
end

bash 'test hawq' do
  cwd "#{hawq_home}/src/backend"
  environment 'PATH' => '/usr/local/maven/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin',
              'LD_LIBRARY_PATH' => "#{node[:hawq_build][:libhdfs3][:home]}/dist/lib:#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/dist/lib:#{node[:hawq_build][:thrift][:prefix]}/lib"
  code 'make unittest-check'
  user node[:hawq_build][:build_user]
end

bash 'make install' do
  cwd "#{hawq_home}"
  environment 'PATH' => '/usr/local/maven/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin',
              'LD_LIBRARY_PATH' => "#{node[:hawq_build][:libhdfs3][:home]}/dist/lib:#{node[:hawq_build][:libyarn][:home]}/depends/libyarn/dist/lib:#{node[:hawq_build][:thrift][:prefix]}/lib"
  creates "/usr/local/hawq"
  code 'make install'
end
