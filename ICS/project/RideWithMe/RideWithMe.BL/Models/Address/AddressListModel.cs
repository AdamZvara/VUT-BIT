using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AutoMapper;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models.Address
{
    public record AddressListModel(
        Guid Id,
        string Street,
        string City) : ModelBase
    {
        public Guid Id { get; set; } = Id;
        public string Street { get; set; } = Street;
        public string City { get; set; } = City;

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<AddressEntity, AddressListModel>();
            }
        }
    }
}
