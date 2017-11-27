/*
** Python Bindings for the QBDI library.
**
** Maintainers:
**
**   - Jonathan Salwan (Quarkslab)
**
**
** Env:
**
**   - LD_PRELOAD=<libpyqbdi.so>
**   - PYQBDI_TOOL=<your_python_tool.py>
**
**
** Syntax:
**
**   $ LD_PRELOAD=./build/tools/pyqbdi/libpyqbdi.so PYQBDI_TOOL=./examples/pyqbdi/trace_inst.py /usr/bin/id
**
**
** Exemple of tool:
**
**   import pyqbdi
**
**   def mycb(vm, gpr, fpr):
**       inst = vm.getInstAnalysis(pyqbdi.ANALYSIS_INSTRUCTION | pyqbdi.ANALYSIS_DISASSEMBLY)
**       print "0x%x: %s" %(inst.address, inst.disassembly)
**       return pyqbdi.CONTINUE
**
**   def pyqbdipreload_on_run(vm, start, stop):
**       vm.addCodeCB(pyqbdi.PREINST, mycb)
**       vm.run(start, stop)
*/

#include <Python.h>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <list>
#include <longintrepr.h>

#if defined(__unix__) || defined(__APPLE__)
  #include <dlfcn.h>
#endif

#ifdef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__
#endif
#include <cstdio>

#ifndef __STDC_LIB_EXT1__
//! Secure open as fopen is deprecated on windows but fopen_s is not standard
int fopen_s(FILE** fd, const char* fn, const char* flags) {
  *fd = fopen(fn, flags);
  if(*fd == 0)
    return -1;
  else
    return 0;
}
#endif

#include "QBDIPreload.h"

#include "VM.h"
#include "Memory.h"
#include "Platform.h"

/* Init the QBDIPreload */
QBDIPRELOAD_INIT;



namespace QBDI {
  namespace Bindings {
    namespace Python {

      /* The pyqbdi module */
      PyObject* module = nullptr;

      /* argc of the instrumented binary */
      int argc = 0;

      /* argv of the instrumented binary */
      char** argv = nullptr;

      //! pyInstAnalysis object.
      typedef struct {
        PyObject_HEAD
        QBDI::InstAnalysis* inst;
      } InstAnalysis_Object;

      //! pyGPRState object.
      typedef struct {
        PyObject_HEAD
        QBDI::GPRState* gpr;
      } GPRState_Object;

      //! pyFPRState object.
      typedef struct {
        PyObject_HEAD
        QBDI::FPRState* fpr;
      } FPRState_Object;

      //! pyMemoryAccess object.
      typedef struct {
        PyObject_HEAD
        QBDI::MemoryAccess* memoryAccess;
      } MemoryAccess_Object;

      //! pyOperandAnalysis object.
      typedef struct {
        PyObject_HEAD
        QBDI::OperandAnalysis* operand;
      } OperandAnalysis_Object;

      //! pyVMInstance object.
      typedef struct {
        PyObject_HEAD
        QBDI::VMInstanceRef vm;
      } VMInstance_Object;

      /*! Checks if the pyObject is a QBDI::InstAnalysis. */
      #define PyInstAnalysis_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::InstAnalysis_Type)

      /*! Returns the QBDI::InstAnalysis. */
      #define PyInstAnalysis_AsInstAnalysis(v) (((QBDI::Bindings::Python::InstAnalysis_Object*)(v))->inst)

      /*! Checks if the pyObject is a QBDI::GPRState. */
      #define PyGPRState_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::GPRState_Type)

      /*! Returns the QBDI::GPRState. */
      #define PyGPRState_AsGPRState(v) (((QBDI::Bindings::Python::GPRState_Object*)(v))->gpr)

      /*! Checks if the pyObject is a QBDI::FPRState. */
      #define PyFPRState_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::FPRState_Type)

      /*! Returns the QBDI::FPRState. */
      #define PyFPRState_AsFPRState(v) (((QBDI::Bindings::Python::FPRState_Object*)(v))->fpr)

      /*! Checks if the pyObject is a QBDI::MemoryAccess. */
      #define PyMemoryAccess_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::MemoryAccess_Type)

      /*! Returns the QBDI::MemoryAccess. */
      #define PyMemoryAccess_AsMemoryAccess(v) (((QBDI::Bindings::Python::MemoryAccess_Object*)(v))->memoryAccess)

      /*! Checks if the pyObject is a QBDI::OperandAnalysis. */
      #define PyOperandAnalysis_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::OperandAnalysis_Type)

      /*! Returns the QBDI::OperandAnalysis. */
      #define PyOperandAnalysis_AsOperandAnalysis(v) (((QBDI::Bindings::Python::OperandAnalysis_Object*)(v))->operand)

      /*! Checks if the pyObject is a QBDI::VMInstance. */
      #define PyVMInstance_Check(v) ((v)->ob_type == &QBDI::Bindings::Python::VMInstance_Type)

      /*! Returns the QBDI::VMInstance. */
      #define PyVMInstance_AsVMInstance(v) (((QBDI::Bindings::Python::VMInstance_Object*)(v))->vm)

      /* Prototypes */
      static PyObject* PyFPRState(const QBDI::FPRState*);
      static PyObject* PyFPRState(void);
      static PyObject* PyGPRState(const QBDI::GPRState*);
      static PyObject* PyGPRState(void);
      static PyObject* PyInstAnalysis(const QBDI::InstAnalysis*);
      static PyObject* PyMemoryAccess(const QBDI::MemoryAccess&);
      static PyObject* PyMemoryAccess(const QBDI::MemoryAccess*);
      static PyObject* PyOperandAnalysis(const QBDI::OperandAnalysis*);
      static PyObject* PyVMInstance(QBDI::VMInstanceRef vm);


      /* Returns a QBDI::rword from a PyLong object */
      QBDI::rword PyLong_AsRword(PyObject* vv) {
        PyLongObject* v;
        QBDI::rword x, prev;
        Py_ssize_t i;

        if (vv == NULL || !PyLong_Check(vv)) {
          if (vv != NULL && PyInt_Check(vv)) {
              QBDI::rword val = PyInt_AsLong(vv);
              return val;
          }
          throw std::runtime_error("QBDI::Bindings::Python::PyLong_AsRword(): Bad internal call.");
        }

        v = reinterpret_cast<PyLongObject*>(vv);
        i = Py_SIZE(v);
        x = 0;
        if (i < 0)
          throw std::runtime_error("QBDI::Bindings::Python::PyLong_AsRword():  Cannot convert negative value to unsigned long.");

        while (--i >= 0) {
            prev = x;
            x = (x << PyLong_SHIFT) | v->ob_digit[i];
            if ((x >> PyLong_SHIFT) != prev)
                throw std::runtime_error("QBDI::Bindings::Python::PyLong_AsRword():  long int too large to convert.");
        }

        return x;
      }


      /* PyOperandAnalysis destructor */
      static void OperandAnalysis_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyOperandAnalysis_AsOperandAnalysis(self));
        Py_DECREF(self);
      }


      /* OperandAnalysis attributes */
      static PyObject* OperandAnalysis_getattro(PyObject* self, PyObject* name) {
        try {
          if (std::string(PyString_AsString(name)) == "type")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->type);

          else if (std::string(PyString_AsString(name)) == "value")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->value);

