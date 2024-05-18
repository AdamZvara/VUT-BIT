using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;

namespace RideWithMe.App.ViewModels;

public class UserMenuViewModel : ViewModelBase, IUserMenuViewModel
{
    private readonly IMediator _mediator;
    private readonly UserFacade _userFacade;

    public UserMenuViewModel(
        IMediator mediator,
        UserFacade userFacade)
    {
        _mediator = mediator;
        _userFacade = userFacade;

        SignOutCommand = new RelayCommand(SignOutUser);
        EditUserCommand = new RelayCommand(EditUser);
        CreateCarCommand = new RelayCommand(CreateCar);
    }


    public UserWrapper? Model;

    public ICommand SignOutCommand { get; }
    public ICommand CreateCarCommand { get; }
    public ICommand EditUserCommand { get; }

    private void SignOutUser() => _mediator.Send(new SignOutMessage<UserWrapper>());

    private void EditUser()
    {
        _mediator.Send(new OpenUserDetailMessage<UserWrapper> {Id = Model.Id});
        _mediator.Send(new CloseUserMenuMessage<UserWrapper>());
    }

    public async Task LoadAsync(Guid Id)
    {
        Model = await _userFacade.GetAsync(Id);
    }

    private void CreateCar() => _mediator.Send(new OpenCarDetailMessage<CarWrapper>());
}