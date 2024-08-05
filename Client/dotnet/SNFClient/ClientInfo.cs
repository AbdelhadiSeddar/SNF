using System;

namespace SNFClient
{
    /// <summary>
    ///     Contains Informations about the client Connected through the <see cref="Connection"/> instance
    /// </summary>
    public class ClientInfo
    {
        #region Static ClientInfo
        /// <summary>
        /// Returns The Library Version 
        /// </summary>
        /// <remarks>
        ///     It returns a string that is as similair to the Semantic Versioning 2.0.0
        ///     Libraries May have Different PATCH Levels 
        /// </remarks>
        public static string Version 
            => "" + Version_MAJOR 
            + "." + Version_MINOR 
            + "." + Version_PATCH 
            + Version_EXTRA;
        /// <summary>
        ///     Major Version Number
        /// </summary>
        public static int Version_MAJOR => 0;
        /// <summary>
        ///     Minor Version Number
        /// </summary>
        public static int Version_MINOR => 0;
        /// <summary>
        ///     Patch Version Number
        /// </summary>
        public static int Version_PATCH => 2;
        /// <summary>
        ///     Extra labels ( eg Alpha Beta )
        /// </summary>
        public static string Version_EXTRA => "-alpha";
        #endregion

        private Guid _guid;
        /// <summary>
        ///     Unique Identifier for the Client, Must be given by the Server
        /// </summary>
        public Guid UUID 
            { get => _guid; set => _guid = value; }
        /// <summary>
        ///     Creates a new Client Info with a random UUID
        /// </summary>
        public ClientInfo()
        {
            _guid = Guid.NewGuid();
        }
        /// <summary>
        ///     Creates a new ClientInfo instance with a Set UUID
        /// </summary>
        /// <param name="guid">Client's UUID</param>
        public ClientInfo(Guid guid)
        {
            _guid = guid;
        }
        /// <summary>
        ///     Creates a new ClientInfo instance with a Set UUID
        /// </summary>
        /// <param name="guid">Client's UUID</param>
        public ClientInfo(string guid)
            : this(new Guid(guid)) { }

    }
}
