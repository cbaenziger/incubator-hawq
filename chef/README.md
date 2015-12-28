# Apache HAWQ Chef Infrastructure
---
Apache HAWQ is a large project with many dependencies. To build these dependencies and ensure HAWQ is consistently built across multiple operating systems the Chef [Chef](https://github.com/chef/chef) configuration management and [Test-Kitchen](https://github.com/test-kitchen/test-kitchen) virutalization infrastructure can be used.

Further, the Chef recipes here can be used by continuous build infrastructure such as Jenkins to ensure a consistently setup slave node for all builds.

Trying various Java versions can be overridden by modifying the Chef node attributes:
````
node['java']['install_flavor']
node['java']['jdk_version']
````

To change the OS one is building against, change the Test-Kitchen `.kitchen.yml` platform identifier.

To test changes build for all OSes on any Chef and Test-Kitchen supported platform, one may use an all-in-one package for Chef and Test-Kitchen such as [ChefDK](https://downloads.chef.io/chef-dk/). Once installed, one may include `/opt/chefdk/embedded/bin/` in one's PATH. Lastly:
* Run `kitchen list` to see all test targets
* To run the full HAWQ build, one can run: `kitchen converge 'hawq-.*'`
* To reset one's environment, run: `kitchen destroy '.*'`
