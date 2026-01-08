package Core

import (
	"fmt"
	"strings"
)

const (
	OpcodeRankCategory OpcodeRank = iota
	OpcodeRankSubCategory
	OpcodeRankCommand
	OpcodeRankDetail
)

var opcode_def_cb OpcodeCommandCallback = nil

type OpcodeRootStructure struct {
	categories map[byte]*OpcodeCategoryMember
	isInit     bool
	defaultCB  OpcodeCommandCallback
}

func (root *OpcodeRootStructure) DefineCategory(code byte) *OpcodeCategoryMember {
	if _, ok := root.categories[code]; !ok {
		item := OpcodeCategoryMember{
			subCategories: make(map[byte]*OpcodeSubCategoryMember),
		}
		item.SetValue(code)
		item.root = root
		root.categories[code] = &item
		return &item
	}
	return nil
}
func (root *OpcodeRootStructure) GetCategory(code byte) *OpcodeCategoryMember {
	return root.categories[code]
}

func (root *OpcodeRootStructure) DefineDefaultCallback(cb OpcodeCommandCallback) {
	root.defaultCB = cb
}
func (st *OpcodeRootStructure) GetOpcode(category byte, subCategory byte, command byte, detail byte) *Opcode {
	var cat *OpcodeCategoryMember
	var scat *OpcodeSubCategoryMember
	var cmd *OpcodeCommandMember
	var det *OpcodeDetailMember
	// TODO: Make unique Errors to know which OPcodeMember is defined or not.
	if cat = st.GetCategory(category); cat == nil {
		return nil
	}
	if scat = cat.GetSubCategory(subCategory); scat == nil {
		return nil
	}
	if cmd = scat.GetCommand(command); cmd == nil {
		return nil
	}
	if det = cmd.GetDetail(detail); det == nil {
		return nil
	}

	re := &Opcode{}
	re.Category = cat
	re.SubCategory = scat
	re.Command = cmd
	re.Detail = det
	re.parentStruct = st
	return re
}

type Opcode struct {
	parentStruct *OpcodeRootStructure
	Category     *OpcodeCategoryMember
	SubCategory  *OpcodeSubCategoryMember
	Command      *OpcodeCommandMember
	Detail       *OpcodeDetailMember
}

func (o Opcode) ToBytes() []byte {
	return []byte{
		o.Category.GetValue(),
		o.SubCategory.GetValue(),
		o.Command.GetValue(),
		o.Detail.GetValue(),
	}
}

func (root *OpcodeRootStructure) OpcodeParse(data [4]byte) (*Opcode, error) {
	var op *Opcode

	op = root.GetOpcode(data[0], data[1], data[2], data[3])
	if op == nil {
		return nil, SNFErrorOpcodeInvalid{
			OPCode: data,
		}
	}

	return op, nil
}

type OpcodeRank int

// /
type OpcodeMember struct {
	val byte
}

func (o *OpcodeMember) GetValue() byte {
	return o.val
}
func (o *OpcodeMember) SetValue(v byte) {
	o.val = v
}

type OpcodeCategoryMember struct {
	OpcodeMember
	root          *OpcodeRootStructure
	subCategories map[byte]*OpcodeSubCategoryMember
}

func (p *OpcodeCategoryMember) DefineSubCategory(code byte) *OpcodeSubCategoryMember {
	if _, ok := p.subCategories[code]; !ok {
		item := OpcodeSubCategoryMember{
			commands: make(map[byte]*OpcodeCommandMember),
		}
		item.SetValue(code)
		item.parent = p
		p.subCategories[code] = &item
		return &item
	}
	return nil
}
func (cat *OpcodeCategoryMember) GetSubCategory(code byte) *OpcodeSubCategoryMember {
	return cat.subCategories[code]
}

type OpcodeSubCategoryMember struct {
	OpcodeMember
	parent   *OpcodeCategoryMember
	commands map[byte]*OpcodeCommandMember
}

