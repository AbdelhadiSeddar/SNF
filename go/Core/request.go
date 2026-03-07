package Core

import (
	"encoding/binary"
	"sync"
)

type RequestResponseCB func(Request)
type RequestFailureCB func(*Request, error)

type Request struct {
	uid  [16]byte
	op   *Opcode
	args []string
	resp RequestResponseCB
	fail RequestFailureCB
}

var (
	mu      sync.Mutex = sync.Mutex{}
	current [16]byte
)

func Next() [16]byte {
	mu.Lock()
	defer mu.Unlock()

	for i := 14; i >= 0; i-- {
		current[i]++

		if current[i] != 0 {
			break
		}
	}

	return current
}
func RequestGen() *Request {
	return &Request{}
}

func (r *Request) Client() {
	r.uid = Next()
	r.uid[15] = 1
}
func (r *Request) Server() {
	r.uid = Next()
	r.uid[15] = 0
}

func (r *Request) SetUID(uid [16]byte) {
	r.uid = uid
}
func (r *Request) GetUID() [16]byte {
	return r.uid
}

func (r *Request) SetOpcode(op *Opcode) *Request {
	r.op = op
	return r
}
func (r *Request) GetOpcode() *Opcode {
	return r.op
}

func (r *Request) ArgAdd(arg string) *Request {
	r.args = append(r.args, arg)
	return r
}

func (r *Request) ArgsAdd(args []string) {
	r.args = append(r.args, args...)
}
func (r *Request) GetArgs() []string {
	return r.args
}

// Similair to r.SetUID(original.GetUID())
func (r *Request) RespondsTo(original *Request) *Request {
	if original != nil {
		r.uid = original.GetUID()
	}
	return r
}
func (r *Request) OnResponse(resp RequestResponseCB) *Request {
	r.resp = resp
	return r
}
func (r *Request) OnFailure(fail RequestFailureCB) *Request {
	r.fail = fail
	return r
}
func (r *Request) CallResponse(Response *Request) {
	if r.resp != nil {
		r.resp(*Response)
	}
}
func (r *Request) CallFailure(Original *Request, err error) {
	if r.fail != nil {
		r.fail(Original, err)
	}
}

func (r *Request) argsToBytes() []byte {
	var allArgs []byte
	last_index := len(r.args) - 1

	for i, arg := range r.args {
		allArgs = append(allArgs, []byte(arg)...)

		if i < last_index {
			allArgs = append(allArgs, '\x1F')
		}
	}
	return allArgs
}

func (r *Request) ToBytes() []byte {
	var ret []byte
	ret = append(ret, r.op.ToBytes()...)
	ret = append(ret, r.uid[:]...)
	ret = binary.BigEndian.AppendUint32(ret, uint32(len(r.args)))
	args_bytes := r.argsToBytes()
	ret = binary.BigEndian.AppendUint32(ret, uint32(len(args_bytes)))
	ret = append(ret, args_bytes...)
	return ret
}
func FromBytes(data []byte) ([4]byte, uint32, *Request) {
	r := &Request{}

	// Opcode
	opDat := [4]byte(data[:4])
	data = data[4:]
	// UID (16 bytes)
	copy(r.uid[:], data[:16])
	data = data[16:]

	// Args count (4 bytes)
	argsCount := binary.BigEndian.Uint32(data[:4])
	data = data[4:]

	// Args bytes length (4 bytes)
	argsBytesLen := binary.BigEndian.Uint32(data[:4])
	data = data[4:]

	// Args bytes
	argsBytes := data[:argsBytesLen]
	data = data[argsBytesLen:]

	// Split args on 0x1F separator
	r.args = []string{}
	start := 0
	for i := 0; i < int(argsBytesLen); i++ {
		if argsBytes[i] == 0x1F {
			r.args = append(r.args, string(argsBytes[start:i]))
			start = i + 1
		}
	}
	if start < int(argsBytesLen) {
		r.args = append(r.args, string(argsBytes[start:]))
	}

	return opDat, argsCount, r
}
