package Server

import (
	"net"
	"strconv"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

type SNFServerStatus int

const (
	SNFServerAccepting SNFServerStatus = iota
	SNFServerTemporarlyRefusing
	SNFServerRefusing
)

var snfServerStatus *SNFServerStatus = nil

func GetStatus() SNFServerStatus {
	if snfServerStatus == nil {
		panic("ISR is not compiled.")
	}
	return *snfServerStatus
}

func SetStatus(status SNFServerStatus) {
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
		//println(core.SNFOpcodePrint())
		panic(err.Error())
	}
	if clients == nil {
		snfClientInit()
	}
	SetStatus(SNFServerAccepting)
}
func IsInit() bool {
	return IsInit() && snfOPStruct != nil && clients != nil
}
func Start() error {
	if err := VarsIsInit(); err == false {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Server Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
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
