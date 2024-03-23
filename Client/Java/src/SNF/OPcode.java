package SNF;

import SNF.Exceptions.*;
import SNF.OPCode.*;
import SNF.OPCode.Member.*;

public class OPcode {
	public static final int LENGTH = 4;
	private static boolean Initialized = false;

	public static void Init() {
		if(isInitialized())
			return;
		Member.Init();
		try {
			Base.Init();
		} catch (OPCodeNotInitializedException e) {
			/*
			 * To be avoided as the only way this exception would be triggered in this case
			 * is if SNF.OPCode.Member.Init() wasn't called before.
			 * 
			 * Print Stack Trace will be left in case any undocumented reason for triggering
			 * this Exception would be found.
			 */
			e.printStackTrace();
		}
		Initialized = true;
	}

	private Member Category;
	private Member SubCategory;
	private Member Command;
	private Member Detail;

	private OPcode() throws OPCodeNotInitializedException {
		if (!Initialized)
			throw new OPCodeNotInitializedException();
	}

	public OPcode(byte Category, byte SubCategory, byte Command) throws OPCodeNotInitializedException{
		this();
		try {		
			this.Category = Member.getCategory(Category);
			this.SubCategory = Member.getSubCategory(this.Category, SubCategory);
			this.Command = Member.getCommand(this.SubCategory, Command);
			this.Detail = this.Command.getSub().getFirst();
		} catch (OPCodeUnregistredRankValueException e) {
			System.out.println("Copied Invalid");
			copy(Base.getInvalid());
		}
	}
	public OPcode(byte Category, byte SubCategory, byte Command, byte Detail) throws OPCodeNotInitializedException{
		this(Category, SubCategory, Command);
		try {
			this.Detail = Member.getDetail(this.Command, Detail);
		} catch (OPCodeUnregistredRankValueException e) {
			this.Detail = this.Command.getSub(Base.DET_UNDETAILED);
		}		
	}
	public OPcode(Member Category, Member SubCategory, Member Command) throws OPCodeNotInitializedException {
		this();
		this.Category = Category;
		this.SubCategory = SubCategory;
		this.Command = Command;
		this.Detail = Command.getSub().getFirst();
	}
	public OPcode(Member Category, Member SubCategory, Member Command, Member Detail) throws OPCodeNotInitializedException {
		this(Category, SubCategory, Command);
		this.Detail = Detail;
	}

	public Member get(Rank R) throws OPCodeInvalidRankException {
		switch (R) {
		case Category:
			return Category;
		case SubCategory:
			return SubCategory;
		case Command:
			return Command;
		case Detail:
			return Detail;
		case Undefined:
		default:
			throw new OPCodeInvalidRankException(R);
		}
	}
	public byte[] get() {
		byte[] re = new byte[4];
		re[0] = Category.getValue();
		re[1] = SubCategory.getValue();
		re[2] = Command.getValue();
		re[3] = Detail.getValue();
		return re;
	}
	public void set(Rank R, Member Value) throws OPCodeInvalidRankException, OPCodeIncompatibleRanksException {
		switch (R) {
		case Category:
			if(Value.getRank() == R)
				this.Category = Value;
			else
				throw new OPCodeIncompatibleRanksException(R, Value.getRank());
			break;
		case SubCategory:
			if(Value.getRank() == R)
				this.SubCategory = Value;
			else
				throw new OPCodeIncompatibleRanksException(R, Value.getRank());
			break;
		case Command:
			if(Value.getRank() == R)
				this.Command = Value;
			else
				throw new OPCodeIncompatibleRanksException(R, Value.getRank());
			break;
		case Detail:
			if(Value.getRank() == R)
				this.Detail = Value;
			else
				throw new OPCodeIncompatibleRanksException(R, Value.getRank());
			break;
		case Undefined:
		default:
			throw new OPCodeInvalidRankException(R);
		}
	}
	public void copy(OPcode CopyFrom)
	{
		for(Rank R : Rank.values())
		{
			if(R == Rank.Undefined)
				break;
			
			try {
				set(R, CopyFrom.get(R));
			} catch (OPCodeInvalidRankException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (OPCodeIncompatibleRanksException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	public static boolean isInitialized() {
		return Initialized;
	}
}
