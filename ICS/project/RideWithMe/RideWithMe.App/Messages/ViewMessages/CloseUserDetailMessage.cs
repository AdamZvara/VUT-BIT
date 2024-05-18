using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record CloseUserDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}

