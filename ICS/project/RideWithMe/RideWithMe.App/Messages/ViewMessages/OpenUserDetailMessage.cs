using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record OpenUserDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}

