package Core

const (
	SNFVersion = "0.0.4"
	SNPVersion = "0.0.1"
)

type SNFVarRegistry struct {
	vars []interface{}
}

func SNFNewVarRegistry(numVars int) *SNFVarRegistry {
	return &SNFVarRegistry{vars: make([]interface{}, numVars)}
}

func (r *SNFVarRegistry) Set(key int, value interface{}) {
	r.vars[key] = value
}

func (r *SNFVarRegistry) Get(key int) (interface{}, bool) {
	val := r.vars[key]
	return val, val != nil
}
