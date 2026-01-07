package Core

import (
	"fmt"
	"strings"
)

const (
	SNFOpcodeRankCategory SNFOpcodeRank = iota
	SNFOpcodeRankSubCategory
	SNFOpcodeRankCommand
	SNFOpcodeRankDetail
)

var opcode_def_cb SNFOpcodeCommandCallback = nil

type SNFOpcodeRootStructure struct {
	categories map[byte]*SNFOpcodeCategoryMember
	isInit     bool
	defaultCB  SNFOpcodeCommandCallback
}

func (root *SNFOpcodeRootStructure) DefineCategory(code byte) *SNFOpcodeCategoryMember {
	if _, ok := root.categories[code]; !ok {
		item := SNFOpcodeCategoryMember{
			subCategories: make(map[byte]*SNFOpcodeSubCategoryMember),
		}
		item.SetValue(code)
		item.root = root
		root.categories[code] = &item
		return &item
	}
	return nil
}
func (root *SNFOpcodeRootStructure) GetCategory(code byte) *SNFOpcodeCategoryMember {
	return root.categories[code]
}

func (root *SNFOpcodeRootStructure) DefineDefaultCallback(cb SNFOpcodeCommandCallback) {
	root.defaultCB = cb
}
func (st *SNFOpcodeRootStructure) GetOpcode(category byte, subCategory byte, command byte, detail byte) *SNFOpcode {
	var cat *SNFOpcodeCategoryMember
	var scat *SNFOpcodeSubCategoryMember
	var cmd *SNFOpcodeCommandMember
	var det *SNFOpcodeDetailMember
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

	re := &SNFOpcode{}
	re.Category = cat
	re.SubCategory = scat
	re.Command = cmd
	re.Detail = det
	re.parentStruct = st
	return re
}

type SNFOpcode struct {
	parentStruct *SNFOpcodeRootStructure
	Category     *SNFOpcodeCategoryMember
	SubCategory  *SNFOpcodeSubCategoryMember
	Command      *SNFOpcodeCommandMember
	Detail       *SNFOpcodeDetailMember
}

func (o SNFOpcode) ToBytes() []byte {
	return []byte{
		o.Category.GetValue(),
		o.SubCategory.GetValue(),
		o.Command.GetValue(),
		o.Detail.GetValue(),
	}
}

func (root *SNFOpcodeRootStructure) SNFOpcodeParse(data [4]byte) (*SNFOpcode, error) {
	var op *SNFOpcode

	op = root.GetOpcode(data[0], data[1], data[2], data[3])
	if op == nil {
		return nil, SNFErrorOpcodeInvalid{
			OPCode: data,
		}
	}

	return op, nil
}

type SNFOpcodeRank int

// /
type SNFOpcodeMember struct {
	val byte
}

func (o *SNFOpcodeMember) GetValue() byte {
	return o.val
}
func (o *SNFOpcodeMember) SetValue(v byte) {
	o.val = v
}

type SNFOpcodeCategoryMember struct {
	SNFOpcodeMember
	root          *SNFOpcodeRootStructure
	subCategories map[byte]*SNFOpcodeSubCategoryMember
}

func (p *SNFOpcodeCategoryMember) DefineSubCategory(code byte) *SNFOpcodeSubCategoryMember {
	if _, ok := p.subCategories[code]; !ok {
		item := SNFOpcodeSubCategoryMember{
			commands: make(map[byte]*SNFOpcodeCommandMember),
		}
		item.SetValue(code)
		item.parent = p
		p.subCategories[code] = &item
		return &item
	}
	return nil
}
func (cat *SNFOpcodeCategoryMember) GetSubCategory(code byte) *SNFOpcodeSubCategoryMember {
	return cat.subCategories[code]
}

type SNFOpcodeSubCategoryMember struct {
	SNFOpcodeMember
	parent   *SNFOpcodeCategoryMember
	commands map[byte]*SNFOpcodeCommandMember
}

