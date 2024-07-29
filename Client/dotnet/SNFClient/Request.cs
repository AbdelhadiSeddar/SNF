using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Text;

using SNFClient.Callbacks;
using SNFClient.Exceptions;
using System.Net.Sockets;
using System.Security.Cryptography;

namespace SNFClient
{
    /// <summary>
    /// "Request" Members are the object that are sent through SNF.Client.Connection
    /// </summary>
    public class Request
    {
        #region Static Request
        /// <summary>
        /// The length of the UID of each Request "Request"
        /// </summary>
        public static int _UID_LENGTH 
            => 16;
        /// <summary>
        /// Saved the total amount of generated requests
        /// </summary>
        private static int _TOTAL_GENERATED_REQUESTS = 0;
        /// <summary>
        /// Generates a new UID for Each Request
        /// </summary>
        /// <returns></returns>
        public static byte[] generateUID()
        {
            byte[] UID = new byte[_UID_LENGTH];
            int gen = (++_TOTAL_GENERATED_REQUESTS);
            for (int i = 0; i < _UID_LENGTH - 1; i++)
            {
                UID[i] = (byte)((gen != 0 ? (((gen % 64) + 33) * (i == 0 ? 1 : i)) : '0') & 0xFF);
                gen /= 64;
            }
            UID[_UID_LENGTH - 1] = 0x00;
            return UID;
        }
        #endregion

        #region Callbacks

        /// <summary>
        /// Saves The Callback When this request is responded to. 
        /// </summary>
        internal RequestResponseCB OnResponse;
        /// <summary>
        /// Calls <paramref name="Response"/> Member
        /// </summary>
        /// <param name="Response"></param>
        internal void Responded(Request Response)
        {
            OnResponse?.Invoke(Response);
        }

        #endregion

        private byte[] uid;
        private OPCode op;
        private LinkedList<byte[]> args = new LinkedList<byte[]>();
        private bool awaitsresponse = false;

        private Request() { }

        /// <summary>
        /// Used to get the UID of the Request
        /// </summary>
        /// <remarks>
        /// > [!NOTE]
        /// > You can generate a UID using <see cref="generateUID"/>
        /// </remarks>
        public byte[] UID 
            => uid;
        /// <summary>
        /// Used to get the <see cref="OPCode"/> of the the Request
        /// </summary>
        /// <seealso cref="OPCode"/>
        public OPCode OPCode
            => op;
        /// <summary>
        /// Used to get the Arguments that would be sent with the Request
        /// </summary>
        public byte[][] Args 
            => args.ToArray();
        /// <summary>
        /// Used to get if the request await a response
        /// </summary>
        public bool Awaitsresponse 
            => awaitsresponse;

        /// <summary>
        /// Constructor for a new Request Instance.
        /// </summary>
        /// <param name="UID">Unique IDentifier for the Request</param>
        /// <param name="OPCODE">OPCode of the Request</param>
        /// <param name="Args">Arguments to be Sent</param>
        /// <param name="OnResponse">Response CallBack</param>
        public Request(byte[] UID, OPCode OPCODE, byte[][] Args, RequestResponseCB OnResponse) : this()
        {
            this.uid = UID;
            this.op = OPCODE;
            foreach (byte[] arg in Args)
                args.AddLast(arg);
            if(OnResponse != null)
                awaitsresponse = true;

            this.OnResponse += OnResponse;
        }
        /// <summary>
        /// Constructor for a new Request Instance without a CallBack
        /// </summary>
        /// <param name="UID">Unique IDentifier for the Request</param>
        /// <param name="OPCODE">OPCode of the Request</param>
        /// <param name="Args">Arguments to be Sent</param>
        public Request(byte[] UID, OPCode OPCODE, byte[][] Args)
            : this(UID, OPCODE, Args, null) { }
        /// <summary>
        /// Constructor for a new Request Instance with a UID Generated with <see cref="generateUID"/>
        /// </summary>
        /// <param name="OPCODE">OPCode of the Request</param>
        /// <param name="Args">Arguments to be Sent</param>
        /// <param name="OnResponse">Response CallBack</param>
        public Request(OPCode OPCODE, byte[][] Args, RequestResponseCB OnResponse)
            : this ( generateUID(), OPCODE, Args, OnResponse) { }

        /// <summary>
        /// Constructor for a new Request Instance with a UID Generated with <see cref="generateUID"/> and without a Callback
        /// </summary>
        /// <param name="OPCODE">OPCode of the Request</param>
        /// <param name="Args">Arguments to be Sent</param>
        public Request(OPCode OPCODE, byte[][] Args)
            : this ( OPCODE, Args, null) { }
        /// <summary>
        /// Constructor for a new Request Instance with a UID Generated with <see cref="generateUID"/> and without any Arguments
        /// </summary>
        /// <param name="OPCODE">OPCode of the Request</param>
        /// <param name="OnResponse">Response CallBack</param>
        public Request(OPCode OPCODE, RequestResponseCB OnResponse)
            : this (OPCODE, new byte[0][], OnResponse) { }
        /// <summary>
        /// Constructor for a new Request Instance with a UID Generated with <see cref="generateUID"/> and without any Arguments or a Callback
        /// </summary>
        /// <param name="OPCODE">OPCode of the Request</param>
        public Request(OPCode OPCODE)
            : this (OPCODE, new byte[0][], null) { }

