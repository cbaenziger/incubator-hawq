%W{
   boost-devel
   krb5-devel
   krb5-libs
   libgsasl
   libgsasl-devel
   libuuid
   libuuid-devel
   libxml2-devel
   protobuf-compiler
   protobuf-devel
  }.each do |pkg|
  package pkg do
    action :install
  end
end
