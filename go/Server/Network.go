package Server

import (
	"net"
	"strconv"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

type ConnectionType int

const (
	SNF_CONN_TYPE_IP ConnectionType = iota
	SNF_CONN_TYPE_PIPE
	SNF_CONN_TYPES
)

type Status int

const (
	ServerAccepting Status = iota
	ServerTemporarlyRefusing
	ServerRefusing
)

var snfServerStatus *Status = nil

func GetStatus() Status {
	if snfServerStatus == nil {
		panic("ISR is not compiled. Call Init()")
	}
	return *snfServerStatus
}

func SetStatus(status Status) {
	if snfServerStatus != nil {
		return
	}
	snfServerStatus = &status
	if err := InitialRequestCompile(byte(status)); err != nil {
		panic(err.Error())
	}
}
func Init() {
	if VarsIsInit() {
		return
	}
	if err := snfInitServerVars(); err != nil {
		panic(err.Error())
	}
	if clients == nil {
		snfClientInit()
	}
	SetStatus(ServerAccepting)
}
func IsInit() bool {
	return VarsIsInit() && snfOPStruct != nil && clients != nil
}

func getSocket() (net.Listener, error) {
	temp, found := GetVar(SNF_VAR_CONN_TYPE)
	T := temp.(int)
	if !found || T >= int(SNF_CONN_TYPES) {
		return nil, core.SNFErrorIntialization{FailedComponent: "Getting Socket, Check PIPE Name or IP Port"}
	}

	if T == int(SNF_CONN_TYPE_IP) {

		var tmp any
		//TODO: Add limitations if needed
		tmp, _ = GetVar(SNF_VAR_CONN_IP_PORT)
		port := tmp.(int)

		addr, _ := GetVar(SNF_VAR_CONN_IP_ADDR)
		address := addr.(string) + ":" + strconv.Itoa(port)

		listener, err := net.Listen("tcp", address)
    return listener, err

	} else if T == int(SNF_CONN_TYPE_PIPE) {
    // OS Dependant. See network_<windows/unix>.go
    return getPipeListener() 
  } else {
    return nil, core.SNFErrorUnallowedValue{
      Is: "invalid connection type in variable SNF_VAR_CONN_TYPE",
      ShouldvBeen: "SNF_CONN_TYPE_PIPE or SNF_CONN_TYPE_IP",
    }
  }
}
func Start() error {
	if err := IsInit(); err == false {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Server Definitions",
			RecommendedAction: "Call Init() first!",
		}.Error())
	}

  listener, err := getSocket()
  if err != nil {
    return err
  }
	for {
		conn, err := listener.Accept()
		if err == nil {
			go ClientHandleNew(conn)
		}
	}
}

// Send a message over the network connection.
func Send(conn net.Conn, data []byte) error {
	if _, err := conn.Write(data); err != nil {
		return err
	}
	return nil
}

// Receive a message from the network connection into a provided buffer.
func Receive(conn net.Conn, buffer []byte) (int, error) {
	n, err := conn.Read(buffer)
	if err != nil {
		return 0, err
	}
	return n, nil
}
