In the following example we assume you have the kdreports source available in
/path/to/kdreports-source.  replace '/path/to/kdreports-source' to fit your needs.

$ conan create -s build_type=Release -o kdreports:build_examples=True -o kdreports:build_tests=True /path/to/kdreports-source/conan

Configuration options:
 * build_static
   Builds static versions of the libraries. Default=False

 * build_tests
   Build the test harness. Default=False

 * build_examples
   Build the examples. Default=True
