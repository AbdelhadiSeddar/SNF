package SRBNP.Exceptions;

public class CStringInvalidException extends RuntimeException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public CStringInvalidException(String str)
	{
		super("Invalid C String (Missing Null Operator ) Length " + str.length());
	}
}
