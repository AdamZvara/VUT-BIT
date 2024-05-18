using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;

namespace RideWithMe.DAL.Seeds;

public static class UserSeeds
{
    public static readonly UserEntity User1 = new(
        Id: Guid.Parse(input: "BE270D96-B993-4E81-9CAA-A28C0AB2ED0A"),
        FirstName: "Adam",
        LastName: "Zvara",
        ImageUrl: "https://ih1.redbubble.net/image.450287996.4220/flat,1000x1000,075,f.u1.jpg")
    {
    };
    public static readonly UserEntity User2 = new(
        Id: Guid.Parse(input: "751D384F-0C43-4AB1-90C8-AFCD5E44DC50"),
        FirstName: "Tomáš",
        LastName: "Matuš",
        ImageUrl: "https://st.depositphotos.com/1144472/2003/i/950/depositphotos_20030237-stock-photo-cheerful-young-man-over-white.jpg")
    {
    };
    public static readonly UserEntity User3 = new(
        Id: Guid.Parse(input: "10D5275C-9853-4ACC-9D0F-B2B10F639FF6"),
        FirstName: "Vojta",
        LastName: "Eichler",
        ImageUrl: "https://cdn.discordapp.com/attachments/816755874520891463/969168681357688872/unknown.png")
    {
    };

    public static void Seed(this ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<UserEntity>().HasData(
            User1 with { Cars = Array.Empty<CarEntity>(), RidePassengers = Array.Empty<RidePassengers>(), DriverRides = Array.Empty<RideEntity>() },
            User2 with { Cars = Array.Empty<CarEntity>(), RidePassengers = Array.Empty<RidePassengers>(), DriverRides = Array.Empty<RideEntity>() },
            User3 with { Cars = Array.Empty<CarEntity>(), RidePassengers = Array.Empty<RidePassengers>(), DriverRides = Array.Empty<RideEntity>() }
            );
    }
}