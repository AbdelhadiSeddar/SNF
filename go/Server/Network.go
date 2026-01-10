package Server

import (
	"net"
	"strconv"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
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
func Start() error {
	if err := VarsIsInit(); err == false {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Server Definitions",
			RecommendedAction: "Call Init() first!",
		}.Error())
	}
	var tmp any
	//TODO: Add limitations if needed
	tmp, _ = GetVar(SNF_VAR_PORT)
	port := tmp.(int)

	addr, _ := GetVar(SNF_VAR_ADDR)
	address := addr.(string) + ":" + strconv.Itoa(port)

	listener, err := net.Listen("tcp", address)
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
