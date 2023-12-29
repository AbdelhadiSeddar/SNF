package SRBNP;

public final class _BASE_OPCODE {
	public final static int _LENGTH = 4;
	
	public final static byte[] CLT_CONNECT    = "000\0".getBytes();
	public final static byte[] CLT_RECONNECT  = "001\0".getBytes();
	public final static byte[] CLT_DISCONNECT = "002\0".getBytes();
	public final static byte[] CLT_UPDATE     = "003\0".getBytes();
	public final static byte[] CLT_KICK       = "004\0".getBytes();
	
	public final static byte[] CLT_CONFIRM    = "005\0".getBytes();
	public final static byte[] CLT_REJECTED   = "006\0".getBytes();
	
	public static boolean isValid_OPCODE(byte[] _OPCODE)
	{
		if(_OPCODE.length != _LENGTH)
			return false;
		for(int i = 0 ; i < _LENGTH -1 ; i++ )
			if(_OPCODE[i] != "0".getBytes()[0])
				return false;
		if(_OPCODE[_LENGTH -1] < CLT_CONNECT[0]
				&& _OPCODE[_LENGTH -1] > CLT_REJECTED[0])
			return false;
		
		return true;
	}
}
