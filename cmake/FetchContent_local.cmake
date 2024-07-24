if(__add_qbdi_fetchContent)
  return()
endif()
set(__add_qbdi_fetchContent ON)

include(FetchContent)

if(CMAKE_VERSION VERSION_LESS "3.28.0")
  set(FETCHCONTENT_EXCLUDE_FROM_ALL "")
  macro(fetchcontent_makeavailable_exclude_from_all)
    set(list_targets "${ARGN}")
    foreach(target IN LISTS list_targets)
      FetchContent_GetProperties(${target} POPULATED do_target_populate)
      if(NOT ${do_target_populate})
        FetchContent_Populate(${target})
        FetchContent_GetProperties(
          ${target}
          SOURCE_DIR target_SOURCE_DIR
          BINARY_DIR target_BINARY_DIR)
        if(EXISTS "${target_SOURCE_DIR}/CMakeLists.txt")
          add_subdirectory("${target_SOURCE_DIR}" "${target_BINARY_DIR}"
                           EXCLUDE_FROM_ALL)
        endif()
      endif()
      FetchContent_GetProperties(${target})
    endforeach()
  endmacro()
else()
  set(FETCHCONTENT_EXCLUDE_FROM_ALL EXCLUDE_FROM_ALL)
  macro(fetchcontent_makeavailable_exclude_from_all)
    FetchContent_MakeAvailable(${ARGN})
  endmacro()
endif()
