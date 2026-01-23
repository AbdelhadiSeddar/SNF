//go:build windows

package Client

import (
	"net"

	"github.com/Microsoft/go-winio"
)

func dialPipe(address string) (net.Conn, error) {
	return winio.DialPipe(address, nil)
}
