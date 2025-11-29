package Client

import (
	"encoding/binary"
	"sync/atomic"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

type SNFRequestResponseCB func(SNFRequest)
type SNFRequestFailureCB func(*SNFRequest, error)

var nextUID uint64 = 0

type SNFRequest struct {
	cr                core.SNFRequest
	onResponse        SNFRequestResponseCB
	onResponseFailure SNFRequestFailureCB
}

func (r *SNFRequest) SetResponseCallback(cb SNFRequestResponseCB) *SNFRequest {
	r.onResponse = cb
	return r
}
func (r *SNFRequest) SetResponseFailureCallback(cb SNFRequestFailureCB) *SNFRequest {
	r.onResponseFailure = cb
	return r
}
func (r *SNFRequest) Core() *core.SNFRequest {
	return &r.cr
}
func (r *SNFRequest) SetCore(cr core.SNFRequest) *SNFRequest {
	r.cr = cr
	return r
}
func (r *SNFRequest) GetArgs() []string {
	return r.cr.GetArgs()
}

func generateUID() [16]byte {
	var uid []byte
	uid = binary.BigEndian.AppendUint64(uid, uint64(0))
	uid = binary.BigEndian.AppendUint64(uid, atomic.AddUint64(&nextUID, uint64(1)))
	return [16]byte(uid)
}

func snfRequestParseHeader(header []byte) (*SNFRequest, [4]byte, uint32, uint32) {
	req := &SNFRequest{
		cr: core.SNFRequest{},
	}

	op := [4]byte(header[:4])
	header = header[4:]
	req.cr.SetUID([16]byte(header[:16]))
	header = header[16:]
	args_count := binary.BigEndian.Uint32(header[:4])
	header = header[4:]
	args_size := binary.BigEndian.Uint32(header[:4])

	return req, op, args_count, args_size
}

func (r *SNFRequest) snfRequestParseArguments(content []byte) uint32 {
	// Split args on 0x1F separator
	argsBytesLen := len(content)
	start := 0
	amount := 0
	for i := 0; i < int(argsBytesLen); i++ {
		if content[i] == 0x1F {
			amount++
			r.cr.ArgAdd(string(content[start:i]))
			start = i + 1
		}
	}
	if start < int(argsBytesLen) {
		amount++
		r.cr.ArgAdd(string(content[start:]))
	}
	return uint32(amount)
}
