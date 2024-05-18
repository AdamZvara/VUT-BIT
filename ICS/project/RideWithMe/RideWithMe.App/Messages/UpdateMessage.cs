using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record UpdateMessage<T> : Message<T>
        where T : IModel
    {
    }
}
