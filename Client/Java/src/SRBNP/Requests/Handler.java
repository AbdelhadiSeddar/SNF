package SRBNP.Requests;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

import SRBNP.CString;
import SRBNP.Connection;
import SRBNP.Request;
import SRBNP.Utility;
import SRBNP.Exceptions.*;

public class Handler {
	private static Handler RH;
	private static Semaphore Sem = new Semaphore(0);
	private static Boolean KeepRunning = true;
	private static StreamOutHandler OutsHandler;
	private static StreamInHandler InsHandler;
	private static OutputStream StreamOut;
	private static InputStream StreamIn;

	private class StreamOutHandler extends Thread {

		@Override
		public void run() {

			while (KeepRunning) {
				try {
					Sem.acquire();
					if (!KeepRunning)
						return;
					Request R;
					synchronized (QueuedRequests) {
						R = QueuedRequests.remove();
					}
					Send(R);
					if (R.AwaitsResponse())
						synchronized (UnrespondedRequests) {
							UnrespondedRequests.add(R);
						}
				} catch (InterruptedException | IOException e) {
					e.printStackTrace();
				}
			}

		}
	}

	private class StreamInHandler extends Thread {

		@Override
		public void run() {
			return;
		}
	}

	protected static Queue<Request> QueuedRequests = new LinkedList<Request>();

	protected static LinkedList<Request> UnrespondedRequests = new LinkedList<Request>();

	public static void Initialize() {
		RH = new Handler();

		InsHandler = RH.new StreamInHandler();
		OutsHandler = RH.new StreamOutHandler();
		
		StartHandlers();
	}

	public static boolean isInitialized() {
		if (RH == null || InsHandler == null || OutsHandler == null)
			return false;
		if (RH == null || StreamIn == null || StreamOut == null)
			return false;
		return true;
	}

	public static void setStreams(InputStream In, OutputStream Out) {
		StreamIn = In;
		StreamOut = Out;
	}

	public static void addRequest(Request Rqst) throws RequestsHandlerNotInitializedException {
		if (!isInitialized())
			throw new RequestsHandlerNotInitializedException();

		synchronized (QueuedRequests) {
			QueuedRequests.add(Rqst);
			Sem.release();
		}
	}

	public static void Send(Request r) throws IOException {
		byte[] CUID = CString.FromString(Connection.getClientInfo().getUuid().toString()).getBytes();
		byte[] RUID = r.getUID();
		byte[] OPCODE = r.getOPCODE();
		String Arg = "";
		for (String arg : r.getArguments()) {
			Arg += arg;
			Arg += Utility.UNIT_SEPARATOR;
		}
		CString toCSend = CString.FromString(Arg);

		byte[] fb = Utility.IntToBytes(toCSend.length() == 0 ? 0 : toCSend.nlength());
		
		StreamOut.write(CUID);
		write(OPCODE);
		write(RUID);
		write(fb);
		if (toCSend.length() > 0)
			StreamOut.write(toCSend.getBytes());

	}

	public static void StartHandlers() {
		if (!InsHandler.isAlive())
			InsHandler.start();
		if (!OutsHandler.isAlive())
			OutsHandler.start();
	}

	private static void write(byte[] Content) throws IOException
	{
		if(StreamOut == null)
			throw new RequestsHandlerNotInitializedException();
		for(byte b : Content)
		{
			StreamOut.write(b);
		}
	}
	public static void JoinHandlers() throws InterruptedException {
		InsHandler.join();
		OutsHandler.join();
	}

	public static void StopHandlers() {
		KeepRunning = false;
		Sem.release();
	}

}
