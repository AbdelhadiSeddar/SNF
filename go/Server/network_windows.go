//go:build windows

package Server

import (
	"net"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
	"github.com/Microsoft/go-winio"
)

// WHile
func getPipeListener() (net.Listener, error) {
	variable, found := GetVar(SNF_VAR_CONN_PIPE_NAME)

	if !found {
		return nil, core.SNFErrorUninitialized{
			Component:         "Undefined Named Pipe",
			RecommendedAction: "Call Server.Init() First! and/or Server.SetVar(SNF_VAR_CONN_PIPE_NAME, <YOUR_PIPE_NAME>)",
		}
	}
	pipe, ok := variable.(string)
  if !ok {
    return nil, core.SNFErrorUnallowedValue{
      Is: "of Unknown Type",
      ShouldvBeen: "a string eg: (windows)\"\\\\.\\pipe\\<PIPENAME>\"",
    }
  }
	return winio.ListenPipe(pipe, nil)
}
