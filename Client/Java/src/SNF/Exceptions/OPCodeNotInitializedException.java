package SNF.Exceptions;

public class OPCodeNotInitializedException extends Exception{

	/**
	 * 
	 */
	private static final long serialVersionUID = 4847494310616338457L;

	public OPCodeNotInitializedException()
	{
		super("OPCode Has not been Intialized Properly!!");
	}
}
