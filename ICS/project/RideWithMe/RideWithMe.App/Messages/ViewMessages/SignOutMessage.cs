using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record SignOutMessage<T> : Message<T>
        where T : IModel
    {
    }
}
