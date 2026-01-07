package Client

import core "github.com/AbdelhadiSeddar/SNF/go/Core"

type ClientVarKey int

const (
	CLIENT_VAR_ID ClientVarKey = iota
	CLIENT_VAR_STATUS
	CLIENT_N_VARS
)

var ClientVars = core.SNFNewVarRegistry(int(CLIENT_N_VARS))

func SetClientVar(key ClientVarKey, value interface{}) {
	ClientVars.Set(int(key), value)
}

func GetClientVar(key ClientVarKey) (interface{}, bool) {
	return ClientVars.Get(int(key))
}
