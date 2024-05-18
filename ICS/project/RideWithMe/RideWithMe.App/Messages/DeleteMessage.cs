using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record DeleteMessage<T> : Message<T>
        where T : IModel
    {
    }
}