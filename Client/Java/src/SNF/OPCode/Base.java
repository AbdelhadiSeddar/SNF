package SNF.OPCode;

import SNF.OPcode;
import SNF.Exceptions.OPCodeIncompatibleRanksException;
import SNF.Exceptions.OPCodeNotInitializedException;
import SNF.Exceptions.OPCodeUnregistredRankValueException;
import SNF.OPCode.Member.Rank;

public class Base {

	public static final byte CATEGORY = 0x00;
	public static final byte SUBCATEGORY = 0x00;
	public static final byte DET_UNDETAILED = 0x00;

	public static final byte CMD_CONNECT = 0x00;
	public static final byte CMD_RECONNECT = 0x01;
	public static final byte CMD_DISCONNECT = 0x02;
	public static final byte CMD_SRBNP_VER = 0x03;
	public static final byte CMD_KICK = 0x04;
	public static final byte CMD_CONFIRM = 0x05;
	public static final byte CMD_REJECT = 0x06;

	public static final byte CMD_INVALID = (byte) 0xFF;
	public static final byte DET_INVALID_UNREGISTRED_OPCODE = 0x01;
	public static final byte DET_INVALID_ERROR_PROTOCOL = 0x02;

	private static Member MEM_CATEGORY;
	private static Member MEM_SUBCATEGORY;
	private static Member MEM_INVALID;

	public static void Init() throws OPCodeNotInitializedException {
		try {
			MEM_CATEGORY = new Member(Rank.Category, CATEGORY, "Base SNF Opcode category", null);
			Member.addCategory(MEM_CATEGORY);
			MEM_SUBCATEGORY = new Member(Rank.SubCategory, SUBCATEGORY, "Base SNF Opcode sub-category", MEM_CATEGORY);
			Member.addSubCategory(MEM_SUBCATEGORY);
			Member.addCommand(new Member(Rank.Command, CMD_CONNECT, "Used when client is attempting to connect", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_RECONNECT, "Used when client is attempting to reconnect or is forced to.", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_DISCONNECT, "When client attemps to disconnect.", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_SRBNP_VER, "When client requests SNF version of the Server.", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_KICK, "When client was forced to disconnect ( Kicked ).", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_CONFIRM, "When client's reuqest was confirmed.", MEM_SUBCATEGORY));
			Member.addCommand(new Member(Rank.Command, CMD_REJECT, "When client's request was rejected.", MEM_SUBCATEGORY));
			MEM_INVALID = new Member(Rank.Command, CMD_INVALID,
					"When client's request was invalid either, wrong version or incomplete request.", MEM_SUBCATEGORY);
			Member.addCommand(MEM_INVALID);
			Member.addDetail(new Member(Rank.Detail, DET_INVALID_UNREGISTRED_OPCODE, "Received opcode was not registred", MEM_INVALID));
			Member.addDetail(new Member(Rank.Detail, DET_INVALID_ERROR_PROTOCOL, "Protocol used is invalid", MEM_INVALID));
		} catch (OPCodeNotInitializedException | OPCodeIncompatibleRanksException | OPCodeUnregistredRankValueException e) {
			e.printStackTrace();
		}
	}

	public static OPcode get(byte Command) {
		return get(Command, DET_UNDETAILED);
	}

	public static OPcode get(byte Command, byte Detail) {
		try {
			return new OPcode(CATEGORY, SUBCATEGORY, Command, Detail);
		} catch (OPCodeNotInitializedException e) {
			OPcode.Init();
			return get(Command, Detail);
		}
	}

	public static OPcode getInvalid() throws OPCodeNotInitializedException {
		return getInvalid(DET_UNDETAILED);
	}

	public static OPcode getInvalid(byte Detail) throws OPCodeNotInitializedException {
		if (MEM_CATEGORY == null || MEM_SUBCATEGORY == null || MEM_INVALID == null)
			throw new OPCodeNotInitializedException();
		return new OPcode(MEM_CATEGORY, MEM_SUBCATEGORY, MEM_INVALID, MEM_INVALID.getSub(Detail));
	}
}
