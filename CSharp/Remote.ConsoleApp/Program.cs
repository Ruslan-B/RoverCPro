using System;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using Remote.ConsoleApp.Contracts;
using SharpDX.DirectInput;

namespace Remote.ConsoleApp
{
    internal static class BytesHelper
    {
        public static byte[] GetBytes<T>(T str) where T : struct
        {
            var size = Marshal.SizeOf<T>();
            var arr = new byte[size];

            var ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(str, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        private static T FromBytes<T>(byte[] arr) where T : struct
        {
            var size = Marshal.SizeOf<T>();
            var ptr = Marshal.AllocHGlobal(size);

            Marshal.Copy(arr, 0, ptr, size);

            var str = Marshal.PtrToStructure<T>(ptr);
            Marshal.FreeHGlobal(ptr);

            return str;
        }
    }

    internal static class Program
    {
        private static void Main(string[] args)
        {
            // Initialize DirectInput
            var directInput = new DirectInput();

            // Find a Joystick Guid
            var joystickGuid = Guid.Empty;
            do
            {
                foreach (var deviceInstance in directInput.GetDevices(DeviceType.Gamepad,
                    DeviceEnumerationFlags.AllDevices))
                {
                    joystickGuid = deviceInstance.InstanceGuid;
                    break;
                }


                if (joystickGuid == Guid.Empty)
                    foreach (var deviceInstance in directInput.GetDevices(DeviceType.Joystick,
                        DeviceEnumerationFlags.AllDevices))
                    {
                        joystickGuid = deviceInstance.InstanceGuid;
                        break;
                    }

                if (joystickGuid != Guid.Empty) break;
                Console.WriteLine("No joystick/Gamepad found. Press any key to retry.");
                Console.ReadKey();
            } while (true);

            // Instantiate the joystick
            var joystick = new Joystick(directInput, joystickGuid);
            joystick.Properties.BufferSize = 128;

            Console.WriteLine($"Found Joystick/Gamepad {joystick.Information.ProductName}");

            joystick.Acquire();

            var movePacketHeader = new PackerHeader
            {
                Id = (byte) 'R',
                PayloadType = PayloadType.Move,
                PayloadSize = (byte) Marshal.SizeOf<RoverMove>()
            };
            var movePacketBytes = BytesHelper.GetBytes(movePacketHeader);
            var move = new RoverMove {MoveX = 0, MoveY = 0, Rotation = 0, Grip = false};

            var roverIpAddress = IPAddress.Parse("192.168.178.43");
            var port = 32768;
            var endPoint = new IPEndPoint(roverIpAddress, port);
            var udpClient = new UdpClient(port);
            var sw = Stopwatch.StartNew();
            while (true)
            {
                joystick.Poll();
                var data = joystick.GetBufferedData();
                foreach (var state in data)
                {
                    const float upperThreshold = 20000f;
                    const int center = 32768;
                    const int deadZone = 10000;

                    var centered = center - state.Value;
                    var limited = Math.Min(Math.Max(centered, -upperThreshold), upperThreshold);
                    var normalized =(sbyte) (100 * limited / upperThreshold);
                    var filtered = Math.Abs(limited) > deadZone ? normalized : (sbyte) 0;
                    switch (state.Offset)
                    {
                        case JoystickOffset.X:
                            move.MoveX = filtered;
                            break;
                        case JoystickOffset.Y:
                            move.MoveY = filtered;
                            break;
                        case JoystickOffset.RotationX:
                            move.Rotation = filtered;
                            break;
                        case JoystickOffset.RotationY:
                            move.MoveX = filtered;
                            break;
                        case JoystickOffset.Buttons0:
                            move.Grip = true;
                            break;
                        case JoystickOffset.Buttons1:
                            move.Grip = false;
                            break;
                        default:
                            continue;
                    }

                    Console.WriteLine($"Move X: {move.MoveX} Y:{move.MoveY} R:{move.Rotation} G:{move.Grip}");
                    
                    var available = udpClient.Available;
                    if (available > 0) Console.WriteLine($"Got back {available} bytes");
                }

                if (sw.Elapsed <= TimeSpan.FromMilliseconds(50)) continue;

                sw.Restart();
                var bytes = movePacketBytes.Concat(BytesHelper.GetBytes(move)).ToArray();
                udpClient.Send(bytes, bytes.Length, endPoint);
            }
        }
    }
}