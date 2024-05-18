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
    public sealed class UserFacadeTests : CRUDFacadeTestsBase
    {

        private readonly UserFacade _userFacadeSUT;

        public UserFacadeTests(ITestOutputHelper output) : base(output)
        {
            _userFacadeSUT = new UserFacade(UnitOfWorkFactory, Mapper);
        }

        [Fact]
        public async Task GetById_UserEntity()
        {
            var driver = await _userFacadeSUT.GetAsync(UserSeeds.DriverEntity.Id);

            DeepAssert.Equal(Mapper.Map<UserModel>(UserSeeds.DriverEntity), driver);
        }
        [Fact]
        public async Task GetById_IsNull_NonExistentUser()
        {
            var nullUser = await _userFacadeSUT.GetAsync(UserSeeds.EmptyUserEntity.Id);

            Assert.Null(nullUser);
        }
        [Fact]
        public async Task DeleteById_CarEntityToDelete()
        {
            await _userFacadeSUT.DeleteAsync(UserSeeds.UserEntityDelete.Id);

            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            Assert.False(await dbxAssert.Cars.AnyAsync(i => i.Id == UserSeeds.UserEntityDelete.Id));
        }
        [Fact]
        public async Task InsertOrUpdate_NewUser_UserAdded()
        {
            //Arrange
            var user = new UserModel(
                FirstName: "Pepa",
                LastName: "Stopar")
            {
                Id = Guid.Parse("DC76F808-5CDD-4151-A3DB-6109DD7E43B5"),
            };

            //Act
            user = await _userFacadeSUT.SaveAsync(user);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var userFromDb = await dbxAssert.Users.SingleAsync(i => i.Id == user.Id);
            DeepAssert.Equal(user,  Mapper.Map<UserModel>(userFromDb));
        }

        [Fact]
        public async Task GetCount_ChangedAfterAdded()
        {
            // Arrange
            var newUser = new UserModel
            (
                FirstName:"Pepa",
                LastName: "Rovnochodec"
            );
            var oldCount = await _userFacadeSUT.GetCount();
            // Act
            await _userFacadeSUT.SaveAsync(newUser);
            // Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var newCount = dbxAssert.Users.Count();
            Assert.Equal(oldCount + 1, newCount);
        }
        [Fact]
        public async Task InsertOrUpdate_UserUpdated_UserEntityToUpdate()
        {
            //Arrange
            var oldUser = await _userFacadeSUT.GetAsync(UserSeeds.UserEntityUpdate.Id);
            var newLastName = "Kvapil";
            var updatedUser = oldUser! with
            {
                LastName = newLastName,
            };
            //Act
            await _userFacadeSUT.SaveAsync(updatedUser);

            //Assert
            await using var dbxAssert = await DbContextFactory.CreateDbContextAsync();
            var userFromDb = await dbxAssert.Users.SingleAsync(i => i.Id == oldUser.Id);
            Assert.Equal(userFromDb.LastName, newLastName);
        }
    }
}
