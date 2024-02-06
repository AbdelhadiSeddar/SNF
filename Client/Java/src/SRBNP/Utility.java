package SRBNP;

public class Utility {

	public final static char UNIT_SEPARATOR = (char)31;
	public static byte[] getFbyte(int StringLength)
	{
		int strlen = StringLength;
		if(strlen > 9999)
			return "9999\0".getBytes();
		char[] reArr = new char[5];
		for(int i = 0 ; i < 4 ; i++)
		{
			reArr[i] = (char)((strlen % 10) + 48);
			strlen /= 10;
		}
		reArr[4] = '\0';
		return reArr.toString().getBytes();
	}

	public static byte[] getFbyte(String Str)
	{
		return getFbyte(Str.length());
	}
}
