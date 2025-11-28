package Client

import (
	"context"
	"encoding/binary"
	"errors"
	"net"
	"sync"
	"time"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

// Default Time Duration a Client should wait between each interactions before timing out and re-trying out
var SNFConnectionDefaultTimeOut time.Duration = time.Minute

// Amount of times a connection is retried before giving up. (0 means no re-tries)
var SNFConnectionDefaultRetries int = 0

type OnResponseCallback func(SNFRequest, error)
type OnConnectCallback func()
type OnSNFFailCallback func(error)
type OnSocketFailCallback func(error)
type OnExceptionCallback func(error)
type OnTimeoutCallback func()

// SNFConnection represents a client connection to the SNF server.
type SNFConnectionInfo struct {
	once         bool
	once_d       bool
	conn         net.Conn
	address      string
	uuid         string
	timeOut      time.Duration
	retries      int
	isConnected  bool
	requestQueue chan *SNFRequest
	requestsSent map[[16]byte]*SNFRequest
}
type SNFConnectionCallbacks struct {
	onResponseCallback  OnResponseCallback
	onConnectCallback   OnConnectCallback
	onExceptionCallback OnExceptionCallback
	onTimeoutCallback   OnTimeoutCallback
}
type SNFConnection struct {
	SNFConnectionInfo
	SNFConnectionCallbacks
	opcodes *core.SNFOpcodeRootStructure

	mu sync.Mutex
}

func SNFNewConnection() *SNFConnection {
	ret := &SNFConnection{}

	ret.timeOut = SNFConnectionDefaultTimeOut
	ret.retries = SNFConnectionDefaultRetries

	return ret
}

func (r *SNFConnection) SetAddress(address string) *SNFConnection {
	r.address = address
	return r
}
func (r *SNFConnection) SetOpcodeStruct(op *core.SNFOpcodeRootStructure) *SNFConnection {
	r.opcodes = op
	return r
}

// If r.Once() was not called. This will be ignored.
func (r *SNFConnection) OnResponse(cb OnResponseCallback) *SNFConnection {
	r.onResponseCallback = cb
	return r
}
func (r *SNFConnection) OnConnect(cb OnConnectCallback) *SNFConnection {
	r.onConnectCallback = cb
	return r
}

// If this connection's retries are > 0 it will retry after returning this function if set
func (r *SNFConnection) OnException(cb OnConnectCallback) *SNFConnection {
	r.onConnectCallback = cb
	return r
}

// If this connection's retries are > 0 it will retry after returning this function if set
func (r *SNFConnection) OnTimeout(cb OnConnectCallback) *SNFConnection {
	r.onConnectCallback = cb
	return r
}

// Note Oneshot will note be implemented yet.
func (r *SNFConnection) Once(req *SNFRequest) *SNFConnection {
	r.once = true
	r.requestQueue <- req

	return r
}

func (r *SNFConnection) Connect() *SNFConnection {
	var err error
	dialer := net.Dialer{
		Timeout: r.timeOut,
	}
	retries := r.retries
	for {
		if retries == 0 {
			return nil
		}
		r.conn, err = dialer.Dial("tcp", r.address)
		if err != nil {
			if errors.Is(err, context.DeadlineExceeded) || errors.Is(err, net.ErrClosed) && err.Error() == "i/o timeout" {
				if r.onTimeoutCallback != nil {
					r.onTimeoutCallback()
				}
			} else {
				if r.onExceptionCallback != nil {
					r.onExceptionCallback(err)
				}
			}
			retries--
			continue
		}
		break
	}
	{
		buf := make([]byte, 16)

		n, err := r.conn.Read(buf)
		if err != nil {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(err)
			}
			r.conn.Close()
			return nil
		} else if n < 16 {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(core.SNFErrorUninitialized{
					Component:         "\"Connection\"",
					RecommendedAction: "Communication failure with the server. Verify Server and re-try.",
				})
			}
			r.conn.Close()
			return nil
		}

		op := r.opcodes.GetOpcode(
			buf[0],
			buf[1],
			buf[2],
			buf[3],
		)
		if op == nil || op.Category.GetValue() != op.SubCategory.GetValue() ||
			op.Category.GetValue() != 0 ||
			op.Command.GetValue() != core.SNF_OPCODE_BASE_CMD_CONFIRM ||
			op.Detail.GetValue() != core.SNF_OPCODE_BASE_DET_UNDETAILED {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(core.SNFErrorUninitialized{
					Component:         "\"Connection\"",
					RecommendedAction: "Communication failure with the server. Verify compatibility with the Server and re-try.",
				})
			}
			r.conn.Close()
			return nil
		}
		buf = buf[4:]

		args_amount := binary.BigEndian.Uint32(buf[:4])
		buf = buf[4:] // Advance the buffer again

		// Read the second uint32 (next 4 bytes)
		args_size := binary.BigEndian.Uint32(buf[:4])

		if args_amount != 1 || args_size == 0 {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(core.SNFErrorUninitialized{
					Component:         "\"Connection\"",
					RecommendedAction: "Communication mixup failure with the server. Verify compatibility with the Server and re-try.",
				})
			}
			r.conn.Close()
			return nil
		}

		arg := make([]byte, args_size)
		n, err = r.conn.Read(arg)
		if err != nil {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(err)
			}
			r.conn.Close()
			return nil
		} else if n < int(args_size) {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(core.SNFErrorUninitialized{
					Component:         "\"Connection\"",
					RecommendedAction: "Communication failure with the server. Verify Server and re-try.",
				})
			}
			r.conn.Close()
			return nil
		}

		//FIXME Handle versioning correctly. Currwent anyversion is accepted.
		go r.handleRequests()
	}
	return r
}

func (r *SNFConnection) Opcodes() *core.SNFOpcodeRootStructure {
	return r.opcodes
}

func (r *SNFConnection) SendRequest(req *SNFRequest) bool {
	if r.once {
		return false
	}
	r.requestQueue <- req
	return true
}

func (r *SNFConnection) handleRequestsincoming() {
	var header_buffer [28]byte
	for {
		n, err := r.conn.Read(header_buffer[:])
		if err != nil {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(err)
			}
			r.conn.Close()
			return
		} else if n < 28 {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(core.SNFErrorUninitialized{
					Component:         "\"Connection\"",
					RecommendedAction: "Communication failure with the server. Verify Server and re-try.",
				})
			}
			r.conn.Close()
			return
		}

		rq, op, args_amount, args_size := snfRequestParseHeader(header_buffer[:])

		opcode := r.Opcodes().GetOpcode(
			op[0],
			op[1],
			op[2],
			op[3],
		)
		if opcode == nil {
			// FIXME handle invalid opcode better
			continue
		}
		argsbuff := make([]byte, args_size)
		true_args_amount := rq.snfRequestParseArguments(argsbuff)
		if true_args_amount != args_amount {
			continue
		}
		if item, ok := r.requestsSent[rq.cr.GetUID()]; ok {
			if item.onResponse != nil {
				go item.onResponse(*rq)
			}
		}
	}
}

func (r *SNFConnection) handleRequests() {
	go r.handleRequestsincoming()

	for {
		rq := <-r.requestQueue
		if _, err := r.conn.Write(rq.cr.ToBytes()); err != nil {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(err)
			}
			r.conn.Close()
			return
		}

		r.requestsSent[rq.cr.GetUID()] = rq

	}
}
