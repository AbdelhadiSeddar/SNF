package SRBNP.Exceptions;

public class InvalidCStringException extends RuntimeException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public InvalidCStringException(String str)
	{
		super("Invalid C String (Missing Null Operator ) Length " + str.length());
	}
}
