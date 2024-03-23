package SNF.Requests;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

import SNF.CString;
import SNF.Connection;
import SNF.OPcode;
import SNF.Request;
import SNF.Utility;
import SNF.Exceptions.*;
import SNF.OPCode.Base;

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
			while (true)
				synchronized (Connection.get()) {
					if (Connection.get().isInitialized())
						break;
				}
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
			while (true)
				synchronized (Connection.get()) {
					if (Connection.get().isInitialized())
						break;
				}
			while (KeepRunning) {
				try {
					Request R = Receive();
					
					synchronized (UnrespondedRequests) {
						for (Request r : UnrespondedRequests) {
							r.InvokeResponseHandler(R);
							UnrespondedRequests.remove(r);
							break;
						}
					}
				} catch (IOException | OPCodeNotInitializedException e) {
					e.printStackTrace();
					break;
				}
			}
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
		byte[] CUID = CString.FromString(Connection.get().getClientInfo().getUuid().toString()).getBytes();
		byte[] RUID = r.getUID();
		byte[] OPCODE = r.getOPCODE();
		LinkedList<Byte> arg = new LinkedList<Byte>();
		for (byte[] Arg : r.getArguments()) {
			for (byte b : Arg)
				arg.add(b);
			arg.add((byte) Utility.UNIT_SEPARATOR);
		}
		arg.add((byte) 0x00);
		byte[] fb = Utility.IntToBytes(arg.size() == 1 ? 0 : arg.size());
		byte[] Content = new byte[arg.size()];
		for (int i = 0; i < arg.size(); i++)
			Content[i] = arg.get(i);

		StreamOut.write(CUID);
		write(OPCODE);
		write(RUID);
		write(fb);
		if (arg.size() > 1)
			write(Content);

	}

	public static Request Receive() throws OPCodeNotInitializedException, IOException {
		Request re = new Request(Base.getInvalid());

		byte[] opcode = read(4);
		byte[] uid = read(16);
		byte[] msgsize = read(4);
		int size = Utility.BytesToInt(msgsize);
		byte[][] Content = read(size, Utility.UNIT_SEPARATOR);
		
		re.setOPCODE(new OPcode(opcode[0], opcode[1], opcode[2], opcode[3]));
		re.setUID(uid);
		re.setArguments(Content);
		return re;
	}

	public static void StartHandlers() {
		if (!InsHandler.isAlive())
			InsHandler.start();
		if (!OutsHandler.isAlive())
			OutsHandler.start();
	}

	private static void write(byte[] Content) throws IOException {
		if (StreamOut == null)
			throw new RequestsHandlerNotInitializedException();
		for (byte b : Content) {
			StreamOut.write(b);
		}
	}

	private static byte[] read(int length) throws IOException {
		if (StreamIn == null)
			throw new RequestsHandlerNotInitializedException();
		byte[] re = new byte[length];
		for (int i = 0; i < length; i++) {
			re[i] = (byte) StreamIn.read();
		}
		return re;
	}

	private static byte[][] read(int length, int Splitter) throws IOException {
		if (StreamIn == null)
			throw new RequestsHandlerNotInitializedException();
		Splitter &= 0xFF;
		LinkedList<byte[]> re = new LinkedList<byte[]>();
		for (int i = 0; i < length; i++) {
			LinkedList<Byte> word = new LinkedList<Byte>();
			while (i < length) {
				byte b = (byte) StreamIn.read();
				if (b == Splitter)
					break;
				i++;
				word.add(b);
			}
			byte[] wordArr = new byte[word.size()];
			for (int u = 0; u < word.size(); u++)
				wordArr[u] = word.get(u);

			re.add(wordArr);
		}
		byte[][] reArr = new byte[re.size()][];
		re.toArray(reArr);
		return reArr;
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
