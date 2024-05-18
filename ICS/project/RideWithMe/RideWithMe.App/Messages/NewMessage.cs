using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record NewMessage<T> : Message<T>
        where T : IModel
    {
    }
}
