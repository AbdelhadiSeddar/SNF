package SRBNP.Exceptions;

import SRBNP.OPCode.Member.Rank;

public class OPCodeIncompatibleRanksException extends RuntimeException
{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public OPCodeIncompatibleRanksException()
	{
		super("Incompatible Ranks for operation");
	}
	
	public OPCodeIncompatibleRanksException(Rank Destination, Rank Source)
	{
		super("Trying to replace a " + Destination.toString() + " with a " + Source.toString()+".");
	}

}
