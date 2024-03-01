package SRBNP;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import SRBNP.Exceptions.*;
import SRBNP.OPCode.Base;
import SRBNP.Requests.Handler;
import SRBNP.Requests.Listener;

public class Connection {
	private static Connection Current;
	private static String ServerHost = "127.0.0.1";
	private static int ServerPort = 9114;

	private ClientInfo Client;
	private Socket Server;
	private InputStream StreamIn;
	private OutputStream StreamOut;
	private boolean initialized = false;

	public Connection() throws UnknownHostException, IOException, CStringInvalidException, InterruptedException {
		Current = this;
	}

	public void Connect(Listener OnConnect)
			throws UnknownHostException, IOException, CStringInvalidException, InterruptedException {
		synchronized (get()) {
			Client = new ClientInfo();
			Server = new Socket(ServerHost, ServerPort);
			StreamIn = Server.getInputStream();
			StreamOut = Server.getOutputStream();
			OPcode.Init();
			Handler.setStreams(StreamIn, StreamOut);
			Handler.Initialize();
			Send(Base.get(Base.CMD_CONNECT));
			Client.setUuid(new String(StreamIn.readNBytes(36)));
			initialized = true;
		}
	}

	public void Connect(String URL, int Port, Listener OnConnect)
			throws UnknownHostException, CStringInvalidException, IOException, InterruptedException {
		ServerHost = URL;
		ServerPort = Port;
		Connect(OnConnect);
	}

	public void Connect(String URL, int Port)
			throws UnknownHostException, CStringInvalidException, IOException, InterruptedException {
		Connect(URL, Port, null);
	}

	public void getVersion(Listener OnResponse) {
		Handler.addRequest(new Request(Base.get(Base.CMD_SRBNP_VER), OnResponse));
	}

	public void Stop() {
		Handler.StopHandlers();
	}

	public void Join() throws InterruptedException {
		Handler.JoinHandlers();
	}

	public void Send(OPcode _OPCODE) throws IOException {

		StreamOut.write(_OPCODE.get(), 0, OPcode.LENGTH);
	}

	public void Send(Request Rqst) {
		Handler.addRequest(Rqst);
	}

	public ClientInfo getClientInfo() {
		return get().Client;
	}

	public static Connection get() {
		return Current;
	}

	public boolean isInitialized() {
		return initialized;
	}

}
