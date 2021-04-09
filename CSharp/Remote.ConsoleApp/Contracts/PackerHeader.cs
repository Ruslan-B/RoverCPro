using System.Runtime.InteropServices;

namespace Remote.ConsoleApp.Contracts
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PackerHeader
    {
        public byte Id;
        public PayloadType PayloadType;
        public byte PayloadSize;
    }
}