name             "hawq_build"
license          "Apache License 2.0"
description      "Builds Apache HAWQ"
long_description IO.read(File.join(File.dirname(__FILE__), 'README.md'))
version          "0.5.0"

depends 'apt'
depends 'ark'
depends 'java'
depends 'maven'
depends 'ubuntu'
depends 'yum-epel'
