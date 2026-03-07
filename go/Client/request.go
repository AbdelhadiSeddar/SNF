package Client

import (
	"encoding/binary"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

func snfRequestParseHeader(header []byte) (*core.Request, [4]byte, uint32, uint32) {
	req := &core.Request{}

	op := [4]byte(header[:4])
	header = header[4:]
	req.SetUID([16]byte(header[:16]))
	header = header[16:]
	args_count := binary.BigEndian.Uint32(header[:4])
	header = header[4:]
	args_size := binary.BigEndian.Uint32(header[:4])

	return req, op, args_count, args_size
}

func snfRequestParseArguments(r *core.Request, content []byte) uint32 {
	// Split args on 0x1F separator
	argsBytesLen := len(content)
	start := 0
	amount := 0
	for i := 0; i < int(argsBytesLen); i++ {
		if content[i] == 0x1F {
			amount++
			r.ArgAdd(string(content[start:i]))
			start = i + 1
		}
	}
	if start < int(argsBytesLen) {
		amount++
		r.ArgAdd(string(content[start:]))
	}
	return uint32(amount)
}
