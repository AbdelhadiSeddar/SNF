package Server

import (
	"fmt"
	"sync"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

var snfISRRWMutex *sync.RWMutex = nil
var snfInitialServerRequest *[]byte

// SNFServerInitialRequestCompile compiles the initial server request
//
// Command: MUST be one of \\
// - SNF_OPCODE_CMD_CONNECT:
func SNFServerInitialRequestCompile(Command byte) error {
	if !SNFServerIsInit() {
		return core.SNFErrorUninitialized{
			Component:         "\"All\"",
			RecommendedAction: "Call SNFServerInit() first!",
		}
	}
	switch Command {
	case core.SNF_OPCODE_BASE_CMD_CONNECT:
	case core.SNF_OPCODE_BASE_CMD_RECONNECT:
	case core.SNF_OPCODE_BASE_CMD_DISCONNECT:
	default:
		return core.SNFErrorUnallowedValue{
			Is:       fmt.Sprintf("%d", Command),
			Shouldbe: "SNF_OPCODE_BASE_CMD_CONNECT, SNF_OPCODE_BASE_CMD_DISCONNECT or SNF_OPCODE_BASE_CMD_RECONNECT",
		}
	}
	if snfISRRWMutex == nil {
		snfISRRWMutex = new(sync.RWMutex)
	}
	var temp_isr []byte
	appver, _ := GetVar(SNF_VAR_APP_VER)
	appname, _ := GetVar(SNF_VAR_APP_NAME)
	var temp_server_info string = fmt.Sprintf("%s|%s@%s|%s@%s",
		core.SNPVersion,
		core.SNFVersion,
		"snf",
		appver.(string),
		appname.(string),
	)

	temp_isr = append(temp_isr, temp_server_info...)

	// Define the opcode
	temp_isr = append(temp_isr, 0x00, 0x00, Command, 0x00)
	/// Defining Needed stuff |4B_Amounts of Arguments|4B__Size of Arguments__|
	temp_isr = append(temp_isr, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
	//TODO: Allow for TimeOut for not accepting Clients.

	snfISRRWMutex.Lock()
	snfInitialServerRequest = &temp_isr
	defer snfISRRWMutex.Unlock()

	return nil
}

func SNFServerInitialRequestGet() ([]byte, error) {
	if !SNFServerIsInit() {
		return nil, core.SNFErrorUninitialized{
			Component:         "\"All\"",
			RecommendedAction: "Call SNFServerInit() first!",
		}
	}
	var ret []byte = make([]byte, len(*snfInitialServerRequest))
	snfISRRWMutex.RLock()
	copy(ret, *snfInitialServerRequest)
	defer snfISRRWMutex.RUnlock()
	return ret, nil
}
