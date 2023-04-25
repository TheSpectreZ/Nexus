using System;
using Nexus;

namespace Sandbox
{
    public class Player : Entity
    {
        public override void OnCreate() 
        {
            Console.WriteLine("Player.OnCreate");
        }
        public override void OnUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");
        }
        public override void OnDestroy()
        {
            Console.WriteLine("Player.OnDestroy");
        }
    }
}
