package Client

import (
	"context"
	"encoding/binary"
	"errors"
	"io"
	"net"
	"sync"
	"time"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

var DefaultTimeOut time.Duration = time.Minute
var DefaultRetries int = 0

type OnConnectCallback func()
type OnSNFFailCallback func(error)
type OnSocketFailCallback func(error)
type OnExceptionCallback func(error)
type OnTimeoutCallback func()

type ConnectionInfo struct {
	once         bool
	once_done    bool
	conn         net.Conn
	address      string
	uuid         string
	timeOut      time.Duration
	retries      int
	isConnected  bool
	requestQueue chan *Request
	requestsSent map[[16]byte]*Request
}

type ConnectionCallbacks struct {
	onConnectCallback   OnConnectCallback
	onExceptionCallback OnExceptionCallback
	onTimeoutCallback   OnTimeoutCallback
}

type Connection struct {
	ConnectionInfo
	ConnectionCallbacks
	opcodes *core.OpcodeRootStructure
	mapLock sync.RWMutex
	mu      sync.Mutex
}

func NewConnection() *Connection {
	ret := &Connection{}
	ret.timeOut = DefaultTimeOut
	ret.retries = DefaultRetries
	ret.requestQueue = make(chan *Request, 100)
	ret.requestsSent = make(map[[16]byte]*Request)
	ret.SetOpcodeStruct(nil)
	return ret
}

func (r *Connection) SetAddress(address string) *Connection {
	r.address = address
	return r
}

func (r *Connection) SetOpcodeStruct(op *core.OpcodeRootStructure) *Connection {
	if op == nil {
		r.opcodes = core.NewOpodeStructure(nil)
	} else {
		r.opcodes = op
	}
	return r
}

func (r *Connection) OPCodes() *core.OpcodeRootStructure {
	return r.opcodes
}

func (r *Connection) OnConnect(cb OnConnectCallback) *Connection {
	r.onConnectCallback = cb
	return r
}

func (r *Connection) OnException(cb OnExceptionCallback) *Connection {
	r.onExceptionCallback = cb
	return r
}

func (r *Connection) OnTimeout(cb OnTimeoutCallback) *Connection {
	r.onTimeoutCallback = cb
	return r
}

func (r *Connection) Once(req *Request) *Connection {
	r.once = true
	r.requestQueue <- req
	return r
}

func (r *Connection) Connect() *Connection {
	var err error
	dialer := net.Dialer{
		KeepAlive: 30 * time.Second,
	}
	failure := false
	retries := r.retries
	for {
		failure = false
		r.conn, err = dialer.Dial("tcp", r.address)
		if err != nil {
			failure = true
			if errors.Is(err, context.DeadlineExceeded) || (errors.Is(err, net.ErrClosed) && err.Error() == "i/o timeout") {
				if r.onTimeoutCallback != nil {
					r.onTimeoutCallback()
				}
			} else {
				if r.onExceptionCallback != nil {
					r.onExceptionCallback(err)
				}
			}
			if retries > 0 {
				retries--
				time.Sleep(time.Second)
				continue
			}
		}
		break
	}
	if failure {
		return nil
	}
	{
		buf := make([]byte, 12)

		if _, err := io.ReadFull(r.conn, buf); err != nil {
			r.handleError(err)
			return nil
		}

		op := r.opcodes.GetOpcode(
			buf[0],
			buf[1],
			buf[2],
			buf[3],
		)
		if op == nil || !op.IsBase() ||
			op.Command.GetValue() != core.SNF_OPCODE_BASE_CMD_CONNECT {
			r.handleError(core.SNFErrorUninitialized{
				Component:         "\"Connection\"",
				RecommendedAction: "Communication failure with the server. Verify compatibility with the Server and re-try.",
			})
			return nil
		}
		buf = buf[4:]

		args_amount := binary.BigEndian.Uint32(buf[:4])
		buf = buf[4:]

		args_size := binary.BigEndian.Uint32(buf[:4])

		if args_amount != 1 || args_size == 0 {
			r.handleError(core.SNFErrorUninitialized{
				Component:         "\"Connection\"",
				RecommendedAction: "Communication mixup failure with the server. Verify compatibility with the Server and re-try.",
			})
			return nil
		}

		arg := make([]byte, args_size)
		if _, err = io.ReadFull(r.conn, arg); err != nil {
			r.handleError(err)
			return nil
		}

		var confirmConnection []byte = []byte{
			0x00,
			0x00,
			core.SNF_OPCODE_BASE_CMD_CONNECT,
			0x00,
		}
		if _, err := r.conn.Write(confirmConnection); err != nil {
			r.handleError(err)
			return nil
		}

		opcodeBuf := make([]byte, 4)
		if _, err := io.ReadFull(r.conn, opcodeBuf); err != nil {
			r.handleError(err)
			return nil
		}
		opcode := r.Opcodes().GetOpcode(opcodeBuf[0], opcodeBuf[1], opcodeBuf[2], opcodeBuf[3])
		if opcode == nil || !opcode.IsBase() || opcode.Command.GetValue() != core.SNF_OPCODE_BASE_CMD_CONFIRM {
			r.handleError(core.SNFErrorUninitialized{
				Component:         "\"Connection\"",
				RecommendedAction: "Communication failure with the server. Verify compatibility with the Server and re-try.",
			})
			return nil
		}
		metaBuf := make([]byte, 8)
		if _, err := io.ReadFull(r.conn, metaBuf); err != nil {
			r.handleError(err)
			return nil
		}

		serverArgsCount := binary.BigEndian.Uint32(metaBuf[:4])
		serverArgsSize := binary.BigEndian.Uint32(metaBuf[4:])

		if serverArgsCount != 1 || serverArgsSize != 36 {
			r.handleError(core.SNFErrorUninitialized{
				Component:         "\"Connection\"",
				RecommendedAction: "Server protocol mismatch. Expected 1 Argument of size 36 (UUID).",
			})
			return nil
		}

		uuidBuf := make([]byte, 36)
		if _, err := io.ReadFull(r.conn, uuidBuf); err != nil {
			r.handleError(err)
			return nil
		}
		r.uuid = string(uuidBuf)
		if r.onConnectCallback != nil {
			r.onConnectCallback()
		}
		go r.handleRequests()
	}
	return r
}

func (r *Connection) handleError(err error) {
	if r.onExceptionCallback != nil {
		r.onExceptionCallback(err)
	}
	r.conn.Close()
}

func (r *Connection) Opcodes() *core.OpcodeRootStructure {
	return r.opcodes
}

func (r *Connection) NewRequest(req *core.Request) *Request {
	ret := Request{}
	ret.cr = *req
	return &ret
}

func (r *Connection) SendRequest(req *Request) bool {
	if r.once {
		if r.once_done {
			return false
		}
		r.once_done = true
	}
	req.cr.SetUID(generateUID())
	r.requestQueue <- req

	return true
}

func (r *Connection) handleRequestsincoming() {
	var header_buffer [28]byte
	for {
		if err := r.conn.SetReadDeadline(time.Time{}); err != nil {
			r.handleError(err)
			return
		}

		_, err := io.ReadFull(r.conn, header_buffer[:])
		if err != nil {
			r.handleError(err)
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
			if args_size > 0 {
				io.CopyN(io.Discard, r.conn, int64(args_size))
			}
			continue
		}
		argsbuff := make([]byte, args_size)

		_, err = io.ReadFull(r.conn, argsbuff)
		if err != nil {
			r.handleError(err)
			return
		}

		true_args_amount := rq.snfRequestParseArguments(argsbuff)
		if true_args_amount != args_amount {
			continue
		}

		r.mapLock.RLock()
		item, ok := r.requestsSent[rq.cr.GetUID()]
		r.mapLock.RUnlock()

		if ok {
			if item.onResponse != nil {
				go item.onResponse(*rq)
			}
		}
	}
}

func (r *Connection) handleRequests() {
	go r.handleRequestsincoming()

	for {
		rq := <-r.requestQueue
		ToSend := append([]byte(r.uuid), rq.cr.ToBytes()...)

		if _, err := r.conn.Write(ToSend); err != nil {
			if r.onExceptionCallback != nil {
				r.onExceptionCallback(err)
			}
			r.conn.Close()
			return
		}

		r.mapLock.Lock()
		r.requestsSent[rq.cr.GetUID()] = rq
		r.mapLock.Unlock()
	}
}
