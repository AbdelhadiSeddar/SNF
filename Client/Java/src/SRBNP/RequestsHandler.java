package SRBNP;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

import SRBNP.Exceptions.*;

class RequestsHandler {	
	private static Semaphore Sem = new Semaphore(0);
	private static Boolean KeepRunning = true;	
	private static StreamOutHandler OutsHandler;
	private static StreamInHandler InsHandler;
	private static OutputStream	StreamOut;
	private static InputStream 	StreamIn;
	
	private class StreamOutHandler extends Thread {
		
		@Override
		public void run() {
			
			while(KeepRunning)
			{
				try {
					Sem.acquire();
					Request R;
					synchronized(QueuedRequests)
					{
						R = QueuedRequests.remove();
					}		
					Send(R);
					synchronized(UnrespondedRequests)
					{
						UnrespondedRequests.add(R);
					}
					return;
				} catch (InterruptedException | IOException e) {
					// TODO Auto-generated catch block
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
	
	protected static Queue<Request> QueuedRequests
		= new LinkedList<Request>();

	protected static LinkedList<Request> UnrespondedRequests
		= new LinkedList<Request>();
	
	public static void Intialize()
	{
		(InsHandler = (new RequestsHandler()).new StreamInHandler()).start();
		(OutsHandler = (new RequestsHandler()).new StreamOutHandler()).start();
	}
	public static boolean isInitialized()
	{
		if(InsHandler == null || OutsHandler == null)
			return false;
		if(StreamIn == null || StreamOut == null)
			return false;
		return true;
	}
	
	public static void setStreams(InputStream In, OutputStream Out)
	{
		StreamIn = In;
		StreamOut = Out;
	}
	
	public static void addRequest(Request Rqst) throws RequestsHandlerNotInitializedException
	{
		if(!isInitialized())
			throw new RequestsHandlerNotInitializedException();
		
		synchronized (QueuedRequests) 
		{
			QueuedRequests.add(Rqst);
			Sem.release();
		}
	}
	
	
	private static void Send(Request r) throws IOException
	{
		String toSend = new String();
		toSend += r.getUID();
		toSend += r.getOPCODE();
		for(String arg :r.getArguments())
		{
			toSend += arg;
			toSend += Utility.UNIT_SEPARATOR;
		}
		
		byte[] UUID 
			= CString.FromString(
					Connection
					.getCurrent()
					.getClient()
					.getUuid()
					.toString()
					)
			.toString()
			.getBytes();
		byte[] fb = Utility.getFbyte(toSend);
		
		StreamOut.write(UUID);
		StreamOut.write(fb);
		StreamOut.write(toSend.getBytes());
		
	}
	
	
	public static void JoinHandlers() throws InterruptedException
	{
		InsHandler.join();
		OutsHandler.join();
	}
	public static void StopHandlers() {
		KeepRunning = false;
		
	}
	
	
}