          else if (std::string(PyString_AsString(name)) == "size")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->size);

          else if (std::string(PyString_AsString(name)) == "regOff")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->regOff);

          else if (std::string(PyString_AsString(name)) == "regCtxIdx")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->regCtxIdx);

          else if (std::string(PyString_AsString(name)) == "regName") {
            const OperandAnalysis* operand = PyOperandAnalysis_AsOperandAnalysis(self);

            if (operand->regName)
              return PyString_FromString(operand->regName);

            Py_RETURN_NONE;
          }

          else if (std::string(PyString_AsString(name)) == "regAccess")
            return PyLong_FromLong(PyOperandAnalysis_AsOperandAnalysis(self)->regAccess);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyObject_GenericGetAttr((PyObject *)self, name);
      }


      /* Description of the python representation of an OperandAnalysis */
      PyTypeObject OperandAnalysis_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "OperandAnalysis",                          /* tp_name */
        sizeof(OperandAnalysis_Object),             /* tp_basicsize */
        0,                                          /* tp_itemsize */
        OperandAnalysis_dealloc,                    /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        (getattrofunc)OperandAnalysis_getattro,     /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "OperandtAnalysis objects",                 /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        0,                                          /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyOperandAnalysis(const QBDI::OperandAnalysis* operandAnalysis) {
        OperandAnalysis_Object* object;

        PyType_Ready(&OperandAnalysis_Type);
        object = PyObject_NEW(OperandAnalysis_Object, &OperandAnalysis_Type);
        if (object != NULL) {
          object->operand = static_cast<QBDI::OperandAnalysis*>(malloc(sizeof(*operandAnalysis)));
          std::memcpy(object->operand, operandAnalysis, sizeof(*operandAnalysis));
        }

        return (PyObject*)object;
      }


      /* PyInstAnalysis destructor */
      static void InstAnalysis_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyInstAnalysis_AsInstAnalysis(self));
        Py_DECREF(self);
      }


      /* InstAnalysis attributes */
      static PyObject* InstAnalysis_getattro(PyObject* self, PyObject* name) {
        try {
          if (std::string(PyString_AsString(name)) == "mnemonic") {
            const InstAnalysis* inst = PyInstAnalysis_AsInstAnalysis(self);

            if (inst->mnemonic)
              return PyString_FromString(inst->mnemonic);

            Py_RETURN_NONE;
          }

          else if (std::string(PyString_AsString(name)) == "address")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->address);

          else if (std::string(PyString_AsString(name)) == "instSize")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->instSize);

          else if (std::string(PyString_AsString(name)) == "affectControlFlow")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->affectControlFlow);

          else if (std::string(PyString_AsString(name)) == "isBranch")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->isBranch);

          else if (std::string(PyString_AsString(name)) == "isCall")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->isCall);

          else if (std::string(PyString_AsString(name)) == "isReturn")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->isReturn);

          else if (std::string(PyString_AsString(name)) == "isCompare")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->isCompare);

          else if (std::string(PyString_AsString(name)) == "isPredicable")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->isPredicable);

          else if (std::string(PyString_AsString(name)) == "mayLoad")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->mayLoad);

          else if (std::string(PyString_AsString(name)) == "mayStore")
            return PyBool_FromLong(PyInstAnalysis_AsInstAnalysis(self)->mayStore);

          else if (std::string(PyString_AsString(name)) == "disassembly") {
            const InstAnalysis* inst = PyInstAnalysis_AsInstAnalysis(self);

            if (inst->disassembly)
              return PyString_FromString(PyInstAnalysis_AsInstAnalysis(self)->disassembly);

            Py_RETURN_NONE;
          }

          else if (std::string(PyString_AsString(name)) == "numOperands")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->numOperands);

          else if (std::string(PyString_AsString(name)) == "operands") {
            const InstAnalysis* inst = PyInstAnalysis_AsInstAnalysis(self);
            PyObject* ret = PyList_New(inst->numOperands);

            for (uint8_t i = 0; i < inst->numOperands; i++) {
              PyList_SetItem(ret, i, PyOperandAnalysis(&(inst->operands[i])));
            }

            return ret;
          }

          else if (std::string(PyString_AsString(name)) == "symbol") {
            const InstAnalysis* inst = PyInstAnalysis_AsInstAnalysis(self);

            if (inst->symbol)
              return PyString_FromString(inst->symbol);

            Py_RETURN_NONE;
          }

          else if (std::string(PyString_AsString(name)) == "symbolOffset")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->symbolOffset);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyObject_GenericGetAttr((PyObject *)self, name);
      }


      /* Description of the python representation of an InstAnalysis */
      PyTypeObject InstAnalysis_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "InstAnalysis",                             /* tp_name */
        sizeof(InstAnalysis_Object),                /* tp_basicsize */
        0,                                          /* tp_itemsize */
        InstAnalysis_dealloc,                       /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        (getattrofunc)InstAnalysis_getattro,        /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "InstAnalysis objects",                     /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        0,                                          /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyInstAnalysis(const QBDI::InstAnalysis* instAnalysis) {
        InstAnalysis_Object* object;

        PyType_Ready(&InstAnalysis_Type);
        object = PyObject_NEW(InstAnalysis_Object, &InstAnalysis_Type);
        if (object != NULL) {
          object->inst = static_cast<QBDI::InstAnalysis*>(malloc(sizeof(*instAnalysis)));
          std::memcpy(object->inst, instAnalysis, sizeof(*instAnalysis));
        }

        return (PyObject*)object;
      }


      /* PyGPRState destructor */
      static void GPRState_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyGPRState_AsGPRState(self));
        Py_DECREF(self);
      }


      /* PyGPRState get attributes */
      static PyObject* GPRState_getattro(PyObject* self, PyObject* name) {
        try {
          #if defined(QBDI_ARCH_X86_64)
          if (std::string(PyString_AsString(name)) == "rax")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rax);

          else if (std::string(PyString_AsString(name)) == "rbx")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rbx);

          else if (std::string(PyString_AsString(name)) == "rcx")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rcx);

          else if (std::string(PyString_AsString(name)) == "rdx")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rdx);

          else if (std::string(PyString_AsString(name)) == "rsi")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rsi);

          else if (std::string(PyString_AsString(name)) == "rdi")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rdi);

          else if (std::string(PyString_AsString(name)) == "r8")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r8);

          else if (std::string(PyString_AsString(name)) == "r9")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r9);

          else if (std::string(PyString_AsString(name)) == "r10")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r10);

          else if (std::string(PyString_AsString(name)) == "r11")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r11);

          else if (std::string(PyString_AsString(name)) == "r12")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r12);

          else if (std::string(PyString_AsString(name)) == "r13")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r13);

          else if (std::string(PyString_AsString(name)) == "r14")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r14);

          else if (std::string(PyString_AsString(name)) == "r15")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r15);

          else if (std::string(PyString_AsString(name)) == "rbp")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rbp);

          else if (std::string(PyString_AsString(name)) == "rsp")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rsp);

          else if (std::string(PyString_AsString(name)) == "rip")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->rip);

          else if (std::string(PyString_AsString(name)) == "eflags")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->eflags);
          #endif

          #if defined(QBDI_ARCH_ARM)
          if (std::string(PyString_AsString(name)) == "r0")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r0);

          else if (std::string(PyString_AsString(name)) == "r1")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r1);

          else if (std::string(PyString_AsString(name)) == "r2")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r2);

          else if (std::string(PyString_AsString(name)) == "r3")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r3);

          else if (std::string(PyString_AsString(name)) == "r4")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r4);

          else if (std::string(PyString_AsString(name)) == "r5")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r5);

          else if (std::string(PyString_AsString(name)) == "r6")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r6);

          else if (std::string(PyString_AsString(name)) == "r7")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r7);

          else if (std::string(PyString_AsString(name)) == "r8")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r8);

          else if (std::string(PyString_AsString(name)) == "r9")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r9);

          else if (std::string(PyString_AsString(name)) == "r10")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r10);

          else if (std::string(PyString_AsString(name)) == "r12")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->r12);

          else if (std::string(PyString_AsString(name)) == "fp")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->fp);

          else if (std::string(PyString_AsString(name)) == "sp")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->sp);

          else if (std::string(PyString_AsString(name)) == "lr")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->lr);

          else if (std::string(PyString_AsString(name)) == "pc")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->pc);

          else if (std::string(PyString_AsString(name)) == "cpsr")
            return PyLong_FromLong(PyGPRState_AsGPRState(self)->cpsr);
          #endif
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyObject_GenericGetAttr((PyObject *)self, name);
      }


      /* PyGPRState set attributes */
      static int GPRState_setattro(PyObject* self, PyObject* name, PyObject* item) {
        if (item == nullptr) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::GPRState::setattro(): Cannot delete the '%s' attribute", PyString_AsString(name));
          return -1;
        }

        if (!PyLong_Check(item) && !PyInt_Check(item)) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::GPRState::setattro(): Assignment must be an integer, not '%.200s'", item->ob_type->tp_name);
          return -1;
        }

        try {
          #if defined(QBDI_ARCH_X86_64)
            if (std::string(PyString_AsString(name)) == "rax")
              PyGPRState_AsGPRState(self)->rax = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rbx")
              PyGPRState_AsGPRState(self)->rbx = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rcx")
              PyGPRState_AsGPRState(self)->rcx = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rdx")
              PyGPRState_AsGPRState(self)->rdx = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rsi")
              PyGPRState_AsGPRState(self)->rsi = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rdi")
              PyGPRState_AsGPRState(self)->rdi = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r8")
              PyGPRState_AsGPRState(self)->r8 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r9")
              PyGPRState_AsGPRState(self)->r9 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r10")
              PyGPRState_AsGPRState(self)->r10 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r11")
              PyGPRState_AsGPRState(self)->r11 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r12")
              PyGPRState_AsGPRState(self)->r12 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r13")
              PyGPRState_AsGPRState(self)->r13 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r14")
              PyGPRState_AsGPRState(self)->r14 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r15")
              PyGPRState_AsGPRState(self)->r15 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rbp")
              PyGPRState_AsGPRState(self)->rbp = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rsp")
              PyGPRState_AsGPRState(self)->rsp = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "rip")
              PyGPRState_AsGPRState(self)->rip = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "eflags")
              PyGPRState_AsGPRState(self)->eflags = PyLong_AsRword(item);

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif

          #if defined(QBDI_ARCH_ARM)
            if (std::string(PyString_AsString(name)) == "r0")
              PyGPRState_AsGPRState(self)->r0 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r1")
              PyGPRState_AsGPRState(self)->r1 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r2")
              PyGPRState_AsGPRState(self)->r2 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r3")
              PyGPRState_AsGPRState(self)->r3 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r4")
              PyGPRState_AsGPRState(self)->r4 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r5")
              PyGPRState_AsGPRState(self)->r5 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r6")
              PyGPRState_AsGPRState(self)->r6 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r7")
              PyGPRState_AsGPRState(self)->r7 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r8")
              PyGPRState_AsGPRState(self)->r8 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r9")
              PyGPRState_AsGPRState(self)->r9 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r10")
              PyGPRState_AsGPRState(self)->r10 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "r12")
              PyGPRState_AsGPRState(self)->r12 = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "fp")
              PyGPRState_AsGPRState(self)->fp = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "sp")
              PyGPRState_AsGPRState(self)->sp = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "lr")
              PyGPRState_AsGPRState(self)->lr = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "pc")
              PyGPRState_AsGPRState(self)->pc = PyLong_AsRword(item);

            else if (std::string(PyString_AsString(name)) == "cpsr")
              PyGPRState_AsGPRState(self)->cpsr = PyLong_AsRword(item);

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif
        }
        catch (const std::exception& e) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::GPRState::setattro(): %s", e.what());
          return -1;
        }

        return 0;
      }


      /* Description of the python representation of a GPRState */
      PyTypeObject GPRState_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "GPRState",                                 /* tp_name */
        sizeof(GPRState_Object),                    /* tp_basicsize */
        0,                                          /* tp_itemsize */
        GPRState_dealloc,                           /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        (getattrofunc)GPRState_getattro,            /* tp_getattro */
        (setattrofunc)GPRState_setattro,            /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "GPRState objects",                         /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        0,                                          /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyGPRState(const QBDI::GPRState* gpr) {
        GPRState_Object* object;

        PyType_Ready(&GPRState_Type);
        object = PyObject_NEW(GPRState_Object, &GPRState_Type);
        if (object != NULL) {
          object->gpr = static_cast<QBDI::GPRState*>(malloc(sizeof(*gpr)));
          std::memcpy(object->gpr, gpr, sizeof(*gpr));
        }

        return (PyObject*)object;
      }


      static PyObject* PyGPRState(void) {
        GPRState_Object* object;

        PyType_Ready(&GPRState_Type);
        object = PyObject_NEW(GPRState_Object, &GPRState_Type);
        if (object != NULL) {
          object->gpr = static_cast<QBDI::GPRState*>(malloc(sizeof(QBDI::GPRState)));
          std::memset(object->gpr, 0x00, sizeof(QBDI::GPRState));
        }

        return (PyObject*)object;
      }


      /* PyFPRState destructor */
      static void FPRState_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyFPRState_AsFPRState(self));
        Py_DECREF(self);
      }


      /* PyFPRState attributes */
      static PyObject* FPRState_getattro(PyObject* self, PyObject* name) {
        try {
          #if defined(QBDI_ARCH_X86_64)
          if (std::string(PyString_AsString(name)) == "ftw")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->ftw);

          else if (std::string(PyString_AsString(name)) == "fop")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->fop);

          else if (std::string(PyString_AsString(name)) == "ip")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->ip);

          else if (std::string(PyString_AsString(name)) == "cs")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->cs);

          else if (std::string(PyString_AsString(name)) == "dp")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->dp);

          else if (std::string(PyString_AsString(name)) == "ds")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->ds);

          else if (std::string(PyString_AsString(name)) == "mxcsr")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->mxcsr);

          else if (std::string(PyString_AsString(name)) == "mxcsrmask")
            return PyLong_FromLong(PyFPRState_AsFPRState(self)->mxcsrmask);

          else if (std::string(PyString_AsString(name)) == "stmm0")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm0.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm1")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm1.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm2")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm2.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm3")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm3.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm4")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm4.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm5")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm5.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm6")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm6.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "stmm7")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->stmm7.reg), 10, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm0")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm0), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm1")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm1), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm2")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm2), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm3")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm3), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm4")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm4), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm5")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm5), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm6")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm6), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm7")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm7), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm8")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm8), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm9")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm9), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm10")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm10), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm11")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm11), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm12")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm12), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm13")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm13), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm14")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm14), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "xmm15")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->xmm15), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm0")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm0), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm1")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm1), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm2")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm2), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm3")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm3), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm4")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm4), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm5")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm5), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm6")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm6), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm7")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm7), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm8")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm8), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm9")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm9), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm10")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm10), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm11")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm11), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm12")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm12), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm13")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm13), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm14")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm14), 16, true, false);

          else if (std::string(PyString_AsString(name)) == "ymm15")
            return _PyLong_FromByteArray(reinterpret_cast<const unsigned char*>(PyFPRState_AsFPRState(self)->ymm15), 16, true, false);
          #endif

          #if defined(QBDI_ARCH_ARM)
          if (std::string(PyString_AsString(name)) == "s0")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[0]);

          else if (std::string(PyString_AsString(name)) == "s1")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[1]);

          else if (std::string(PyString_AsString(name)) == "s2")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[2]);

          else if (std::string(PyString_AsString(name)) == "s3")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[3]);

          else if (std::string(PyString_AsString(name)) == "s4")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[4]);

          else if (std::string(PyString_AsString(name)) == "s5")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[5]);

          else if (std::string(PyString_AsString(name)) == "s6")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[6]);

          else if (std::string(PyString_AsString(name)) == "s7")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[7]);

          else if (std::string(PyString_AsString(name)) == "s8")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[8]);

          else if (std::string(PyString_AsString(name)) == "s9")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[9]);

          else if (std::string(PyString_AsString(name)) == "s10")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[10]);

          else if (std::string(PyString_AsString(name)) == "s11")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[11]);

          else if (std::string(PyString_AsString(name)) == "s12")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[12]);

          else if (std::string(PyString_AsString(name)) == "s13")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[13]);

          else if (std::string(PyString_AsString(name)) == "s14")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[14]);

          else if (std::string(PyString_AsString(name)) == "s15")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[15]);

          else if (std::string(PyString_AsString(name)) == "s16")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[16]);

          else if (std::string(PyString_AsString(name)) == "s17")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[17]);

          else if (std::string(PyString_AsString(name)) == "s18")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[18]);

          else if (std::string(PyString_AsString(name)) == "s19")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[19]);

          else if (std::string(PyString_AsString(name)) == "s20")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[20]);

          else if (std::string(PyString_AsString(name)) == "s21")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[21]);

          else if (std::string(PyString_AsString(name)) == "s22")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[22]);

          else if (std::string(PyString_AsString(name)) == "s23")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[23]);

          else if (std::string(PyString_AsString(name)) == "s24")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[24]);

          else if (std::string(PyString_AsString(name)) == "s25")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[25]);

          else if (std::string(PyString_AsString(name)) == "s26")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[26]);

          else if (std::string(PyString_AsString(name)) == "s27")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[27]);

          else if (std::string(PyString_AsString(name)) == "s28")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[28]);

          else if (std::string(PyString_AsString(name)) == "s29")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[29]);

          else if (std::string(PyString_AsString(name)) == "s30")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[30]);

          else if (std::string(PyString_AsString(name)) == "s31")
            return PyFloat_FromDouble(PyFPRState_AsFPRState(self)->s[31]);
          #endif
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyObject_GenericGetAttr((PyObject *)self, name);
      }


      #if defined(QBDI_ARCH_X86_64)
      /* Convert a Py{Int,Long} to a bytes array */
      static void qbdi_PyLong_AsByteArray(void* bytes, PyObject* o, size_t size) {
        unsigned long long value = 0;

        std::memset(bytes, 0x00, size);
        if (PyInt_Check(o)) {
          value = PyInt_AsLong(o);
          std::memcpy(bytes, &value, sizeof(value));
        }
        else {
          PyLongObject *v = (PyLongObject*)o;
          _PyLong_AsByteArray(v, reinterpret_cast<unsigned char*>(bytes), size, true, true);
        }
      }
      #endif


      /* PyFPRState set attributes */
      static int FPRState_setattro(PyObject* self, PyObject* name, PyObject* item) {
        #if defined(QBDI_ARCH_X86_64)
        unsigned char bytes[16] = {0};
        #endif

        if (item == nullptr) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::FPRState::setattro(): Cannot delete the '%s' attribute", PyString_AsString(name));
          return -1;
        }

        #if defined(QBDI_ARCH_X86_64)
        if (!PyLong_Check(item) && !PyInt_Check(item)) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::FPRState::setattro(): Assignment must be an integer, not '%.200s'", item->ob_type->tp_name);
          return -1;
        }

        /* Convert a Py{Int,Long} to a bytes array */
        QBDI::Bindings::Python::qbdi_PyLong_AsByteArray(bytes, item, sizeof(bytes));
        #endif

        #if defined(QBDI_ARCH_ARM)
        if (!PyFloat_Check(item)) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::FPRState::setattro(): Assignment must be a float, not '%.200s'", item->ob_type->tp_name);
          return -1;
        }
        #endif

        try {
          #if defined(QBDI_ARCH_X86_64)
            if (std::string(PyString_AsString(name)) == "ftw")
              PyFPRState_AsFPRState(self)->ftw = PyLong_AsRword(item) & 0xff;

            else if (std::string(PyString_AsString(name)) == "fop")
              PyFPRState_AsFPRState(self)->fop = PyLong_AsRword(item) & 0xffff;

            else if (std::string(PyString_AsString(name)) == "ip")
              PyFPRState_AsFPRState(self)->ip = PyLong_AsRword(item) & 0xffffffff;

            else if (std::string(PyString_AsString(name)) == "cs")
              PyFPRState_AsFPRState(self)->cs = PyLong_AsRword(item) & 0xffff;

            else if (std::string(PyString_AsString(name)) == "dp")
              PyFPRState_AsFPRState(self)->dp = PyLong_AsRword(item) & 0xffffffff;

            else if (std::string(PyString_AsString(name)) == "ds")
              PyFPRState_AsFPRState(self)->ds = PyLong_AsRword(item) & 0xffff;

            else if (std::string(PyString_AsString(name)) == "mxcsr")
              PyFPRState_AsFPRState(self)->mxcsr = PyLong_AsRword(item) & 0xffffffff;

            else if (std::string(PyString_AsString(name)) == "mxcsrmask")
              PyFPRState_AsFPRState(self)->mxcsrmask = PyLong_AsRword(item) & 0xffffffff;

            else if (std::string(PyString_AsString(name)) == "stmm0")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm0.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm0.reg));

            else if (std::string(PyString_AsString(name)) == "stmm1")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm1.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm1.reg));

            else if (std::string(PyString_AsString(name)) == "stmm2")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm2.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm2.reg));

            else if (std::string(PyString_AsString(name)) == "stmm3")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm3.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm3.reg));

            else if (std::string(PyString_AsString(name)) == "stmm4")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm4.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm4.reg));

            else if (std::string(PyString_AsString(name)) == "stmm5")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm5.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm5.reg));

            else if (std::string(PyString_AsString(name)) == "stmm6")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm6.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm6.reg));

            else if (std::string(PyString_AsString(name)) == "stmm7")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm7.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm7.reg));

            else if (std::string(PyString_AsString(name)) == "stmm2")
              std::memcpy(PyFPRState_AsFPRState(self)->stmm2.reg, bytes, sizeof(PyFPRState_AsFPRState(self)->stmm2.reg));

            else if (std::string(PyString_AsString(name)) == "xmm0")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm0, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm0));

            else if (std::string(PyString_AsString(name)) == "xmm1")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm1, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm1));

            else if (std::string(PyString_AsString(name)) == "xmm2")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm2, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm2));

            else if (std::string(PyString_AsString(name)) == "xmm3")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm3, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm3));

            else if (std::string(PyString_AsString(name)) == "xmm4")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm4, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm4));

            else if (std::string(PyString_AsString(name)) == "xmm5")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm5, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm5));

            else if (std::string(PyString_AsString(name)) == "xmm6")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm6, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm6));

            else if (std::string(PyString_AsString(name)) == "xmm7")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm7, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm7));

            else if (std::string(PyString_AsString(name)) == "xmm8")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm8, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm8));

            else if (std::string(PyString_AsString(name)) == "xmm9")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm9, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm9));

            else if (std::string(PyString_AsString(name)) == "xmm10")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm10, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm10));

            else if (std::string(PyString_AsString(name)) == "xmm11")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm11, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm11));

            else if (std::string(PyString_AsString(name)) == "xmm12")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm12, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm12));

            else if (std::string(PyString_AsString(name)) == "xmm13")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm13, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm13));

            else if (std::string(PyString_AsString(name)) == "xmm14")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm14, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm14));

            else if (std::string(PyString_AsString(name)) == "xmm15")
              std::memcpy(PyFPRState_AsFPRState(self)->xmm15, bytes, sizeof(PyFPRState_AsFPRState(self)->xmm15));

            else if (std::string(PyString_AsString(name)) == "ymm0")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm0, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm0));

            else if (std::string(PyString_AsString(name)) == "ymm1")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm1, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm1));

            else if (std::string(PyString_AsString(name)) == "ymm2")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm2, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm2));

            else if (std::string(PyString_AsString(name)) == "ymm3")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm3, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm3));

            else if (std::string(PyString_AsString(name)) == "ymm4")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm4, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm4));

            else if (std::string(PyString_AsString(name)) == "ymm5")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm5, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm5));

            else if (std::string(PyString_AsString(name)) == "ymm6")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm6, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm6));

            else if (std::string(PyString_AsString(name)) == "ymm7")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm7, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm7));

            else if (std::string(PyString_AsString(name)) == "ymm8")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm8, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm8));

            else if (std::string(PyString_AsString(name)) == "ymm9")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm9, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm9));

            else if (std::string(PyString_AsString(name)) == "ymm10")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm10, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm10));

            else if (std::string(PyString_AsString(name)) == "ymm11")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm11, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm11));

            else if (std::string(PyString_AsString(name)) == "ymm12")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm12, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm12));

            else if (std::string(PyString_AsString(name)) == "ymm13")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm13, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm13));

            else if (std::string(PyString_AsString(name)) == "ymm14")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm14, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm14));

            else if (std::string(PyString_AsString(name)) == "ymm15")
              std::memcpy(PyFPRState_AsFPRState(self)->ymm15, bytes, sizeof(PyFPRState_AsFPRState(self)->ymm15));

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif

          #if defined(QBDI_ARCH_ARM)
            if (std::string(PyString_AsString(name)) == "s0")
              PyFPRState_AsFPRState(self)->s[0] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s1")
              PyFPRState_AsFPRState(self)->s[1] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s2")
              PyFPRState_AsFPRState(self)->s[2] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s3")
              PyFPRState_AsFPRState(self)->s[3] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s4")
              PyFPRState_AsFPRState(self)->s[4] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s5")
              PyFPRState_AsFPRState(self)->s[5] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s6")
              PyFPRState_AsFPRState(self)->s[6] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s7")
              PyFPRState_AsFPRState(self)->s[7] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s8")
              PyFPRState_AsFPRState(self)->s[8] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s9")
              PyFPRState_AsFPRState(self)->s[9] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s10")
              PyFPRState_AsFPRState(self)->s[10] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s11")
              PyFPRState_AsFPRState(self)->s[11] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s12")
              PyFPRState_AsFPRState(self)->s[12] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s13")
              PyFPRState_AsFPRState(self)->s[13] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s14")
              PyFPRState_AsFPRState(self)->s[14] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s15")
              PyFPRState_AsFPRState(self)->s[15] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s16")
              PyFPRState_AsFPRState(self)->s[16] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s17")
              PyFPRState_AsFPRState(self)->s[17] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s18")
              PyFPRState_AsFPRState(self)->s[18] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s19")
              PyFPRState_AsFPRState(self)->s[19] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s20")
              PyFPRState_AsFPRState(self)->s[20] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s21")
              PyFPRState_AsFPRState(self)->s[21] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s22")
              PyFPRState_AsFPRState(self)->s[22] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s23")
              PyFPRState_AsFPRState(self)->s[23] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s24")
              PyFPRState_AsFPRState(self)->s[24] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s25")
              PyFPRState_AsFPRState(self)->s[25] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s26")
              PyFPRState_AsFPRState(self)->s[26] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s27")
              PyFPRState_AsFPRState(self)->s[27] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s28")
              PyFPRState_AsFPRState(self)->s[28] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s29")
              PyFPRState_AsFPRState(self)->s[29] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s30")
              PyFPRState_AsFPRState(self)->s[30] = PyFloat_AsDouble(item);

            else if (std::string(PyString_AsString(name)) == "s31")
              PyFPRState_AsFPRState(self)->s[31] = PyFloat_AsDouble(item);

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif
        }
        catch (const std::exception& e) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::FPRState::setattro(): %s", e.what());
          return -1;
        }

        return 0;
      }


      /* Description of the python representation of a FPRState */
      PyTypeObject FPRState_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "FPRState",                                 /* tp_name */
        sizeof(FPRState_Object),                    /* tp_basicsize */
        0,                                          /* tp_itemsize */
        FPRState_dealloc,                           /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        (getattrofunc)FPRState_getattro,            /* tp_getattro */
        (setattrofunc)FPRState_setattro,            /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "FPRState objects",                         /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        0,                                          /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyFPRState(const QBDI::FPRState* fpr) {
        FPRState_Object* object;

        PyType_Ready(&FPRState_Type);
        object = PyObject_NEW(FPRState_Object, &FPRState_Type);
        if (object != NULL) {
          object->fpr = static_cast<QBDI::FPRState*>(malloc(sizeof(*fpr)));
          std::memcpy(object->fpr, fpr, sizeof(*fpr));
        }

        return (PyObject*)object;
      }


      static PyObject* PyFPRState(void) {
        FPRState_Object* object;

        PyType_Ready(&FPRState_Type);
        object = PyObject_NEW(FPRState_Object, &FPRState_Type);
        if (object != NULL) {
          object->fpr = static_cast<QBDI::FPRState*>(malloc(sizeof(QBDI::FPRState)));
          std::memset(object->fpr, 0x00, sizeof(QBDI::FPRState));
        }

        return (PyObject*)object;
      }


      /* PyMemoryAccess destructor */
      static void MemoryAccess_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyMemoryAccess_AsMemoryAccess(self));
        Py_DECREF(self);
      }


      /* MemoryAccess attributes */
      static PyObject* MemoryAccess_getattro(PyObject* self, PyObject* name) {
        try {
          if (std::string(PyString_AsString(name)) == "instAddress")
            return PyLong_FromLong(PyMemoryAccess_AsMemoryAccess(self)->instAddress);

          else if (std::string(PyString_AsString(name)) == "accessAddress")
            return PyLong_FromLong(PyMemoryAccess_AsMemoryAccess(self)->accessAddress);

          else if (std::string(PyString_AsString(name)) == "value")
            return PyLong_FromLong(PyMemoryAccess_AsMemoryAccess(self)->value);

          else if (std::string(PyString_AsString(name)) == "size")
            return PyLong_FromLong(PyMemoryAccess_AsMemoryAccess(self)->size);

          else if (std::string(PyString_AsString(name)) == "type")
            return PyLong_FromLong(PyMemoryAccess_AsMemoryAccess(self)->type);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyObject_GenericGetAttr((PyObject *)self, name);
      }


      /* Description of the python representation of a MemoryAccess */
      PyTypeObject MemoryAccess_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "MemoryAccess",                             /* tp_name */
        sizeof(MemoryAccess_Object),                /* tp_basicsize */
        0,                                          /* tp_itemsize */
        MemoryAccess_dealloc,                       /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        (getattrofunc)MemoryAccess_getattro,        /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "MemoryAccess objects",                     /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        0,                                          /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyMemoryAccess(const QBDI::MemoryAccess* memoryAccess) {
        MemoryAccess_Object* object;

        PyType_Ready(&MemoryAccess_Type);
        object = PyObject_NEW(MemoryAccess_Object, &MemoryAccess_Type);
        if (object != NULL) {
          object->memoryAccess = static_cast<QBDI::MemoryAccess*>(malloc(sizeof(*memoryAccess)));
          std::memcpy(object->memoryAccess, memoryAccess, sizeof(*memoryAccess));
        }

        return (PyObject*)object;
      }


      static PyObject* PyMemoryAccess(const QBDI::MemoryAccess& memoryAccess) {
        return PyMemoryAccess(&memoryAccess);
      }


      /* Trampoline for python callbacks */
      static QBDI::VMAction trampoline(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* function) {
        /* Create function arguments */
        PyObject* args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, QBDI::Bindings::Python::PyVMInstance(vm));
        PyTuple_SetItem(args, 1, QBDI::Bindings::Python::PyGPRState(gprState));
        PyTuple_SetItem(args, 2, QBDI::Bindings::Python::PyFPRState(fprState));

        /* Call the function and check the return value */
        PyObject* ret = PyObject_CallObject((PyObject*)function, args);
        Py_DECREF(args);
        if (ret == nullptr) {
          PyErr_Print();
          exit(1);
        }

        /* Default: We continue the instrumentation */
        if (!PyLong_Check(ret) && !PyInt_Check(ret))
          return QBDI::CONTINUE;

        /* Otherwise, return the user's value */
        return static_cast<QBDI::VMAction>(PyLong_AsLong(ret));
      }


      /* Trampoline2 for python callbacks */
      static QBDI::VMAction trampoline(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* function) {
        return trampoline(vm, gprState, fprState, function);
      }


      /* PyVMInstance destructor */
      static void VMInstance_dealloc(PyObject* self) {
        std::cout << std::flush;
        Py_DECREF(self);
      }


      /*! Register a callback for when a specific address is executed.
       *
       * @param[in] address   Code address which will trigger the callback.
       * @param[in] pos       Relative position of the callback (QBDI_PREINST / QBDI_POSTINST).
       * @param[in] cbk       A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addCodeAddrCB(PyObject* self, PyObject* args) {
        PyObject* addr     = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &addr, &pos, &function);

        if (addr == nullptr || (!PyLong_Check(addr) && !PyInt_Check(addr)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeAddrCB(): Expects an integer as first argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeAddrCB(): Expects an InstPosition as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeAddrCB(): Expects a function as third argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addCodeAddrCB(PyLong_AsRword(addr),
                                                                    static_cast<QBDI::InstPosition>(PyInt_AsLong(pos)),
                                                                    QBDI::Bindings::Python::trampoline,
                                                                    function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Register a callback event for a specific instruction event.
       *
       * @param[in] pos       Relative position of the event callback (QBDI_PREINST / QBDI_POSTINST).
       * @param[in] cbk       A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addCodeCB(PyObject* self, PyObject* args) {
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &pos, &function);

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeCB(): Expects an InstPosition as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeCB(): Expects a function as second argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addCodeCB(static_cast<QBDI::InstPosition>(PyInt_AsLong(pos)),
                                                                QBDI::Bindings::Python::trampoline,
                                                                function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Register a callback for when a specific address range is executed.
       *
       * @param[in] start     Start of the address range which will trigger the callback.
       * @param[in] end       End of the address range which will trigger the callback.
       * @param[in] pos       Relative position of the callback (QBDI_PREINST / QBDI_POSTINST).
       * @param[in] cbk       A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addCodeRangeCB(PyObject* self, PyObject* args) {
        PyObject* start    = nullptr;
        PyObject* end      = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOOO", &start, &end, &pos, &function);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeRangeCB(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeRangeCB(): Expects an integer as second argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeRangeCB(): Expects an InstPosition as thrid argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addCodeRangeCB(): Expects a function as fourth argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addCodeRangeCB(PyLong_AsRword(start),
                                                                     PyLong_AsRword(end),
                                                                     static_cast<QBDI::InstPosition>(PyInt_AsLong(pos)),
                                                                     QBDI::Bindings::Python::trampoline,
                                                                     function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Add the executable address ranges of a module to the set of instrumented address ranges.
       *
       * @param[in] name  The module's name.
       *
       * @return  True if at least one range was added to the instrumented ranges.
       */
      static PyObject* vm_addInstrumentedModule(PyObject* self, PyObject* module) {
        if (!PyString_Check(module))
          return PyErr_Format(PyExc_TypeError, "QBDI::Bindings::Python::VMInstance::addInstrumentedModule(): Expects a sting as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->addInstrumentedModule(PyString_AsString(module)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Add the executable address ranges of a module to the set of instrumented address ranges
       * using an address belonging to the module.
       *
       * @param[in] addr      An address contained by module's range.
       *
       * @return  True if at least one range was added to the instrumented ranges.
       */
      static PyObject* vm_addInstrumentedModuleFromAddr(PyObject* self, PyObject* addr) {
        if (!PyLong_Check(addr) && !PyInt_Check(addr))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addInstrumentedModuleFromAddr(): Expects an integer as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->addInstrumentedModuleFromAddr(PyLong_AsRword(addr)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Add an address range to the set of instrumented address ranges.
       *
       * @param[in] start  Start address of the range (included).
       * @param[in] end    End address of the range (excluded).
       */
      static PyObject* vm_addInstrumentedRange(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addInstrumentedRange(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addInstrumentedRange(): Expects an integer as second argument.");

        try {
          PyVMInstance_AsVMInstance(self)->addInstrumentedRange(PyLong_AsRword(start), PyLong_AsRword(end));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Register a callback event for every memory access matching the type bitfield made by an
       *  instruction.
       *
       * @param[in] type       A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
       * @param[in] cbk        A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addMemAccessCB(PyObject* self, PyObject* args) {
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &type, &function);

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemAccessCB(): Expects a MemoryAccessType as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemAccessCB(): Expects a function as second argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addMemAccessCB(static_cast<QBDI::MemoryAccessType>(PyInt_AsLong(type)),
                                                                     QBDI::Bindings::Python::trampoline,
                                                                     function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Add a virtual callback which is triggered for any memory access at a specific address 
       *  matching the access type. Virtual callbacks are called via callback forwarding by a
       *  gate callback triggered on every memory access. This incurs a high performance cost.
       *
       * @param[in] address  Code address which will trigger the callback.
       * @param[in] type     A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
       * @param[in] cbk      A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addMemAddrCB(PyObject* self, PyObject* args) {
        PyObject* addr     = nullptr;
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &addr, &type, &function);

        if (addr == nullptr || (!PyLong_Check(addr) && !PyInt_Check(addr)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemAddrCB(): Expects an integer as first argument.");

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemAddrCB(): Expects a MemoryAccessType as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemAddrCB(): Expects a function as third argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addMemAddrCB(PyLong_AsRword(addr),
                                                                   static_cast<QBDI::MemoryAccessType>(PyInt_AsLong(type)),
                                                                   QBDI::Bindings::Python::trampoline,
                                                                   function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Add a virtual callback which is triggered for any memory access in a specific address range
       *  matching the access type. Virtual callbacks are called via callback forwarding by a
       *  gate callback triggered on every memory access. This incurs a high performance cost.
       *
       * @param[in] start    Start of the address range which will trigger the callback.
       * @param[in] end      End of the address range which will trigger the callback.
       * @param[in] type     A mode bitfield: either QBDI_MEMORY_READ, QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
       * @param[in] cbk      A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addMemRangeCB(PyObject* self, PyObject* args) {
        PyObject* start    = nullptr;
        PyObject* end      = nullptr;
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOOO", &start, &end, &type, &function);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemRangeCB(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemRangeCB(): Expects an integer as second argument.");

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemRangeCB(): Expects a MemoryAccessType as third argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMemRangeCB(): Expects a function as fourth argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addMemRangeCB(PyLong_AsRword(start),
                                                                    PyLong_AsRword(end),
                                                                    static_cast<QBDI::MemoryAccessType>(PyInt_AsLong(type)),
                                                                    QBDI::Bindings::Python::trampoline,
                                                                    function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Register a callback event if the instruction matches the mnemonic.
       *
       * @param[in] mnemonic   Mnemonic to match.
       * @param[in] pos        Relative position of the event callback (QBDI_PREINST / QBDI_POSTINST).
       * @param[in] cbk        A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addMnemonicCB(PyObject* self, PyObject* args) {
        PyObject* mnemonic = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &mnemonic, &pos, &function);

        if (mnemonic == nullptr || !PyString_Check(mnemonic))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMnemonicCB(): Expects a string as first argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMnemonicCB(): Expects an InstPosition as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addMnemonicCB(): Expects a function as third argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addMnemonicCB(PyString_AsString(mnemonic),
                                                                    static_cast<QBDI::InstPosition>(PyInt_AsLong(pos)),
                                                                    QBDI::Bindings::Python::trampoline,
                                                                    function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Register a callback event for a specific VM event.
       *
       * @param[in] mask      A mask of VM event type which will trigger the callback.
       * @param[in] cbk       A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* vm_addVMEventCB(PyObject* self, PyObject* args) {
        PyObject* mask     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &mask, &function);

        if (mask == nullptr || (!PyLong_Check(mask) && !PyInt_Check(mask)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addVMEventCB(): Expects a VMEvent as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::addVMEventCB(): Expects a function as second argument.");

        try {
          retValue = PyVMInstance_AsVMInstance(self)->addVMEventCB(static_cast<QBDI::VMEvent>(PyInt_AsLong(mask)),
                                                                   QBDI::Bindings::Python::trampoline,
                                                                   function);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return PyLong_FromLong(retValue);
      }


      /*! Call a function using the DBI (and its current state).
       *
       * @param[in] function   Address of the function start instruction.
       * @param[in] args       The arguments as dictionary {0:arg0, 1:arg1, 2:arg2, ...}.
       * @param[in] ap         An stdarg va_list object.
       *
       * @return (True, retValue) if at least one block has been executed.
       */
      static PyObject* vm_call(PyObject* self, PyObject* args) {
        PyObject* function = nullptr;
        PyObject* fargs    = nullptr;
        PyObject* ret      = nullptr;
        QBDI::rword retVal = 0;         /* return value of the called function */
        bool retCall       = false;     /* return vaule of QBDI */

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &function, &fargs);

        if (function == nullptr || (!PyLong_Check(function) && !PyInt_Check(function)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::call(): Expects an integer (function address) as first argument.");

        if (fargs == nullptr || !PyDict_Check(fargs))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::call(): Expects a dictionary as second argument.");

        std::vector<QBDI::rword> cfargs;
        for (Py_ssize_t i = 0; i < PyDict_Size(fargs); i++) {
          PyObject* index = PyInt_FromLong(i);
          PyObject* item  = PyDict_GetItem(fargs, index);

          if (item == nullptr)
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::call(): key %ld not found", i);

          if (!PyLong_Check(item) && !PyInt_Check(item))
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::call(): Expects integers as dictionary contents.");

          cfargs.push_back(PyLong_AsRword(item));

          Py_DECREF(index);
        }

        try {
          retCall = PyVMInstance_AsVMInstance(self)->callA(&retVal,
                                                           PyLong_AsRword(function),
                                                           cfargs.size(),
                                                           cfargs.data());
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        /* Return a tuple: (call status, ret value) */
        ret = PyTuple_New(2);
        PyTuple_SetItem(ret, 0, PyBool_FromLong(retCall));
        PyTuple_SetItem(ret, 1, PyLong_FromLong(retVal));

        return ret;
      }


      /*! Clear the entire translation cache.
       *
       * @return None.
       */
      static PyObject* vm_clearAllCache(PyObject* self, PyObject* noarg) {
        try {
          PyVMInstance_AsVMInstance(self)->clearAllCache();
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Clear a specific address range from the translation cache.
       *
       * @param[in] start        Start of the address range to clear from the cache.
       * @param[in] end          End of the address range to clear from the cache.
       *
       * @return None.
       */
      static PyObject* vm_clearCache(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::clearCache(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::clearCache(): Expects an integer as second argument.");

        try {
          PyVMInstance_AsVMInstance(self)->clearCache(PyLong_AsRword(start), PyLong_AsRword(end));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Remove all the registered instrumentations.
       *
       * @return None.
       */
      static PyObject* vm_deleteAllInstrumentations(PyObject* self, PyObject* noarg) {
        try {
          PyVMInstance_AsVMInstance(self)->deleteAllInstrumentations();
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Remove an instrumentation.
       *
       * @param[in] id        The id of the instrumentation to remove.
       *
       * @return  True if instrumentation has been removed.
       */
      static PyObject* vm_deleteInstrumentation(PyObject* self, PyObject* id) {
        if (!PyLong_Check(id) && !PyInt_Check(id))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::deleteInstrumentation(): Expects an integer as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->deleteInstrumentation(PyLong_AsRword(id) & 0xffffffff) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Obtain the memory accesses made by the last executed basic block.
       *  Return None if the basic block made no memory access.
       *
       * @return An array of memory accesses made by the basic block.
       */
      static PyObject* vm_getBBMemoryAccess(PyObject* self, PyObject* noarg) {
        PyObject* ret = nullptr;
        size_t index  = 0;

        try {
          std::vector<QBDI::MemoryAccess> memoryAccesses = PyVMInstance_AsVMInstance(self)->getBBMemoryAccess();

          /* If there is no memory access, just return None */
          if (!memoryAccesses.size())
            Py_RETURN_NONE;

          /* Otherwise, return a list of MemoryAccess */
          ret = PyList_New(memoryAccesses.size());
          for (auto& memoryAccess : memoryAccesses) {
            PyList_SetItem(ret, index++, PyMemoryAccess(memoryAccess));
          }
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return ret;
      }


      /*! Obtain the current floating point register state.
       *
       * @return A structure containing the FPR state.
       */
      static PyObject* vm_getFPRState(PyObject* self, PyObject* noarg) {
        try {
          return PyFPRState(PyVMInstance_AsVMInstance(self)->getFPRState());
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Obtain the current general purpose register state.
       *
       * @return A structure containing the General Purpose Registers state.
       */
      static PyObject* vm_getGPRState(PyObject* self, PyObject* noarg) {
        try {
          return PyGPRState(PyVMInstance_AsVMInstance(self)->getGPRState());
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Obtain the analysis of an instruction metadata. Analysis results are cached in the VM.
      *  The validity of the returned pointer is only guaranteed until the end of the callback, else
      *  a deepcopy of the structure is required.
      *
      * @param[in] type         Properties to retrieve during analysis.
      *
      * @return A InstAnalysis structure containing the analysis result.
      */
      static PyObject* vm_getInstAnalysis(PyObject* self, PyObject* args) {
        PyObject* type = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|O", &type);

        try {
          if (type == nullptr)
            return PyInstAnalysis(PyVMInstance_AsVMInstance(self)->getInstAnalysis());

          else if (PyLong_Check(type) || PyInt_Check(type))
            return PyInstAnalysis(PyVMInstance_AsVMInstance(self)->getInstAnalysis(static_cast<QBDI::AnalysisType>(PyLong_AsLong(type))));

          else
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::getInstAnalysis(): Expects an AnalysisType as first argument or no argument.");
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Obtain the memory accesses made by the last executed instruction.
       *  Return Noneif the instruction made no memory access.
       *
       * @return An array of memory accesses made by the instruction.
       */
      static PyObject* vm_getInstMemoryAccess(PyObject* self, PyObject* noarg) {
        PyObject* ret = nullptr;
        size_t index  = 0;

        try {
          std::vector<QBDI::MemoryAccess> memoryAccesses = PyVMInstance_AsVMInstance(self)->getInstMemoryAccess();

          /* If there is no memory access, just return None */
          if (!memoryAccesses.size())
            Py_RETURN_NONE;

          /* Otherwise, return a list of MemoryAccess */
          ret = PyList_New(memoryAccesses.size());
          for (auto& memoryAccess : memoryAccesses) {
            PyList_SetItem(ret, index++, PyMemoryAccess(memoryAccess));
          }
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return ret;
      }


      /*! Adds all the executable memory maps to the instrumented range set.
       *
       * @return  True if at least one range was added to the instrumented ranges.
       */
      static PyObject* vm_instrumentAllExecutableMaps(PyObject* self, PyObject* noarg) {
        try {
          if (PyVMInstance_AsVMInstance(self)->instrumentAllExecutableMaps() == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Pre-cache a known basic block
       *
       *  @param[in]  pc           Start address of a basic block
       *
       * @return True if basic block has been inserted in cache.
       */
      static PyObject* vm_precacheBasicBlock(PyObject* self, PyObject* pc) {
        if (!PyLong_Check(pc) && !PyInt_Check(pc))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::precacheBasicBlock(): Expects an integer as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->precacheBasicBlock(PyLong_AsRword(pc)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Read a memory content from a base address.
       *
       * @param[in] address   Base address.
       * @param[in] size      Read size.
       *
       * @return Bytes of content.
       */
      static PyObject* vm_readMemory(PyObject* self, PyObject* args) {
        PyObject* address = nullptr;
        PyObject* size    = nullptr;
        PyObject* ret     = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &address, &size);

        if (address == nullptr || (!PyLong_Check(address) && !PyInt_Check(address)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::readMemory(): Expects an integer as first argument.");

        if (size == nullptr || (!PyLong_Check(size) && !PyInt_Check(size)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::readMemory(): Expects an integer as second argument.");

        try {
          ret = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(PyLong_AsRword(address)), PyLong_AsLong(size));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        return ret;
      }


      /*! Add instrumentation rules to log memory access using inline instrumentation and
       *  instruction shadows.
       *
       * @param[in] type      Memory mode bitfield to activate the logging for: either QBDI_MEMORY_READ,
       *                      QBDI_MEMORY_WRITE or both (QBDI_MEMORY_READ_WRITE).
       *
       * @return True if inline memory logging is supported, False if not or in case of error.
       */
      static PyObject* vm_recordMemoryAccess(PyObject* self, PyObject* type) {
        if (!PyLong_Check(type) && !PyInt_Check(type))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::recordMemoryAccess(): Expects a MemoryAccessType as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->recordMemoryAccess(static_cast<QBDI::MemoryAccessType>(PyLong_AsLong(type))) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Remove all instrumented ranges.
       *
       * @return None
       */
      static PyObject* vm_removeAllInstrumentedRanges(PyObject* self, PyObject* noarg) {
        try {
          PyVMInstance_AsVMInstance(self)->removeAllInstrumentedRanges();
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Remove the executable address ranges of a module from the set of instrumented address ranges.
       *
       * @param[in] name      The module's name.
       *
       * @return  True if at least one range was removed from the instrumented ranges.
       */
      static PyObject* vm_removeInstrumentedModule(PyObject* self, PyObject* module) {
        if (!PyString_Check(module))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::removeInstrumentedModule(): Expects a string as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->removeInstrumentedModule(PyString_AsString(module)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Remove the executable address ranges of a module from the set of instrumented address ranges.
       * using an address belonging to the module.
       *
       * @param[in] addr      An address contained by module's range.
       *
       * @return  True if at least one range was removed from the instrumented ranges.
       */
      static PyObject* vm_removeInstrumentedModuleFromAddr(PyObject* self, PyObject* addr) {
        if (!PyLong_Check(addr) && !PyInt_Check(addr))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::removeInstrumentedModuleFromAddr(): Expects an integer as first argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->removeInstrumentedModuleFromAddr(PyLong_AsRword(addr)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Remove an address range from the set of instrumented address ranges.
       *
       * @param[in] start     Start address of the range (included).
       * @param[in] end       End address of the range (excluded).
       *
       * @return None.
       */
      static PyObject* vm_removeInstrumentedRange(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::removeInstrumentedRange(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::removeInstrumentedRange(): Expects an integer as second argument.");

        try {
          PyVMInstance_AsVMInstance(self)->removeInstrumentedRange(PyLong_AsRword(start), PyLong_AsRword(end));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
        Py_RETURN_NONE;
      }


      /*! Start the execution by the DBI from a given address (and stop when another is reached).
       *
       * @param[in] start     Address of the first instruction to execute.
       * @param[in] stop      Stop the execution when this instruction is reached.
       *
       * @return  True if at least one block has been executed.
       */
      static PyObject* vm_run(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::run(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::run(): Expects an integer as second argument.");

        try {
          if (PyVMInstance_AsVMInstance(self)->run(PyLong_AsRword(start), PyLong_AsRword(end)) == true)
            return PyBool_FromLong(true);
          return PyBool_FromLong(false);
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Set the FPR state.
       *
       * @param[in] fprState A structure containing the FPR state.
       *
       * @return None
       */
      static PyObject* vm_setFPRState(PyObject* self, PyObject* arg) {
        if (!PyFPRState_Check(arg))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::setFPRState(): Expects a FPRState as first argument.");

        try {
          PyVMInstance_AsVMInstance(self)->setFPRState(PyFPRState_AsFPRState(arg));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        Py_RETURN_NONE;
      }


      /*! Set the GPR state.
       *
       * @param[in] gprState A structure containing the GPR state.
       *
       * @return None
       */
      static PyObject* vm_setGPRState(PyObject* self, PyObject* arg) {
        if (!PyGPRState_Check(arg))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::VMInstance::setGPRState(): Expects a GPRState as first argument.");

        try {
          PyVMInstance_AsVMInstance(self)->setGPRState(PyGPRState_AsGPRState(arg));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        Py_RETURN_NONE;
      }


      /* The VMInstance callbacks */
      PyMethodDef VMInstance_callbacks[] = {
        {"addCodeAddrCB",                     (PyCFunction)vm_addCodeAddrCB,                      METH_VARARGS,  "Register a callback for when a specific address is executed."},
        {"addCodeCB",                         (PyCFunction)vm_addCodeCB,                          METH_VARARGS,  "Register a callback event for a specific instruction event."},
        {"addCodeRangeCB",                    (PyCFunction)vm_addCodeRangeCB,                     METH_VARARGS,  "Register a callback for when a specific address range is executed."},
        {"addInstrumentedModule",             (PyCFunction)vm_addInstrumentedModule,              METH_O,        "Add the executable address ranges of a module to the set of instrumented address ranges."},
        {"addInstrumentedModuleFromAddr",     (PyCFunction)vm_addInstrumentedModuleFromAddr,      METH_O,        "Add the executable address ranges of a module to the set of instrumented address ranges using an address belonging to the module."},
        {"addInstrumentedRange",              (PyCFunction)vm_addInstrumentedRange,               METH_VARARGS,  "Add an address range to the set of instrumented address ranges."},
        {"addMemAccessCB",                    (PyCFunction)vm_addMemAccessCB,                     METH_VARARGS,  "Register a callback event for every memory access matching the type bitfield made by an instruction."},
        {"addMemAddrCB",                      (PyCFunction)vm_addMemAddrCB,                       METH_VARARGS,  "Add a virtual callback which is triggered for any memory access at a specific address matching the access type."},
        {"addMemRangeCB",                     (PyCFunction)vm_addMemRangeCB,                      METH_VARARGS,  "Add a virtual callback which is triggered for any memory access in a specific address range matching the access type."},
        {"addMnemonicCB",                     (PyCFunction)vm_addMnemonicCB,                      METH_VARARGS,  "Register a callback event if the instruction matches the mnemonic."},
        {"addVMEventCB",                      (PyCFunction)vm_addVMEventCB,                       METH_VARARGS,  "Register a callback event for a specific VM event."},
        {"call",                              (PyCFunction)vm_call,                               METH_VARARGS,  "Call a function using the DBI (and its current state)."},
        {"clearAllCache",                     (PyCFunction)vm_clearAllCache,                      METH_NOARGS,   "Clear the entire translation cache."},
        {"clearCache",                        (PyCFunction)vm_clearCache,                         METH_VARARGS,  "Clear a specific address range from the translation cache."},
        {"deleteAllInstrumentations",         (PyCFunction)vm_deleteAllInstrumentations,          METH_NOARGS,   "Remove all the registered instrumentations."},
        {"deleteInstrumentation",             (PyCFunction)vm_deleteInstrumentation,              METH_O,        "Remove an instrumentation."},
        {"getBBMemoryAccess",                 (PyCFunction)vm_getBBMemoryAccess,                  METH_NOARGS,   "Obtain the memory accesses made by the last executed basic block."},
        {"getFPRState",                       (PyCFunction)vm_getFPRState,                        METH_NOARGS,   "Obtain the current floating point register state."},
        {"getGPRState",                       (PyCFunction)vm_getGPRState,                        METH_NOARGS,   "Obtain the current general purpose register state."},
        {"getInstAnalysis",                   (PyCFunction)vm_getInstAnalysis,                    METH_VARARGS,  "Obtain the analysis of an instruction metadata."},
        {"getInstMemoryAccess",               (PyCFunction)vm_getInstMemoryAccess,                METH_NOARGS,   "Obtain the memory accesses made by the last executed instruction."},
        {"instrumentAllExecutableMaps",       (PyCFunction)vm_instrumentAllExecutableMaps,        METH_NOARGS,   "Adds all the executable memory maps to the instrumented range set."},
        {"precacheBasicBlock",                (PyCFunction)vm_precacheBasicBlock,                 METH_O,        "Pre-cache a known basic block"},
        {"readMemory",                        (PyCFunction)vm_readMemory,                         METH_VARARGS,  "Read a memory content from a base address."},
        {"recordMemoryAccess",                (PyCFunction)vm_recordMemoryAccess,                 METH_O,        "Add instrumentation rules to log memory access using inline instrumentation and instruction shadows."},
        {"removeAllInstrumentedRanges",       (PyCFunction)vm_removeAllInstrumentedRanges,        METH_NOARGS,   "Remove all instrumented ranges."},
        {"removeInstrumentedModule",          (PyCFunction)vm_removeInstrumentedModule,           METH_O,        "Remove the executable address ranges of a module from the set of instrumented address ranges."},
        {"removeInstrumentedModuleFromAddr",  (PyCFunction)vm_removeInstrumentedModuleFromAddr,   METH_O,        "Remove the executable address ranges of a module from the set of instrumented address ranges using an address belonging to the module."},
        {"removeInstrumentedRange",           (PyCFunction)vm_removeInstrumentedRange,            METH_VARARGS,  "Remove an address range from the set of instrumented address ranges."},
        {"run",                               (PyCFunction)vm_run,                                METH_VARARGS,  "Start the execution by the DBI from a given address (and stop when another is reached)."},
        {"setFPRState",                       (PyCFunction)vm_setFPRState,                        METH_O,        "Obtain the current floating point register state."},
        {"setGPRState",                       (PyCFunction)vm_setGPRState,                        METH_O,        "Obtain the current general purpose register state."},
        {nullptr,                             nullptr,                                            0,             nullptr}
      };


      /* Description of the python representation of a VMInstance */
      PyTypeObject VMInstance_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                                          /* ob_size */
        "VMInstance",                               /* tp_name */
        sizeof(VMInstance_Object),                  /* tp_basicsize */
        0,                                          /* tp_itemsize */
        VMInstance_dealloc,                         /* tp_dealloc */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_compare */
        0,                                          /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        0,                                          /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,                         /* tp_flags */
        "VMInstance objects",                       /* tp_doc */
        0,                                          /* tp_traverse */
        0,                                          /* tp_clear */
        0,                                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        0,                                          /* tp_iter */
        0,                                          /* tp_iternext */
        VMInstance_callbacks,                       /* tp_methods */
        0,                                          /* tp_members */
        0,                                          /* tp_getset */
        0,                                          /* tp_base */
        0,                                          /* tp_dict */
        0,                                          /* tp_descr_get */
        0,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        0,                                          /* tp_init */
        0,                                          /* tp_alloc */
        0,                                          /* tp_new */
        0,                                          /* tp_free */
        0,                                          /* tp_is_gc */
        0,                                          /* tp_bases */
        0,                                          /* tp_mro */
        0,                                          /* tp_cache */
        0,                                          /* tp_subclasses */
        0,                                          /* tp_weaklist */
        0,                                          /* tp_del */
        0                                           /* tp_version_tag */
      };


      static PyObject* PyVMInstance(QBDI::VMInstanceRef vm) {
        VMInstance_Object* object;

        PyType_Ready(&VMInstance_Type);
        object = PyObject_NEW(VMInstance_Object, &VMInstance_Type);
        if (object != NULL)
          object->vm = vm;

        return (PyObject*)object;
      }


      /*! FPRState constructor.
       *
       * @param[in] fprstate  The FPR state structure.
       *
       * @return A structure containing the FPR state.
       */
      static PyObject* pyqbdi_FPRState(PyObject* self, PyObject* args) {
        PyObject* fpr = nullptr;

        /* Extract argument */
        PyArg_ParseTuple(args, "|O", &fpr);

        try {
          if (fpr == nullptr)
            return QBDI::Bindings::Python::PyFPRState();

          else if (fpr != nullptr && PyFPRState_Check(fpr))
            return QBDI::Bindings::Python::PyFPRState(PyFPRState_AsFPRState(fpr));

          else
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::FPRState(): Expects no argument or only one FPRState as first argument.");
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! GPRState constructor.
       *
       * @param[in] gprstate  The GPR state structure.
       *
       * @return A structure containing the GPR state.
       */
      static PyObject* pyqbdi_GPRState(PyObject* self, PyObject* args) {
        PyObject* gpr = nullptr;

        /* Extract argument */
        PyArg_ParseTuple(args, "|O", &gpr);

        try {
          if (gpr == nullptr)
            return QBDI::Bindings::Python::PyGPRState();

          else if (gpr != nullptr && PyGPRState_Check(gpr))
            return QBDI::Bindings::Python::PyGPRState(PyGPRState_AsGPRState(gpr));

          else
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::GPRState(): Expects no argument or only one GPRState as first argument.");
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /* The pyqbdi callbacks */
      PyMethodDef pyqbdiCallbacks[] = {
        {"FPRState",  (PyCFunction)pyqbdi_FPRState,  METH_VARARGS,  "FPRState constructor."},
        {"GPRState",  (PyCFunction)pyqbdi_GPRState,  METH_VARARGS,  "GPRState constructor."},
        {nullptr,     nullptr,                       0,             nullptr}
      };


      /* Python entry point */
      static void init(void) {
        /* Initialize python */
        Py_Initialize();

        /* Initialize sys.argv  */
        if (QBDI::Bindings::Python::argc && QBDI::Bindings::Python::argv)
          PySys_SetArgv(QBDI::Bindings::Python::argc, QBDI::Bindings::Python::argv);
        else
          PySys_SetArgvEx(0, nullptr, 0);

        QBDI::Bindings::Python::module = Py_InitModule("pyqbdi", QBDI::Bindings::Python::pyqbdiCallbacks);
        if (QBDI::Bindings::Python::module == nullptr) {
          std::cerr << "Failed to initialize the pyqbdi bindings" << std::endl;
          PyErr_Print();
          exit(1);
        }

        /* Initialize enums */
        PyModule_AddObject(QBDI::Bindings::Python::module, "ANALYSIS_DISASSEMBLY",  PyInt_FromLong(QBDI::ANALYSIS_DISASSEMBLY));
        PyModule_AddObject(QBDI::Bindings::Python::module, "ANALYSIS_INSTRUCTION",  PyInt_FromLong(QBDI::ANALYSIS_INSTRUCTION));
        PyModule_AddObject(QBDI::Bindings::Python::module, "ANALYSIS_OPERANDS",     PyInt_FromLong(QBDI::ANALYSIS_OPERANDS));
        PyModule_AddObject(QBDI::Bindings::Python::module, "ANALYSIS_SYMBOL",       PyInt_FromLong(QBDI::ANALYSIS_SYMBOL));
        PyModule_AddObject(QBDI::Bindings::Python::module, "BASIC_BLOCK_ENTRY",     PyInt_FromLong(QBDI::BASIC_BLOCK_ENTRY));
        PyModule_AddObject(QBDI::Bindings::Python::module, "BASIC_BLOCK_EXIT",      PyInt_FromLong(QBDI::BASIC_BLOCK_EXIT));
        PyModule_AddObject(QBDI::Bindings::Python::module, "BASIC_BLOCK_NEW",       PyInt_FromLong(QBDI::BASIC_BLOCK_NEW));
        PyModule_AddObject(QBDI::Bindings::Python::module, "BREAK_TO_VM",           PyInt_FromLong(QBDI::BREAK_TO_VM));
        PyModule_AddObject(QBDI::Bindings::Python::module, "CONTINUE",              PyInt_FromLong(QBDI::CONTINUE));
        PyModule_AddObject(QBDI::Bindings::Python::module, "EXEC_TRANSFER_CALL",    PyInt_FromLong(QBDI::EXEC_TRANSFER_CALL));
        PyModule_AddObject(QBDI::Bindings::Python::module, "INVALID_EVENTID",       PyInt_FromLong(QBDI::INVALID_EVENTID));
        PyModule_AddObject(QBDI::Bindings::Python::module, "MEMORY_READ_WRITE",     PyInt_FromLong(QBDI::MEMORY_READ_WRITE));
        PyModule_AddObject(QBDI::Bindings::Python::module, "MEMORY_WRITE",          PyInt_FromLong(QBDI::MEMORY_WRITE));
        PyModule_AddObject(QBDI::Bindings::Python::module, "OPERAND_GPR",           PyInt_FromLong(QBDI::OPERAND_GPR));
        PyModule_AddObject(QBDI::Bindings::Python::module, "OPERAND_IMM",           PyInt_FromLong(QBDI::OPERAND_IMM));
        PyModule_AddObject(QBDI::Bindings::Python::module, "OPERAND_INVALID",       PyInt_FromLong(QBDI::OPERAND_INVALID));
        PyModule_AddObject(QBDI::Bindings::Python::module, "OPERAND_PRED",          PyInt_FromLong(QBDI::OPERAND_PRED));
        PyModule_AddObject(QBDI::Bindings::Python::module, "POSTINST",              PyInt_FromLong(QBDI::POSTINST));
        PyModule_AddObject(QBDI::Bindings::Python::module, "PREINST",               PyInt_FromLong(QBDI::PREINST));
        PyModule_AddObject(QBDI::Bindings::Python::module, "REGISTER_READ",         PyInt_FromLong(QBDI::REGISTER_READ));
        PyModule_AddObject(QBDI::Bindings::Python::module, "REGISTER_READ_WRITE",   PyInt_FromLong(QBDI::REGISTER_READ_WRITE));
        PyModule_AddObject(QBDI::Bindings::Python::module, "REGISTER_WRITE",        PyInt_FromLong(QBDI::REGISTER_WRITE));
        PyModule_AddObject(QBDI::Bindings::Python::module, "SEQUENCE_ENTRY",        PyInt_FromLong(QBDI::SEQUENCE_ENTRY));
        PyModule_AddObject(QBDI::Bindings::Python::module, "SEQUENCE_EXIT",         PyInt_FromLong(QBDI::SEQUENCE_EXIT));
        PyModule_AddObject(QBDI::Bindings::Python::module, "SIGNAL",                PyInt_FromLong(QBDI::SIGNAL));
        PyModule_AddObject(QBDI::Bindings::Python::module, "STOP",                  PyInt_FromLong(QBDI::STOP));
        PyModule_AddObject(QBDI::Bindings::Python::module, "SYSCALL_ENTRY",         PyInt_FromLong(QBDI::SYSCALL_ENTRY));
        PyModule_AddObject(QBDI::Bindings::Python::module, "SYSCALL_EXIT",          PyInt_FromLong(QBDI::SYSCALL_EXIT));
      }


      static bool execScript(const char* fileName, VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
        #if defined(__unix__) || defined(__APPLE__)
        /* On some Linux distro, we must load libpython to successfully load all others modules */
        void* handle = dlopen(PYTHON_LIBRARIES, RTLD_LAZY | RTLD_GLOBAL);
        if (!handle)
          throw std::runtime_error("QBDI::binding::python::execScript(): Cannot load the Python library.");
        #endif

        FILE* fd = nullptr;
        int err = fopen_s(&fd, fileName, "r");
        if (err != 0)
          throw std::runtime_error("QBDI::binding::python::execScript(): Script file can't be found.");

        /* Parse and execute the file */
        PyRun_SimpleFile(fd, fileName);

        /* Get the pyqbdipreload_on_run global function */
        PyObject* main = PyImport_ImportModule("__main__");
        PyObject* dict = PyModule_GetDict(main);
        PyObject* pyqbdipreload_on_run = PyDict_GetItemString(dict, "pyqbdipreload_on_run");

        /* Free unused resources */
        Py_DECREF(main);
        Py_DECREF(dict);

        /* Execute the pyqbdipreload_on_run function if exists */
        if (pyqbdipreload_on_run != nullptr) {
          if (PyCallable_Check(pyqbdipreload_on_run)) {
            /* Create function arguments */
            PyObject* args = PyTuple_New(3);
            PyTuple_SetItem(args, 0, QBDI::Bindings::Python::PyVMInstance(vm));
            PyTuple_SetItem(args, 1, PyLong_FromLong(start));
            PyTuple_SetItem(args, 2, PyLong_FromLong(stop));

            /* Call the function and check the return value */
            PyObject* ret = PyObject_CallObject(pyqbdipreload_on_run, args);
            Py_DECREF(args);
            if (ret == nullptr) {
              PyErr_Print();
              exit(1);
            }
          }
        }

        fclose(fd);
        return true;
      }

      /* Undef all defined macros of this scope */
      #undef PyFPRState_AsFPRState
      #undef PyFPRState_Check
      #undef PyGPRState_AsGPRState
      #undef PyGPRState_Check
      #undef PyInstAnalysis_AsInstAnalysis
      #undef PyInstAnalysis_Check
      #undef PyMemoryAccess_AsMemoryAccess
      #undef PyMemoryAccess_Check

    }; // Python
  }; // Bindings
}; // QBDI


int QBDI::qbdipreload_on_start(void* main) {
  return QBDIPRELOAD_NOT_HANDLED;
}


int QBDI::qbdipreload_on_premain(void* gprCtx, void* fpuCtx) {
  return QBDIPRELOAD_NOT_HANDLED;
}


int QBDI::qbdipreload_on_main(int argc, char** argv) {
  QBDI::Bindings::Python::argc = argc;
  QBDI::Bindings::Python::argv = argv;

  return QBDIPRELOAD_NOT_HANDLED;
}


int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
  const char* fileTool = std::getenv("PYQBDI_TOOL");

  if (fileTool == nullptr) {
    std::cerr << "QBDI::qbdipreload_on_run(): PYQBDI_TOOL not found !" << std::endl;
    exit(1);
  }

  try {
    QBDI::Bindings::Python::init();
    QBDI::Bindings::Python::execScript(fileTool, vm, start, stop);
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return QBDIPRELOAD_NO_ERROR;
}


int QBDI::qbdipreload_on_exit(int status) {
  return QBDIPRELOAD_NO_ERROR;
}
