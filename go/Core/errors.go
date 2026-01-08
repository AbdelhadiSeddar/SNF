package Core

import (
	"fmt"
	"net"
)

// SNFErrorIntialization is the error returned when a critical component could not be initialized
type SNFErrorIntialization struct {
	FailedComponent string
}

func (e SNFErrorIntialization) Error() string {
	return fmt.Sprintf("Could not initialize %s", e.FailedComponent)
}

type SNFErrorUninitialized struct {
	Component         string
	RecommendedAction string `json:"RecommendedAction,omitempty"`
}

func (e SNFErrorUninitialized) Error() string {
	var str string = ""
	if len(e.RecommendedAction) > 0 {
		str = fmt.Sprintf(", Recommended Action: %s", e.RecommendedAction)
	}
	return fmt.Sprintf("Critical Component %s was not Initialized!!!%s", e.Component, str)
}

type SNFErrorAlreadyInitialized struct {
	Component         string
	RecommendedAction string `json:",omitempty"`
}

func (e SNFErrorAlreadyInitialized) Error() string {
	var str string = ""
	if len(e.RecommendedAction) > 0 {
		str = fmt.Sprintf(", Recommended Action: %s", e.RecommendedAction)
	}
	return fmt.Sprintf("Critical Component %s is Inexpectedly Initialized!!!%s. Leaving...", e.Component, str)
}

type SNFErrorUnallowedValue struct {
	Is          string
	ShouldvBeen string `json:",omitempty"`
}

func (e SNFErrorUnallowedValue) Error() string {
	var str string = ""
	if len(e.ShouldvBeen) > 0 {
		str = fmt.Sprintf("\n Should've been %s", e.ShouldvBeen)
	}
	return fmt.Sprintf("Value %s is Invalid %s", e.Is, str)
}

type SNFErrorUninitializedValue struct {
	OfWhat            string
	RecommendedAction string `json:",omitempty"`
}

func (e SNFErrorUninitializedValue) Error() string {
	var str string = ""
	if len(e.RecommendedAction) > 0 {
		str = fmt.Sprintf("\n Recommended Action: \n %s", e.RecommendedAction)
	}
	return fmt.Sprintf("Value of %s is Uninitialized %s", e.OfWhat, str)
}

// OPCode Errors

// SNFErrorOpcodeMemberNotFound is the error returned when a member is not found in the opcode structure
type SNFErrorOpcodeMemberNotFound struct {
	Category      byte
	SubCategory   byte
	Command       byte
	Detail        byte
	NotFoundValue byte
	NotFoundRank  OpcodeRank
}

func (e SNFErrorOpcodeMemberNotFound) Error() string {
	var Rank string = ""
	var under string = ""
	switch e.NotFoundRank {
	case OpcodeRankCategory:
		Rank = "Category"
	case OpcodeRankSubCategory:
		Rank = "Sub-Category"
		under = fmt.Sprintf("Under Category [0x%02x]", e.Category)
	case OpcodeRankCommand:
		Rank = "Command"
		under = fmt.Sprintf("Under Category [0x%02x] Sub-Category [0x%02x]", e.Category, e.SubCategory)
	case OpcodeRankDetail:
		Rank = "Detail"
		under = fmt.Sprintf("Under Category [0x%02x] Sub-Category [0x%02x] Command [0x%02x]", e.Category, e.SubCategory, e.Command)
	default:
		Rank = fmt.Sprintf("Unknown [0x%02x]", e.NotFoundRank)
	}

	return fmt.Sprintf("Could not find Member [0x%02x] with  %s Rank %s", e.NotFoundValue, Rank, under)
}

type SNFErrorOpcodeInvalid struct {
	OPCode [4]byte
}

func (e SNFErrorOpcodeInvalid) Error() string {
	return fmt.Sprintf("Could not find [0x%02x][0x%02x][0x%02x][0x%02x] OPCode",
		e.OPCode[0],
		e.OPCode[1],
		e.OPCode[2],
		e.OPCode[3])
}

// Client Errors

// SNFErrorClientMismatch is the error when there is a mismatch between a client's true uuid anbd the oens attached to their request
type SNFErrorClientMismatch struct {
	Conn net.Conn
}

func (e SNFErrorClientMismatch) Error() string {
	return fmt.Sprintf("Client on %s, Tried Spoofing their UUID", e.Conn.RemoteAddr().String())
}
