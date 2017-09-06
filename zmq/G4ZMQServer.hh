/*============================================================================
Copyright 2017 Koichi Murakami

Distributed under the OSI-approved BSD License (the "License");
see accompanying file LICENSE for details.

This software is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the License for more information.
============================================================================*/
#ifndef G4ZMQ_SERVER_H_
#define G4ZMQ_SERVER_H_

#include "G4VBasicShell.hh"

class G4UItcsh;

class G4ZMQServer : public  G4VBasicShell {
public:
  G4ZMQServer();
   ~G4ZMQServer();

  void SetEndpoint(const G4String& endpoint);
  G4String GetEndpoint() const;

  void SetDebug(G4bool flag);

  virtual G4UIsession* SessionStart();
  virtual void PauseSessionStart(const G4String& message);

  virtual G4int ReceiveG4cout(const G4String& coutString);
  virtual G4int ReceiveG4cerr(const G4String& cerrString);

private:
  G4bool qdebug_;
  G4String endpoint_;
  G4UItcsh* shell_;

  G4String GetCommand(const G4String& input);

  virtual void ExecuteCommand(const G4String& command);
  virtual G4bool GetHelpChoice(G4int& );
  virtual void ExitHelp() const;

};

// ==========================================================================
inline void G4ZMQServer::SetEndpoint(const G4String& endpoint)
{
  endpoint_ = endpoint;
}

inline G4String G4ZMQServer::GetEndpoint() const
{
  return endpoint_;
}

inline void G4ZMQServer::SetDebug(G4bool flag)
{
  qdebug_ = flag;
}

#endif
