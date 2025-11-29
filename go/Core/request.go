package Core

import "encoding/binary"

var SNFRequestUIDNull = [16]byte{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

type SNFRequest struct {
	uid  [16]byte
	op   *SNFOpcode
	args []string
}

func SNFRequestGen() *SNFRequest {
	return &SNFRequest{}
}

func (r *SNFRequest) SetUID(uid [16]byte) *SNFRequest {
	r.uid = uid
	return r
}
func (r *SNFRequest) GetUID() [16]byte {
	return r.uid
}

func (r *SNFRequest) SetOpcode(op *SNFOpcode) *SNFRequest {
	r.op = op
	return r
}
func (r *SNFRequest) GetOpcode() *SNFOpcode {
	return r.op
}

func (r *SNFRequest) ArgAdd(arg string) *SNFRequest {
	r.args = append(r.args, arg)
	return r
}

func (r *SNFRequest) ArgsAdd(args []string) {
	r.args = append(r.args, args...)
}
func (r *SNFRequest) GetArgs() []string {
	return r.args
}

// Similair to r.SetUID(original.GetUID())
func (r *SNFRequest) RespondsTo(original *SNFRequest) *SNFRequest {
	if original == nil {
		r.uid = SNFRequestUIDNull
	} else {
		r.uid = original.GetUID()
	}
	return r
}

// Similair to r.SetUID(SNFRequestUIDNull) or r.RespondsTo(nil)
func (r *SNFRequest) ServerRequest() *SNFRequest {
	return r.RespondsTo(nil)
}

func (r *SNFRequest) argsToBytes() []byte {
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

func (r *SNFRequest) ToBytes() []byte {
	var ret []byte
	ret = append(ret, r.op.ToBytes()...)
	ret = append(ret, r.uid[:]...)
	ret = binary.BigEndian.AppendUint32(ret, uint32(len(r.args)))
	args_bytes := r.argsToBytes()
	ret = binary.BigEndian.AppendUint32(ret, uint32(len(args_bytes)))
	ret = append(ret, args_bytes...)
	return ret
}
func FromBytes(data []byte) ([4]byte, uint32, *SNFRequest) {
	r := &SNFRequest{}

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
