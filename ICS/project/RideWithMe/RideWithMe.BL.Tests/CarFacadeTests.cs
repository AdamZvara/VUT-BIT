using RideWithMe.BL.Tests;
using Microsoft.EntityFrameworkCore;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;
using RideWithMe.Common.Enums;
using RideWithMe.Common.Tests;
using RideWithMe.Common.Tests.Seeds;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xunit;
using Xunit.Abstractions;

namespace RideWithMe.BL.Tests
{
    public sealed class CarFacadeTests : CRUDFacadeTestsBase
    {
        private readonly CarFacade _carFacadeSUT;

        public CarFacadeTests(ITestOutputHelper output) : base(output)
        {
            _carFacadeSUT = new CarFacade(UnitOfWorkFactory, Mapper);
        }

        [Fact]
        public async Task Create_NewCar_DoesNotThrow()
        {
            var model = new CarDetailModel
            (
                OwnerId: UserSeeds.DriverEntity.Id,
                RegistrationDate: DateTime.Now.AddYears(-1),
                CarTypeId: CarTypeId.Cabriolet,
                BrandId: CarBrandId.Bugatti,
                Seats: 4
            );

            var _ = await _carFacadeSUT.SaveAsync(model);
        }


        [Fact]
        public async Task GetAll_GetSingle_CarEntity()
        {
            var allCars = await _carFacadeSUT.GetAllAsync();
            var car = allCars.Single(i => i.Id == CarSeeds.CarEntity.Id);

            DeepAssert.Equal(Mapper.Map<CarListModel>(CarSeeds.CarEntity), car);
        }    
        [Fact]
        public async Task GetById_CarEntity()
        {
            var car = await _carFacadeSUT.GetAsync(CarSeeds.CarEntity.Id);

            DeepAssert.Equal(Mapper.Map<CarDetailModel>(CarSeeds.CarEntity), car);
        }

        [Fact]
        public async Task GetById_IsNull_NonExistentCar()
        {
            var nullCar = await _carFacadeSUT.GetAsync(CarSeeds.EmptyCarEntity.Id);

            Assert.Null(nullCar);
        }
        [Fact]
        public async Task GetById_Owner_NotNull()
        {
            var car = await _carFacadeSUT.GetAsync(CarSeeds.CarEntity.Id);

            Assert.NotNull(car!.Owner);
        }

        [Fact]
        public async Task GetById_IsNotNull_ExistentCar()
        {
            var nullCar = await _carFacadeSUT.GetAsync(CarSeeds.CarEntity.Id);

            Assert.NotNull(nullCar);
        }

        [Fact]
        public async Task DeleteById_CarEntityToDelete()
        {
            await _carFacadeSUT.DeleteAsync(CarSeeds.CarEntityToDelete.Id);

            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            Assert.False(await dbxAssert.Cars.AnyAsync(i => i.Id == CarSeeds.CarEntityToDelete.Id));
        }

        [Fact]
        public async Task InsertOrUpdate_NewCar_CarAdded()
        {
            //Arrange
            var car = new CarDetailModel
               (
                   OwnerId: UserSeeds.DriverEntity.Id,
                   RegistrationDate: DateTime.Now.AddYears(-6),
                   CarTypeId: CarTypeId.Cabriolet,
                   BrandId: CarBrandId.AlfaRomeo,
                   Seats: 3
               )
            {
                Id = Guid.Parse("2F5C9BEC-A2B6-4A2F-A38B-8AD410B61B27"),
            };

            //Act
            car = await _carFacadeSUT.SaveAsync(car);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var carFromDb = await dbxAssert.Cars.SingleAsync(i => i.Id == car.Id);
        }

        [Fact]
        public async Task InsertOrUpdate_CarUpdated_CarEntityToUpdate()
        {
            //Arrange
            var carUpdate = new CarDetailModel
            (
                OwnerId: UserSeeds.DriverEntity.Id,
                RegistrationDate: DateTime.Now.AddYears(-6),
                CarTypeId: CarTypeId.Cabriolet,
                BrandId: CarBrandId.AlfaRomeo,
                Seats: 4
            )
            {
                Id = CarSeeds.CarEntityToUpdate.Id,
            };

            carUpdate.CarTypeId = CarTypeId.Sedan;
            carUpdate.BrandId = CarBrandId.Acura;

            //Act
            await _carFacadeSUT.SaveAsync(carUpdate);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var updatedCarFromDb = await dbxAssert.Cars.SingleAsync(i => i.Id == carUpdate.Id);
            DeepAssert.Equal(carUpdate, Mapper.Map<CarDetailModel>(updatedCarFromDb));
        }

        [Fact]
        public async Task GetCount_ChangedAfterAdded()
        {
            // Arrange
            var newCar = new CarDetailModel
            (
                OwnerId: UserSeeds.DriverEntity.Id,
                RegistrationDate: DateTime.Now.AddYears(-6),
                CarTypeId: CarTypeId.Roadster,
                BrandId: CarBrandId.AstonMartin,
                Seats: 2
            );
            int oldCount = await _carFacadeSUT.GetCount();
            // Act
            await _carFacadeSUT.SaveAsync(newCar);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            int newCount = dbxAssert.Cars.Count();
            Assert.Equal(oldCount + 1, newCount);
        }

        [Fact]
        public async Task ChangeCarOwner_CarWithNoOwner()
        {
            // Arrange
            var carWithNoOwner = await _carFacadeSUT.GetAsync(CarSeeds.CarWithNoOwner.Id);
            Assert.NotNull(carWithNoOwner);
            // Act
            await _carFacadeSUT.ChangeCarOwner(carWithNoOwner!, UserSeeds.DriverEntity.Id);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var updatedCarFromDb = await dbxAssert.Cars.SingleAsync(i => i.Id == carWithNoOwner!.Id);
            Assert.True(updatedCarFromDb.OwnerId == UserSeeds.DriverEntity.Id);
        }
        [Fact]
        public async Task ChangeCarOwner_CarWithLegitOwner()
        {
            // Arrange
            var carWithLegitOwner = await _carFacadeSUT.GetAsync(CarSeeds.CarWithLegitOwner.Id);
            Assert.NotNull(carWithLegitOwner);
            var oldOwnerId = carWithLegitOwner!.OwnerId;
            // Act
            await _carFacadeSUT.ChangeCarOwner(carWithLegitOwner!, UserSeeds.DriverEntity.Id);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var updatedCarFromDb = await dbxAssert.Cars.SingleAsync(i => i.Id == carWithLegitOwner!.Id);
            Assert.False(oldOwnerId == updatedCarFromDb.OwnerId);
            Assert.True(updatedCarFromDb.OwnerId == UserSeeds.DriverEntity.Id);
        }

    }
}
