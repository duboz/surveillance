INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${VLE_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(sir SHARED infection.cpp transmission.cpp)

TARGET_LINK_LIBRARIES(sir
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS sir
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)