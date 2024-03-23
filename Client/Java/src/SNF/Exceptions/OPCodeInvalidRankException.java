package SNF.Exceptions;

import SNF.OPCode.Member.Rank;

public class OPCodeInvalidRankException extends Exception
{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public OPCodeInvalidRankException(Rank r)
	{
		super("Incompatible Rank for operation. Rank : " + r.toString());
	}

}
