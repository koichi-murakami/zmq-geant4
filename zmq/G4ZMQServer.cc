/*============================================================================
Copyright 2017 Koichi Murakami

Distributed under the OSI-approved BSD License (the "License");
see accompanying file LICENSE for details.

This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the License for more information.
============================================================================*/
//#include "G4StateManager.hh"
//#include "G4UIcommandTree.hh"
//#include "G4UIcommand.hh"
//#include "G4UIcommandStatus.hh"
#include <cstring>
#include <sstream>
#include <zmq.hpp>
#include "G4UItcsh.hh"
#include "G4UImanager.hh"
#include "G4ZMQServer.hh"


// --------------------------------------------------------------------------
namespace {

G4UImanager* ui_manager = nullptr;
G4bool qexit = false;
G4bool qcont = false;
std::stringstream cout_stream;
std::string black_str = "\033[30m";

void ThrowException(const std::string& message)
{
  std::stringstream ss;
  ss << "[ERROR] " << message << std::endl;
  throw std::runtime_error(ss.str());
}

} // end of namespace

// --------------------------------------------------------------------------
G4ZMQServer::G4ZMQServer()
{
  endpoint_ = "tcp://127.0.0.1:5555";
  qdebug_ = false;
  shell_= new G4UItcsh();
  shell_-> SetLsColor(BLUE, RED);

  ::ui_manager = G4UImanager::GetUIpointer();
  ::ui_manager-> SetSession(this);
  ::ui_manager-> SetCoutDestination(this);

  ::qexit = false;
  ::qcont = false;
}

// --------------------------------------------------------------------------
G4ZMQServer::~G4ZMQServer()
{
  delete shell_;
}

// --------------------------------------------------------------------------
G4UIsession* G4ZMQServer::SessionStart()
{
  zmq::context_t context(1);
  zmq::socket_t socket( context, ZMQ_REP );
  socket.bind(endpoint_);

  enum { kBufferSize = 4096 };
  char buffer[kBufferSize];

  ::qexit = true;
  while ( ::qexit ) {
    std::cout << "@@ Waiting..." << std::endl;

    // waiting command
    zmq::message_t request;
    G4bool qok = socket.recv(&request);
    if ( qok ==  false ) ::ThrowException("G4ZMQSever: socket recv error");
    auto end_pos = request.size();
    if ( end_pos >= kBufferSize  ) end_pos = kBufferSize - 1;
    std::memcpy(buffer, request.data(), end_pos);
    buffer[end_pos] = '\0';
    std::string cmd_str = buffer;

    std::cout << "@@ recv=" << cmd_str << "=" << std::endl;

    // store output & send back response
    ::cout_stream.str("");

    if ( cmd_str == "ping") {
      G4cout << "pong" << G4endl;
    } else if ( cmd_str == "help") {
      G4cout << "help <command>" << G4endl;
    } else {
      G4String new_command = GetCommand(cmd_str);
      std::cout << ::black_str << "@@@ newcomd=" << new_command << std::endl;
      ExecuteCommand(new_command);
    }

    std::string reply = ::cout_stream.str();
    size_t cout_size = reply.size();
    zmq::message_t message(cout_size);
    std::strncpy((char*)message.data(), reply.c_str(), cout_size);
    qok = socket.send(message);
    if ( qok ==  false ) ::ThrowException("G4ZMQServer: socket send error");
  }

  return nullptr;
}

// --------------------------------------------------------------------------
void G4ZMQServer::PauseSessionStart(const G4String& msg)
{
}

// --------------------------------------------------------------------------
G4int G4ZMQServer::ReceiveG4cout(const G4String& coutString)
{
  if ( qdebug_ ) {
    std::cout << coutString << std::flush;
  }

  ::cout_stream << coutString << std::flush;

  return 0;
}

// --------------------------------------------------------------------------
G4int G4ZMQServer::ReceiveG4cerr(const G4String& cerrString)
{
  if ( qdebug_ ) {
    std::cerr << cerrString << std::flush;
  }

  ::cout_stream << cerrString << std::flush;

  return 0;
}

