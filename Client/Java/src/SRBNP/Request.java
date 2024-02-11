package SRBNP;

import java.util.LinkedList;

public class Request {
	public final static int _UID_LENGTH = 15;
	private static int _TOTAL__GENERATED_REQUESTS = 9457099;

	private boolean IS_RESPONSE = true;
	private char[] UID = "000000000000000".toCharArray();
	private byte[] OPCODE;
	private LinkedList<String> Arguments = new LinkedList<String>();

	private ResponseListener Response = null;

	public Request(boolean isResponse, byte[] OPCODE, ResponseListener OnResponse) {
		this.OPCODE = _BASE_OPCODE.isValid_BaseOPCODE(OPCODE) ? OPCODE.clone() : _BASE_OPCODE.VERSION;
		this.IS_RESPONSE = isResponse;
		if (!IS_RESPONSE)
			UID = generateUID();
		this.Response = OnResponse;
	}

	public Request(boolean isResponse, byte[] OPCODE) {
		this(isResponse, OPCODE, null);
	}

	public Request(byte[] OPCODE, ResponseListener OnResponse) {
		this(false, OPCODE, OnResponse);
	}

	public Request(byte[] OPCODE) {
		this(false, OPCODE, null);
	}

	public Request(byte[] OPCODE, String[] Args, ResponseListener OnResponse) {
		this(OPCODE, OnResponse);
		for (String arg : Args)
			Arguments.add(arg);
	}

	public Request(byte[] OPCODE, String[] Args) {
		this(OPCODE, Args, null);
	}

	public void add(String Argument) {
		Arguments.add(Argument);
	}

	public char[] generateUID() {
		UID = new char[_UID_LENGTH];
		int gen = (++_TOTAL__GENERATED_REQUESTS);
		for (int i = 0; i < _UID_LENGTH; i++) {
			UID[i] = gen != 0 ? (char) (((gen % 64) + 33) * (i == 0 ? 1 : i)) : '0';
			gen /= 64;
		}
		return UID;
	}

	public char[] getUID() {
		return UID;
	}

	public byte[] getOPCODE() {
		return OPCODE;
	}

	public boolean hasResponseHandler() {
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

	public ResponseListener getResponse() {
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
