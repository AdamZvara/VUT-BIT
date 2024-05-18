using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AutoMapper;
using RideWithMe.BL.Models.Address;
using RideWithMe.DAL.Entities;
using RideWithMe.DAL.UnitOfWork;

namespace RideWithMe.BL.Facades
{
    public class AddressFacade : CRUDFacade<AddressEntity, AddressListModel, AddressDetailModel>
    {
        public AddressFacade(IUnitOfWorkFactory unitOfWorkFactory, IMapper mapper) : base(unitOfWorkFactory, mapper)
        {
        }
    }
}
