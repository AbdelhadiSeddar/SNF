package SRBNP;

import java.nio.charset.StandardCharsets;

import SRBNP.Exceptions.*;

public class CString {
	String Content;
	
	static char NullChr = Character.MIN_VALUE;

	public static CString FromString(String str) {
		return new CString(str + NullChr);
	}

	public CString(String str) throws CStringInvalidException {
		if (str.length() < 1 || str.lastIndexOf(NullChr) == -1 || str.lastIndexOf(NullChr) != str.length() - 1)
			throw new CStringInvalidException(str);
		else
			Content = str;
	}

	public CString(byte[] nBytes) throws CStringInvalidException {
		this(new String(nBytes));
	}

	/*
	 * Returns the length
	 */
	public int length() {
		return Content.length() - 1;
	}
	/*
	 * returns the lenght including the nullcharacter
	 */
	public int nlength() {
		return Content.length();
	}

	public String toString() {
		return Content.substring(0, Content.length() - 1);
	}
	
	public char[] toCharArray() {
		return Content.toCharArray();
	}

	public byte[] getBytes() {
		return Content.getBytes(StandardCharsets.US_ASCII);
	}
}
