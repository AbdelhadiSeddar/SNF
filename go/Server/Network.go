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

func SNFGetStatus() SNFServerStatus {
	if snfServerStatus == nil {
		panic("ISR is not compiled.")
	}
	return *snfServerStatus
}

func SNFSetStatus(status SNFServerStatus) {
	if snfServerStatus != nil {
		return
	}
	snfServerStatus = &status
	if err := SNFServerInitialRequestCompile(byte(status)); err != nil {
		panic(err.Error())
	}
}
func SNFServerInit() {
	if SNFServerVarsIsInit() {
		return
	}
	if err := snfInitServerVars(); err != nil {
		//println(core.SNFOpcodePrint())
		panic(err.Error())
	}
	if clients == nil {
		snfClientInit()
	}
	SNFSetStatus(SNFServerAccepting)
}
func SNFServerIsInit() bool {
	return SNFServerVarsIsInit() && snfOPStruct != nil && clients != nil
}
func SNFServerStart() error {
	if err := SNFServerVarsIsInit(); err == false {
		panic(core.SNFErrorUninitialized{
			Component:         "Core Server Definitions",
			RecommendedAction: "Call SNFServerInit() first!",
		}.Error())
	}
	var tmp interface{}
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
			go SNFClientHandleNew(conn)
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
