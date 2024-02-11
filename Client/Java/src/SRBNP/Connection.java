package SRBNP;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import SRBNP.Exceptions.*;

public class Connection {
	private static Connection Current;
	private static String ServerHost = "127.0.0.1";
	private static int ServerPort = 9114;

	private ClientInfo Client;
	private Socket Server;
	private InputStream StreamIn;
	private OutputStream StreamOut;

	public Connection() 
			throws UnknownHostException, IOException, InvalidCStringException, InterruptedException {
		Current = this;
	}

	public void Connect(ResponseListener OnConnect)
			throws UnknownHostException, IOException, InvalidCStringException, InterruptedException {
		Client = new ClientInfo();
		Server = new Socket(ServerHost, ServerPort);
		StreamIn = Server.getInputStream();
		StreamOut = Server.getOutputStream();
		RequestsHandler.Intialize();
		RequestsHandler.setStreams(StreamIn, StreamOut);
		Send(_BASE_OPCODE.CONNECT);
		Client.setUuid(new CString(Connection.get().StreamIn.readNBytes(37)));
	}

	public void Connect(String URL, int Port, ResponseListener OnConnect)
			throws UnknownHostException, InvalidCStringException, IOException, InterruptedException {
		ServerHost = URL;
		ServerPort = Port;
		Connect(OnConnect);
	}

	public void Connect(String URL, int Port)
			throws UnknownHostException, InvalidCStringException, IOException, InterruptedException {
		Connect(URL, Port, null);
	}

	public void getVersion(ResponseListener OnResponse) {
		RequestsHandler.addRequest(new Request(_BASE_OPCODE.VERSION, OnResponse));
	}

	public void Stop() {
		RequestsHandler.StopHandlers();
	}

	public void Join() throws InterruptedException {
		RequestsHandler.JoinHandlers();
	}

	public void Send(byte[] _OPCODE) throws IOException {

		StreamOut.write(_OPCODE, 0, _BASE_OPCODE._LENGTH);
	}

	public void Send(Request Rqst) {
		RequestsHandler.addRequest(Rqst);
	}

	public static ClientInfo getClientInfo() {
		return get().Client;
	}

	public static Connection get() {
		return Current;
	}

}
