using System;
using Nexus;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent Transform;

        public float speed;

        public override void OnCreate() 
        {
            speed = 1.0f;;
            Transform = GetComponent<TransformComponent>();
        }
        public override void OnUpdate(float ts)
        {
            Vector3 pos = Transform.Position;

            if(Input.IsKeyPressed(KeyCode.Right))
                pos.X += speed * ts;
            else if(Input.IsKeyPressed(KeyCode.Left))
                pos.X -= speed * ts;
            if(Input.IsKeyPressed(KeyCode.Up))
                pos.Z += speed * ts;
            else if(Input.IsKeyPressed(KeyCode.Down))
                pos.Z -= speed * ts;

            Transform.Position = pos;
        }
        public override void OnDestroy()
        {
            
        }
    }
}
