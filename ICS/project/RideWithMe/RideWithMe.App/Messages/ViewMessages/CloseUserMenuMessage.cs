using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record CloseUserMenuMessage<T> : Message<T>
        where T : IModel
    {
    }
}