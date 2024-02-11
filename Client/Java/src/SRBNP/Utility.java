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
}
