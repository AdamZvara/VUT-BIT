using RideWithMe.App.Enums;

namespace RideWithMe.App.ViewModels.Interfaces;

public interface IAddressListViewModel : IListViewModel
{
    public void SetType(LocationTypes type);

}