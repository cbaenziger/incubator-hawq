%W{
   bison
   flex
   libboost-all-dev
  }.each do |pkg|
  package pkg do
    action :install
  end
end
