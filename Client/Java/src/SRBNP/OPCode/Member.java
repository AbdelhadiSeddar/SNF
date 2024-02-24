package SRBNP.OPCode;

import SRBNP.Exceptions.*;

import java.util.LinkedList;

public class Member {
	private static Member Undefined_Detail;

	private static LinkedList<Member> Categories = new LinkedList<Member>();
	public static LinkedList<Member> getCategories() {
		return Categories;
	}

	public static Member getCategory(byte Category) throws OPCodeUnregistredRankValueException {
		for (Member cat : Categories) {
			if (cat.getValue() == Category)
				return cat;
		}
		throw new OPCodeUnregistredRankValueException(Rank.Category, Category);
	}
	public static Member getSubCategory(Member Category, byte SubCategory) throws OPCodeUnregistredRankValueException {
		for (Member sub : Category.getSub()) {
			if (sub.getValue() == SubCategory) {
				return sub;
			}
		}
		throw new OPCodeUnregistredRankValueException(Category, SubCategory);
	}
	public static Member getSubCategory(byte Category, byte SubCategory) throws OPCodeUnregistredRankValueException {
		return getSubCategory(getCategory(Category), SubCategory);
	}
	public static Member getCommand(Member SubCategory, byte Command) throws OPCodeUnregistredRankValueException {
		for (Member cmd : SubCategory.getSub()) {
			if (cmd.getValue() == Command)
				return cmd;
		}
		throw new OPCodeUnregistredRankValueException(SubCategory, Command);
	}
	public static Member getCommand(byte Category, byte SubCategory, byte Command) throws OPCodeUnregistredRankValueException {
		return getCommand(getSubCategory(Category, SubCategory), SubCategory);
	}
	public static Member getDetail(Member Command, byte Detail) throws OPCodeUnregistredRankValueException {
		for (Member det : Command.getSub()) {
			if (det.getValue() == Detail)
				return det;
		}
		throw new OPCodeUnregistredRankValueException(Command, Detail);
	}
	public static Member getDetail(byte Category, byte SubCategory, byte Command, byte Detail) throws OPCodeUnregistredRankValueException {
		return getDetail(getCommand(Category, SubCategory, Command), Detail);
	}
	
	public static void addCategory(Member Category) throws OPCodeIncompatibleRanksException {
		if(Category.rank != Rank.Category)
			throw new OPCodeIncompatibleRanksException();
		Categories.add(Category);
	}
	
	public static void addSubCategory(Member SubCategory) throws OPCodeIncompatibleRanksException, OPCodeUnregistredRankValueException
	{
		if(SubCategory.rank != Rank.SubCategory)
			throw new OPCodeIncompatibleRanksException();
		if(SubCategory.parent == null)
			throw new OPCodeUnregistredRankValueException(SubCategory, (byte)0x00);
		
		SubCategory.parent.addSub(SubCategory);
	}
	public static void addCommand(Member Command) throws OPCodeIncompatibleRanksException, OPCodeUnregistredRankValueException
	{
		if(Command.rank != Rank.Command)
			throw new OPCodeIncompatibleRanksException();
		if(Command.parent == null)
			throw new OPCodeUnregistredRankValueException(Command, (byte)0x00);
		
		Command.parent.addSub(Command);
		Command.sub.add(Undefined_Detail.clone());
	}
	public static void addDetail(Member Detail) throws OPCodeIncompatibleRanksException, OPCodeUnregistredRankValueException
	{
		if(Detail.rank != Rank.Detail)
			throw new OPCodeIncompatibleRanksException();
		if(Detail.parent == null)
			throw new OPCodeUnregistredRankValueException(Detail, (byte)0x00);
		
		Detail.parent.addSub(Detail);
	}
	public static boolean check(byte Category, byte SubCategory, byte Command, byte Detail)
	{
		try {
			return getDetail(Category, SubCategory, Command, Detail) != null;
		} catch (OPCodeUnregistredRankValueException e)
		{
			return false;
		}
	}
	public static void Init() {
		Undefined_Detail = new Member();
		Undefined_Detail.definition = "Undefined Detail";
		Undefined_Detail.rank = Rank.Detail;
		Undefined_Detail.value = 0x00;
		Undefined_Detail.parent = null;
	}

	private LinkedList<Member> sub = new LinkedList<Member>();

	public enum Rank {
		Category, SubCategory, Command, Detail, Undefined
	}

	private byte value;
	private String definition;
	private Rank rank;

	private Member parent;

	private Member() {
		value = 0x00;
		definition = null;
		parent = null;
		rank = Rank.Undefined;
	}

	public Member(Rank rank, byte Value, String Definition, Member parent) throws OPCodeNotInitializedException {
		this();
		if (Undefined_Detail == null)
			throw new OPCodeNotInitializedException();
		this.rank = rank;
		this.value = Value;
		this.definition = Definition;

		this.parent = parent;
	}

	public boolean compare(Member OPCodeMember) throws OPCodeIncompatibleRanksException {
		if (OPCodeMember.getRank() != this.rank)
			throw new OPCodeIncompatibleRanksException();

		return OPCodeMember.getValue() == this.value;
	}

	public byte getValue() {
		return value;
	}

	public void setValue(byte value) {
		this.value = value;
	}

	public String getDefinition() {
		return definition;
	}

	public void setDefinition(String definition) {
		this.definition = definition;
	}

	public Rank getRank() {
		return rank;
	}

	public void setRank(Rank rank) {
		this.rank = rank;
	}

	public Member getParent() {
		return parent;
	}

	public void setParent(Member parent) {
		this.parent = parent;
	}

	public Member getSub(byte Value) {
		for (Member m : sub) {
			if (m.getValue() == Value)
				return m;
		}
		return sub.getFirst();
	}

	public LinkedList<Member> getSub() {
		return sub;
	}

	public void addSub(Member m) {
		this.getSub().addLast(m);
		m.setParent(this);
	}

	public Member clone() {
		return clone(this.parent);
	}

	public Member clone(Member Parent) {
		Member cln = new Member();
		cln.setRank(this.rank);
		cln.setValue(this.value);
		cln.setDefinition(this.definition);
		cln.setParent(Parent);

		return cln;
	}
}
