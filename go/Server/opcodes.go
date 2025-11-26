package Server

import (
	"fmt"

	core "github.com/AbdelhadiSeddar/SNF/go/Core"
)

var snfOPStruct *core.SNFOpcodeRootStructure

func snfServerDefaultCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: snfOPStruct.GetBaseOpcode(
			core.SNF_OPCODE_BASE_CMD_INVALID,
			core.SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE,
		),
	}, nil
}
func snfServerInvalidProtocolCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: snfOPStruct.GetBaseOpcode(
			core.SNF_OPCODE_BASE_CMD_INVALID,
			core.SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL,
		),
	}, nil
}
func snfServerConfirmCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: snfOPStruct.GetUBaseOpcode(
			core.SNF_OPCODE_BASE_CMD_CONFIRM,
		),
	}, nil
}
func snfServerRejectCallBack(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	return core.SNFRequest{
		UID: Original.UID,
		OPCODE: snfOPStruct.GetUBaseOpcode(
			core.SNF_OPCODE_BASE_CMD_REJECT,
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

	if snfOPStruct != nil {
		panic(core.SNFErrorAlreadyInitialized{
			Component:         "Opcode Base",
			RecommendedAction: "Never Call SNFOpcodeBaseInit()!!!",
		}.Error())
	}
	snfOPStruct = core.SNFNewOpodeStructure(snfServerDefaultCallBack)
	SetVar(SNF_VAR_INITIALIZED_OPCODE, true)
	cmd := snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_CONNECT,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_CONNECT),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBConnect,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_RECONNECT,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_RECONNECT),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBReconnect,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_DISCONNECT,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_DISCONNECT),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBDisonnect,
	)
	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_VER_INF,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_VER_INF),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBVerInfo,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_KICK,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_KICK),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBKick,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_CONFIRM,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_CONFIRM),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBConfirm,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_REJECT,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_REJECT),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBReject,
	)

	cmd = snfOPStruct.GetBaseCommand(
		core.SNF_OPCODE_BASE_CMD_INVALID,
	)
	if cmd == nil {
		panic(
			core.SNFErrorUninitializedValue{
				OfWhat: fmt.Sprintf("Command [0x%02x]", core.SNF_OPCODE_BASE_CMD_INVALID),
			}.Error(),
		)
	}
	cmd.SetCallback(
		snfServerCBInvalid,
	)

	return nil
}

// snfServerCBConnect is the callback for the connect 0x00 base command
func snfServerCBConnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerInvalidProtocolCallBack(Original, Sender)
	return result, err
}

// snfServerCBReconnect is the callback for the reconnect 0x01 base command
func snfServerCBReconnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerConfirmCallBack(Original, Sender)
	return result, err
}

// snfServerCBDisonnect is the callback for the disconnect 0x02 base command
func snfServerCBDisonnect(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	result, err := snfServerInvalidProtocolCallBack(Original, Sender)
	return result, err
}

// snfServerCBVerInfo is the callback for the verinfo 0x03 base command
func snfServerCBVerInfo(Original core.SNFRequest, Sender interface{}) (core.SNFRequest, error) {
	if Original.OPCODE.Detail.GetValue() == core.SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL {
		return *core.SNFRequestGenResponse(
			&Original,
			snfOPStruct.GetUBaseOpcode(
				core.SNF_OPCODE_BASE_CMD_CONFIRM,
			),
			core.SNFRequestArgGen(
				[]byte(core.SNFVersion))), nil
	}

	return *core.SNFRequestGenResponse(
		&Original,
		snfOPStruct.GetUBaseOpcode(
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
