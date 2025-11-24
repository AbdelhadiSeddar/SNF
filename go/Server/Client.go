package Server

import (
	"encoding/binary"
	"errors"
	"net"
	"sync"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
	"github.com/google/uuid"
)

type SNFClientConnectionMode int

const (
	SNFClientConnectionModeOneshot SNFClientConnectionMode = iota
	SNFClientConnectionModeRegular
	SNFClientConnectionModeMultishot
)

type SNFClient struct {
	UUID      string
	Conn      net.Conn
	Mode      SNFClientConnectionMode
	modeLimit uint32 // 0+ For MultiShot
	Data      interface{}
	//TODO: Redesign this
	Mutex sync.Mutex
}

type SNFClientHandlers struct {
	OnConnect func(*SNFClient) int
	OnAccept  func(*SNFClient)
}

var clients *sync.Map = nil

func snfClientInit() {
	if clients != nil {
		return
	}
	clients = new(sync.Map)
}

func SNFClientAdd(uuid string, conn net.Conn, data interface{}) *SNFClient {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
		})
	}
	if _, ok := clients.Load(uuid); ok {
		return nil
	}
	client := &SNFClient{UUID: uuid, Conn: conn, Data: data}
	clients.Store(uuid, client)
	return client
}

func SNFClientGet(uuid string) (*SNFClient, bool) {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
		})
	}

	v, ok := clients.Load(uuid)
	if !ok {
		return nil, false
	}
	return v.(*SNFClient), true
}

func SNFClientRemove(uuid string) {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
		})
	}
	clients.Delete(uuid)
}

func SNFClientHandleNew(conn net.Conn) {
	defer conn.Close()
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
		})
	}
	var opcode []byte = make([]byte, 4)

	n, err := conn.Read(opcode)
	if err != nil || n < 4 {
		return
	}
	if !(opcode[0] == opcode[1] &&
		opcode[0] == 0x00 &&
		opcode[2] >= core.SNF_OPCODE_BASE_CMD_CONNECT &&
		opcode[2] <= core.SNF_OPCODE_BASE_CMD_DISCONNECT) {
		//Invalid Opcode
		if err := Send(conn,
			[]byte{
				0x00,                             /*OPC Cat*/
				0x00,                             /*OPC SubCat*/
				core.SNF_OPCODE_BASE_CMD_INVALID, /*OPC CMD*/
				core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL, /*OPC DET*/
				0x00, 0x00, 0x00, 0x00, /*N Args*/
				0x00, 0x00, 0x00, 0x00, /*S Args*/
			},
		); err != nil {
			return
		}
	}
	var client *SNFClient = nil
	switch opcode[2] {
	case core.SNF_OPCODE_BASE_CMD_CONNECT:
		switch opcode[3] {
		case core.SNF_OPCODE_BASE_DET_UNDETAILED:
			client = SNFClientAdd(uuid.NewString(), conn, nil)
			client.Mode = SNFClientConnectionModeRegular
		case core.SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT:
			client = SNFClientAdd(uuid.NewString(), conn, nil)
			client.Mode = SNFClientConnectionModeMultishot
			var reqCount uint32

			if err := binary.Read(conn, binary.BigEndian, &reqCount); err != nil {
				return
			}
			client.modeLimit = reqCount
		case core.SNF_OPCODE_BASE_DET_CONNECT_ONESHOT:
			client = &SNFClient{
				Conn:      conn,
				Mode:      SNFClientConnectionModeOneshot,
				modeLimit: 1,
			}
		default:
			Send(conn,
				[]byte{
					0x00,                             /*OPC Cat*/
					0x00,                             /*OPC SubCat*/
					core.SNF_OPCODE_BASE_CMD_INVALID, /*OPC CMD*/
					core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL, /*OPC DET*/
					0x00, 0x00, 0x00, 0x00, /*N Args*/
					0x00, 0x00, 0x00, 0x00 /*S Args*/},
			)
		}

	case core.SNF_OPCODE_BASE_CMD_RECONNECT:
		// Temporary
		Send(conn,
			[]byte{
				0x00,                             /*OPC Cat*/
				0x00,                             /*OPC SubCat*/
				core.SNF_OPCODE_BASE_CMD_INVALID, /*OPC CMD*/
				core.SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE, /*OPC DET*/
				0x00, 0x00, 0x00, 0x00, /*N Args*/
				0x00, 0x00, 0x00, 0x00 /*S Args*/},
		)
		return
	case core.SNF_OPCODE_BASE_CMD_DISCONNECT:
	default:
		//Temporary Different just to know they are different thatn reconnect
		Send(conn,
			[]byte{
				0x00,                             /*OPC Cat*/
				0x00,                             /*OPC SubCat*/
				core.SNF_OPCODE_BASE_CMD_INVALID, /*OPC CMD*/
				core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL, /*OPC DET*/
				0x00, 0x00, 0x00, 0x00, /*N Args*/
				0x00, 0x00, 0x00, 0x00 /*S Args*/},
		)
		return
	}
	SNFClientHandle(client)
}

func SNFClientHandle(client *SNFClient) {

	for {
		req, err := SNFRequestFetch(client)
		if err != nil {
			switch {
			case errors.Is(err, core.SNFErrorOpcodeInvalid{}):
				//Error handling comes later.
				SNFRequestRespond(
					client,
					req,
					core.SNFRequestGenResponse(
						req,
						core.SNFOpcodeGetBase(
							core.SNF_OPCODE_BASE_CMD_INVALID,
							core.SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE,
						),
						nil,
					),
				)
				return
			default:
				// Respond with the value of the error/ Debug only
				SNFRequestRespond(
					client,
					req,
					core.SNFRequestGenResponse(
						req,
						core.SNFOpcodeGetBase(
							core.SNF_OPCODE_BASE_CMD_INVALID,
							core.SNF_OPCODE_BASE_DET_UNDETAILED,
						),
						&core.SNFRequestArg{
							Arg: []byte(err.Error()),
						},
					),
				)
				return
			}
		}

	}
}
