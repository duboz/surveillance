INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${VLE_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES(
  ${VLE_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS})

ADD_LIBRARY(x_ray SHARED
                        xRay.cpp
                	xRay.hpp
	               	active_collector.cpp
                	active_collector.hpp)

TARGET_LINK_LIBRARIES(x_ray
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS x_ray
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(controller SHARED
			controller.hpp
			controller.cpp)

TARGET_LINK_LIBRARIES(controller
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS controller
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(delay SHARED
                        delay.cpp
			delay.hpp)

TARGET_LINK_LIBRARIES(delay
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS delay
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(targeted SHARED
			targeted.cpp
			targeted.hpp
			active_collector.cpp
                	active_collector.hpp)

TARGET_LINK_LIBRARIES(targeted
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS targeted
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(spatialControler SHARED
			spatialController.cpp)

TARGET_LINK_LIBRARIES(spatialControler
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS spatialControler
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(abs_Active_collector SHARED
                  	active_collector.cpp
                	active_collector.hpp)

TARGET_LINK_LIBRARIES(abs_Active_collector
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS abs_Active_collector
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(passive_collector SHARED
 	              	passive_collector.cpp
                	passive_collector.hpp)

TARGET_LINK_LIBRARIES(passive_collector
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS passive_collector
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(SurvScheduler SHARED
 			surv_scheduler.cpp)

TARGET_LINK_LIBRARIES(SurvScheduler
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS SurvScheduler
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(information_center SHARED
			information_center.cpp)

TARGET_LINK_LIBRARIES(information_center
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS information_center
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(infection
		SHARED	
		infection.cpp
		infection.hpp)

TARGET_LINK_LIBRARIES(infection
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS infection
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(transmission
		SHARED	
		transmission.cpp
		transmission.hpp)

TARGET_LINK_LIBRARIES(transmission
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS transmission
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(data_collector
	SHARED
	data_collector.hpp
      	data_collector.cpp)

TARGET_LINK_LIBRARIES(data_collector
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS data_collector
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(dyn_vertexExecutive SHARED	
			vertexExecutive.cpp
			vertexExecutive.hpp)

TARGET_LINK_LIBRARIES(dyn_vertexExecutive
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS dyn_vertexExecutive
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(dyn_graphExecutive SHARED	
			graphExecutive.cpp
			GraphTranslator.cpp
			GraphTranslator.hpp)

TARGET_LINK_LIBRARIES(dyn_graphExecutive
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS dyn_graphExecutive
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

ADD_LIBRARY(utils_graphTranslator SHARED	
			GraphTranslator.cpp
			GraphTranslator.hpp)

TARGET_LINK_LIBRARIES(utils_graphTranslator
  ${VLE_LIBRARIES}
  ${Boost_LIBRARIES})

INSTALL(TARGETS utils_graphTranslator
  RUNTIME DESTINATION lib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

