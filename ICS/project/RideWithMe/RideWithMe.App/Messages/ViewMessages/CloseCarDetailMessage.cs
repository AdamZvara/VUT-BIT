using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages;

public record CloseCarDetailMessage<T> : Message<T>
    where T : IModel
{
}