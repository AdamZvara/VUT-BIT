using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record CloseRideDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}

