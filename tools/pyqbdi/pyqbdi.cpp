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
**   $ LD_PRELOAD=./libpyqbdi.so PYQBDI_TOOL=./tool.py /usr/bin/id
**
**
** Exemple of tool:
**
**   import pyqbdi
**
**   def mycb(inst, gpr, fpr):
**       print "0x%x: %s" %(inst.address, inst.disassembly)
**       return pyqbdi.CONTINUE
**
**   if __name__ == '__main__':
**       pyqbdi.addCodeCB(pyqbdi.PREINST, mycb)
**       pyqbdi.run(pyqbdi.start, pyqbdi.stop)
*/

#include <Python.h>
#include <longintrepr.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <list>

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

      /* The Virtual Machine reference */
      QBDI::VMInstanceRef vm = nullptr;

      /* The start address of the DBI */
      QBDI::rword start = 0;

      /* The stop address of the DBI */
      QBDI::rword stop = 0;

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


      /* PyInstAnalysis destructor */
      static void InstAnalysis_dealloc(PyObject* self) {
        std::cout << std::flush;
        free(PyInstAnalysis_AsInstAnalysis(self));
        Py_DECREF(self);
      }


      /* InstAnalysis attributes */
      static PyObject* InstAnalysis_getattro(PyObject* self, PyObject* name) {
        try {
          if (std::string(PyString_AsString(name)) == "mnemonic")
            return PyString_FromString(PyInstAnalysis_AsInstAnalysis(self)->mnemonic);

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

          else if (std::string(PyString_AsString(name)) == "disassembly")
            return PyString_FromString(PyInstAnalysis_AsInstAnalysis(self)->disassembly);

          else if (std::string(PyString_AsString(name)) == "numOperands")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->numOperands);

          else if (std::string(PyString_AsString(name)) == "symbol")
            return PyString_FromString(PyInstAnalysis_AsInstAnalysis(self)->symbol);

          else if (std::string(PyString_AsString(name)) == "symbolOffset")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->symbolOffset);

          else if (std::string(PyString_AsString(name)) == "analysisType")
            return PyLong_FromLong(PyInstAnalysis_AsInstAnalysis(self)->analysisType);
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
              PyGPRState_AsGPRState(self)->rax = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rbx")
              PyGPRState_AsGPRState(self)->rbx = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rcx")
              PyGPRState_AsGPRState(self)->rcx = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rdx")
              PyGPRState_AsGPRState(self)->rdx = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rsi")
              PyGPRState_AsGPRState(self)->rsi = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rdi")
              PyGPRState_AsGPRState(self)->rdi = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r8")
              PyGPRState_AsGPRState(self)->r8 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r9")
              PyGPRState_AsGPRState(self)->r9 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r10")
              PyGPRState_AsGPRState(self)->r10 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r11")
              PyGPRState_AsGPRState(self)->r11 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r12")
              PyGPRState_AsGPRState(self)->r12 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r13")
              PyGPRState_AsGPRState(self)->r13 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r14")
              PyGPRState_AsGPRState(self)->r14 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r15")
              PyGPRState_AsGPRState(self)->r15 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rbp")
              PyGPRState_AsGPRState(self)->rbp = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rsp")
              PyGPRState_AsGPRState(self)->rsp = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "rip")
              PyGPRState_AsGPRState(self)->rip = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "eflags")
              PyGPRState_AsGPRState(self)->eflags = PyLong_AsLong(item);

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif

          #if defined(QBDI_ARCH_ARM)
            if (std::string(PyString_AsString(name)) == "r0")
              PyGPRState_AsGPRState(self)->r0 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r1")
              PyGPRState_AsGPRState(self)->r1 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r2")
              PyGPRState_AsGPRState(self)->r2 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r3")
              PyGPRState_AsGPRState(self)->r3 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r4")
              PyGPRState_AsGPRState(self)->r4 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r5")
              PyGPRState_AsGPRState(self)->r5 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r6")
              PyGPRState_AsGPRState(self)->r6 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r7")
              PyGPRState_AsGPRState(self)->r7 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r8")
              PyGPRState_AsGPRState(self)->r8 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r9")
              PyGPRState_AsGPRState(self)->r9 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r10")
              PyGPRState_AsGPRState(self)->r10 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "r12")
              PyGPRState_AsGPRState(self)->r12 = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "fp")
              PyGPRState_AsGPRState(self)->fp = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "sp")
              PyGPRState_AsGPRState(self)->sp = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "lr")
              PyGPRState_AsGPRState(self)->lr = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "pc")
              PyGPRState_AsGPRState(self)->pc = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "cpsr")
              PyGPRState_AsGPRState(self)->cpsr = PyLong_AsLong(item);

            else
              return PyObject_GenericSetAttr(self, name, item);
          #endif
        }
        catch (const std::exception& e) {
          PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::GPRState::setattro(): %s", e.what());
          return -1;
        }

        QBDI::Bindings::Python::vm->setGPRState(PyGPRState_AsGPRState(self));
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
              PyFPRState_AsFPRState(self)->ftw = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "fop")
              PyFPRState_AsFPRState(self)->fop = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "ip")
              PyFPRState_AsFPRState(self)->ip = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "cs")
              PyFPRState_AsFPRState(self)->cs = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "dp")
              PyFPRState_AsFPRState(self)->dp = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "ds")
              PyFPRState_AsFPRState(self)->ds = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "mxcsr")
              PyFPRState_AsFPRState(self)->mxcsr = PyLong_AsLong(item);

            else if (std::string(PyString_AsString(name)) == "mxcsrmask")
              PyFPRState_AsFPRState(self)->mxcsrmask = PyLong_AsLong(item);

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

        QBDI::Bindings::Python::vm->setFPRState(PyFPRState_AsFPRState(self));
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
      static QBDI::VMAction trampoline(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *function) {
        const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY | QBDI::ANALYSIS_SYMBOL);

        /* Create function arguments */
        PyObject* args = PyTuple_New(3);
        PyTuple_SetItem(args, 0, QBDI::Bindings::Python::PyInstAnalysis(instAnalysis));
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
      static QBDI::VMAction trampoline(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *function) {
        return trampoline(vm, gprState, fprState, function);
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


      /*! Register a callback for when a specific address is executed.
       *
       * @param[in] address   Code address which will trigger the callback.
       * @param[in] pos       Relative position of the callback (QBDI_PREINST / QBDI_POSTINST).
       * @param[in] cbk       A function pointer to the callback.
       *
       * @return The id of the registered instrumentation (or QBDI::INVALID_EVENTID
       * in case of failure).
       */
      static PyObject* pyqbdi_addCodeAddrCB(PyObject* self, PyObject* args) {
        PyObject* addr     = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &addr, &pos, &function);

        if (addr == nullptr || (!PyLong_Check(addr) && !PyInt_Check(addr)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeAddrCB(): Expects an integer as first argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeAddrCB(): Expects an InstPosition as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeAddrCB(): Expects a function as third argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addCodeAddrCB(PyLong_AsLong(addr),
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
      static PyObject* pyqbdi_addCodeCB(PyObject* self, PyObject* args) {
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &pos, &function);

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeCB(): Expects an InstPosition as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeCB(): Expects a function as second argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addCodeCB(static_cast<QBDI::InstPosition>(PyInt_AsLong(pos)),
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
      static PyObject* pyqbdi_addCodeRangeCB(PyObject* self, PyObject* args) {
        PyObject* start    = nullptr;
        PyObject* end      = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOOO", &start, &end, &pos, &function);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeRangeCB(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeRangeCB(): Expects an integer as second argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeRangeCB(): Expects an InstPosition as thrid argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addCodeRangeCB(): Expects a function as fourth argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addCodeRangeCB(PyLong_AsLong(start),
                                                                PyLong_AsLong(end),
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
      static PyObject* pyqbdi_addInstrumentedModule(PyObject* self, PyObject* module) {
        if (!PyString_Check(module))
          return PyErr_Format(PyExc_TypeError, "QBDI::Bindings::Python::addInstrumentedModule(): Expects a sting as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->addInstrumentedModule(PyString_AsString(module)) == true)
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
      static PyObject* pyqbdi_addInstrumentedModuleFromAddr(PyObject* self, PyObject* addr) {
        if (!PyLong_Check(addr) && !PyInt_Check(addr))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addInstrumentedModuleFromAddr(): Expects an integer as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->addInstrumentedModuleFromAddr(PyLong_AsLong(addr)) == true)
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
      static PyObject* pyqbdi_addInstrumentedRange(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addInstrumentedRange(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addInstrumentedRange(): Expects an integer as second argument.");

        try {
          QBDI::Bindings::Python::vm->addInstrumentedRange(PyLong_AsLong(start), PyLong_AsLong(end));
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
      static PyObject* pyqbdi_addMemAccessCB(PyObject* self, PyObject* args) {
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &type, &function);

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemAccessCB(): Expects a MemoryAccessType as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemAccessCB(): Expects a function as second argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addMemAccessCB(static_cast<QBDI::MemoryAccessType>(PyInt_AsLong(type)),
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
      static PyObject* pyqbdi_addMemAddrCB(PyObject* self, PyObject* args) {
        PyObject* addr     = nullptr;
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &addr, &type, &function);

        if (addr == nullptr || (!PyLong_Check(addr) && !PyInt_Check(addr)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemAddrCB(): Expects an integer as first argument.");

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemAddrCB(): Expects a MemoryAccessType as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemAddrCB(): Expects a function as third argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addMemAddrCB(PyLong_AsLong(addr),
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
      static PyObject* pyqbdi_addMemRangeCB(PyObject* self, PyObject* args) {
        PyObject* start    = nullptr;
        PyObject* end      = nullptr;
        PyObject* type     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOOO", &start, &end, &type, &function);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemRangeCB(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemRangeCB(): Expects an integer as second argument.");

        if (type == nullptr || (!PyLong_Check(type) && !PyInt_Check(type)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemRangeCB(): Expects a MemoryAccessType as third argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMemRangeCB(): Expects a function as fourth argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addMemRangeCB(PyLong_AsLong(start),
                                                               PyLong_AsLong(end),
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
      static PyObject* pyqbdi_addMnemonicCB(PyObject* self, PyObject* args) {
        PyObject* mnemonic = nullptr;
        PyObject* pos      = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OOO", &mnemonic, &pos, &function);

        if (mnemonic == nullptr || !PyString_Check(mnemonic))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMnemonicCB(): Expects a string as first argument.");

        if (pos == nullptr || (!PyLong_Check(pos) && !PyInt_Check(pos)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMnemonicCB(): Expects an InstPosition as second argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addMnemonicCB(): Expects a function as third argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addMnemonicCB(PyString_AsString(mnemonic),
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
      static PyObject* pyqbdi_addVMEventCB(PyObject* self, PyObject* args) {
        PyObject* mask     = nullptr;
        PyObject* function = nullptr;
        uint32_t retValue  = QBDI::INVALID_EVENTID;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &mask, &function);

        if (mask == nullptr || (!PyLong_Check(mask) && !PyInt_Check(mask)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addVMEventCB(): Expects a VMEvent as first argument.");

        if (function == nullptr || !PyCallable_Check(function))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::addVMEventCB(): Expects a function as second argument.");

        try {
          retValue = QBDI::Bindings::Python::vm->addVMEventCB(static_cast<QBDI::VMEvent>(PyInt_AsLong(mask)),
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
      static PyObject* pyqbdi_call(PyObject* self, PyObject* args) {
        PyObject* function = nullptr;
        PyObject* fargs    = nullptr;
        PyObject* ret      = nullptr;
        QBDI::rword retVal = 0;         /* return value of the called function */
        bool retCall       = false;     /* return vaule of QBDI */

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &function, &fargs);

        if (function == nullptr || (!PyLong_Check(function) && !PyInt_Check(function)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::call(): Expects an integer (function address) as first argument.");

        if (fargs == nullptr || !PyDict_Check(fargs))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::call(): Expects a dictionary as second argument.");

        std::vector<QBDI::rword> cfargs;
        for (Py_ssize_t i = 0; i < PyDict_Size(fargs); i++) {
          PyObject* index = PyInt_FromLong(i);
          PyObject* item  = PyDict_GetItem(fargs, index);

          if (item == nullptr)
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::call(): key %ld not found", i);

          if (!PyLong_Check(item) && !PyInt_Check(item))
            return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::call(): Expects integers as dictionary contents.");

          cfargs.push_back(reinterpret_cast<QBDI::rword>(PyLong_AsUnsignedLong(item)));

          Py_DECREF(index);
        }

        try {
          retCall = QBDI::Bindings::Python::vm->callA(&retVal,
                                                      reinterpret_cast<QBDI::rword>(PyLong_AsUnsignedLong(function)),
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
      static PyObject* pyqbdi_clearAllCache(PyObject* self, PyObject* noarg) {
        try {
          QBDI::Bindings::Python::vm->clearAllCache();
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
      static PyObject* pyqbdi_clearCache(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::clearCache(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::clearCache(): Expects an integer as second argument.");

        try {
          QBDI::Bindings::Python::vm->clearCache(PyLong_AsLong(start), PyLong_AsLong(end));
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
      static PyObject* pyqbdi_deleteAllInstrumentations(PyObject* self, PyObject* noarg) {
        try {
          QBDI::Bindings::Python::vm->deleteAllInstrumentations();
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
      static PyObject* pyqbdi_deleteInstrumentation(PyObject* self, PyObject* id) {
        if (!PyLong_Check(id) && !PyInt_Check(id))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::deleteInstrumentation(): Expects an integer as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->deleteInstrumentation(PyLong_AsLong(id)) == true)
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
      static PyObject* pyqbdi_getBBMemoryAccess(PyObject* self, PyObject* noarg) {
        PyObject* ret = nullptr;
        size_t index  = 0;

        try {
          std::vector<QBDI::MemoryAccess> memoryAccesses = QBDI::Bindings::Python::vm->getBBMemoryAccess();

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
      static PyObject* pyqbdi_getFPRState(PyObject* self, PyObject* noarg) {
        try {
          return PyFPRState(QBDI::Bindings::Python::vm->getFPRState());
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }
      }


      /*! Obtain the current general purpose register state.
       *
       * @return A structure containing the General Purpose Registers state.
       */
      static PyObject* pyqbdi_getGPRState(PyObject* self, PyObject* noarg) {
        try {
          return PyGPRState(QBDI::Bindings::Python::vm->getGPRState());
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
      static PyObject* pyqbdi_getInstAnalysis(PyObject* self, PyObject* type) {
        if (!PyLong_Check(type) && !PyInt_Check(type))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::getInstAnalysis(): Expects an AnalysisType as first argument.");

        try {
          return PyInstAnalysis(QBDI::Bindings::Python::vm->getInstAnalysis(static_cast<QBDI::AnalysisType>(PyLong_AsLong(type))));
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
      static PyObject* pyqbdi_getInstMemoryAccess(PyObject* self, PyObject* noarg) {
        PyObject* ret = nullptr;
        size_t index  = 0;

        try {
          std::vector<QBDI::MemoryAccess> memoryAccesses = QBDI::Bindings::Python::vm->getInstMemoryAccess();

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
      static PyObject* pyqbdi_instrumentAllExecutableMaps(PyObject* self, PyObject* noarg) {
        try {
          if (QBDI::Bindings::Python::vm->instrumentAllExecutableMaps() == true)
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
      static PyObject* pyqbdi_precacheBasicBlock(PyObject* self, PyObject* pc) {
        if (!PyLong_Check(pc) && !PyInt_Check(pc))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::precacheBasicBlock(): Expects an integer as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->precacheBasicBlock(PyLong_AsLong(pc)) == true)
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
      static PyObject* pyqbdi_readMemory(PyObject* self, PyObject* args) {
        PyObject* address = nullptr;
        PyObject* size    = nullptr;
        PyObject* ret     = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &address, &size);

        if (address == nullptr || (!PyLong_Check(address) && !PyInt_Check(address)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::readMemory(): Expects an integer as first argument.");

        if (size == nullptr || (!PyLong_Check(size) && !PyInt_Check(size)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::readMemory(): Expects an integer as second argument.");

        try {
          ret = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(PyLong_AsLong(address)), PyLong_AsLong(size));
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
      static PyObject* pyqbdi_recordMemoryAccess(PyObject* self, PyObject* type) {
        if (!PyLong_Check(type) && !PyInt_Check(type))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::recordMemoryAccess(): Expects a MemoryAccessType as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->recordMemoryAccess(static_cast<QBDI::MemoryAccessType>(PyLong_AsLong(type))) == true)
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
      static PyObject* pyqbdi_removeAllInstrumentedRanges(PyObject* self, PyObject* noarg) {
        try {
          QBDI::Bindings::Python::vm->removeAllInstrumentedRanges();
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
      static PyObject* pyqbdi_removeInstrumentedModule(PyObject* self, PyObject* module) {
        if (!PyString_Check(module))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::removeInstrumentedModule(): Expects a string as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->removeInstrumentedModule(PyString_AsString(module)) == true)
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
      static PyObject* pyqbdi_removeInstrumentedModuleFromAddr(PyObject* self, PyObject* addr) {
        if (!PyLong_Check(addr) && !PyInt_Check(addr))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::removeInstrumentedModuleFromAddr(): Expects an integer as first argument.");

        try {
          if (QBDI::Bindings::Python::vm->removeInstrumentedModuleFromAddr(PyLong_AsLong(addr)) == true)
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
      static PyObject* pyqbdi_removeInstrumentedRange(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::removeInstrumentedRange(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::removeInstrumentedRange(): Expects an integer as second argument.");

        try {
          QBDI::Bindings::Python::vm->removeInstrumentedRange(PyLong_AsLong(start), PyLong_AsLong(end));
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
      static PyObject* pyqbdi_run(PyObject* self, PyObject* args) {
        PyObject* start  = nullptr;
        PyObject* end    = nullptr;

        /* Extract arguments */
        PyArg_ParseTuple(args, "|OO", &start, &end);

        if (start == nullptr || (!PyLong_Check(start) && !PyInt_Check(start)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::run(): Expects an integer as first argument.");

        if (end == nullptr || (!PyLong_Check(end) && !PyInt_Check(end)))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::run(): Expects an integer as second argument.");

        try {
          if (QBDI::Bindings::Python::vm->run(PyLong_AsLong(start), PyLong_AsLong(end)) == true)
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
      static PyObject* pyqbdi_setFPRState(PyObject* self, PyObject* arg) {
        if (!PyFPRState_Check(arg))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::setFPRState(): Expects a FPRState as first argument.");

        try {
          QBDI::Bindings::Python::vm->setFPRState(PyFPRState_AsFPRState(arg));
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
      static PyObject* pyqbdi_setGPRState(PyObject* self, PyObject* arg) {
        if (!PyGPRState_Check(arg))
          return PyErr_Format(PyExc_TypeError, "QBDI:Bindings::Python::setGPRState(): Expects a GPRState as first argument.");

        try {
          QBDI::Bindings::Python::vm->setGPRState(PyGPRState_AsGPRState(arg));
        }
        catch (const std::exception& e) {
          return PyErr_Format(PyExc_TypeError, "%s", e.what());
        }

        Py_RETURN_NONE;
      }


      /* The pyqbdi callbacks */
      PyMethodDef pyqbdiCallbacks[] = {
        {"FPRState",                          (PyCFunction)pyqbdi_FPRState,                           METH_VARARGS,  "FPRState constructor."},
        {"GPRState",                          (PyCFunction)pyqbdi_GPRState,                           METH_VARARGS,  "GPRState constructor."},
        {"addCodeAddrCB",                     (PyCFunction)pyqbdi_addCodeAddrCB,                      METH_VARARGS,  "Register a callback for when a specific address is executed."},
        {"addCodeCB",                         (PyCFunction)pyqbdi_addCodeCB,                          METH_VARARGS,  "Register a callback event for a specific instruction event."},
        {"addCodeRangeCB",                    (PyCFunction)pyqbdi_addCodeRangeCB,                     METH_VARARGS,  "Register a callback for when a specific address range is executed."},
        {"addInstrumentedModule",             (PyCFunction)pyqbdi_addInstrumentedModule,              METH_O,        "Add the executable address ranges of a module to the set of instrumented address ranges."},
        {"addInstrumentedModuleFromAddr",     (PyCFunction)pyqbdi_addInstrumentedModuleFromAddr,      METH_O,        "Add the executable address ranges of a module to the set of instrumented address ranges using an address belonging to the module."},
        {"addInstrumentedRange",              (PyCFunction)pyqbdi_addInstrumentedRange,               METH_VARARGS,  "Add an address range to the set of instrumented address ranges."},
        {"addMemAccessCB",                    (PyCFunction)pyqbdi_addMemAccessCB,                     METH_VARARGS,  "Register a callback event for every memory access matching the type bitfield made by an instruction."},
        {"addMemAddrCB",                      (PyCFunction)pyqbdi_addMemAddrCB,                       METH_VARARGS,  "Add a virtual callback which is triggered for any memory access at a specific address matching the access type."},
        {"addMemRangeCB",                     (PyCFunction)pyqbdi_addMemRangeCB,                      METH_VARARGS,  "Add a virtual callback which is triggered for any memory access in a specific address range matching the access type."},
        {"addMnemonicCB",                     (PyCFunction)pyqbdi_addMnemonicCB,                      METH_VARARGS,  "Register a callback event if the instruction matches the mnemonic."},
        {"addVMEventCB",                      (PyCFunction)pyqbdi_addVMEventCB,                       METH_VARARGS,  "Register a callback event for a specific VM event."},
        {"call",                              (PyCFunction)pyqbdi_call,                               METH_VARARGS,  "Call a function using the DBI (and its current state)."},
        {"clearAllCache",                     (PyCFunction)pyqbdi_clearAllCache,                      METH_NOARGS,   "Clear the entire translation cache."},
        {"clearCache",                        (PyCFunction)pyqbdi_clearCache,                         METH_VARARGS,  "Clear a specific address range from the translation cache."},
        {"deleteAllInstrumentations",         (PyCFunction)pyqbdi_deleteAllInstrumentations,          METH_NOARGS,   "Remove all the registered instrumentations."},
        {"deleteInstrumentation",             (PyCFunction)pyqbdi_deleteInstrumentation,              METH_O,        "Remove an instrumentation."},
        {"getBBMemoryAccess",                 (PyCFunction)pyqbdi_getBBMemoryAccess,                  METH_NOARGS,   "Obtain the memory accesses made by the last executed basic block."},
        {"getFPRState",                       (PyCFunction)pyqbdi_getFPRState,                        METH_NOARGS,   "Obtain the current floating point register state."},
        {"getGPRState",                       (PyCFunction)pyqbdi_getGPRState,                        METH_NOARGS,   "Obtain the current general purpose register state."},
        {"getInstAnalysis",                   (PyCFunction)pyqbdi_getInstAnalysis,                    METH_O,        "Obtain the analysis of an instruction metadata."},
        {"getInstMemoryAccess",               (PyCFunction)pyqbdi_getInstMemoryAccess,                METH_NOARGS,   "Obtain the memory accesses made by the last executed instruction."},
        {"instrumentAllExecutableMaps",       (PyCFunction)pyqbdi_instrumentAllExecutableMaps,        METH_NOARGS,   "Adds all the executable memory maps to the instrumented range set."},
        {"precacheBasicBlock",                (PyCFunction)pyqbdi_precacheBasicBlock,                 METH_O,        "Pre-cache a known basic block"},
        {"readMemory",                        (PyCFunction)pyqbdi_readMemory,                         METH_VARARGS,  "Read a memory content from a base address."},
        {"recordMemoryAccess",                (PyCFunction)pyqbdi_recordMemoryAccess,                 METH_O,        "Add instrumentation rules to log memory access using inline instrumentation and instruction shadows."},
        {"removeAllInstrumentedRanges",       (PyCFunction)pyqbdi_removeAllInstrumentedRanges,        METH_NOARGS,   "Remove all instrumented ranges."},
        {"removeInstrumentedModule",          (PyCFunction)pyqbdi_removeInstrumentedModule,           METH_O,        "Remove the executable address ranges of a module from the set of instrumented address ranges."},
        {"removeInstrumentedModuleFromAddr",  (PyCFunction)pyqbdi_removeInstrumentedModuleFromAddr,   METH_O,        "Remove the executable address ranges of a module from the set of instrumented address ranges using an address belonging to the module."},
        {"removeInstrumentedRange",           (PyCFunction)pyqbdi_removeInstrumentedRange,            METH_VARARGS,  "Remove an address range from the set of instrumented address ranges."},
        {"run",                               (PyCFunction)pyqbdi_run,                                METH_VARARGS,  "Start the execution by the DBI from a given address (and stop when another is reached)."},
        {"setFPRState",                       (PyCFunction)pyqbdi_setFPRState,                        METH_O,        "Obtain the current floating point register state."},
        {"setGPRState",                       (PyCFunction)pyqbdi_setGPRState,                        METH_O,        "Obtain the current general purpose register state."},
        {nullptr,                             nullptr,                                                0,             nullptr}
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
        PyModule_AddObject(QBDI::Bindings::Python::module, "start",                 PyLong_FromLong(QBDI::Bindings::Python::start));
        PyModule_AddObject(QBDI::Bindings::Python::module, "stop",                  PyLong_FromLong(QBDI::Bindings::Python::stop));
      }


      static bool execScript(const char* fileName) {
        #if defined(__unix__) || defined(__APPLE__)
        /* On some Linux distro, we must load libpython to successfully load all others modules */
        void* handle = dlopen(PYTHON_LIBRARIES, RTLD_LAZY | RTLD_GLOBAL);
        if (!handle)
          throw std::runtime_error("QBDI::binding::python::execScript(): Cannot load the Python library.");
        #endif

        FILE* fd = nullptr;
        auto err = fopen_s(&fd, fileName, "r");
        if (err != 0)
          throw std::runtime_error("QBDI::binding::python::execScript(): Script file can't be found.");

        PyRun_SimpleFile(fd, fileName);

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
  QBDI::Bindings::Python::vm    = vm;
  QBDI::Bindings::Python::start = start;
  QBDI::Bindings::Python::stop  = stop;

  QBDI::Bindings::Python::init();
  if(const char* fileTool = std::getenv("PYQBDI_TOOL"))
    QBDI::Bindings::Python::execScript(fileTool);

  return QBDIPRELOAD_NO_ERROR;
}


int QBDI::qbdipreload_on_exit(int status) {
  return QBDIPRELOAD_NO_ERROR;
}