// --------------------------------------------------------------------------
G4String G4ZMQServer::GetCommand(const G4String& input)
{
  const std::string nullstr = "";
  G4String cmdstr = input;

  G4String cstr = cmdstr.strip(G4String::leading);
  if ( cstr.length() == 0 ) {
    cmdstr = nullstr;

  // define built-in shell commands...
  } else if ( cstr(0) == '#' ) {
    G4cout << cstr << G4endl;
    cmdstr = nullstr;

  } else if ( cstr == "ls" || cstr.substr(0,3) == "ls " ) {
    ListDirectory(cstr);
    cmdstr = nullstr;

  } else if ( cstr == "lc" || cstr.substr(0,3) == "lc " ) {
    shell_-> ListCommand(cstr.remove(0,2));
    cmdstr = nullstr;

  } else if(cstr == "pwd") {
    G4cout << "Current Command Directory : "
           << GetCurrentWorkingDirectory() << G4endl;
    cmdstr = nullstr;

  } else if( cstr == "cwd" ) {l
    shell_-> ShowCurrentDirectory();
    cmdstr = nullstr;

  } else if(cstr == "cd" || cstr.substr(0,3) == "cd ") {
    ChangeDirectoryCommand(cstr);
    shell_-> SetCurrentDirectory(GetCurrentWorkingDirectory());
    cmdstr = nullstr;

  } else if(cstr == "help" || cstr.substr(0,5) == "help ") {
    TerminalHelp(cstr);
    cmdstr = nullstr;

  } else if(cstr(0) == '?') {
    ShowCurrent(cstr);
    cmdstr = nullstr;

  } else if(cstr == "hist" || cstr == "history") { // "hist/history"
    G4int nh= ::ui_manager-> GetNumberOfHistory();
    for (G4int i=0; i<nh; i++) {
      G4cout << i << ": " << ::ui_manager->GetPreviousCommand(i) << G4endl;
    }
    cmdstr= nullstr;

  } else if(cstr(0) == '!') {   // "!"
    G4String ss= cstr(1, cstr.length()-1);
    G4int vl;
    const char* tt= ss;
    std::istringstream is(tt);
    is >> vl;
    G4int nh = ::ui_manager-> GetNumberOfHistory();
    if(vl>=0 && vl<nh) {
      cmdstr= ::ui_manager-> GetPreviousCommand(vl);
      G4cout << cmdstr << G4endl;
    } else {
      G4cerr << "history " << vl << " is not found." << G4endl;
      cmdstr= nullstr;
    }

  } else if(cstr == "exit") {   // "exit"
    if( :: qcont ) {
      G4cout << "You are now processing RUN." << G4endl;
      G4cout << "Please abort it using \"/run/abort\" command first" << G4endl;
      G4cout << " and use \"continue\" command until the application"
       << G4endl;
      G4cout << " becomes to Idle." << G4endl;
    } else {
      ::qexit = FALSE;
      cmdstr= nullstr;
    }



  } else if( cstr == "cont" || cstr == "continue"){     // "cont/continu"
    ::qcont = FALSE;
    cmdstr= nullstr;

  } else if( cstr.empty() ){ // NULL command
    cmdstr= nullstr;

  } else {
  }

  return ModifyToFullPathCommand(cmdstr);
}


// --------------------------------------------------------------------------
void G4ZMQServer::ExecuteCommand(const G4String& command)
{
  if(command.length()<2) return;

  G4int returnVal = ::ui_manager-> ApplyCommand(command);

  G4int paramIndex = returnVal % 100;
  // 0 - 98 : paramIndex-th parameter is invalid
  // 99     : convination of parameters is invalid
  G4int commandStatus = returnVal - paramIndex;

  G4UIcommand* cmd = nullptr;
  if( commandStatus != fCommandSucceeded ) {
    cmd = FindCommand(command);
  }

  switch ( commandStatus ) {
  case fCommandSucceeded:
    break;
  case fCommandNotFound:
    G4cerr << "command <" << ::ui_manager-> SolveAlias(command)
           << "> not found" << G4endl;
    break;
  case fIllegalApplicationState:
    G4cerr << "illegal application state -- command refused" << G4endl;
    break;
  case fParameterOutOfRange:
    G4cerr << "Parameter is out of range" << G4endl;
   break;
  case fParameterOutOfCandidates:
    G4cerr << "Parameter is out of candidate list (index "
           << paramIndex << ")" << G4endl;
    G4cerr << "Candidates : "
           << cmd-> GetParameter(paramIndex)-> GetParameterCandidates()
           << G4endl;
    break;
  case fParameterUnreadable:
    G4cerr << "Parameter is wrong type and/or is not omittable (index "
           << paramIndex << ")" << G4endl;
    break;
  case fAliasNotFound:
    break;
  default:
    G4cerr << "command refused (" << commandStatus << ")" << G4endl;
    break;
 }
}

// --------------------------------------------------------------------------
G4bool G4ZMQServer::GetHelpChoice(G4int&)
{
  return true;
}

// --------------------------------------------------------------------------
void G4ZMQServer::ExitHelp() const
{
}