        /// <summary>
        /// Gets the First Argument of a Request
        /// </summary>
        /// <returns>
        ///    First Argument | <c>null</c> if instance has no Arguments
        /// </returns>
        public byte[] FirstArg()
            => Args.Length > 0 ? Args[0] : null;
        /// <summary>
        /// Gets the Last Argument of a Request
        /// </summary>
        /// <returns>
        ///    last Argument | <c>null</c> if instance has no Arguments
        /// </returns>
        public byte[] LastArg()
            => Args.Length > 0 ? Args[Args.Length -1] : null;
        /// <summary>
        /// Get the indexed Argument
        /// </summary>
        /// <param name="Index">Index of the aregument</param>
        /// <returns>
        ///    Indexed Argument
        /// </returns>
        public byte[] Arg(int Index)
            => Args.Length > 0 ?
                Args[Index]
                : null;
        
        /// <summary>
        /// This Class where each instance Handles the created Requests
        /// and sends them through the <see cref="Connection"/> it was Initialized
        /// with.
        /// </summary>
        public class Handler
        {
            internal Handler() { }
            #region Static Handler
            private static bool isInit = false;
            /// <summary>
            /// Gets if the Handler Instance is Initialized or not
            /// </summary>
            public static bool isInitialized => isInit;
            private static Handler _instance;
            /// <summary>
            /// Allows you to Change/Get the default Handler instance
            /// </summary>
            public static Handler Instance { get => _instance; set => _instance = value; }
            /// <summary>
            /// Initializes a new Handler instance with the <see cref="Connection"/> instance
            /// </summary>
            /// <param name="Cnx">The Connection Instance</param>
            public static void Initialize(Connection Cnx)
            {
                _instance = new Handler();
                _instance.InstanceInistialize(Cnx);
                isInit = true;
            }
            #endregion

            #region Callbacks

            internal SimpleCB OnConnection;
            #endregion
            
            internal Queue<Request> QueuedRequest = new Queue<Request>();
            internal LinkedList<Request> UnrespondedRequests = new LinkedList<Request>();

            private bool KeepRunning = true;
            private Semaphore _sem = new Semaphore(0, 5);
            private Semaphore _wait_Connection = new Semaphore(0, 1);
            private Connection _connection;
            private Stream _stream => _connection?._stream;
            private object _reader;
            private object _writer;

            private Thread StreamReaderhandler;
            private Thread StreamWriterhandler;

            private void HandleStreamReader()
            {
                if (!OPCode.Base.isInitialized)
                    throw new ClassUninitialized("OPCode.Base");
                _wait_Connection.WaitOne();

                while(KeepRunning)
                {
                    try
                    {
                        Request R = Receive();
                        Request ClientR = null;
                        
                        lock(UnrespondedRequests)
                        {
                            foreach(Request request in UnrespondedRequests)
                            {
                                for (int i = 0; i < request.UID.Length; i++)
                                    if (request.UID[i] != R.UID[i])
                                        break;
                                ClientR = request;
                                break;
                            }
                        }
                        if(ClientR != null)
                            UnrespondedRequests.Remove(ClientR);
                        ClientR?.Responded(R);
                    }
                    catch (Exception ex)
                    {
                        _connection.InvokeOnException(ex);
                    }
                }
                
            }
            private void HandleStreamWriter()
            {
                if (!OPCode.Base.isInitialized)
                    throw new ClassUninitialized("OPCode.Base");
                if (_stream == null || _connection == null)
                    throw new ClassUninitialized("Request.Handler");
                try
                {
                    if (_connection.ClientInfo == null)
                    {
                        OPCode op = OPCode.Base.get(OPCode.Base.CMD_CONNECT);
                        if (op == null)
                            throw new ClassUninitialized("OPCode.Base");

                        _stream.Write(op.ToBytes(), 0, 4);
                        byte[] CUID = new byte[36];
                        _stream.Read(CUID, 0, 36);
                        string CUID_Str = Encoding.ASCII.GetString(CUID);
                        _connection._client = new ClientInfo(CUID_Str);
                        OnConnection?.Invoke();
                        _connection.InvokeOnConnection();
                    }
                    _wait_Connection.Release(1);
                }
                catch (SocketException ex)
                {
                    _connection.InvokeOnSocketFail(ex);
                    
                    _wait_Connection.Release();
                    return;
                }

                while (KeepRunning)
                {
                    try
                    {
                        _sem.WaitOne();
                        Request R;
                        lock (QueuedRequest)
                        {
                            R = QueuedRequest.Dequeue();
                        }
                        Send(R);
                        
                        if(R.awaitsresponse)
                            lock(UnrespondedRequests)
                            {
                                UnrespondedRequests.AddLast(R);
                            }
                    }
                    catch (Exception ex)
                    {
                        _connection.InvokeOnException(ex);
                    }
                }
            }

