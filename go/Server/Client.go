package Server

import (
	"encoding/binary"
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
	Mutex     sync.Mutex
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
		client = &SNFClient{
			Conn: conn,
		}
		switch opcode[3] {
		case core.SNF_OPCODE_BASE_DET_UNDETAILED:
			client.Mode = SNFClientConnectionModeRegular
			client.UUID = uuid.New().String()
			// Read 4-byte big-endian request count
		case core.SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT:
			client.Mode = SNFClientConnectionModeMultishot
			client.UUID = uuid.New().String()
			var reqCount uint32

			if err := binary.Read(conn, binary.BigEndian, &reqCount); err != nil {
				return
			}
			client.modeLimit = reqCount
		}

	case core.SNF_OPCODE_BASE_CMD_RECONNECT:
		//TODO: Handle Reconnect
	case core.SNF_OPCODE_BASE_CMD_DISCONNECT:
	default:
		//Invalid Opcode
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
	// TODO: Create CLient Handling after finishing properly Handling NewClients
}
