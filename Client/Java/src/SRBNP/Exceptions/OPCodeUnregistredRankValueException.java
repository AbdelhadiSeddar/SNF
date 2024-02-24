package SRBNP.Exceptions;

import SRBNP.OPCode.Member;
import SRBNP.OPCode.Member.Rank;

public class OPCodeUnregistredRankValueException extends Exception{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public OPCodeUnregistredRankValueException(Member Parent, byte Value)
	{
		super(Parent.getRank().toString() + " [" + Parent.getValue() + "] Parent. Has no Sub-Code [" + Value + "].");
	}
	
	public OPCodeUnregistredRankValueException(Rank r, byte Value)
	{
		super("There is no " + r.toString() + " with Code [" + Value + "].");
	}
}
