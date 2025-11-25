package Server

import (
	"encoding/binary"
	"fmt"
	"strings"
	"sync"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

var snfISRRWMutex *sync.RWMutex = nil
var snfInitialServerRequest *[]byte

// SNFServerInitialRequestCompile compiles the initial server request
//
// Parameters:
//
//	Command: Values SNF_OPCODE_BASE_CMD_[CONNECT/RECONNECT/DISCONNECT]
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
			Is:          fmt.Sprintf("%d", Command),
			ShouldvBeen: "SNF_OPCODE_BASE_CMD_CONNECT, SNF_OPCODE_BASE_CMD_DISCONNECT or SNF_OPCODE_BASE_CMD_RECONNECT",
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

func SNFRequestFetch(client *SNFClient) (*core.SNFRequest, error) {
	if client.Mode != SNFClientConnectionModeOneshot {
		buf := make([]byte, 36)
		_, err := Receive(client.Conn, buf)
		if err != nil {
			return nil, err
		}
		if string(buf) != client.UUID {
			return nil, core.SNFErrorClientMismatch{
				Conn: client.Conn,
			}
		}
	}
	var req core.SNFRequest
	{ //OPCODE
		var buf [4]byte
		op, err := core.SNFOpcodeParse([4]byte(buf))
		if err != nil {
			return nil, core.SNFErrorOpcodeInvalid{
				OPCode: [4]byte(buf),
			}
		}
		req.OPCODE = op
	}
	{ //REQUID
		buf := make([]byte, 16)
		_, err := Receive(client.Conn, buf)
		if err != nil {
			return nil, err
		}
		req.UID = [16]byte(buf)
	}
	//Args
	{
		var reqArgsCount uint32
		if err := binary.Read(client.Conn, binary.BigEndian, &reqArgsCount); err != nil {
			return nil, err
		}
		var reqArgsSize uint32
		if err := binary.Read(client.Conn, binary.BigEndian, &reqArgsSize); err != nil {
			return nil, err
		}
		if (reqArgsCount == 0 || reqArgsSize == 0) && (reqArgsCount+reqArgsSize) != 0 {
			return nil, core.SNFErrorUnallowedValue{
				Is:          "of either Amount or Size of Argument is 0",
				ShouldvBeen: "both 0 or neither",
			}
		}
		// Arg Handling
		if reqArgsCount > 0 && reqArgsSize > 0 {
			args := make([]byte, reqArgsSize)

			if _, err := Receive(client.Conn, args); err != nil {
				return nil, err
			}

			args_s := string(args)
			args_s_split := strings.Split(args_s, "\x1F")
			if len(args_s_split) != int(reqArgsCount) {
				return nil, core.SNFErrorUnallowedValue{
					Is:          fmt.Sprintf("of the number of arguments in Request %x is %d", req.UID, len(args_s_split)),
					ShouldvBeen: fmt.Sprintf("%d", reqArgsCount),
				}
			}
			var args_h *core.SNFRequestArg = nil
			var args_c *core.SNFRequestArg = nil
			for i := 0; i < int(reqArgsCount); i++ {
				arg := &core.SNFRequestArg{
					Arg: []byte(args_s_split[i]),
				}
				if args_h == nil {
					args_h = arg
				}
				if args_c == nil {
					args_c = arg
				} else {
					args_c.Next = arg
					args_c = args_c.Next
				}
			}
		}

	}

	return &req, nil
}
func SNFRequestSend(client *SNFClient, Request *core.SNFRequest) error {
	if client.Mode != SNFClientConnectionModeOneshot {
		if _, err := client.Conn.Write([]byte(client.UUID)); err != nil {
			return err
		}
	}
	// OPCODE
	if _, err := client.Conn.Write(Request.OPCODE.ToBytes()); err != nil {
		return err
	}
	// REQUID (echo original UID)
	if _, err := client.Conn.Write(Request.UID[:]); err != nil {
		return err
	}
	// Args
	var argCount uint32
	var argSize uint32
	var argBuf []byte
	if Request.Args != nil {
		for arg := Request.Args; arg != nil; arg = arg.Next {
			argCount++
			argBuf = append(argBuf, arg.Arg...)
			if arg.Next != nil {
				argBuf = append(argBuf, 0x1F)
			}
		}
		argSize = uint32(len(argBuf))
	}
	if err := binary.Write(client.Conn, binary.BigEndian, argCount); err != nil {
		return err
	}
	if err := binary.Write(client.Conn, binary.BigEndian, argSize); err != nil {
		return err
	}
	if argSize > 0 {
		if _, err := client.Conn.Write(argBuf); err != nil {
			return err
		}
	}
	return nil
}
