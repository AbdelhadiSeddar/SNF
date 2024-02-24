package SRBNP;

import java.util.LinkedList;

import SRBNP.Requests.Listener;

public class Request {
	public final static int _UID_LENGTH = 16;
	private static int _TOTAL__GENERATED_REQUESTS = 0;


	public static byte[] generateUID() {
		byte[] UID = new byte[_UID_LENGTH];
		int gen = (++_TOTAL__GENERATED_REQUESTS);
		for (int i = 0; i < _UID_LENGTH -1; i++) {
			UID[i] = (byte) ((gen != 0 ? (((gen % 64) + 33) * (i == 0 ? 1 : i)) : '0') & (byte)0xFF);
			gen /= 64;
		}
		UID[_UID_LENGTH -1 ] = 0x00;
		return UID;
	}

	private boolean IS_RESPONSE;
	private byte[] UID;
	private OPcode OPCODE;
	private LinkedList<String> Arguments = new LinkedList<String>();

	private Listener Response = null;

	public Request(boolean isResponse, OPcode OPCODE, Listener OnResponse) {
		this.OPCODE = OPCODE;
		this.IS_RESPONSE = isResponse;
		if (!IS_RESPONSE)
			UID = generateUID();
		this.Response = OnResponse;
	}

	public Request(boolean isResponse, OPcode OPCODE) {
		this(isResponse, OPCODE, null);
	}

	public Request(OPcode OPCODE, Listener OnResponse) {
		this(false, OPCODE, OnResponse);
	}

	public Request(OPcode OPCODE) {
		this(false, OPCODE, null);
	}

	public Request(OPcode OPCODE, String[] Args, Listener OnResponse) {
		this(OPCODE, OnResponse);
		for (String arg : Args)
			Arguments.add(arg);
	}

	public Request(OPcode OPCODE, String[] Args) {
		this(OPCODE, Args, null);
	}

	public void add(String Argument) {
		Arguments.add(Argument);
	}
	public byte[] getUID() {
		return UID;
	}

	public byte[] getOPCODE() {
		return OPCODE.get();
	}

	public boolean AwaitsResponse() {
		return Response == null ? false : true;
	}

	public void InvokeResponseHandler(Request Response) {
		if (this.Response != null) {
			this.Response.OnResponse(Response);
		}
	}

	public String[] getArguments() {
		String[] re = new String[Arguments.size()];
		Arguments.toArray(re);
		return re;
	}

	public Listener getResponse() {
		return Response;
	}

	public void AwaitResponse() {
		synchronized (Response) {
			try {
				Response.wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}
