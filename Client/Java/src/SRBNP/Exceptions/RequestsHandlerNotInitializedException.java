package SRBNP.Exceptions;

public class RequestsHandlerNotInitializedException extends RuntimeException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public RequestsHandlerNotInitializedException()
	{
		super("Ruquests Handler is not Initialized.");
	}
}
