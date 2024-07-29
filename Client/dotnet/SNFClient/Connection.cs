using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;
using SNFClient.Callbacks;
using SNFClient.Exceptions;

namespace SNFClient
{
    /// <summary>
    ///     Objects of this class are what controls and handles Connections to the Host
    /// </summary>
    public class Connection
    {
        #region Static Connection
        private static Connection _instance = null;
        private static string ServerHost = "localhost";
        private static int ServerPort = 9114;
        private static bool isInit = false;

        /// <summary>
        /// Returns the Global Connection Instance
        /// </summary>
        /// <returns>
        ///     The Global Connection | <c>null</c> if no instance was registred
        /// </returns>
        public static Connection get() => _instance;
        /// <summary>
        /// Returns if the Global Connection Instance ( See <see cref="get"/> ) 
        /// is instatiated or not.
        /// </summary>
        /// <seealso cref="Instantiate()"/>
        public static bool isInitialized { get => isInit; }
        /// <summary>
        /// Instantiates a new Global Connection Instance 
        /// </summary>
        /// <param name="host">The SNF host to Connect to</param>
        /// <param name="Port">The Port of the SNF host</param>
        /// <returns>
        ///     The new Connection Instance
        /// </returns> 
        /// <exception cref="ClassUninitialized">
        ///     If OPCode was not Initialized
        ///  </exception>
        public static Connection Instantiate(string host, int Port)
        {
            if (!OPCode.Base.isInitialized)
                throw new ClassUninitialized("OPCode.Base");
            
            Connection Connection = new Connection();

            Connection._host = host;
            Connection._port = Port;

            Connection.isInstInit = true;

            return Connection;
        }
        /// <summary>
        /// Instantiats a Global Connection instance
        /// </summary>
        /// <remarks>
        /// @note 
        ///     If you want to instantiate a non Global Connection Instance use 
        ///     <see cref="Instantiate(string, int)"/>
        /// </remarks>
        /// <returns>
        ///     The Global Connection Instance
        /// </returns>
        public static Connection Instantiate()
            => (_instance = Instantiate(ServerHost, ServerPort));

        /// <summary>
        /// Changes The Default Host URL
        /// </summary>
        /// <param name="host">New Default SNF Host</param>
        public static void SetDefaultHost(string host) => ServerHost = host;
        /// <summary>
        /// Changes The Default Port 
        /// </summary>
        /// <param name="port">New Default SNF Port</param>
        public static void SetDefaultPort(int port) => ServerPort = port;
        
        /// <summary>
        /// Send a Request through the Global Connection Instance
        /// </summary>
        /// <param name="R">Request to send</param>
        /// <exception cref="ClassInstanceUninitialized">
        ///     If The Global Connection Instance is not instantiated
        ///     <seealso cref="Instantiate()"/>
        /// </exception>
        public static void InstanceSend(Request R)
        {
            if (_instance == null)
                throw new ClassInstanceUninitialized(new Connection().ToString());
            _instance.Send(R);
        }
        #endregion

        #region Callbacks
        private SimpleCB OnConnect;
        internal void InvokeOnConnection()
        {
            OnConnect?.Invoke();
        }
        private SimpleCB OnSNFFail;
        internal void InvokeOnSNFFail()
        {
            OnSNFFail?.Invoke();
        }
        private SocketExceptionCB OnSocketFail;
        internal void InvokeOnSocketFail(SocketException ex)
        {
            OnSocketFail?.Invoke(ex);
        }
        private ExceptionCB OnException;
        internal void InvokeOnException(Exception ex)
        {
            OnException?.Invoke(ex);
        }
        private SimpleCB OnTimeout;
        internal void InvokeOnTimeout()
        {
            OnTimeout?.Invoke();
        }
        #endregion

        private Request.Handler _handler;
        private Socket _socket = null;
        private string _host = ServerHost;
        private int _port = ServerPort;
        private bool isInstInit = false;
        private bool _isconn = false;
        private Semaphore _cnx_sem = new Semaphore(0, 5);

        internal Stream _stream = null;
        internal ClientInfo _client = null;

        /// <summary>
        /// Returns if the Instance is currently Connected
        /// </summary>
        public bool isConnected 
            => _isconn;
        /// <summary>
        /// Returns The Instance's Host URL
        /// </summary>
        public string Host 
            => _host;
        /// <summary>
        /// Returns The instance's connection port
        /// </summary>
        public int Port 
            => _port;
        /// <summary>
        /// Returns ClientInfo of the Client
        /// </summary>
        /// <seealso cref="ClientInfo"/>
        public ClientInfo ClientInfo 
            => _client;
        /// <summary>
        /// Returns if the instance is initialized or not
        /// </summary>
        /// <remarks> 
        ///    @note If  instance is not Initialized, then instanciate a new one, 
        ///    as by default Any instantiated Connection object is initialized if no
        ///    problem occured.
        /// </remarks>
        public bool isInstanceInitialized 
            => isInstInit;

