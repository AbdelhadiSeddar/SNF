package Core

var SNFRequestUIDNull = [16]byte{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

type SNFRequest struct {
	UID    [16]byte
	OPCODE *SNFOpcode
	Args   *SNFRequestArg
}

type SNFRequestArg struct {
	Arg  []byte
	Next *SNFRequestArg
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
	return &SNFRequestArg{Arg: append([]byte(nil), arg...)}
}

func SNFRequestArgInsert(req *SNFRequest, a *SNFRequestArg) {
	if req == nil || a == nil {
		return
	}
	if req.Args == nil {
		req.Args = a
		return
	}
	p := req.Args
	for p.Next != nil {
		p = p.Next
	}
	p.Next = a
}

func SNFRequestGenResponse(original *SNFRequest, opcode *SNFOpcode, args *SNFRequestArg) *SNFRequest {
	var uid [16]byte
	if original != nil {
		uid = original.UID
	}
	r := SNFRequestGenWUID(uid)
	r.OPCODE = opcode
	r.Args = args
	return r
}

func SNFRequestGenServerOpcode(opcode *SNFOpcode) *SNFRequest {
	return SNFRequestGenResponse(SNFRequestGenWUID(SNFRequestUIDNull), opcode, nil)
}

func (req *SNFRequest) getAllArgsBytes() []byte {
	if req == nil || req.Args == nil {
		return nil
	}
	var allArgs []byte
	p := req.Args
	for p != nil {
		allArgs = append(allArgs, p.Arg...)
		p = p.Next
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
	b = append(b,
		req.OPCODE.Category.GetValue(),
		req.OPCODE.SubCategory.GetValue(),
		req.OPCODE.Command.GetValue(),
		req.OPCODE.Detail.GetValue(),
	)
	b = append(b, req.UID[:]...)
	b = append(b, req.OPCODE.ToBytes()...)
	b = append(b, args...)
	return b
}
