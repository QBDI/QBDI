.. _developer-testing:

Testing
=======

Making mistakes while writing a DBI framework is particularly easy and often fatal. Executing 
code generated at runtime is a dangerous game and close attention should be paid to the patching 
engine. That is why we use two different testing methods.

Functional Test Suite
---------------------

QBDI has a small functional test suite implemented using `Google Test <https://github.com/google/googletest>`_ 
which verifies the essential functions of QBDI. This test suite is automatically built and made
available in the ``test/`` build subdirectory::

    $ ./test/QBDITest --gtest_color=yes
    [==========] Running 57 tests from 9 test cases.
    [----------] Global test environment set-up.
    ...
    [==========] 57 tests from 9 test cases ran. (47 ms total)
    [  PASSED  ] 57 tests.


Validator
---------

The validator is a generic test system allowing to compare a normal execution with an instrumented 
execution. This way a large number of programs can be used as a test suite to ensure that QBDI does 
not alter their normal functions. It is only compatible with Linux and macOS at the moment.

The idea is to pilot a debugging session using an instrumented execution of the same program. These 
two instances share the same environment and arguments and originated from the same fork thus have 
the same memory layout. This allows to compare the program state at each step and verify that both 
execution take the same path and give the same result. The validator is not only capable of 
determining if two executions differ but also capable of identifying where they diverged. It thus 
double down as a debugging tool.

There is, however, a few caveats to this approach. First, the two instances of the program will 
compete for resources. This means running ``sha1sum test.txt`` will work because the two instances 
can read the same file at the same time, but removing a directory ``rmdir testdir/`` will always fail because 
only one instance will be able to delete the directory. Second, the dynamic memory allocations will 
not match on the two instances. This is because the instrumented instance is running the whole QBDI 
framework and validator instrumentation which is making extra allocations in between the original 
program allocations. A partial mitigation was nevertheless implemented which tracks allocations on both 
sides and compute an allowed offset for specific memory address ranges.

One of the essential concepts of the validator are error cascades. They establish a probable 
causality chain between errors and allow to backtrack from the point where the execution crashed 
or diverged to the probable cause. Indeed, an error might only cause problems thousands of 
instructions later.

The validator uses dynamic library injection to take control of a program startup. Options are 
communicated using environment variables. The following options are available:

``VALIDATOR_VERBOSITY``
    * ``Stat``: Only display execution statistics. This is the default.
    * ``Summary``: Display execution statistics and error cascade summaries.
    * ``Detail``: Display execution statistics and complete error cascades.
    * ``Full``: Display full execution trace, execution statistics and complete error cascades.

``VALIDATOR_COVERAGE``
    Specify a file name where instruction coverage statistics will be written out.

Linux
^^^^^

The validator uses ``LD_PRELOAD`` for injection under Linux and an example command line would be::

    $ LD_PRELOAD=./tools/validator/libvalidator.so VALIDATOR_VERBOSITY=Detail VALIDATOR_COVERAGE=coverage.txt ls 

macOS
^^^^^

The validator uses ``DYLD_INSERT_LIBRARIES`` for injection under macOS and an example command line would be::

    $ DYLD_INSERT_LIBRARIES=./tools/validator/libvalidator.dyld VALIDATOR_VERBOSITY=Detail VALIDATOR_COVERAGE=coverage.txt ./ls 

Please note that, under macOS, SIP prevents from debugging system binaries. A workaround is to copy 
the target binary in a local directory. Also, for the moment, the validator requires root to 
obtain debugging rights.

Validation Runner
-----------------

The validation runner is an automation system to run a series of validation task specified in a 
configuration file and aggregate the results. Those results are stored in a database which enables 
to perform historical comparison between validation runs and warns in case of an anomaly.