        /// <summary>
        /// Create a new Connection Object
        /// </summary>
        public Connection()
        { }
        /// <summary>
        /// Connects The instance to the target host.
        /// </summary>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        public Connection Connect()
        {
            _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            _handler = new Request.Handler();
            try
            {
                _socket.Connect(Host, Port);

                _stream = new NetworkStream(_socket);
                _handler.OnConnection += () =>
                {
                    _isconn = true;
                    _cnx_sem.Release();
                };
                _handler.InstanceInistialize(this);
            }
            catch (SocketException e)
            {
                InvokeOnSocketFail(e);
                return null;
            }
            return this;
        }
        private void InitialRequestResponse(Request Response)
        {
            if(Response.OPCode.isBase())
            {
                if (Response.OPCode?.Command.Value == OPCode.Base.CMD_CONFIRM)
                {
                    InvokeOnConnection();
                }
                else
                    InvokeOnSNFFail();
            }
            else
                InvokeOnSNFFail();
        }
        /// <summary>
        ///     Set's the connection host for this instance
        /// </summary>
        /// <param name="Host">The new connection host</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        /// <seealso cref="SetDefaultHost(string)"/>
        public Connection SetHost(string Host)
        {
            if (_isconn)
                throw new Exceptions.Connection.AlreadyConnected();

            _host = Host;
            return this;
        }

        /// <summary>
        ///     Sets the connection host's port for this instance
        /// </summary>
        /// <param name="Port">The new Conenction's Port</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        /// <seealso cref="SetDefaultPort(int)"/>
        public Connection SetPort(int Port)
        {
            if (_isconn)
                throw new Exceptions.Connection.AlreadyConnected();

            _port = Port;
            return this;
        }
        /// <summary>
        ///     Sets The callbackto be called Upon Connection
        /// </summary>
        /// <param name="OnConnect">Callback to called</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        public Connection SetOnConnection(SimpleCB OnConnect)
        {
            if (_handler != null)
                throw new Exceptions.Connection.Connecting();

            this.OnConnect += OnConnect;
            return this;
        }
        /// <summary>
        ///     Sets The callbackto be called Upon failure of the SNF Protocol
        /// </summary>
        /// <param name="OnSNFFail">Callback to called</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        public Connection SetOnSNFFail(SimpleCB OnSNFFail)
        {
            if (_handler != null)
                throw new Exceptions.Connection.Connecting();

            this.OnSNFFail += OnSNFFail;
            return this;
        }
        /// <summary>
        ///     Sets The callbackto be called Upon Socket Connection Failure
        /// </summary>
        /// <param name="OnSocketFail">Callback to called</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        public Connection SetOnSocketFail(SocketExceptionCB OnSocketFail)
        {
            if (_handler != null)
                throw new Exceptions.Connection.Connecting();

            this.OnSocketFail += OnSocketFail;
            return this;
        }
        /// <summary>
        ///     Sets The callbackto be called Upon Any Exception that is not 
        /// </summary>
        /// <param name="OnException">Callback to called</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        public Connection SetOnException(ExceptionCB OnException)
        {
            if (_handler != null)
                throw new Exceptions.Connection.Connecting();

            this.OnException += OnException;
            return this;
        }
        /// <summary>
        ///     Sets The callbackto be called Connection Timeout
        /// </summary>
        /// <param name="OnTimeout">Callback to called</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="Exceptions.Connection.AlreadyConnected">
        ///     Cannot Change this variable when the connection has already connected
        /// </exception>
        public Connection SetOnTimeout(SimpleCB OnTimeout)
        {
            if (_handler != null)
                throw new Exceptions.Connection.Connecting();

            this.OnTimeout += OnTimeout;
            return this;
        }
        /// <summary>
        ///     Block Until the instance is Connected
        /// </summary>
        /// <param name="timeout"> Maximum Allowed Timeout</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="InstanceUninitialized">
        ///     Instance was not Initialized
        /// </exception>
        /// <exception cref="Exceptions.Connection.NotConnecting">
        ///     Instance was not connecting, Call this after <see cref="Connect"/>
        /// </exception>
        public Connection AwaitConnection(TimeSpan timeout)
        {
            if (!isInstanceInitialized)
                throw new InstanceUninitialized(this.ToString());
            if (_handler == null)
                throw new Exceptions.Connection.NotConnecting();
            if (_cnx_sem.WaitOne(timeout))
                return this;
            else
            {
                if(!_isconn)
                    OnTimeout?.Invoke();
                return null;
            }
        }
        /// <summary>
        ///     Block Until the instance is Connected in Miliseconds
        /// </summary>
        /// <param name="milisecondtimeout"> Maximum Allowed Timeout in Miliseconds</param>
        /// <returns>
        ///     Connection Instance
        /// </returns>
        /// <exception cref="InstanceUninitialized">
        ///     Instance was not Initialized
        /// </exception>
        /// <exception cref="Exceptions.Connection.NotConnecting">
        ///     Instance was not connecting, Call this after <see cref="Connect"/>
        /// </exception>
        public Connection AwaitConnection(int milisecondtimeout)
        {
            if (!isInstanceInitialized)
                throw new InstanceUninitialized(this.ToString());
            if(_handler == null)
                throw new Exceptions.Connection.NotConnecting();

            if (_cnx_sem.WaitOne(milisecondtimeout))
                return this;
            else
                // TODO: Somehow delete the instance
                return null;
        }
        /// <summary>
        ///     Sends the request through the connection instance
        /// </summary>
        /// <param name="r">Request to be sent</param>
        /// <exception cref="InstanceUninitialized">
        ///     Instance was not initialized
        /// </exception>
        /// <exception cref="Exceptions.Connection.NotConnected">
        ///     Instance was not connected
        /// </exception>
        public void Send(Request r)
        {
            if (!isInstanceInitialized) 
                throw new InstanceUninitialized();
            if (!_isconn)
                throw new Exceptions.Connection.NotConnected();

            _handler.AddRequest(r);
        }

    }


}
