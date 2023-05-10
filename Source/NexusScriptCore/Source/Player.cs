using System;
using Nexus;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent Transform;
        private RigidBodyComponent RigidBody;
        public override void OnCreate() 
        {
            Console.WriteLine($"Player.OnCreate: {ID}");
            Transform = GetComponent<TransformComponent>();

            if(HasComponent<RigidBodyComponent>())
            {
                RigidBody = GetComponent<RigidBodyComponent>();
                RigidBody.Mass = 100;
            }

        }
        public override void OnUpdate(float ts)
        {
            if(Input.IsKeyPressed(KeyCode.Space))
            {
                Console.WriteLine("Jump!!");
            }

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
