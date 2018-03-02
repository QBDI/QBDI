.. highlight:: c++

Logging System
==============

LogSys is a singleton controlling the logging output of QBDI (see ``src/Utility/LogSys.h``). It 
relies on a tag and priority filtering system to provide a fine-grained control of the output. 
Indeed QBDI logging system is quite verbose, detailing at every step and every action taken by the 
Engine. 

Each log entry is assigned a tag describing the component it comes from and a priority describing 
its importance. The tags follow the convention ``Class::Method`` or just ``function``. 

The methods generating the most important logs are: 

* ``Engine::run`` for everything related to execution decisions.
* ``Engine::patch`` for patch generations.
* ``Engine::instrument`` for instrumentation generations.
* ``ExecBlockManager::getProgrammedExecBlock`` for code cache lookup.
* ``ExecBlock::writeSequence`` for JIT code generation.
* ``ExecBlock::execute`` for sequence execution and callback execution.

The priority comes in three different levels:

.. doxygenenum:: LogPriority
   :project: QBDI_C

.. warning:: It is important to note that ``Debug`` level logging is only enabled in debug build. 
             It can, however, be manually re-enabled in release build, please look into 
             ``src/Utility/LogSys.h`` for more information.

Controlling Log Output
----------------------

The log filters act as a whitelist of which logs to output. By default there are no filters and 
thus no output. A filter can be added by calling the :cpp:func:`addLogFilter`.

.. doxygenfunction:: QBDI::addLogFilter
   :project: QBDI_CPP

The filters specify the minimum log priority to output for every entry matching the tag specified. 
For example, a filter with a tag ``Engine::patch`` and priority ``Warning`` would output all logs 
with priority ``Warning`` and ``Error`` coming from ``Engine::patch``. The tag parameter is 
actually fuzzy matched so ``Engine::*`` would match everything coming out of the engine. A good 
default filter to have to ensure that all errors and warnings are reported is::

   QBDI::addLogFilter("*", QBDI::LogPriority::Warning);

By default logs are output to ``stderr`` but this can be changed with :cpp:func:`setLogOutput`.

.. doxygenfunction:: QBDI::setLogOutput
   :project: QBDI_CPP

For example, redirecting the logs to a file can be done with the line below::

   FILE* logFile = fopen(".qbdi_log", "w");
   QBDI::setLogOutput(logFile);
  

Adding Log Output
-----------------

The log system can only be used inside C++ source code. It relies on using the :cpp:var:`QBDI::LOGSYS` 
singleton to log data using the :cpp:func:`QBDI::LogSys::log` and :cpp:func:`QBDI::LogSys::logCallback`.
The later is useful in the case the computation required to generate the text of the log output is 
costly: the callback is only called in the case the tag and priority was matched by a filter.

.. doxygenvariable:: QBDI::LOGSYS
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::LogSys::log
   :project: QBDI_CPP
 
.. doxygenfunction:: QBDI::LogSys::logCallback
   :project: QBDI_CPP

It is, however, not recommended to use those methods directly and use our macro system which controls 
the presence of logging in release builds.

.. doxygendefine:: LogCallback
   :project: QBDI_CPP

.. doxygendefine:: LogDebug
   :project: QBDI_CPP

.. doxygendefine:: LogWarning
   :project: QBDI_CPP

.. doxygendefine:: LogError
   :project: QBDI_CPP

Below is an example of adding (very verbose) logging to a simple source code::

   bool appendToFile(const char* fileName, const char* toAppend) {
       FILE* f = nullptr;
       size_t len = strlen(toAppend);

       LogDebug("appendToFile", "Appending %s to file %s", toAppend, fileName);

       if(len == 0) {
           LogWarning("appendToFile", "toAppend is empty!");
           return true;
       }

       f = fopen(fileName, "a");
       if(f == nullptr) {
           LogError("appendToFile", "Failed to open file %s", fileName);
           return false;
       }

       if(fwrite((const void*) toAppend, len, 1, f) != len) {
           LogError("appendToFile", "Failed to append to file %s", fileName);
           fclose(f);
           return false;
       }

       fclose(f);
       return true;
   }
