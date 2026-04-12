package Server

import (
	"encoding/binary"
	"errors"
	"net"
	"sync"

	"github.com/AbdelhadiSeddar/SNF/go/Core"
	core "github.com/AbdelhadiSeddar/SNF/go/Core"
	"github.com/google/uuid"
)

type ClientConnectionMode int

const (
	ClientConnectionModeOneshot ClientConnectionMode = iota
	ClientConnectionModeRegular
	ClientConnectionModeMultishot
)

type Client struct {
	UUID         [16]byte
	Conn         net.Conn
	Mode         ClientConnectionMode
	modeLimit    uint32 // 0+ For MultiShot
	Data         any
	sentRequests map[[16]byte]*core.Request
	rqstsMutex   sync.RWMutex
}

type OnConnectCallBack func(*Client) int
type OnAcceptCallBack func(*Client)

type ClientHandlers struct {
	OnConnect OnConnectCallBack
	OnAccept  OnAcceptCallBack
}

type ClientDataInitCallBack func() any

var clients *sync.Map = nil

func snfClientInit() {
	if clients != nil {
		return
	}
	clients = new(sync.Map)
}
func (c *Client) Send(req *core.Request) {
	if req == nil {
		return
	}

	RequestSend(c, req)
}

func ClientAdd(uuid [16]byte, conn net.Conn, data any) *Client {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call Init() first!",
		})
	}
	if _, ok := clients.Load(uuid); ok {
		return nil
	}
	client := &Client{UUID: uuid, Conn: conn, Data: data}
	clients.Store(uuid, client)
	return client
}

func ClientGet(uuid string) (*Client, bool) {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call Init() first!",
		})
	}

	v, ok := clients.Load(uuid)
	if !ok {
		return nil, false
	}
	return v.(*Client), true
}

func ClientRemove(uuid [16]byte) {
	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call Init() first!",
		})
	}
	clients.Delete(uuid)
}

func ClientHandleNew(conn net.Conn) {
	defer conn.Close()

	if clients == nil {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Client Definitions",
			RecommendedAction: "Call Init() first!",
		})
	}
	hand, ok := GetVar(SNF_VAR_CLTS_HANDLERS)
	h := hand.(ClientHandlers)
	if ok {
		if h.OnAccept != nil {
			h.OnAccept(&Client{Conn: conn})
		}
	}

	// Send the appropriate stuff
	isr, err := InitialRequestGet()
	if err != nil {
		panic(err.Error())
	}
	send(conn, isr)

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
		if err := send(conn,
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
	clt_init, ok := GetVar(SNF_VAR_CLTS_DATA_INIT)
	var data any = nil

	if ok {
		data = clt_init.(ClientDataInitCallBack)()
	}
	var client *Client = nil
	switch opcode[2] {
	case core.SNF_OPCODE_BASE_CMD_CONNECT:
		switch opcode[3] {
		case core.SNF_OPCODE_BASE_DET_UNDETAILED:
			client = ClientAdd([16]byte(uuid.New()), conn, data)
			client.Mode = ClientConnectionModeRegular
		case core.SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT:
			client = ClientAdd([16]byte(uuid.New()), conn, data)
			client.Mode = ClientConnectionModeMultishot
			var reqCount uint32

			if err := binary.Read(conn, binary.BigEndian, &reqCount); err != nil {
				return
			}
			client.modeLimit = reqCount
		case core.SNF_OPCODE_BASE_DET_CONNECT_ONESHOT:
			client = &Client{
				Conn:      conn,
				Mode:      ClientConnectionModeOneshot,
				modeLimit: 1,
				Data:      data,
			}
		default:
			send(conn,
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
		if client.Mode != ClientConnectionModeOneshot {
			snd := []byte{
				0x00,                                /*OPC Cat*/
				0x00,                                /*OPC SubCat*/
				core.SNF_OPCODE_BASE_CMD_CONFIRM,    /*OPC CMD*/
				core.SNF_OPCODE_BASE_DET_UNDETAILED, /*OPC DET*/
				0x00, 0x00, 0x00, 0x01,              /*N Args*/
				0x00, 0x00, 0x00, 0x10, /*S Args*/
			}
			snd = append(snd, client.UUID[:]...)
			send(conn, snd)
		}
	case core.SNF_OPCODE_BASE_CMD_RECONNECT:
		// Temporary
		send(conn,
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
		send(conn,
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
	if ok && h.OnConnect != nil {
		h.OnConnect(client)
	}
	ClientHandle(client)
}

func ClientHandle(client *Client) {
	defer func(client *Client) {
		if client != nil && client.Conn != nil {
			client.Conn.Close()
			client.Conn = nil
		}
	}(client)
	for {
		req, err := RequestFetch(client)
		if err != nil {
			switch {
			case errors.Is(err, core.SNFErrorOpcodeInvalid{}):
				//Error handling comes later.
				Send(client,
					core.RequestGen().
						RespondsTo(req).
						SetOpcode(
							snfOPStruct.GetBaseOpcode(
								core.SNF_OPCODE_BASE_CMD_INVALID,
								core.SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE,
							)),
				)
				return
			default:
				// Respond with the value of the error/ Debug only
				Send(client,
					core.RequestGen().
						RespondsTo(req).
						SetOpcode(
							snfOPStruct.GetBaseOpcode(
								core.SNF_OPCODE_BASE_CMD_INVALID,
								core.SNF_OPCODE_BASE_DET_UNDETAILED,
							)),
				)
				return
			}
		}
		var res *Core.Request
		if req.GetUID()[15] == 0 {
			client.rqstsMutex.RLock()
			item, ok := client.sentRequests[req.GetUID()]
			client.rqstsMutex.RUnlock()
			if ok {
				go item.CallResponse(req)
			}
		} else {

			// Calling the function
			f := req.GetOpcode().Command.GetCallback()
			if f == nil {
				err = RequestSend(client,
					core.RequestGen().
						RespondsTo(req).
						SetOpcode(
							snfOPStruct.GetBaseOpcode(
								core.SNF_OPCODE_BASE_CMD_INVALID,
								core.SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE,
							)),
				)
				if err != nil {
					return
				}
			}

			//FIXME: This sounds so wrong!
			res, err = f(*req, client)
			if err != nil {
				res = core.RequestGen().
					RespondsTo(req).
					SetOpcode(
						snfOPStruct.GetBaseOpcode(
							core.SNF_OPCODE_BASE_CMD_INVALID,
							core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL,
						),
					)
			}

			ResponseSend(client, res)
			if req.GetOpcode().IsBase() && req.GetOpcode().Command.GetValue() == core.SNF_OPCODE_BASE_CMD_DISCONNECT {
				ClientRemove(client.UUID)
				return
			}
		}
	}
}
