%W{
   automake
   cmake 
   g++ 
   gcc 
   git
   gperf
   libtool
   make 
   pkg-config
  }.each do |pkg|
  package pkg do
    action :install
  end
end
