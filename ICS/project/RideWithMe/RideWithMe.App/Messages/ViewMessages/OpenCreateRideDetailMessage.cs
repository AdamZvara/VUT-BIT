﻿using RideWithMe.BL.Models;

namespace RideWithMe.App.Messages.ViewMessages
{
    public record OpenCreateRideDetailMessage<T> : Message<T>
        where T : IModel
    {
    }
}