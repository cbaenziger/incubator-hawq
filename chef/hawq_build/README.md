This cookbook builds HAWQ

Most recipes for dependencies follow a similar boilerplate:
* Setup a home or build directory
* Run bootstrap if necessay
* configure
* make
* make install

We use test-kitchen to test that building each dependency works. Sadly artifacts are not promoted up so we then run an increasing series of recipes to ensure HAWQ builds successfully in the end