            private bool isInstInit = false;
            /// <summary>
            /// Gets is the Instance is Initialized or not
            /// </summary>
            public bool IsInstanceInitialized 
                => isInstInit;

            internal void InstanceInistialize(Connection Connection)
            {
                if (isInstInit)
                    return;
                if (Connection == null)
                {
                    if (Connection.get() == null)
                        throw new ClassUninitialized("Connection");
                    else
                        _connection = Connection.get();
                }
                else
                    this._connection = Connection;

                _reader = new object();
                _writer = new object();
                StreamReaderhandler = new Thread(new ThreadStart(HandleStreamReader));
                StreamWriterhandler = new Thread(new ThreadStart(HandleStreamWriter));
                StreamReaderhandler.Start();
                StreamWriterhandler.Start();

                isInstInit = true;
            }

            internal void AddRequest(Request request)
            {
                if (!this.isInstInit)
                    throw new ClassUninitialized("Request.Handler");

                lock (QueuedRequest)
                {
                    QueuedRequest.Enqueue(request);
                    _sem.Release();
                }
            }

            internal void Send(Request r)
            {
                if(_stream == null || _connection == null || _connection._client == null)
                    throw new ClassUninitialized();
                if (r.OPCode == null )
                    return;

                byte[] CUID = Encoding.ASCII.GetBytes(_connection._client.UUID.ToString());
                byte[] RUID = r.UID;
                byte[] OPCODE = r.OPCode.ToBytes();
                LinkedList<byte> args = new LinkedList<byte>();
                foreach (byte[] arg in r.args)
                {
                    foreach (byte b in arg)
                        args.AddLast(b);
                    args.AddLast((byte)Utility.UNIT_SEPARATOR);
                }
                args.AddLast(0x00);
                byte[] Content = args.ToArray();
                byte[] fb = Utility.IntToBytes(args.Count == 1 ? 0 : args.Count);

                write(CUID);
                write(OPCODE);
                write(RUID);
                write(fb);
                if (args.Count > 1)
                    write(Content);
            }
            private Request Receive()
            {
                byte[] opcode = read(4);
                byte[] RUID = read(_UID_LENGTH);
                byte[] fb = read(4);
                byte[] Content = read(Utility.BytesToInt(fb));
                LinkedList<byte[]> args = new LinkedList<byte[]>();
                if(Utility.BytesToInt(fb) > 0)
                {
                    LinkedList<byte> arg = new LinkedList<byte>();
                    foreach(byte b in Content)
                    {
                        if(b == Utility.UNIT_SEPARATOR)
                        {
                            args.AddLast(arg.ToArray());
                            arg = new LinkedList<byte>();
                        }
                        arg.AddLast(b);
                    }
                    args.AddLast(arg.ToArray());
                }
                return new Request(
                    RUID,
                    OPCode.get(
                        opcode[0], 
                        opcode[1], 
                        opcode[2], 
                        opcode[3]
                        ),
                    args.ToArray()
                    );
            }
            private void write(byte[] bytes)
            {
                if (!this.isInstInit)
                    throw new ClassUninitialized("Request.Handler");
                lock (_writer)
                {
                    _stream.Write(bytes, 0, bytes.Length);
                }
            }
            private byte[] read(int length)
            {
                if (!this.isInstInit)
                    throw new ClassUninitialized("Request.Handler");
                byte[] re = new byte[length];
                lock (_reader)
                {
                    _stream.Read(re, 0, length);
                }
                return re;
            }
            private byte[][] read(int length, byte Splitter)
            {
                if (!this.isInstInit)
                    throw new ClassUninitialized("Request.Handler");

                LinkedList<byte[]> re = new LinkedList<byte[]>();

                Splitter &= 0xFF;

                for(int i = 0; i < length; i++)
                {
                    LinkedList<byte> word = new LinkedList<byte>();
                    while( i < length )
                    {
                        int b = _stream.ReadByte();
                        if (b == -1)
                            break;
                        byte B = (byte)(b & 0xFF);
                        i++;
                        word.AddLast(B);
                    }
                    re.AddLast( word.ToArray());
                }
                return re.ToArray();
            }
            /// <summary>
            /// Joins until The Handler's Threads are stopped
            /// </summary>
            public void JoinHandlers()
            {
                StreamReaderhandler.Join();
                StreamWriterhandler.Join();
            }
            /// <summary>
            /// Sends a Stop signal
            /// </summary>
            /// <remarks>
            /// > [!NOTE]
            /// > It will not guarantee that it will be stopped immediatly, call <see cref="JoinHandlers"/> to wait until it stops
            /// </remarks>
            public void StopHandlers()
            {
                KeepRunning = true;
                _sem.Release();
            }

        }

    }
}
