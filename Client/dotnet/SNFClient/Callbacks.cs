using System;
using System.Net.Sockets;

namespace SNFClient
{
    /// <summary>
    ///     Defines The Callbacks need for Event Handling
    /// </summary>
    namespace Callbacks
    {
        /// <summary>
        ///     Used for Events that Requires no Arguments
        /// </summary>
        public delegate void SimpleCB();

        /// <summary>
        ///     Used for Handling Any unhandled Exception
        /// </summary>
        /// <param name="ex">The Unknown Exception</param>
        public delegate void ExceptionCB(Exception ex);
        /// <summary>
        ///     Used For Handling any Exception that happend during The Connection to The Host
        /// </summary>
        /// <param name="ex">Socket Exception</param>
        public delegate void SocketExceptionCB(SocketException ex);
        /// <summary>
        ///     Used for handling Incoming requests Either Response or Server.
        /// </summary>
        /// <param name="Response"> The Server's Request (Response or not)</param>
        public delegate void RequestCB(Request Response);
    }
}
