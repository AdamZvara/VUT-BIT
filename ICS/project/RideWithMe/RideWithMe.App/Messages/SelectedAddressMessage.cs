using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RideWithMe.BL.Models;
using System.Threading.Tasks;
using RideWithMe.App.Enums;

namespace RideWithMe.App.Messages
{
    public record SelectedAddressMessage<T> : SelectedMessage<T>
        where T : IModel
    {
        public LocationTypes LocationType { get; set; } = LocationTypes.None;

    }
}
