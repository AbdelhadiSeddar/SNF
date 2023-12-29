package SRBNP;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import SRBNP._BASE_OPCODE;

public class Connection{
	private static String 	ServerHost = "127.0.0.1";
	private static int		ServerPort = 9114;
	
	private ClientInfo 		Client;
	private Socket			Server;
	private InputStream 	StreamIn;
	private OutputStream	StreamOut;
	
	public Connection()   throws UnknownHostException, IOException
	{
		this.Connect();	
	}
	
	public void Connect()  throws UnknownHostException, IOException
	{
		Client = new ClientInfo();
		Server = new Socket(ServerHost , ServerPort);
		StreamIn = Server.getInputStream();
		StreamOut = Server.getOutputStream();
		
		Send_BASE_OPCODE(_BASE_OPCODE.CLT_CONNECT);
		byte[] Str = StreamIn.readNBytes(37);
		System.out.println(new String(Str));

		Send_BASE_OPCODE(_BASE_OPCODE.CLT_DISCONNECT);
		Server.close();
	}
	
	public void Send_BASE_OPCODE(byte[] _OPCODE) throws IOException
	{
		if(StreamOut == null && !_BASE_OPCODE.isValid_OPCODE(_OPCODE))
			return;
		
		StreamOut.write(_OPCODE, 0, _BASE_OPCODE._LENGTH);
	}
	

	
	

}
