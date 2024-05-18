using System;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.Services.MessageDialog;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;
using AsyncRelayCommand = RideWithMe.App.Commands.AsyncRelayCommand;

namespace RideWithMe.App.ViewModels;

public class UserDetailViewModel: ViewModelBase, IUserDetailViewModel
{
    private readonly IMediator _mediator;
    private readonly UserFacade _userFacade;

    public UserDetailViewModel(
        UserFacade userFacade,
        IMediator mediator)
    {
        _userFacade = userFacade;
        _mediator = mediator;

        SaveCommand = new AsyncRelayCommand(SaveAsync, CanSave);
        DeleteCommand = new AsyncRelayCommand(DeleteAsync);
        CloseViewCommand = new RelayCommand(CloseDetailView);
    }

    public UserWrapper? Model { get; private set; }
    public ICommand SaveCommand { get; }
    public ICommand DeleteCommand { get; }
    public ICommand CloseViewCommand { get; }

    private void CloseDetailView() => _mediator.Send(new CloseUserDetailMessage<UserWrapper>());

    public async Task LoadAsync(Guid id)
    {
        Model = await _userFacade.GetAsync(id) ?? UserModel.Empty;
    }

    public async Task SaveAsync()
    {
        if (Model == null)
        {
            throw new InvalidOperationException("Cant save null");
        }

        Model = await _userFacade.SaveAsync(Model.Model);
        // Send message to UserList to update itself
        _mediator.Send(new UpdateMessage<UserWrapper> { Model = Model});
        // Send message to MainView to switch to UserListView
        _mediator.Send(new CloseUserDetailMessage<UserWrapper>());
        // Send message to RideWithMeView in case it was user edit and not new user
        _mediator.Send(new EditUserMessage<UserWrapper>());
    }

    private bool CanSave() => Model?.IsValid ?? false;

    public async Task DeleteAsync(){}
    public void LoadInDesignMode(){}
}