func (p *OpcodeSubCategoryMember) DefineCommand(code byte, callback OpcodeCommandCallback) *OpcodeCommandMember {
	if _, ok := p.commands[code]; !ok {
		item := OpcodeCommandMember{
			details: make(map[byte]*OpcodeDetailMember),
		}
		item.SetValue(code)
		item.parent = p
		item.details[byte(0)] = &OpcodeDetailMember{
			parent: &item,
		}
		item.details[byte(0)].SetValue(byte(0))
		if callback == nil {
			item.f = p.parent.root.defaultCB
		} else {
			item.f = callback
		}
		p.commands[code] = &item
		return &item
	}
	return nil
}
func (p *OpcodeSubCategoryMember) GetCommand(code byte) *OpcodeCommandMember {
	return p.commands[code]
}

type OpcodeCommandMember struct {
	OpcodeMember
	f       OpcodeCommandCallback
	parent  *OpcodeSubCategoryMember
	details map[byte]*OpcodeDetailMember
}

func (p *OpcodeCommandMember) DefineDetail(code byte) *OpcodeDetailMember {
	if _, ok := p.details[code]; !ok {
		item := OpcodeDetailMember{}
		item.SetValue(code)
		item.parent = p
		p.details[code] = &item
		return &item
	}
	return nil
}
func (o *OpcodeCommandMember) SetCallback(cb OpcodeCommandCallback) {
	o.f = cb
}
func (o *OpcodeCommandMember) GetCallback() OpcodeCommandCallback {
	return o.f
}
func (p *OpcodeCommandMember) GetDetail(code byte) *OpcodeDetailMember {
	return p.details[code]
}

type OpcodeDetailMember struct {
	OpcodeMember
	parent *OpcodeCommandMember
}

type OpcodeCommandCallback func(Original Request, Sender any) (Request, error)

// var snf_opcode_ll *snfOpcodeLLItem
// var snf_opcode_base_isinit bool = false

