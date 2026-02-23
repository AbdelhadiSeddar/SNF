package Server

import (
	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

type ServerVarKey int

const (
	/*
	 Used to set how many threads the SNFramework can use simultaneously

	 Default : 4
	*/
	SNF_VAR_THREADS ServerVarKey = iota
	/*
		Used to define the server's connection type.
		Could be either SNF_CONN_TYPE_IP or SNF_CONN_TYPE_PIPE

		Default : SNF_CONN_TYPE_IP
	*/
	SNF_VAR_CONN_TYPE
	/*
		Used to define the server's PIPE Name

		WARNING: For windows `\\.\<pipe path>`, and for linux `/<pipe path>`

		Default: `/tmp/pipe`
	*/
	SNF_VAR_CONN_PIPE_NAME
	/*
		Used to define the server's port

		Default: 9114
	*/
	SNF_VAR_CONN_IP_PORT
	/*
		Used to define the server's allowed addresses

		Default: 0.0.0.0
	*/
	SNF_VAR_CONN_IP_ADDR
	//
	SNF_VAR_MAX_QUEUE
	//TODO: Check this
	SNF_VAR_CLTS_DATA_INIT
	/*
		Used to define an instance of ClientHandlers which contains

		OnAccept(*Client) int & OnConnect(*Client) .
	*/
	SNF_VAR_CLTS_HANDLERS
	// Undefined
	SNF_VAR_CLTS_DEFAULTCB
	/*
		*Unimplemented*

		Allows to limit the max amount of a request

		Default : 4096
	*/
	SNF_VAR_RQST_MAX_LENGTH
	/*
		  INFO: Read-Only

			Infomrs if the structure is initialized or not,
	*/
	SNF_VAR_INITIALIZED
	/*
		  INFO: Read-Only

			Infomrs if the opcode structure is initialized or not,
	*/
	SNF_VAR_INITIALIZED_OPCODE
	/*
		  INFO: Read-Only

			Infomrs if the client structure is initialized or not,
	*/
	SNF_VAR_INITIALIZED_CLTS
	/*
		  Used to define the server to the client.

			Default: "Unkown"
	*/
	SNF_VAR_APP_NAME
	/*
		Used to define the server's version to the client

		Default : 0
	*/
	SNF_VAR_APP_VER
	/*
		WARNING: This is an invalid variable, it is only here to define how many variables ther are.
	*/
	SNF_N_VARS
)

var ServerVars *core.VarRegistry = nil

/*
Used to set a variable's value using their key index
*/
func SetVar(key ServerVarKey, value any) {
	ServerVars.Set(int(key), value)
}

// Used to get a variable's value using their key index
func GetVar(key ServerVarKey) (any, bool) {
	return ServerVars.Get(int(key))
}

func snfInitServerVars() error {
	if ServerVars == nil {
		ServerVars = core.NewVarRegistry(int(SNF_N_VARS))
	}
	if VarsIsInit() {
		return nil
	}
	SetVar(SNF_VAR_THREADS, 4)
	SetVar(SNF_VAR_CONN_TYPE, SNF_CONN_TYPE_IP)
	SetVar(SNF_VAR_CONN_PIPE_NAME, "/tmp/pipe")
	SetVar(SNF_VAR_CONN_IP_PORT, 9114)
	SetVar(SNF_VAR_CONN_IP_ADDR, "0.0.0.0")
	SetVar(SNF_VAR_MAX_QUEUE, 1000)
	SetVar(SNF_VAR_CLTS_DATA_INIT, nil)
	SetVar(SNF_VAR_CLTS_HANDLERS, nil)
	SetVar(SNF_VAR_CLTS_DEFAULTCB, snfServerDefaultCallBack)
	SetVar(SNF_VAR_RQST_MAX_LENGTH, 4096)
	SetVar(SNF_VAR_INITIALIZED, true)
	SetVar(SNF_VAR_INITIALIZED_OPCODE, false)
	SetVar(SNF_VAR_INITIALIZED_CLTS, 100)
	SetVar(SNF_VAR_APP_NAME, "Unkown")
	SetVar(SNF_VAR_APP_VER, "0")

	err := snfServerSetOpcodeCallbacks()
	if err != nil {
		return err
	}

	return nil
}

// Returns if the Variable structure was defined.
func VarsIsInit() bool {
	if ServerVars != nil {
		var res, ok = GetVar(SNF_VAR_INITIALIZED)
		if ok && res.(bool) == true {
			return true
		}
	}
	return false
}
