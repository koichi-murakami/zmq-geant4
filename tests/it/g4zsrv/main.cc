/*============================================================================
Copyright 2017 Koichi Murakami

Distributed under the OSI-approved BSD License (the "License");
see accompanying file LICENSE for details.

This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the License for more information.
============================================================================*/
#include <getopt.h>
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4StateManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "appbuilder.h"
#ifdef ENABLE_VIS
#include "G4VisExecutive.hh"
#endif
#include "zmq/G4ZMQServer.hh"

namespace {
// --------------------------------------------------------------------------
void show_version()
{
  std::cout << "g4zsrv version 1.0.0" << std::endl;
  /*
  const char* version_str = G4ZNQ_VERSION_MAJOR "."
                            G4ZMQ_VERSION_MINOR ".";

  std::cout << "g4zsrv version 1.0.0"
            << " (" << version_str << ::build_head << "."
            << ::build_tail << ")" << std::endl;
  */
}

// --------------------------------------------------------------------------
void show_help()
{
  std::cout << "g4zsrv" << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << "g4zsrv [options]"
            << std::endl << std::endl;
  std::cout << "   -h, --help          show this message." << std::endl
            << "   -v  --version       show program name/version." << std::endl
            << "   -n, --network       network [127.0.0.1/lo]" << std::endl
            << "   -p, --port          port [5555]" << std::endl
            << "   -s, --session=type  specify session type" << std::endl
            << "   -i, --init=macro    specify initial macro"
            << std::endl;
  std::cout << std::endl;
}

} // end of namespace

// --------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // optional parameters
  bool qhelp = false;
  bool qversion = false;
  std::string network_name = "127.0.0.1";
  std::string port_num = "5555";
  std::string session_type = "";
  std::string init_macro = "";

  struct option long_options[] = {
    {"help",    no_argument,       NULL, 'h'},
    {"version", no_argument,       NULL, 'v'},
    {"network", required_argument, NULL, 'n'},
    {"port",    required_argument, NULL, 'p'},
    {"session", required_argument, NULL, 's'},
    {"init",    required_argument, NULL, 'i'},
    {NULL,      0,                 NULL,  0}
  };

  while (1) {
    int option_index = -1;

    int c = getopt_long(argc, argv, "hvs:i:", long_options, &option_index);

    if (c == -1) break;

    switch (c) {
    case 'h' :
      qhelp = true;
      break;
    case 'v' :
      qversion = true;
      break;
    case 'n' :
      network_name = optarg;
      break;
    case 'p' :
      port_num = optarg;
      break;
    case 's' :
      session_type = optarg;
      break;
    case 'i' :
      init_macro = optarg;
      break;
    default:
      std::exit(EXIT_FAILURE);
      break;
    }
  }

  if ( qhelp ) {
    ::show_version();
    ::show_help();
  }

  if ( qversion ) {
    ::show_version();
  }

  if ( qhelp || qversion ) {
    std::exit(EXIT_SUCCESS);
  }

  std::string endpoint = "tcp://" + network_name + ":" + port_num;

  // ----------------------------------------------------------------------
  std::cout << "=============================================================="
            << std::endl;
  ::show_version();
  std::cout << "@@@ endpoint :" << endpoint << std::endl;
  std::cout << "=============================================================="
            << std::endl;

  // ----------------------------------------------------------------------
  // G4 managers & setup application
  G4RunManager* run_manager = new G4RunManager();
  G4UImanager* ui_manager = G4UImanager::GetUIpointer();

  AppBuilder* appbuilder = new AppBuilder();
  appbuilder-> SetupApplication();

  // ----------------------------------------------------------------------
#ifdef ENABLE_VIS
  G4VisManager* vis_manager = new G4VisExecutive("quiet");
  vis_manager-> Initialize();
#endif

  // do init macro
  if (init_macro != "" ) {
    G4String command = "/control/execute ";
    ui_manager-> ApplyCommand(command + init_macro);
  }

  if ( session_type == "" ) {
    G4ZMQServer* zmq_session = new G4ZMQServer();
    zmq_session-> SetEndpoint(endpoint);
    zmq_session-> SetDebug(true);
    zmq_session -> SessionStart();
    delete zmq_session;
  } else {
    G4UIExecutive* ui_session = new G4UIExecutive(argc, argv, session_type);
    ui_session-> SetPrompt("[40;01;33mecal[40;31m(%s)[40;36m[%/][00;01;30m:");
    ui_session-> SetLsColor(BLUE, RED);
    ui_session-> SessionStart();
    delete ui_session;
  }

  // ----------------------------------------------------------------------
  delete appbuilder;
  delete run_manager;
#ifdef ENABLE_VIS
  delete vis_manager;
#endif

  return EXIT_SUCCESS;
}
