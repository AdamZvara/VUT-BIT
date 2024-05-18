using Microsoft.EntityFrameworkCore;
using RideWithMe.Common.Enums;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.Seeds;

public static class CarSeeds
{
    public static readonly CarEntity Car1 = new(
        Id: Guid.Parse(input: "B730A8FF-ED82-47C5-8302-8B992A3F85D7"),
        RegistrationDate: DateTime.ParseExact("2019-05-23", "yyyy-MM-dd",
            System.Globalization.CultureInfo.InvariantCulture), 
        ImageUrl: "https://previews.123rf.com/images/m600maxx/m600maxx1109/m600maxx110900074/10685034-sport-auto.jpg",
        OwnerId: Guid.Parse("BE270D96-B993-4E81-9CAA-A28C0AB2ED0A"), 
        CarTypeId: CarTypeId.Sport,
        BrandId: CarBrandId.Bentley, Seats: 4)
    {
    };

    public static readonly CarEntity Car2 = new(
        Id: Guid.Parse(input: "31E613FC-8BF1-46A9-B4F4-5DD47B26DC6A"),
        RegistrationDate: DateTime.ParseExact("2016-01-15", "yyyy-MM-dd",
            System.Globalization.CultureInfo.InvariantCulture),
        ImageUrl: null,
        OwnerId: Guid.Parse("BE270D96-B993-4E81-9CAA-A28C0AB2ED0A"),
        CarTypeId: CarTypeId.SUV,
        BrandId: CarBrandId.BMW, Seats: 3)
    {
    };

    public static readonly CarEntity Car3 = new(
        Id: Guid.Parse(input: "869C2C8B-F68F-4474-870F-30AD45CB6795"),
        RegistrationDate: DateTime.ParseExact("2020-06-07", "yyyy-MM-dd",
            System.Globalization.CultureInfo.InvariantCulture),
        ImageUrl: null,
        OwnerId: Guid.Parse("751D384F-0C43-4AB1-90C8-AFCD5E44DC50"),
        CarTypeId: CarTypeId.Cabriolet,
        BrandId: CarBrandId.Skoda, Seats: 2)
    {
    };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<CarEntity>().HasData(
            Car1 with { Owner = null, Rides = Array.Empty<RideEntity>() },
            Car2 with { Owner = null, Rides = Array.Empty<RideEntity>() },
            Car3 with { Owner = null, Rides = Array.Empty<RideEntity>() }
        );
    }
}