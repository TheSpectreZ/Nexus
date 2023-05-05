using System;
using Nexus;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent Transform;
        public override void OnCreate() 
        {
            Console.WriteLine($"Player.OnCreate: {ID}");
            Transform = GetComponent<TransformComponent>();
        }
        public override void OnUpdate(float ts)
        {
            Vector3 pos = Transform.Position;
            pos.X += 10 * ts;
            Transform.Position = pos;
        }
        public override void OnDestroy()
        {
            Console.WriteLine("Player.OnDestroy");
        }
    }
}
