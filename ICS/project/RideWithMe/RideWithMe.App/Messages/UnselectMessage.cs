using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record UnselectMessage<T> : Message<T>
        where T : IModel
    {
    }
}
