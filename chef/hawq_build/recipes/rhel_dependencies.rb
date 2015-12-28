include_recipe 'maven::default'

%W{
   automake
   cmake 
   gcc 
   gcc-c++
   git
   gperf
   libtool
   make 
  }.each do |pkg|
  package pkg do
    action :install
  end
end
