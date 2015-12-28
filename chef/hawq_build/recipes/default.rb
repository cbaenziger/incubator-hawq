if node['platform_family'] == 'debian'
  include_recipe 'hawq_build::debian_dependencies'
elsif node['platform_family'] == 'rhel'
  include_recipe 'hawq_build::rhel_dependencies'
end
