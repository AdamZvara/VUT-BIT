using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.Seeds;

public static class RideSeeds
{
    public static readonly RideEntity Ride1 = new(
        Id: Guid.Parse(input: "67F64610-C380-45BE-954E-5F70F1F1D129"),
        StartTime: DateTime.ParseExact("2022-05-08 14:40", "yyyy-MM-dd HH:mm",
            System.Globalization.CultureInfo.InvariantCulture),
        EndTime: DateTime.ParseExact("2022-05-08 15:10", "yyyy-MM-dd HH:mm",
            System.Globalization.CultureInfo.InvariantCulture),
        CarId: Guid.Parse("B730A8FF-ED82-47C5-8302-8B992A3F85D7"),
        DriverId: Guid.Parse("BE270D96-B993-4E81-9CAA-A28C0AB2ED0A"),
        StartLocationId: Guid.Parse("D37DFFE5-4FD5-4457-BFCC-1D3F0073CC98"),
        EndLocationId: Guid.Parse("59BF8999-2086-4B19-BB79-D616B68FA2B5"))
    {
    };

    public static readonly RideEntity Ride2 = new(
        Id: Guid.Parse(input: "0D6CB79B-30C1-42F6-9F48-60FB55527585"),
        StartTime: DateTime.ParseExact("2022-05-03 09:40", "yyyy-MM-dd HH:mm",
            System.Globalization.CultureInfo.InvariantCulture),
        EndTime: DateTime.ParseExact("2022-05-03 10:00", "yyyy-MM-dd HH:mm",
            System.Globalization.CultureInfo.InvariantCulture),
        CarId: Guid.Parse("869C2C8B-F68F-4474-870F-30AD45CB6795"),
        DriverId: Guid.Parse("751D384F-0C43-4AB1-90C8-AFCD5E44DC50"),
        StartLocationId: Guid.Parse("971293E5-CE4F-4097-9ADA-CF25923D442C"),
        EndLocationId: Guid.Parse("D37DFFE5-4FD5-4457-BFCC-1D3F0073CC98"))
    {
    };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<RideEntity>().HasData(
            Ride1 with { Car = null, Driver = null, EndLocation = null, StartLocation = null, RidePassengers = Array.Empty<RidePassengers>() },
            Ride2 with { Car = null, Driver = null, EndLocation = null, StartLocation = null, RidePassengers = Array.Empty<RidePassengers>() }
        );
    }
}