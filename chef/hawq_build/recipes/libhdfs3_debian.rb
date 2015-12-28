%W{
   krb5-multidev
   libboost-all-dev
   libgsasl7
   libgsasl7-dev
   libgssapi-krb5-2
   libkrb5-3
   libkrb5-dev
   libkrb5support0
   libprotobuf8
   libprotobuf-dev
   libxml2-dev
   protobuf-compiler
   uuid-dev
  }.each do |pkg|
  package pkg do
    action :install
  end
end
