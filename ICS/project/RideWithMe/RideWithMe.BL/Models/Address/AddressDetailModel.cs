using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AutoMapper;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Models.Address
{
    public record AddressDetailModel(
        string State,
        string Street,
        string City,
        int HouseNumber) : ModelBase
    {
        public string State { get; set; } = State;
        public string Street { get; set; } = Street;
        public string City { get; set; } = City;
        public int HouseNumber { get; set; } = HouseNumber;

        public class MapperProfile : Profile
        {
            public MapperProfile()
            {
                CreateMap<AddressEntity, AddressDetailModel>()
                    .ReverseMap();
            }
        }
        public static AddressDetailModel Empty => new(String.Empty, String.Empty, String.Empty, 0);


    }
}
