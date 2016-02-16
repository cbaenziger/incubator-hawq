auth_key_file = ::File.join(node[:hawq_build][:hawq_home_dir], '.ssh/authorized_keys')
ssh_key_file = ::File.join(node[:hawq_build][:hawq_home_dir], '.ssh/id_dsa.pub')

user node[:hawq_build][:hawq_user] do
  home node[:hawq_build][:hawq_home_dir]
  shell '/bin/bash'
  action :create
  manage_home true
end

directory ::File.join(node[:hawq_build][:hawq_home_dir], '.ssh') do
  owner node[:hawq_build][:hawq_user]
  mode '0700'
end

bash 'generate ssh key for hawq' do
  user node[:hawq_build][:hawq_user]
  creates ssh_key_file
  code "ssh-keygen -t dsa -q -f #{ssh_key_file} -P \"\""
end

ruby_block 'append ssh key to authorized keys' do
  def read_key_material(key)
    ::File.readlines(key).\
      select{ |line| line !~ /-----.*PRIVATE KEY----/ }.\
      map{ |line| line.gsub(/\n/, '') }.join('')
  end
  block do
    require 'fileutils'
    key = ''
    begin
      key = read_key_material(ssh_key_file)
      key_present = ::File.read(auth_key_file) !~ \
        ::Regexp.new(::Regexp.escape(key))
    rescue SystemCallError
      key_present = false
    end
    begin
      f = ::File.new(auth_key_file, 'a')
      f.write(key + "\n")
    ensure
      f.close unless f.nil?
      ::FileUtils.chown node[:hawq_build][:hawq_user], nil, auth_key_file
    end
  end  
  not_if { begin
             ::File.read(auth_key_file) =~ \
               ::Regexp.new(::Regexp.escape(read_key_material(ssh_key_file)))
           rescue SystemCallError
             false
           end }
end

bash 'set hdfs homedir for greenplum_path.sh' do
  desired_LIBHDFS3_CONF_line = "LIBHDFS3_CONF=/etc/hadoop/conf/hdfs-site.xml"
  code "sed -i -e 's#^LIBHDFS3_CONF=.*##{desired_LIBHDFS3_CONF_line}#' /usr/local/hawq/greenplum_path.sh"
  not_if "egrep -q '#{::Regexp.escape(desired_LIBHDFS3_CONF_line)}' /usr/local/hawq/greenplum_path.sh"
end

bash 'set LD_LIBRARY_PATH for greenplum_path.sh' do
  desired_LD_LIBRARY_PATH_line = "LD_LIBRARY_PATH=$GPHOME/lib:$GPHOME/ext/python/lib:#{node[:hawq_build][:libhdfs3][:home]}/dist/lib:#{::File.join(node[:hawq_build][:libyarn][:build], '..')}/dist/lib:$LD_LIBRARY_PATH"
  code "sed -i -e 's#^LD_LIBRARY_PATH=.*##{desired_LD_LIBRARY_PATH_line}#' /usr/local/hawq/greenplum_path.sh"
  not_if "egrep -q '#{::Regexp.escape(desired_LD_LIBRARY_PATH_line)}' /usr/local/hawq/greenplum_path.sh"
end

bash 'set PYTHONPATH for greenplum_path.sh' do
  desired_PYTHONPATH_line = "PYTHONPATH=$GPHOME/lib/python:$GPHOME/lib/python/pygresql:$PYTHONPATH"
  code "sed -i -e 's#^PYTHONPATH=.*##{desired_PYTHONPATH_line}#' /usr/local/hawq/greenplum_path.sh"
  not_if "egrep -q '#{::Regexp.escape(desired_PYTHONPATH_line)}' /usr/local/hawq/greenplum_path.sh"
end

bash 'set hdfs name for hawq-site.xml' do
  code "sed -i 's#localhost:8020/hawq_default##{node['bcpc']['hadoop']['hdfs']['cluster-name']}/hawq_default#' /usr/local/hawq/etc/hawq-site.xml"
end

bash 'set hawq owner of hawq' do
  code "chown -R #{node[:hawq_build][:hawq_user]} /usr/local/hawq"
end

bash 'create hawq hdfs dirs' do
  code <<-EOH
    hdfs dfs -mkdir /user/#{node[:hawq_build][:hawq_user]}
    hdfs dfs -mkdir /hawq_default/
    hdfs dfs -chown -R #{node[:hawq_build][:hawq_user]} /hawq_default/ /user/#{node[:hawq_build][:hawq_user]}
  EOH
  user 'hdfs'
end

bash 'create HAWQ cluster' do
  code 'source /usr/local/hawq/greenplum_path.sh; hawq init cluster -a'
  user node[:hawq_build][:hawq_user]
  not_if {{ ::File.exist?("#{node[:hawq_build][:hawq_home_dir]}/hawq-data-directory") }}
end

bash 'start HAWQ cluster' do
  code 'source /usr/local/hawq/greenplum_path.sh; hawq start cluster -a'
  user node[:hawq_build][:hawq_user]
  not_if 'source /usr/local/hawq/greenplum_path.sh; hawq state'
end
  code 'hawq
