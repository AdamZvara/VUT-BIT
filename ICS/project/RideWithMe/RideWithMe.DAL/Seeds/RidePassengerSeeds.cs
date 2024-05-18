using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.Seeds;

public static class RidePassengerSeeds
{
    public static readonly RidePassengers Passenger1 = new(
        Id: Guid.Parse(input: "5643C304-77A3-461F-943E-A065213C4D1F"),
        PassengerId: Guid.Parse("10D5275C-9853-4ACC-9D0F-B2B10F639FF6"), 
        RideId: Guid.Parse("67F64610-C380-45BE-954E-5F70F1F1D129"))
    {
    };
    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<RidePassengers>().HasData(
            Passenger1
        );
    }
}