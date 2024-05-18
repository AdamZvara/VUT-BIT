using System;
using System.Threading.Tasks;

namespace RideWithMe.App.ViewModels.Interfaces;

public interface IUserMenuViewModel
{
    public Task LoadAsync(Guid Id);
}