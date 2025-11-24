package Server

import (
	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

func snfServerDefaultCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: core.SNFOpcodeGetBase(
			core.SNF_OPCODE_BASE_CMD_INVALID,
			core.SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE,
		),
	}, nil
}
func snfServerInvalidProtocolCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: core.SNFOpcodeGetBase(
			core.SNF_OPCODE_BASE_CMD_INVALID,
			core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL,
		),
	}, nil
}

func snfServerSetOpcodeCallbacks() error {
	if !SNFServerVarsIsInit() {
		return core.SNFErrorUninitialized{
			Component:         "Server Variables",
			RecommendedAction: "Do not call this manually",
		}
	}
	SetVar(SNF_VAR_CLTS_DEFAULTCB, snfServerDefaultCallBack)

	if core.SNFOpcodeBaseIsInit() {
		panic(core.SNFErrorAlreadyInitialized{
			Component:         "Opcode Base",
			RecommendedAction: "Never Call SNFOpcodeBaseInit()!!!",
		}.Error())
	} else if !core.SNFOpcodeBaseIsInit() && !core.SNFOpcodeBaseInit(snfServerDefaultCallBack) {
		return core.SNFErrorIntialization{FailedComponent: "SNFOpcodeBase"}
	}
	SetVar(SNF_VAR_INITIALIZED_OPCODE, true)
	var err error
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_CONNECT,
		snfServerCBConnect,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_RECONNECT,
		snfServerCBReconnect,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_DISCONNECT,
		snfServerCBDisonnect,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_VER_INF,
		snfServerCBVerInfo,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_KICK,
		snfServerCBKick,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_CONFIRM,
		snfServerCBConfirm,
	)
	if err != nil {
		return err
	}
	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_REJECT,
		snfServerCBReject,
	)
	if err != nil {
		return err
	}

	err = core.SNFOpcodeAddCommandCallback(
		core.SNF_OPCODE_BASE_CAT,
		core.SNF_OPCODE_BASE_SUBCAT,
		core.SNF_OPCODE_BASE_CMD_INVALID,
		snfServerCBInvalid,
	)
	if err != nil {
		return err
	}

	return nil
}

// snfServerCBConnect is the callback for the connect 0x00 base command
func snfServerCBConnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerInvalidProtocolCallBack(Original, Sender)
	return result, err
}

// snfServerCBReconnect is the callback for the reconnect 0x01 base command
func snfServerCBReconnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerInvalidProtocolCallBack(Original, Sender)
	return result, err
}

// snfServerCBDisonnect is the callback for the disconnect 0x02 base command
func snfServerCBDisonnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerInvalidProtocolCallBack(Original, Sender)
	return result, err
}

// snfServerCBVerInfo is the callback for the verinfo 0x03 base command
func snfServerCBVerInfo(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	if Original.OPCODE.Detail == core.SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL {
		return *core.SNFRequestGenResponse(
			&Original,
			core.SNFOpcodeGetUBase(
				core.SNF_OPCODE_BASE_CMD_CONFIRM,
			),
			core.SNFRequestArgGen(
				[]byte(core.SNFVersion))), nil
	}

	return *core.SNFRequestGenResponse(
		&Original,
		core.SNFOpcodeGetUBase(
			core.SNF_OPCODE_BASE_CMD_CONFIRM,
		),
		core.SNFRequestArgGen(
			[]byte(core.SNFVersion))), nil
}

// snfServerCBKick is the callback for the kick 0x04 base command
func snfServerCBKick(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return snfServerInvalidProtocolCallBack(Original, Sender)
}

// snfServerCBConfirm is the callback for the confirm 0x05 base command
func snfServerCBConfirm(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return snfServerInvalidProtocolCallBack(Original, Sender)
}

// snfServerCBReject is the callback for the reject 0x06 base command
func snfServerCBReject(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return snfServerInvalidProtocolCallBack(Original, Sender)
}

// snfServerCBInvalid is the callback for the invalid 0xFF base command
func snfServerCBInvalid(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return snfServerInvalidProtocolCallBack(Original, Sender)
}
