package SRBNP;

public final class _BASE_OPCODE {
	
	public final static byte[] CONNECT    = "000\0".getBytes();
	public final static byte[] RECONNECT  = "001\0".getBytes();
	public final static byte[] DISCONNECT = "002\0".getBytes();
	public final static byte[] VERSION    = "003\0".getBytes();
	public final static byte[] KICK       = "004\0".getBytes();
	
	public final static byte[] CONFIRM    = "005\0".getBytes();
	public final static byte[] REJECTED   = "006\0".getBytes();
	
	public final static int _LENGTH = CONNECT.length;
	public static boolean isValid_BaseOPCODE(byte[] _OPCODE)
	{
		if(_OPCODE.length != _LENGTH)
			return false;
		for(int i = 0 ; i < _LENGTH - 3 ; i++ )
			if(_OPCODE[i] != "0".getBytes()[0])
				return false;
		if(_OPCODE[2] > REJECTED[2])
			return false;
		
		return true;
	}
}
