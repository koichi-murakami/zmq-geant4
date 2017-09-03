# - build G4zeromq library

# library
add_library(
  G4zeromq SHARED
  G4ZMQserver.cc
)
# headers
set(HEADERS
  G4ZMQserver.hh
)

# install
install(TARGETS G4zeromq LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
