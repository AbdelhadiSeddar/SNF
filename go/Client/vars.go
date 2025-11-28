package Client

import core "github.com/AbdelhadiSeddar/SNF/go/Core"

type SNFConnectionVarKey int

const (
	SNF_CONNECTION_VAR_ID SNFConnectionVarKey = iota
	SNF_CONNECTION_VAR_STATUS
	SNF_CONNECTION_N_VARS
)

var SNFConnectionVars = core.SNFNewVarRegistry(int(SNF_CONNECTION_N_VARS))

func SetSNFConnectionVar(key SNFConnectionVarKey, value interface{}) {
	SNFConnectionVars.Set(int(key), value)
}

func GetSNFConnectionVar(key SNFConnectionVarKey) (interface{}, bool) {
	return SNFConnectionVars.Get(int(key))
}
