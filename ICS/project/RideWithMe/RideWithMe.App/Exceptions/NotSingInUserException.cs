using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RideWithMe.App.Exceptions
{
    internal class NotSingInUserException : Exception
    {
        public NotSingInUserException()
        {
        }

        public NotSingInUserException(string message)
            : base(message)
        {
        }
    }
}
