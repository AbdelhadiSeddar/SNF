package Core

import (
	"fmt"
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
	RecommendedAction string `json:",omitempty"`
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
	Is       string
	Shouldbe string `json:",omitempty"`
}

func (e SNFErrorUnallowedValue) Error() string {
	var str string = ""
	if len(e.Shouldbe) > 0 {
		str = fmt.Sprintf("\n Should've been: \n %s", e.Shouldbe)
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
	NotFoundRank  SNFOpcodeRank
}

func (e SNFErrorOpcodeMemberNotFound) Error() string {
	var Rank string = ""
	var under string = ""
	switch e.NotFoundRank {
	case SNFOpcodeRankCategory:
		Rank = "Category"
	case SNFOpcodeRankSubCategory:
		Rank = "Sub-Category"
		under = fmt.Sprintf("Under Category [0x%02x]", e.Category)
	case SNFOpcodeRankCommand:
		Rank = "Command"
		under = fmt.Sprintf("Under Category [0x%02x] Sub-Category [0x%02x]", e.Category, e.SubCategory)
	case SNFOpcodeRankDetail:
		Rank = "Detail"
		under = fmt.Sprintf("Under Category [0x%02x] Sub-Category [0x%02x] Command [0x%02x]", e.Category, e.SubCategory, e.Command)
	default:
		Rank = fmt.Sprintf("Unknown [0x%02x]", e.NotFoundRank)
	}

	return fmt.Sprintf("Could not find Member [0x%02x] with  %s Rank %s", e.NotFoundValue, Rank, under)
}
