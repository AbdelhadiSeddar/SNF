package Core

const (
	SNFOpcodeRankCategory SNFOpcodeRank = iota
	SNFOpcodeRankSubCategory
	SNFOpcodeRankCommand
	SNFOpcodeRankDetail
)

var opcode_def_cb SNFOpcodeCommandCallback = nil

type SNFOpcodeMember struct {
	ref *snfOpcodeLLItem
}

func (o *SNFOpcodeMember) GetValue() byte {
	return o.ref.OPmmbr
}
func (o *SNFOpcodeMember) GetCallback() SNFOpcodeCommandCallback {
	return o.ref.Func
}

type SNFOpcode struct {
	Category    *SNFOpcodeMember
	SubCategory *SNFOpcodeMember
	Command     *SNFOpcodeMember
	Detail      *SNFOpcodeMember
}
type SNFOpcodeRank int

func (o SNFOpcode) ToBytes() []byte {
	return []byte{
		o.Category.ref.OPmmbr,
		o.SubCategory.ref.OPmmbr,
		o.Command.ref.OPmmbr,
		o.Detail.ref.OPmmbr,
	}
}
func SNFOpcodeParse(data [4]byte) (*SNFOpcode, error) {
	var op *SNFOpcode

	det, ok := SNFOpcodeGetDetail(data[0], data[1], data[2], data[3])
	if !ok {
		return nil, SNFErrorOpcodeInvalid{
			OPCode: data,
		}
	}

	op = &SNFOpcode{}
	op.Detail = det
	op.Command = &SNFOpcodeMember{ref: det.ref.parent}
	op.SubCategory = &SNFOpcodeMember{ref: op.Command.ref.parent}
	op.Category = &SNFOpcodeMember{ref: op.SubCategory.ref.parent}

	return op, nil
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

// true -> ok | false -> EEXIST
func SNFOpcodeDefineCategory(code byte) bool {
	item := &snfOpcodeLLItem{OPmmbr: code}
	re := snf_opcode_ll
	if re == nil {
		snf_opcode_ll = item
		return true
	}

	for re != nil {
		if re.next == nil {
			re.next = item
			return true
		} else if re.next.OPmmbr == item.OPmmbr {
			return false
		}
		re = re.next
	}
	return true
}

func SNFOpcodeDefineSubCategory(category byte, code byte) bool {
	item := &snfOpcodeLLItem{OPmmbr: code}
	if snf_opcode_ll == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	sub := snfOpcodeGetCategory(category)
	if sub == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return true
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return true
		} else if s.next.OPmmbr == item.OPmmbr {
			return false
		}
		s = s.next
	}
	return true
}

func SNFOpcodeDefineCommand(category byte, subCategory byte, code byte) bool {
	item := &snfOpcodeLLItem{OPmmbr: code}
	item.sub = &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED, parent: item}
	if snf_opcode_ll == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	cat := snfOpcodeGetCategory(category)
	if cat == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	sub := snfOpcodeGetSubCategory(cat, subCategory)
	if sub == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return true
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return true
		} else if s.next.OPmmbr == item.OPmmbr {
			return false
		}
		s = s.next
	}
	return true
}

