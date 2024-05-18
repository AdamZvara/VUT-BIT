
using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record CloseAddressDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}