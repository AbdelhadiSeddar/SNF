package Core

var SNFRequestUIDNull = [16]byte{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

type SNFRequest struct {
	UID    [16]byte
	OPCODE *SNFOpcode
	args   *SNFRequestArg
}

type SNFRequestArg struct {
	arg  []byte
	next *SNFRequestArg
}

func SNFRequestGen() *SNFRequest {
	return &SNFRequest{}
}

func SNFRequestGenWUID(uid [16]byte) *SNFRequest {
	r := &SNFRequest{}
	r.UID = uid
	return r
}

func SNFRequestArgGen(arg []byte) *SNFRequestArg {
	return &SNFRequestArg{arg: append([]byte(nil), arg...)}
}

func SNFRequestArgInsert(req *SNFRequest, a *SNFRequestArg) {
	if req == nil || a == nil {
		return
	}
	if req.args == nil {
		req.args = a
		return
	}
	p := req.args
	for p.next != nil {
		p = p.next
	}
	p.next = a
}

func SNFRequestGenResponse(original *SNFRequest, opcode *SNFOpcode, args *SNFRequestArg) *SNFRequest {
	var uid [16]byte
	if original != nil {
		uid = original.UID
	}
	r := SNFRequestGenWUID(uid)
	r.OPCODE = opcode
	r.args = args
	return r
}

func SNFRequestGenServerOpcode(opcode *SNFOpcode) *SNFRequest {
	return SNFRequestGenResponse(SNFRequestGenWUID(SNFRequestUIDNull), opcode, nil)
}

func SNFRequestGenBase(original *SNFRequest, command byte, detail byte) *SNFRequest {
	return SNFRequestGenResponse(original, SNFOpcodeGetBase(command, detail), nil)
}

func SNFRequestGenUBase(original *SNFRequest, command byte) *SNFRequest {
	return SNFRequestGenResponse(original, SNFOpcodeGetUBase(command), nil)
}

func (req *SNFRequest) getAllArgsBytes() []byte {
	if req == nil || req.args == nil {
		return nil
	}
	var allArgs []byte
	p := req.args
	for p != nil {
		allArgs = append(allArgs, p.arg...)
		p = p.next
		if p != nil {
			allArgs = append(allArgs, '|')
		}
	}
	return allArgs
}

func (req *SNFRequest) ToBytes() []byte {
	if req == nil || req.OPCODE == nil {
		return nil
	}
	args := req.getAllArgsBytes()
	var b []byte = make([]byte, 0, 4+16+1+len(args))
	b = append(b, req.OPCODE.Category, req.OPCODE.SubCategory, req.OPCODE.Command, req.OPCODE.Detail)
	b = append(b, req.UID[:]...)
	b = append(b, req.OPCODE.ToBytes()...)
	b = append(b, args...)
	return b
}
