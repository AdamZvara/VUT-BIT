using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages;

public record OpenCarDetailMessage<T> : Message<T>
    where T : IModel

{
}