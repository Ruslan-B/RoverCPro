namespace Remote.ConsoleApp.Contracts
{
    public enum PayloadType : byte
    {
        None = 0,
        Ack = 1,
        Move = 2
    }
}