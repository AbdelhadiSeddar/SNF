package Core

import (
	"errors"
)

type SNFOpcode struct {
	Category    byte
	SubCategory byte
	Command     byte
	Detail      byte
}
type SNFOpcodeRank int

const (
	SNFOpcodeRankCategory SNFOpcodeRank = iota
	SNFOpcodeRankSubCategory
	SNFOpcodeRankCommand
	SNFOpcodeRankDetail
)

func (o SNFOpcode) ToBytes() []byte {
	return []byte{o.Category, o.SubCategory, o.Command, o.Detail}
}
func SNFOpcodeParse(data []byte) (SNFOpcode, error) {
	if len(data) != 4 {
		return SNFOpcode{}, errors.New("opcode must be 4 bytes long")
	}
	return SNFOpcode{data[0], data[1], data[2], data[3]}, nil
}

const SNF_OPCODE_BASE_CAT = 0x00
const SNF_OPCODE_BASE_SUBCAT = 0x00
const SNF_OPCODE_BASE_DET_UNDETAILED = 0x00

const (
	SNF_OPCODE_BASE_CMD_CONNECT           = 0x00
	SNF_OPCODE_BASE_DET_CONNECT_ONESHOT   = 0x01
	SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT = 0x02
)
const SNF_OPCODE_BASE_CMD_RECONNECT = 0x01
const SNF_OPCODE_BASE_CMD_DISCONNECT = 0x02
const (
	SNF_OPCODE_BASE_CMD_VER_INF          = 0x03
	SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL = 0x01
)
const SNF_OPCODE_BASE_CMD_KICK = 0x04
const SNF_OPCODE_BASE_CMD_CONFIRM = 0x05
const SNF_OPCODE_BASE_CMD_REJECT = 0x06
const (
	SNF_OPCODE_BASE_CMD_INVALID                      = 0xFF
	SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE   = 0x01
	SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL       = 0x02
	SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE = 0x03
)

type SNFOpcodeCommandCallback func(Original SNFRequest, Sender interface{}) (SNFRequest, error)
type snfOpcodeLLItem struct {
	OPmmbr byte
	Func   SNFOpcodeCommandCallback
	next   *snfOpcodeLLItem
	parent *snfOpcodeLLItem
	sub    *snfOpcodeLLItem
}

var snf_opcode_ll *snfOpcodeLLItem
var snf_opcode_base_isinit bool = false

func SNFOpcodeDefineCategory(code byte) int {
	item := &snfOpcodeLLItem{OPmmbr: code}
	re := snf_opcode_ll
	if re == nil {
		snf_opcode_ll = item
		return 0
	}

	for re != nil {
		if re.next == nil {
			re.next = item
			return 0
		} else if re.next.OPmmbr == item.OPmmbr {
			return 1
		}
		re = re.next
	}
	return 0
}

func SNFOpcodeDefineSubCategory(category byte, code byte) int {
	item := &snfOpcodeLLItem{OPmmbr: code}
	if snf_opcode_ll == nil {
		return -2
	}
	sub := SNFOpcodeGetCategory(category)
	if sub == nil {
		return -2
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return 0
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return 0
		} else if s.next.OPmmbr == item.OPmmbr {
			return 1
		}
		s = s.next
	}
	return 0
}

func SNFOpcodeDefineCommand(category byte, subCategory byte, code byte) int {
	item := &snfOpcodeLLItem{OPmmbr: code}
	item.sub = &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED, parent: item}
	if snf_opcode_ll == nil {
		return -2
	}
	sub := SNFOpcodeGetSubCategory(category, subCategory)
	if sub == nil {
		return -2
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return 0
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return 0
		} else if s.next.OPmmbr == item.OPmmbr {
			return 1
		}
		s = s.next
	}
	return 0
}

func SNFOpcodeDefineDetail(category byte, subCategory byte, command byte, code byte) int {
	item := &snfOpcodeLLItem{OPmmbr: code}
	if snf_opcode_ll == nil {
		return -2
	}
	sub := SNFOpcodeGetCommand(category, subCategory, command)
	if sub == nil {
		return -2
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return 0
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return 0
		} else if s.next.OPmmbr == item.OPmmbr {
			return 1
		}
		s = s.next
	}
	return 0
}

func SNFOpcodeAddCommandCallback(category byte, subCategory byte, command byte, cb SNFOpcodeCommandCallback) error {
	item := SNFOpcodeGetCommand(category, subCategory, command)
	if item == nil {
		return &SNFErrorOpcodeMemberNotFound{
			NotFoundValue: command,
			NotFoundRank:  SNFOpcodeRankCommand,
			Category:      category,
			SubCategory:   subCategory}
	}
	item.Func = cb
	return nil
}