// if already defined return nil
// Checks if it is a Base Opcode
func (op Opcode) IsBase() bool {
	if op.Category.GetValue() != SNF_OPCODE_BASE_CAT {
		return false
	}
	if op.SubCategory.GetValue() != SNF_OPCODE_BASE_SUBCAT {
		return false
	}
	return true
}
func (st *OpcodeRootStructure) GetUOpcode(category byte, subCategory byte, command byte) *Opcode {
	return st.GetOpcode(category, subCategory, command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

func (st *OpcodeRootStructure) GetBaseCategory() *OpcodeCategoryMember {
	return st.categories[byte(0)]
}

func (st *OpcodeRootStructure) GetBaseSubCategory() *OpcodeSubCategoryMember {
	return st.categories[byte(0)].subCategories[byte(0)]
}

func (st *OpcodeRootStructure) GetBaseCommand(command byte) *OpcodeCommandMember {
	return st.categories[byte(0)].subCategories[byte(0)].commands[command]
}

func (st *OpcodeRootStructure) GetBaseDetail(command byte, detail byte) *OpcodeDetailMember {
	return st.categories[byte(0)].subCategories[byte(0)].commands[command].details[detail]
}

func (st *OpcodeRootStructure) GetBaseOpcode(command byte, detail byte) *Opcode {
	return st.GetOpcode(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func (st *OpcodeRootStructure) GetUBaseOpcode(command byte) *Opcode {
	return st.GetBaseOpcode(command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

// Defiens a new Root Structure for OPcodes. whilst assuing base functionalities are maintained
func NewOpodeStructure(def_cb OpcodeCommandCallback) *OpcodeRootStructure {
	root := &OpcodeRootStructure{
		categories: make(map[byte]*OpcodeCategoryMember),
		isInit:     true,
		defaultCB:  def_cb,
	}

	cat := root.DefineCategory(SNF_OPCODE_BASE_CAT)
	if cat == nil {
		return nil
	}

	scat := cat.DefineSubCategory(SNF_OPCODE_BASE_SUBCAT)
	if scat == nil {
		return nil
	}

	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_CONNECT, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_RECONNECT, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_DISCONNECT, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_VER_INF, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_KICK, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_CONFIRM, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_REJECT, def_cb); cmd == nil {
		return nil
	}
	if cmd := scat.DefineCommand(SNF_OPCODE_BASE_CMD_INVALID, def_cb); cmd == nil {
		return nil
	}

	// define details for commands that need them
	cmdConnect := scat.GetCommand(SNF_OPCODE_BASE_CMD_CONNECT)
	if cmdConnect != nil {
		cmdConnect.DefineDetail(SNF_OPCODE_BASE_DET_UNDETAILED)
		cmdConnect.DefineDetail(SNF_OPCODE_BASE_DET_CONNECT_ONESHOT)
		cmdConnect.DefineDetail(SNF_OPCODE_BASE_DET_CONNECT_MULTISHOT)
	} else {
		return nil
	}

	cmdVer := scat.GetCommand(SNF_OPCODE_BASE_CMD_VER_INF)
	if cmdVer != nil {
		cmdVer.DefineDetail(SNF_OPCODE_BASE_DET_VER_INF_VER_IMPL)
	} else {
		return nil
	}

	cmdInvalid := scat.GetCommand(SNF_OPCODE_BASE_CMD_INVALID)
	if cmdInvalid != nil {
		cmdInvalid.DefineDetail(SNF_OPCODE_BASE_DET_INVALID_UNREGISTRED_OPCODE)
		cmdInvalid.DefineDetail(SNF_OPCODE_BASE_DET_INVALID_ERROR_PROTOCOL)
		cmdInvalid.DefineDetail(SNF_OPCODE_BASE_DET_INVALID_UNIMPLEMENTED_OPCODE)
	}

	return root
}
func SNFOpcodePrint(root OpcodeRootStructure) string {
	var sb strings.Builder
	sb.WriteString("SNFOpcodeRootStructure{\n")
	sb.WriteString(fmt.Sprintf("  isInit: %v\n", root.isInit))
	sb.WriteString("  categories: {\n")
	for catCode, cat := range root.categories {
		sb.WriteString(fmt.Sprintf("    0x%02X: SNFOpcodeCategoryMember{\n", catCode))
		sb.WriteString(fmt.Sprintf("      val: 0x%02X\n", cat.val))
		sb.WriteString("      subCategories: {\n")
		for scCode, sc := range cat.subCategories {
			sb.WriteString(fmt.Sprintf("        0x%02X: SNFOpcodeSubCategoryMember{\n", scCode))
			sb.WriteString(fmt.Sprintf("          val: 0x%02X\n", sc.val))
			sb.WriteString("          commands: {\n")
			for cmdCode, cmd := range sc.commands {
				sb.WriteString(fmt.Sprintf("            0x%02X: SNFOpcodeCommandMember{\n", cmdCode))
				sb.WriteString(fmt.Sprintf("              val: 0x%02X\n", cmd.val))
				if cmd.f != nil {
					sb.WriteString("              callback: <set>\n")
				} else {
					sb.WriteString("              callback: <nil>\n")
				}
				sb.WriteString("              details: {\n")
				for detCode, det := range cmd.details {
					sb.WriteString(fmt.Sprintf("                0x%02X: SNFOpcodeDetailMember{ val: 0x%02X }\n", detCode, det.val))
				}
				sb.WriteString("              }\n")
				sb.WriteString("            }\n")
			}
			sb.WriteString("          }\n")
			sb.WriteString("        }\n")
		}
		sb.WriteString("      }\n")
		sb.WriteString("    }\n")
	}
	sb.WriteString("  }\n")
	sb.WriteString("}")
	return sb.String()
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
