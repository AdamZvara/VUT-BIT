using System.Dynamic;
using Microsoft.EntityFrameworkCore;
using RideWithMe.DAL.Entities;
using RideWithMe.DAL.Seeds;

namespace RideWithMe.DAL
{
	public class RideWithMeDbContext : DbContext
	{
        private readonly bool _seedDemoData;
		public RideWithMeDbContext(DbContextOptions contextOptions, bool seedDemoData = false)
            : base(contextOptions)
        {
            _seedDemoData = seedDemoData;

        }
		public DbSet<AddressEntity> Addresses => Set<AddressEntity>();
		public DbSet<CarEntity> Cars => Set<CarEntity>();
		public DbSet<UserEntity> Users => Set<UserEntity>();
		public DbSet<RideEntity> Rides => Set<RideEntity>();
        public DbSet<RidePassengers> RidePassengers => Set<RidePassengers>();

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<UserEntity>(entity =>
            {
                entity.HasMany(i => i.Cars)
                    .WithOne(i => i.Owner)
                    .OnDelete(DeleteBehavior.Cascade);
                entity.HasMany(i => i.DriverRides)
                    .WithOne(i => i.Driver)
                    .OnDelete(DeleteBehavior.Cascade);
            });

            modelBuilder.Entity<RideEntity>(entity =>
            {
                entity.HasOne(i => i.StartLocation)
                    .WithMany(i => i.StartOfRides)
                    .OnDelete(DeleteBehavior.Restrict);
				entity.HasOne(i => i.EndLocation)
					.WithMany(i => i.EndOfRides)
					.OnDelete(DeleteBehavior.Restrict);
				entity.HasOne(i => i.Car)
					.WithMany(i => i.Rides)
					.OnDelete(DeleteBehavior.Restrict);
            });

            modelBuilder.Entity<RidePassengers>(entity =>
            {
                entity.HasKey(sc => new { sc.PassengerId, sc.RideId });

                entity.HasOne(i => i.Passenger)
                    .WithMany(i => i.RidePassengers)
                    .OnDelete(DeleteBehavior.NoAction);

                entity.HasOne(i => i.Ride)
                    .WithMany(i => i.RidePassengers)
                    .OnDelete(DeleteBehavior.NoAction);
            });

            if (_seedDemoData)
            {
                UserSeeds.Seed(modelBuilder);
                CarSeeds.Seed(modelBuilder);
                AddressSeeds.Seed(modelBuilder);
                RideSeeds.Seed(modelBuilder);
                RidePassengerSeeds.Seed(modelBuilder);
            }

        }
	}

}