func SNFOpcodeGetCategory(category byte) *snfOpcodeLLItem {
	srch := snf_opcode_ll
	for srch != nil {
		if srch.OPmmbr == category {
			break
		}
		srch = srch.next
	}
	return srch
}

func SNFOpcodeGetSubCategory(category byte, subCategory byte) *snfOpcodeLLItem {
	srch := SNFOpcodeGetCategory(category)
	if srch == nil {
		return nil
	}
	srch = srch.sub
	for srch != nil {
		if srch.OPmmbr == subCategory {
			break
		}
		srch = srch.next
	}
	return srch
}

func SNFOpcodeGetCommand(category byte, subCategory byte, command byte) *snfOpcodeLLItem {
	srch := SNFOpcodeGetSubCategory(category, subCategory)
	if srch == nil {
		return nil
	}
	srch = srch.sub
	for srch != nil {
		if srch.OPmmbr == command {
			break
		}
		srch = srch.next
	}
	return srch
}

func SNFOpcodeGetDetail(category byte, subCategory byte, command byte, detail byte) *snfOpcodeLLItem {
	srch := SNFOpcodeGetCommand(category, subCategory, command)
	if srch == nil {
		return nil
	}
	srch = srch.sub
	for srch != nil {
		if srch.OPmmbr == detail {
			break
		}
		srch = srch.next
	}
	return srch
}

func SNFOpcodeGet(category byte, subCategory byte, command byte, detail byte) *SNFOpcode {
	item := SNFOpcodeGetDetail(category, subCategory, command, detail)
	if item == nil {
		return nil
	}
	re := &SNFOpcode{}
	re.Category = category
	re.SubCategory = subCategory
	re.Command = command
	re.Detail = detail
	return re
}

func SNFOpcodeGetU(category byte, subCategory byte, command byte) *SNFOpcode {
	return SNFOpcodeGet(category, subCategory, command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

func SNFOpcodeIsBase(op *SNFOpcode) int {
	if op.Category != SNF_OPCODE_BASE_CAT {
		return 0
	}
	if op.SubCategory != SNF_OPCODE_BASE_SUBCAT {
		return 0
	}
	if (op.Command < SNF_OPCODE_BASE_CMD_CONNECT || op.Command > SNF_OPCODE_BASE_CMD_REJECT) || op.Command != SNF_OPCODE_BASE_CMD_INVALID {
		return 0
	}
	return 1
}

func SNFOpcodeGetBaseCategory() snfOpcodeLLItem {
	return *SNFOpcodeGetCategory(SNF_OPCODE_BASE_CAT)
}

func SNFOpcodeGetBaseSubCategory() snfOpcodeLLItem {
	return *SNFOpcodeGetSubCategory(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT)
}

func SNFOpcodeGetBaseCommand(command byte) snfOpcodeLLItem {
	return *SNFOpcodeGetCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command)
}

func SNFOpcodeGetBaseDetail(command byte, detail byte) snfOpcodeLLItem {
	return *SNFOpcodeGetDetail(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func SNFOpcodeGetBase(command byte, detail byte) *SNFOpcode {
	return SNFOpcodeGet(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func SNFOpcodeGetUBase(command byte) *SNFOpcode {
	return SNFOpcodeGetBase(command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

func snfOpcodeDefineBase() bool {
	if SNFOpcodeDefineCategory(SNF_OPCODE_BASE_CAT) < 0 {
		return false
	}
	if SNFOpcodeDefineSubCategory(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_CONNECT) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_RECONNECT) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_DISCONNECT) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_VER_INF) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_KICK) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_CONFIRM) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_REJECT) < 0 {
		return false
	}
	if SNFOpcodeDefineCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_INVALID) < 0 {
		return false
	}
	if SNFOpcodeDefineDetail(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_INVALID, SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE) < 0 {
		return false
	}
	if SNFOpcodeDefineDetail(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_INVALID, SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL) < 0 {
		return false
	}
	if SNFOpcodeDefineDetail(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, SNF_OPCODE_BASE_CMD_INVALID, SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE) < 0 {
		return false
	}
	snf_opcode_base_isinit = true
	return true
}

func SNFOpcodeBaseInit() bool {
	if snf_opcode_base_isinit == true {
		return true
	}
	return snfOpcodeDefineBase()
}

func SNFOpcodeBaseIsInit() bool {
	return snf_opcode_base_isinit
}
