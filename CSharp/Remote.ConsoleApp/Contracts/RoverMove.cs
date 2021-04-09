using System.Runtime.InteropServices;

namespace Remote.ConsoleApp.Contracts
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct RoverMove
    {
        public sbyte MoveX;
        public sbyte MoveY;
        public sbyte Rotation;
        public bool Grip;
    }
}