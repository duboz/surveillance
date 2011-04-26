INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${VLE_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(surveillance SHARED
                        xRay.cpp
                	xRay.hpp
			controller.cpp
                	passive_collector.cpp
                	passive_collector.hpp)

ADD_LIBRARY(sir SHARED	infection.cpp
			infection.hpp
			transmission.cpp
			transmission.hpp
			SirDifferentialEquation.cpp
			SirDifferentialEquation.hpp
      			data_collector.hpp
      			data_collector.cpp
      			collector_test.cpp)

ADD_LIBRARY(graph SHARED	
			vertexExecutive.cpp
			vertexExecutive.hpp
			graphExecutive.cpp)

TARGET_LINK_LIBRARIES(surveillance
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

TARGET_LINK_LIBRARIES(sir
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

TARGET_LINK_LIBRARIES(graph
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS surveillance
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(TARGETS sir
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

INSTALL(TARGETS graph
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

