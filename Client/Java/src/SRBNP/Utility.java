package SRBNP;

import java.nio.ByteBuffer;

public class Utility {

	public final static char UNIT_SEPARATOR = (char)31;
	
	public final static byte[] IntToBytes(int integer)
	{
		ByteBuffer b = ByteBuffer.allocate(4);
		b.putInt(integer);

		return b.array();
	}
	
	public final static int BytesToInt(byte[] bytes)
	{
		int re = 0;
		for (int i = 0; i < bytes.length; i++)
	    {
	        re += (bytes[bytes.length - i - 1] << (8 * i));
	    }
		return re;
	}
}
