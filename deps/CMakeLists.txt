set(DEPENDENCY_SATISFIED
    On
    PARENT_SCOPE)

# LLVM

find_package(PythonInterp 3 REQUIRED)

add_custom_target(
  llvm
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py prepare
          ${QBDI_PLATFORM}-${QBDI_ARCH}
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py build
          ${QBDI_PLATFORM}-${QBDI_ARCH} ${QBDI_CCACHE}
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/llvm/build.py
          package ${QBDI_PLATFORM}-${QBDI_ARCH})

file(
  GLOB LLVMIncludeFile
  LIST_DIRECTORIES true
  "${CMAKE_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include/*")
file(
  GLOB LLVMLibFile
  LIST_DIRECTORIES true
  "${CMAKE_SOURCE_DIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib/*")

if(NOT (LLVMLibFile AND LLVMIncludeFile))

  set(DEPENDENCY_SATISFIED
      Off
      PARENT_SCOPE)
  message(WARNING "\n" "    LLVM is not build for this platform,\n"
                  "    first execute make|ninja llvm then rerun cmake\n")
endif()
