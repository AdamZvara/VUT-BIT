using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record CloseCreateRideDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}
