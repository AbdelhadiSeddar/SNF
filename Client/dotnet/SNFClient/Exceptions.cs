using System;

namespace SNFClient
{

    /// <summary>
    ///     Contains all exception thrown by SNFClient
    /// </summary>
    namespace Exceptions
    {
        /// <summary>
        ///     Exceptions when a certain action requires a class to be initialized
        /// </summary>
        /// <remarks>
        ///     Do not throw this Exception Outside of SNFClient
        /// </remarks>
        public class ClassUninitialized: Exception
        {
            /// <summary>
            ///     Called in Case a non-specified Class is needed but not initialized. 
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public ClassUninitialized()
                : base("A Class of SNFClient was not Initialized properly!") {}
            /// <summary>
            ///     Called in case a specified class is needed but not initialized. 
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public ClassUninitialized(string Class)
                : base($"{Class} was not Initialized properly!") {}
        }
        /// <summary>
        ///     Thorown in case a certain instance was not Initialized Properly
        /// </summary>
        /// <remarks>
        ///     Do not throw this Exception Outside of SNFClient
        /// </remarks>
        public class InstanceUninitialized
            : Exception
        {
            /// <summary>
            ///     Thrown in case an instance of a non-specified class was not initialized properly.
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public InstanceUninitialized()
                : base("An Instance of a SNFClient Class was not Initialized properly!") { }
            /// <summary>
            ///     Thrown in case an instance of a specified class was not initialized properly.
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public InstanceUninitialized(string Class) 
                : base($"An Instance of {Class} was not Initialized properly!") { }
        }
        /// <summary>
        ///     Thrown in case a global instance was not initialized properly.
        /// </summary>
        /// <remarks>
        ///     Do not throw this Exception Outside of SNFClient
        /// </remarks>
        public class ClassInstanceUninitialized
            : Exception
        {
            /// <summary>
            ///     Thrown in case a global instance in a non-specified class was not initialized properly
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public ClassInstanceUninitialized()
                : base("The Instance in a SNFClient Class was not Initialized properly!") { }
            /// <summary>
            ///     Thrown in case a global instance in a specified class was not initialized properly
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public ClassInstanceUninitialized(string Class)
                : base($"The Instance in {Class} was not Initialized properly!") { }
        }

        /// <summary>
        ///     Contains Exception related to <see cref="SNFClient.OPCode"/>
        /// </summary>
        /// <remarks>
        ///     Do not throw this Exception Outside of SNFClient
        /// </remarks>
        public static class OPCode
        {
            /// <summary>
            ///     Thrown When an <see cref="SNFClient.OPCode.Member"/> was put somewhere different than it's rank
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public class InvalidRankFound
                : Exception
            {
                /// <summary>
                ///     Thrown When an <see cref="SNFClient.OPCode.Member"/> was put somewhere 
                ///     different than it's <see cref="SNFClient.OPCode.Member.Rank"/>
                /// </summary>
                /// <remarks>
                ///     Do not throw this Exception Outside of SNFClient
                /// </remarks>
                /// <param name="Expected">Member's expected Rank</param>
                /// <param name="Found">Member's actual Rank</param>
                public InvalidRankFound(
                    SNFClient.OPCode.Member.Rank Expected,
                    SNFClient.OPCode.Member.Rank Found
                    ) 
                    : base($"Invalid Rank, Expected {Expected} Found {Found} ") {}
            }
            public class Unregistred
                : Exception
            {
                public Unregistred(
                        byte Code
                    )
                    : base($"OPCode Member 0x{Code} is not registred") { }
            }
        }
        /// <summary>
        ///     Contains Exception related to <see cref="SNFClient.Connection"/>
        /// </summary>
        /// <remarks>
        ///     Do not throw this Exception Outside of SNFClient
        /// </remarks>
        public static class Connection
        {
            /// <summary>
            ///     Thrown in case you are changing a value that cannot be changed whilst establishing a Connection
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public class Connecting
                : Exception
            {
                /// <summary>
                ///     Thrown in case you are changing a value that cannot be changed whilst establishing a Connection
                /// </summary>
                /// <remarks>
                ///     Do not throw this Exception Outside of SNFClient
                /// </remarks>
                public Connecting()
                    : base("Instance Connecting, Call this before Connect()!!") { }
            }
            /// <summary>
            ///     Thrown in case you are changing a value that cannot be changed after establishing a Connection
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public class AlreadyConnected
                : Exception
            {
                /// <summary>
                ///     Thrown in case you are changing a value that cannot be changed after establishing a Connection
                /// </summary>
                /// <remarks>
                ///     Do not throw this Exception Outside of SNFClient
                /// </remarks>
                public AlreadyConnected()
                    : base("Instance Already Connected, You cannot Change this.") { }
            }
            /// <summary>
            ///     Thrown in case you are calling a method that requires to be called while establishing a Connection
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public class NotConnecting
                : Exception
            {
                /// <summary>
                ///     Thrown in case you are calling a method that requires to be called while establishing a Connection
                /// </summary>
                /// <remarks>
                ///     Do not throw this Exception Outside of SNFClient
                /// </remarks>
                public NotConnecting()
                    : base("Instance Not Connecting, Call Connect() Beforehand!!") {}
            }
            /// <summary>
            ///     Thrown in case you are calling a method that requires establishing a Connection
            /// </summary>
            /// <remarks>
            ///     Do not throw this Exception Outside of SNFClient
            /// </remarks>
            public class NotConnected
                : Exception
            {
                /// <summary>
                ///     Thrown in case you are calling a method that requires to be called while establishing a Connection
                /// </summary>
                /// <remarks>
                ///     Do not throw this Exception Outside of SNFClient
                /// </remarks>
                public NotConnected()
                    : base("Instance Not Connected, Call Connect() Beforehand!!") { }
            }
        }
    }
}
