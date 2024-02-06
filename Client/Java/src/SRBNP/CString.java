package SRBNP;

import SRBNP.Exceptions.*;

public class CString {
	String Content;
	
	
	public static CString FromString(String str)
	{
		try {
			return new CString(str);
		} 
		catch (InvalidCStringException e)
		{
			return new CString(str + '\0');
		}
	}
	
	CString(String str) throws InvalidCStringException
	{
		if(str.length() < 1 || str.lastIndexOf('\0') == -1 || str.lastIndexOf('\0') != str.length() - 1)
			throw new InvalidCStringException(str);
		else		
			Content = str;
	}
	
	public CString(byte[] nBytes) throws InvalidCStringException {
		this(new String(nBytes));
	}
	
	/*
	 * Returns the length
	 * */
	public int length()
	{
		return Content.length() - 1;
	}
	
	public int nlength()
	{
		return Content.length();
	}
	
	public String toString()
	{
		return Content.substring(0, Content.length() -1);
	}
}