func (p *SNFOpcodeSubCategoryMember) DefineCommand(code byte, callback SNFOpcodeCommandCallback) *SNFOpcodeCommandMember {
	if _, ok := p.commands[code]; !ok {
		item := SNFOpcodeCommandMember{
			details: make(map[byte]*SNFOpcodeDetailMember),
		}
		item.SetValue(code)
		item.parent = p
		item.details[byte(0)] = &SNFOpcodeDetailMember{
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
func (p *SNFOpcodeSubCategoryMember) GetCommand(code byte) *SNFOpcodeCommandMember {
	return p.commands[code]
}

type SNFOpcodeCommandMember struct {
	SNFOpcodeMember
	f       SNFOpcodeCommandCallback
	parent  *SNFOpcodeSubCategoryMember
	details map[byte]*SNFOpcodeDetailMember
}

func (p *SNFOpcodeCommandMember) DefineDetail(code byte) *SNFOpcodeDetailMember {
	if _, ok := p.details[code]; !ok {
		item := SNFOpcodeDetailMember{}
		item.SetValue(code)
		item.parent = p
		p.details[code] = &item
		return &item
	}
	return nil
}
func (o *SNFOpcodeCommandMember) SetCallback(cb SNFOpcodeCommandCallback) {
	o.f = cb
}
func (o *SNFOpcodeCommandMember) GetCallback() SNFOpcodeCommandCallback {
	return o.f
}
func (p *SNFOpcodeCommandMember) GetDetail(code byte) *SNFOpcodeDetailMember {
	return p.details[code]
}

type SNFOpcodeDetailMember struct {
	SNFOpcodeMember
	parent *SNFOpcodeCommandMember
}

type SNFOpcodeCommandCallback func(Original SNFRequest, Sender interface{}) (SNFRequest, error)

// var snf_opcode_ll *snfOpcodeLLItem
// var snf_opcode_base_isinit bool = false

// if already defined return nil
// Checks if it is a Base Opcode
func (op SNFOpcode) IsBase() bool {
	if op.Category.GetValue() != SNF_OPCODE_BASE_CAT {
		return false
	}
	if op.SubCategory.GetValue() != SNF_OPCODE_BASE_SUBCAT {
		return false
	}
	return true
}
func (st *SNFOpcodeRootStructure) GetUOpcode(category byte, subCategory byte, command byte) *SNFOpcode {
	return st.GetOpcode(category, subCategory, command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

func (st *SNFOpcodeRootStructure) GetBaseCategory() *SNFOpcodeCategoryMember {
	return st.categories[byte(0)]
}

func (st *SNFOpcodeRootStructure) GetBaseSubCategory() *SNFOpcodeSubCategoryMember {
	return st.categories[byte(0)].subCategories[byte(0)]
}

func (st *SNFOpcodeRootStructure) GetBaseCommand(command byte) *SNFOpcodeCommandMember {
	return st.categories[byte(0)].subCategories[byte(0)].commands[command]
}

func (st *SNFOpcodeRootStructure) GetBaseDetail(command byte, detail byte) *SNFOpcodeDetailMember {
	return st.categories[byte(0)].subCategories[byte(0)].commands[command].details[detail]
}

func (st *SNFOpcodeRootStructure) GetBaseOpcode(command byte, detail byte) *SNFOpcode {
	return st.GetOpcode(SNF_OPCODE_BASE_CAT, SNF_OPCODE_BASE_SUBCAT, command, detail)
}

func (st *SNFOpcodeRootStructure) GetUBaseOpcode(command byte) *SNFOpcode {
	return st.GetBaseOpcode(command, SNF_OPCODE_BASE_DET_UNDETAILED)
}

// Defiens a new Root Structure for OPcodes. whilst assuing base functionalities are maintained
func SNFNewOpodeStructure(def_cb SNFOpcodeCommandCallback) *SNFOpcodeRootStructure {
	root := &SNFOpcodeRootStructure{
		categories: make(map[byte]*SNFOpcodeCategoryMember),
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
func SNFOpcodePrint(root SNFOpcodeRootStructure) string {
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
