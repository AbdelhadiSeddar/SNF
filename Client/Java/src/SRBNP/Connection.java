package SRBNP;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

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
		
	}
	

	
	

}
