cmake_minimum_required(VERSION 3.17)
project(ctp)

add_library(ctp::md SHARED IMPORTED GLOBAL)
set_property(TARGET ctp::md PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/)
IF (WIN32)
  set_property(TARGET ctp::md PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/thostmduserapi_se.dll)
  set_property(TARGET ctp::md PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/thostmduserapi_se.lib)
ELSE()
  set_property(TARGET ctp::md PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/traderapi_se_linux64/thostmduserapi_se.so)
ENDIF()


add_library(ctp::td SHARED IMPORTED GLOBAL)
set_property(TARGET ctp::td PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/)
IF (WIN32)
  set_property(TARGET ctp::td PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/thosttraderapi_se.dll)
  set_property(TARGET ctp::td PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/traderapi64_se_windows/thosttraderapi_se.lib)
ELSE()
  set_property(TARGET ctp::td PROPERTY IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/traderapi_se_linux64/thosttraderapi_se.so)
ENDIF()