func SNFOpcodeDefineDetail(category byte, subCategory byte, command byte, code byte) bool {
	item := &snfOpcodeLLItem{OPmmbr: code}
	if snf_opcode_ll == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	tmp, ok := SNFOpcodeGetSubCategory(category, subCategory)
	if !ok {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	sub := snfOpcodeGetCommand(tmp.ref, command)
	if sub == nil {
		panic(
			SNFErrorUninitialized{
				Component: "OPcodes Undefined",
			}.Error(),
		)
	}
	item.parent = sub
	if sub.sub == nil {
		sub.sub = item
		return true
	}
	s := sub.sub
	for s != nil {
		if s.next == nil {
			s.next = item
			return true
		} else if s.next.OPmmbr == item.OPmmbr {
			return false
		}
		s = s.next
	}
	return true
}

func SNFOpcodeAddCommandCallback(category byte, subCategory byte, command byte, cb SNFOpcodeCommandCallback) error {
	item, ok := SNFOpcodeGetCommand(category, subCategory, command)
	if !ok {
		return &SNFErrorOpcodeMemberNotFound{
			NotFoundValue: command,
			NotFoundRank:  SNFOpcodeRankCommand,
			Category:      category,
			SubCategory:   subCategory}
	}
	item.ref.Func = cb
	return nil
}

func snfOpcodeGetCategory(category byte) *snfOpcodeLLItem {
	srch := snf_opcode_ll
	for srch != nil {
		if srch.OPmmbr == category {
			break
		}
		srch = srch.next
	}
	return srch
}
func SNFOpcodeGetCategory(category byte) (*SNFOpcodeMember, bool) {
	srch := snfOpcodeGetCategory(category)
	if srch == nil {
		return nil, false
	}
	return &SNFOpcodeMember{ref: srch}, true
}

func snfOpcodeGetSubCategory(parent *snfOpcodeLLItem, subCategory byte) *snfOpcodeLLItem {
	srch := parent.sub
	for srch != nil {
		if srch.OPmmbr == subCategory {
			break
		}
		srch = srch.next
	}
	return srch
}
func SNFOpcodeGetSubCategory(category byte, subCategory byte) (*SNFOpcodeMember, bool) {
	srch := snfOpcodeGetSubCategory(
		snfOpcodeGetCategory(category),
		subCategory,
	)
	if srch != nil {
		return nil, false
	}
	return &SNFOpcodeMember{ref: srch}, true
}

func snfOpcodeGetCommand(parent *snfOpcodeLLItem, command byte) *snfOpcodeLLItem {
	srch := parent.sub
	for srch != nil {
		if srch.OPmmbr == command {
			break
		}
		srch = srch.next
	}
	return srch
}
func SNFOpcodeGetCommand(category byte, subCategory byte, command byte) (*SNFOpcodeMember, bool) {
	srch, ok := SNFOpcodeGetSubCategory(category, subCategory)
	if !ok {
		return nil, false
	}
	cmd := snfOpcodeGetCommand(srch.ref, command)
	if cmd == nil {
		return nil, false
	}
	return &SNFOpcodeMember{ref: cmd}, true
}

func snfOpcodeGetDetail(parent *snfOpcodeLLItem, detail byte) *snfOpcodeLLItem {
	srch := parent.sub
	for srch != nil {
		if srch.OPmmbr == detail {
			break
		}
		srch = srch.next
	}
	return srch
}
func SNFOpcodeGetDetail(category byte, subCategory byte, command byte, detail byte) (*SNFOpcodeMember, bool) {
	srch, ok := SNFOpcodeGetCommand(category, subCategory, command)
	if !ok {
		return nil, false
	}
	det := snfOpcodeGetDetail(srch.ref, detail)
	if det == nil {
		return nil, false
	}
	return &SNFOpcodeMember{ref: det}, true
}

func SNFOpcodeGet(category byte, subCategory byte, command byte, detail byte) *SNFOpcode {
	var cat, subcat, cmd, det *snfOpcodeLLItem
	// TODO: Make unique Errors to know which OPcodeMember is defined or not.
	if cat = snfOpcodeGetCategory(category); cat == nil {
		return nil
	}
	if subcat = snfOpcodeGetSubCategory(cat, subCategory); subcat == nil {
		return nil
	}
	if cmd = snfOpcodeGetCommand(subcat, command); cmd == nil {
		return nil
	}
	if det = snfOpcodeGetDetail(cmd, detail); det == nil {
		return nil
	}

	re := &SNFOpcode{}
	re.Category = &SNFOpcodeMember{ref: cat}
	re.SubCategory = &SNFOpcodeMember{ref: subcat}
	re.Command = &SNFOpcodeMember{ref: cmd}
	re.Detail = &SNFOpcodeMember{ref: det}
	return re
}

func SNFOpcodeGetU(category byte, subCategory byte, command byte) *SNFOpcode {
	return SNFOpcodeGet(category, subCategory, command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

// Checks if it is a Base Opcode
func SNFOpcodeIsBase(op SNFOpcode) bool {
	if op.Category.ref.OPmmbr == SNF_OPCODE_BASE_CAT {
		return false
	}
	if op.SubCategory.ref.OPmmbr != SNF_OPCODE_BASE_SUBCAT {
		return false
	}
	return true
}

func SNFOpcodeGetBaseCategory() (*SNFOpcodeMember, bool) {
	return SNFOpcodeGetCategory(SNF_OPCODE_BASE_CAT)
}

func SNFOpcodeGetBaseSubCategory() (*SNFOpcodeMember, bool) {
	return SNFOpcodeGetSubCategory(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT)
}

func SNFOpcodeGetBaseCommand(command byte) (*SNFOpcodeMember, bool) {
	return SNFOpcodeGetCommand(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command)
}

func SNFOpcodeGetBaseDetail(command byte, detail byte) (*SNFOpcodeMember, bool) {
	return SNFOpcodeGetDetail(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func SNFOpcodeGetBase(command byte, detail byte) *SNFOpcode {
	return SNFOpcodeGet(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func SNFOpcodeGetUBase(command byte) *SNFOpcode {
	return SNFOpcodeGetBase(command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

func snfOpcodeDefineBase() bool {

	root := &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_CAT}
	snf_opcode_ll = root

	sub := &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_SUBCAT, parent: root}
	root.sub = sub

	cmdConnect := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_CONNECT,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func: opcode_def_cb}
	sub.sub = cmdConnect

	cmdRecon := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_RECONNECT,
		parent: sub,
		sub:    &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func:   opcode_def_cb}
	cmdConnect.next = cmdRecon

	cmdDisc := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_DISCONNECT,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func: opcode_def_cb}
	cmdRecon.next = cmdDisc

	cmdVer := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_VER_INF,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL},
		Func: opcode_def_cb}
	cmdDisc.next = cmdVer

	cmdKick := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_KICK,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func: opcode_def_cb}
	cmdVer.next = cmdKick

	cmdConf := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_CONFIRM,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func: opcode_def_cb}
	cmdKick.next = cmdConf

	cmdRej := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_REJECT,
		parent: sub,
		sub: &snfOpcodeLLItem{
			OPmmbr: SNF_OPCODE_BASE_DET_UNDETAILED},
		Func: opcode_def_cb}
	cmdConf.next = cmdRej

	cmdInv := &snfOpcodeLLItem{
		OPmmbr: SNF_OPCODE_BASE_CMD_INVALID,
		parent: sub,
		Func:   opcode_def_cb}
	cmdRej.next = cmdInv

	detUnreg := &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE, parent: cmdInv}
	cmdInv.sub = detUnreg

	detErr := &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL, parent: cmdInv}
	detUnreg.next = detErr

	detUnimpl := &snfOpcodeLLItem{OPmmbr: SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE, parent: cmdInv}
	detErr.next = detUnimpl

	snf_opcode_base_isinit = true
	return true
}
func SNFOpcodeBaseInit(def SNFOpcodeCommandCallback) bool {
	if snf_opcode_base_isinit == true {
		return true
	}
	opcode_def_cb = def
	return snfOpcodeDefineBase()
}

func SNFOpcodeBaseIsInit() bool {
	return snf_opcode_base_isinit
}
