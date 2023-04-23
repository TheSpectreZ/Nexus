using System;

namespace Nexus
{
    public class Main
    {
        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main Construtor");
        }

        ~Main()
        {
            Console.WriteLine("Main Destructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World From C#");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }
        
        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
    }
}
