%W{
   bison
   boost
   boost-devel
   flex
   openssl-devel
  }.each do |pkg|
  package pkg do
    action :install
  end
end
