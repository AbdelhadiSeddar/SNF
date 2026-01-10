package Core

const (
	SNFVersion = "0.0.3"
	SNPVersion = "0.0.1"
)

type VarRegistry struct {
	vars []any
}

func NewVarRegistry(numVars int) *VarRegistry {
	return &VarRegistry{vars: make([]any, numVars)}
}

func (r *VarRegistry) Set(key int, value any) {
	r.vars[key] = value
}

func (r *VarRegistry) Get(key int) (any, bool) {
	val := r.vars[key]
	return val, val != nil
}
