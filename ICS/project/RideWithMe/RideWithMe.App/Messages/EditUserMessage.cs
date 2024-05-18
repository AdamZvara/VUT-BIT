using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages
{
    public record EditUserMessage<T> : Message<T>
        where T : IModel
    {
    }
}
