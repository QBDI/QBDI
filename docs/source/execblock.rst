ExecBlock
=========

Introduction
------------

The ExecBlock is a concept which tries to simplify the problem of context switching between the 
host and the guest.

The main problem behind context switching is to be able to reference a memory location owned by the 
host in the context of the guest. Loading the memory location in a register effectively destroys a 
guest value which would thus need to be saved somewhere. One could allow the usage of the guest 
stack and save values on it but this design has two major drawbacks. Firstly, although supposedly 
unused, this modifies guest memory and could have side effects if the program uses uninitialized 
stack values or in the case of unforeseen optimizations. Secondly, this assumes that the stack 
registers always point on the stack or that a stack exists at all which might not be the case for 
more exotic languages or assembly code. The only possible mechanism left is to use relative 
addressing load and store. While x86 and x86-64 allow 32 bits offset, ARM only allows 11 bits 
offset [#]_. The ExecBlock design thus only requires to be able to load and store general purpose 
registers with an address offset up to 4096 bytes.

.. note::

    By default, AVX registers are also saved, one may want to disable this in case where it is not
    necessary, thus improving perfomances. It can be achieved using the environment flag 
    **QBDI_FORCE_DISABLE_AVX**.


Some modern operating systems do not allow the allocation of memory pages with read, write and 
execute permissions (RWX) for security reasons as this greatly facilitates remote code execution 
exploits. It is thus necessary to allocate two separate pages, with different permissions, for code 
and data. By exploiting the fact that most architectures use memory pages of 4096 bytes [#]_, 
allocating a data memory page next to a code memory page would allow the first instruction to 
address at least the first address of the data memory page. 

.. [#] Thumb supports even less but this is not a problem as, with the exception of embedded ARM 
       architectures (the Cortex-M series), one can always switch between ARM mode and Thumb mode.
.. [#] This is at least true for x86, x86_64, ARM, ARM64 and PowerPC.

Memory Layout
-------------

An ExecBlock is thus composed of two contiguous memory pages: the first one is called the code 
block and has read and execute permissions (RX) and the second one is called the data block and has 
read and write permissions (RW).  The code block contains a prologue, responsible for the host to 
guest context switching, followed by the code of the translated basic block and finally the 
epilogue responsible for the guest to host switching. Both the prologue and the epilogue use the 
beginning of the data block to store the context data. The context is split in three parts: the 
GPR context, the FPR context and the host context. The GPR and FPR context are straight forward 
and documented in the API itself as the GPRState and FPRState (see the State Management part of the 
API). The host context is used to store host data and a memory pointer called the selector. The 
selector is used by the prologue to determine on which basic block to jump next. The remaining 
space in the data block is used for shadows which can be used to store any data needed by the 
patching or instrumentation process.

.. image:: images/execblock_v3.svg

Reference
---------

.. doxygenclass:: QBDI::ExecBlock
   :members:
