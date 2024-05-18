using AutoMapper;
using RideWithMe.BL.Models;
using RideWithMe.DAL;
using RideWithMe.DAL.UnitOfWork;
using RideWithMe.DAL.Entities;

namespace RideWithMe.BL.Facades;

public class UserFacade : CRUDFacade<UserEntity, UserModel, UserModel>
{
    public UserFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
    {
    }
}