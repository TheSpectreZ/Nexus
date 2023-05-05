using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Nexus
{
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 zero = new Vector3(0);

        public Vector3(float value)
        {
            X = value;Y = value;Z = value;
        }

        public Vector3(float x, float y, float z)
        {
            X = x; Y = y; Z = z;
        }
    }
}
