//go:build !windows

package Client

import (
	"net"
)

func dialPipe(address string) (net.Conn, error) {
	return net.Dial("unix", address)
}
