using System;

namespace SNFClient
{
    /// <summary>
    /// This is an Internal class that contains members that would be used in this project
    /// </summary>
    internal class Utility
    {
        internal static char UNIT_SEPARATOR => (char)31;

        internal static byte[] IntToBytes(int interger) 
            => BitConverter.GetBytes(interger);

        internal static int BytesToInt(byte[] bytes) 
        {
            int re = 0;
            for (int i = 0; i < 4; i++)
            {
                re += (bytes[3 - i] << (8 * i));
            }
            return re;
        }
    }
}
