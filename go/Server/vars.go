package Server

import (
	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

type ServerVarKey int

const (
	SNF_VAR_THREADS ServerVarKey = iota
  SNF_VAR_CONN_TYPE
  SNF_VAR_CONN_PIPE_NAME
	SNF_VAR_CONN_IP_PORT
	SNF_VAR_CONN_IP_ADDR
	SNF_VAR_MAX_QUEUE
	SNF_VAR_CLTS_DATA_SIZE
	SNF_VAR_CLTS_HANDLERS
	SNF_VAR_CLTS_DEFAULTCB
	SNF_VAR_RQST_MAX_LENGTH
	SNF_VAR_INITIALIZED
	SNF_VAR_INITIALIZED_OPCODE
	SNF_VAR_INITIALIZED_CLTS
	SNF_VAR_APP_NAME
	SNF_VAR_APP_VER
	SNF_N_VARS
)

var ServerVars *core.VarRegistry = nil

func SetVar(key ServerVarKey, value any) {
	ServerVars.Set(int(key), value)
}

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
	SetVar(SNF_VAR_CLTS_DATA_SIZE, -1)
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
func VarsIsInit() bool {
	if ServerVars != nil {
		var res, ok = GetVar(SNF_VAR_INITIALIZED)
		if ok && res.(bool) == true {
			return true
		}
	}
	return false
}
