using System;

namespace Nexus
{
    public class Input
    {
        static public bool IsKeyPressed(KeyCode keyCode)
        {
            return InternalCalls.Input_IsKeyPressed((UInt16)keyCode);
        }
        static public bool IsMouseButtonPressed(MouseCode mouseCode)
        {
            return InternalCalls.Input_IsMouseButtonPressed((UInt16)mouseCode);
        }
    }
}
