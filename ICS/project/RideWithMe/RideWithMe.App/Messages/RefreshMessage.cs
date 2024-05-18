using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record RefreshMessage<T> : Message<T>
        where T : IModel
    {
    }
}
