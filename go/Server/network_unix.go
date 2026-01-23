//go:build !windows

package Server

import (
	"net"
	"os"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

// Wile
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
			Is:          "of Unknown Type",
			ShouldvBeen: "a string eg: (linux)\"/tmp/pipe.sock\"",
		}
	}

	os.Remove(pipe)

	return net.Listen("unix", pipe)

}

