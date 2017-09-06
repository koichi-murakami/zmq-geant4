# executable
#------------------------------------------------------------------------------
set(APP_NAME g4zsrv)
#------------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR} ${GEANT4_INCLUDE_DIR})

link_directories(${GEANT4_LIBRARY_DIR} ${CMAKE_BINARY_DIR}/zmq)

if(ENABLE_VIS)
  add_definitions(-DG4INTY_USE_X -DG4VIS_USE_OPENGLX -DG4VIS_USE_OPENGLQT)
  add_definitions(-DENABLE_VIS)
endif()

# executable
add_executable(${APP_NAME}
  appbuilder.cc calscorer.cc ecalgeom.cc eventaction.cc main.cc
  particlegun.cc runaction.cc simdata.cc stepaction.cc
)

if(ENABLE_VIS)
  target_link_libraries(${APP_NAME} ${GEANT4_LIBRARIES_WITH_VIS} G4zeromq zmq)
else()
  target_link_libraries(${APP_NAME} ${GEANT4_LIBRARIES} G4zeromq zmq)
endif()

#set_target_properties(${APP_NAME}
#                      PROPERTIES INSTALL_RPATH
#                      ${GEANT4_LIBRARY_DIR} ${CMAKE_BINARY_DIR}/zmq
#                      BUILD_WITH_INSTALL_RPATH TRUE)

install(TARGETS ${APP_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/g4zsrv)
install(FILES config.tmpl DESTINATION ${CMAKE_INSTALL_PREFIX}/g4zsrv)
